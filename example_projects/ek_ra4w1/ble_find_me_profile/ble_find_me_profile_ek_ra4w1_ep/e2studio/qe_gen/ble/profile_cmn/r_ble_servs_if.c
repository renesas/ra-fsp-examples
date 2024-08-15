/***********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#include <stdlib.h>
#include <string.h>
#include "r_ble_servs_if.h"

static const st_ble_servs_info_t *gs_servs[BLE_PRF_MAX_NUM_OF_SERVS];
static uint8_t gs_num_of_servs;

static void find_attr(uint16_t attr_hdl,
                      const st_ble_servs_info_t **pp_serv,
                      const st_ble_servs_char_info_t **pp_char)
{
    for (uint8_t s = 0; s < gs_num_of_servs; s++)
    {
        for (uint8_t c = 0; c < gs_servs[s]->num_of_chars; c++)
        {
            if ((gs_servs[s]->pp_chars[c]->start_hdl <= attr_hdl) &&
                (gs_servs[s]->pp_chars[c]->end_hdl >= attr_hdl))
            {
                *pp_serv = gs_servs[s];
                *pp_char = gs_servs[s]->pp_chars[c];
                return;
            }
        }
    }
}

static void desc_write_evt_handler(uint16_t conn_hdl, uint16_t attr_hdl, st_ble_gatt_value_t *p_value)
{
    const st_ble_servs_info_t       *p_serv = NULL;
    const st_ble_servs_char_info_t  *p_char = NULL;

    find_attr(attr_hdl, &p_serv, &p_char);

    if ((NULL != p_serv) && (NULL != p_char))
    {
        if (NULL != p_char->pp_descs)
        {
            for (uint8_t d = 0; d < p_char->num_of_descs; d++)
            {
                if ((p_char->pp_descs[d]->attr_hdl == attr_hdl) &&
                    (NULL != p_char->pp_descs[d]->decode))
                {
                    ble_status_t ret;

                    void *p_app_value;
                    p_app_value = malloc(p_char->pp_descs[d]->app_size);
                    if(NULL == p_app_value)
                    {
                        ret = BLE_ERR_MEM_ALLOC_FAILED;
                    }
                    else
                    {
                        ret = p_char->pp_descs[d]->decode(p_app_value, (const st_ble_gatt_value_t *)p_value);
                    }

                    if (NULL != p_char->pp_descs[d]->write_req_cb)
                    {
                        p_char->pp_descs[d]->write_req_cb(&p_char->pp_descs[d], conn_hdl, ret, (const void *)p_app_value);
                    }
                    else
                    {
                        st_ble_servs_evt_data_t evt_data = {
                            .conn_hdl  = conn_hdl,
                            .param_len = (NULL != p_app_value)?p_char->pp_descs[d]->app_size:0,
                            .p_param   = p_app_value,
                        };
                        if (NULL != p_serv->cb)
                        {
                            p_serv->cb(BLE_SERVS_MULTI_ATTR_EVENT(p_char->pp_descs[d]->desc_idx, p_char->pp_descs[d]->inst_idx, BLE_SERVS_WRITE_REQ), ret, &evt_data);
                        }
                    }
                    if (NULL != p_app_value)
                    {
                        free(p_app_value);
                    }
                    break;
                }
            }
        }
    }
}

static void desc_read_evt_handler(uint16_t conn_hdl, uint16_t attr_hdl)
{
    const st_ble_servs_info_t       *p_serv = NULL;
    const st_ble_servs_char_info_t  *p_char = NULL;

    find_attr(attr_hdl, &p_serv, &p_char);

    if ((NULL != p_serv) && (NULL != p_char))
    {
        if (NULL != p_char->pp_descs)
        {
            for (uint8_t d = 0; d < p_char->num_of_descs; d++)
            {
                if (p_char->pp_descs[d]->attr_hdl == attr_hdl)
                {
                    if (NULL != p_char->pp_descs[d]->read_req_cb)
                    {
                        p_char->pp_descs[d]->read_req_cb(&p_char->pp_descs[d], conn_hdl);
                    }
                    else
                    {
                        st_ble_servs_evt_data_t evt_data = {
                            .conn_hdl   = conn_hdl,
                            .param_len  = 0,
                            .p_param    = NULL,
                        };
                        if (NULL != p_serv->cb)
                        {
                            p_serv->cb(BLE_SERVS_MULTI_ATTR_EVENT(p_char->pp_descs[d]->desc_idx, p_char->pp_descs[d]->inst_idx, BLE_SERVS_READ_REQ), BLE_SUCCESS, &evt_data);
                        }
                    }
                    break;
                }
            }
        }
    }
}

static void ble_gatts_db_app_cb(uint16_t conn_hdl, uint8_t db_op, uint16_t attr_hdl, st_ble_gatt_value_t *p_value)
{
    const st_ble_servs_info_t       *p_serv = NULL;
    const st_ble_servs_char_info_t  *p_char = NULL;


    find_attr(attr_hdl, &p_serv, &p_char);

    if ((NULL == p_serv) || (NULL == p_char))
    {
        return;
    }

    switch (db_op)
    {
        case BLE_GATTS_OP_CHAR_PEER_READ_REQ:
        {
            if (NULL != p_char->read_req_cb)
            {
                p_char->read_req_cb(p_char, conn_hdl);
            }
            else
            {
                st_ble_servs_evt_data_t evt_data = {
                    .conn_hdl  = conn_hdl,
                    .param_len = 0,
                    .p_param   = NULL,
                };
                if (NULL != p_serv->cb)
                {
                    p_serv->cb(BLE_SERVS_MULTI_ATTR_EVENT(p_char->char_idx, p_char->inst_idx, BLE_SERVS_READ_REQ), BLE_SUCCESS, &evt_data);
                }
            }
        } break;

        case BLE_GATTS_OP_CHAR_PEER_WRITE_REQ:
        {
            if (NULL != p_char->decode)
            {
                ble_status_t ret;
                void *p_app_value;

                p_app_value = malloc(p_char->app_size);
                if(NULL == p_app_value)
                {
                    ret = BLE_ERR_MEM_ALLOC_FAILED;
                }
                else
                {
                    ret = p_char->decode(p_app_value, p_value);
                }
                if (NULL != p_char->write_req_cb)
                {
                    p_char->write_req_cb(p_char, conn_hdl, ret, p_app_value);
                }
                else
                {
                    st_ble_servs_evt_data_t evt_data = {
                        .conn_hdl  = conn_hdl,
                        .param_len = (NULL != p_app_value)?p_char->app_size:0,
                        .p_param   = p_app_value,
                    };

                    if (NULL != p_serv->cb)
                    {
                        p_serv->cb(BLE_SERVS_MULTI_ATTR_EVENT(p_char->char_idx, p_char->inst_idx, BLE_SERVS_WRITE_REQ), ret, &evt_data);
                    }
                }
                if (NULL != p_app_value)
                {
                    free(p_app_value);
                }
            }
        } break;

        case BLE_GATTS_OP_CHAR_PEER_WRITE_CMD:
        {
            if (NULL != p_char->decode)
            {
                ble_status_t ret;
                void *p_app_value;

                p_app_value = malloc(p_char->app_size);
                if(NULL == p_app_value)
                {
                    ret = BLE_ERR_MEM_ALLOC_FAILED;
                }
                else
                {
                    ret = p_char->decode(p_app_value, p_value);
                }
                if (NULL != p_char->write_cmd_cb)
                {
                    p_char->write_cmd_cb(p_char, conn_hdl, ret, p_app_value);
                }
                else
                {
                    st_ble_servs_evt_data_t evt_data = {
                        .conn_hdl  = conn_hdl,
                        .param_len = (NULL != p_app_value)?p_char->app_size:0,
                        .p_param   = p_app_value,
                    };
                    if (NULL != p_serv->cb)
                    {
                        p_serv->cb(BLE_SERVS_MULTI_ATTR_EVENT(p_char->char_idx, p_char->inst_idx, BLE_SERVS_WRITE_CMD), ret, &evt_data);
                    }
                }
                if (NULL != p_app_value)
                {
                    free(p_app_value);
                }
            }
        } break;

        case BLE_GATTS_OP_CHAR_PEER_CLI_CNFG_WRITE_REQ:
        case BLE_GATTS_OP_CHAR_PEER_SER_CNFG_WRITE_REQ:
        case BLE_GATTS_OP_CHAR_PEER_USR_DESC_WRITE_REQ:
        case BLE_GATTS_OP_CHAR_PEER_HLD_DESC_WRITE_REQ:
        {
            desc_write_evt_handler(conn_hdl, attr_hdl, p_value);
        } break;

        case BLE_GATTS_OP_CHAR_PEER_CLI_CNFG_READ_REQ:
        case BLE_GATTS_OP_CHAR_PEER_SER_CNFG_READ_REQ:
        case BLE_GATTS_OP_CHAR_PEER_USR_DESC_READ_REQ:
        case BLE_GATTS_OP_CHAR_PEER_HLD_DESC_READ_REQ:
        {
            desc_read_evt_handler(conn_hdl, attr_hdl);
        } break;

        default:
        {
            /* Do nothing. */
        } break;
    }
}

