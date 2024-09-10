/**********************************************************************************************************************
* Copyright (c) 2018 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/

#include <string.h>
#include <stdlib.h>

#include "r_ble_cli.h"
#include "r_ble_cmd.h"
#include "r_ble_cmd_gatt.h"

#if (BLE_CFG_CMD_LINE_EN == 1)

#define BLE_LOG_TAG "gatts"
#include "logger/r_ble_logger.h"

#define pf R_BLE_CLI_Printf

static uint16_t gs_cur_attr_hdl;

/*----------------------------------------------------------------------------------------------------------------------
    gattc read command
----------------------------------------------------------------------------------------------------------------------*/
static void exec_gattc_read(int argc, char *argv[])
{
    if (argc != 3)
    {
        pf("gattc %s: unrecognized operands\n", argv[0]);
        return;
    }

    uint16_t conn_hdl;
    conn_hdl = (uint16_t)strtol(argv[1], NULL, 0);

    uint16_t attr_hdl;
    attr_hdl = (uint16_t)strtol(argv[2], NULL, 0);

    ble_status_t ret;
    ret = R_BLE_GATTC_ReadChar(conn_hdl, attr_hdl);
    if (ret == BLE_SUCCESS)
    {
        gs_cur_attr_hdl = attr_hdl;
    }
    else 
    {
        pf("gattc %s: failed with 0x%04X\n", argv[0], ret);
    }
}

static void abort_gattc_read(void)
{
    /* Just wait for read response event. */
}

static const st_ble_cli_cmd_t gattc_read_cmd =
{
    .p_name = "read",
    .exec   = exec_gattc_read,
    .abort  = abort_gattc_read,
    .p_help = "Usage: gattc read conn_hdl attr_hdl\n"
               "Read char value from remote GATT db",
};


/*----------------------------------------------------------------------------------------------------------------------
    gattc wwr command
----------------------------------------------------------------------------------------------------------------------*/
static void exec_gattc_wwr(int argc, char *argv[])
{
    if (argc != 4)
    {
        pf("gattc %s: unrecognized operands\n", argv[0]);
        return;
    }

    uint16_t conn_hdl;
    conn_hdl = (uint16_t)strtol(argv[1], NULL, 0);

    uint16_t attr_hdl;
    attr_hdl = (uint16_t)strtol(argv[2], NULL, 0);

    uint8_t value[BLE_CMD_ATTR_VAL_MAX_LEN] = { 0 };
    st_ble_gatt_hdl_value_pair_t write_data = {
        .attr_hdl = attr_hdl,
        .value = {
            .value_len = 0,
            .p_value = value,
        },
    };

    R_BLE_CMD_ParseValues(argv[3], value, &write_data.value.value_len);

    ble_status_t ret;
    ret = R_BLE_GATTC_WriteCharWithoutRsp(conn_hdl, &write_data);
    if (ret != BLE_SUCCESS)
    {
        pf("gattc %s: failed with 0x%04X\n", argv[0], ret);
    }
}

static const st_ble_cli_cmd_t gattc_wwr_cmd =
{
    .p_name = "wwr",
    .exec   = exec_gattc_wwr,
    .p_help = "Usage: gattc wwr conn_hdl attr_hdl values\n"
              "Write char value to remote GATT db",
};

/*----------------------------------------------------------------------------------------------------------------------
    gattc write command
----------------------------------------------------------------------------------------------------------------------*/
static void exec_gattc_write(int argc, char *argv[])
{
    if (argc != 4)
    {
        pf("gattc %s: unrecognized operands\n", argv[0]);
        return;
    }

    uint16_t conn_hdl;
    conn_hdl = (uint16_t)strtol(argv[1], NULL, 0);

    uint16_t attr_hdl;
    attr_hdl = (uint16_t)strtol(argv[2], NULL, 0);

    uint8_t value[BLE_CMD_ATTR_VAL_MAX_LEN] = { 0 };
    st_ble_gatt_hdl_value_pair_t write_data = {
        .attr_hdl = attr_hdl,
        .value = {
            .value_len = 0,
            .p_value = value,
        },
    };

    R_BLE_CMD_ParseValues(argv[3], value, &write_data.value.value_len);

    ble_status_t ret;
    ret = R_BLE_GATTC_WriteChar(conn_hdl, &write_data);
    if (ret != BLE_SUCCESS)
    {
        pf("gattc %s: failed with 0x%04X\n", argv[0], ret);
    }
}

static const st_ble_cli_cmd_t gattc_write_cmd =
{
    .p_name = "write",
    .exec   = exec_gattc_write,
    .p_help = "Usage: gattc write conn_hdl attr_hdl values\n"
              "Write char value to remote GATT db without response",
};

/*----------------------------------------------------------------------------------------------------------------------
    gattc command
----------------------------------------------------------------------------------------------------------------------*/

static const st_ble_cli_cmd_t * const gattc_sub_cmds[] =
{
    &gattc_read_cmd,
    &gattc_wwr_cmd,
    &gattc_write_cmd,
};

const st_ble_cli_cmd_t g_gattc_cmd =
{
    .p_name      = "gattc",
    .p_cmds      = gattc_sub_cmds,
    .num_of_cmds = sizeof(gattc_sub_cmds) / sizeof(gattc_sub_cmds[0]),
    .p_help      = "Sub Command: read, wwr, write\n"
                   "Try 'gattc sucmd help' for more information",
};

/*----------------------------------------------------------------------------------------------------------------------
    gatts set command
----------------------------------------------------------------------------------------------------------------------*/
static void exec_gatts_set(int argc, char *argv[])
{
    if (argc != 3)
    {
        pf("gatts %s: unrecognized operands\n", argv[0]);
        return;
    }

    uint8_t value[BLE_CMD_ATTR_VAL_MAX_LEN] = { 0 };
    st_ble_gatt_value_t gatt_value = {
        .value_len = 0,
        .p_value  = value,
    };

    uint16_t attr_hdl;
    attr_hdl = (uint16_t)strtol(argv[1], NULL, 0);
    R_BLE_CMD_ParseValues(argv[2], gatt_value.p_value, &gatt_value.value_len);

    ble_status_t ret;
    ret = R_BLE_GATTS_SetAttr(BLE_GAP_INVALID_CONN_HDL, attr_hdl, &gatt_value);
    if (ret != BLE_SUCCESS)
    {
        pf("gatts %s: failed with 0x%04X\n", argv[0], ret);
    }
}

static const st_ble_cli_cmd_t gatts_set_cmd =
{
    .p_name = "set",
    .exec   = exec_gatts_set,
    .p_help = "Usage: gatts set attr_hdl values\n"
              "Set local GATT db char value",
};

/*----------------------------------------------------------------------------------------------------------------------
    gatts get command
----------------------------------------------------------------------------------------------------------------------*/
static void exec_gatts_get(int argc, char *argv[])
{
    if (argc != 2)
    {
        pf("gatts %s: unrecognized operands\n", argv[0]);
        return;
    }

    uint16_t attr_hdl;
    attr_hdl = (uint16_t)strtol(argv[1], NULL, 0);

    st_ble_gatt_value_t gatt_value;

    ble_status_t ret;
    ret = R_BLE_GATTS_GetAttr(BLE_GAP_INVALID_CONN_HDL, attr_hdl, &gatt_value);
    if (ret == BLE_SUCCESS)
    {
        R_BLE_CMD_PrintValues(gatt_value.p_value, gatt_value.value_len);
        pf("\n");
    }
    else
    {
        pf("gatts %s: failed with 0x%04X\n", argv[0], ret);
    }
}

static const st_ble_cli_cmd_t gatts_get_cmd =
{
    .p_name = "get",
    .exec   = exec_gatts_get,
    .p_help = "Usage: gatts get attr_hdl\n"
              "Get char value from local GATT db",
};

/*----------------------------------------------------------------------------------------------------------------------
    gatts notify command
----------------------------------------------------------------------------------------------------------------------*/
static void exec_gatts_notify(int argc, char *argv[])
{
    if (argc != 4)
    {
        pf("gatts %s: unrecognized operands\n", argv[0]);
        return;
    }

    uint8_t value[BLE_CMD_ATTR_VAL_MAX_LEN] = { 0 };
    st_ble_gatt_hdl_value_pair_t ntf_data = {
        .attr_hdl = 0x0000,
        .value = {
            .p_value = value,
            .value_len = 0,
        },
    };

    uint16_t conn_hdl;
    conn_hdl = (uint16_t)strtol(argv[1], NULL, 0);

    ntf_data.attr_hdl = (uint16_t)strtol(argv[2], NULL, 0);

    R_BLE_CMD_ParseValues(argv[3], ntf_data.value.p_value, &ntf_data.value.value_len);

    ble_status_t ret;
    ret = R_BLE_GATTS_Notification(conn_hdl, &ntf_data);
    if (ret != BLE_SUCCESS)
    {
        pf("gatts %s: failed with 0x%04X\n", argv[0], ret);
    }
}

static const st_ble_cli_cmd_t gatts_notify_cmd =
{
    .p_name = "notify",
    .exec   = exec_gatts_notify,
    .p_help = "Usage: gatts notfiy conn_hdl attr_hdl values\n"
              "Send notification",
};

