/**********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/
#include <string.h>
#include <stdlib.h>

#include "r_ble_cli.h"
#include "r_ble_cmd.h"
#include "r_ble_cmd_gap.h"

#if (BLE_CFG_CMD_LINE_EN == 1)

#define BLE_LOG_TAG "gap_cmd"
#include "logger/r_ble_logger.h"

#define pf R_BLE_CLI_Printf

static uint8_t gs_adv_data[31];

static st_ble_gap_adv_param_t gs_adv_param =
{
    .adv_hdl           = BLE_CMD_DEFAULT_ADV_HDL,
    .adv_prop_type     = BLE_GAP_LEGACY_PROP_ADV_IND,
    .adv_intv_min      = BLE_CMD_DEFAULT_ADV_INTV,
    .adv_intv_max      = BLE_CMD_DEFAULT_ADV_INTV,
    .adv_ch_map        = BLE_GAP_ADV_CH_ALL,
    .o_addr_type       = BLE_GAP_ADDR_PUBLIC,
    .filter_policy     = BLE_GAP_ADV_ALLOW_SCAN_ANY_CONN_ANY,
    .adv_phy           = BLE_GAP_ADV_PHY_1M,
    .scan_req_ntf_flag = BLE_GAP_SCAN_REQ_NTF_DISABLE,
};

static st_ble_gap_adv_param_t gs_non_conn_adv_param =
{
    .adv_hdl           = 0x02,
    .adv_prop_type     = 0x0000,
    .adv_intv_min      = BLE_CMD_DEFAULT_ADV_INTV,
    .adv_intv_max      = BLE_CMD_DEFAULT_ADV_INTV,
    .adv_ch_map        = BLE_GAP_ADV_CH_ALL,
    .o_addr_type       = BLE_GAP_ADDR_PUBLIC,
    .filter_policy     = BLE_GAP_ADV_ALLOW_SCAN_ANY_CONN_ANY,
    .adv_phy           = BLE_GAP_ADV_PHY_1M,
    .sec_adv_phy       = BLE_GAP_ADV_PHY_1M,
    .scan_req_ntf_flag = BLE_GAP_SCAN_REQ_NTF_DISABLE,
};

static st_ble_gap_perd_adv_param_t  gs_perd_adv_param =
{
    .adv_hdl          = 0x02,
    .prop_type        = 0x0000,
    .perd_intv_min    = 0x0100,
    .perd_intv_max    = 0x0100,
};

static uint8_t gs_data[13] = {2, 1, 6, 9, 9, 'R','E','L','-','I','U','T','5'};

static st_ble_gap_adv_data_t gs_adv_data_param =
{
    .adv_hdl          = 0x000,
    .data_type        = BLE_GAP_ADV_DATA_MODE,
    .data_length      = sizeof(gs_data),
    .p_data           = gs_data,
    .zero_length_flag = BLE_GAP_DATA_0_CLEAR,
};

static uint8_t gs_perd_data[8] = {'R','E','L','-','I','U','T','1'};

static st_ble_gap_adv_data_t gs_perd_adv_data_param =
{
    .adv_hdl          = 0x02,
    .data_type        = BLE_GAP_PERD_ADV_DATA_MODE,
    .data_length      = sizeof(gs_perd_data),
    .p_data           = gs_perd_data,
    .zero_length_flag = BLE_GAP_DATA_0_CLEAR,
};


typedef struct
{
    uint16_t conn_hdl;
    st_ble_dev_addr_t addr;
} st_device_info_t;

#define GAP_CONN_DEV_MAX        (7)
static st_device_info_t connected_device_info[GAP_CONN_DEV_MAX];

static ble_cmd_adv_rept_filter_t *gs_filters;
static uint8_t gs_filter_data[31];

static ble_cmd_adv_rept_cb_t gs_adv_rept_cb;

static st_ble_gap_scan_phy_param_t gs_scan_coded_phy_param =
{
    .scan_type   = BLE_GAP_SCAN_ACTIVE,
    .scan_intv   = 0x0200,
    .scan_window = 0x0100,
};

static st_ble_gap_scan_phy_param_t gs_scan_1M_phy_param =
{
    .scan_type   = BLE_GAP_SCAN_ACTIVE,
    //.scan_intv   = BLE_CMD_DEFAULT_SCAN_INTV,
    //.scan_window = BLE_CMD_DEFAULT_SCAN_INTV,
    .scan_intv   = 0x0640,
    .scan_window = 0x0190,
};

static st_ble_gap_scan_param_t gs_scan_param =
{
    .o_addr_type       = BLE_GAP_ADDR_PUBLIC,
    .filter_policy     = BLE_GAP_SCAN_ALLOW_ADV_ALL,
    .p_phy_param_1M    = &gs_scan_1M_phy_param,
    .p_phy_param_coded = &gs_scan_coded_phy_param,
};

static st_ble_gap_scan_on_t gs_scan_enable =
{
    .proc_type   = BLE_GAP_SC_PROC_OBS,
    .filter_dups = BLE_GAP_SCAN_FILT_DUPLIC_ENABLE,
    .duration    = 0x0000,
    .period      = 0x0000,
};

static st_ble_gap_conn_param_t gs_conn_param =
{
    .conn_intv_min = 0x130,
    .conn_intv_max = 0x130,
    .conn_latency  = 0x0000,
    .sup_to        = 0x03BB,
    .min_ce_length = 0xFFFF,
    .max_ce_length = 0xFFFF,
};

static st_ble_gap_conn_phy_param_t gs_conn_1m_phy_param =
{
    .scan_intv    = 0x0300,
    .scan_window  = 0x0300,
    .p_conn_param = &gs_conn_param,
};

static st_ble_gap_create_conn_param_t gs_create_conn_param =
{
    .init_filter_policy  = BLE_GAP_INIT_FILT_USE_ADDR,
    .remote_bd_addr      = { 0 },
    .remote_bd_addr_type = BLE_GAP_ADDR_PUBLIC,
    .own_addr_type       = BLE_GAP_ADDR_PUBLIC,
    .p_conn_param_1M     = &gs_conn_1m_phy_param,
};

static ble_cmd_adv_rept_filter_t scan_filter[2];

static uint16_t gs_pairing_conn_hdl = BLE_GAP_INVALID_CONN_HDL;


static void ble_cmd_adv_rept_cb(uint8_t *addr, uint8_t addr_type)
{
    pf("%s\n", BLE_BD_ADDR_STR(addr, addr_type));
}

static void parse_adv_rept(uint8_t *addr, uint8_t addr_type, uint8_t *data, uint8_t len)
{
    uint16_t pos = 0;

    while (pos < len)
    {
        /* Each advertising structure have following constructs.
           - Lenght: 1 byte (The length of AD type + AD data)
           - AD type: 1 byte
           - AD data: variable
        */
        uint8_t length = (uint8_t)(data[pos] - 1);
        uint8_t type   = data[pos + 1];
        ble_cmd_adv_rept_filter_t *filter = gs_filters;

        if ((type == filter->ad_type) &&
            (length == filter->len) &&
            (memcmp(filter->ad_data, &data[pos+2], length) == 0))
        {
            gs_adv_rept_cb(addr, addr_type);
            break;
        }
        pos = (uint8_t)(pos+length+2);
    }
}

