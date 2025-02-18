/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
* this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name: r_ble_lss.h
 * Description : The header file for LED Switch service.
 **********************************************************************************************************************/
/***********************************************************************************************************************
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
