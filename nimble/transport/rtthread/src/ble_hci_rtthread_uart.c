/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-07-16     Lenoyan      the first version
 */

#include <assert.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <sysinit/sysinit.h>
#include <syscfg/syscfg.h>
#include "os/os_mbuf.h"
#include "nimble/transport.h"
#include "nimble/transport/hci_h4.h"

#include <rtthread.h>
#include <rtdevice.h>

/* hci_h4 rx instance  */
struct hci_h4_sm g_hci_h4sm;

/* uart device handle */
static rt_device_t g_serial;

static int hci_uart_frame_cb(uint8_t pkt_type, void *data)
{
    switch (pkt_type) {
    case HCI_H4_EVT:
        return ble_transport_to_hs_evt(data);
    case HCI_H4_ACL:
        return ble_transport_to_hs_acl(data);
    default:
        assert(0);
        break;
    }
    return -1;
}

// RT-Thread UART rx data
static void rtthread_uart_rx_entry(void *parameter)
{
    uint8_t data[64];
    size_t data_len;
    while (1) {
        data_len = rt_device_read(g_serial, 0, &data, 64);
        if (data_len > 0) {
            hci_h4_sm_rx(&g_hci_h4sm, data, data_len);
        }
    }
}

// UART send data
static void rtthread_uart_tx(const uint8_t *buf, size_t len)
{
    size_t remaining = len;
    size_t tx_size = 0;

    while (remaining > 0) {
        tx_size = rt_device_write(g_serial, 0, buf, remaining);
        buf += tx_size;
        remaining -= tx_size;
    }

}

// init uart
static int rtthread_hci_uart_init(void)
{
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;  

    g_serial = rt_device_find(PKG_NIMBLE_HCI_UART_DEVICE_NAME);
    if (!g_serial) {
        return -1;
    }

    config.baud_rate = BAUD_RATE_115200;
    config.data_bits = DATA_BITS_8;
    config.stop_bits = STOP_BITS_1;
    config.bufsz     = 128;
    config.parity    = PARITY_NONE;

    rt_device_control(g_serial, RT_DEVICE_CTRL_CONFIG, &config);

    rt_device_open(g_serial, RT_DEVICE_FLAG_INT_RX);

    rt_thread_t rx_thread = rt_thread_create("hci_uart_rx", rtthread_uart_rx_entry,
                                            RT_NULL, 1024, 25, 10);
    if (rx_thread != RT_NULL) {
        rt_thread_startup(rx_thread);
    } else {
        return -1;
    }

    return 0;
}

int ble_transport_to_ll_cmd_impl(void *buf)
{
    uint8_t indicator = HCI_H4_CMD;
    uint8_t *cmd_pkt_data = (uint8_t *)buf;
    size_t pkt_len = cmd_pkt_data[2] + 3;  // parameter_len + header_len(3) 
    
    rtthread_uart_tx(&indicator, 1);  // send indicator
    rtthread_uart_tx(cmd_pkt_data, pkt_len);  //send cmd pkt data

    ble_transport_free(buf);  //free hci pkt 

    return 0;
}

int ble_transport_to_ll_acl_impl(struct os_mbuf *om)
{
    uint8_t indicator = HCI_H4_ACL;

    rtthread_uart_tx(&indicator, 1);  // send indicator

    // single-list, send all node data
    struct os_mbuf *x = om;
    while (x != NULL)
    {
        rtthread_uart_tx(x->om_data, x->om_len);
        x = SLIST_NEXT(x, om_next);
    }
    
    os_mbuf_free_chain(om); // free os_mbuf chain

    return 0;
}

static int rtthread_ble_transport_init(void)
{
    int rc;
    SYSINIT_ASSERT_ACTIVE();

    rc = rtthread_hci_uart_init();
    SYSINIT_PANIC_ASSERT(rc == 0);

    // init hci_h4 instance
    hci_h4_sm_init(&g_hci_h4sm, &hci_h4_allocs_from_ll, hci_uart_frame_cb);
    return 0;
}

#ifdef PKG_NIMBLE_HCI_USING_RTT_UART
INIT_APP_EXPORT(rtthread_ble_transport_init);
#endif