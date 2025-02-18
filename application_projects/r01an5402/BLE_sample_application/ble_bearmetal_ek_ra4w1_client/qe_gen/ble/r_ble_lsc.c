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
 * File Name: r_ble_lsc.c
 * Description : The source file for LED Switch client.
 **********************************************************************************************************************/
#include "r_ble_lsc.h"
#include "profile_cmn/r_ble_servc_if.h"

static st_ble_servc_info_t gs_client_info;

/*----------------------------------------------------------------------------------------------------------------------
    Switch State Client Characteristic Configuration descriptor
----------------------------------------------------------------------------------------------------------------------*/

/* Switch State characteristic descriptors attribute handles */
static uint16_t gs_switch_state_cli_cnfg_desc_hdls[BLE_SERVC_MAX_NUM_OF_SAVED];

static const st_ble_servc_desc_info_t gs_switch_state_cli_cnfg ={
    .uuid_16     = BLE_LSC_SWITCH_STATE_CLI_CNFG_UUID,
    .uuid_type   = BLE_GATT_16_BIT_UUID_FORMAT,
    .app_size    = sizeof(uint16_t),
    .db_size     = BLE_LSC_SWITCH_STATE_CLI_CNFG_LEN,
    .desc_idx    = BLE_LSC_SWITCH_STATE_CLI_CNFG_IDX,
    .p_attr_hdls = gs_switch_state_cli_cnfg_desc_hdls,
    .decode      = (ble_servc_attr_decode_t)decode_uint16_t,
    .encode      = (ble_servc_attr_encode_t)encode_uint16_t,
};

ble_status_t R_BLE_LSC_WriteSwitchStateCliCnfg(uint16_t conn_hdl, const uint16_t *p_value) // @suppress("API function naming")
{
    return R_BLE_SERVC_WriteDesc(&gs_switch_state_cli_cnfg, conn_hdl, p_value);
}

ble_status_t R_BLE_LSC_ReadSwitchStateCliCnfg(uint16_t conn_hdl) // @suppress("API function naming")
{
    return R_BLE_SERVC_ReadDesc(&gs_switch_state_cli_cnfg, conn_hdl);
}

/*----------------------------------------------------------------------------------------------------------------------
    Switch State Characteristic
----------------------------------------------------------------------------------------------------------------------*/


/* Switch State characteristic 128 bit UUID */
const uint8_t BLE_LSC_SWITCH_STATE_UUID[BLE_GATT_128_BIT_UUID_SIZE] = { 0xE0, 0xFC, 0x8E, 0x8E, 0x96, 0xB4, 0x01, 0xAB, 0x67, 0x42, 0x05, 0x5F, 0x57, 0x7F, 0x83, 0x58 };

/* Switch State characteristic attribute handles */
static st_ble_gatt_hdl_range_t gs_switch_state_char_ranges[BLE_SERVC_MAX_NUM_OF_SAVED];

/* Switch State characteristic descriptor definition */
static const st_ble_servc_desc_info_t *gspp_switch_state_descs[] = {
    &gs_switch_state_cli_cnfg,
};

/* Switch State characteristic definition */
const st_ble_servc_char_info_t gs_switch_state_char = {
    .uuid_128     = BLE_LSC_SWITCH_STATE_UUID,
    .uuid_type    = BLE_GATT_128_BIT_UUID_FORMAT,
    .app_size     = sizeof(uint8_t),
    .db_size      = BLE_LSC_SWITCH_STATE_LEN,
    .char_idx     = BLE_LSC_SWITCH_STATE_IDX,
    .p_attr_hdls  = gs_switch_state_char_ranges,
    .decode       = (ble_servc_attr_decode_t)decode_uint8_t,
    .encode       = (ble_servc_attr_encode_t)encode_uint8_t,
    .num_of_descs = ARRAY_SIZE(gspp_switch_state_descs),
    .pp_descs     = gspp_switch_state_descs,
};

void R_BLE_LSC_GetSwitchStateAttrHdl(const st_ble_dev_addr_t *p_addr, st_ble_lsc_switch_state_attr_hdl_t *p_hdl)
{
    uint8_t conn_idx;

    conn_idx = R_BLE_SERVC_GetConnIdx(p_addr);
    if (BLE_SERVC_MAX_NUM_OF_SAVED > conn_idx)
    {
        p_hdl->range = gs_switch_state_char_ranges[conn_idx];
        p_hdl->cli_cnfg_desc_hdl = gs_switch_state_cli_cnfg_desc_hdls[conn_idx];
    }
}

