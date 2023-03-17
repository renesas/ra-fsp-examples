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
#include <stdlib.h>
#include <string.h>
#include "r_ble_servc_if.h"
#include "discovery/r_ble_disc.h"

typedef struct {
    st_ble_dev_addr_t  bd_addr;
    uint16_t           conn_hdl;
    uint8_t            conn_idx;

    /* Used for Long Read */
    uint16_t           read_attr_hdl;
    uint8_t           *p_read_buf;
    uint16_t           read_buf_pos;

    /* Used for Long Write */
    uint16_t           write_attr_hdl;
} st_ble_conn_info_t;

static st_ble_conn_info_t gs_conn_info[BLE_SERVC_MAX_NUM_OF_SAVED];
static const st_ble_servc_info_t *gs_clients[BLE_SERVC_MAX_NUM_OF_CLIENTS];
static uint8_t gs_num_of_clients;

static uint8_t find_conn_idx_from_bd_addr(const st_ble_dev_addr_t *p_addr)
{
    for (uint8_t i = 0; i < BLE_SERVC_MAX_NUM_OF_SAVED; i++)
    {
        if ((gs_conn_info[i].bd_addr.type == p_addr->type) && 
            (0 == memcmp(gs_conn_info[i].bd_addr.addr, p_addr->addr, BLE_BD_ADDR_LEN)))
        {
            return i;
        }
    }

    return 0xFF;
}

static uint8_t find_conn_idx_from_conn_hdl(uint16_t conn_hdl)
{
    for (uint8_t i = 0; i < BLE_SERVC_MAX_NUM_OF_SAVED; i++)
    {
        if (gs_conn_info[i].conn_hdl == conn_hdl)
        {
            return i;
        }
    }

    return 0xFF;
}

static void set_conn_idx(uint16_t conn_hdl, const st_ble_dev_addr_t *p_addr)
{
    for (uint8_t i = 0; i < BLE_SERVC_MAX_NUM_OF_SAVED; i++)
    {
        /* If we already have the peer attr hdls, use it. */
        if ((gs_conn_info[i].bd_addr.type == p_addr->type) && 
            (0 == memcmp(gs_conn_info[i].bd_addr.addr, p_addr->addr, BLE_BD_ADDR_LEN)))
        {
            gs_conn_info[i].conn_hdl = conn_hdl;
            return;
        }
    }

    for (uint8_t i = 0; i < BLE_SERVC_MAX_NUM_OF_SAVED; i++)
    {
        if (BLE_GAP_INVALID_CONN_HDL == gs_conn_info[i].conn_hdl)
        {
            memcpy(&gs_conn_info[i].bd_addr, p_addr, sizeof(gs_conn_info[i].bd_addr));
            gs_conn_info[i].conn_hdl = conn_hdl;
            break;
        }
    }
}

static void clear_conn_idx(uint16_t conn_hdl)
{
    uint8_t conn_idx;

    conn_idx = find_conn_idx_from_conn_hdl(conn_hdl);

    if (0xFF != conn_idx)
    {
        gs_conn_info[conn_idx].conn_hdl = BLE_GAP_INVALID_CONN_HDL;
        if (NULL != gs_conn_info[conn_idx].p_read_buf)
        {
            free(gs_conn_info[conn_idx].p_read_buf);
            gs_conn_info[conn_idx].p_read_buf = NULL;
        }
        gs_conn_info[conn_idx].read_buf_pos  = 0;
        gs_conn_info[conn_idx].read_attr_hdl = 0;
    }
}

static void find_attr(uint8_t conn_idx,
                      uint16_t attr_hdl,
                      const st_ble_servc_info_t **pp_client,
                      const st_ble_servc_char_info_t **pp_char)
{
    if (BLE_SERVC_MAX_NUM_OF_SAVED <= conn_idx)
    {
        return;
    }
    for (uint8_t s = 0; s < gs_num_of_clients; s++)
    {
        for (uint8_t c = 0; c < gs_clients[s]->num_of_chars; c++)
        {
            if ((gs_clients[s]->p_attr_hdls[conn_idx].start_hdl != 0x00) &&
                (gs_clients[s]->pp_chars[c]->p_attr_hdls[conn_idx].start_hdl <= attr_hdl) &&
                (gs_clients[s]->pp_chars[c]->p_attr_hdls[conn_idx].end_hdl >= attr_hdl))
            {
                *pp_client = gs_clients[s];
                *pp_char   = gs_clients[s]->pp_chars[c];
                return;
            }
        }
    }
}

