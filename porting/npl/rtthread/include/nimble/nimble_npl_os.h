/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Date           Author       Notes
 * 2018-12-18     ZeroFree     first implementation
 */

#ifndef _NIMBLE_NPL_OS_H_
#define _NIMBLE_NPL_OS_H_

#include <assert.h>
#include <stdint.h>
#include <string.h>

// TODO: _Static_assert support
#ifndef _Static_assert
#define _Static_assert(...)
#endif

#define BLE_NPL_OS_ALIGNMENT    4

#define BLE_NPL_TIME_FOREVER    -1

typedef int32_t ble_npl_time_t;
typedef int32_t ble_npl_stime_t;

struct ble_npl_event
{
    bool queued;
    ble_npl_event_fn *fn;
    void *arg;
};

struct ble_npl_eventq
{
    void *q;
};

struct ble_npl_callout
{
    void *handle;
    struct ble_npl_eventq *evq;
    struct ble_npl_event ev;
};

struct ble_npl_mutex
{
    void *handle;
};

struct ble_npl_sem
{
    void  *handle;
};

static inline bool ble_npl_os_started(void)
{
    return 1;
}

#endif  /* _NPL_H_ */