static void ble_cmd_start_legacy_adv_process(int process)
{
    ble_status_t ret;

    switch (process)
    {
        case 0:
        {
            ret = R_BLE_GAP_SetAdvParam(&gs_adv_param);
            if (ret != BLE_SUCCESS)
            {
                BLE_LOG_ERR("R_BLE_GAP_SetAdvParam is failed: 0x%04X", ret);
            }
        } break;

        case 1:
        {
            ret = R_BLE_GAP_SetAdvSresData(&gs_adv_data_param);
            if (ret != BLE_SUCCESS)
            {
                BLE_LOG_ERR("R_BLE_GAP_SetAdvSresData is failed: 0x%04X", ret);
            }
        } break;

        case 2:
        {
            ret = R_BLE_GAP_StartAdv(BLE_CMD_DEFAULT_ADV_HDL, 0, 0);
            if (ret != BLE_SUCCESS)
            {
                BLE_LOG_ERR("R_BLE_GAP_StartAdv is failed: 0x%04X", ret);
            }
        } break;

        default:
        {
            /* Do nothing. */
        } break;
    }
}

static void ble_cmd_start_perd_adv_process(int process)
{
    ble_status_t ret;

    switch (process)
    {
        case 0:
        {
            ret = R_BLE_GAP_SetAdvParam(&gs_non_conn_adv_param);
            if (ret != BLE_SUCCESS)
            {
                BLE_LOG_ERR("R_BLE_GAP_SetAdvParam is failed: 0x%04X", ret);
            }
        } break;

        case 1:
        {
            ret = R_BLE_GAP_SetPerdAdvParam(&gs_perd_adv_param);
            if (ret != BLE_SUCCESS)
            {
                BLE_LOG_ERR("R_BLE_GAP_SetAdvSresData is failed: 0x%04X", ret);
            }
        } break;

        case 2:
        {
            ret = R_BLE_GAP_SetAdvSresData(&gs_perd_adv_data_param);
            if (ret != BLE_SUCCESS)
            {
                BLE_LOG_ERR("R_BLE_GAP_SetAdvSresData is failed: 0x%04X", ret);
            }
        } break;

        case 3:
        {
            ret = R_BLE_GAP_StartPerdAdv(2);
            if (ret != BLE_SUCCESS)
            {
                BLE_LOG_ERR("R_BLE_GAP_StartAdv is failed: 0x%04X", ret);
            }
        } break;

        case 4:
        {
            ret = R_BLE_GAP_StartAdv(2, 0, 0);
            if (ret != BLE_SUCCESS)
            {
                BLE_LOG_ERR("R_BLE_GAP_StartAdv is failed: 0x%04X", ret);
            }
        } break;

        default:
        {
            /* Do nothing. */
        } break;
    }
}

/*----------------------------------------------------------------------------------------------------------------------
    gap white list command
----------------------------------------------------------------------------------------------------------------------*/
static void exec_gap_white(int argc, char *argv[])
{


    if (strcmp(argv[1], "reg") == 0)
    {
        if (argc != 4)
        {
            pf("gap %s: unrecognized operands\n", argv[0]);
            return;
        }
        st_ble_dev_addr_t dev;

        R_BLE_CMD_ParseAddr(argv[2], dev.addr);
        dev.type = (uint8_t)strtol(argv[3], NULL, 0);
        R_BLE_GAP_ConfWhiteList(BLE_GAP_LIST_ADD_DEV, &dev, 1);
    }
    else if (strcmp(argv[1], "del") == 0)
    {
        st_ble_dev_addr_t dev;

        if (argc != 4)
        {
            pf("gap %s: unrecognized operands\n", argv[0]);
            return;
        }
        R_BLE_CMD_ParseAddr(argv[2], dev.addr);
        dev.type = (uint8_t)strtol(argv[3], NULL, 0);
        R_BLE_GAP_ConfWhiteList(BLE_GAP_LIST_REM_DEV, &dev, 1);
    }
    else if (strcmp(argv[1], "clear") == 0)
    {
        R_BLE_GAP_ConfWhiteList(BLE_GAP_LIST_CLR, NULL, 1);
    }
    else
    {
        pf("gap %s: unrecognized operand '%s'\n", argv[0], argv[1]);
    }
}