void R_BLE_SERVS_GattsCb(uint16_t type, ble_status_t result, st_ble_gatts_evt_data_t *p_data)
{
    static uint16_t s_write_long_attr_hdl = BLE_GATT_INVALID_ATTR_HDL_VAL;
    
    switch (type)
    {
        case BLE_GATTS_EVENT_CONN_IND:
        case BLE_GATTS_EVENT_DISCONN_IND:
        break;

        case BLE_GATTS_EVENT_EX_MTU_REQ:
        {
            R_BLE_GATTS_RspExMtu(p_data->conn_hdl, BLE_PRF_MTU_SIZE);
        } 
        break;

        case BLE_GATTS_EVENT_HDL_VAL_CNF:
        {
            const st_ble_gatts_cfm_evt_t *p_cfm_evt_param =
                (st_ble_gatts_cfm_evt_t *)p_data->p_param;

            const st_ble_servs_info_t       *p_serv = NULL;
            const st_ble_servs_char_info_t  *p_char = NULL;

            find_attr(p_cfm_evt_param->attr_hdl, &p_serv, &p_char);

            if ((NULL != p_serv) && (NULL != p_char))
            {
                if ((p_char->start_hdl + 1) == p_cfm_evt_param->attr_hdl)
                {
                    if (NULL != p_char->hdl_val_cnf_cb)
                    {
                        p_char->hdl_val_cnf_cb(p_char, p_data->conn_hdl);
                    }
                    else
                    {
                        st_ble_servs_evt_data_t evt_data = {
                            .conn_hdl  = p_data->conn_hdl,
                            .param_len = 0,
                            .p_param   = NULL,
                        };
                        if (NULL != p_serv->cb)
                        {
                            p_serv->cb(BLE_SERVS_MULTI_ATTR_EVENT(p_char->char_idx, p_char->inst_idx, BLE_SERVS_HDL_VAL_CNF), result, &evt_data);
                        }
                    }
                }
            }
        } break;

        case BLE_GATTS_EVENT_DB_ACCESS_IND:
        {
            const st_ble_gatts_db_access_evt_t *p_db_access_evt_param =
                (st_ble_gatts_db_access_evt_t *)p_data->p_param;

            ble_gatts_db_app_cb(p_data->conn_hdl,
                                p_db_access_evt_param->p_params->db_op,
                                p_db_access_evt_param->p_params->attr_hdl,
                                &p_db_access_evt_param->p_params->value);
        } break;

        case BLE_GATTS_EVENT_WRITE_RSP_COMP:
        {
            st_ble_gatts_write_rsp_evt_t *p_write_rsp_evt_param =
                (st_ble_gatts_write_rsp_evt_t *)p_data->p_param;

            const st_ble_servs_info_t       *p_serv = NULL;
            const st_ble_servs_char_info_t  *p_char = NULL;

            find_attr(p_write_rsp_evt_param->attr_hdl, &p_serv, &p_char);

            if ((NULL != p_serv) && (NULL != p_char))
            {
                /* Characteristic */
                if (((p_char->start_hdl + 1) == p_write_rsp_evt_param->attr_hdl) && (NULL != p_char->decode))
                {
                    ble_status_t ret;

                    void *p_app_value;
                    p_app_value = malloc(p_char->app_size);
                    if(NULL == p_app_value)
                    {
                        ret = BLE_ERR_MEM_ALLOC_FAILED;
                    }
                    else
                    {
                        ret = R_BLE_SERVS_GetChar(p_char, p_data->conn_hdl, p_app_value);
                    }

                    if (NULL != p_char->write_comp_cb)
                    {
                        p_char->write_comp_cb(p_char, p_data->conn_hdl, ret, p_app_value);
                    }
                    else
                    {
                        st_ble_servs_evt_data_t evt_data = {
                            .conn_hdl  = p_data->conn_hdl,
                            .param_len = (NULL != p_app_value)?p_char->app_size:0,
                            .p_param   = p_app_value,
                        };
                        if (NULL != p_serv->cb)
                        {
                            p_serv->cb(BLE_SERVS_MULTI_ATTR_EVENT(p_char->char_idx, p_char->inst_idx, BLE_SERVS_WRITE_COMP), ret, &evt_data);
                        }
                    }

                    if (NULL != p_app_value)
                    {
                        free(p_app_value);
                    }
                }
                /* Descriptor */
                else
                {
                    if (NULL != p_char->pp_descs)
                    {
                        for (uint8_t d = 0; d < p_char->num_of_descs; d++)
                        {
                            if ((p_char->pp_descs[d]->attr_hdl == p_write_rsp_evt_param->attr_hdl) &&
                                (NULL != p_char->pp_descs[d]->decode))
                            {
                                ble_status_t ret;

                                void *p_app_value;
                                p_app_value = malloc(p_char->pp_descs[d]->app_size);
                                if(NULL == p_app_value)
                                {
                                    ret = BLE_ERR_MEM_ALLOC_FAILED;
                                }
                                else
                                {
                                    st_ble_gatt_value_t gatt_value;
                                    R_BLE_GATTS_GetAttr(p_data->conn_hdl, p_write_rsp_evt_param->attr_hdl, &gatt_value);

                                    ret = p_char->pp_descs[d]->decode(p_app_value, &gatt_value);
                                }

                                if (NULL != p_char->pp_descs[d]->write_comp_cb)
                                {
                                    p_char->pp_descs[d]->write_comp_cb(&p_char->pp_descs[d], p_data->conn_hdl, ret, p_app_value);
                                }
                                else
                                {
                                    st_ble_servs_evt_data_t evt_data = {
                                        .conn_hdl  = p_data->conn_hdl,
                                        .param_len = (NULL != p_app_value)?p_char->pp_descs[d]->app_size:0,
                                        .p_param   = p_app_value,
                                    };
                                    if (NULL != p_serv->cb)
                                    {
                                        p_serv->cb(BLE_SERVS_MULTI_ATTR_EVENT(p_char->pp_descs[d]->desc_idx, p_char->pp_descs[d]->inst_idx, BLE_SERVS_WRITE_COMP), ret, &evt_data);
                                    }
                                }
                                if (NULL != p_app_value)
                                {
                                    free(p_app_value);
                                }
                                break;
                            }
                        }
                    }
                }
            }
        } break;

        case BLE_GATTS_EVENT_PREPARE_WRITE_RSP_COMP:
        {
             st_ble_gatts_prepare_write_rsp_evt_t *p_prepare_write_evt_param =
                (st_ble_gatts_prepare_write_rsp_evt_t *)p_data->p_param;

             s_write_long_attr_hdl = p_prepare_write_evt_param->attr_hdl;
        } break;

        case BLE_GATTS_EVENT_EXE_WRITE_RSP_COMP:
        {
            st_ble_gatts_exe_write_rsp_evt_t *p_exe_write_evt_param =
                (st_ble_gatts_exe_write_rsp_evt_t *)p_data->p_param;

            const st_ble_servs_info_t       *p_serv = NULL;
            const st_ble_servs_char_info_t  *p_char = NULL;

            find_attr(s_write_long_attr_hdl, &p_serv, &p_char);

            if ((NULL != p_serv) && (NULL != p_char))
            {
                if (BLE_GATTC_EXECUTE_WRITE_EXEC_FLAG == p_exe_write_evt_param->exe_flag)
                {
                    /* Characteristic */
                    if (((p_char->start_hdl + 1) == s_write_long_attr_hdl) && (NULL != p_char->decode))
                    {
                        ble_status_t ret;

                        void *p_app_value;
                        p_app_value = malloc(p_char->app_size);
                        if(NULL == p_app_value)
                        {
                            ret = BLE_ERR_MEM_ALLOC_FAILED;
                        }
                        else
                        {
                            ret = R_BLE_SERVS_GetChar(p_char, p_data->conn_hdl, p_app_value);
                        }
                        if (NULL != p_char->write_comp_cb)
                        {
                            p_char->write_comp_cb(p_char, p_data->conn_hdl, ret, p_app_value);
                        }
                        else
                        {
                            st_ble_servs_evt_data_t evt_data = {
                                .conn_hdl  = p_data->conn_hdl,
                                .param_len = (NULL != p_app_value)?p_char->app_size:0,
                                .p_param   = p_app_value,
                            };
                            if (NULL != p_serv->cb)
                            {
                                p_serv->cb(BLE_SERVS_MULTI_ATTR_EVENT(p_char->char_idx, p_char->inst_idx, BLE_SERVS_WRITE_COMP), ret, &evt_data);
                            }
                        }
                        if (NULL != p_app_value)
                        {
                            free(p_app_value);
                        }
                        s_write_long_attr_hdl = BLE_GATT_INVALID_ATTR_HDL_VAL;
                    }
                    /* Descriptor */
                    else
                    {
                        if (NULL != p_char->pp_descs)
                        {
                            for (uint8_t d = 0; d < p_char->num_of_descs; d++)
                            {
                                if ((p_char->pp_descs[d]->attr_hdl == s_write_long_attr_hdl) &&
                                    (NULL != p_char->pp_descs[d]->decode))
                                {
                                    ble_status_t ret;

                                    void *p_app_value = NULL;
                                    p_app_value = malloc(p_char->pp_descs[d]->app_size);
                                    if(NULL == p_app_value)
                                    {
                                        ret = BLE_ERR_MEM_ALLOC_FAILED;
                                    }
                                    else
                                    {
                                        st_ble_gatt_value_t gatt_value;
                                        ret = R_BLE_GATTS_GetAttr(p_data->conn_hdl, s_write_long_attr_hdl, &gatt_value);

                                        if (BLE_SUCCESS == ret)
                                        {
                                            ret = p_char->pp_descs[d]->decode(p_app_value, &gatt_value);
                                        }
                                    }

                                    if (NULL != p_char->pp_descs[d]->write_comp_cb)
                                    {
                                        p_char->pp_descs[d]->write_comp_cb(&p_char->pp_descs[d], p_data->conn_hdl, ret, p_app_value);
                                    }
                                    else
                                    {
                                        st_ble_servs_evt_data_t evt_data = {
                                            .conn_hdl  = p_data->conn_hdl,
                                            .param_len = (NULL != p_app_value)?p_char->pp_descs[d]->app_size:0,
                                            .p_param   = p_app_value,
                                        };
                                        if (NULL != p_serv->cb)
                                        {
                                            p_serv->cb(BLE_SERVS_MULTI_ATTR_EVENT(p_char->pp_descs[d]->desc_idx, p_char->pp_descs[d]->inst_idx, BLE_SERVS_WRITE_COMP), ret, &evt_data);
                                        }
                                    }

                                    if (NULL != p_app_value)
                                    {
                                        free(p_app_value);
                                    }
                                    s_write_long_attr_hdl = BLE_GATT_INVALID_ATTR_HDL_VAL;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        } break;

        case BLE_GATTS_EVENT_READ_RSP_COMP:
        case BLE_GATTS_EVENT_READ_BY_TYPE_RSP_COMP:
        case BLE_GATTS_EVENT_READ_BLOB_RSP_COMP:
        case BLE_GATTS_EVENT_READ_MULTI_RSP_COMP:
        case BLE_GATTS_EVENT_INVALID:
        default:
        {
        } break;
    }
}

void R_BLE_SERVS_VsCb(uint16_t type, ble_status_t result, st_ble_vs_evt_data_t * p_data)
{
    /* unused arg */
    (void)result;

    switch (type)
    {
        case BLE_VS_EVENT_TX_FLOW_STATE_CHG:
        {
            st_ble_vs_tx_flow_chg_evt_t *p_tx_flow_chg_param =
                (st_ble_vs_tx_flow_chg_evt_t *)p_data->p_param;

            uint32_t buffer_num;
            R_BLE_VS_GetTxBufferNum(&buffer_num);

            if (BLE_VS_TX_FLOW_CTL_ON == p_tx_flow_chg_param->state)
            {
                for (uint8_t s = 0; s < gs_num_of_servs; s++)
                {
                    for (uint8_t c = 0; c < gs_servs[s]->num_of_chars; c++)
                    {
                        if (NULL != gs_servs[s]->pp_chars[c]->flow_ctrl_cb)
                        {
                            gs_servs[s]->pp_chars[c]->flow_ctrl_cb(gs_servs[s]->pp_chars[c]);
                        }
                    }
                }
            }
        } break;

        default:
        {
            /* Do nothing */
        } break;
    }
}

ble_status_t R_BLE_SERVS_Init(void)
{
    R_BLE_VS_StartTxFlowEvtNtf();
    gs_num_of_servs = 0;

    return BLE_SUCCESS;
}

ble_status_t R_BLE_SERVS_RegisterServer(const st_ble_servs_info_t *p_info)
{
    ble_status_t ret;
    if( BLE_PRF_MAX_NUM_OF_SERVS > gs_num_of_servs )
    {
        gs_servs[gs_num_of_servs++] = p_info;
        ret = BLE_SUCCESS;
    }
    else
    {
        ret = BLE_ERR_CONTEXT_FULL;
    }

    return ret;
}

ble_status_t R_BLE_SERVS_SendHdlVal(const st_ble_servs_char_info_t *p_attr, uint16_t conn_hdl,  const void *p_app_value, bool is_notify)
{
    ble_status_t ret;

    if ((NULL == p_attr) || (NULL == p_attr->encode) || (NULL == p_attr->pp_descs))
    {
        return BLE_ERR_INVALID_PTR;
    }

    if (NULL == p_app_value)
    {
        return BLE_ERR_INVALID_ARG;
    }

    if (BLE_GAP_INVALID_CONN_HDL == conn_hdl)
    {
        return BLE_ERR_INVALID_HDL;
    }

    /* Check CCCD */
    uint16_t cccd = 0;
    R_BLE_SERVS_GetDesc(p_attr->pp_descs[0], conn_hdl, &cccd);

    void *p_gatt_value = malloc(p_attr->db_size);
    if(NULL == p_gatt_value)
    {
        return BLE_ERR_MEM_ALLOC_FAILED;
    }

    st_ble_gatt_hdl_value_pair_t hdl_val_data = {
        .attr_hdl        = (uint16_t)(p_attr->start_hdl + 1),
        .value.value_len = p_attr->db_size,
        .value.p_value   = (uint8_t *)p_gatt_value,
    };

    ret = p_attr->encode(p_app_value, &hdl_val_data.value);

    if (BLE_SUCCESS == ret)
    {
        if ((is_notify == true) && (cccd & BLE_GATTS_CLI_CNFG_NOTIFICATION))
        {
            ret = R_BLE_GATTS_Notification(conn_hdl, &hdl_val_data);
        }
        else if ((is_notify == false) && (cccd & BLE_GATTS_CLI_CNFG_INDICATION))
        {
            ret = R_BLE_GATTS_Indication(conn_hdl, &hdl_val_data);
        }
        else 
        {
            ret = BLE_ERR_INVALID_OPERATION;
        }
    }

    free(p_gatt_value);

    return ret;
}

ble_status_t R_BLE_SERVS_GetChar(const st_ble_servs_char_info_t *p_attr, uint16_t conn_hdl, void *p_app_value)
{
    ble_status_t ret;

    if (NULL == p_app_value)
    {
        return BLE_ERR_INVALID_ARG;
    }

    if ((NULL == p_attr) || (NULL == p_attr->decode))
    {
        return BLE_ERR_INVALID_PTR;
    }

    st_ble_gatt_value_t gatt_value;

    ret = R_BLE_GATTS_GetAttr(conn_hdl, (uint16_t)(p_attr->start_hdl + 1), &gatt_value);

    if (BLE_SUCCESS == ret)
    {
        ret = p_attr->decode(p_app_value, &gatt_value);
    }

    return ret;
}

ble_status_t R_BLE_SERVS_SetChar(const st_ble_servs_char_info_t *p_attr, uint16_t conn_hdl, const void *p_app_value)
{
    ble_status_t ret;

    if (NULL == p_app_value)
    {
        return BLE_ERR_INVALID_ARG;
    }

    if ((NULL == p_attr) || (NULL == p_attr->encode))
    {
        return BLE_ERR_INVALID_PTR;
    }

    void *p_gatt_value = malloc(p_attr->db_size);
    if(NULL == p_gatt_value)
    {
        return BLE_ERR_MEM_ALLOC_FAILED;
    }

    st_ble_gatt_value_t gatt_value = {
        .p_value   = p_gatt_value,
        .value_len = p_attr->db_size,
    };

    ret = p_attr->encode(p_app_value, &gatt_value);

    if (BLE_SUCCESS == ret)
    {
        ret = R_BLE_GATTS_SetAttr(conn_hdl, (uint16_t)(p_attr->start_hdl + 1), &gatt_value);
    }

    free(p_gatt_value);

    return ret;
}

ble_status_t R_BLE_SERVS_GetDesc(const st_ble_servs_desc_info_t *p_attr, uint16_t conn_hdl, void *p_app_value)
{
    ble_status_t ret;

    if (NULL == p_app_value)
    {
        return BLE_ERR_INVALID_ARG;
    }

    if ((NULL == p_attr) || (NULL == p_attr->decode))
    {
        return BLE_ERR_INVALID_PTR;
    }

    st_ble_gatt_value_t gatt_value;

    ret = R_BLE_GATTS_GetAttr(conn_hdl, p_attr->attr_hdl, &gatt_value);

    if (BLE_SUCCESS == ret)
    {
        ret = p_attr->decode(p_app_value, &gatt_value);
    }

    return ret;
}

ble_status_t R_BLE_SERVS_SetDesc(const st_ble_servs_desc_info_t *p_attr, uint16_t conn_hdl, const void *p_app_value)
{
    ble_status_t ret;

    if (NULL == p_app_value)
    {
        return BLE_ERR_INVALID_ARG;
    }

    if ((NULL == p_attr) || (NULL == p_attr->encode))
    {
        return BLE_ERR_INVALID_PTR;
    }

    void *p_gatt_value = malloc(p_attr->db_size);
    if(NULL == p_gatt_value)
    {
        return BLE_ERR_MEM_ALLOC_FAILED;
    }

    st_ble_gatt_value_t gatt_value = {
        .p_value   = p_gatt_value,
        .value_len = p_attr->db_size,
    };

    ret = p_attr->encode(p_app_value, &gatt_value);

    if (BLE_SUCCESS == ret)
    {
        ret = R_BLE_GATTS_SetAttr(conn_hdl, p_attr->attr_hdl, &gatt_value);
    }

    free(p_gatt_value);

    return ret;
}
