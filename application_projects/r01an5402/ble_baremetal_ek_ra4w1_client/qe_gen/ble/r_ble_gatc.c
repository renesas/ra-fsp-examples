/**********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name: r_ble_gatc.c
 * Version : 1.0
 * Description : The source file for Generic Attribute client.
 **********************************************************************************************************************/
#include "r_ble_gatc.h"
#include "profile_cmn/r_ble_servc_if.h"

static st_ble_servc_info_t gs_client_info;

/*----------------------------------------------------------------------------------------------------------------------
    Service Changed Client Characteristic Configuration descriptor
----------------------------------------------------------------------------------------------------------------------*/

/* Service Changed characteristic descriptors attribute handles */
static uint16_t gs_serv_changed_cli_cnfg_desc_hdls[BLE_SERVC_MAX_NUM_OF_SAVED];

static const st_ble_servc_desc_info_t gs_serv_changed_cli_cnfg ={
    .uuid_16     = BLE_GATC_SERV_CHANGED_CLI_CNFG_UUID,
    .uuid_type   = BLE_GATT_16_BIT_UUID_FORMAT,
    .app_size    = sizeof(uint16_t),
    .db_size     = sizeof(uint16_t),
    .desc_idx    = BLE_GATC_SERV_CHANGED_CLI_CNFG_IDX,
    .p_attr_hdls = gs_serv_changed_cli_cnfg_desc_hdls,
    .decode      = (ble_servc_attr_decode_t)decode_uint16_t,
    .encode      = (ble_servc_attr_encode_t)encode_uint16_t,
};

ble_status_t R_BLE_GATC_WriteServChangedCliCnfg(uint16_t conn_hdl, const uint16_t *p_value) // @suppress("API function naming")
{
    return R_BLE_SERVC_WriteDesc(&gs_serv_changed_cli_cnfg, conn_hdl, p_value);
}

ble_status_t R_BLE_GATC_ReadServChangedCliCnfg(uint16_t conn_hdl) // @suppress("API function naming")
{
    return R_BLE_SERVC_ReadDesc(&gs_serv_changed_cli_cnfg, conn_hdl);
}

/*----------------------------------------------------------------------------------------------------------------------
    Service Changed Characteristic
----------------------------------------------------------------------------------------------------------------------*/

/* Service Changed characteristic attribute handles */
static st_ble_gatt_hdl_range_t gs_serv_changed_char_ranges[BLE_SERVC_MAX_NUM_OF_SAVED];

static ble_status_t decode_st_ble_gatc_serv_changed_t(st_ble_gatc_serv_changed_t *p_app_value, const st_ble_gatt_value_t *p_gatt_value)
{
    uint32_t pos = 0;

    /* Check the length */
    if (BLE_GATC_SERV_CHANGED_LEN != p_gatt_value->value_len)
    {
        return BLE_ERR_INVALID_DATA;
    }

    BT_UNPACK_LE_2_BYTE(&p_app_value->start_hdl, &p_gatt_value->p_value[pos]);
    pos += 2;

    BT_UNPACK_LE_2_BYTE(&p_app_value->end_hdl, &p_gatt_value->p_value[pos]);
    pos += 2;

    return BLE_SUCCESS;
}

static ble_status_t encode_st_ble_gatc_serv_changed_t(const st_ble_gatc_serv_changed_t *p_app_value, st_ble_gatt_value_t *p_gatt_value)
{
    uint32_t pos = 0;

    BT_PACK_LE_2_BYTE(&p_gatt_value->p_value[pos], &p_app_value->start_hdl);
    pos += 2;

    BT_PACK_LE_2_BYTE(&p_gatt_value->p_value[pos], &p_app_value->end_hdl);
    pos += 2;

    /* Update length */
    p_gatt_value->value_len = (uint16_t)pos;

    return BLE_SUCCESS;
}

/* Service Changed characteristic descriptor definition */
static const st_ble_servc_desc_info_t *gspp_serv_changed_descs[] = {
    &gs_serv_changed_cli_cnfg,
};

/* Service Changed characteristic definition */
const st_ble_servc_char_info_t gs_serv_changed_char = {
    .uuid_16      = BLE_GATC_SERV_CHANGED_UUID,
    .uuid_type    = BLE_GATT_16_BIT_UUID_FORMAT,
    .app_size     = sizeof(st_ble_gatc_serv_changed_t),
    .db_size      = BLE_GATC_SERV_CHANGED_LEN,
    .char_idx     = BLE_GATC_SERV_CHANGED_IDX,
    .p_attr_hdls  = gs_serv_changed_char_ranges,
    .decode       = (ble_servc_attr_decode_t)decode_st_ble_gatc_serv_changed_t,
    .encode       = (ble_servc_attr_encode_t)encode_st_ble_gatc_serv_changed_t,
    .num_of_descs = ARRAY_SIZE(gspp_serv_changed_descs),
    .pp_descs     = gspp_serv_changed_descs,
};

void R_BLE_GATC_GetServChangedAttrHdl(const st_ble_dev_addr_t *p_addr, st_ble_gatc_serv_changed_attr_hdl_t *p_hdl)
{
    uint8_t conn_idx;

    conn_idx = R_BLE_SERVC_GetConnIdx(p_addr);

    p_hdl->range = gs_serv_changed_char_ranges[conn_idx];
    p_hdl->cli_cnfg_desc_hdl = gs_serv_changed_cli_cnfg_desc_hdls[conn_idx];
}


/*----------------------------------------------------------------------------------------------------------------------
    Generic Attribute client
----------------------------------------------------------------------------------------------------------------------*/

/* Generic Attribute client attribute handles */
static st_ble_gatt_hdl_range_t gs_gatc_ranges[BLE_SERVC_MAX_NUM_OF_SAVED];

const st_ble_servc_char_info_t *gspp_gatc_chars[] = {
    &gs_serv_changed_char,
};

static st_ble_servc_info_t gs_client_info = {
    .pp_chars     = gspp_gatc_chars,
    .num_of_chars = ARRAY_SIZE(gspp_gatc_chars),
    .p_attr_hdls  = gs_gatc_ranges,
};

ble_status_t R_BLE_GATC_Init(ble_servc_app_cb_t cb) // @suppress("API function naming")
{
    if (NULL == cb)
    {
        return BLE_ERR_INVALID_PTR;
    }

    gs_client_info.cb = cb;

    return R_BLE_SERVC_RegisterClient(&gs_client_info);
}

void R_BLE_GATC_ServDiscCb(uint16_t conn_hdl, uint8_t serv_idx, uint16_t type, void *p_param) // @suppress("API function naming")
{
    R_BLE_SERVC_ServDiscCb(&gs_client_info, conn_hdl, serv_idx, type, p_param);
}

void R_BLE_GATC_GetServAttrHdl(const st_ble_dev_addr_t *p_addr, st_ble_gatt_hdl_range_t *p_hdl)
{
    uint8_t conn_idx;

    conn_idx = R_BLE_SERVC_GetConnIdx(p_addr);

    *p_hdl = gs_gatc_ranges[conn_idx];
}