static void read_evt_handler(uint16_t conn_hdl, uint16_t attr_hdl, st_ble_gatt_value_t *p_value, ble_status_t result)
{
    uint8_t conn_idx = find_conn_idx_from_conn_hdl(conn_hdl);

    if (0xFF == conn_idx)
    {
        return;
    }

    const st_ble_servc_info_t      *p_client = NULL;
    const st_ble_servc_char_info_t *p_attr   = NULL;
    find_attr(conn_idx, attr_hdl, &p_client, &p_attr);

    if (NULL != p_attr)
    {
        /* Characteristic */
        if (attr_hdl == p_attr->p_attr_hdls[conn_idx].start_hdl + 1)
        {
            if (BLE_SUCCESS == result)
            {
                ble_status_t ret;
                void *p_app_value;

                p_app_value = malloc(p_attr->app_size);
                if(NULL == p_app_value)
                {
                    ret = BLE_ERR_MEM_ALLOC_FAILED;
                }
                else
                {
                    memset(p_app_value, 0x00, p_attr->app_size);

                    ret = p_attr->decode(p_app_value, p_value);
                }
                if (NULL != p_attr->read_rsp_cb)
                {
                    p_attr->read_rsp_cb(p_attr, conn_hdl, ret, p_app_value);
                }
                else
                {
                    st_ble_servc_evt_data_t evt_data = {
                        .conn_hdl  = conn_hdl,
                        .param_len = (NULL != p_app_value)?p_attr->app_size:0,
                        .p_param   = p_app_value,
                    };
                    if ((NULL != p_client) && (NULL != p_client->cb))
                    {
                        p_client->cb(BLE_SERVC_MULTI_ATTR_EVENT(p_attr->char_idx, p_attr->inst_idx, BLE_SERVC_READ_RSP), ret, &evt_data);
                    }
                }
                if (NULL != p_app_value)
                {
                    free(p_app_value);
                }
            }
            else
            {
                if (NULL != p_attr->read_rsp_cb)
                {
                    p_attr->read_rsp_cb(p_attr, conn_hdl, result, NULL);
                }
                else
                {
                    st_ble_servc_evt_data_t evt_data = {
                        .conn_hdl  = conn_hdl,
                        .param_len = 0,
                        .p_param   = NULL,
                    };
                    if ((NULL != p_client) && (NULL != p_client->cb))
                    {
                        p_client->cb(BLE_SERVC_MULTI_ATTR_EVENT(p_attr->char_idx, p_attr->inst_idx, BLE_SERVC_READ_RSP), result, &evt_data);
                    }
                }
            }
        }
        /* Descriptor */
        else
        {
            for (uint8_t d = 0; d < p_attr->num_of_descs; d++)
            {
                if (attr_hdl == p_attr->pp_descs[d]->p_attr_hdls[conn_idx])
                {
                    if (BLE_SUCCESS == result)
                    {
                        ble_status_t ret;
                        uint8_t *p_app_value;

                        p_app_value = malloc(p_attr->pp_descs[d]->app_size);
                        if(NULL == p_app_value)
                        {
                            ret = BLE_ERR_MEM_ALLOC_FAILED;
                        }
                        else
                        {
                            memset(p_app_value, 0x00, p_attr->pp_descs[d]->app_size);

                            ret = p_attr->pp_descs[d]->decode(p_app_value, p_value);
                        }
                        if (NULL != p_attr->pp_descs[d]->read_rsp_cb)
                        {
                            p_attr->pp_descs[d]->read_rsp_cb(&p_attr->pp_descs[d], conn_hdl, ret, p_app_value);
                        }
                        else
                        {
                            st_ble_servc_evt_data_t evt_data = {
                                .conn_hdl  = conn_hdl,
                                .param_len = (NULL != p_app_value)?p_attr->pp_descs[d]->app_size:0,
                                .p_param   = p_app_value,
                            };
                            if ((NULL != p_client) && (NULL != p_client->cb))
                            {
                                p_client->cb(BLE_SERVC_MULTI_ATTR_EVENT(p_attr->pp_descs[d]->desc_idx, p_attr->inst_idx, BLE_SERVC_READ_RSP), ret, &evt_data);
                            }
                        }
                        if (NULL != p_app_value)
                        {
                            free(p_app_value);
                        }
                    }
                    else
                    {
                        if (NULL != p_attr->pp_descs[d]->read_rsp_cb)
                        {
                            p_attr->pp_descs[d]->read_rsp_cb(&p_attr->pp_descs[d], conn_hdl, result, NULL);
                        }
                        else
                        {
                            st_ble_servc_evt_data_t evt_data = {
                                .conn_hdl  = conn_hdl,
                                .param_len = 0,
                                .p_param   = NULL,
                            };
                            if ((NULL != p_client) && (NULL != p_client->cb))
                            {
                                p_client->cb(BLE_SERVC_MULTI_ATTR_EVENT(p_attr->pp_descs[d]->desc_idx, p_attr->inst_idx, BLE_SERVC_READ_RSP), result, &evt_data);
                            }
                        }
                    }
                    break;
                }
            }
        }
    }
}

static void write_evt_handler(uint16_t conn_hdl, uint16_t attr_hdl, ble_status_t result)
{
    uint8_t conn_idx = find_conn_idx_from_conn_hdl(conn_hdl);

    if (0xFF == conn_idx)
    {
        return;
    }

    const st_ble_servc_info_t      *p_client = NULL;
    const st_ble_servc_char_info_t *p_attr   = NULL;
    find_attr(conn_idx, attr_hdl, &p_client, &p_attr);

    if (NULL != p_attr)
    {
        /* Characteristics */
        if (attr_hdl == p_attr->p_attr_hdls[conn_idx].start_hdl + 1)
        {
            if (NULL != p_attr->write_rsp_cb)
            {
                p_attr->write_rsp_cb(p_attr, conn_hdl, result);
            }
            else
            {
                st_ble_servc_evt_data_t evt_data = {
                    .conn_hdl  = conn_hdl,
                    .param_len = 0,
                    .p_param   = NULL,
                };
                if ((NULL != p_client) && (NULL != p_client->cb))
                {
                    p_client->cb(BLE_SERVC_MULTI_ATTR_EVENT(p_attr->char_idx, p_attr->inst_idx, BLE_SERVC_WRITE_RSP), result, &evt_data);
                }
            }
        }
        /* Descriptors */
        else
        {
            for (uint8_t d = 0; d < p_attr->num_of_descs; d++)
            {
                if (attr_hdl == p_attr->pp_descs[d]->p_attr_hdls[conn_idx])
                {
                    if (NULL != p_attr->pp_descs[d]->write_rsp_cb)
                    {
                        p_attr->pp_descs[d]->write_rsp_cb(&p_attr->pp_descs[d], conn_hdl, result);
                    }
                    else
                    {
                        st_ble_servc_evt_data_t evt_data = {
                            .conn_hdl  = conn_hdl,
                            .param_len = 0,
                            .p_param   = NULL,
                        };
                        if ((NULL != p_client) && (NULL != p_client->cb))
                        {
                            p_client->cb(BLE_SERVC_MULTI_ATTR_EVENT(p_attr->pp_descs[d]->desc_idx, p_attr->inst_idx, BLE_SERVC_WRITE_RSP), result, &evt_data);
                        }
                    }
                    break;
                }
            }
        }
    }
}

