from building import *
import rtconfig

cwd = GetCurrentDir()
path = [cwd]
src = []

path += [
    cwd + '/nimble/include',
    cwd + '/nimble/host/include',
    cwd + '/nimble/host/src',
    cwd + '/nimble/host/services/ans/include',
    cwd + '/nimble/host/services/bas/include',
    cwd + '/nimble/host/services/bleuart/include',
    cwd + '/nimble/host/services/gap/include',
    cwd + '/nimble/host/services/gatt/include',
    cwd + '/nimble/host/services/ias/include',
    cwd + '/nimble/host/services/lls/include',
    cwd + '/nimble/host/services/tps/include',
    cwd + '/nimble/host/store/ram/include',
    cwd + '/nimble/host/util/include',
    cwd + '/nimble/transport/include',
    cwd + '/nimble/transport/common/hci_h4/include',
    cwd + '/porting/npl/rtthread/include',
    cwd + '/porting/nimble/include']

# Host stack
src += Split('''
    nimble/host/src/ble_att.c
    nimble/host/src/ble_att_clt.c
    nimble/host/src/ble_att_cmd.c
    nimble/host/src/ble_att_svr.c
    nimble/host/src/ble_eddystone.c
    nimble/host/src/ble_gap.c
    nimble/host/src/ble_gattc.c
    nimble/host/src/ble_gatts.c
    nimble/host/src/ble_gatts_lcl.c
    nimble/host/src/ble_hs_adv.c
    nimble/host/src/ble_hs_atomic.c
    nimble/host/src/ble_hs.c
    nimble/host/src/ble_hs_cfg.c
    nimble/host/src/ble_hs_conn.c
    nimble/host/src/ble_hs_flow.c
    nimble/host/src/ble_hs_hci.c
    nimble/host/src/ble_hs_hci_cmd.c
    nimble/host/src/ble_hs_hci_evt.c
    nimble/host/src/ble_hs_hci_util.c
    nimble/host/src/ble_hs_id.c
    nimble/host/src/ble_hs_log.c
    nimble/host/src/ble_hs_mbuf.c
    nimble/host/src/ble_hs_mqueue.c
    nimble/host/src/ble_hs_misc.c
    nimble/host/src/ble_hs_periodic_sync.c
    nimble/host/src/ble_hs_pvcy.c
    nimble/host/src/ble_hs_startup.c
    nimble/host/src/ble_hs_stop.c
    nimble/host/src/ble_ibeacon.c
    nimble/host/src/ble_l2cap.c
    nimble/host/src/ble_l2cap_coc.c
    nimble/host/src/ble_l2cap_sig.c
    nimble/host/src/ble_l2cap_sig_cmd.c
    nimble/host/src/ble_sm_alg.c
    nimble/host/src/ble_sm.c
    nimble/host/src/ble_sm_cmd.c
    nimble/host/src/ble_sm_lgcy.c
    nimble/host/src/ble_sm_sc.c
    nimble/host/src/ble_store.c
    nimble/host/src/ble_store_util.c
    nimble/host/src/ble_uuid.c
    nimble/host/services/ans/src/ble_svc_ans.c
    nimble/host/services/bas/src/ble_svc_bas.c
    nimble/host/services/gap/src/ble_svc_gap.c
    nimble/host/services/gatt/src/ble_svc_gatt.c
    nimble/host/services/ias/src/ble_svc_ias.c
    nimble/host/services/lls/src/ble_svc_lls.c
    nimble/host/services/tps/src/ble_svc_tps.c
    nimble/host/store/ram/src/ble_store_ram.c
    nimble/host/util/src/addr.c
    ''')

# HCI transport
src += Split('''
    nimble/transport/src/monitor.c
    nimble/transport/src/transport.c
    nimble/transport/common/hci_h4/src/hci_h4.c
    ''')

if GetDepend(['PKG_NIMBLE_HCI_USING_RTT_UART']):
    src += Split("""
        nimble/transport/rtthread/src/ble_hci_rtthread_uart.c
    """)

