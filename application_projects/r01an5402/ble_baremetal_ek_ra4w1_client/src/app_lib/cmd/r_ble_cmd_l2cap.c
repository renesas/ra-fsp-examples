/**********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/
#include <string.h>
#include <stdlib.h>

#include "r_ble_cli.h"
#include "r_ble_cmd.h"
#include "r_ble_cmd_l2cap.h"

#if (BLE_CFG_CMD_LINE_EN == 1)

#define BLE_LOG_TAG "l2cap_cmd"
#include "logger/r_ble_logger.h"

#define pf R_BLE_CLI_Printf

static uint8_t gs_l2cap_data[251];


static ble_status_t l2cap_service_conn_hdlr(st_ble_l2cap_cf_conn_evt_t * p_conn_evt, uint16_t conn_hdl);

static void exec_l2cap_psm(int argc, char *argv[]);
static void exec_l2cap_conn(int argc, char *argv[]);
static void exec_l2cap_credit(int argc, char *argv[]);
static void exec_l2cap_data(int argc, char *argv[]);

static void l2cap_app_cb(uint16_t event_type, ble_status_t event_result, st_ble_l2cap_cf_evt_data_t * p_event_data);


/*----------------------------------------------------------------------------------------------------------------------
    l2cap psm command
----------------------------------------------------------------------------------------------------------------------*/
static void exec_l2cap_psm(int argc, char *argv[])
{
    if (strcmp(argv[1], "reg") == 0)
    {
        if (argc != 4)
        {
            pf("l2cap %s: unrecognized operands\n", argv[1]);
            return;
        }
        
        uint16_t psm, lwm;
        psm = (uint16_t)strtol(argv[2], NULL, 0);
        lwm = (uint16_t)strtol(argv[3], NULL, 0);
        R_BLE_CMD_RegisterL2capPSM(psm, lwm);
    }
    else if (strcmp(argv[1], "dereg") == 0)
    {
        if (argc != 3)
        {
            pf("l2cap %s: unrecognized operands\n", argv[1]);
            return;
        }

        uint16_t psm;
        psm = (uint16_t)strtol(argv[2], NULL, 0);
        R_BLE_L2CAP_DeregisterCfPsm(psm);
    }
    else
    {
        pf("l2cap %s: unrecognized operand '%s'\n", argv[0], argv[1]);
    }
}

static const st_ble_cli_cmd_t l2cap_psm_cmd =
{
    .p_name = "psm",
    .exec   = exec_l2cap_psm,
    .p_help = "Usage: l2cap psm (reg|dereg) psm (lwm: reg only) \n"
              "Register or Deregister l2cap credit based service.",
};

/*----------------------------------------------------------------------------------------------------------------------
    l2cap connect command
----------------------------------------------------------------------------------------------------------------------*/
static void exec_l2cap_conn(int argc, char *argv[])
{
    if (strcmp(argv[1], "req") == 0)
    {
        if (argc != 6)
        {
            pf("l2cap %s: unrecognized operands\n", argv[1]);
            return;
        }
        
        uint16_t conn_hdl, psm, mtu, credit;
        conn_hdl = (uint16_t)strtol(argv[2], NULL, 0);
        psm = (uint16_t)strtol(argv[3], NULL, 0);
        mtu = (uint16_t)strtol(argv[4], NULL, 0);
        credit = (uint16_t)strtol(argv[5], NULL, 0);
        R_BLE_CMD_RequestL2capConn(conn_hdl, psm, mtu, credit);
    }
    else if (strcmp(argv[1], "rsp") == 0)
    {
        if (argc != 6)
        {
            pf("l2cap %s: unrecognized operands\n", argv[1]);
            return;
        }
        uint16_t lcid, response, mtu, credit;
        lcid = (uint16_t)strtol(argv[2], NULL, 0);
        response = (uint16_t)strtol(argv[3], NULL, 0);
        mtu = (uint16_t)strtol(argv[4], NULL, 0);
        credit = (uint16_t)strtol(argv[5], NULL, 0);
        R_BLE_CMD_ReplyL2capConn(lcid, response, mtu, credit);
    }
    else if (strcmp(argv[1], "disconn") == 0)
    {
        if (argc != 3)
        {
            pf("l2cap %s: unrecognized operands\n", argv[1]);
            return;
        }
        uint16_t lcid;
        lcid = (uint16_t)strtol(argv[2], NULL, 0);
        R_BLE_L2CAP_DisconnectCf(lcid);
    }
    else
    {
        pf("l2cap %s: unrecognized operand '%s'\n", argv[0], argv[1]);
    }
}

