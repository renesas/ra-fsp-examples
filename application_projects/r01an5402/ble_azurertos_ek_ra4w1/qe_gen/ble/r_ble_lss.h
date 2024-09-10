/**********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name: r_ble_lss.h
 * Version : 1.0
 * Description : The header file for LED Switch service.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 31.12.2999 1.00 First Release
 ***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @file
 * @defgroup lss LED Switch Service Server
 * @{
 * @ingroup profile
 * @brief   This service exposes a control point to allow a peer device to control LEDs and switched on the device.
 **********************************************************************************************************************/

#include "profile_cmn/r_ble_servs_if.h"
#include "gatt_db.h"

#ifndef R_BLE_LSS_H
#define R_BLE_LSS_H

/*----------------------------------------------------------------------------------------------------------------------
    Switch State Characteristic
----------------------------------------------------------------------------------------------------------------------*/

/***************************************************************************//**
 * @brief     Send notification of  Switch State characteristic value to the remote device.
 * @param[in] conn_hdl Connection handle.
 * @param[in] p_value  Characteristic value to send.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_LSS_NotifySwitchState(uint16_t conn_hdl, const uint8_t *p_value);

/***************************************************************************//**
 * @brief     Set Switch State cli cnfg descriptor value to the local GATT database.
 * @param[in] conn_hdl Connection handle.
 * @param[in] p_value  Descriptor value to set.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_LSS_SetSwitchStateCliCnfg(uint16_t conn_hdl, const uint16_t *p_value);

/***************************************************************************//**
 * @brief     Get Switch State cli cnfg descriptor value from the local GATT database.
 * @param[in] conn_hdl Connection handle.
 * @param[out] p_value  Output location for the acquired descriptor value.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_LSS_GetSwitchStateCliCnfg(uint16_t conn_hdl, uint16_t *p_value);

/*----------------------------------------------------------------------------------------------------------------------
    LED Blink Rate Characteristic
----------------------------------------------------------------------------------------------------------------------*/
/***************************************************************************//**
 * @brief     Set LED Blink Rate characteristic value to the local GATT database.
 * @param[in] p_value  Characteristic value to set.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_LSS_SetBlinkRate(const uint8_t *p_value);

/***************************************************************************//**
 * @brief     Get LED Blink Rate characteristic value from the local GATT database.
 * @param[out] p_value  Output location for the acquired descriptor value.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_LSS_GetBlinkRate(uint8_t *p_value);

/*----------------------------------------------------------------------------------------------------------------------
    LED Switch Service
----------------------------------------------------------------------------------------------------------------------*/

/***************************************************************************//**
 * @brief LED Switch characteristic Index.
*******************************************************************************/
typedef enum {
    BLE_LSS_SWITCH_STATE_IDX,
    BLE_LSS_SWITCH_STATE_CLI_CNFG_IDX,
    BLE_LSS_BLINK_RATE_IDX,
} e_ble_lss_char_idx_t;

/***************************************************************************//**
 * @brief LED Switch event type.
*******************************************************************************/
typedef enum {
    /* Switch State */
    BLE_LSS_EVENT_SWITCH_STATE_CLI_CNFG_WRITE_REQ = BLE_SERVS_ATTR_EVENT(BLE_LSS_SWITCH_STATE_CLI_CNFG_IDX, BLE_SERVS_WRITE_REQ),
    BLE_LSS_EVENT_SWITCH_STATE_CLI_CNFG_WRITE_COMP = BLE_SERVS_ATTR_EVENT(BLE_LSS_SWITCH_STATE_CLI_CNFG_IDX, BLE_SERVS_WRITE_COMP),
    BLE_LSS_EVENT_SWITCH_STATE_CLI_CNFG_READ_REQ = BLE_SERVS_ATTR_EVENT(BLE_LSS_SWITCH_STATE_CLI_CNFG_IDX, BLE_SERVS_READ_REQ),
    /* LED Blink Rate */
    BLE_LSS_EVENT_BLINK_RATE_WRITE_REQ = BLE_SERVS_ATTR_EVENT(BLE_LSS_BLINK_RATE_IDX, BLE_SERVS_WRITE_REQ),
    BLE_LSS_EVENT_BLINK_RATE_WRITE_COMP = BLE_SERVS_ATTR_EVENT(BLE_LSS_BLINK_RATE_IDX, BLE_SERVS_WRITE_COMP),
    BLE_LSS_EVENT_BLINK_RATE_READ_REQ = BLE_SERVS_ATTR_EVENT(BLE_LSS_BLINK_RATE_IDX, BLE_SERVS_READ_REQ),
} e_ble_lss_event_t;

/***************************************************************************//**
 * @brief     Initialize LED Switch service.
 * @param[in] cb Service callback.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_LSS_Init(ble_servs_app_cb_t cb);

#endif /* R_BLE_LSS_H */

/** @} */
