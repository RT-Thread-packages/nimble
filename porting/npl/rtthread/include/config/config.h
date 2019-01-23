/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Date           Author       Notes
 * 2018-12-29     ChenYong     first implementation
 */

#ifndef __NUMBLE_CONFIG_H__
#define __NUMBLE_CONFIG_H__

#include <rtconfig.h>

#ifdef NIMBT_ROLE_PERIPHERAL
#define MYNEWT_VAL_BLE_ROLE_PERIPHERAL           1
#endif

#ifdef NIMBT_ROLE_CENTRAL
#define MYNEWT_VAL_BLE_ROLE_CENTRAL              1
#endif 

#ifdef NIMBT_ROLE_BROADCASTER
#define MYNEWT_VAL_BLE_ROLE_BROADCASTER          1
#endif

#ifdef NIMBT_ROLE_OBSERVER
#define MYNEWT_VAL_BLE_ROLE_OBSERVER             1
#endif

#ifdef NIMBLE_CTLR
#define MYNEWT_VAL_BLE_CTLR                      1
#endif

#endif
