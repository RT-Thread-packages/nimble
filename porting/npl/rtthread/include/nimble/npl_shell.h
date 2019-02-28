/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Date           Author       Notes
 * 2019-02-14     ZeroFree     first implementation
 */

#ifndef __NPL_SHELL_H__
#define __NPL_SHELL_H__

#include <rtthread.h>

/** @brief Callback called when command is entered.
 *
 *  @param argc Number of parameters passed.
 *  @param argv Array of option strings. First option is always command name.
 *
 * @return 0 in case of success or negative value in case of error.
 */
typedef int (*shell_cmd_func_t)(int argc, char *argv[]);

struct shell_param
{
    const char *param_name;
    const char *help;
};

struct shell_cmd_help
{
    const char *summary;
    const char *usage;
    const struct shell_param *params;
};

struct shell_cmd
{
    const char *sc_cmd;
    shell_cmd_func_t sc_cmd_func;
    const struct shell_cmd_help *help;
};

struct shell_module
{
    const char *name;
    const struct shell_cmd *commands;
};

/** @brief Register a shell_module object
 *
 *  @param shell_name Module name to be entered in shell console.
 *
 *  @param shell_commands Array of commands to register.
 *  The array should be terminated with an empty element.
 */
int shell_register(const char *shell_name,
                   const struct shell_cmd *shell_commands);

/** @brief Optionally register a default module, to avoid typing it in
 *  shell console.
 *
 *  @param name Module name.
 */
void shell_register_default_module(const char *name);

/** @brief Callback to get the current prompt.
 *
 *  @returns Current prompt string.
 */
typedef const char *(*shell_prompt_function_t)(void);

void shell_process_command(char *line);

#define console_printf rt_kprintf

#define OS_TICKS_PER_SEC RT_TICK_PER_SECOND

#define MYNEWT_VAL_SHELL_CMD_HELP 1

#if MYNEWT
#include "bsp/bsp.h"
#else
#define bssnz_t
#endif

void console_write(const char *str, int cnt);

#endif
