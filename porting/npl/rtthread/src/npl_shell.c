/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Date           Author       Notes
 * 2019-02-14     ZeroFree     first implementation
 */

#include <string.h>
#include "nimble/nimble_npl.h"
#include "nimble/npl_shell.h"

#include <rtthread.h>

#define SHELL_PROMPT "shell"
#define SHELL_MAX_MODULES 3
#define SHELL_PROMPT_SUFFIX "> "

static const char *get_command_and_module(char *argv[], int *module);
static int set_default_module(const char *name);
static void print_prompt(void);
static int get_destination_module(const char *module_str, int len);
static int show_cmd_help(char *argv[]);

static struct shell_module shell_modules[SHELL_MAX_MODULES];
static size_t num_of_shell_entities;

static const char *prompt;
static int default_module = -1;

static shell_cmd_func_t app_cmd_handler;
static shell_prompt_function_t app_prompt_handler;

void console_write(const char *str, int cnt)
{
    rt_device_write(rt_console_get_device(), 0, str, cnt);
}

int shell_register(const char *module_name, const struct shell_cmd *commands)
{
    if (num_of_shell_entities >= SHELL_MAX_MODULES)
    {
        console_printf("Max number of modules reached\n");
        return -1;
    }

    shell_modules[num_of_shell_entities].name = module_name;
    shell_modules[num_of_shell_entities].commands = commands;
    ++num_of_shell_entities;

    return 0;
}

void shell_register_default_module(const char *name)
{
    int result = set_default_module(name);

    if (result != -1)
    {
        console_printf("\n");
        print_prompt();
    }
}

static void print_modules(void)
{
    int module;

    for (module = 0; module < num_of_shell_entities; module++)
    {
        console_printf("%s\n", shell_modules[module].name);
    }
}

static void print_module_commands(const int module)
{
    const struct shell_module *shell_module = &shell_modules[module];
    int i;

    console_printf("help\n");

    for (i = 0; shell_module->commands[i].sc_cmd; i++)
    {
        console_printf("%-30s", shell_module->commands[i].sc_cmd);
        if (shell_module->commands[i].help &&
                shell_module->commands[i].help->summary)
        {
            console_printf("%s", shell_module->commands[i].help->summary);
        }
        console_printf("\n");
    }
}

static int show_help(int argc, char *argv[])
{
    int module;

    /* help per command */
    if ((argc > 2) || ((default_module != -1) && (argc == 2)))
    {
        return show_cmd_help(&argv[1]);
    }

    /* help per module */
    if ((argc == 2) || ((default_module != -1) && (argc == 1)))
    {
        if (default_module == -1)
        {
            module = get_destination_module(argv[1], -1);
            if (module == -1)
            {
                console_printf("Illegal module %s\n", argv[1]);
                return 0;
            }
        }
        else
        {
            module = default_module;
        }

        print_module_commands(module);
    }
    else     /* help for all entities */
    {
        console_printf("Available modules:\n");
        print_modules();
        console_printf("To select a module, enter 'select <module name>'.\n");
    }

    return 0;
}

static int set_default_module(const char *name)
{
    int module;

    module = get_destination_module(name, -1);

    if (module == -1)
    {
        console_printf("Illegal module %s, default is not changed\n", name);
        return -1;
    }

    default_module = module;

    return 0;
}

static int select_module(int argc, char *argv[])
{
    if (argc == 1)
    {
        default_module = -1;
    }
    else
    {
        set_default_module(argv[1]);
    }

    return 0;
}

static size_t line2argv(char *str, char *argv[], size_t size)
{
    size_t argc = 0;

    if (!strlen(str))
    {
        return 0;
    }

    while (*str && *str == ' ')
    {
        str++;
    }

    if (!*str)
    {
        return 0;
    }

    argv[argc++] = str;

    while ((str = strchr(str, ' ')))
    {
        *str++ = '\0';

        while (*str && *str == ' ')
        {
            str++;
        }

        if (!*str)
        {
            break;
        }

        argv[argc++] = str;

        if (argc == size)
        {
            console_printf("Too many parameters (max %zu)\n", size - 1);
            return 0;
        }
    }

    /* keep it POSIX style where argv[argc] is required to be NULL */
    argv[argc] = NULL;

    return argc;
}

static shell_cmd_func_t get_cb(int argc, char *argv[])
{
    const char *first_string = argv[0];
    int module = -1;
    const struct shell_module *shell_module;
    const char *command;
    int i;

    if (!first_string || first_string[0] == '\0')
    {
        console_printf("Illegal parameter\n");
        return NULL;
    }

    if (!strcmp(first_string, "help"))
    {
        return show_help;
    }

    if (!strcmp(first_string, "select"))
    {
        return select_module;
    }

    if ((argc == 1) && (default_module == -1))
    {
        console_printf("Missing parameter\n");
        return NULL;
    }

    command = get_command_and_module(argv, &module);
    if ((module == -1) || (command == NULL))
    {
        return NULL;
    }

    shell_module = &shell_modules[module];
    for (i = 0; shell_module->commands[i].sc_cmd; i++)
    {
        if (!strcmp(command, shell_module->commands[i].sc_cmd))
        {
            return shell_module->commands[i].sc_cmd_func;
        }
    }

    return NULL;
}


