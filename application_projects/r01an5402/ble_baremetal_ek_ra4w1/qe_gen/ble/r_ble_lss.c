/**********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name: r_ble_lss.c
 * Version : 1.0
 * Description : The source file for LED Switch service.
 **********************************************************************************************************************/

#include "r_ble_lss.h"
#include "profile_cmn/r_ble_servs_if.h"
#include "gatt_db.h"

static st_ble_servs_info_t gs_servs_info;

/*----------------------------------------------------------------------------------------------------------------------
    Switch State Client Characteristic Configuration descriptor
----------------------------------------------------------------------------------------------------------------------*/

static const st_ble_servs_desc_info_t gs_switch_state_cli_cnfg = {
    .attr_hdl = BLE_LSS_SWITCH_STATE_CLI_CNFG_DESC_HDL,
    .app_size = sizeof(uint16_t),
    .desc_idx = BLE_LSS_SWITCH_STATE_CLI_CNFG_IDX,
    .db_size  = BLE_LSS_SWITCH_STATE_CLI_CNFG_LEN,
    .decode   = (ble_servs_attr_decode_t)decode_uint16_t,
    .encode   = (ble_servs_attr_encode_t)encode_uint16_t,
};

ble_status_t R_BLE_LSS_SetSwitchStateCliCnfg(uint16_t conn_hdl, const uint16_t *p_value)
{
    return R_BLE_SERVS_SetDesc(&gs_switch_state_cli_cnfg, conn_hdl, (const void *)p_value);
}

ble_status_t R_BLE_LSS_GetSwitchStateCliCnfg(uint16_t conn_hdl, uint16_t *p_value)
{
    return R_BLE_SERVS_GetDesc(&gs_switch_state_cli_cnfg, conn_hdl, (void *)p_value);
}

/*----------------------------------------------------------------------------------------------------------------------
    Switch State characteristic
----------------------------------------------------------------------------------------------------------------------*/

/* Switch State characteristic descriptor definition */
static const st_ble_servs_desc_info_t *gspp_switch_state_descs[] = {
    &gs_switch_state_cli_cnfg,
};

/* Switch State characteristic definition */
static const st_ble_servs_char_info_t gs_switch_state_char = {
    .start_hdl    = BLE_LSS_SWITCH_STATE_DECL_HDL,
    .end_hdl      = BLE_LSS_SWITCH_STATE_CLI_CNFG_DESC_HDL,
    .char_idx     = BLE_LSS_SWITCH_STATE_IDX,
    .app_size     = sizeof(uint8_t),
    .db_size      = BLE_LSS_SWITCH_STATE_LEN,
    .decode       = (ble_servs_attr_decode_t)decode_uint8_t,
    .encode       = (ble_servs_attr_encode_t)encode_uint8_t,
    .pp_descs     = gspp_switch_state_descs,
    .num_of_descs = ARRAY_SIZE(gspp_switch_state_descs),
};

ble_status_t R_BLE_LSS_NotifySwitchState(uint16_t conn_hdl, const uint8_t *p_value)
{
    return R_BLE_SERVS_SendHdlVal(&gs_switch_state_char, conn_hdl, (const void *)p_value, true);
}

/*----------------------------------------------------------------------------------------------------------------------
    LED Blink Rate characteristic
----------------------------------------------------------------------------------------------------------------------*/

/* LED Blink Rate characteristic definition */
static const st_ble_servs_char_info_t gs_blink_rate_char = {
    .start_hdl    = BLE_LSS_BLINK_RATE_DECL_HDL,
    .end_hdl      = BLE_LSS_BLINK_RATE_VAL_HDL,
    .char_idx     = BLE_LSS_BLINK_RATE_IDX,
    .app_size     = sizeof(uint8_t),
    .db_size      = BLE_LSS_BLINK_RATE_LEN,
    .decode       = (ble_servs_attr_decode_t)decode_uint8_t,
    .encode       = (ble_servs_attr_encode_t)encode_uint8_t,
};

ble_status_t R_BLE_LSS_SetBlinkRate(const uint8_t *p_value)
{
    return R_BLE_SERVS_SetChar(&gs_blink_rate_char, BLE_GAP_INVALID_CONN_HDL, (const void *)p_value);
}

ble_status_t R_BLE_LSS_GetBlinkRate(uint8_t *p_value)
{
    return R_BLE_SERVS_GetChar(&gs_blink_rate_char, BLE_GAP_INVALID_CONN_HDL, (void *)p_value);
}

/*----------------------------------------------------------------------------------------------------------------------
    LED Switch server
----------------------------------------------------------------------------------------------------------------------*/

/* LED Switch characteristics definition */
static const st_ble_servs_char_info_t *gspp_chars[] = {
    &gs_switch_state_char,
    &gs_blink_rate_char,
};

/* LED Switch service definition */
static st_ble_servs_info_t gs_servs_info = {
    .pp_chars     = gspp_chars,
    .num_of_chars = ARRAY_SIZE(gspp_chars),
};

ble_status_t R_BLE_LSS_Init(ble_servs_app_cb_t cb)
{
    if (NULL == cb)
    {
        return BLE_ERR_INVALID_PTR;
    }

    gs_servs_info.cb = cb;

    return R_BLE_SERVS_RegisterServer(&gs_servs_info);
}
