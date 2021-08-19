/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Date           Author       Notes
 * 2018-12-18     ChenYong     first implementation
 * 2021-08-19     WaterFishJ   fix the assert bug
 */


#include <stddef.h>
#include <rtthread.h>
#include <rthw.h>
#include "syscfg/syscfg.h"
#include "nimble/nimble_port.h"

#if NIMBLE_CFG_CONTROLLER
static rt_thread_t ll_task_h;
#endif

extern void ble_ll_task(void *arg);

#ifdef RT_DEBUG
RT_WEAK void __aeabi_assert(const char *expr, const char *file, int line)
{
    rt_assert_handler(expr, file, line);
}
#endif

int nimble_port_rtthread_init(void)
{
    nimble_port_init();

#if NIMBLE_CFG_CONTROLLER
    /*
     * Create task where NimBLE LL will run. This one is required as LL has its
     * own event queue and should have highest priority. The task function is
     * provided by NimBLE and in case of FreeRTOS it does not need to be wrapped
     * since it has compatible prototype.
     */
    ll_task_h = rt_thread_create("ll", ble_ll_task, NULL, 
                                MYNEWT_VAL(BLE_CTLR_THREAD_STACK_SIZE),
                                MYNEWT_VAL(BLE_CTLR_THREAD_PRIORITY), 10);
    if (ll_task_h != RT_NULL)
        rt_thread_startup(ll_task_h);

#endif
    
    return 0;
}

void ble_hs_thread_entry(void *parameter)
{
    nimble_port_run();
}

void ble_hs_thread_startup(void)
{
    rt_thread_t tid;

    tid = rt_thread_create("host", ble_hs_thread_entry, NULL,
                           MYNEWT_VAL(BLE_HOST_THREAD_STACK_SIZE),
                           MYNEWT_VAL(BLE_HOST_THREAD_PRIORITY), 10);
    if (tid != RT_NULL)
        rt_thread_startup(tid);
}

INIT_COMPONENT_EXPORT(nimble_port_rtthread_init);
