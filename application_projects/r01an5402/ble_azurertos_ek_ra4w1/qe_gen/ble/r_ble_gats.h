/**********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name: r_ble_gats.h
 * Version : 1.0
 * Description : The header file for Generic Attribute service.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 31.12.2999 1.00 First Release
 ***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @file
 * @defgroup gats Generic Attribute Service Server
 * @{
 * @ingroup profile
 * @brief   The Generic Attribute Service contains generic information of the GATT attributes.
 **********************************************************************************************************************/
#include "profile_cmn/r_ble_servs_if.h"
#include "gatt_db.h"

#ifndef R_BLE_GATS_H
#define R_BLE_GATS_H

/*----------------------------------------------------------------------------------------------------------------------
    Service Changed Characteristic
----------------------------------------------------------------------------------------------------------------------*/

/***************************************************************************//**
 * @brief Service Changed value structure.
*******************************************************************************/
typedef struct {
    uint16_t start_hdl; /**< Start of Affected Attribute Handle Range */
    uint16_t end_hdl; /**< End of Affected Attribute Handle Range */
} st_ble_gats_serv_changed_t;

/***************************************************************************//**
 * @brief     Send indication of  Service Changed characteristic value to the remote device.
 * @param[in] conn_hdl Connection handle.
 * @param[in] p_value  Characteristic value to send.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_GATS_IndicateServChanged(uint16_t conn_hdl, const st_ble_gats_serv_changed_t *p_value);

/***************************************************************************//**
 * @brief     Set Service Changed cli cnfg descriptor value to the local GATT database.
 * @param[in] conn_hdl Connection handle.
 * @param[in] p_value  Descriptor value to set.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_GATS_SetServChangedCliCnfg(uint16_t conn_hdl, const uint16_t *p_value);

/***************************************************************************//**
 * @brief     Get Service Changed cli cnfg descriptor value from the local GATT database.
 * @param[in] conn_hdl Connection handle.
 * @param[out] p_value  Output location for the acquired descriptor value.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_GATS_GetServChangedCliCnfg(uint16_t conn_hdl, uint16_t *p_value);

/*----------------------------------------------------------------------------------------------------------------------
    Generic Attribute Service
----------------------------------------------------------------------------------------------------------------------*/

/***************************************************************************//**
 * @brief Generic Attribute characteristic Index.
*******************************************************************************/
typedef enum {
    BLE_GATS_SERV_CHGED_IDX,
    BLE_GATS_SERV_CHGED_CLI_CNFG_IDX,
} st_ble_gats_char_idx_t;

/***************************************************************************//**
 * @brief Generic Attribute event type.
*******************************************************************************/
typedef enum {
    /* Service Changed */
    BLE_GATS_EVENT_SERV_CHGED_HDL_VAL_CNF = BLE_SERVS_ATTR_EVENT(BLE_GATS_SERV_CHGED_IDX, BLE_SERVS_HDL_VAL_CNF),
} e_ble_gats_event_t;

/***************************************************************************//**
 * @brief     Initialize Generic Attribute service.
 * @param[in] cb Service callback.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_GATS_Init(ble_servs_app_cb_t cb);

#endif /* R_BLE_GATS_H */

/** @} */