/*----------------------------------------------------------------------------------------------------------------------
    gatts indicate command
----------------------------------------------------------------------------------------------------------------------*/
static void exec_gatts_indicate(int argc, char *argv[])
{
    if (argc != 4)
    {
        pf("gatts %s: unrecognized operands\n", argv[0]);
        return;
    }

    uint8_t value[BLE_CMD_ATTR_VAL_MAX_LEN] = { 0 };
    st_ble_gatt_hdl_value_pair_t ind_data = {
        .attr_hdl = 0x0000,
        .value = {
            .p_value = value,
            .value_len = 0,
        },
    };

    uint16_t conn_hdl;
    conn_hdl = (uint16_t)strtol(argv[1], NULL, 0);

    ind_data.attr_hdl = (uint16_t)strtol(argv[2], NULL, 0);

    R_BLE_CMD_ParseValues(argv[3], ind_data.value.p_value, &ind_data.value.value_len);

    ble_status_t ret;
    ret = R_BLE_GATTS_Indication(conn_hdl, &ind_data);
    if (ret != BLE_SUCCESS)
    {
        pf("gatts %s: failed with 0x%04X\n", argv[0], ret);
    }
}

static const st_ble_cli_cmd_t gatts_indicate_cmd =
{
    .p_name = "indicate",
    .exec   = exec_gatts_indicate,
    .p_help = "Usage: gatts indicate conn_hdl attr_hdl values\n"
              "Send indication",
};


/*----------------------------------------------------------------------------------------------------------------------
    gatts command
----------------------------------------------------------------------------------------------------------------------*/
static const st_ble_cli_cmd_t * const gatts_sub_cmds[] =
{
    &gatts_get_cmd,
    &gatts_set_cmd,
    &gatts_notify_cmd,
    &gatts_indicate_cmd,
};

const st_ble_cli_cmd_t g_gatts_cmd =
{
    .p_name      = "gatts",
    .p_cmds      = gatts_sub_cmds,
    .num_of_cmds = sizeof(gatts_sub_cmds)/sizeof(gatts_sub_cmds[0]),
    .p_help      = "Sub Command: set, get, notify, indicate\n"
                   "Try 'gatts sucmd help' for more information",
};

void R_BLE_CMD_GattcCb(uint16_t type, ble_status_t result, st_ble_gattc_evt_data_t *data)
{
    FSP_PARAMETER_NOT_USED(result);
    switch (type)
    {
        case BLE_GATTC_EVENT_CHAR_READ_RSP:
        {
            st_ble_gattc_rd_char_evt_t *rd_char_evt_param =
                (st_ble_gattc_rd_char_evt_t *)data->p_param;

            st_ble_gatt_hdl_value_pair_t *hdl_value_pair = &rd_char_evt_param->read_data;

            if (hdl_value_pair->attr_hdl == gs_cur_attr_hdl)
            {
                R_BLE_CMD_PrintValues(hdl_value_pair->value.p_value, hdl_value_pair->value.value_len);
                pf("\n");
                R_BLE_CLI_SetCmdComp();
                gs_cur_attr_hdl = BLE_GATT_INVALID_ATTR_HDL_VAL;
            }
        } break;

        case BLE_GATTC_EVENT_CHAR_WRITE_RSP:
        {
	    /*
            st_ble_gatt_wr_char_rsp_evt_t *wr_char_rsp_evt_param =
                (st_ble_gatt_wr_char_rsp_evt_t *)data->event_param;

            wr_char_rsp_evt_param->value_hdl;
	    */
        } break;

        case BLE_GATTC_EVENT_ERROR_RSP:
        {
            st_ble_gattc_err_rsp_evt_t *err_rsp_evt_param =
                (st_ble_gattc_err_rsp_evt_t *)data->p_param;

            pf("gattc: error response received. opcode=0x%04X, attr_hdl:0x%04X, rsp=0x%04X\n",
               err_rsp_evt_param->op_code, err_rsp_evt_param->attr_hdl, err_rsp_evt_param->rsp_code);
        } break;
    }
}

static void ble_gatt_db_cb(uint16_t conn_hdl, st_ble_gatts_db_params_t *params)
{
    FSP_PARAMETER_NOT_USED(conn_hdl);

    switch (params->db_op)
    {
        case BLE_GATTS_OP_CHAR_PEER_READ_REQ:
        {
        } break;

        case BLE_GATTS_OP_CHAR_PEER_WRITE_REQ:
        {
        } break;

        case BLE_GATTS_OP_CHAR_PEER_WRITE_CMD:
        {
        } break;

        case BLE_GATTS_OP_CHAR_PEER_CLI_CNFG_READ_REQ:
        {
        } break;

        case BLE_GATTS_OP_CHAR_PEER_CLI_CNFG_WRITE_REQ:
        {
        } break;
    }
}

void R_BLE_CMD_GattsCb(uint16_t type, ble_status_t result, st_ble_gatts_evt_data_t *data)
{
    FSP_PARAMETER_NOT_USED(result);

    switch (type)
    {
        case BLE_GATTS_EVENT_HDL_VAL_CNF:
        {
        } break;

        case BLE_GATTS_EVENT_DB_ACCESS_IND:
        {
            st_ble_gatts_db_access_evt_t *db_access_evt_param =
                (st_ble_gatts_db_access_evt_t *)data->p_param;
            ble_gatt_db_cb(data->conn_hdl, db_access_evt_param->p_params);
        } break;
    }
}

#endif /* (BLE_CFG_CMD_LINE_EN == 1) */
