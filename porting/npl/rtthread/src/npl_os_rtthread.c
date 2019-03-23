/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Date           Author       Notes
 * 2018-12-18     ZeroFree     first implementation
 */

#include <assert.h>
#include <stddef.h>
#include <string.h>
#include "nimble/nimble_npl.h"

#include <rtthread.h>
#include <rthw.h>

static void npl_rtthread_callout_init(struct ble_npl_callout *co, struct ble_npl_eventq *evq, ble_npl_event_fn *ev_cb, void *ev_arg);
static ble_npl_error_t npl_rtthread_callout_reset(struct ble_npl_callout *co, ble_npl_time_t ticks);
static ble_npl_time_t npl_rtthread_callout_remaining_ticks(struct ble_npl_callout *co, ble_npl_time_t now);
static ble_npl_error_t npl_rtthread_time_ms_to_ticks(uint32_t ms, ble_npl_time_t *out_ticks);
static ble_npl_error_t npl_rtthread_time_ticks_to_ms(ble_npl_time_t ticks, uint32_t *out_ms);

void *ble_npl_get_current_task_id(void)
{
    return rt_thread_self();
}

int ble_npl_task_init(struct ble_npl_task *t, const char *name, 
        ble_npl_task_fn *func,void *arg, uint8_t prio, 
        uint32_t sanity_itvl, uint32_t *stack_bottom, uint16_t stack_size)
{
    rt_thread_t tid;

    tid = rt_thread_create(name, func, arg, stack_size, prio, 10);
    if(tid)
    {
        t->t = tid;
        rt_thread_startup(tid);
        return RT_EOK;
    }
    return -RT_ERROR;
}

void ble_npl_eventq_init(struct ble_npl_eventq *evq)
{
    evq->q = rt_mq_create("npl_evq", sizeof(struct ble_npl_eventq *), 32, RT_IPC_FLAG_FIFO);
}

struct ble_npl_event *ble_npl_eventq_get(struct ble_npl_eventq *evq, ble_npl_time_t tmo)
{
    struct ble_npl_event *ev = NULL;

    rt_mq_recv((struct rt_messagequeue *)evq->q, &ev, sizeof(struct ble_npl_eventq *), tmo);

    if (ev)
    {
        ev->queued = false;
    }

    return ev;
}

void ble_npl_eventq_put(struct ble_npl_eventq *evq, struct ble_npl_event *ev)
{
    int ret;

    if (ev->queued)
    {
        return;
    }

    ev->queued = true;
    ret = rt_mq_send((struct rt_messagequeue *)evq->q, &ev, sizeof(struct ble_npl_eventq *));

    RT_ASSERT(ret == RT_EOK);
}

void ble_npl_eventq_remove(struct ble_npl_eventq *evq, struct ble_npl_event *ev)
{
    if (!ev->queued)
    {
        return;
    }

    rt_mq_control((struct rt_messagequeue *)evq->q, RT_IPC_CMD_RESET, RT_NULL);
    ev->queued = false;
}

void ble_npl_event_run(struct ble_npl_event *ev)
{
    ev->fn(ev);
}

bool ble_npl_eventq_is_empty(struct ble_npl_eventq *evq)
{
    int count;
    rt_base_t level;

    level = rt_hw_interrupt_disable();
    count = ((struct rt_messagequeue *)evq->q)->entry;
    rt_hw_interrupt_enable(level);

    return count ? true : false;
}

void ble_npl_event_init(struct ble_npl_event *ev, ble_npl_event_fn *fn,
                        void *arg)
{
    memset(ev, 0, sizeof(*ev));
    ev->fn = fn;
    ev->arg = arg;
}

bool ble_npl_event_is_queued(struct ble_npl_event *ev)
{
    return ev->queued;
}

void *ble_npl_event_get_arg(struct ble_npl_event *ev)
{
    return ev->arg;
}

void ble_npl_event_set_arg(struct ble_npl_event *ev, void *arg)
{
    ev->arg = arg;
}

