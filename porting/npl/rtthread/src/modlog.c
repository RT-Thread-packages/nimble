/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Date           Author       Notes
 * 2018-12-27     ChenYong     first implementation
 */

#include <string.h>

#include <rtthread.h>
#include <rthw.h>

#include <modlog/modlog.h>

/* This log is the first line of logs */
static rt_bool_t console_is_fline = RT_TRUE;

#ifdef NIMBLE_DEBUG_LEVEL
#define MODLOG_DEBUG_LEVEL   NIMBLE_DEBUG_LEVEL
#else
#define MODLOG_DEBUG_LEVEL   MODLOG_LEVEL_INFO
#endif /* NIMBLE_DBG_LEVEL */

#define DBG_ENABLE
#define DBG_COLOR
#define DBG_LEVEL            MODLOG_DEBUG_LEVEL

#ifndef DBG_SECTION_NAME
#define DBG_SECTION_NAME     "nimble"
#endif

#include <rtdbg.h>

static rt_size_t modlog_vprintf(const char *fmt, va_list args)
{
    static char modlog_buf[RT_CONSOLEBUF_SIZE];
    rt_device_t dev = rt_console_get_device();
    rt_size_t length;
    
    length = rt_vsnprintf(modlog_buf, sizeof(modlog_buf) - 1, fmt, args);
    
    #ifdef RT_USING_DEVICE
    if (dev == RT_NULL)
    {
        rt_hw_console_output(modlog_buf);
    }
    else
    {
        rt_uint16_t old_flag = dev->open_flag;

        dev->open_flag |= RT_DEVICE_FLAG_STREAM;
        rt_device_write(dev, 0, modlog_buf, length);
        dev->open_flag = old_flag;
    }
#else
    rt_hw_console_output(log);
#endif
    
    return length;
}

void modlog_dummy(uint8_t level, const char *msg, ...)
{
    va_list args;
    
    RT_ASSERT(msg);
    
    va_start(args, msg);
    switch (level)
    {
    case MODLOG_LEVEL_CRITICAL:
#if (MODLOG_DEBUG_LEVEL >= MODLOG_LEVEL_LOG)
    case MODLOG_LEVEL_LOG:
        if(console_is_fline) rt_kprintf("[D/%s] ", DBG_SECTION_NAME);
        modlog_vprintf(msg, args);
        break;
#endif
#if (MODLOG_DEBUG_LEVEL >= MODLOG_LEVEL_INFO)
    case MODLOG_LEVEL_INFO:
        if(console_is_fline) rt_kprintf("[I/%s] ", DBG_SECTION_NAME);
        modlog_vprintf(msg, args);
        break;
#endif
#if (MODLOG_DEBUG_LEVEL >= MODLOG_LEVEL_WARING)
    case MODLOG_LEVEL_WARING:
        if(console_is_fline) rt_kprintf("[W/%s] ", DBG_SECTION_NAME);
        modlog_vprintf(msg, args);
        break;
#endif
#if (MODLOG_DEBUG_LEVEL >= MODLOG_LEVEL_ERROR)
    case MODLOG_LEVEL_ERROR:
        if(console_is_fline) rt_kprintf("[E/%s] ", DBG_SECTION_NAME);
        modlog_vprintf(msg, args);
        break;
#endif
    }
    
    if (strchr(msg, '\n') != RT_NULL)
    {
        console_is_fline = RT_TRUE;
    }
    else
    {
        console_is_fline = RT_FALSE;
    }
    
    va_end(args);
}
