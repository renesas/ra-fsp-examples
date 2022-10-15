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
 * File Name: r_ble_gapc.c
 * Description : The source file for GAP Service client.
 **********************************************************************************************************************/
#include "r_ble_gapc.h"
#include "profile_cmn/r_ble_servc_if.h"

static st_ble_servc_info_t gs_client_info;

/* Start user code for function prototype declarations and global variables. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/*----------------------------------------------------------------------------------------------------------------------
    Device Name Characteristic : The Device Name characteristic shall contain the name of the device.
----------------------------------------------------------------------------------------------------------------------*/


/* Device Name characteristic attribute handles */
static st_ble_gatt_hdl_range_t gs_dev_name_char_ranges[BLE_SERVC_MAX_NUM_OF_SAVED];

static ble_status_t decode_st_ble_gapc_dev_name_t(st_ble_gapc_dev_name_t *p_app_value, const st_ble_gatt_value_t *p_gatt_value)
{
    /* Start user code for Device Name characteristic value decode function. Do not edit comment generated here */
    FSP_PARAMETER_NOT_USED (*p_app_value);
    FSP_PARAMETER_NOT_USED (*p_gatt_value);
    /* End user code. Do not edit comment generated here */
    return BLE_SUCCESS;
}

static ble_status_t encode_st_ble_gapc_dev_name_t(const st_ble_gapc_dev_name_t *p_app_value, st_ble_gatt_value_t *p_gatt_value)
{
    /* Start user code for Device Name characteristic value encode function. Do not edit comment generated here */
    FSP_PARAMETER_NOT_USED (*p_app_value);
    FSP_PARAMETER_NOT_USED (*p_gatt_value);
    /* End user code. Do not edit comment generated here */
    return BLE_SUCCESS;
}

/* Device Name characteristic definition */
static const st_ble_servc_char_info_t gs_dev_name_char = {
    .uuid_16      = BLE_GAPC_DEV_NAME_UUID,
    .uuid_type    = BLE_GATT_16_BIT_UUID_FORMAT,
    .app_size     = sizeof(st_ble_gapc_dev_name_t),
    .db_size      = BLE_GAPC_DEV_NAME_LEN,
    .char_idx     = BLE_GAPC_DEV_NAME_IDX,
    .p_attr_hdls  = gs_dev_name_char_ranges,
    .decode       = (ble_servc_attr_decode_t)decode_st_ble_gapc_dev_name_t,
    .encode       = (ble_servc_attr_encode_t)encode_st_ble_gapc_dev_name_t,
};

ble_status_t R_BLE_GAPC_WriteDevName(uint16_t conn_hdl, const st_ble_gapc_dev_name_t *p_value) // @suppress("API function naming")
{
    return R_BLE_SERVC_WriteChar(&gs_dev_name_char, conn_hdl, p_value);
}

ble_status_t R_BLE_GAPC_ReadDevName(uint16_t conn_hdl) // @suppress("API function naming")
{
    return R_BLE_SERVC_ReadChar(&gs_dev_name_char, conn_hdl);
}

void R_BLE_GAPC_GetDevNameAttrHdl(const st_ble_dev_addr_t *p_addr, st_ble_gapc_dev_name_attr_hdl_t *p_hdl)
{
    uint8_t conn_idx;

    conn_idx = R_BLE_SERVC_GetConnIdx(p_addr);
    if (BLE_SERVC_MAX_NUM_OF_SAVED > conn_idx)
    {
        p_hdl->range = gs_dev_name_char_ranges[conn_idx];
    }
}

/*----------------------------------------------------------------------------------------------------------------------
    Appearance Characteristic : The Appearance characteristic defines the representation of the external appearance of the device.
----------------------------------------------------------------------------------------------------------------------*/


/* Appearance characteristic attribute handles */
static st_ble_gatt_hdl_range_t gs_appearance_char_ranges[BLE_SERVC_MAX_NUM_OF_SAVED];

/* Appearance characteristic definition */
static const st_ble_servc_char_info_t gs_appearance_char = {
    .uuid_16      = BLE_GAPC_APPEARANCE_UUID,
    .uuid_type    = BLE_GATT_16_BIT_UUID_FORMAT,
    .app_size     = sizeof(uint16_t),
    .db_size      = BLE_GAPC_APPEARANCE_LEN,
    .char_idx     = BLE_GAPC_APPEARANCE_IDX,
    .p_attr_hdls  = gs_appearance_char_ranges,
    .decode       = (ble_servc_attr_decode_t)decode_uint16_t,
    .encode       = (ble_servc_attr_encode_t)encode_uint16_t,
};

ble_status_t R_BLE_GAPC_ReadAppearance(uint16_t conn_hdl) // @suppress("API function naming")
{
    return R_BLE_SERVC_ReadChar(&gs_appearance_char, conn_hdl);
}

