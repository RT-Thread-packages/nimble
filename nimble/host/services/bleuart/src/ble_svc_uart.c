/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "sysinit/sysinit.h"
#include "host/ble_hs.h"
#include "host/ble_uuid.h"
#include "bleuart/bleuart.h"
#include "os/endian.h"

#include <rtthread.h>
#include <rtdevice.h>
#include <rthw.h>

#define MYNEWT_VAL_BLEUART_MAX_INPUT 128

#define ESC_KEY                      0x1B
#define BACKSPACE_KEY                0x08
#define DELECT_KEY                   0x7F

static void bleuart_deinit(void);
    
struct bleuart_console
{
    struct rt_semaphore *rx_end;
    struct rt_ringbuffer *rx_fifo;
    rt_err_t (*old_rx_ind)(rt_device_t dev, rt_size_t size);
};

/* ble uart attr read handle */
uint16_t g_bleuart_attr_read_handle;

/* ble uart attr write handle */
uint16_t g_bleuart_attr_write_handle;

/* Pointer to a console buffer */
char *console_buf;

/* ble uart console */
static struct bleuart_console bleuart;

uint16_t g_console_conn_handle;
/**
 * The vendor specific "bleuart" service consists of one write no-rsp characteristic
 * and one notification only read charateristic
 *     o "write no-rsp": a single-byte characteristic that can be written only
 *       over a non-encrypted connection
 *     o "read": a single-byte characteristic that can always be read only via
 *       notifications
 */

/* {6E400001-B5A3-F393-E0A9-E50E24DCCA9E} */
const ble_uuid128_t gatt_svr_svc_uart_uuid =
    BLE_UUID128_INIT(0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0,
                     0x93, 0xf3, 0xa3, 0xb5, 0x01, 0x00, 0x40, 0x6e);

/* {6E400002-B5A3-F393-E0A9-E50E24DCCA9E} */
const ble_uuid128_t gatt_svr_chr_uart_write_uuid =
    BLE_UUID128_INIT(0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0,
                     0x93, 0xf3, 0xa3, 0xb5, 0x02, 0x00, 0x40, 0x6e);


/* {6E400003-B5A3-F393-E0A9-E50E24DCCA9E} */
const ble_uuid128_t gatt_svr_chr_uart_read_uuid =
    BLE_UUID128_INIT(0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0,
                     0x93, 0xf3, 0xa3, 0xb5, 0x03, 0x00, 0x40, 0x6e);

static int
gatt_svr_chr_access_uart_write(uint16_t conn_handle, uint16_t attr_handle,
                              struct ble_gatt_access_ctxt *ctxt, void *arg);

static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
    {
        /* Service: uart */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &gatt_svr_svc_uart_uuid.u,
        .characteristics = (struct ble_gatt_chr_def[]) { {
            .uuid = &gatt_svr_chr_uart_read_uuid.u,
            .val_handle = &g_bleuart_attr_read_handle,
            .access_cb = gatt_svr_chr_access_uart_write,
            .flags = BLE_GATT_CHR_F_NOTIFY,
        }, {
            /* Characteristic: Write */
            .uuid = &gatt_svr_chr_uart_write_uuid.u,
            .access_cb = gatt_svr_chr_access_uart_write,
            .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_NO_RSP,
            .val_handle = &g_bleuart_attr_write_handle,
        }, {
            0, /* No more characteristics in this service */
        } },
    },

    {
        0, /* No more services */
    },
};

static int
gatt_svr_chr_access_uart_write(uint16_t conn_handle, uint16_t attr_handle,
                               struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    struct os_mbuf *om = ctxt->om;
    switch (ctxt->op) {
        case BLE_GATT_ACCESS_OP_WRITE_CHR:
              while(om) {
                  rt_device_write(rt_console_get_device(), 0, (char *)om->om_data, om->om_len);
                  om = SLIST_NEXT(om, om_next);
              }
              rt_device_write(rt_console_get_device(), 0, "\n", 1);
              return 0;
        default:
            assert(0);
            return BLE_ATT_ERR_UNLIKELY;
    }
}

/**
 * bleuart GATT server initialization
 *
 * @param eventq
 * @return 0 on success; non-zero on failure
 */
int
bleuart_gatt_svr_init(void)
{
    int rc;

    rc = ble_gatts_count_cfg(gatt_svr_svcs);
    if (rc != 0) {
        goto err;
    }

    rc = ble_gatts_add_svcs(gatt_svr_svcs);
    if (rc != 0) {
        return rc;
    }

err:
    return rc;
}

static rt_err_t bleuart_rx_ind(rt_device_t dev, rt_size_t size)
{
    uint8_t ch;
    int i;

    for(i = 0; i < size; i++)
    {
        /* read a char */
        if (rt_device_read(dev, 0, &ch, 1))
        {
            rt_ringbuffer_put_force(bleuart.rx_fifo, &ch, 1);
            rt_sem_release(bleuart.rx_end);
        }
    }

    return RT_EOK;
}

static uint8_t bleuart_read(void)
{
    uint8_t ch;

    rt_sem_take(bleuart.rx_end, RT_WAITING_FOREVER);
    rt_ringbuffer_getchar(bleuart.rx_fifo, &ch);

    return ch;
}

/**
 * Reads console and sends data over BLE
 */
static void bleuart_uart_proc(void)
{
    int off = 0;
    char ch;
    struct os_mbuf *om;

    rt_kprintf("======== Welcome to enter bluetooth uart mode ========\n");
    rt_kprintf("Press 'ESC' to exit.\n");

    /* process user input */
    while (ESC_KEY != (ch = bleuart_read()))
    {
        if(ch != '\r' && ch != '\n')
        {
            if(ch == BACKSPACE_KEY || ch == DELECT_KEY)
            {
                if(off)
                {
                    console_buf[off--] = 0;
                    rt_kprintf("\b \b");
                }
            }
            else
            {
                console_buf[off++] = ch;
                rt_kprintf("%c", ch);
                continue;
            }
        }    

        console_buf[off] = '\0';
        rt_kprintf("\n");
        om = ble_hs_mbuf_from_flat(console_buf, off);
        if (!om) {
            return;
        }
        ble_gattc_notify_custom(g_console_conn_handle,
                                g_bleuart_attr_read_handle, om);
        off = 0;
    }

    bleuart_deinit();
}

/**
 * Sets the global connection handle
 *
 * @param connection handle
 */
void
bleuart_set_conn_handle(uint16_t conn_handle) {
    g_console_conn_handle = conn_handle;
}

static void bleuart_deinit(void)
{
    rt_base_t level;
    rt_device_t uart_console;

    level = rt_hw_interrupt_disable();
    uart_console = rt_console_get_device();
    if(uart_console)
    {
        rt_device_set_rx_indicate(uart_console, bleuart.old_rx_ind);
    }
    rt_hw_interrupt_enable(level);

    if (console_buf != RT_NULL)
    {
        rt_free(console_buf);
        console_buf = RT_NULL;
    }

    if (bleuart.rx_end != RT_NULL)
    {
        rt_sem_delete(bleuart.rx_end);
        bleuart.rx_end = RT_NULL;
    }

    if (bleuart.rx_fifo != RT_NULL)
    {
        rt_ringbuffer_destroy(bleuart.rx_fifo);
        bleuart.rx_fifo = RT_NULL;
    }
}

/**
 * BLEuart console initialization
 *
 * @param Maximum input
 */
int bleuart_init(void)
{
    int rc;
    rt_base_t level;
    rt_device_t uart_console;

    /* Ensure this function only gets called by sysinit. */
    SYSINIT_ASSERT_ACTIVE();

    /* create buffer for send */
    console_buf = rt_malloc(MYNEWT_VAL(BLEUART_MAX_INPUT));
    if (console_buf == RT_NULL)
    {
        rc = -RT_ENOMEM;
        goto __exit;
    }

    /* create semaphore for the end of char recived */
    bleuart.rx_end = rt_sem_create("bleuart", 0, RT_IPC_FLAG_FIFO);
    if (bleuart.rx_end == RT_NULL)
    {
        rc = -RT_ENOMEM;
        goto __exit;
    }

    /* create recived fifo */
    bleuart.rx_fifo = rt_ringbuffer_create(MYNEWT_VAL(BLEUART_MAX_INPUT));
    if (bleuart.rx_fifo == RT_NULL)
    {
        rc = -RT_ENOMEM;
        goto __exit;
    }

    level = rt_hw_interrupt_disable();
    uart_console = rt_console_get_device();
    if(uart_console)
    {
        /* back uart console old indicate callback */
        bleuart.old_rx_ind = uart_console->rx_indicate;
        rt_device_set_rx_indicate(uart_console, bleuart_rx_ind);
    }
    rt_hw_interrupt_enable(level);

    /* Reads console and sends data over BLE */
    bleuart_uart_proc();

    return RT_EOK;

__exit:
    if (console_buf != RT_NULL)
    {
        rt_free(console_buf);
        console_buf = RT_NULL;
    }

    if (bleuart.rx_end != RT_NULL)
    {
        rt_sem_delete(bleuart.rx_end);
        bleuart.rx_end = RT_NULL;
    }

    if (bleuart.rx_fifo != RT_NULL)
    {
        rt_ringbuffer_destroy(bleuart.rx_fifo);
        bleuart.rx_fifo = RT_NULL;
    }

    return rc;
}