/*----------------------------------------------------------------------------------------------------------------------
    LED Blink Rate Characteristic
----------------------------------------------------------------------------------------------------------------------*/


/* LED Blink Rate characteristic 128 bit UUID */
const uint8_t BLE_LSC_BLINK_RATE_UUID[BLE_GATT_128_BIT_UUID_SIZE] = { 0xE0, 0xFC, 0x8E, 0x8E, 0x96, 0xB4, 0x01, 0xAB, 0x67, 0x42, 0x05, 0x5F, 0x2F, 0xC3, 0x83, 0x58 };

/* LED Blink Rate characteristic attribute handles */
static st_ble_gatt_hdl_range_t gs_blink_rate_char_ranges[BLE_SERVC_MAX_NUM_OF_SAVED];

/* LED Blink Rate characteristic definition */
const st_ble_servc_char_info_t gs_blink_rate_char = {
    .uuid_128     = BLE_LSC_BLINK_RATE_UUID,
    .uuid_type    = BLE_GATT_128_BIT_UUID_FORMAT,
    .app_size     = sizeof(uint8_t),
    .db_size      = BLE_LSC_BLINK_RATE_LEN,
    .char_idx     = BLE_LSC_BLINK_RATE_IDX,
    .p_attr_hdls  = gs_blink_rate_char_ranges,
    .decode       = (ble_servc_attr_decode_t)decode_uint8_t,
    .encode       = (ble_servc_attr_encode_t)encode_uint8_t,
};

ble_status_t R_BLE_LSC_WriteBlinkRate(uint16_t conn_hdl, const uint8_t *p_value) // @suppress("API function naming")
{
    return R_BLE_SERVC_WriteChar(&gs_blink_rate_char, conn_hdl, p_value);
}

ble_status_t R_BLE_LSC_ReadBlinkRate(uint16_t conn_hdl) // @suppress("API function naming")
{
    return R_BLE_SERVC_ReadChar(&gs_blink_rate_char, conn_hdl);
}

void R_BLE_LSC_GetBlinkRateAttrHdl(const st_ble_dev_addr_t *p_addr, st_ble_lsc_blink_rate_attr_hdl_t *p_hdl)
{
    uint8_t conn_idx;

    conn_idx = R_BLE_SERVC_GetConnIdx(p_addr);
    if (BLE_SERVC_MAX_NUM_OF_SAVED > conn_idx)
    {
        p_hdl->range = gs_blink_rate_char_ranges[conn_idx];
    }
}


/*----------------------------------------------------------------------------------------------------------------------
    LED Switch client
----------------------------------------------------------------------------------------------------------------------*/

/* LED Switch client attribute handles */
static st_ble_gatt_hdl_range_t gs_lsc_ranges[BLE_SERVC_MAX_NUM_OF_SAVED];

const st_ble_servc_char_info_t *gspp_lsc_chars[] = {
    &gs_switch_state_char,
    &gs_blink_rate_char,
};

static st_ble_servc_info_t gs_client_info = {
    .pp_chars     = gspp_lsc_chars,
    .num_of_chars = ARRAY_SIZE(gspp_lsc_chars),
    .p_attr_hdls  = gs_lsc_ranges,
};

ble_status_t R_BLE_LSC_Init(ble_servc_app_cb_t cb) // @suppress("API function naming")
{
    if (NULL == cb)
    {
        return BLE_ERR_INVALID_PTR;
    }

    gs_client_info.cb = cb;

    return R_BLE_SERVC_RegisterClient(&gs_client_info);
}

void R_BLE_LSC_ServDiscCb(uint16_t conn_hdl, uint8_t serv_idx, uint16_t type, void *p_param) // @suppress("API function naming")
{
    R_BLE_SERVC_ServDiscCb(&gs_client_info, conn_hdl, serv_idx, type, p_param);
}

void R_BLE_LSC_GetServAttrHdl(const st_ble_dev_addr_t *p_addr, st_ble_gatt_hdl_range_t *p_hdl)
{
    uint8_t conn_idx;

    conn_idx = R_BLE_SERVC_GetConnIdx(p_addr);
    if (BLE_SERVC_MAX_NUM_OF_SAVED > conn_idx)
    {
        *p_hdl = gs_lsc_ranges[conn_idx];
    }
}
