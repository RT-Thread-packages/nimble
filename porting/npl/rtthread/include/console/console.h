/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Date           Author       Notes
 * 2018-12-29     ChenYong     first implementation
 */

#ifndef __CONSOLE_H__
#define __CONSOLE_H__

// TODO:
#include <rtthread.h>

#define console_printf rt_kprintf

/**
 * XXX: Deprecated.  Use LOG_DFLT instead.
 *
 * @brief Writes a formatted text entry with the specified level to the
 * default log module.
 *
 * The provided log level must be one of the following tokens:
 *     o CRITICAL
 *     o ERROR
 *     o WARN
 *     o INFO
 *     o DEBUG
 *
 * This expands to nothing if the global log level is greater than
 * the specified level.
 *
 * @param ml_lvl_               The log level of the entry to write.
 */
#define MODLOG_DFLT(ml_lvl_, ...) \
    MODLOG(ml_lvl_, LOG_MODULE_DEFAULT, __VA_ARGS__)

#endif
