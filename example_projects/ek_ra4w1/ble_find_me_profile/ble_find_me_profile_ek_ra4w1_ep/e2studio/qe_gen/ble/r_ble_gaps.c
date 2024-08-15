/***********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/***********************************************************************************************************************
 * File Name: r_ble_gaps.c
 * Description : The source file for GAP Service Server.
 **********************************************************************************************************************/

#include "r_ble_gaps.h"
#include "profile_cmn/r_ble_servs_if.h"
#include "gatt_db.h"

static st_ble_servs_info_t gs_servs_info;

/* Start user code for function prototype declarations and global variables. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */



/*----------------------------------------------------------------------------------------------------------------------
    Device Name characteristic : The Device Name characteristic shall contain the name of the device.
----------------------------------------------------------------------------------------------------------------------*/

static ble_status_t decode_st_ble_gaps_dev_name_t(st_ble_gaps_dev_name_t *p_app_value, const st_ble_gatt_value_t *p_gatt_value)
{
    /* Start user code for Device Name characteristic value decode function. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
#ifndef BLE_GAPS_DISABLE_ENCODE_DECODE


    uint32_t pos = 0;

    for (uint32_t i=0;i<128;i++)
    {
        BT_UNPACK_LE_1_BYTE(&p_app_value->name[i],&p_gatt_value->p_value[pos]);
        pos += 1;
    }

    BT_UNPACK_LE_1_BYTE(&p_app_value->length,&p_gatt_value->p_value[pos]);
    pos += 1;
#endif /* BLE_GAPS_DISABLE_ENCODE_DECODE */


    return BLE_SUCCESS;
}
static ble_status_t encode_st_ble_gaps_dev_name_t(const st_ble_gaps_dev_name_t *p_app_value, st_ble_gatt_value_t *p_gatt_value)
{
    /* Start user code for Device Name characteristic value encode function. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
#ifndef BLE_GAPS_DISABLE_ENCODE_DECODE

    uint32_t pos = 0;

    for (uint32_t i=0;i<128;i++)
    {
        BT_PACK_LE_1_BYTE(&p_gatt_value->p_value[pos],&p_app_value->name[i]);
        pos += 1;
    }

    BT_PACK_LE_1_BYTE(&p_gatt_value->p_value[pos],&p_app_value->length);
    pos += 1;
    
    p_gatt_value->value_len = (uint16_t)pos;
#endif /* BLE_GAPS_DISABLE_ENCODE_DECODE */


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
    Appearance characteristic : The Appearance characteristic defines the representation of the external appearance of the device.
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
    Peripheral Preferred Connection Parameters characteristic : The Peripheral Preferred Connection Parameters (PPCP) characteristic contains the preferred connection parameters of the Peripheral
----------------------------------------------------------------------------------------------------------------------*/

static ble_status_t decode_st_ble_gaps_per_pref_conn_param_t(st_ble_gaps_per_pref_conn_param_t *p_app_value, const st_ble_gatt_value_t *p_gatt_value)
{
    /* Start user code for Peripheral Preferred Connection Parameters characteristic value decode function. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
#ifndef BLE_GAPS_DISABLE_ENCODE_DECODE


    uint32_t pos = 0;

    BT_UNPACK_LE_2_BYTE(&p_app_value->minimum_connection_interval,&p_gatt_value->p_value[pos]);
    pos += 2;

    BT_UNPACK_LE_2_BYTE(&p_app_value->maximum_connection_interval,&p_gatt_value->p_value[pos]);
    pos += 2;

    BT_UNPACK_LE_2_BYTE(&p_app_value->slave_latency,&p_gatt_value->p_value[pos]);
    pos += 2;

    BT_UNPACK_LE_2_BYTE(&p_app_value->connection_supervision_timeout_multiplier,&p_gatt_value->p_value[pos]);
    pos += 2;
#endif /* BLE_GAPS_DISABLE_ENCODE_DECODE */


    return BLE_SUCCESS;
}
static ble_status_t encode_st_ble_gaps_per_pref_conn_param_t(const st_ble_gaps_per_pref_conn_param_t *p_app_value, st_ble_gatt_value_t *p_gatt_value)
{
    /* Start user code for Peripheral Preferred Connection Parameters characteristic value encode function. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
#ifndef BLE_GAPS_DISABLE_ENCODE_DECODE

    uint32_t pos = 0;

    BT_PACK_LE_2_BYTE(&p_gatt_value->p_value[pos],&p_app_value->minimum_connection_interval);
    pos += 2;

    BT_PACK_LE_2_BYTE(&p_gatt_value->p_value[pos],&p_app_value->maximum_connection_interval);
    pos += 2;

    BT_PACK_LE_2_BYTE(&p_gatt_value->p_value[pos],&p_app_value->slave_latency);
    pos += 2;

    BT_PACK_LE_2_BYTE(&p_gatt_value->p_value[pos],&p_app_value->connection_supervision_timeout_multiplier);
    pos += 2;
    
    p_gatt_value->value_len = (uint16_t)pos;
#endif /* BLE_GAPS_DISABLE_ENCODE_DECODE */


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
    Central Address Resolution characteristic : The Peripheral should check if the peer device supports address resolution by reading the Central Address Resolution characteristic.
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
    Resolvable Private Address Only characteristic : The device should check if the peer will only use Resolvable Private Addresses (RPAs) after bonding by reading the Resolvable Private Address Only characteristic.
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
    GAP Service Server
----------------------------------------------------------------------------------------------------------------------*/

/* GAP Service characteristics definition */
static const st_ble_servs_char_info_t *gspp_chars[] = {
    &gs_dev_name_char,
    &gs_appearance_char,
    &gs_per_pref_conn_param_char,
    &gs_cent_addr_rslv_char,
    &gs_rslv_priv_addr_only_char,
};

/* GAP Service server definition */
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





/* Start user code for function definitions. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */