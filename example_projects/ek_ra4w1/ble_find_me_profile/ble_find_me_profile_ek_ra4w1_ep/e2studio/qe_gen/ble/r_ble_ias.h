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
* Copyright (C) 2019-2020 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name: r_ble_ias.h
 * Description : The header file for Immediate Alert Service Server.
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @file
 * @defgroup ias Immediate Alert Service Server
 * @{
 * @ingroup profile
 * @brief   This service exposes a control point to allow a peer device to cause the device to immediately alert.
 **********************************************************************************************************************/
#include "profile_cmn/r_ble_servs_if.h"
#include "gatt_db.h"

#ifndef R_BLE_IAS_H
#define R_BLE_IAS_H


/*----------------------------------------------------------------------------------------------------------------------
    Alert Level Characteristic : The level of an alert a device is to sound. If this level is changed while the alert is being sounded,the new level should take effect.
----------------------------------------------------------------------------------------------------------------------*/
/***************************************************************************//**
 * @brief Alert Level Level enumeration.
*******************************************************************************/
typedef enum {
    BLE_IAS_ALERT_LEVEL_LEVEL_NO_ALERT = 0, /**< No Alert */
    BLE_IAS_ALERT_LEVEL_LEVEL_MILD_ALERT = 1, /**< Mild Alert */
    BLE_IAS_ALERT_LEVEL_LEVEL_HIGH_ALERT = 2, /**< High Alert */
} e_ble_ias_alert_level_level_t;

/*----------------------------------------------------------------------------------------------------------------------
    Immediate Alert Service Server
----------------------------------------------------------------------------------------------------------------------*/

/***************************************************************************//**
 * @brief Immediate Alert Service characteristic Index.
*******************************************************************************/
typedef enum {
    BLE_IAS_ALERT_LEVEL_IDX,
} e_ble_ias_char_idx_t;

/***************************************************************************//**
 * @brief Immediate Alert Service event type.
*******************************************************************************/
typedef enum {
    /* Alert Level */
    BLE_IAS_EVENT_ALERT_LEVEL_WRITE_CMD = BLE_SERVS_ATTR_EVENT(BLE_IAS_ALERT_LEVEL_IDX, BLE_SERVS_WRITE_CMD),
} e_ble_ias_event_t;

/***************************************************************************//**
 * @brief     Initialize Immediate Alert Service server.
 * @param[in] cb Service callback.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_IAS_Init(ble_servs_app_cb_t cb);


#endif /* R_BLE_IAS_H */

/** @} */