static const st_ble_cli_cmd_t gap_white_cmd =
{
    .p_name = "wl",
    .exec   = exec_gap_white,
    .p_help = "Usage: gap wl (reg|del|clear)\n"
              "Register or delete or clear white list.",
};

/*----------------------------------------------------------------------------------------------------------------------
    gap adv command
----------------------------------------------------------------------------------------------------------------------*/
static void exec_gap_adv(int argc, char *argv[])
{
    if (argc != 2)
    {
        pf("gap %s: unrecognized operands\n", argv[0]);
        return;
    }

    if (strcmp(argv[1], "start") == 0)
    {
        R_BLE_CMD_StartLegacyAdv(gs_adv_data, sizeof(gs_adv_data)/sizeof(gs_adv_data[0]));
    }
    else if (strcmp(argv[1], "stop") == 0)
    {
        R_BLE_CMD_StopLegacyAdv();
    }
    else
    {
        pf("gap %s: unrecognized operand '%s'\n", argv[0], argv[1]);
    }
}

static const st_ble_cli_cmd_t gap_adv_cmd =
{
    .p_name = "adv",
    .exec   = exec_gap_adv,
    .p_help = "Usage: gap adv (start|stop)\n"
              "Start or stop legacy advertising.",
};

/*----------------------------------------------------------------------------------------------------------------------
    gap periodic adv command
----------------------------------------------------------------------------------------------------------------------*/
static void exec_gap_perd_adv(int argc, char *argv[])
{
    if (argc != 2)
    {
        pf("gap %s: unrecognized operands\n", argv[0]);
        return;
    }

    if (strcmp(argv[1], "start") == 0)
    {
        R_BLE_CMD_StartPerdAdv();
    }
    else if (strcmp(argv[1], "stop") == 0)
    {
        R_BLE_CMD_StopPerdAdv();
    }
    else
    {
        pf("gap %s: unrecognized operand '%s'\n", argv[0], argv[1]);
    }
}

static const st_ble_cli_cmd_t gap_perd_adv_cmd =
{
    .p_name = "perd",
    .exec   = exec_gap_perd_adv,
    .p_help = "Usage: gap perd (start|stop)\n"
              "Start or stop periodic advertising.",
};


/*----------------------------------------------------------------------------------------------------------------------
    gap scan command
----------------------------------------------------------------------------------------------------------------------*/
static void exec_gap_scan(int argc, char *argv[])
{
    if ((argc != 1) && (argc != 3))
    {
        pf("gap %s: unrecognized operands\n", argv[0]);
        return;
    }

    if (argc == 1)
    {
        R_BLE_CMD_StartLegacyScan(ble_cmd_adv_rept_cb, NULL);
    }
    else
    {
        uint16_t len;
        scan_filter[0].ad_type = (uint8_t)strtol(argv[1], NULL, 0);
        R_BLE_CMD_ParseValues(argv[2], gs_filter_data, &len);
        scan_filter[0].len = len;
        scan_filter[0].ad_data = gs_filter_data;
        R_BLE_CMD_StartLegacyScan(ble_cmd_adv_rept_cb, scan_filter);
    }
}

static void abort_gap_scan(void)
{
    R_BLE_CMD_StopLegacyScan();
}

static const st_ble_cli_cmd_t gap_scan_cmd =
{
    .p_name = "scan",
    .exec   = exec_gap_scan,
    .abort  = abort_gap_scan,
    .p_help = "Usage: gap scan [ad_type] [ad_data]\n"
              "Start scanning, stop with Ctrl-C",
};

/*----------------------------------------------------------------------------------------------------------------------
    gap periodic sync command
----------------------------------------------------------------------------------------------------------------------*/
static void exec_gap_sync(int argc, char *argv[])
{

    st_ble_dev_addr_t advr;
    uint8_t adv_sid = 0;

    if (strcmp(argv[1], "create") == 0)
    {
        if (argc != 4)
        {
            pf("gap %s: unrecognized operands\n", argv[0]);
            R_BLE_CLI_SetCmdComp();
            return;
        }
        R_BLE_CMD_ParseAddr(argv[2], advr.addr);
        advr.type = 0;
        adv_sid = (uint8_t)strtol(argv[3], NULL, 0);

        R_BLE_GAP_CreateSync(&advr, adv_sid, 0, 0x100);
    }
    else if (strcmp(argv[1], "cancel") == 0)
    {
        R_BLE_GAP_CancelCreateSync();
    }
    else if (strcmp(argv[1], "term") == 0)
    {
        if (argc != 2)
        {
            pf("gap %s: unrecognized operands\n", argv[0]);
            R_BLE_CLI_SetCmdComp();
            return;
        }
        uint8_t sync_hdl;
        sync_hdl = (uint8_t)strtol(argv[2], NULL, 0);
        R_BLE_GAP_TerminateSync(sync_hdl);
    }
    else
    {
        pf("gap %s: unrecognized operand '%s'\n", argv[0], argv[1]);
        R_BLE_CLI_SetCmdComp();
    }
}

