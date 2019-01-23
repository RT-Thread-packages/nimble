/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Date           Author       Notes
 * 2018-12-18     ChenYong     first implementation
 */


#include <stddef.h>
#include <rtthread.h>
#include <rthw.h>
#include "syscfg/syscfg.h"
#include "nimble/nimble_port.h"

#define NIMBLE_THRAED_STACK_SIZE (1024 * 2)
#define NIMBLE_THREAD_PRIORITY (10)

#if NIMBLE_CFG_CONTROLLER
static rt_thread_t ll_task_h;
#endif
static rt_thread_t host_task_h;

extern void ble_ll_task(void *arg);
extern void nimble_port_run(void);

RT_WEAK void __aeabi_assert(const char *expr, const char *file, int line)
{
    rt_assert_handler(expr, file, line);
}

int nimble_port_rtthread_init(void)
{
    // TODO:
    // ble_hci_sock_init();
    nimble_port_init();

#if NIMBLE_CFG_CONTROLLER
    /*
     * Create task where NimBLE LL will run. This one is required as LL has its
     * own event queue and should have highest priority. The task function is
     * provided by NimBLE and in case of FreeRTOS it does not need to be wrapped
     * since it has compatible prototype.
     */
    ll_task_h = rt_thread_create("ll", ble_ll_task, NULL, NIMBLE_THRAED_STACK_SIZE, NIMBLE_THREAD_PRIORITY - 1, 10);
    if (ll_task_h != RT_NULL)
        rt_thread_startup(ll_task_h);

#endif


    /* XXX Need to have template for store */
    // ble_store_ram_init();

    /*
     * Create task where NimBLE host will run. It is not strictly necessary to
     * have separate task for NimBLE host, but since something needs to handle
     * default queue it is just easier to make separate task which does this.
     */
    // host_task_h = rt_thread_create("ble", nimble_port_run, NULL, NIMBLE_THRAED_STACK_SIZE, NIMBLE_THREAD_PRIORITY + 1, 10);
    // if(host_task_h != RT_NULL)
    //     rt_thread_startup(host_task_h);

    // return 0;
}

int nimble_ble_enable(void)
{
    rt_thread_t tid;

    tid = rt_thread_create("ble", nimble_port_run, NULL, NIMBLE_THRAED_STACK_SIZE, NIMBLE_THREAD_PRIORITY + 1, 10);
    if (tid != RT_NULL)
        rt_thread_startup(tid);

}

INIT_COMPONENT_EXPORT(nimble_port_rtthread_init);