void R_BLE_SERVC_GattcCb(uint16_t type, ble_status_t result, st_ble_gattc_evt_data_t * p_data)
{
    uint8_t conn_idx = find_conn_idx_from_conn_hdl(p_data->conn_hdl);
    if (BLE_GATTC_EVENT_CONN_IND != type)
    {
        if (0xFF == conn_idx)
        {
            return;
        }
    }
    switch (type)
    {
        case BLE_GATTC_EVENT_CONN_IND:
        {
            st_ble_gattc_conn_evt_t *p_conn_evt_param =
                (st_ble_gattc_conn_evt_t *)p_data->p_param;

            set_conn_idx(p_data->conn_hdl, p_conn_evt_param->p_addr);
        } break;

        case BLE_GATTC_EVENT_DISCONN_IND:
        {
            clear_conn_idx(p_data->conn_hdl);
        } break;

        case BLE_GATTC_EVENT_CHAR_READ_RSP:
        {
            st_ble_gattc_rd_char_evt_t *p_rd_char_evt_param =
                (st_ble_gattc_rd_char_evt_t *)p_data->p_param;

            read_evt_handler(p_data->conn_hdl,
                             p_rd_char_evt_param->read_data.attr_hdl,
                             &p_rd_char_evt_param->read_data.value,
                             result);
        } break;

        case BLE_GATTC_EVENT_CHAR_PART_READ_RSP:
        {
            st_ble_gattc_rd_char_evt_t *p_rd_char_evt_param =
                (st_ble_gattc_rd_char_evt_t *)p_data->p_param;

            const st_ble_servc_info_t      *p_client = NULL;
            const st_ble_servc_char_info_t *p_attr   = NULL;
            find_attr(conn_idx, p_rd_char_evt_param->read_data.attr_hdl, &p_client, &p_attr);

            if (NULL != p_attr)
            {
                if (p_rd_char_evt_param->read_data.attr_hdl ==
                    p_attr->p_attr_hdls[conn_idx].start_hdl + 1)
                {
                    if (BLE_SUCCESS == result)
                    {
                        if (NULL == gs_conn_info[conn_idx].p_read_buf)
                        {
                            gs_conn_info[conn_idx].p_read_buf = malloc(p_attr->db_size);
                            if(NULL == gs_conn_info[conn_idx].p_read_buf)
                            {
                                if (NULL != p_attr->read_rsp_cb)
                                {
                                    p_attr->read_rsp_cb(p_attr, p_data->conn_hdl, BLE_ERR_MEM_ALLOC_FAILED, NULL);
                                }
                                else
                                {
                                    st_ble_servc_evt_data_t evt_data = {
                                        .conn_hdl  = p_data->conn_hdl,
                                        .param_len = 0,
                                        .p_param   = NULL
                                    };
                                    if ((NULL != p_client) && (NULL != p_client->cb))
                                    {
                                        p_client->cb(BLE_SERVC_MULTI_ATTR_EVENT(p_attr->char_idx, p_attr->inst_idx, BLE_SERVC_READ_RSP), BLE_ERR_MEM_ALLOC_FAILED, &evt_data);
                                    }
                                }
                                break;
                            }
                            memset(gs_conn_info[conn_idx].p_read_buf, 0x00, p_attr->db_size);

                            gs_conn_info[conn_idx].read_buf_pos  = 0;
                            gs_conn_info[conn_idx].read_attr_hdl = p_rd_char_evt_param->read_data.attr_hdl;
                        }

                        memcpy(&gs_conn_info[conn_idx].p_read_buf[gs_conn_info[conn_idx].read_buf_pos],
                               p_rd_char_evt_param->read_data.value.p_value, 
                               p_rd_char_evt_param->read_data.value.value_len);
                        gs_conn_info[conn_idx].read_buf_pos =
                            (uint16_t )(gs_conn_info[conn_idx].read_buf_pos + p_rd_char_evt_param->read_data.value.value_len);
                    }
                    else
                    {
                        if (NULL != gs_conn_info[conn_idx].p_read_buf)
                        {
                            free(gs_conn_info[conn_idx].p_read_buf);
                            gs_conn_info[conn_idx].p_read_buf = NULL;
                            gs_conn_info[conn_idx].read_buf_pos  = 0;
                            gs_conn_info[conn_idx].read_attr_hdl = 0;
                        }

                        if (NULL != p_attr->read_rsp_cb)
                        {
                            p_attr->read_rsp_cb(p_attr, p_data->conn_hdl, result, NULL);
                        }
                        else
                        {
                            st_ble_servc_evt_data_t evt_data = {
                                .conn_hdl  = p_data->conn_hdl,
                                .param_len = 0,
                                .p_param   = NULL,
                            };
                            if ((NULL != p_client) && (NULL != p_client->cb))
                            {
                                p_client->cb(BLE_SERVC_MULTI_ATTR_EVENT(p_attr->char_idx, p_attr->inst_idx, BLE_SERVC_READ_RSP), result, &evt_data);
                            }
                        }
                    }
                }
                else
                {
                    for (uint8_t d = 0; d < p_attr->num_of_descs; d++)
                    {
                        if (p_rd_char_evt_param->read_data.attr_hdl ==
                            p_attr->pp_descs[d]->p_attr_hdls[conn_idx])
                        {
                            if (BLE_SUCCESS == result)
                            {
                                if (NULL == gs_conn_info[conn_idx].p_read_buf)
                                {
                                    gs_conn_info[conn_idx].p_read_buf    = malloc(p_attr->pp_descs[d]->db_size);
                                    if(NULL == gs_conn_info[conn_idx].p_read_buf)
                                    {
                                        st_ble_servc_evt_data_t evt_data = {
                                            .conn_hdl  = p_data->conn_hdl,
                                            .param_len = 0,
                                            .p_param   = NULL
                                        };
                                        if ((NULL != p_client) && (NULL != p_client->cb))
                                        {
                                            p_client->cb(BLE_SERVC_MULTI_ATTR_EVENT(p_attr->pp_descs[d]->desc_idx, p_attr->pp_descs[d]->inst_idx, BLE_SERVC_READ_RSP), BLE_ERR_MEM_ALLOC_FAILED, &evt_data);
                                        }
                                        break;
                                    }
                                    memset(gs_conn_info[conn_idx].p_read_buf, 0x00, p_attr->pp_descs[d]->db_size);

                                    gs_conn_info[conn_idx].read_buf_pos  = 0;
                                    gs_conn_info[conn_idx].read_attr_hdl = p_rd_char_evt_param->read_data.attr_hdl;
                                }

                                memcpy(&gs_conn_info[conn_idx].p_read_buf[gs_conn_info[conn_idx].read_buf_pos],
                                       p_rd_char_evt_param->read_data.value.p_value, 
                                       p_rd_char_evt_param->read_data.value.value_len);
                                gs_conn_info[conn_idx].read_buf_pos =
                                    (uint16_t )(gs_conn_info[conn_idx].read_buf_pos + p_rd_char_evt_param->read_data.value.value_len);
                            }
                            else
                            {
                                if (NULL != gs_conn_info[conn_idx].p_read_buf)
                                {
                                    free(gs_conn_info[conn_idx].p_read_buf);
                                    gs_conn_info[conn_idx].p_read_buf = NULL;
                                    gs_conn_info[conn_idx].read_buf_pos  = 0;
                                    gs_conn_info[conn_idx].read_attr_hdl = 0;
                                }

                                if (NULL != p_attr->pp_descs[d]->read_rsp_cb)
                                {
                                    p_attr->pp_descs[d]->read_rsp_cb(p_attr, p_data->conn_hdl, result, NULL);
                                }
                                else
                                {
                                    st_ble_servc_evt_data_t evt_data = {
                                        .conn_hdl  = p_data->conn_hdl,
                                        .param_len = 0,
                                        .p_param   = NULL,
                                    };
                                    if ((NULL != p_client) && (NULL != p_client->cb))
                                    {
                                        p_client->cb(BLE_SERVC_MULTI_ATTR_EVENT(p_attr->pp_descs[d]->desc_idx, p_attr->pp_descs[d]->inst_idx, BLE_SERVC_READ_RSP), result, &evt_data);
                                    }
                                }
                            }
                            break;
                        }
                    }
                }
            }
        } break;

        case BLE_GATTC_EVENT_LONG_CHAR_READ_COMP:
        {
            if (NULL != gs_conn_info[conn_idx].p_read_buf)
            {
                st_ble_gatt_value_t value = {
                    .p_value   = gs_conn_info[conn_idx].p_read_buf,
                    .value_len = gs_conn_info[conn_idx].read_buf_pos,
                };

                read_evt_handler(p_data->conn_hdl,
                                 gs_conn_info[conn_idx].read_attr_hdl,
                                 &value,
                                 result);

                free(gs_conn_info[conn_idx].p_read_buf);
            }
            gs_conn_info[conn_idx].p_read_buf = NULL;
            gs_conn_info[conn_idx].read_buf_pos  = 0;
            gs_conn_info[conn_idx].read_attr_hdl = 0;
        } break;

        case BLE_GATTC_EVENT_CHAR_WRITE_RSP:
        {
            st_ble_gattc_wr_char_evt_t *p_wr_char_evt_param =
                (st_ble_gattc_wr_char_evt_t *)p_data->p_param;
            write_evt_handler(p_data->conn_hdl, p_wr_char_evt_param->value_hdl, result);
        } break;

        case BLE_GATTC_EVENT_CHAR_PART_WRITE_RSP:
        {
            st_ble_gattc_char_part_wr_evt_t *p_char_part_wr_param =
                (st_ble_gattc_char_part_wr_evt_t *)p_data->p_param;

            gs_conn_info[conn_idx].write_attr_hdl = p_char_part_wr_param->write_data.attr_hdl;
        } break;

        case BLE_GATTC_EVENT_LONG_CHAR_WRITE_COMP:
        {
            write_evt_handler(p_data->conn_hdl, gs_conn_info[conn_idx].write_attr_hdl, result);
        } break;

        case BLE_GATTC_EVENT_HDL_VAL_NTF:
        {
            st_ble_gatt_hdl_value_pair_t *p_hdl_value_pair_param =
                (st_ble_gatt_hdl_value_pair_t *)p_data->p_param;

            const st_ble_servc_info_t      *p_client = NULL;
            const st_ble_servc_char_info_t *p_attr   = NULL;
            find_attr(conn_idx, p_hdl_value_pair_param->attr_hdl, &p_client, &p_attr);

            if ((NULL != p_attr) && (NULL != p_attr->decode))
            {
                void *p_app_value;

                p_app_value = malloc(p_attr->app_size);
                if(NULL == p_app_value)
                {
                    result = BLE_ERR_MEM_ALLOC_FAILED;
                }
                else
                {
                    memset(p_app_value, 0x00, p_attr->app_size);

                    p_attr->decode(p_app_value, &p_hdl_value_pair_param->value);
                }
                if (NULL != p_attr->hdl_val_ntf_cb)
                {
                    p_attr->hdl_val_ntf_cb(p_attr, p_data->conn_hdl, p_app_value);
                }
                else
                {
                    st_ble_servc_evt_data_t evt_data = {
                        .conn_hdl  = p_data->conn_hdl,
                        .param_len = (NULL != p_app_value)?p_attr->app_size:0,
                        .p_param   = p_app_value,
                    };
                    if ((NULL != p_client) && (NULL != p_client->cb))
                    {
                        p_client->cb(BLE_SERVC_MULTI_ATTR_EVENT(p_attr->char_idx, p_attr->inst_idx, BLE_SERVC_HDL_VAL_NTF), result, &evt_data);
                    }
                }
                if (NULL != p_app_value)
                {
                    free(p_app_value);
                }
            }
        } break;

        case BLE_GATTC_EVENT_HDL_VAL_IND:
        {
            st_ble_gatt_hdl_value_pair_t *p_hdl_value_pair_param =
                (st_ble_gatt_hdl_value_pair_t *)p_data->p_param;

            const st_ble_servc_info_t      *p_client = NULL;
            const st_ble_servc_char_info_t *p_attr   = NULL;
            find_attr(conn_idx, p_hdl_value_pair_param->attr_hdl, &p_client, &p_attr);

            if ((NULL != p_attr) && (NULL != p_attr->decode))
            {
                void *p_app_value;

                p_app_value = malloc(p_attr->app_size);
                if(NULL == p_app_value)
                {
                    result = BLE_ERR_MEM_ALLOC_FAILED;
                }
                else
                {
                    memset(p_app_value, 0x00, p_attr->app_size);

                    p_attr->decode(p_app_value, &p_hdl_value_pair_param->value);
                }

                if (NULL != p_attr->hdl_val_ind_cb)
                {
                    p_attr->hdl_val_ind_cb(p_attr, p_data->conn_hdl, p_app_value);
                }
                else
                {
                    st_ble_servc_evt_data_t evt_data = {
                        .conn_hdl  = p_data->conn_hdl,
                        .param_len = (NULL != p_app_value)?p_attr->app_size:0,
                        .p_param   = p_app_value,
                    };
                    if ((NULL != p_client) && (NULL != p_client->cb))
                    {
                        p_client->cb(BLE_SERVC_MULTI_ATTR_EVENT(p_attr->char_idx, p_attr->inst_idx, BLE_SERVC_HDL_VAL_IND), result, &evt_data);
                    }
                }

                if (NULL != p_app_value)
                {
                    free(p_app_value);
                }
            }
        } break;

        case BLE_GATTC_EVENT_ERROR_RSP:
        {
            st_ble_gattc_err_rsp_evt_t *p_err_rsp_evt_param =
                (st_ble_gattc_err_rsp_evt_t *)p_data->p_param;

            const st_ble_servc_info_t      *p_client = NULL;
            const st_ble_servc_char_info_t *p_attr   = NULL;
            find_attr(conn_idx, p_err_rsp_evt_param->attr_hdl, &p_client, &p_attr);

            if (NULL != p_attr)
            {
                switch (p_err_rsp_evt_param->op_code)
                {
                    case 0x0A: /* Read Request */
                    {
                        if (NULL != p_attr->read_rsp_cb)
                        {
                            p_attr->read_rsp_cb(p_attr, p_data->conn_hdl, p_err_rsp_evt_param->rsp_code, NULL);
                        }
                        else
                        {
                            st_ble_servc_evt_data_t evt_data = {
                                .conn_hdl  = p_data->conn_hdl,
                                .param_len = 2,
                                .p_param   = &p_err_rsp_evt_param->attr_hdl,
                            };
                            if ((NULL != p_client) && (NULL != p_client->cb))
                            {
                                p_client->cb(BLE_SERVC_MULTI_ATTR_EVENT(p_attr->char_idx, p_attr->inst_idx, BLE_SERVC_READ_RSP), p_err_rsp_evt_param->rsp_code, &evt_data);
                            }
                        }
                    } break;

                    case 0x12: /* Write Request) */
                    {
                        if (NULL != p_attr->write_rsp_cb)
                        {
                            p_attr->write_rsp_cb(p_attr, p_data->conn_hdl, p_err_rsp_evt_param->rsp_code);
                        }
                        else
                        {
                            st_ble_servc_evt_data_t evt_data = {
                                .conn_hdl  = p_data->conn_hdl,
                                .param_len = 2,
                                .p_param   = &p_err_rsp_evt_param->attr_hdl,
                            };
                            if ((NULL != p_client) && (NULL != p_client->cb))
                            {
                                p_client->cb(BLE_SERVC_MULTI_ATTR_EVENT(p_attr->char_idx, p_attr->inst_idx, BLE_SERVC_WRITE_RSP), p_err_rsp_evt_param->rsp_code, &evt_data);
                            }
                        }
                    } break;
                }
            }
        } break;

        case BLE_GATTC_EVENT_INVALID:
        {
        } break;

        case BLE_GATTC_EVENT_EX_MTU_RSP:
        case BLE_GATTC_EVENT_CHAR_READ_BY_UUID_RSP:
        case BLE_GATTC_EVENT_MULTI_CHAR_READ_RSP:
        case BLE_GATTC_EVENT_RELIABLE_WRITES_TX_COMP:
        case BLE_GATTC_EVENT_RELIABLE_WRITES_COMP:
        default:
        {
            break;
        }
    }
}