static const char *get_prompt(void)
{
    const char *str;

    if (app_prompt_handler)
    {

        str = app_prompt_handler();
        if (str)
        {
            return str;
        }
    }

    if (default_module != -1)
    {
        return shell_modules[default_module].name;
    }

    return prompt;
}

static void print_prompt(void)
{
    console_printf("%s%s", get_prompt(), SHELL_PROMPT_SUFFIX);
}

static int get_destination_module(const char *module_str, int len)
{
    int i;

    for (i = 0; i < num_of_shell_entities; i++)
    {
        if (len < 0)
        {
            if (!strcmp(module_str, shell_modules[i].name))
            {
                return i;
            }
        }
        else
        {
            if (!strncmp(module_str, shell_modules[i].name, len))
            {
                return i;
            }
        }
    }

    return -1;
}

/* For a specific command: argv[0] = module name, argv[1] = command name
 * If a default module was selected: argv[0] = command name
 */
static const char *get_command_and_module(char *argv[], int *module)
{
    *module = -1;

    if (!argv[0])
    {
        console_printf("Unrecognized command\n");
        return NULL;
    }

    if (default_module == -1)
    {
        if (!argv[1] || argv[1][0] == '\0')
        {
            console_printf("Unrecognized command: %s\n", argv[0]);
            return NULL;
        }

        *module = get_destination_module(argv[0], -1);
        if (*module == -1)
        {
            console_printf("Illegal module %s\n", argv[0]);
            return NULL;
        }

        return argv[1];
    }

    *module = default_module;
    return argv[0];
}

static void print_command_params(const int module, const int command)
{
    const struct shell_module *shell_module = &shell_modules[module];
    const struct shell_cmd *shell_cmd = &shell_module->commands[command];
    int i;

    if (!(shell_cmd->help && shell_cmd->help->params))
    {
        return;
    }

    for (i = 0; shell_cmd->help->params[i].param_name; i++)
    {
        console_printf("%-30s%s\n", shell_cmd->help->params[i].param_name,
                       shell_cmd->help->params[i].help);
    }
}

static int show_cmd_help(char *argv[])
{
    const char *command = NULL;
    int module = -1;
    const struct shell_module *shell_module = NULL;
    const struct shell_cmd *cmd;
    int i;

    command = get_command_and_module(argv, &module);
    if ((module == -1) || (command == NULL))
    {
        return 0;
    }

    shell_module = &shell_modules[module];
    for (i = 0; shell_module->commands[i].sc_cmd; i++)
    {
        cmd = &shell_module->commands[i];

        if (!strcmp(command, cmd->sc_cmd))
        {

            if (!cmd->help || (!cmd->help->summary &&
                               !cmd->help->usage &&
                               !cmd->help->params))
            {
                console_printf("(no help available)\n");
                return 0;
            }

            if (cmd->help->summary)
            {
                console_printf("Summary:\n");
                console_printf("%s\n", cmd->help->summary);
            }

            if (cmd->help->usage)
            {
                console_printf("Usage:\n");
                console_printf("%s\n", cmd->help->usage);
            }

            if (cmd->help->params)
            {
                console_printf("Parameters:\n");
                print_command_params(module, i);
            }

            return 0;
        }
    }

    console_printf("Unrecognized command: %s\n", argv[0]);
    return 0;
}

void shell_process_command(char *line)
{
    char *argv[FINSH_CMD_SIZE + 1];
    shell_cmd_func_t sc_cmd_func;
    size_t argc_offset = 0;
    size_t argc;

    argc = line2argv(line, argv, FINSH_CMD_SIZE + 1);
    if (!argc)
    {
        print_prompt();
        return;
    }

    sc_cmd_func = get_cb(argc, argv);
    if (!sc_cmd_func)
    {
        if (app_cmd_handler != NULL)
        {
            sc_cmd_func = app_cmd_handler;
        }
        else
        {
            console_printf("Unrecognized command: %s\n", argv[0]);
            console_printf("Type 'help' for list of available commands\n");
            print_prompt();
            return;
        }
    }

    /* Allow invoking a cmd with module name as a prefix; a command should
     * not know how it was invoked (with or without prefix)
     */
    if (default_module == -1 && sc_cmd_func != select_module &&
            sc_cmd_func != show_help)
    {
        argc_offset = 1;
    }

    /* Execute callback with arguments */
    if (sc_cmd_func(argc - argc_offset, &argv[argc_offset]) < 0)
    {
        show_cmd_help(argv);
    }

    print_prompt();
}
