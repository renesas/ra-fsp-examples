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
* Copyright (C) 2019-2022 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name: r_ble_tps.h
 * Description : The header file for Tx Power Service Server.
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @file
 * @defgroup tps Tx Power Service Server
 * @{
 * @ingroup profile
 * @brief   This service exposes a device's current transmit power level when in a connection.
 **********************************************************************************************************************/
#include "profile_cmn/r_ble_servs_if.h"
#include "gatt_db.h"

#ifndef R_BLE_TPS_H
#define R_BLE_TPS_H

/* Start user code for file includes and macro definitions. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/*----------------------------------------------------------------------------------------------------------------------
    Tx Power Level Characteristic : The Transmit Power Level characteristic represents the current transmit power level in dBm,and the level ranges from -100 dBm to +20 dBm to a resolution of 1 dBm.
----------------------------------------------------------------------------------------------------------------------*/
/***************************************************************************//**
 * @brief     Set Tx Power Level characteristic value to the local GATT database.
 * @param[in] conn_hdl Connection handle.
 * @param[in] p_value  Characteristic value to set.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_TPS_SetTxPowerLevel(uint16_t conn_hdl, const int8_t *p_value);

/***************************************************************************//**
 * @brief     Get Tx Power Level characteristic value from the local GATT database.
 * @param[in] conn_hdl Connection handle.
 * @param[out] p_value  Output location for the acquired descriptor value.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_TPS_GetTxPowerLevel(uint16_t conn_hdl, int8_t *p_value);

/*----------------------------------------------------------------------------------------------------------------------
    Tx Power Service Server
----------------------------------------------------------------------------------------------------------------------*/

/***************************************************************************//**
 * @brief Tx Power Service characteristic Index.
*******************************************************************************/
typedef enum {
    BLE_TPS_TX_POWER_LEVEL_IDX,
} e_ble_tps_char_idx_t;

/***************************************************************************//**
 * @brief Tx Power Service event type.
*******************************************************************************/
typedef enum {
    /* Tx Power Level */
    BLE_TPS_EVENT_TX_POWER_LEVEL_READ_REQ = BLE_SERVS_ATTR_EVENT(BLE_TPS_TX_POWER_LEVEL_IDX, BLE_SERVS_READ_REQ),
} e_ble_tps_event_t;

/***************************************************************************//**
 * @brief     Initialize Tx Power Service server.
 * @param[in] cb Service callback.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_TPS_Init(ble_servs_app_cb_t cb);

/* Start user code for function declarations. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

#endif /* R_BLE_TPS_H */

/** @} */