static const st_ble_cli_cmd_t l2cap_conn_cmd =
{
    .p_name = "conn",
    .exec   = exec_l2cap_conn,
    .p_help = "Usage: l2cap conn (req|rsp|disconn) ... \n"
              "Connect req or Connect rsp or Disconnect l2cap credit based service.",
};



/*----------------------------------------------------------------------------------------------------------------------
    l2cap credit command
----------------------------------------------------------------------------------------------------------------------*/
static void exec_l2cap_credit(int argc, char *argv[])
{
    if (strcmp(argv[1], "send") == 0)
    {
        if (argc != 4)
        {
            pf("l2cap %s: unrecognized operands\n", argv[1]);
            return;
        }
        
        uint16_t lcid, credit;
        lcid = (uint16_t)strtol(argv[2], NULL, 0);
        credit = (uint16_t)strtol(argv[3], NULL, 0);
        R_BLE_L2CAP_SendCfCredit(lcid, credit);
    }
    else
    {
        pf("l2cap %s: unrecognized operand '%s'\n", argv[0], argv[1]);
    }
}

static const st_ble_cli_cmd_t l2cap_credit_cmd =
{
    .p_name = "credit",
    .exec   = exec_l2cap_credit,
    .p_help = "Usage: l2cap credit send credit \n"
              "Send credit for l2cap credit based service.",
};

/*----------------------------------------------------------------------------------------------------------------------
    l2cap data send
----------------------------------------------------------------------------------------------------------------------*/
static void exec_l2cap_data(int argc, char *argv[])
{
    if (strcmp(argv[1], "send") == 0)
    {
        if (argc != 5)
        {
            pf("l2cap %s: unrecognized operands\n", argv[1]);
            return;
        }
        
        uint16_t conn_hdl, lcid, data_len;
        conn_hdl = (uint16_t)strtol(argv[2], NULL, 0);
        lcid = (uint16_t)strtol(argv[3], NULL, 0);
        data_len = (uint16_t)strtol(argv[4], NULL, 0);
        memset(gs_l2cap_data, 0x01, data_len);
        R_BLE_L2CAP_SendCfData(conn_hdl, lcid, data_len, gs_l2cap_data);
    }
    else
    {
        pf("l2cap %s: unrecognized operand '%s'\n", argv[0], argv[1]);
    }
}

static const st_ble_cli_cmd_t l2cap_data_cmd =
{
    .p_name = "data",
    .exec   = exec_l2cap_data,
    .p_help = "Usage: l2cap data send conn_hdl lcid data_len \n"
              "Send credit for l2cap credit based service.",
};


/*----------------------------------------------------------------------------------------------------------------------
    l2cap command
----------------------------------------------------------------------------------------------------------------------*/
static const st_ble_cli_cmd_t * const l2cap_sub_cmds[] =
{
    &l2cap_psm_cmd,
    &l2cap_conn_cmd,
    &l2cap_credit_cmd,
    &l2cap_data_cmd
};

const st_ble_cli_cmd_t g_l2cap_cmd =
{
    .p_name      = "l2cap",
    .p_cmds      = l2cap_sub_cmds,
    .num_of_cmds = sizeof(l2cap_sub_cmds)/sizeof(l2cap_sub_cmds[0]),
    .p_help      = "Sub Command: psm, conn, credit, data\n"
                   "Try 'l2cap subcmd help' for more information",
};

