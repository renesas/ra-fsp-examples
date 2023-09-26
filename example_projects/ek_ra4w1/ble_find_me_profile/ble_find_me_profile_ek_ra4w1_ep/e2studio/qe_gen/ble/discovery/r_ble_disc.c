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
* Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
#include <string.h>
#include "r_ble_disc.h"

static uint8_t gs_state_discovery_service_type;
static uint8_t gs_target_inc_serv_pos;

/* enum */
typedef enum
{
    BLE_DISC_STATE_DISCOVERY_PRIMARY_SERVICE=0,
    BLE_DISC_STATE_DISCOVERY_INCLUDE_SERVICE,
}e_ble_disc_state_t;

/* Entries */
static const st_ble_disc_entry_t *gs_prim_entries;
static uint8_t gs_num_of_prim_entries;
static uint8_t gs_prim_entry_pos;

/* Primary Service */
static st_ble_gatt_hdl_range_t gs_serv_range;
static uint8_t gs_num_of_servs;

/* Included Srvice */
static st_disc_inc_serv_param_t gs_inc_servs[BLE_DISC_INC_SERV_MAX_NUM];
static uint8_t gs_inc_serv_pos;
static uint8_t gs_num_of_inc_servs;

/* Characteristic */
static st_disc_char_param_t gs_chars[BLE_DISC_CHAR_MAX_NUM];
static uint8_t gs_num_of_chars;
static uint8_t gs_char_pos;

/* Descriptor */
static st_disc_desc_param_t gs_descs[BLE_DISC_DESC_MAX_NUM];
static uint8_t gs_num_of_descs;

static bool gs_in_progress;
static ble_disc_comp_cb_t gs_comp_cb;
static uint16_t gs_disc_conn_hdl;

static ble_status_t r_ble_start_serv_disc(uint16_t conn_hdl);
static void r_ble_start_inc_serv_disc(uint16_t conn_hdl);
static void r_ble_start_char_disc(uint16_t conn_hdl);
static void r_ble_start_desc_disc(uint16_t conn_hdl);


static void r_ble_stop_serv_disc(uint16_t conn_hdl, bool cb_en)
{
    if( false != cb_en )
    {
        if (NULL != gs_comp_cb)
        {
            gs_comp_cb(conn_hdl);
        }
    }
    gs_in_progress = false;
    gs_comp_cb = NULL;
    gs_disc_conn_hdl = BLE_GAP_INVALID_CONN_HDL;
}

static bool r_ble_disc_next_prim_serv(uint16_t conn_hdl)
{
    gs_prim_entry_pos++;

    if (gs_prim_entry_pos < gs_num_of_prim_entries)
    {
        gs_inc_serv_pos = 0;
        gs_state_discovery_service_type = BLE_DISC_STATE_DISCOVERY_PRIMARY_SERVICE;
        r_ble_start_serv_disc(conn_hdl);
    }
    else
    {
        r_ble_stop_serv_disc(conn_hdl, true);
    }

    return true;
}

