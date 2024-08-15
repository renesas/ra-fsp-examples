/***********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
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
