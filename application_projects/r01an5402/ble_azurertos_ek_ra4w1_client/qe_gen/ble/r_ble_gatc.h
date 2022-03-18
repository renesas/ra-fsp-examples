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
 * File Name: r_ble_gatc.h
 * Version : 1.0
 * Description : The header file for Generic Attribute client.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 31.12.2999 1.00 First Release
 ***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @file
 * @defgroup gatc Generic Attribute Service Client
 * @{
 * @ingroup profile
 * @brief   This is the client for the Generic Attribute Service.
 **********************************************************************************************************************/
#include "profile_cmn/r_ble_servc_if.h"

#ifndef R_BLE_GATC_H
#define R_BLE_GATC_H

/*----------------------------------------------------------------------------------------------------------------------
    Service Changed Characteristic
----------------------------------------------------------------------------------------------------------------------*/
#define BLE_GATC_SERV_CHANGED_UUID (0x2A05)
#define BLE_GATC_SERV_CHANGED_LEN (4)
#define BLE_GATC_SERV_CHANGED_CLI_CNFG_UUID (0x2902)
#define BLE_GATC_SERV_CHANGED_CLI_CNFG_LEN (2)

/***************************************************************************//**
 * @brief Service Changed value structure.
*******************************************************************************/
typedef struct {
    uint16_t start_hdl; /**< Start of Affected Attribute Handle Range */
    uint16_t end_hdl; /**< End of Affected Attribute Handle Range */
} st_ble_gatc_serv_changed_t;

/***************************************************************************//**
 * @brief Service Changed attribute handle value.
*******************************************************************************/
typedef struct {
    st_ble_gatt_hdl_range_t range;
    uint16_t cli_cnfg_desc_hdl;
} st_ble_gatc_serv_changed_attr_hdl_t;

/***************************************************************************//**
 * @brief     Read Service Changed characteristic Client Characteristic Configuration descriptor value from the remote GATT database.
 * @param[in] conn_hdl Connection handle.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_GATC_ReadServChangedCliCnfg(uint16_t conn_hdl);

/***************************************************************************//**
 * @brief     Write Service Changed characteristic Client Characteristic Configuration descriptor value to remote GATT database.
 * @param[in] conn_hdl Connection handle.
 * @param[in] p_value Service Changed characteristic Client Characteristic Configuration descriptor value to write.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_GATC_WriteServChangedCliCnfg(uint16_t conn_hdl, const uint16_t *p_value);

/***************************************************************************//**
 * @brief      Get Service Changed attribute handles.
 * @param[in]  p_addr Bluetooth device address for the attribute handles.
 * @param[out] p_hdl  The pointer to store the retrieved attribute handles.
 * @return    @ref ble_status_t
*******************************************************************************/
void R_BLE_GATC_GetServChangedAttrHdl(const st_ble_dev_addr_t *p_addr, st_ble_gatc_serv_changed_attr_hdl_t *p_hdl);


/*----------------------------------------------------------------------------------------------------------------------
    Generic Attribute Client
----------------------------------------------------------------------------------------------------------------------*/

/***************************************************************************//**
 * @brief Generic Attribute client event data.
*******************************************************************************/
typedef struct {
    uint16_t    conn_hdl;  /**< Connection handle */
    uint16_t    param_len; /**< Event parameter length */
    const void *p_param;   /**< Event parameter */
} st_ble_gatc_evt_data_t;

/***************************************************************************//**
 * @brief Generic Attribute characteristic ID.
*******************************************************************************/
typedef enum {
    BLE_GATC_SERV_CHANGED_IDX,
    BLE_GATC_SERV_CHANGED_CLI_CNFG_IDX,
} st_ble_gatc_char_idx_t;

/***************************************************************************//**
 * @brief Generic Attribute client event type.
*******************************************************************************/
typedef enum {
    /* Service Changed */
    BLE_GATC_EVENT_SERV_CHANGED_HDL_VAL_IND = BLE_SERVC_ATTR_EVENT(BLE_GATC_SERV_CHANGED_IDX, BLE_SERVC_HDL_VAL_IND),
    BLE_GATC_EVENT_SERV_CHANGED_CLI_CNFG_READ_RSP = BLE_SERVC_ATTR_EVENT(BLE_GATC_SERV_CHANGED_CLI_CNFG_IDX, BLE_SERVC_READ_RSP),
    BLE_GATC_EVENT_SERV_CHANGED_CLI_CNFG_WRITE_RSP = BLE_SERVC_ATTR_EVENT(BLE_GATC_SERV_CHANGED_CLI_CNFG_IDX, BLE_SERVC_WRITE_RSP),
} e_ble_gatc_event_t;

/***************************************************************************//**
 * @brief     Initialize Generic Attribute client.
 * @param[in] cb Client callback.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_GATC_Init(ble_servc_app_cb_t cb);

/***************************************************************************//**
 * @brief     Generic Attribute client discovery callback.
 * @param[in] conn_hdl Connection handle
 * @param[in] serv_idx Service instance index.
 * @param[in] type     Service discovery event type.
 * @param[in] p_param  Service discovery event parameter.
 * @return    @ref ble_status_t
*******************************************************************************/
void R_BLE_GATC_ServDiscCb(uint16_t conn_hdl, uint8_t serv_idx, uint16_t type, void *p_param);

/***************************************************************************//**
 * @brief     Get Generic Attribute client attribute handle.
 * @param[in]  p_addr Bluetooth device address for the attribute handles.
 * @param[out] p_hdl  The pointer to store the retrieved attribute handles.
*******************************************************************************/
void R_BLE_GATC_GetServAttrHdl(const st_ble_dev_addr_t *p_addr, st_ble_gatt_hdl_range_t *p_hdl);

#endif /* R_BLE_GATC_H */

/** @} */