# mesh
if GetDepend(['PKG_NIMBLE_MESH']):
    path += [cwd + '/nimble/host/mesh/include']

    src += Split('''
        nimble/host/mesh/src/access.c
        nimble/host/mesh/src/adv_ext.c
        nimble/host/mesh/src/adv_legacy.c
        nimble/host/mesh/src/adv.c
        nimble/host/mesh/src/aes-ccm.c
        nimble/host/mesh/src/app_keys.c
        nimble/host/mesh/src/beacon.c
        nimble/host/mesh/src/cdb.c
        nimble/host/mesh/src/cfg_cli.c
        nimble/host/mesh/src/cfg_srv.c
        nimble/host/mesh/src/cfg.c
        nimble/host/mesh/src/crypto.c
        nimble/host/mesh/src/friend.c
        nimble/host/mesh/src/glue.c
        nimble/host/mesh/src/health_cli.c
        nimble/host/mesh/src/health_srv.c
        nimble/host/mesh/src/heartbeat.c
        nimble/host/mesh/src/light_model.c
        nimble/host/mesh/src/lpn.c
        nimble/host/mesh/src/mesh.c
        nimble/host/mesh/src/model_cli.c
        nimble/host/mesh/src/model_srv.c
        nimble/host/mesh/src/msg.c
        nimble/host/mesh/src/net.c
        nimble/host/mesh/src/pb_adv.c
        nimble/host/mesh/src/pb_gatt_srv.c
        nimble/host/mesh/src/pb_gatt.c
        nimble/host/mesh/src/prov_device.c
        nimble/host/mesh/src/prov.c
        nimble/host/mesh/src/provisioner.c
        nimble/host/mesh/src/proxy_msg.c
        nimble/host/mesh/src/proxy_srv.c
        nimble/host/mesh/src/rpl.c
        nimble/host/mesh/src/settings.c
        nimble/host/mesh/src/shell.c
        nimble/host/mesh/src/subnet.c
        nimble/host/mesh/src/transport.c
    ''')

# Few utils and data structures copied from Mynewt
src += Split('''
    porting/nimble/src/nimble_port.c''')

# Few utils and data structures copied from Mynewt
src += Split('''
    porting/nimble/src/endian.c
    porting/nimble/src/mem.c
    porting/nimble/src/os_mbuf.c
    porting/nimble/src/os_mempool.c
    porting/nimble/src/os_msys_init.c
    porting/npl/rtthread/src/nimble_port_rtthread.c
    porting/npl/rtthread/src/npl_os_rtthread.c
    porting/npl/rtthread/src/npl_shell.c
    ''')

# tinycrypt
path += [cwd + '/ext/tinycrypt/include']

src += Split('''
    ext/tinycrypt/src/aes_decrypt.c
    ext/tinycrypt/src/aes_encrypt.c
    ext/tinycrypt/src/cmac_mode.c
    ext/tinycrypt/src/ecc.c
    ext/tinycrypt/src/ecc_dh.c
    ext/tinycrypt/src/utils.c
    ''')

# controller
if GetDepend(['PKG_NIMBLE_CTLR']):
    CPPDEFINES = ['NIMBLE_CFG_CONTROLLER=1']
else:
    CPPDEFINES = []

if GetDepend(['PKG_NIMBLE_CTLR']):
    path += [cwd + '/nimble/controller/include']
        

    if GetDepend(['PKG_NIMBLE_BSP_NRF51']):
        path += [cwd + '/nimble/drivers/nrf51/include']
        src += Split("""
            nimble/drivers/nrf51/src/ble_hw.c
            nimble/drivers/nrf51/src/ble_phy.c
            """)
    else:
        path += [cwd + '/nimble/drivers/nrf52/include']
        src += Split("""
            nimble/drivers/nrf52/src/ble_hw.c
            nimble/drivers/nrf52/src/ble_phy.c
            """)

    src += Split("""
        nimble/controller/src/ble_ll_sched.c
        nimble/controller/src/ble_ll_whitelist.c
        nimble/controller/src/ble_ll_ctrl.c
        nimble/controller/src/ble_ll_hci.c
        nimble/controller/src/ble_ll_supp_cmd.c
        nimble/controller/src/ble_ll_adv.c
        nimble/controller/src/ble_ll_conn.c
        nimble/controller/src/ble_ll_resolv.c
        nimble/controller/src/ble_ll_conn_hci.c
        nimble/controller/src/ble_ll_rand.c
        nimble/controller/src/ble_ll.c
        nimble/controller/src/ble_ll_scan.c
        nimble/controller/src/ble_ll_dtm.c
        nimble/controller/src/ble_ll_hci_ev.c
        nimble/controller/src/ble_ll_hci_vs.c
        nimble/controller/src/ble_ll_iso.c
        nimble/controller/src/ble_ll_rfmgmt.c
        nimble/controller/src/ble_ll_scan_aux.c
        nimble/controller/src/ble_ll_sync.c
        nimble/controller/src/ble_ll_utils.c
        porting/nimble/src/os_cputime.c
        porting/nimble/src/os_cputime_pwr2.c
        porting/nimble/src/hal_timer.c
        porting/npl/rtthread/src/nrf5x_isr.c
        """)