ble_status_t R_BLE_SERVC_Init(void)
{
    for (uint8_t i = 0; i < BLE_SERVC_MAX_NUM_OF_SAVED; i++)
    {
        gs_conn_info[i].conn_hdl = BLE_GAP_INVALID_CONN_HDL;
    }
    gs_num_of_clients = 0;
    return BLE_SUCCESS;
}

ble_status_t R_BLE_SERVC_RegisterClient(const st_ble_servc_info_t *p_info)
{
    ble_status_t ret;
    if( BLE_SERVC_MAX_NUM_OF_CLIENTS > gs_num_of_clients )
    {
        gs_clients[gs_num_of_clients++] = p_info;
        ret = BLE_SUCCESS;
    }
    else
    {
        ret = BLE_ERR_CONTEXT_FULL;
    }

    return ret;
}

ble_status_t R_BLE_SERVC_ReadChar(
    const st_ble_servc_char_info_t *p_attr, uint16_t conn_hdl)
{
    ble_status_t ret;

    if (NULL == p_attr)
    {
        return BLE_ERR_INVALID_ARG;
    }

    uint8_t conn_idx = find_conn_idx_from_conn_hdl(conn_hdl);
    if (0xFF == conn_idx)
    {
        return BLE_ERR_INVALID_HDL;
    }

    uint16_t mtu = BLE_GATT_DEFAULT_MTU;
    R_BLE_GATT_GetMtu(conn_hdl, &mtu);

    if (p_attr->db_size <= (mtu - 3))
    {
        ret = R_BLE_GATTC_ReadChar(conn_hdl, (uint16_t)(p_attr->p_attr_hdls[conn_idx].start_hdl+1));
    }
    else
    {
        ret = R_BLE_GATTC_ReadLongChar(conn_hdl, (uint16_t)(p_attr->p_attr_hdls[conn_idx].start_hdl+1), 0);
    }

    return ret;
}