void R_BLE_GAPC_GetAppearanceAttrHdl(const st_ble_dev_addr_t *p_addr, st_ble_gapc_appearance_attr_hdl_t *p_hdl)
{
    uint8_t conn_idx;

    conn_idx = R_BLE_SERVC_GetConnIdx(p_addr);
    if (BLE_SERVC_MAX_NUM_OF_SAVED > conn_idx)
    {
        p_hdl->range = gs_appearance_char_ranges[conn_idx];
    }
}

/*----------------------------------------------------------------------------------------------------------------------
    Peripheral Preferred Connection Parameters Characteristic : The Peripheral Preferred Connection Parameters (PPCP) characteristic contains the preferred connection parameters of the Peripheral
----------------------------------------------------------------------------------------------------------------------*/


/* Peripheral Preferred Connection Parameters characteristic attribute handles */
static st_ble_gatt_hdl_range_t gs_per_pref_conn_param_char_ranges[BLE_SERVC_MAX_NUM_OF_SAVED];

static ble_status_t decode_st_ble_gapc_per_pref_conn_param_t(st_ble_gapc_per_pref_conn_param_t *p_app_value, const st_ble_gatt_value_t *p_gatt_value)
{
    /* Start user code for Peripheral Preferred Connection Parameters characteristic value decode function. Do not edit comment generated here */
    FSP_PARAMETER_NOT_USED (*p_app_value);
    FSP_PARAMETER_NOT_USED (*p_gatt_value);
    /* End user code. Do not edit comment generated here */
    return BLE_SUCCESS;
}

static ble_status_t encode_st_ble_gapc_per_pref_conn_param_t(const st_ble_gapc_per_pref_conn_param_t *p_app_value, st_ble_gatt_value_t *p_gatt_value)
{
    /* Start user code for Peripheral Preferred Connection Parameters characteristic value encode function. Do not edit comment generated here */
    FSP_PARAMETER_NOT_USED (*p_app_value);
    FSP_PARAMETER_NOT_USED (*p_gatt_value);
    /* End user code. Do not edit comment generated here */
    return BLE_SUCCESS;
}

/* Peripheral Preferred Connection Parameters characteristic definition */
static const st_ble_servc_char_info_t gs_per_pref_conn_param_char = {
    .uuid_16      = BLE_GAPC_PER_PREF_CONN_PARAM_UUID,
    .uuid_type    = BLE_GATT_16_BIT_UUID_FORMAT,
    .app_size     = sizeof(st_ble_gapc_per_pref_conn_param_t),
    .db_size      = BLE_GAPC_PER_PREF_CONN_PARAM_LEN,
    .char_idx     = BLE_GAPC_PER_PREF_CONN_PARAM_IDX,
    .p_attr_hdls  = gs_per_pref_conn_param_char_ranges,
    .decode       = (ble_servc_attr_decode_t)decode_st_ble_gapc_per_pref_conn_param_t,
    .encode       = (ble_servc_attr_encode_t)encode_st_ble_gapc_per_pref_conn_param_t,
};

ble_status_t R_BLE_GAPC_ReadPerPrefConnParam(uint16_t conn_hdl) // @suppress("API function naming")
{
    return R_BLE_SERVC_ReadChar(&gs_per_pref_conn_param_char, conn_hdl);
}

void R_BLE_GAPC_GetPerPrefConnParamAttrHdl(const st_ble_dev_addr_t *p_addr, st_ble_gapc_per_pref_conn_param_attr_hdl_t *p_hdl)
{
    uint8_t conn_idx;

    conn_idx = R_BLE_SERVC_GetConnIdx(p_addr);
    if (BLE_SERVC_MAX_NUM_OF_SAVED > conn_idx)
    {
        p_hdl->range = gs_per_pref_conn_param_char_ranges[conn_idx];
    }
}

/*----------------------------------------------------------------------------------------------------------------------
    Central Address Resolution Characteristic : The Peripheral should check if the peer device supports address resolution by reading the Central Address Resolution characteristic.
----------------------------------------------------------------------------------------------------------------------*/


/* Central Address Resolution characteristic attribute handles */
static st_ble_gatt_hdl_range_t gs_cent_addr_rslv_char_ranges[BLE_SERVC_MAX_NUM_OF_SAVED];

/* Central Address Resolution characteristic definition */
static const st_ble_servc_char_info_t gs_cent_addr_rslv_char = {
    .uuid_16      = BLE_GAPC_CENT_ADDR_RSLV_UUID,
    .uuid_type    = BLE_GATT_16_BIT_UUID_FORMAT,
    .app_size     = sizeof(uint8_t),
    .db_size      = BLE_GAPC_CENT_ADDR_RSLV_LEN,
    .char_idx     = BLE_GAPC_CENT_ADDR_RSLV_IDX,
    .p_attr_hdls  = gs_cent_addr_rslv_char_ranges,
    .decode       = (ble_servc_attr_decode_t)decode_uint8_t,
    .encode       = (ble_servc_attr_encode_t)encode_uint8_t,
};

