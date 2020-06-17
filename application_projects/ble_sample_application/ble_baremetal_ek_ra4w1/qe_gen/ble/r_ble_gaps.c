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
 * File Name: r_ble_gaps.c
 * Version : 1.0
 * Description : The source file for Generic Access service.
 **********************************************************************************************************************/
#include <string.h>
#include "r_ble_gaps.h"
#include "profile_cmn/r_ble_servs_if.h"
#include "gatt_db.h"

static st_ble_servs_info_t gs_servs_info;

/*----------------------------------------------------------------------------------------------------------------------
    Device Name characteristic
----------------------------------------------------------------------------------------------------------------------*/

static ble_status_t decode_st_ble_gaps_dev_name_t(st_ble_gaps_dev_name_t *p_app_value, const st_ble_gatt_value_t *p_gatt_value)
{
    if (BLE_GAPS_DEV_NAME_NAME_LEN < p_gatt_value->value_len)
    {
        return BLE_ERR_INVALID_DATA;
    }

    memset(p_app_value->name, 0x00, BLE_GAPS_DEV_NAME_NAME_LEN);

    strcpy(p_app_value->name, (char *)p_gatt_value->p_value);
    p_app_value->length = (uint8_t)p_gatt_value->value_len;

    return BLE_SUCCESS;
}

static ble_status_t encode_st_ble_gaps_dev_name_t(const st_ble_gaps_dev_name_t *p_app_value, st_ble_gatt_value_t *p_gatt_value)
{
    if (BLE_GAPS_DEV_NAME_NAME_LEN < p_app_value->length)
    {
        return BLE_ERR_INVALID_DATA;
    }

    strncpy((char *)p_gatt_value->p_value, p_app_value->name, p_app_value->length);

    return BLE_SUCCESS;
}

/* Device Name characteristic definition */
static const st_ble_servs_char_info_t gs_dev_name_char = {
    .start_hdl    = BLE_GAPS_DEV_NAME_DECL_HDL,
    .end_hdl      = BLE_GAPS_DEV_NAME_VAL_HDL,
    .char_idx     = BLE_GAPS_DEV_NAME_IDX,
    .app_size     = sizeof(st_ble_gaps_dev_name_t),
    .db_size      = BLE_GAPS_DEV_NAME_LEN,
    .decode       = (ble_servs_attr_decode_t)decode_st_ble_gaps_dev_name_t,
    .encode       = (ble_servs_attr_encode_t)encode_st_ble_gaps_dev_name_t,
};

ble_status_t R_BLE_GAPS_SetDevName(const st_ble_gaps_dev_name_t *p_value)
{
    return R_BLE_SERVS_SetChar(&gs_dev_name_char, BLE_GAP_INVALID_CONN_HDL, (const void *)p_value);
}

ble_status_t R_BLE_GAPS_GetDevName(st_ble_gaps_dev_name_t *p_value)
{
    return R_BLE_SERVS_GetChar(&gs_dev_name_char, BLE_GAP_INVALID_CONN_HDL, (void *)p_value);
}

/*----------------------------------------------------------------------------------------------------------------------
    Appearance characteristic
----------------------------------------------------------------------------------------------------------------------*/

/* Appearance characteristic definition */
static const st_ble_servs_char_info_t gs_appearance_char = {
    .start_hdl    = BLE_GAPS_APPEARANCE_DECL_HDL,
    .end_hdl      = BLE_GAPS_APPEARANCE_VAL_HDL,
    .char_idx     = BLE_GAPS_APPEARANCE_IDX,
    .app_size     = sizeof(uint16_t),
    .db_size      = BLE_GAPS_APPEARANCE_LEN,
    .decode       = (ble_servs_attr_decode_t)decode_uint16_t,
    .encode       = (ble_servs_attr_encode_t)encode_uint16_t,
};

ble_status_t R_BLE_GAPS_SetAppearance(const uint16_t *p_value)
{
    return R_BLE_SERVS_SetChar(&gs_appearance_char, BLE_GAP_INVALID_CONN_HDL, (const void *)p_value);
}

ble_status_t R_BLE_GAPS_GetAppearance(uint16_t *p_value)
{
    return R_BLE_SERVS_GetChar(&gs_appearance_char, BLE_GAP_INVALID_CONN_HDL, (void *)p_value);
}

/*----------------------------------------------------------------------------------------------------------------------
    Peripheral Preferred Connection Parameters characteristic
----------------------------------------------------------------------------------------------------------------------*/

static ble_status_t decode_st_ble_gaps_per_pref_conn_param_t(st_ble_gaps_per_pref_conn_param_t *p_app_value, const st_ble_gatt_value_t *p_gatt_value)
{
    uint32_t pos = 0;

    if (p_gatt_value->value_len < BLE_GAPS_PER_PREF_CONN_PARAM_LEN)
    {
        return BLE_ERR_INVALID_DATA;
    }

    BT_UNPACK_LE_2_BYTE(&p_app_value->min_conn_intv, &p_gatt_value->p_value[pos]);
    pos += 2;
    BT_UNPACK_LE_2_BYTE(&p_app_value->max_conn_intv, &p_gatt_value->p_value[pos]);
    pos += 2;
    BT_UNPACK_LE_2_BYTE(&p_app_value->slave_latency, &p_gatt_value->p_value[pos]);
    pos += 2;
    BT_UNPACK_LE_2_BYTE(&p_app_value->conn_sup_timeout_multiplier, &p_gatt_value->p_value[pos]);
    pos += 2;

    return BLE_SUCCESS;
}

static ble_status_t encode_st_ble_gaps_per_pref_conn_param_t(const st_ble_gaps_per_pref_conn_param_t *p_app_value, st_ble_gatt_value_t *p_gatt_value)
{
    uint32_t pos = 0;

    BT_PACK_LE_2_BYTE(&p_gatt_value->p_value[pos], &p_app_value->min_conn_intv);
    pos += 2;
    BT_PACK_LE_2_BYTE(&p_gatt_value->p_value[pos], &p_app_value->max_conn_intv);
    pos += 2;
    BT_PACK_LE_2_BYTE(&p_gatt_value->p_value[pos], &p_app_value->slave_latency);
    pos += 2;
    BT_PACK_LE_2_BYTE(&p_gatt_value->p_value[pos], &p_app_value->conn_sup_timeout_multiplier);
    pos += 2;

    p_gatt_value->value_len = (uint16_t)pos;

    return BLE_SUCCESS;
}

/* Peripheral Preferred Connection Parameters characteristic definition */
static const st_ble_servs_char_info_t gs_per_pref_conn_param_char = {
    .start_hdl    = BLE_GAPS_PER_PREF_CONN_PARAM_DECL_HDL,
    .end_hdl      = BLE_GAPS_PER_PREF_CONN_PARAM_VAL_HDL,
    .char_idx     = BLE_GAPS_PER_PREF_CONN_PARAM_IDX,
    .app_size     = sizeof(st_ble_gaps_per_pref_conn_param_t),
    .db_size      = BLE_GAPS_PER_PREF_CONN_PARAM_LEN,
    .decode       = (ble_servs_attr_decode_t)decode_st_ble_gaps_per_pref_conn_param_t,
    .encode       = (ble_servs_attr_encode_t)encode_st_ble_gaps_per_pref_conn_param_t,
};

ble_status_t R_BLE_GAPS_SetPerPrefConnParam(const st_ble_gaps_per_pref_conn_param_t *p_value)
{
    return R_BLE_SERVS_SetChar(&gs_per_pref_conn_param_char, BLE_GAP_INVALID_CONN_HDL, (const void *)p_value);
}

ble_status_t R_BLE_GAPS_GetPerPrefConnParam(st_ble_gaps_per_pref_conn_param_t *p_value)
{
    return R_BLE_SERVS_GetChar(&gs_per_pref_conn_param_char, BLE_GAP_INVALID_CONN_HDL, (void *)p_value);
}