ble_status_t R_BLE_SERVC_WriteChar(
    const st_ble_servc_char_info_t *p_attr, uint16_t conn_hdl, const void *p_app_value)
{
    ble_status_t ret;

    if ((NULL == p_attr) || (NULL == p_app_value))
    {
        return BLE_ERR_INVALID_ARG;
    }

    uint8_t conn_idx = find_conn_idx_from_conn_hdl(conn_hdl);
    if (0xFF == conn_idx)
    {
        return BLE_ERR_INVALID_HDL;
    }

    uint8_t *p_byte_value;
    p_byte_value = malloc(p_attr->db_size);
    if(NULL == p_byte_value)
    {
        return BLE_ERR_MEM_ALLOC_FAILED;
    }
    memset(p_byte_value, 0x00, p_attr->db_size);

    st_ble_gatt_value_t gatt_value = {
        .p_value   = p_byte_value,
        .value_len = p_attr->db_size,
    };

    ret = p_attr->encode(p_app_value, &gatt_value);

    st_ble_gatt_hdl_value_pair_t write_data = {
        .attr_hdl        = (uint16_t)(p_attr->p_attr_hdls[conn_idx].start_hdl + 1),
        .value.p_value   = gatt_value.p_value,
        .value.value_len = gatt_value.value_len,
    };

    if (BLE_SUCCESS != ret)
    {
        free(p_byte_value);
        return BLE_ERR_INVALID_DATA;
    }

    uint16_t mtu = BLE_GATT_DEFAULT_MTU;
    R_BLE_GATT_GetMtu(conn_hdl, &mtu);

    if (write_data.value.value_len <= (mtu - 3))
    {
        ret = R_BLE_GATTC_WriteChar(conn_hdl, &write_data);
    }
    else
    {
        ret = R_BLE_GATTC_WriteLongChar(conn_hdl, &write_data, 0);
    }

    free(p_byte_value);

    return ret;
}