static void abort_gap_sync(void)
{
    R_BLE_GAP_CancelCreateSync();
}

static const st_ble_cli_cmd_t gap_sync_cmd =
{
    .p_name = "sync",
    .exec   = exec_gap_sync,
    .abort  = abort_gap_sync,
    .p_help = "Usage: gap sync | cancel \n"
              "Start sync | cancel",
};

/*----------------------------------------------------------------------------------------------------------------------
    gap conn command
----------------------------------------------------------------------------------------------------------------------*/
static void abort_gap_conn(void)
{
    R_BLE_GAP_CancelCreateConn();
}

static void exec_gap_conn(int argc, char *argv[])
{
    if (argc != 3)
    {
        pf("gap %s: unrecognized operands\n", argv[0]);
        R_BLE_CLI_SetCmdComp();
        return;
    }

    R_BLE_CMD_ParseAddr(argv[1], gs_create_conn_param.remote_bd_addr);

    if (strcmp(argv[2], "rnd") == 0)
    {
        gs_create_conn_param.remote_bd_addr_type = BLE_GAP_ADDR_RAND;
    }
    else
    {
        gs_create_conn_param.remote_bd_addr_type = BLE_GAP_ADDR_PUBLIC;
    }

    ble_status_t ret;
    ret = R_BLE_GAP_CreateConn(&gs_create_conn_param);
    if (ret != BLE_SUCCESS)
    {
        pf("gap %s: failed with 0x%04X\n", argv[0], ret);
        R_BLE_CLI_SetCmdComp();
    }
}

static const st_ble_cli_cmd_t gap_conn_cmd =
{
    .p_name = "conn",
    .exec   = exec_gap_conn,
    .abort  = abort_gap_conn,
    .p_help = "Usage: gap conn addr (pub|rnd)\n"
              "Create connection, cancel with Ctrl-C",
};

/*----------------------------------------------------------------------------------------------------------------------
    gap disconn command
----------------------------------------------------------------------------------------------------------------------*/
static void exec_gap_disconn(int argc, char *argv[])
{
    if (argc != 2)
    {
        pf("gap %s: unrecognized operands\n", argv[0]);
        R_BLE_CLI_SetCmdComp();
        return;
    }

    uint16_t conn_hdl;
    conn_hdl = (uint16_t)strtol(argv[1], NULL, 0);

    ble_status_t ret;
    /* 0x13: Remote User Terminated Connection. */
    ret = R_BLE_GAP_Disconnect(conn_hdl, 0x13);
    if (ret != BLE_SUCCESS)
    {
        pf("gap %s: failed with 0x%04X\n", argv[0], ret);
        R_BLE_CLI_SetCmdComp();
    }
}

static const st_ble_cli_cmd_t gap_disconn_cmd =
{
    .p_name = "disconn",
    .exec   = exec_gap_disconn,
    .p_help = "Usage: gap disconn conn_hdl\n"
              "Disconnect a connection",
};

/*----------------------------------------------------------------------------------------------------------------------
    gap device command
----------------------------------------------------------------------------------------------------------------------*/
static void exec_gap_device(int argc, char *argv[])
{
    FSP_PARAMETER_NOT_USED(argc);
    FSP_PARAMETER_NOT_USED(argv);

    for (int i = 0; i < GAP_CONN_DEV_MAX; i++)
    {
        if (connected_device_info[i].conn_hdl != BLE_GAP_INVALID_CONN_HDL)
        {
            pf("conn_hdl=0x%04X, bdaddr=%s\n",
                             connected_device_info[i].conn_hdl,
                             BLE_BD_ADDR_STR(connected_device_info[i].addr.addr,
                                             connected_device_info[i].addr.type));
        }
    }
}

static const st_ble_cli_cmd_t gap_device_cmd =
{
    .p_name = "device",
    .exec   = exec_gap_device,
    .p_help = "Usage: gap device\n"
              "List connecting devices",
};