/*----------------------------------------------------------------------------------------------------------------------
    Central Address Resolution characteristic
----------------------------------------------------------------------------------------------------------------------*/

/* Central Address Resolution characteristic definition */
static const st_ble_servs_char_info_t gs_cent_addr_rslv_char = {
    .start_hdl    = BLE_GAPS_CENT_ADDR_RSLV_DECL_HDL,
    .end_hdl      = BLE_GAPS_CENT_ADDR_RSLV_VAL_HDL,
    .char_idx     = BLE_GAPS_CENT_ADDR_RSLV_IDX,
    .app_size     = sizeof(uint8_t),
    .db_size      = BLE_GAPS_CENT_ADDR_RSLV_LEN,
    .decode       = (ble_servs_attr_decode_t)decode_uint8_t,
    .encode       = (ble_servs_attr_encode_t)encode_uint8_t,
};

ble_status_t R_BLE_GAPS_SetCentAddrRslv(const uint8_t *p_value)
{
    return R_BLE_SERVS_SetChar(&gs_cent_addr_rslv_char, BLE_GAP_INVALID_CONN_HDL, (const void *)p_value);
}

ble_status_t R_BLE_GAPS_GetCentAddrRslv(uint8_t *p_value)
{
    return R_BLE_SERVS_GetChar(&gs_cent_addr_rslv_char, BLE_GAP_INVALID_CONN_HDL, (void *)p_value);
}

/*----------------------------------------------------------------------------------------------------------------------
    Resolvable Private Address Only characteristic
----------------------------------------------------------------------------------------------------------------------*/

/* Resolvable Private Address Only characteristic definition */
static const st_ble_servs_char_info_t gs_rslv_priv_addr_only_char = {
    .start_hdl    = BLE_GAPS_RSLV_PRIV_ADDR_ONLY_DECL_HDL,
    .end_hdl      = BLE_GAPS_RSLV_PRIV_ADDR_ONLY_VAL_HDL,
    .char_idx     = BLE_GAPS_RSLV_PRIV_ADDR_ONLY_IDX,
    .app_size     = sizeof(uint8_t),
    .db_size      = BLE_GAPS_RSLV_PRIV_ADDR_ONLY_LEN,
    .decode       = (ble_servs_attr_decode_t)decode_uint8_t,
    .encode       = (ble_servs_attr_encode_t)encode_uint8_t,
};

ble_status_t R_BLE_GAPS_SetRslvPrivAddrOnly(const uint8_t *p_value)
{
    return R_BLE_SERVS_SetChar(&gs_rslv_priv_addr_only_char, BLE_GAP_INVALID_CONN_HDL, (const void *)p_value);
}

ble_status_t R_BLE_GAPS_GetRslvPrivAddrOnly(uint8_t *p_value)
{
    return R_BLE_SERVS_GetChar(&gs_rslv_priv_addr_only_char, BLE_GAP_INVALID_CONN_HDL, (void *)p_value);
}

/*----------------------------------------------------------------------------------------------------------------------
    Generic Access server
----------------------------------------------------------------------------------------------------------------------*/

/* Generic Access characteristics definition */
static const st_ble_servs_char_info_t *gspp_chars[] = {
    &gs_dev_name_char,
    &gs_appearance_char,
    &gs_per_pref_conn_param_char,
    &gs_cent_addr_rslv_char,
    &gs_rslv_priv_addr_only_char,
};

/* Generic Access service definition */
static st_ble_servs_info_t gs_servs_info = {
    .pp_chars     = gspp_chars,
    .num_of_chars = ARRAY_SIZE(gspp_chars),
};

ble_status_t R_BLE_GAPS_Init(ble_servs_app_cb_t cb)
{
    if (NULL == cb)
    {
        return BLE_ERR_INVALID_PTR;
    }

    gs_servs_info.cb = cb;

    return R_BLE_SERVS_RegisterServer(&gs_servs_info);
}