ble_status_t R_BLE_SERVC_WriteCmdChar(
    const st_ble_servc_char_info_t *p_attr, uint16_t conn_hdl, const void *p_app_value)
{
    ble_status_t ret;

    if ((NULL == p_attr) || (NULL == p_app_value))
    {
        return BLE_ERR_INVALID_ARG;
    }

    uint8_t conn_idx = find_conn_idx_from_conn_hdl(conn_hdl);
    if (0xFF == conn_idx)
    {
        return BLE_ERR_INVALID_HDL;
    }

    uint8_t *p_byte_value;
    p_byte_value = malloc(p_attr->db_size);
    if(NULL == p_byte_value)
    {
        return BLE_ERR_MEM_ALLOC_FAILED;
    }
    memset(p_byte_value, 0x00, p_attr->db_size);

    st_ble_gatt_value_t gatt_value = {
        .p_value   = p_byte_value,
        .value_len = p_attr->db_size,
    };

    ret = p_attr->encode(p_app_value, &gatt_value);

    st_ble_gatt_hdl_value_pair_t write_data = {
        .attr_hdl        = (uint16_t)(p_attr->p_attr_hdls[conn_idx].start_hdl + 1),
        .value.p_value   = gatt_value.p_value,
        .value.value_len = gatt_value.value_len,
    };

    if (BLE_SUCCESS != ret)
    {
        free(p_byte_value);
        return BLE_ERR_INVALID_DATA;
    }

    ret = R_BLE_GATTC_WriteCharWithoutRsp(conn_hdl, &write_data);

    free(p_byte_value);

    return ret;
}

ble_status_t R_BLE_SERVC_ReadDesc(const st_ble_servc_desc_info_t *p_attr, uint16_t conn_hdl)
{
    if (NULL == p_attr)
    {
        return BLE_ERR_INVALID_ARG;
    }

    uint8_t conn_idx = find_conn_idx_from_conn_hdl(conn_hdl);
    if (0xFF == conn_idx)
    {
        return BLE_ERR_INVALID_HDL;
    }

    uint16_t mtu = BLE_GATT_DEFAULT_MTU;
    R_BLE_GATT_GetMtu(conn_hdl, &mtu);

    if (p_attr->db_size <= (mtu - 1))
    {
        return R_BLE_GATTC_ReadChar(conn_hdl, p_attr->p_attr_hdls[conn_idx]);
    }
    else
    {
        return R_BLE_GATTC_ReadLongChar(conn_hdl, p_attr->p_attr_hdls[conn_idx], 0);
    }
}

ble_status_t R_BLE_SERVC_ReadDesc_with_Type(const st_ble_servc_desc_info_t *p_attr, uint16_t conn_hdl, int32_t type)
{
    if (NULL == p_attr)
    {
        return BLE_ERR_INVALID_ARG;
    }

    uint8_t conn_idx = find_conn_idx_from_conn_hdl(conn_hdl);
    if (0xFF == conn_idx)
    {
        return BLE_ERR_INVALID_HDL;
    }

    uint16_t mtu = BLE_GATT_DEFAULT_MTU;
    R_BLE_GATT_GetMtu(conn_hdl, &mtu);

    if (0 == type)
    {
        return R_BLE_GATTC_ReadChar(conn_hdl, p_attr->p_attr_hdls[conn_idx]);
    }
    else
    {
        return R_BLE_GATTC_ReadLongChar(conn_hdl, p_attr->p_attr_hdls[conn_idx], 0);
    }
}