ble_status_t R_BLE_GAPC_ReadCentAddrRslv(uint16_t conn_hdl) // @suppress("API function naming")
{
    return R_BLE_SERVC_ReadChar(&gs_cent_addr_rslv_char, conn_hdl);
}

void R_BLE_GAPC_GetCentAddrRslvAttrHdl(const st_ble_dev_addr_t *p_addr, st_ble_gapc_cent_addr_rslv_attr_hdl_t *p_hdl)
{
    uint8_t conn_idx;

    conn_idx = R_BLE_SERVC_GetConnIdx(p_addr);
    if (BLE_SERVC_MAX_NUM_OF_SAVED > conn_idx)
    {
        p_hdl->range = gs_cent_addr_rslv_char_ranges[conn_idx];
    }
}

/*----------------------------------------------------------------------------------------------------------------------
    Resolvable Private Address Only Characteristic : The device should check if the peer will only use Resolvable Private Addresses (RPAs) after bonding by reading the Resolvable Private Address Only characteristic.
----------------------------------------------------------------------------------------------------------------------*/


/* Resolvable Private Address Only characteristic attribute handles */
static st_ble_gatt_hdl_range_t gs_rslv_priv_addr_only_char_ranges[BLE_SERVC_MAX_NUM_OF_SAVED];

/* Resolvable Private Address Only characteristic definition */
static const st_ble_servc_char_info_t gs_rslv_priv_addr_only_char = {
    .uuid_16      = BLE_GAPC_RSLV_PRIV_ADDR_ONLY_UUID,
    .uuid_type    = BLE_GATT_16_BIT_UUID_FORMAT,
    .app_size     = sizeof(uint8_t),
    .db_size      = BLE_GAPC_RSLV_PRIV_ADDR_ONLY_LEN,
    .char_idx     = BLE_GAPC_RSLV_PRIV_ADDR_ONLY_IDX,
    .p_attr_hdls  = gs_rslv_priv_addr_only_char_ranges,
    .decode       = (ble_servc_attr_decode_t)decode_uint8_t,
    .encode       = (ble_servc_attr_encode_t)encode_uint8_t,
};

ble_status_t R_BLE_GAPC_ReadRslvPrivAddrOnly(uint16_t conn_hdl) // @suppress("API function naming")
{
    return R_BLE_SERVC_ReadChar(&gs_rslv_priv_addr_only_char, conn_hdl);
}

void R_BLE_GAPC_GetRslvPrivAddrOnlyAttrHdl(const st_ble_dev_addr_t *p_addr, st_ble_gapc_rslv_priv_addr_only_attr_hdl_t *p_hdl)
{
    uint8_t conn_idx;

    conn_idx = R_BLE_SERVC_GetConnIdx(p_addr);
    if (BLE_SERVC_MAX_NUM_OF_SAVED > conn_idx)
    {
        p_hdl->range = gs_rslv_priv_addr_only_char_ranges[conn_idx];
    }
}


/*----------------------------------------------------------------------------------------------------------------------
    GAP Service client
----------------------------------------------------------------------------------------------------------------------*/

/* GAP Service client attribute handles */
static st_ble_gatt_hdl_range_t gs_gapc_ranges[BLE_SERVC_MAX_NUM_OF_SAVED];

const st_ble_servc_char_info_t *gspp_gapc_chars[] = {
    &gs_dev_name_char,
    &gs_appearance_char,
    &gs_per_pref_conn_param_char,
    &gs_cent_addr_rslv_char,
    &gs_rslv_priv_addr_only_char,
};

static st_ble_servc_info_t gs_client_info = {
    .pp_chars     = gspp_gapc_chars,
    .num_of_chars = ARRAY_SIZE(gspp_gapc_chars),
    .p_attr_hdls  = gs_gapc_ranges,
};

ble_status_t R_BLE_GAPC_Init(ble_servc_app_cb_t cb) // @suppress("API function naming")
{
    if (NULL == cb)
    {
        return BLE_ERR_INVALID_PTR;
    }

    gs_client_info.cb = cb;

    return R_BLE_SERVC_RegisterClient(&gs_client_info);
}

void R_BLE_GAPC_ServDiscCb(uint16_t conn_hdl, uint8_t serv_idx, uint16_t type, void *p_param) // @suppress("API function naming")
{
    R_BLE_SERVC_ServDiscCb(&gs_client_info, conn_hdl, serv_idx, type, p_param);
}

void R_BLE_GAPC_GetServAttrHdl(const st_ble_dev_addr_t *p_addr, st_ble_gatt_hdl_range_t *p_hdl)
{
    uint8_t conn_idx;

    conn_idx = R_BLE_SERVC_GetConnIdx(p_addr);

    *p_hdl = gs_gapc_ranges[conn_idx];
}

/* Start user code for function definitions. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