# advertiser sample
if GetDepend(['PKG_NIMBLE_SAMPLE_ADVERTISER']):
    src += Split("""
        apps/advertiser/src/advertiser.c
    """)

# central sample
if GetDepend(['PKG_NIMBLE_SAMPLE_CENTRAL']):
    path += [cwd + '/apps/blecent/src']
    
    src += Split("""
        apps/blecent/src/blecent.c
        apps/blecent/src/misc.c
        apps/blecent/src/peer.c
        """)

# blecsc sensor
if GetDepend(['PKG_NIMBLE_SAMPLE_BLECSC']):
    path += [cwd + '/apps/blecsc/src']

    src += Split("""
        apps/blecsc/src/blecsc_sens.c
        apps/blecsc/src/gatt_svr.c
        """)


# peripheral heart-rate sample
if GetDepend(['PKG_NIMBLE_SAMPLE_PER_HR']):
    path += [cwd + '/apps/blehr/src']
    
    src += Split("""
        apps/blehr/src/blehr.c
        apps/blehr/src/gatt_svr.c
        """)

# bleprph sample
if GetDepend(['PKG_NIMBLE_SAMPLE_PERIPHERAL']):
    path += [cwd + '/apps/bleprph/src']
    
    src += Split("""
        apps/bleprph/src/bleprph.c
        apps/bleprph/src/gatt_svr.c
        apps/bleprph/src/misc.c
        """)

# bletest
if GetDepend(['PKG_NIMBLE_SAMPLE_BLETEST']):
    path += [cwd + '/apps/bletest/src']

    src += Split("""
        apps/bletest/src/bletest_hci.c
        apps/bletest/src/bletest.c
        """)

# bleuart
if GetDepend(['PKG_NIMBLE_SAMPLE_BLEUART']):
    path += [cwd + '/apps/bleuart/src']

    src += Split("""
        apps/bleuart/src/bleuart.c
        nimble/host/services/bleuart/src/bleuart.c
        """)

# btshell
if GetDepend(['PKG_NIMBLE_SAMPLE_BTSHELL']):
    path += [cwd + '/apps/btshell/src']

    src += Split("""
        apps/btshell/src/btshell.c
        apps/btshell/src/cmd_gatt.c
        apps/btshell/src/cmd_l2cap.c
        apps/btshell/src/cmd.c
        apps/btshell/src/gatt_svr.c
        apps/btshell/src/misc.c
        apps/btshell/src/parse.c
        """)

# ble external advertiser sample
if GetDepend(['PKG_NIMBLE_SAMPLE_EXT_ADVERTISER']):
    path += [cwd + '/apps/ext_advertiser/src']

    src += Split("""
        apps/ext_advertiser/src/ext_advertiser.c
        """)

# beacon sample
if GetDepend(['PKG_NIMBLE_SAMPLE_BEACON']):
    src += Glob('apps/ibeacon/src/ibeacon.c')

LOCAL_CCFLAGS = ''

if rtconfig.CROSS_TOOL == 'keil':
    LOCAL_CCFLAGS += ' --gnu --diag_suppress=111'
    # __BYTE_ORDER__ & __ORDER_BIG_ENDIAN__ & __ORDER_LITTLE_ENDIAN__ is not defined in keil
    CPPDEFINES.append('__ORDER_LITTLE_ENDIAN__=1234')
    CPPDEFINES.append('__ORDER_BIG_ENDIAN__=4321')
    CPPDEFINES.append('__BYTE_ORDER__=1234')
    
if rtconfig.CROSS_TOOL == 'gcc':
    LOCAL_CCFLAGS += ' -Wno-format -Wno-unused-variable -Wno-unused-but-set-variable'

group = DefineGroup('nimble', src, depend = ['PKG_USING_NIMBLE'], CPPPATH = path, CPPDEFINES = CPPDEFINES, LOCAL_CCFLAGS = LOCAL_CCFLAGS)

Return('group')