ble_status_t R_BLE_SERVC_WriteDesc(
    const st_ble_servc_desc_info_t *p_attr, uint16_t conn_hdl, const void *p_app_value)
{
    ble_status_t ret;

    if ((NULL == p_attr) || (NULL == p_app_value))
    {
        return BLE_ERR_INVALID_ARG;
    }

    uint8_t conn_idx = find_conn_idx_from_conn_hdl(conn_hdl);
    if (0xFF == conn_idx)
    {
        return BLE_ERR_INVALID_HDL;
    }

    uint8_t *p_byte_value;
    p_byte_value = malloc(p_attr->db_size);
    if(NULL == p_byte_value)
    {
        return BLE_ERR_MEM_ALLOC_FAILED;
    }
    memset(p_byte_value, 0x00, p_attr->db_size);

    st_ble_gatt_value_t gatt_value = {
        .p_value   = p_byte_value,
        .value_len = p_attr->db_size,
    };

    ret = p_attr->encode(p_app_value, &gatt_value);

    st_ble_gatt_hdl_value_pair_t write_data = {
        .attr_hdl        = p_attr->p_attr_hdls[conn_idx],
        .value.p_value   = gatt_value.p_value,
        .value.value_len = gatt_value.value_len,
    };

    if (BLE_SUCCESS != ret)
    {
        free(p_byte_value);
        return BLE_ERR_INVALID_DATA;
    }

    uint16_t mtu = BLE_GATT_DEFAULT_MTU;
    R_BLE_GATT_GetMtu(conn_hdl, &mtu);

    if (p_attr->db_size <= (mtu - 3))
    {
        ret = R_BLE_GATTC_WriteChar(conn_hdl, &write_data);
    }
    else
    {
        ret = R_BLE_GATTC_WriteLongChar(conn_hdl, &write_data, 0);
    }

    free(p_byte_value);

    return ret;
}

ble_status_t R_BLE_SERVC_WriteDesc2(
    const st_ble_servc_desc_info_t *p_attr, uint16_t conn_hdl, const void *p_app_value)
{
    ble_status_t ret;

    if ((NULL == p_attr) || (NULL == p_app_value))
    {
        return BLE_ERR_INVALID_ARG;
    }

    uint8_t conn_idx = find_conn_idx_from_conn_hdl(conn_hdl);
    if (0xFF == conn_idx)
    {
        return BLE_ERR_INVALID_HDL;
    }

    uint8_t *p_byte_value;
    p_byte_value = malloc(p_attr->db_size);
    if(NULL == p_byte_value)
    {
        return BLE_ERR_MEM_ALLOC_FAILED;
    }
    memset(p_byte_value, 0x00, p_attr->db_size);

    st_ble_gatt_value_t gatt_value = {
        .p_value   = p_byte_value,
        .value_len = p_attr->db_size,
    };

    ret = p_attr->encode(p_app_value, &gatt_value);

    st_ble_gatt_hdl_value_pair_t write_data = {
        .attr_hdl        = p_attr->p_attr_hdls[conn_idx],
        .value.p_value   = gatt_value.p_value,
        .value.value_len = gatt_value.value_len,
    };

    if (BLE_SUCCESS != ret)
    {
        free(p_byte_value);
        return BLE_ERR_INVALID_DATA;
    }

    uint16_t mtu = BLE_GATT_DEFAULT_MTU;
    R_BLE_GATT_GetMtu(conn_hdl, &mtu);

    if (write_data.value.value_len <= (mtu - 3))
    {
        ret = R_BLE_GATTC_WriteChar(conn_hdl, &write_data);
    }
    else
    {
        ret = R_BLE_GATTC_WriteLongChar(conn_hdl, &write_data, 0);
    }

    free(p_byte_value);

    return ret;
}



ble_status_t R_BLE_SERVC_WriteDesc_with_Size(
    const st_ble_servc_desc_info_t *p_attr, uint16_t conn_hdl, const st_ble_seq_data_t *p_app_value)
{
    ble_status_t ret;

    if ((NULL == p_attr) || (NULL == p_app_value))
    {
        return BLE_ERR_INVALID_ARG;
    }

    uint8_t conn_idx = find_conn_idx_from_conn_hdl(conn_hdl);
    if (0xFF == conn_idx)
    {
        return BLE_ERR_INVALID_HDL;
    }

    uint8_t *p_byte_value;
    uint16_t write_len;
    write_len = (p_app_value->len < p_attr->db_size) ? p_app_value->len : p_attr->db_size;

    p_byte_value = malloc(write_len);
    if(NULL == p_byte_value)
    {
        return BLE_ERR_MEM_ALLOC_FAILED;
    }
    memset(p_byte_value, 0x00, write_len);

    st_ble_gatt_value_t gatt_value = {
        .p_value   = p_byte_value,
        .value_len = write_len,
    };

    ret = p_attr->encode(p_app_value->data, &gatt_value);

    st_ble_gatt_hdl_value_pair_t write_data = {
        .attr_hdl        = p_attr->p_attr_hdls[conn_idx],
        .value.p_value   = gatt_value.p_value,
        .value.value_len = gatt_value.value_len,
    };

    if (BLE_SUCCESS != ret)
    {
        free(p_byte_value);
        return BLE_ERR_INVALID_DATA;
    }

    uint16_t mtu = BLE_GATT_DEFAULT_MTU;
    R_BLE_GATT_GetMtu(conn_hdl, &mtu);

    if (write_len <= (mtu - 3))
    {
        ret = R_BLE_GATTC_WriteChar(conn_hdl, &write_data);
    }
    else
    {
        ret = R_BLE_GATTC_WriteLongChar(conn_hdl, &write_data, 0);
    }

    free(p_byte_value);

    return ret;
}




