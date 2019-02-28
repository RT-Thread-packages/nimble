
#include "os/os.h"
#include "nimble/hci_common.h"
#include "host/ble_hs_hci.h"
#include "ble_hs_priv.h"

#include <rtthread.h>

static void ble_app_set_addr(void)
{
    ble_addr_t addr;
    int rc;

    rc = ble_hs_id_gen_rnd(1, &addr);
    RT_ASSERT(rc == 0);

    rc = ble_hs_id_set_rnd(addr.val);
    RT_ASSERT(rc == 0);
}

static void ble_app_advertise(void)
{
    struct ble_gap_adv_params adv_params;
    uint8_t uuid128[16];
    int rc;
    int8_t tx_pwr;

    /* Arbitrarily set the UUID to a string of 0x11 bytes. */
    memset(uuid128, 0x11, sizeof uuid128);

    rc = ble_hs_hci_util_read_adv_tx_pwr(&tx_pwr);
    RT_ASSERT(rc == 0);

    /* Major version=2; minor version=10. */
    rc = ble_ibeacon_set_adv_data(uuid128, 2, 10, tx_pwr);
    RT_ASSERT(rc == 0);

    /* Begin advertising. */
    adv_params = (struct ble_gap_adv_params){ 0 };
    rc = ble_gap_adv_start(BLE_OWN_ADDR_RANDOM, NULL, BLE_HS_FOREVER,
                           &adv_params, NULL, NULL);
    RT_ASSERT(rc == 0);
}

static void ble_app_on_sync(void)
{
    /* Generate a non-resolvable private address. */
    ble_app_set_addr();

    /* Advertise indefinitely. */
    ble_app_advertise();
}

static int ble_ibeacon(void)
{
    static int init_flag = 0;

    if (init_flag)
        return 0;
    init_flag = 1;

    ble_hs_cfg.sync_cb = ble_app_on_sync;

    /* startup bluetooth host stack*/
    ble_hs_thread_startup();

    return 0;
}
MSH_CMD_EXPORT_ALIAS(ble_ibeacon, ble_ibeacon, "bluetoooth ibeacon sample");