void R_BLE_CMD_RegisterL2capPSM(uint16_t psm, uint16_t lwm)
{
    ble_status_t retval;
    retval = R_BLE_L2CAP_RegisterCfPsm(l2cap_app_cb, psm, lwm);
    pf("R_BLE_L2CAP_RegisterCfPsm() retval : 0x%04x\n", retval);
}

void R_BLE_CMD_RequestL2capConn(uint16_t conn_hdl, uint16_t psm, uint16_t mtu, uint16_t credit)
{
    st_ble_l2cap_conn_req_param_t conn_req = {
        .local_psm = psm,
        .remote_psm = psm,
        .mtu = mtu,
        .mps = mtu,
        .credit = credit
    };

    ble_status_t retval;
    retval = R_BLE_L2CAP_ReqCfConn(conn_hdl, &conn_req);
    pf("R_BLE_L2CAP_ReqCfConn() retval : 0x%04x\n", retval);
}

void R_BLE_CMD_ReplyL2capConn(uint16_t lcid, uint16_t response, uint16_t mtu, uint16_t credit)
{
    st_ble_l2cap_conn_rsp_param_t conn_rsp = {
        .lcid = lcid,
        .response = response,
        .mtu = mtu,
        .mps = mtu,
        .credit = credit
    };
    ble_status_t retval;
    retval = R_BLE_L2CAP_RspCfConn(&conn_rsp);
    pf("R_BLE_L2CAP_RspCfConn() retval : 0x%04x\n", retval);
}

