/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Date           Author       Notes
 * 2022-05-20     Jackistang   first implementation
 */

#ifndef H_MYNEWT_LOGCFG_
#define H_MYNEWT_LOGCFG_

#include "log_common/ignore.h"
#include <rtconfig.h>

#define DBG_TAG "nimble"

#ifdef NIMBLE_DEBUG_LEVEL_D
#define DBG_LVL DBG_LOG
#endif
#ifdef NIMBLE_DEBUG_LEVEL_I
#define DBG_LVL DBG_INFO
#endif
#ifdef NIMBLE_DEBUG_LEVEL_W
#define DBG_LVL DBG_WARNING
#endif
#ifdef NIMBLE_DEBUG_LEVEL_E
#define DBG_LVL DBG_ERROR
#endif

#include "rtdbg.h"

#define BLE_HS_LOG_DEBUG(...)       LOG_D(__VA_ARGS__)
#define BLE_HS_LOG_INFO(...)        LOG_I(__VA_ARGS__)
#define BLE_HS_LOG_WARN(...)        LOG_W(__VA_ARGS__)
#define BLE_HS_LOG_ERROR(...)       LOG_E(__VA_ARGS__)
#define BLE_HS_LOG_CRITICAL(...)    LOG_E(__VA_ARGS__)
#define BLE_HS_LOG_DISABLED(...)    IGNORE(__VA_ARGS__)

#define MODLOG_DFLT_DEBUG(...)     LOG_D(__VA_ARGS__)
#define MODLOG_DFLT_INFO(...)      LOG_I(__VA_ARGS__)
#define MODLOG_DFLT_WARN(...)      LOG_W(__VA_ARGS__)
#define MODLOG_DFLT_ERROR(...)     LOG_E(__VA_ARGS__)
#define MODLOG_DFLT_CRITICAL(...)  LOG_E(__VA_ARGS__)
#define MODLOG_DFLT_DISABLED(...)  IGNORE(__VA_ARGS__)

#define MODLOG_DFLT(ml_lvl_, ...) \
    MODLOG_DFLT_ ## ml_lvl_(__VA_ARGS__)

#endif