ble_npl_error_t ble_npl_mutex_init(struct ble_npl_mutex *mu)
{
    if (!mu)
    {
        return BLE_NPL_INVALID_PARAM;
    }

    mu->handle = rt_mutex_create("mutx", RT_IPC_FLAG_FIFO);
    RT_ASSERT(mu->handle);

    return BLE_NPL_OK;
}

ble_npl_error_t ble_npl_mutex_pend(struct ble_npl_mutex *mu, ble_npl_time_t timeout)
{
    int ret;

    if (!mu)
    {
        return BLE_NPL_INVALID_PARAM;
    }

    RT_ASSERT(mu->handle);

    ret = rt_mutex_take((struct rt_mutex *)mu->handle, timeout);

    return ret == RT_EOK ? BLE_NPL_OK : BLE_NPL_TIMEOUT;
}

ble_npl_error_t ble_npl_mutex_release(struct ble_npl_mutex *mu)
{
    int ret;

    if (!mu)
    {
        return BLE_NPL_INVALID_PARAM;
    }

    RT_ASSERT(mu->handle);

    ret = rt_mutex_release((struct rt_mutex *)mu->handle);

    return ret == RT_EOK ? BLE_NPL_OK : BLE_NPL_ERROR;
}

ble_npl_error_t ble_npl_sem_init(struct ble_npl_sem *sem, uint16_t tokens)
{
    if (!sem)
    {
        return BLE_NPL_INVALID_PARAM;
    }

    sem->handle = rt_sem_create("npl_sem", tokens, RT_IPC_FLAG_FIFO);
    RT_ASSERT(sem->handle);

    return BLE_NPL_OK;
}

ble_npl_error_t ble_npl_sem_pend(struct ble_npl_sem *sem, ble_npl_time_t timeout)
{
    int ret;

    if (!sem)
    {
        return BLE_NPL_INVALID_PARAM;
    }

    RT_ASSERT(sem->handle);

    ret = rt_sem_take((struct rt_semaphore *)sem->handle, timeout);

    return ret == RT_EOK ? BLE_NPL_OK : BLE_NPL_TIMEOUT;
}

ble_npl_error_t ble_npl_sem_release(struct ble_npl_sem *sem)
{
    int ret;

    if (!sem)
    {
        return BLE_NPL_INVALID_PARAM;
    }

    RT_ASSERT(sem->handle);

    ret = rt_sem_release((struct rt_semaphore *) sem->handle);

    return ret == RT_EOK ? BLE_NPL_OK : BLE_NPL_ERROR;
}

uint16_t ble_npl_sem_get_count(struct ble_npl_sem *sem)
{
    int count;
    rt_base_t level;

    RT_ASSERT(sem->handle);

    level = rt_hw_interrupt_disable();
    count = ((struct rt_semaphore *)sem->handle)->value;
    rt_hw_interrupt_enable(level);

    return count;
}

void ble_npl_callout_init(struct ble_npl_callout *co, struct ble_npl_eventq *evq,
                          ble_npl_event_fn *ev_cb, void *ev_arg)
{
    npl_rtthread_callout_init(co, evq, ev_cb, ev_arg);
}

ble_npl_error_t ble_npl_callout_reset(struct ble_npl_callout *co, ble_npl_time_t ticks)
{
    return npl_rtthread_callout_reset(co, ticks);
}

void ble_npl_callout_stop(struct ble_npl_callout *co)
{
    if (co->handle)
        rt_timer_stop((struct rt_timer *)co->handle);
}

bool ble_npl_callout_is_active(struct ble_npl_callout *co)
{
    return (((struct rt_timer *)co->handle)->parent.flag & RT_TIMER_FLAG_ACTIVATED) ? true : false;
}

ble_npl_time_t ble_npl_callout_get_ticks(struct ble_npl_callout *co)
{
    return ((struct rt_timer *)co->handle)->timeout_tick;
}

ble_npl_time_t ble_npl_callout_remaining_ticks(struct ble_npl_callout *co,
        ble_npl_time_t time)
{
    return npl_rtthread_callout_remaining_ticks(co, time);
}