static void l2cap_app_cb(uint16_t event_type, ble_status_t event_result, st_ble_l2cap_cf_evt_data_t * p_event_data)
{
#if 0
    ble_status_t retval;

    retval = BLE_SUCCESS;
#endif
    pf ("result : 0x%04x.\n", event_result);

    switch(event_type)
    {
        case BLE_L2CAP_EVENT_CF_CONN_CNF:
            pf ("Received BLE_L2CAP_EVENT_CF_CONN_CNF\n");
            {
#if 0
                st_ble_l2cap_cf_conn_evt_t * p_conn_param;
                p_conn_param = (st_ble_l2cap_cf_conn_evt_t *)p_event_data->p_param;
#endif
                pf ("event_result : 0x%04x\n", event_result);
                //pf ("cid : 0x%04x\n", p_conn_param->cid);
                //pf ("psm : 0x%04x\n", p_conn_param->psm);
                //pf ("mtu : 0x%04x\n", p_conn_param->mtu);
                //pf ("mps : 0x%04x\n", p_conn_param->mps);
                //pf ("credit : 0x%04x\n", p_conn_param->credit);
                //l2cap_service_conn_hdlr(p_conn_param, p_event_data->conn_hdl);
            }
            break;
        case BLE_L2CAP_EVENT_CF_CONN_IND:
            pf ("Received BLE_L2CAP_EVENT_CF_CONN_IND\n");
            {
                st_ble_l2cap_cf_conn_evt_t * p_conn_param;
                p_conn_param = (st_ble_l2cap_cf_conn_evt_t *)p_event_data->p_param;
                pf ("event_result : 0x%04x\n", event_result);
                //pf ("cid : 0x%04x\n", p_conn_param->cid);
                //pf ("psm : 0x%04x\n", p_conn_param->psm);
                //pf ("mtu : 0x%04x\n", p_conn_param->mtu);
                //pf ("mps : 0x%04x\n", p_conn_param->mps);
                //pf ("credit : 0x%04x\n", p_conn_param->credit);
                l2cap_service_conn_hdlr(p_conn_param, p_event_data->conn_hdl);
            }
            break;
        case BLE_L2CAP_EVENT_CF_DISCONN_CNF:
            pf ("Received BLE_L2CAP_EVENT_CF_DISCONN_CNF\n");
            {
                st_ble_l2cap_cf_disconn_evt_t * p_disconn_param;
                p_disconn_param = (st_ble_l2cap_cf_disconn_evt_t *)p_event_data->p_param;
                pf ("cid : 0x%04x\n", p_disconn_param->cid);
            }
            break;
        case BLE_L2CAP_EVENT_CF_DISCONN_IND:
            pf ("Received BLE_L2CAP_EVENT_CF_DISCONN_IND\n");
            {
                st_ble_l2cap_cf_disconn_evt_t * p_disconn_param;
                p_disconn_param = (st_ble_l2cap_cf_disconn_evt_t *)p_event_data->p_param;
                pf ("cid : 0x%04x\n", p_disconn_param->cid);
            }
            break;
        case BLE_L2CAP_EVENT_CF_RX_DATA_IND:
            pf ("Received BLE_L2CAP_EVENT_CF_RX_DATA_IND\n");
            {
                st_ble_l2cap_cf_data_evt_t * p_data_param;
                uint16_t i=0;
                p_data_param = (st_ble_l2cap_cf_data_evt_t *)p_event_data->p_param;
                pf ("cid : 0x%04x\n", p_data_param->cid);
                pf ("psm : 0x%04x\n", p_data_param->psm);
                pf ("data_length : 0x%04x\n", p_data_param->data_len);
                for(i=0; i<p_data_param->data_len; i++)
                {
                    pf ("data[%d] : 0x%02x\n", i, p_data_param->p_data[i]);
                }
            }
            break;
        case BLE_L2CAP_EVENT_CF_LOW_RX_CRD_IND:
            pf ("Received BLE_L2CAP_EVENT_CF_LOW_RX_CRD_IND\n");
            {
                st_ble_l2cap_cf_credit_evt_t * p_credit_param;
                p_credit_param = (st_ble_l2cap_cf_credit_evt_t *)p_event_data->p_param;
                pf ("cid : 0x%04x\n", p_credit_param->cid);
                pf ("psm : 0x%04x\n", p_credit_param->psm);
                pf ("credit : 0x%04x\n", p_credit_param->credit);
            }
            break;
        case BLE_L2CAP_EVENT_CF_TX_CRD_IND:
            pf ("Received BLE_L2CAP_EVENT_CF_TX_CRD_IND\n");
            {
                st_ble_l2cap_cf_credit_evt_t * p_credit_param;
                p_credit_param = (st_ble_l2cap_cf_credit_evt_t *)p_event_data->p_param;
                pf ("cid : 0x%04x\n", p_credit_param->cid);
                pf ("psm : 0x%04x\n", p_credit_param->psm);
                pf ("credit : 0x%04x\n", p_credit_param->credit);
            }
            break;
        case BLE_L2CAP_EVENT_CF_TX_DATA_CNF:
            pf ("Received BLE_L2CAP_EVENT_CF_TX_DATA_CNF\n");
            {
                st_ble_l2cap_cf_data_evt_t * p_data_param;
                uint16_t i=0;
                p_data_param = (st_ble_l2cap_cf_data_evt_t *)p_event_data->p_param;
                pf ("cid : 0x%04x\n", p_data_param->cid);
                pf ("psm : 0x%04x\n", p_data_param->psm);
                pf ("data_length : 0x%04x\n", p_data_param->data_len);
                for(i=0; i<p_data_param->data_len; i++)
                {
                    pf ("data[%d] : 0x%02x\n", i, p_data_param->p_data[i]);
                }
            }
            break;
        case BLE_L2CAP_EVENT_CMD_REJ:
            pf ("Received BLE_L2CAP_EVENT_CMD_REJ\n");
            {
                st_ble_l2cap_rej_evt_t * p_cmd_reject_param;
                p_cmd_reject_param = (st_ble_l2cap_rej_evt_t *)p_event_data->p_param;
                pf ("reason : 0x%04x\n", p_cmd_reject_param->reason);
                pf ("data_1 : 0x%04x\n", p_cmd_reject_param->data_1);
                pf ("data_2 : 0x%04x\n", p_cmd_reject_param->data_2);
            }
            break;
        default:
            pf (
            "Received L2CAP Event  "
            "= 0x%04X!\n", event_type);
            break;
    }

    return ;
}