/*----------------------------------------------------------------------------------------------------------------------
    gap authentication command
----------------------------------------------------------------------------------------------------------------------*/
static void exec_gap_auth(int argc, char *argv[])
{
    uint16_t conn_hdl;
    uint32_t passkey;
    ble_status_t ret = BLE_SUCCESS;
    uint8_t resp;

    if (argc > 7)
    {
        pf("auth %s: unrecognized operands\n", argv[0]);
        return;
    }

    if (strcmp(argv[1], "start") == 0)
    {
        conn_hdl = (uint16_t)strtol(argv[2], NULL, 0);

        st_ble_gap_auth_info_t sec_info;

        /** check security information */
        ret = R_BLE_GAP_GetDevSecInfo(conn_hdl, &sec_info);
        if(BLE_SUCCESS == ret)
        {
            ret = R_BLE_GAP_StartEnc(conn_hdl);
        }
        else
        {
            ret = R_BLE_GAP_StartPairing(conn_hdl);
        }
    }
    else if(strcmp(argv[1], "passkey") == 0)
    {
        passkey = (uint32_t)strtol(argv[2], NULL, 10);
        ret = R_BLE_GAP_ReplyPasskeyEntry(gs_pairing_conn_hdl, passkey, BLE_GAP_PAIRING_ACCEPT);
    }
#if 0
    else if(strcmp(argv[1], "update") == 0)
    {
        st_ble_gap_conn_param_t g_conn_updt_param = {
            .conn_intv_min = 0x0014,
            .conn_intv_max = 0x0014,
            .conn_latency  = 0x0000,
            .sup_to        = 0x012C,
            .min_ce_length = 0xFFFF,
            .max_ce_length = 0xFFFF,
        };

        conn_hdl = (uint32_t)strtol(argv[2], NULL, 0);

        ret = R_BLE_GAP_UpdConn(conn_hdl, BLE_GAP_CONN_UPD_MODE_REQ, 0x0000, &g_conn_updt_param);
        pf("R_BLE_GAP_UpdConn() : 0x%04x\n", ret);
    }
#endif
    else if(strcmp(argv[1], "param") == 0)
    {
        st_ble_gap_pairing_param_t pair_params;

        pair_params.iocap = (uint8_t)((uint32_t)strtol(argv[2], NULL, 0));
        pair_params.mitm = (uint8_t)((uint32_t)strtol(argv[3], NULL, 0));
        pair_params.bonding = (uint8_t)((uint32_t)strtol(argv[4], NULL, 0));
        pair_params.max_key_size = (uint8_t)((uint32_t)strtol(argv[5], NULL, 0));
        pair_params.min_key_size = 7;
        pair_params.loc_key_dist = (uint8_t)((uint32_t)strtol(argv[6], NULL, 0));
        pair_params.rem_key_dist = (uint8_t)((uint32_t)strtol(argv[6], NULL, 0));
        pair_params.key_notf = 0;
        pair_params.sec_conn_only = 0;

        ret = R_BLE_GAP_SetPairingParams(&pair_params);
    }
    else if(strcmp(argv[1], "oob") == 0)
    {
        if(strcmp(argv[2], "gen") == 0)
        {
            ret = R_BLE_GAP_CreateScOobData();
            pf("R_BLE_GAP_CreateScOobData() : 0x%04x\n", ret);
        }
        else if(strcmp(argv[2], "remote") == 0)
        {
            st_ble_dev_addr_t remote;
            st_ble_gap_oob_data_t oob;
            int32_t i;

            if (argc != 6)
            {
                pf("gap %s: unrecognized operands\n", argv[0]);
                R_BLE_CLI_SetCmdComp();
                return;
            }

            R_BLE_CMD_ParseAddr(argv[3], remote.addr);
            remote.type = 0;
            char num_str[2];
            
            for(i=0; i<16; i++)
            {
                memcpy(num_str, argv[4] + i*2, 2);
                oob.sc_cnf_val[i] = (uint8_t)strtol(num_str, NULL, 0);
            }

            for(i=0; i<16; i++)
            {
                memcpy(num_str, argv[5] + i*2, 2);
                oob.sc_rand[i] = (uint8_t)strtol(num_str, NULL, 0);
            }

            ret = R_BLE_GAP_SetRemOobData(&remote, BLE_GAP_OOB_DATA_PRESENT, &oob);
            pf("R_BLE_GAP_SetRemOobData() : 0x%04x\n", ret);
        }

    }
    else if(strcmp(argv[1], "numcmp") == 0)
    {
        resp = BLE_GAP_PAIRING_REJECT;
        if(strcmp(argv[2], "yes") == 0)
        {
            resp = BLE_GAP_PAIRING_ACCEPT;
        }

        ret = R_BLE_GAP_ReplyNumComp(gs_pairing_conn_hdl, resp);
    }
    else if(strcmp(argv[1], "del") == 0)
    {
        int32_t local = BLE_GAP_SEC_DEL_LOC_NONE;
        int32_t remote = BLE_GAP_SEC_DEL_REM_NONE;
        ble_gap_del_bond_cb_t cb = NULL;
        st_ble_dev_addr_t rem_dev;

        if(strcmp(argv[2], "local") == 0)
        {
            local = BLE_GAP_SEC_DEL_LOC_ALL;
        }
        else if(strcmp(argv[2], "remote") == 0)
        {
            if(argc > 3)
            {
                if(strcmp(argv[3], "addr") == 0 && (6 == argc))
                {
                    remote = BLE_GAP_SEC_DEL_REM_SA;
                    R_BLE_CMD_ParseAddr(argv[4], rem_dev.addr);
                    rem_dev.type = (uint8_t)((strcmp(argv[5], "rnd") == 0) ? BLE_GAP_ADDR_RAND : BLE_GAP_ADDR_PUBLIC);
                }
                else if(strcmp(argv[3], "all") == 0)
                {
                    remote = BLE_GAP_SEC_DEL_REM_ALL;
                }
                else
                {
                    if(strcmp(argv[3], "not_conn") == 0)
                    {
                        remote = BLE_GAP_SEC_DEL_REM_NOT_CONN;
                    }
                }
#if (BLE_ABS_CFG_ENABLE_SECURE_DATA == 1)
                //cb = R_BLE_SECD_DelRemKeys;
#endif /* (BLE_ABS_CFG_ENABLE_SECURE_DATA == 1) */
            }
        }
        else
        {
            if(strcmp(argv[2], "all") == 0)
            {
                local = BLE_GAP_SEC_DEL_LOC_ALL;
                remote = BLE_GAP_SEC_DEL_REM_ALL;
            }
        }

        R_BLE_GAP_DeleteBondInfo(local, remote, &rem_dev, cb);
    }
    else
    {
        pf("auth %s: unrecognized operands\n", argv[1]);
    }

    if (ret != BLE_SUCCESS)
    {
        pf("abs %s: failed with 0x%04X\n", argv[0], ret);
    }
}