void ble_npl_callout_set_arg(struct ble_npl_callout *co, void *arg)
{
    co->ev.arg = arg;
}

ble_npl_time_t ble_npl_time_get(void)
{
    return rt_tick_get();
}

ble_npl_error_t ble_npl_time_ms_to_ticks(uint32_t ms, ble_npl_time_t *out_ticks)
{
    return npl_rtthread_time_ms_to_ticks(ms, out_ticks);
}

ble_npl_error_t ble_npl_time_ticks_to_ms(ble_npl_time_t ticks, uint32_t *out_ms)
{
    return npl_rtthread_time_ticks_to_ms(ticks, out_ms);
}

ble_npl_time_t ble_npl_time_ms_to_ticks32(uint32_t ms)
{
    return ms * RT_TICK_PER_SECOND / 1000;
}

uint32_t ble_npl_time_ticks_to_ms32(ble_npl_time_t ticks)
{
    return ticks * 1000 / RT_TICK_PER_SECOND;
}

void ble_npl_time_delay(ble_npl_time_t ticks)
{
    rt_thread_delay(ticks);
}

uint32_t ble_npl_hw_enter_critical(void)
{
    return rt_hw_interrupt_disable();
}

void ble_npl_hw_exit_critical(uint32_t ctx)
{
    rt_hw_interrupt_enable(ctx);
}

static void os_callout_timer_cb(void *parameter)
{
    struct ble_npl_callout *co;

    co = (struct ble_npl_callout *)parameter;
    RT_ASSERT(co);

    if (co->evq)
    {
        ble_npl_eventq_put(co->evq, &co->ev);
    }
    else
    {
        co->ev.fn(&co->ev);
    }
}

static void npl_rtthread_callout_init(struct ble_npl_callout *co, struct ble_npl_eventq *evq,
                                      ble_npl_event_fn *ev_cb, void *ev_arg)
{
    memset(co, 0, sizeof(*co));
    co->handle = rt_timer_create("co", os_callout_timer_cb, co, 0, RT_TIMER_FLAG_ONE_SHOT | RT_TIMER_FLAG_SOFT_TIMER);
    co->evq = evq;
    ble_npl_event_init(&co->ev, ev_cb, ev_arg);
}

static ble_npl_error_t npl_rtthread_callout_reset(struct ble_npl_callout *co, ble_npl_time_t ticks)
{
    if (ticks == 0)
    {
        ticks = 1;
    }

    rt_timer_stop((struct rt_timer *)co->handle);
    rt_timer_control((struct rt_timer *)co->handle, RT_TIMER_CTRL_SET_TIME, &ticks);
    rt_timer_start((struct rt_timer *)co->handle);

    return BLE_NPL_OK;
}

static ble_npl_time_t npl_rtthread_callout_remaining_ticks(struct ble_npl_callout *co,
        ble_npl_time_t now)
{
    ble_npl_time_t rt;
    uint32_t exp;
    rt_base_t level;

    level = rt_hw_interrupt_disable();
    exp = ((struct rt_timer *)co->handle)->timeout_tick;
    rt_hw_interrupt_enable(level);

    if (exp > now)
    {
        rt = exp - now;
    }
    else
    {
        rt = 0;
    }

    return rt;
}

static ble_npl_error_t npl_rtthread_time_ms_to_ticks(uint32_t ms, ble_npl_time_t *out_ticks)
{
    uint64_t ticks;

    ticks = rt_tick_from_millisecond(ms);
    if (ticks > UINT32_MAX)
    {
        return BLE_NPL_EINVAL;
    }

    *out_ticks = ticks;

    return BLE_NPL_OK;
}

static ble_npl_error_t npl_rtthread_time_ticks_to_ms(ble_npl_time_t ticks, uint32_t *out_ms)
{
    uint64_t ms;

    ms = ((uint64_t)ticks * 1000) / RT_TICK_PER_SECOND;
    if (ms > UINT32_MAX)
    {
        return BLE_NPL_EINVAL;
    }

    *out_ms = ms;

    return BLE_NPL_OK;
}