static ble_status_t l2cap_service_conn_hdlr(st_ble_l2cap_cf_conn_evt_t * p_conn_evt, uint16_t conn_hdl)
{
    ble_status_t retval;

    switch(p_conn_evt->psm)
    {
        /* Insufficient Authentication Test */
        case 0x0080:
        {
            st_ble_gap_auth_info_t sec_info;
            st_ble_l2cap_conn_rsp_param_t conn_rsp_param;

            conn_rsp_param.	lcid = p_conn_evt->cid;
            conn_rsp_param.	response = BLE_L2CAP_CF_RSP_SUCCESS;
            conn_rsp_param.	mtu = p_conn_evt->mtu;
            conn_rsp_param.	mps = p_conn_evt->mps;
            conn_rsp_param.	credit = p_conn_evt->credit;

            retval = R_BLE_GAP_GetDevSecInfo(conn_hdl, &sec_info);
            pf("R_BLE_GAP_GetDevSecInfo() retval : 0x%04x\n", retval);
            if(BLE_SUCCESS != retval)
            {
                conn_rsp_param.	response = BLE_L2CAP_CF_RSP_RFSD_INSF_AUTH;
            }

            retval = R_BLE_L2CAP_RspCfConn(&conn_rsp_param);
            pf("R_BLE_L2CAP_RspCfConn() retval : 0x%04x\n", retval);
        } break;

        /* Insufficient Authorization Test */
        case 0x0081:
        {
            st_ble_l2cap_conn_rsp_param_t conn_rsp_param;

            conn_rsp_param.	lcid = p_conn_evt->cid;
            conn_rsp_param.	response = BLE_L2CAP_CF_RSP_RFSD_INSF_AUTRZ;
            conn_rsp_param.	mtu = p_conn_evt->mtu;
            conn_rsp_param.	mps = p_conn_evt->mps;
            conn_rsp_param.	credit = p_conn_evt->credit;

            R_BLE_GAP_AuthorizeDev(conn_hdl, BLE_GAP_NOT_AUTHORIZED);

            retval = R_BLE_L2CAP_RspCfConn(&conn_rsp_param);
            pf("R_BLE_L2CAP_RspCfConn() retval : 0x%04x\n", retval);
        } break;

        /* Insufficient Encryption Key Size Test */
        case 0x0082:
        {
            st_ble_gap_auth_info_t sec_info;
            st_ble_l2cap_conn_rsp_param_t conn_rsp_param;

            conn_rsp_param.	lcid = p_conn_evt->cid;
            conn_rsp_param.	response = BLE_L2CAP_CF_RSP_SUCCESS;
            conn_rsp_param.	mtu = p_conn_evt->mtu;
            conn_rsp_param.	mps = p_conn_evt->mps;
            conn_rsp_param.	credit = p_conn_evt->credit;

            retval = R_BLE_GAP_GetDevSecInfo(conn_hdl, &sec_info);
            pf("R_BLE_GAP_GetDevSecInfo() retval : 0x%04x\n", retval);
            if(BLE_SUCCESS != retval)
            {
                conn_rsp_param.	response = BLE_L2CAP_CF_RSP_RFSD_INSF_AUTH;
            }

            if(16 != sec_info.ekey_size)
            {
                conn_rsp_param.	response = BLE_L2CAP_CF_RSP_RFSD_INSF_ENC_KEY;
            }

            retval = R_BLE_L2CAP_RspCfConn(&conn_rsp_param);
            pf("R_BLE_L2CAP_RspCfConn() retval : 0x%04x\n", retval);
        } break;

        default:
            break;
    }


    return BLE_SUCCESS;
}

#endif /* (BLE_CFG_CMD_LINE_EN == 1) */