static const st_ble_cli_cmd_t gap_auth_cmd =
{
    .p_name = "sec",
    .exec   = exec_gap_auth,
    .p_help = "Usage: gap security\n"
              "Pairing",
};

/*----------------------------------------------------------------------------------------------------------------------
    gap command
----------------------------------------------------------------------------------------------------------------------*/
static const st_ble_cli_cmd_t * const gap_sub_cmds[] =
{
    &gap_adv_cmd,
    &gap_scan_cmd,
    &gap_conn_cmd,
    &gap_disconn_cmd,
    &gap_device_cmd,
    &gap_auth_cmd,
    &gap_perd_adv_cmd,
    &gap_sync_cmd,
    &gap_white_cmd,
};

const st_ble_cli_cmd_t g_gap_cmd =
{
    .p_name      = "gap",
    .p_cmds      = gap_sub_cmds,
    .num_of_cmds = sizeof(gap_sub_cmds) / sizeof(gap_sub_cmds[0]),
    .p_help      = "Sub Command: adv, scan, conn, disconn, device\n"
                   "Try 'gap subcmd help' for more information",
};

void R_BLE_CMD_StartLegacyAdv(uint8_t *p_adv_data, uint8_t data_len)
{
    gs_adv_data_param.p_data      = p_adv_data;
    gs_adv_data_param.data_length = data_len;

    gs_adv_param.adv_prop_type    = BLE_GAP_LEGACY_PROP_ADV_IND;

    ble_cmd_start_legacy_adv_process(0);
}

void R_BLE_CMD_StartLegacyAdvNonConn(uint8_t *p_adv_data, uint8_t data_len)
{
    gs_adv_data_param.p_data      = p_adv_data;
    gs_adv_data_param.data_length = data_len;

    gs_adv_param.adv_prop_type    = BLE_GAP_LEGACY_PROP_ADV_NONCONN_IND;

    ble_cmd_start_legacy_adv_process(0);
}

void R_BLE_CMD_StartPerdAdv(void)
{
    ble_cmd_start_perd_adv_process(0);
}

void R_BLE_CMD_StopPerdAdv(void)
{
    R_BLE_GAP_StartAdv(0, 0, 0);
}


void R_BLE_CMD_StopLegacyAdv(void)
{
    R_BLE_GAP_StopAdv(BLE_CMD_DEFAULT_ADV_HDL);
}

void R_BLE_CMD_StartLegacyScan(ble_cmd_adv_rept_cb_t cb, ble_cmd_adv_rept_filter_t *filters)
{
    gs_filters = filters;
    gs_adv_rept_cb = cb;
    R_BLE_GAP_StartScan(&gs_scan_param, &gs_scan_enable);
}

void R_BLE_CMD_CreateConn(uint8_t *addr, uint8_t addr_type)
{
    memcpy(gs_create_conn_param.remote_bd_addr, addr, BLE_BD_ADDR_LEN);

    if (addr_type == BLE_GAP_ADDR_PUBLIC)
    {
        gs_create_conn_param.remote_bd_addr_type = BLE_GAP_ADDR_PUBLIC;
    }
    else
    {
        gs_create_conn_param.remote_bd_addr_type = BLE_GAP_ADDR_RAND;
    }

    R_BLE_GAP_CreateConn(&gs_create_conn_param);
}

void R_BLE_CMD_StopLegacyScan(void)
{
    R_BLE_GAP_StopScan();
}