void R_BLE_SERVC_ServDiscCb(const st_ble_servc_info_t *p_info, uint16_t conn_hdl, uint8_t serv_idx, uint16_t type, void *p_param)
{
    uint8_t conn_idx = find_conn_idx_from_conn_hdl(conn_hdl);
    /* unused arg */
    (void)serv_idx;

    if (0xFF == conn_idx)
    {
        return;
    }

    switch (type)
    {
        case BLE_DISC_PRIM_SERV_FOUND:
        {
            st_disc_serv_param_t *p_serv_param = (st_disc_serv_param_t *)p_param;
            memcpy(&p_info->p_attr_hdls[conn_idx], &p_serv_param->value.serv_16.range, sizeof(p_info->p_attr_hdls[conn_idx]));
        } break;

        case BLE_DISC_CHAR_FOUND:
        {
            st_disc_char_param_t *p_char_param = (st_disc_char_param_t *)p_param;

            for (uint8_t c = 0; c < p_info->num_of_chars; c++)
            {
                if ((BLE_GATT_16_BIT_UUID_FORMAT == p_char_param->uuid_type) &&
                    (p_info->pp_chars[c]->uuid_type == p_char_param->uuid_type) &&
                    (p_info->pp_chars[c]->uuid_16 == p_char_param->value.char_16.uuid_16) &&
                    (BLE_GATT_INVALID_ATTR_HDL_VAL == p_info->pp_chars[c]->p_attr_hdls[conn_idx].start_hdl))
                {
                    p_info->pp_chars[c]->p_attr_hdls[conn_idx].start_hdl = p_char_param->value.char_16.decl_hdl;

                    if (p_char_param->num_of_descs > 0)
                    {
                        p_info->pp_chars[c]->p_attr_hdls[conn_idx].end_hdl =
                            p_char_param->descs[p_char_param->num_of_descs-1].value.desc_16.desc_hdl;
                    }
                    else
                    {
                        p_info->pp_chars[c]->p_attr_hdls[conn_idx].end_hdl = p_char_param->value.char_16.value_hdl;
                    }

                    for (uint8_t d = 0; d < p_char_param->num_of_descs; d++)
                    {
                        for (uint8_t dd = 0; dd < p_info->pp_chars[c]->num_of_descs; dd++)
                        {
                            if ((BLE_GATT_16_BIT_UUID_FORMAT == p_char_param->descs[d].uuid_type) &&
                                (p_info->pp_chars[c]->pp_descs[dd]->uuid_16 == p_char_param->descs[d].value.desc_16.uuid_16) &&
                                (BLE_GATT_INVALID_ATTR_HDL_VAL == p_info->pp_chars[c]->pp_descs[dd]->p_attr_hdls[conn_idx]))
                            {
                                p_info->pp_chars[c]->pp_descs[dd]->p_attr_hdls[conn_idx] = p_char_param->descs[d].value.desc_16.desc_hdl;
                                break;
                            }
                            else if ((BLE_GATT_128_BIT_UUID_FORMAT == p_char_param->descs[d].uuid_type) &&
                                (0 == memcmp(p_info->pp_chars[c]->pp_descs[dd]->uuid_128, p_char_param->descs[d].value.desc_128.uuid_128, 0x10)) &&
                                (BLE_GATT_INVALID_ATTR_HDL_VAL == p_info->pp_chars[c]->pp_descs[dd]->p_attr_hdls[conn_idx]))    
                                {
                                    p_info->pp_chars[c]->pp_descs[dd]->p_attr_hdls[conn_idx] = p_char_param->descs[d].value.desc_128.desc_hdl;
                                    break;
                                }
                        }
                    }
                    break;
                }

                else if ((BLE_GATT_128_BIT_UUID_FORMAT == p_char_param->uuid_type) &&
                    (p_info->pp_chars[c]->uuid_type == p_char_param->uuid_type) &&
                    (0 == memcmp(p_info->pp_chars[c]->uuid_128, p_char_param->value.char_128.uuid_128, 0x10)) &&
                    (BLE_GATT_INVALID_ATTR_HDL_VAL == p_info->pp_chars[c]->p_attr_hdls[conn_idx].start_hdl))
                {
                    p_info->pp_chars[c]->p_attr_hdls[conn_idx].start_hdl = p_char_param->value.char_128.decl_hdl;

                    if (p_char_param->num_of_descs > 0)
                    {
                        p_info->pp_chars[c]->p_attr_hdls[conn_idx].end_hdl =
                            p_char_param->descs[p_char_param->num_of_descs-1].value.desc_16.desc_hdl;
                    }
                    else
                    {
                        p_info->pp_chars[c]->p_attr_hdls[conn_idx].end_hdl = p_char_param->value.char_128.value_hdl;
                    }

                    for (uint8_t d = 0; d < p_char_param->num_of_descs; d++)
                    {
                        for (uint8_t dd = 0; dd < p_info->pp_chars[c]->num_of_descs; dd++)
                        {
                            if ((BLE_GATT_16_BIT_UUID_FORMAT == p_char_param->descs[d].uuid_type) &&
                                (p_info->pp_chars[c]->pp_descs[dd]->uuid_16 == p_char_param->descs[d].value.desc_16.uuid_16) &&
                                (BLE_GATT_INVALID_ATTR_HDL_VAL == p_info->pp_chars[c]->pp_descs[dd]->p_attr_hdls[conn_idx]))
                            {
                                p_info->pp_chars[c]->pp_descs[dd]->p_attr_hdls[conn_idx] = p_char_param->descs[d].value.desc_16.desc_hdl;
                                break;
                            }
                            else if ((BLE_GATT_128_BIT_UUID_FORMAT == p_char_param->descs[d].uuid_type) &&
                                (0 == memcmp(p_info->pp_chars[c]->pp_descs[dd]->uuid_128, p_char_param->descs[d].value.desc_128.uuid_128, 0x10)) &&
                                (BLE_GATT_INVALID_ATTR_HDL_VAL == p_info->pp_chars[c]->pp_descs[dd]->p_attr_hdls[conn_idx]))
                            {
                                p_info->pp_chars[c]->pp_descs[dd]->p_attr_hdls[conn_idx] = p_char_param->descs[d].value.desc_128.desc_hdl;
                                break;
                            }
                        }
                    }
                    break;

                }
            }
        } break;

        default:
        {
            /* Do nothing. */
        } break;
    }
}

uint8_t R_BLE_SERVC_GetConnIdx(const st_ble_dev_addr_t *p_addr)
{
    return find_conn_idx_from_bd_addr(p_addr);
}
