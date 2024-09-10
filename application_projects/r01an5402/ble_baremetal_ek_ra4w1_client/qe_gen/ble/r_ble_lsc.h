/**********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name: r_ble_lsc.h
 * Version : 1.0
 * Description : The header file for LED Switch client.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 31.12.2999 1.00 First Release
 ***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @file
 * @defgroup lsc LED Switch Service Client
 * @{
 * @ingroup profile
 * @brief   This is the client for the LED Switch Service.
 **********************************************************************************************************************/

#include "profile_cmn/r_ble_servc_if.h"

#ifndef R_BLE_LSC_H
#define R_BLE_LSC_H

/*----------------------------------------------------------------------------------------------------------------------
    Switch State Characteristic
----------------------------------------------------------------------------------------------------------------------*/
extern const uint8_t BLE_LSC_SWITCH_STATE_UUID[BLE_GATT_128_BIT_UUID_SIZE];
#define BLE_LSC_SWITCH_STATE_LEN (1)
#define BLE_LSC_SWITCH_STATE_CLI_CNFG_UUID (0x2902)
#define BLE_LSC_SWITCH_STATE_CLI_CNFG_LEN (2)

/***************************************************************************//**
 * @brief Switch State attribute handle value.
*******************************************************************************/
typedef struct {
    st_ble_gatt_hdl_range_t range;
    uint16_t cli_cnfg_desc_hdl;
} st_ble_lsc_switch_state_attr_hdl_t;

/***************************************************************************//**
 * @brief     Read Switch State characteristic Client Characteristic Configuration descriptor value from the remote GATT database.
 * @param[in] conn_hdl Connection handle.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_LSC_ReadSwitchStateCliCnfg(uint16_t conn_hdl);

/***************************************************************************//**
 * @brief     Write Switch State characteristic Client Characteristic Configuration descriptor value to remote GATT database.
 * @param[in] conn_hdl Connection handle.
 * @param[in] p_value Switch State characteristic Client Characteristic Configuration descriptor value to write.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_LSC_WriteSwitchStateCliCnfg(uint16_t conn_hdl, const uint16_t *p_value);

/***************************************************************************//**
 * @brief      Get Switch State attribute handles.
 * @param[in]  p_addr Bluetooth device address for the attribute handles.
 * @param[out] p_hdl  The pointer to store the retrieved attribute handles.
 * @return    @ref ble_status_t
*******************************************************************************/
void R_BLE_LSC_GetSwitchStateAttrHdl(const st_ble_dev_addr_t *p_addr, st_ble_lsc_switch_state_attr_hdl_t *p_hdl);

/*----------------------------------------------------------------------------------------------------------------------
    LED Blink Rate Characteristic
----------------------------------------------------------------------------------------------------------------------*/
extern const uint8_t BLE_LSC_BLINK_RATE_UUID[BLE_GATT_128_BIT_UUID_SIZE];
#define BLE_LSC_BLINK_RATE_LEN (1)
/***************************************************************************//**
 * @brief LED Blink Rate attribute handle value.
*******************************************************************************/
typedef struct {
    st_ble_gatt_hdl_range_t range;
} st_ble_lsc_blink_rate_attr_hdl_t;

/***************************************************************************//**
 * @brief     Read LED Blink Rate characteristic value from the remote GATT database.
 * @param[in] conn_hdl  Connection handle.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_LSC_ReadBlinkRate(uint16_t conn_hdl);

/***************************************************************************//**
 * @brief     Write LED Blink Rate characteristic value to remote GATT database.
 * @param[in] conn_hdl  Connection handle.
 * @param[in] p_value LED Blink Rate characteristic value to write.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_LSC_WriteBlinkRate(uint16_t conn_hdl, const uint8_t *p_value);
;
/***************************************************************************//**
 * @brief      Get LED Blink Rate attribute handles.
 * @param[in]  p_addr Bluetooth device address for the attribute handles.
 * @param[out] p_hdl  The pointer to store the retrieved attribute handles.
 * @return    @ref ble_status_t
*******************************************************************************/
void R_BLE_LSC_GetBlinkRateAttrHdl(const st_ble_dev_addr_t *p_addr, st_ble_lsc_blink_rate_attr_hdl_t *p_hdl);


/*----------------------------------------------------------------------------------------------------------------------
    LED Switch Client
----------------------------------------------------------------------------------------------------------------------*/

/***************************************************************************//**
 * @brief LED Switch client event data.
*******************************************************************************/
typedef struct {
    uint16_t    conn_hdl;  /**< Connection handle */
    uint16_t    param_len; /**< Event parameter length */
    const void *p_param;   /**< Event parameter */
} st_ble_lsc_evt_data_t;

/***************************************************************************//**
 * @brief LED Switch characteristic ID.
*******************************************************************************/
typedef enum {
    BLE_LSC_SWITCH_STATE_IDX,
    BLE_LSC_SWITCH_STATE_CLI_CNFG_IDX,
    BLE_LSC_BLINK_RATE_IDX,
} e_ble_lsc_char_idx_t;

/***************************************************************************//**
 * @brief LED Switch client event type.
*******************************************************************************/
typedef enum {
    /* Switch State */
    BLE_LSC_EVENT_SWITCH_STATE_HDL_VAL_NTF = BLE_SERVC_ATTR_EVENT(BLE_LSC_SWITCH_STATE_IDX, BLE_SERVC_HDL_VAL_NTF),
    BLE_LSC_EVENT_SWITCH_STATE_CLI_CNFG_READ_RSP = BLE_SERVC_ATTR_EVENT(BLE_LSC_SWITCH_STATE_CLI_CNFG_IDX, BLE_SERVC_READ_RSP),
    BLE_LSC_EVENT_SWITCH_STATE_CLI_CNFG_WRITE_RSP = BLE_SERVC_ATTR_EVENT(BLE_LSC_SWITCH_STATE_CLI_CNFG_IDX, BLE_SERVC_WRITE_RSP),
    /* LED Blink Rate */
    BLE_LSC_EVENT_BLINK_RATE_READ_RSP = BLE_SERVC_ATTR_EVENT(BLE_LSC_BLINK_RATE_IDX, BLE_SERVC_READ_RSP),
    BLE_LSC_EVENT_BLINK_RATE_WRITE_RSP = BLE_SERVC_ATTR_EVENT(BLE_LSC_BLINK_RATE_IDX, BLE_SERVC_WRITE_RSP),
} e_ble_lsc_event_t;

/***************************************************************************//**
 * @brief     Initialize LED Switch client.
 * @param[in] cb Client callback.
 * @return    @ref ble_status_t
*******************************************************************************/
ble_status_t R_BLE_LSC_Init(ble_servc_app_cb_t cb);

/***************************************************************************//**
 * @brief     LED Switch client discovery callback.
 * @param[in] conn_hdl Connection handle
 * @param[in] serv_idx Service instance index.
 * @param[in] type     Service discovery event type.
 * @param[in] p_param  Service discovery event parameter.
 * @return    @ref ble_status_t
*******************************************************************************/
void R_BLE_LSC_ServDiscCb(uint16_t conn_hdl, uint8_t serv_idx, uint16_t type, void *p_param);

/***************************************************************************//**
 * @brief     Get LED Switch client attribute handle.
 * @param[in]  p_addr Bluetooth device address for the attribute handles.
 * @param[out] p_hdl  The pointer to store the retrieved attribute handles.
*******************************************************************************/
void R_BLE_LSC_GetServAttrHdl(const st_ble_dev_addr_t *p_addr, st_ble_gatt_hdl_range_t *p_hdl);

#endif /* R_BLE_LSC_H */

/** @} */