static bool r_ble_disc_next_inc_serv(uint16_t conn_hdl)
{
    if (gs_inc_serv_pos < gs_num_of_inc_servs)
    {
        gs_state_discovery_service_type = BLE_DISC_STATE_DISCOVERY_INCLUDE_SERVICE;

        for (uint8_t i = gs_inc_serv_pos; i < gs_num_of_inc_servs; i++)
        {
            for (uint8_t j = 0; j < gs_prim_entries[gs_prim_entry_pos].num_of_inc_servs; j++)
            {
                st_ble_disc_entry_t *p_inc_serv = &gs_prim_entries[gs_prim_entry_pos].inc_servs[j];

                if (p_inc_serv->uuid_type == gs_inc_servs[i].uuid_type)
                {
                    if (
                        ((BLE_GATT_16_BIT_UUID_FORMAT == p_inc_serv->uuid_type) &&
                         ((((uint16_t)p_inc_serv->p_uuid[1] << 8) | (p_inc_serv->p_uuid[0])) == gs_inc_servs[i].value.inc_serv_16.service.uuid_16)) ||
                        ((BLE_GATT_128_BIT_UUID_FORMAT == p_inc_serv->uuid_type) &&
                         (0 == memcmp(p_inc_serv->p_uuid, gs_inc_servs[i].value.inc_serv_128.service.uuid_128, BLE_GATT_128_BIT_UUID_SIZE))))
                    {
                        gs_serv_range.start_hdl = gs_inc_servs[i].value.inc_serv_16.service.range.start_hdl;
                        gs_serv_range.end_hdl   = gs_inc_servs[i].value.inc_serv_16.service.range.end_hdl;
                        gs_inc_serv_pos         = (uint8_t)(i + 1);
                        gs_target_inc_serv_pos  = j;
                        r_ble_start_char_disc(conn_hdl);
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

static bool r_ble_disc_next_char(uint16_t conn_hdl)
{
    gs_char_pos++;

    if (gs_char_pos < gs_num_of_chars)
    {
        r_ble_start_desc_disc(conn_hdl);
        return true;
    }

    return false;
}

static void r_ble_all_disc_process(uint16_t event, uint16_t conn_hdl)
{
    switch (event)
    {
        case BLE_GATTC_EVENT_PRIM_SERV_DISC_COMP:
        {
            if (0 == gs_num_of_servs)
            {
                r_ble_disc_next_prim_serv(conn_hdl);
            }
            else
            {
                r_ble_start_inc_serv_disc(conn_hdl);
            }
        } break;

        case BLE_GATTC_EVENT_INC_SERV_DISC_COMP:
        {
            r_ble_start_char_disc(conn_hdl);
        } break;

        case BLE_GATTC_EVENT_ALL_CHAR_DISC_COMP:
        {
            r_ble_start_desc_disc(conn_hdl);
        } break;

        case BLE_GATTC_EVENT_ALL_CHAR_DESC_DISC_COMP:
        {
            bool next;

            next = r_ble_disc_next_char(conn_hdl);

            if (false == next)
            {
                next = r_ble_disc_next_inc_serv(conn_hdl);

                if (false == next)
                {
                    r_ble_disc_next_prim_serv(conn_hdl);
                }
            }
        } break;

        default:
        {
        } break;
    }
}

static ble_status_t r_ble_start_serv_disc(uint16_t conn_hdl)
{
    gs_num_of_servs = 0;

    memset(&gs_serv_range, 0x00, sizeof(gs_serv_range));

    ble_status_t ret;
    ret = R_BLE_GATTC_DiscPrimServ(conn_hdl,
                             gs_prim_entries[gs_prim_entry_pos].p_uuid,
                             gs_prim_entries[gs_prim_entry_pos].uuid_type);

    if( BLE_SUCCESS != ret )
    {
        r_ble_stop_serv_disc(conn_hdl, (0 < gs_prim_entry_pos)?(true):(false));
    }

    return ret;
}

static void r_ble_start_inc_serv_disc(uint16_t conn_hdl)
{
    gs_num_of_inc_servs = 0;
    gs_inc_serv_pos     = 0;

    for (uint8_t i = 0; i < BLE_DISC_INC_SERV_MAX_NUM; i++)
    {
        memset(&gs_inc_servs[i], 0x00, sizeof(gs_inc_servs[i]));
    }

    ble_status_t ret;
    ret = R_BLE_GATTC_DiscIncServ(conn_hdl, &gs_serv_range);
    if( BLE_SUCCESS != ret )
    {
        r_ble_stop_serv_disc(conn_hdl, true);
    }
}

static void r_ble_start_char_disc(uint16_t conn_hdl)
{
    gs_num_of_chars = 0;
    gs_char_pos     = 0;

    for (uint8_t i = 0; i < BLE_DISC_CHAR_MAX_NUM; i++)
    {
        memset(&gs_chars[i], 0x00, sizeof(gs_chars[i]));
    }

    ble_status_t ret;
    ret = R_BLE_GATTC_DiscAllChar(conn_hdl, &gs_serv_range);
    if( BLE_SUCCESS != ret )
    {
        r_ble_stop_serv_disc(conn_hdl, true);
    }
}

static void r_ble_start_desc_disc(uint16_t conn_hdl)
{
    gs_num_of_descs = 0;

    for (uint8_t i = 0; i < BLE_DISC_DESC_MAX_NUM; i++)
    {
        memset(&gs_descs[i], 0x00, sizeof(gs_descs[i]));
    }

    st_ble_gatt_hdl_range_t range;

    if (BLE_GATT_16_BIT_UUID_FORMAT == gs_chars[gs_char_pos].uuid_type)
    {
        range.start_hdl = gs_chars[gs_char_pos].value.char_16.decl_hdl;
    }
    else
    {
        range.start_hdl = gs_chars[gs_char_pos].value.char_128.decl_hdl;
    }

    if (gs_char_pos < (gs_num_of_chars - 1))
    {
        if (BLE_GATT_16_BIT_UUID_FORMAT == gs_chars[gs_char_pos+1].uuid_type)
        {
            range.end_hdl = gs_chars[gs_char_pos+1].value.char_16.decl_hdl;
        }
        else
        {
            range.end_hdl = gs_chars[gs_char_pos+1].value.char_128.decl_hdl;
        }
    }
    else
    {
        range.end_hdl = gs_serv_range.end_hdl;
    }

    ble_status_t ret;
    ret = R_BLE_GATTC_DiscAllCharDesc(conn_hdl, &range);
    if( BLE_SUCCESS != ret )
    {
        r_ble_stop_serv_disc(conn_hdl, true);
    }
}

static void r_ble_disc_gattc_cb(uint16_t type, ble_status_t result, st_ble_gattc_evt_data_t *p_data) // @suppress("Function length")
{

    if( gs_disc_conn_hdl != p_data->conn_hdl )
    {
        return;
    }
    if( BLE_SUCCESS != result )
    {
        if( BLE_GATTC_EVENT_EX_MTU_RSP != type )
        {
            r_ble_stop_serv_disc(p_data->conn_hdl, true);
        }
        return;
    }

    switch (type)
    {
        case BLE_GATTC_EVENT_DISCONN_IND:
        {
            r_ble_stop_serv_disc(p_data->conn_hdl, false);
        } break;

        case BLE_GATTC_EVENT_ERROR_RSP:
        {
            r_ble_stop_serv_disc(p_data->conn_hdl, true);
        } break;

        case BLE_GATTC_EVENT_PRIM_SERV_16_DISC_IND:
        {
            st_ble_gattc_serv_16_evt_t *p_serv_uuid_16_evt_params =
                (st_ble_gattc_serv_16_evt_t *)p_data->p_param;

            for (uint8_t i = 0; i < (p_data->param_len / sizeof(st_ble_gattc_serv_16_evt_t)); i++)
            {
                if (gs_num_of_servs == gs_prim_entries[gs_prim_entry_pos].idx)
                {
                    st_disc_serv_param_t serv_param = {
                        .uuid_type             = BLE_GATT_16_BIT_UUID_FORMAT,
                        .value.serv_16.range   = p_serv_uuid_16_evt_params[i].range,
                        .value.serv_16.uuid_16 = p_serv_uuid_16_evt_params[i].uuid_16,
                    };

                    gs_prim_entries[gs_prim_entry_pos].serv_cb(
                        p_data->conn_hdl,
                        gs_prim_entries[gs_prim_entry_pos].idx,
                        BLE_DISC_PRIM_SERV_FOUND,
                        &serv_param);

                    memcpy(&gs_serv_range, &p_serv_uuid_16_evt_params[i].range, sizeof(gs_serv_range));
                }

                gs_num_of_servs++;
            }
        } break;

        case BLE_GATTC_EVENT_PRIM_SERV_128_DISC_IND:
        {
            st_ble_gattc_serv_128_evt_t *p_serv_uuid_128_evt_params =
                (st_ble_gattc_serv_128_evt_t *)p_data->p_param;

            for (uint8_t i = 0; i < (p_data->param_len / sizeof(st_ble_gattc_serv_128_evt_t)); i++)
            {
                if (gs_num_of_servs == gs_prim_entries[gs_prim_entry_pos].idx)
                {
                    st_disc_serv_param_t serv_param = {
                        .uuid_type               = BLE_GATT_128_BIT_UUID_FORMAT,
                        .value.serv_128.range    = p_serv_uuid_128_evt_params[i].range,
                    };
                    memcpy(serv_param.value.serv_128.uuid_128, p_serv_uuid_128_evt_params[i].uuid_128, BLE_GATT_128_BIT_UUID_SIZE);

                    gs_prim_entries[gs_prim_entry_pos].serv_cb(
                        p_data->conn_hdl,
                        gs_prim_entries[gs_prim_entry_pos].idx,
                        BLE_DISC_PRIM_SERV_FOUND,
                        &serv_param);
                    memcpy(&gs_serv_range, &p_serv_uuid_128_evt_params[i].range, sizeof(gs_serv_range));
                }

                gs_num_of_servs++;
            }
        } break;

        case BLE_GATTC_EVENT_PRIM_SERV_DISC_COMP:
        {
            if (0 == gs_num_of_servs)
            {
                gs_prim_entries[gs_prim_entry_pos].serv_cb(
                    p_data->conn_hdl,
                    gs_prim_entries[gs_prim_entry_pos].idx,
                    BLE_DISC_PRIM_SERV_NOT_FOUND,
                    NULL);
            }

            r_ble_all_disc_process(BLE_GATTC_EVENT_PRIM_SERV_DISC_COMP, p_data->conn_hdl);
        } break;

        /* ################################################################################# */

        case BLE_GATTC_EVENT_INC_SERV_16_DISC_IND:
        {
            st_ble_gattc_inc_serv_16_evt_t *p_inc_serv_16_evt_param =
                (st_ble_gattc_inc_serv_16_evt_t *)p_data->p_param;

            for (uint8_t i = 0; i < (p_data->param_len / sizeof(st_ble_gattc_inc_serv_16_evt_t)); i++)
            {
                if(gs_num_of_inc_servs < BLE_DISC_INC_SERV_MAX_NUM)
                {
                    gs_inc_servs[gs_num_of_inc_servs].uuid_type = BLE_GATT_16_BIT_UUID_FORMAT;

                    memcpy(&gs_inc_servs[gs_num_of_inc_servs].value.inc_serv_16,
                           &p_inc_serv_16_evt_param[0],
                           sizeof(gs_inc_servs[gs_num_of_inc_servs].value.inc_serv_16));

                    gs_prim_entries[gs_prim_entry_pos].serv_cb(
                        p_data->conn_hdl,
                        gs_prim_entries[gs_prim_entry_pos].idx,
                        BLE_DISC_INC_SERV_FOUND,
                        &gs_inc_servs[gs_num_of_inc_servs]);
                    gs_num_of_inc_servs++;
                }
            }
        } break;

        case BLE_GATTC_EVENT_INC_SERV_128_DISC_IND:
        {
            st_ble_gattc_inc_serv_128_evt_t *p_inc_serv_128_evt_param =
                (st_ble_gattc_inc_serv_128_evt_t *)p_data->p_param;

            for (uint8_t i = 0; i < (p_data->param_len / sizeof(st_ble_gattc_inc_serv_128_evt_t)); i++)
            {
                if(gs_num_of_inc_servs < BLE_DISC_INC_SERV_MAX_NUM)
                {
                    gs_inc_servs[gs_num_of_inc_servs].uuid_type = BLE_GATT_128_BIT_UUID_FORMAT;

                    memcpy(&gs_inc_servs[gs_num_of_inc_servs].value.inc_serv_128,
                           p_inc_serv_128_evt_param,
                           sizeof(gs_inc_servs[gs_num_of_inc_servs].value.inc_serv_128));

                    gs_prim_entries[gs_prim_entry_pos].serv_cb(
                        p_data->conn_hdl,
                        gs_prim_entries[gs_prim_entry_pos].idx,
                        BLE_DISC_INC_SERV_FOUND,
                        &gs_inc_servs[gs_num_of_inc_servs]);

                    gs_num_of_inc_servs++;
                }
            }
        } break;

        case BLE_GATTC_EVENT_INC_SERV_DISC_COMP:
        {
            r_ble_all_disc_process(BLE_GATTC_EVENT_INC_SERV_DISC_COMP, p_data->conn_hdl);
        } break;

        /* ################################################################################# */

        case BLE_GATTC_EVENT_CHAR_16_DISC_IND:
        {
            st_ble_gattc_char_16_evt_t *p_char_16_evt_params =
                (st_ble_gattc_char_16_evt_t *)p_data->p_param;

            for (uint8_t i = 0; i < (p_data->param_len / sizeof(st_ble_gattc_char_16_evt_t)); i++)
            {
                if(gs_num_of_chars < BLE_DISC_CHAR_MAX_NUM)
                {
                    gs_chars[gs_num_of_chars].uuid_type = BLE_GATT_16_BIT_UUID_FORMAT;

                    memcpy(&gs_chars[gs_num_of_chars].value.char_16,
                            &p_char_16_evt_params[i],
                            sizeof(gs_chars[gs_num_of_chars].value.char_16));

                    gs_num_of_chars++;
                }
            }   
        } break;

        case BLE_GATTC_EVENT_CHAR_128_DISC_IND:
        {
            st_ble_gattc_char_128_evt_t *p_char_128_evt_params =
                (st_ble_gattc_char_128_evt_t *)p_data->p_param;

            for (uint8_t i = 0; i < (p_data->param_len / sizeof(st_ble_gattc_char_128_evt_t)); i++)
            {
                if(gs_num_of_chars < BLE_DISC_CHAR_MAX_NUM)
                {
                    gs_chars[gs_num_of_chars].uuid_type = BLE_GATT_128_BIT_UUID_FORMAT;

                    memcpy(&gs_chars[gs_num_of_chars].value.char_128,
                        &p_char_128_evt_params[i],
                        sizeof(gs_chars[gs_num_of_chars].value.char_128));

                    gs_num_of_chars++;
                }
            }
        } break;

        case BLE_GATTC_EVENT_ALL_CHAR_DISC_COMP:
        {
            r_ble_all_disc_process(BLE_GATTC_EVENT_ALL_CHAR_DISC_COMP, p_data->conn_hdl);
        } break;

        /* ################################################################################# */

        case BLE_GATTC_EVENT_CHAR_DESC_16_DISC_IND:
        {
            st_ble_gattc_char_desc_16_evt_t *p_char_desc_16_evt_params =
                (st_ble_gattc_char_desc_16_evt_t *)p_data->p_param;

            for (uint8_t i = 0; i < (p_data->param_len / sizeof(st_ble_gattc_char_desc_16_evt_t)); i++)
            {
                if(gs_num_of_descs < BLE_DISC_DESC_MAX_NUM)
                {
                    gs_descs[gs_num_of_descs].uuid_type = BLE_GATT_16_BIT_UUID_FORMAT;

                    memcpy(&gs_descs[gs_num_of_descs].value.desc_16,
                            &p_char_desc_16_evt_params[i],
                            sizeof(gs_descs[gs_num_of_descs].value.desc_16));

                    gs_num_of_descs++;
                }
            }
        } break;

        case BLE_GATTC_EVENT_CHAR_DESC_128_DISC_IND:
        {
            st_ble_gattc_char_desc_128_evt_t *p_char_desc_128_evt_params =
                (st_ble_gattc_char_desc_128_evt_t *)p_data->p_param;

            for (uint8_t i = 0; i < (p_data->param_len / sizeof(st_ble_gattc_char_desc_128_evt_t)); i++)
            {
                if(gs_num_of_descs < BLE_DISC_DESC_MAX_NUM)
                {
                    gs_descs[gs_num_of_descs].uuid_type =
                        BLE_GATT_128_BIT_UUID_FORMAT;

                    memcpy(&gs_descs[gs_num_of_descs].value.desc_128,
                           &p_char_desc_128_evt_params[i],
                           sizeof(gs_descs[gs_num_of_descs].value.desc_128));

                    gs_num_of_descs++;
                }
            }
        } break;

        case BLE_GATTC_EVENT_ALL_CHAR_DESC_DISC_COMP:
        {
            gs_chars[gs_char_pos].descs        = gs_descs;
            gs_chars[gs_char_pos].num_of_descs = gs_num_of_descs;

            if (BLE_DISC_STATE_DISCOVERY_PRIMARY_SERVICE == gs_state_discovery_service_type)
            {
                gs_prim_entries[gs_prim_entry_pos].serv_cb(
                    p_data->conn_hdl,
                    gs_prim_entries[gs_prim_entry_pos].idx,
                    BLE_DISC_CHAR_FOUND,
                    &gs_chars[gs_char_pos]);
            }
            else if (BLE_DISC_STATE_DISCOVERY_INCLUDE_SERVICE == gs_state_discovery_service_type)
            {
                gs_prim_entries[gs_prim_entry_pos].inc_servs[gs_target_inc_serv_pos].serv_cb(
                    p_data->conn_hdl,
                    gs_prim_entries[gs_prim_entry_pos].inc_servs[gs_target_inc_serv_pos].idx,
                    BLE_DISC_CHAR_FOUND,
                    &gs_chars[gs_char_pos]);
            }
            else
            {
                /* Do nothing. */
            }

            r_ble_all_disc_process(BLE_GATTC_EVENT_ALL_CHAR_DESC_DISC_COMP, p_data->conn_hdl);
        } break;

        /* ################################################################################# */

        default:
        {
            /* Do nothing. */
        } break;
    }
}

ble_status_t R_BLE_DISC_Init(void) // @suppress("API function naming")
{
    /* Initialize static variable and stop previous discovery operation */
    r_ble_stop_serv_disc(BLE_GAP_INVALID_CONN_HDL, false);

    return R_BLE_GATTC_RegisterCb(r_ble_disc_gattc_cb, 1);
}

ble_status_t R_BLE_DISC_Start(uint16_t conn_hdl, const st_ble_disc_entry_t *p_entries, uint8_t num_of_entires, ble_disc_comp_cb_t cb) // @suppress("API function naming")
{
    if (gs_in_progress)
    {
        return BLE_ERR_ALREADY_IN_PROGRESS;
    }

    gs_prim_entries        = p_entries;
    gs_num_of_prim_entries = num_of_entires;

    gs_in_progress    = true;
    gs_comp_cb        = cb;
    gs_prim_entry_pos = 0;
    gs_disc_conn_hdl  = conn_hdl;
    gs_state_discovery_service_type = BLE_DISC_STATE_DISCOVERY_PRIMARY_SERVICE;

    const st_ble_gatt_hdl_range_t serv_init_range = {
        .start_hdl  = 0x00,
        .end_hdl    = 0x00,
    };
    st_disc_serv_param_t serv_init_param;

    /* Initialize service attribute handle with dummy value */
    for (uint8_t i = 0; i < gs_num_of_prim_entries; i++)
    {
        if(gs_prim_entries[i].uuid_type == BLE_GATT_16_BIT_UUID_FORMAT)
        {
            serv_init_param.uuid_type = BLE_GATT_16_BIT_UUID_FORMAT;
            serv_init_param.value.serv_16.range = serv_init_range;
            serv_init_param.value.serv_16.uuid_16 = (uint16_t)(((uint16_t)gs_prim_entries[i].p_uuid[1] << 8) | (gs_prim_entries[i].p_uuid[0]));
        }
        else
        {
            serv_init_param.uuid_type = BLE_GATT_128_BIT_UUID_FORMAT;
            serv_init_param.value.serv_128.range = serv_init_range;
            memcpy(serv_init_param.value.serv_128.uuid_128, gs_prim_entries[i].p_uuid, BLE_GATT_128_BIT_UUID_SIZE);
        }
        gs_prim_entries[i].serv_cb(
            conn_hdl,
            gs_prim_entries[i].idx,
            BLE_DISC_PRIM_SERV_FOUND,
            &serv_init_param);
    }

    return r_ble_start_serv_disc(conn_hdl);
}