void R_BLE_CMD_GapCb(uint16_t type, ble_status_t result, st_ble_evt_data_t *data)
{
    switch (type)
    {
        case BLE_GAP_EVENT_STACK_ON:
        {
            memset(connected_device_info,0xFF,sizeof(connected_device_info));

        } break;
        case BLE_GAP_EVENT_CONN_IND:
        {
            if (result == BLE_SUCCESS)
            {
                st_ble_gap_conn_evt_t *conn_evt_param = 
                    (st_ble_gap_conn_evt_t *)data->p_param;

                int i;

                for (i = 0; i < GAP_CONN_DEV_MAX; i++)
                {
                    if (connected_device_info[i].conn_hdl == BLE_GAP_INVALID_CONN_HDL)
                    {
                        break;
                    }
                }

                connected_device_info[i].conn_hdl = conn_evt_param->conn_hdl;

                switch (conn_evt_param->remote_addr_type)
                {
                    case BLE_GAP_ADDR_PUBLIC:
                    case BLE_GAP_ADDR_RPA_ID_PUBLIC:
                    {
                        memcpy(connected_device_info[i].addr.addr,
                               conn_evt_param->remote_addr,
                               BLE_BD_ADDR_LEN);
                        connected_device_info[i].addr.type = BLE_GAP_ADDR_PUBLIC;
                    } break;

                    case BLE_GAP_ADDR_RAND:
                    case BLE_GAP_ADDR_RPA_ID_RANDOM:
                    {
                        memcpy(connected_device_info[i].addr.addr,
                               conn_evt_param->remote_addr,
                               BLE_BD_ADDR_LEN);
                        connected_device_info[i].addr.type = BLE_GAP_ADDR_RAND;
                    } break;

                    default:
                        break;

                }

                pf("gap: connected conn_hdl:0x%04X, addr:%s\n",
                   connected_device_info[i].conn_hdl,
                   BLE_BD_ADDR_STR(connected_device_info[i].addr.addr,
                                   connected_device_info[i].addr.type));
                R_BLE_CLI_SetCmdComp();
            }
        } break;

        case BLE_GAP_EVENT_DISCONN_IND:
        {
            st_ble_gap_disconn_evt_t *disc_evt_param =
                (st_ble_gap_disconn_evt_t *)data->p_param;

            for (int i = 0; i < GAP_CONN_DEV_MAX; i++)
            {
                if (connected_device_info[i].conn_hdl == disc_evt_param->conn_hdl)
                {
                    pf("gap: disconnected conn_hdl:0x%04X, addr:%s\n",
                       connected_device_info[i].conn_hdl,
                       BLE_BD_ADDR_STR(connected_device_info[i].addr.addr,
                                       connected_device_info[i].addr.type));

                    connected_device_info[i].conn_hdl = BLE_GAP_INVALID_CONN_HDL;
                    break;
                }
            }
        } break;

        case BLE_GAP_EVENT_ADV_REPT_IND:
        {
            st_ble_gap_adv_rept_evt_t *adv_rept_evt_param =
                (st_ble_gap_adv_rept_evt_t *)data->p_param;

            switch (adv_rept_evt_param->adv_rpt_type)
            {
                case 0x00:
                {
                    st_ble_gap_adv_rept_t *adv_rept_param =
                        (st_ble_gap_adv_rept_t *)adv_rept_evt_param->param.p_adv_rpt;

                    if (gs_filters == NULL)
                    {
                        if(gs_adv_rept_cb != NULL)
                        {
                            gs_adv_rept_cb(adv_rept_param->p_addr, adv_rept_param->addr_type);
                        }
                    }
                    else
                    {
                        parse_adv_rept(
                            adv_rept_param->p_addr, adv_rept_param->addr_type,
                            adv_rept_param->p_data, adv_rept_param->len);
                    }
                } break;

                case 0x01:
                {
                    st_ble_gap_ext_adv_rept_t *ext_adv_rept_param =
                        (st_ble_gap_ext_adv_rept_t *)adv_rept_evt_param->param.p_ext_adv_rpt;

                    if (gs_filters == NULL)
                    {
                        if(gs_adv_rept_cb != NULL)
                        {
                            gs_adv_rept_cb(ext_adv_rept_param->p_addr, ext_adv_rept_param->addr_type);
                        }
                    }
                    else
                    {
                        parse_adv_rept(
                            ext_adv_rept_param->p_addr, ext_adv_rept_param->addr_type,
                            ext_adv_rept_param->p_data, ext_adv_rept_param->len);
                    }
                } break;

                case 0x02:
                {
                    st_ble_gap_perd_adv_rept_t * p_perd_adv_rept_param =
                        (st_ble_gap_perd_adv_rept_t *) adv_rept_evt_param->param.p_per_adv_rpt;
                    pf("sync_hdl : 0x%04x \n", p_perd_adv_rept_param->sync_hdl);
                } break;

                default:
                    break;
            }
        } break;

        case BLE_GAP_EVENT_SCAN_ON:
        {
        } break;

        case BLE_GAP_EVENT_SYNC_EST:
        {
            pf("sync established\n");
        } break;

        case BLE_GAP_EVENT_SYNC_TERM:
        {
            pf("sync terminated\n");
        } break;

        case BLE_GAP_EVENT_SYNC_LOST:
        {
            pf("sync lost\n");
        } break;

        case BLE_GAP_EVENT_ADV_PARAM_SET_COMP:
        {
            st_ble_gap_adv_set_evt_t * p_set_adv_hdl =
                (st_ble_gap_adv_set_evt_t *)data->p_param;
            if(0x02 == p_set_adv_hdl->adv_hdl)
            {
                ble_cmd_start_perd_adv_process(1);
            }
            else
            {
                ble_cmd_start_legacy_adv_process(1);
            }
        } break;

        case BLE_GAP_EVENT_PERD_ADV_PARAM_SET_COMP:
        {
            st_ble_gap_adv_data_evt_t * p_set_adv_data_hdl =
                (st_ble_gap_adv_data_evt_t *)data->p_param;
            if(0x02 == p_set_adv_data_hdl->adv_hdl)
            {
                ble_cmd_start_perd_adv_process(2);
            }
        }
        break;

        case BLE_GAP_EVENT_PERD_ADV_ON:
            ble_cmd_start_perd_adv_process(4);
            break;


        case BLE_GAP_EVENT_ADV_DATA_UPD_COMP:
        {
            st_ble_gap_adv_data_evt_t * p_set_adv_hdl =
                (st_ble_gap_adv_data_evt_t *)data->p_param;
            if(0x02 == p_set_adv_hdl->adv_hdl)
            {
                ble_cmd_start_perd_adv_process(3);
            }
            else
            {
                ble_cmd_start_legacy_adv_process(2);
            }
        } break;

        case BLE_GAP_EVENT_PAIRING_REQ:
        {
            pf("receive BLE_GAP_EVENT_PAIRING_REQ\n");
            st_ble_gap_pairing_req_evt_t * p_pair_req_param =
                (st_ble_gap_pairing_req_evt_t *)data->p_param;

            gs_pairing_conn_hdl = p_pair_req_param->conn_hdl;
            R_BLE_GAP_ReplyPairing(gs_pairing_conn_hdl, BLE_GAP_PAIRING_ACCEPT);
        } break;

        case BLE_GAP_EVENT_PASSKEY_ENTRY_REQ:
        {
            pf("receive BLE_GAP_EVENT_PASSKEY_ENTRY_REQ\n");
            st_ble_gap_conn_hdl_evt_t * p_passkey_evt_param =
                (st_ble_gap_conn_hdl_evt_t *)data->p_param;

            gs_pairing_conn_hdl = p_passkey_evt_param->conn_hdl;
        } break;

        case BLE_GAP_EVENT_PASSKEY_DISPLAY_REQ:
        {
            pf("receive BLE_GAP_EVENT_PASSKEY_DISPLAY_REQ\n");
            st_ble_gap_passkey_display_evt_t * p_passkey_disp_evt_param =
                (st_ble_gap_passkey_display_evt_t *)data->p_param;
            pf("conn_hdl : 0%04x, passkey : %06d\n", 
                p_passkey_disp_evt_param->conn_hdl, p_passkey_disp_evt_param->passkey);
        } break;

        case BLE_GAP_EVENT_NUM_COMP_REQ:
        {
            pf("receive BLE_GAP_EVENT_NUM_COMP_REQ\n");
            st_ble_gap_num_comp_evt_t * p_numcmp_evt_param =
                (st_ble_gap_num_comp_evt_t *)data->p_param;

            gs_pairing_conn_hdl = p_numcmp_evt_param->conn_hdl;
            pf("numeric : %06d\n", p_numcmp_evt_param->numeric);
            R_BLE_GAP_ReplyNumComp(p_numcmp_evt_param->conn_hdl, BLE_GAP_PAIRING_ACCEPT);
        } break;

        case BLE_GAP_EVENT_EX_KEY_REQ:
        {
            pf("receive BLE_GAP_EVENT_EX_KEY_REQ\n");
            st_ble_gap_conn_hdl_evt_t * p_key_req =
                (st_ble_gap_conn_hdl_evt_t *)data->p_param;
            R_BLE_GAP_ReplyExKeyInfoReq(p_key_req->conn_hdl);
        } break;

        case BLE_GAP_EVENT_PEER_KEY_INFO:
        {
            pf("receive BLE_GAP_EVENT_PEER_KEY_INFO\n");
            st_ble_gap_peer_key_info_evt_t * p_peer_key =
                (st_ble_gap_peer_key_info_evt_t *)data->p_param;

            pf("LTK : %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x \n", 
                    p_peer_key->key_ex_param.p_keys_info->enc_info[0],
                    p_peer_key->key_ex_param.p_keys_info->enc_info[1],
                    p_peer_key->key_ex_param.p_keys_info->enc_info[2],
                    p_peer_key->key_ex_param.p_keys_info->enc_info[3],
                    p_peer_key->key_ex_param.p_keys_info->enc_info[4],
                    p_peer_key->key_ex_param.p_keys_info->enc_info[5],
                    p_peer_key->key_ex_param.p_keys_info->enc_info[6],
                    p_peer_key->key_ex_param.p_keys_info->enc_info[7],
                    p_peer_key->key_ex_param.p_keys_info->enc_info[8],
                    p_peer_key->key_ex_param.p_keys_info->enc_info[9],
                    p_peer_key->key_ex_param.p_keys_info->enc_info[10],
                    p_peer_key->key_ex_param.p_keys_info->enc_info[11],
                    p_peer_key->key_ex_param.p_keys_info->enc_info[12],
                    p_peer_key->key_ex_param.p_keys_info->enc_info[13],
                    p_peer_key->key_ex_param.p_keys_info->enc_info[14],
                    p_peer_key->key_ex_param.p_keys_info->enc_info[15]);

        } break;

        case BLE_GAP_EVENT_PAIRING_COMP:
        {
            pf("receive BLE_GAP_EVENT_PAIRING_COMP result : 0x%04x\n", result);
            st_ble_gap_pairing_info_evt_t * p_param;
            p_param = (st_ble_gap_pairing_info_evt_t *)data->p_param;
            pf("sec : 0x%02x, mode : 0x%02x,  bond : 0x%02x, key_size : 0x%02x\n", 
                p_param->auth_info.security, p_param->auth_info.pair_mode, 
                p_param->auth_info.bonding, p_param->auth_info.ekey_size);
        } break;

        case BLE_GAP_EVENT_ENC_CHG:
        {
            pf("receive BLE_GAP_EVENT_ENC_CHG result : 0x%04x\n", result);
        } break;

        case BLE_GAP_EVENT_LTK_REQ:
        {
            ble_status_t retval;
            st_ble_gap_ltk_req_evt_t *p_ltk_req_evt_param =
                (st_ble_gap_ltk_req_evt_t *)data->p_param;
            pf("receive BLE_GAP_EVENT_LTK_REQ result : 0x%04x\n", result);
            retval = R_BLE_GAP_ReplyLtkReq(p_ltk_req_evt_param->conn_hdl, p_ltk_req_evt_param->ediv, p_ltk_req_evt_param->p_peer_rand, BLE_GAP_LTK_REQ_ACCEPT);
            pf("R_BLE_GAP_ReplyLtkReq() result : 0x%04x\n", retval);
        } break;

        default:
        	break;

    }
}

#endif /* (BLE_CFG_CMD_LINE_EN == 1) */
