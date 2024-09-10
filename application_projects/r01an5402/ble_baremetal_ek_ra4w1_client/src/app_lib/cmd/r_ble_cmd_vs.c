/**********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/
#include <string.h>
#include <stdlib.h>
#include "r_ble_cli.h"
#include "r_ble_cmd.h"
#include "r_ble_cmd_vs.h"

#if (BLE_CFG_CMD_LINE_EN == 1)

#define BLE_LOG_TAG "vs_cmd"
#include "logger/r_ble_logger.h"

#define pf R_BLE_CLI_Printf

extern void r_ble_rf_control_error(uint32_t err_no);

static uint8_t g_vs_mcu_reset = 0;

/*----------------------------------------------------------------------------------------------------------------------
    vs txp command
----------------------------------------------------------------------------------------------------------------------*/

static void exec_vs_set_tx_power(int argc, char *argv[])
{
    if (argc != 3)
    {
        pf("vs txp %s: unrecognized operands\n", argv[0]);
        return;
    }
    uint16_t conn_hdl = (uint16_t)strtol(argv[1], NULL, 0);
    uint8_t tx_power  = (uint8_t)strtol(argv[2], NULL, 0);

    R_BLE_VS_SetTxPower(conn_hdl,tx_power);
}

static const st_ble_cli_cmd_t vs_txp_set_cmd =
{
    .p_name = "set",
    .exec   = exec_vs_set_tx_power,
    .p_help = "Usage: vs txp set conn_hdl <tx_poewr(0-2)>\n"
              "Set Tx power",
};

static void exec_vs_get_tx_power(int argc, char *argv[])
{
    if (argc != 2)
    {
        pf("vs txp %s: unrecognized operands\n", argv[0]);
        return;
    }

    uint16_t conn_hdl = (uint16_t)strtol(argv[1], NULL, 0);

    R_BLE_VS_GetTxPower(conn_hdl);
}

static const st_ble_cli_cmd_t vs_txp_get_cmd =
{
    .p_name = "get",
    .exec   = exec_vs_get_tx_power,
    .p_help = "Usage: vs txp get conn_hdl\n"
              "Get Tx power",
};

static const st_ble_cli_cmd_t * const vs_txp_sub_cmds[] =
{
    &vs_txp_set_cmd,
    &vs_txp_get_cmd,
};

static const st_ble_cli_cmd_t vs_txp_cmd =
{
    .p_name      = "txp",
    .p_cmds      = vs_txp_sub_cmds,
    .num_of_cmds = sizeof(vs_txp_sub_cmds)/sizeof(vs_txp_sub_cmds[0]),
    .p_help      = "Sub Command: set, get\n"
                   "Try 'vs txp sucmd help' for more information",
};

/*----------------------------------------------------------------------------------------------------------------------
    vs test command
----------------------------------------------------------------------------------------------------------------------*/
static void abort_vs_test(void)
{
    R_BLE_VS_EndTest();
}

static void exec_vs_tx_test_start(int argc, char *argv[])
{
    if (argc != 8)
    {
        pf("vs test %s: unrecognized operands\n", argv[0]);
        R_BLE_CLI_SetCmdComp();
        return;
    }
    st_ble_vs_tx_test_param_t param;
    param.ch                = (uint8_t)strtol(argv[1],NULL,0);
    param.test_data_len     = (uint8_t)strtol(argv[2],NULL,0);
    param.packet_payload    = (uint8_t)strtol(argv[3],NULL,0);
    param.phy               = (uint8_t)strtol(argv[4],NULL,0);
    param.tx_power          = (uint8_t)strtol(argv[5],NULL,0);
    param.option            = (uint8_t)strtol(argv[6],NULL,0);
    param.num_of_packet     = (uint16_t)strtol(argv[7],NULL,0);

    R_BLE_VS_StartTxTest(&param);
}

static const st_ble_cli_cmd_t vs_test_tx_cmd =
{
    .p_name = "tx",
    .exec   = exec_vs_tx_test_start,
    .abort  = abort_vs_test,
    .p_help = "Usage: vs test tx <ch(0-39)> <data_len(0-255)> <payload(0-7)> "
              "<phy(1-4)> <tx_power(0-2)> <option(0-3)> <num_of_packet(0-65535)>\n"
              "Tramsmiter test start",
};

static void exec_vs_rx_test_start(int argc, char *argv[])
{
    if (argc != 3)
    {
        pf("vs test %s: unrecognized operands\n", argv[0]);
        R_BLE_CLI_SetCmdComp();
        return;
    }
    st_ble_vs_rx_test_param_t param;
    param.ch  = (uint8_t)strtol(argv[1],NULL,0);
    param.phy = (uint8_t)strtol(argv[2],NULL,0);

    R_BLE_VS_StartRxTest(&param);
}

static const st_ble_cli_cmd_t vs_test_rx_cmd =
{
    .p_name = "rx",
    .exec   = exec_vs_rx_test_start,
    .abort  = abort_vs_test,
    .p_help = "Usage: vs test rx <ch(0-39)> <phy(1-3)>\n"
              "Reciever test start",
};

static void exec_vs_test_end(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    R_BLE_VS_EndTest();
}

static const st_ble_cli_cmd_t vs_test_end_cmd =
{
    .p_name = "end",
    .exec   = exec_vs_test_end,
    .p_help = "Usage: vs test end\n"
              "Tx or Rx test end",
};

static const st_ble_cli_cmd_t * const vs_test_sub_cmds[] =
{
    &vs_test_tx_cmd,
    &vs_test_rx_cmd,
    &vs_test_end_cmd,
};

static const st_ble_cli_cmd_t vs_test_cmd =
{
    .p_name      = "test",
    .p_cmds      = vs_test_sub_cmds,
    .num_of_cmds = sizeof(vs_test_sub_cmds)/sizeof(vs_test_sub_cmds[0]),
    .p_help      = "Sub Command: tx, rx, end\n"
                   "Try 'vs test sucmd help' for more information",
};

/*----------------------------------------------------------------------------------------------------------------------
    vs addr command
----------------------------------------------------------------------------------------------------------------------*/

static void exec_vs_set_addr(int argc, char *argv[])
{
    if ((argc != 4) && (argc != 5))
    {
        pf("vs addr %s: unrecognized operands\n", argv[0]);
        return;
    }

    uint8_t area;
    st_ble_dev_addr_t dev_addr;
    if (strcmp(argv[1], "df") == 0)
    {
        area = BLE_VS_ADDR_AREA_DATA_FLASH;
    }
    else if (strcmp(argv[1], "curr") == 0)
    {
        area = BLE_VS_ADDR_AREA_REG;
    }
    else
    {
        pf("vs addr %s: unrecognized operands\n", argv[0]);
        return;
    }

    if (strcmp(argv[2], "rnd") == 0)
    {
        dev_addr.type = BLE_GAP_ADDR_RAND;
    }
    else if (strcmp(argv[2], "pub") == 0)
    {
        dev_addr.type = BLE_GAP_ADDR_PUBLIC;
    }
    else
    {
        pf("vs addr %s: unrecognized operands\n", argv[0]);
        return;
    }

    R_BLE_CMD_ParseAddr(argv[3],&dev_addr.addr[0]);
    g_vs_mcu_reset = 0;

    if (argc == 5)
    {
        if(strcmp(argv[4], "mcu_rst") == 0)
        {
            g_vs_mcu_reset = 1;
        }
        else
        {
            pf("vs addr %s: unrecognized operands\n", argv[0]);
            return;
        }
    }

    R_BLE_VS_SetBdAddr(area, &dev_addr);
}

static const st_ble_cli_cmd_t vs_addr_set_cmd =
{
    .p_name = "set",
    .exec   = exec_vs_set_addr,
    .p_help = "Usage: vs addr set (curr|df) (pub|rnd) addr [mcu_rst]\n"
              "Set BD address",
};

static void exec_vs_get_addr(int argc, char *argv[])
{
    if (argc != 3)
    {
        pf("vs addr %s: unrecognized operands\n", argv[0]);
        return;
    }
    uint8_t area;
    uint8_t type;
    if (strcmp(argv[1], "df") == 0)
    {
        area = BLE_VS_ADDR_AREA_DATA_FLASH;
    }
    else if (strcmp(argv[1], "curr") == 0)
    {
        area = BLE_VS_ADDR_AREA_REG;
    }
    else
    {
        pf("vs addr %s: unrecognized operands\n", argv[0]);
        return;
    }

    if (strcmp(argv[2], "rnd") == 0)
    {
        type = BLE_GAP_ADDR_RAND;
    }
    else if (strcmp(argv[2], "pub") == 0)
    {
        type = BLE_GAP_ADDR_PUBLIC;
    }
    else
    {
        pf("vs addr %s: unrecognized operands\n", argv[0]);
        return;
    }

    R_BLE_VS_GetBdAddr(area, type);
}

static const st_ble_cli_cmd_t vs_addr_get_cmd =
{
    .p_name = "get",
    .exec   = exec_vs_get_addr,
    .p_help = "Usage: vs addr get (curr|df) (pub|rnd)\n"
              "Get BD address",
};

static const st_ble_cli_cmd_t * const vs_addr_sub_cmds[] =
{
    &vs_addr_set_cmd,
    &vs_addr_get_cmd,
};

static const st_ble_cli_cmd_t vs_addr_cmd =
{
    .p_name      = "addr",
    .p_cmds      = vs_addr_sub_cmds,
    .num_of_cmds = sizeof(vs_addr_sub_cmds)/sizeof(vs_addr_sub_cmds[0]),
    .p_help      = "Sub Command: set, get\n"
                   "Try 'vs addr sucmd help' for more information",
};

/*----------------------------------------------------------------------------------------------------------------------
    vs scheme command
----------------------------------------------------------------------------------------------------------------------*/

static void exec_vs_set_coding_scheme(int argc, char *argv[])
{
    if (argc != 2)
    {
        pf("vs %s: unrecognized operands\n", argv[0]);
        return;
    }

    uint8_t coding_scheme = (uint8_t)strtol(argv[1],NULL,0);
    R_BLE_VS_SetCodingScheme(coding_scheme);
}

static const st_ble_cli_cmd_t vs_scheme_cmd =
{
    .p_name = "scheme",
    .exec   = exec_vs_set_coding_scheme,
    .p_help = "Usage: vs scheme <scheme(hex)>\n"
              "Set default coding scheme of Coded PHY",
};

/*----------------------------------------------------------------------------------------------------------------------
    vs rfctrl command
----------------------------------------------------------------------------------------------------------------------*/

static void exec_vs_rf_control(int argc, char *argv[])
{
    if ((argc != 2) && (argc != 7))
    {
        pf("vs %s: unrecognized operands\n", argv[0]);
        return;
    }
    st_ble_vs_set_rf_ctrl_param_t param;
    memset(&param,0,sizeof(st_ble_vs_set_rf_ctrl_param_t));

    if (strcmp(argv[1], "on") == 0)
    {
        param.power = BLE_VS_RF_ON;
    }
    else
    {
        param.power = BLE_VS_RF_OFF;
    }

    if( (param.power != BLE_VS_RF_OFF) && (argc == 7) )
    {
        param.option    = (uint8_t)strtol(argv[2],NULL,0);
        param.clval     = (uint8_t)strtol(argv[3],NULL,0);
        param.slow_clock= (uint8_t)strtol(argv[4],NULL,0);
        param.tx_power  = (uint8_t)strtol(argv[5],NULL,0);
        param.rf_option = (uint8_t)strtol(argv[6],NULL,0);
    }
    R_BLE_VS_SetRfControl(&param);
}

static const st_ble_cli_cmd_t vs_rfctrl_cmd =
{
    .p_name  = "rfctrl",
    .exec = exec_vs_rf_control,
    .p_help = "Usage: vs rfctrl <power(on|off)> <option(0|1)> <clval(hex)> "
              "<slw_clk(hex)> <tx_power(0-2)> <rf_opt(hex)>\n"
              "RF power control",
};

/*----------------------------------------------------------------------------------------------------------------------
    vs rand command
----------------------------------------------------------------------------------------------------------------------*/

static void exec_vs_get_rand(int argc, char *argv[])
{
    if (argc != 2)
    {
        pf("vs %s: unrecognized operands\n", argv[0]);
        return;
    }
    uint8_t rand_size = (uint8_t)strtol(argv[1],NULL,0);

    R_BLE_VS_GetRand(rand_size);
}

static const st_ble_cli_cmd_t vs_rand_cmd =
{
    .p_name = "rand",
    .exec   = exec_vs_get_rand,
    .p_help = "Usage: vs rand <rand_size(4-16)>\n"
              "Get random numner 4 to 16 bytes",
};

/*----------------------------------------------------------------------------------------------------------------------
    vs scan_ch_map command
----------------------------------------------------------------------------------------------------------------------*/
static void exec_vs_scan_ch_map_set(int argc, char *argv[])
{
    if (argc < 2)
    {
        pf("vs %s: unrecognized operands\n", argv[0]);
        return;
    }
    uint16_t ch_map = (uint16_t)strtol(argv[1],NULL,0);

    R_BLE_VS_SetScanChMap(ch_map);
    return;
}

static void exec_vs_scan_ch_map_get(int argc, char *argv[])
{
    if (argc < 1)
    {
        pf("vs %s: unrecognized operands\n", argv[0]);
        return;
    }
    
    R_BLE_VS_GetScanChMap();
    return;
}


static const st_ble_cli_cmd_t vs_scan_ch_map_set_cmd =
{
    .p_name = "set",
    .exec   = exec_vs_scan_ch_map_set,
    .p_help = "Usage: vs scan_ch_map set <ch_map(1-7)>\n"
              "Set scan channel map for use",
};

static const st_ble_cli_cmd_t vs_scan_ch_map_get_cmd =
{
    .p_name = "get",
    .exec   = exec_vs_scan_ch_map_get,
    .p_help = "Usage: vs scan_ch_map get\n"
              "Get scan channel map",
};

static const st_ble_cli_cmd_t * const vs_scan_ch_map_sub_cmds[] =
{
    &vs_scan_ch_map_set_cmd,
    &vs_scan_ch_map_get_cmd,
};


static const st_ble_cli_cmd_t vs_scan_ch_map_cmd =
{
    .p_name      = "scan_ch_map",
    .p_cmds      = vs_scan_ch_map_sub_cmds,
    .num_of_cmds = sizeof(vs_scan_ch_map_sub_cmds)/sizeof(vs_scan_ch_map_sub_cmds[0]),
    .p_help      = "Sub Command: set, get\n"
                   "Try 'vs scan_ch_map sucmd help' for more information",
};


/*----------------------------------------------------------------------------------------------------------------------
    vs command
----------------------------------------------------------------------------------------------------------------------*/

static const st_ble_cli_cmd_t * const vs_sub_cmds[] =
{
    &vs_txp_cmd,
    &vs_test_cmd,
    &vs_addr_cmd,
    &vs_scheme_cmd,
    &vs_rfctrl_cmd,
    &vs_rand_cmd,
    &vs_scan_ch_map_cmd,
};

const st_ble_cli_cmd_t g_vs_cmd =
{
    .p_name      = "vs",
    .p_cmds      = vs_sub_cmds,
    .num_of_cmds = sizeof(vs_sub_cmds)/sizeof(vs_sub_cmds[0]),
    .p_help      = "Sub Command: txp, test, addr, scheme, rfctrl, rand\n"
                   "Try 'vs sucmd help' for more information",
};

const char * const vs_evt_name[] =
{
    [BLE_VS_EVENT_SET_TX_POWER & 0x00FFU]           = "BLE_VS_EVENT_SET_TX_POWER",
    [BLE_VS_EVENT_GET_TX_POWER & 0x00FFU]           = "BLE_VS_EVENT_GET_TX_POWER",
    [BLE_VS_EVENT_TX_TEST_START & 0x00FFU]          = "BLE_VS_EVENT_TX_TEST_START",
    [BLE_VS_EVENT_TX_TEST_TERM & 0x00FFU]           = "BLE_VS_EVENT_TX_TEST_TERM",
    [BLE_VS_EVENT_RX_TEST_START & 0x00FFU]          = "BLE_VS_EVENT_RX_TEST_START",
    [BLE_VS_EVENT_TEST_END & 0x00FFU]               = "BLE_VS_EVENT_TEST_END",
    [BLE_VS_EVENT_SET_CODING_SCHEME_COMP & 0x00FFU] = "BLE_VS_EVENT_SET_CODING_SCHEME_COMP",
    [BLE_VS_EVENT_RF_CONTROL_COMP & 0x00FFU]        = "BLE_VS_EVENT_RF_CONTROL_COMP",
    [BLE_VS_EVENT_SET_ADDR_COMP & 0x00FFU]          = "BLE_VS_EVENT_SET_ADDR_COMP",
    [BLE_VS_EVENT_GET_ADDR_COMP & 0x00FFU]          = "BLE_VS_EVENT_GET_ADDR_COMP",
    [BLE_VS_EVENT_GET_RAND & 0x00FFU]               = "BLE_VS_EVENT_GET_RAND",
    [BLE_VS_EVENT_TX_FLOW_STATE_CHG & 0x00FFU]      = "BLE_VS_EVENT_TX_FLOW_STATE_CHG",
    [BLE_VS_EVENT_SET_SCAN_CH_MAP & 0x00FFU]        = "BLE_VS_EVENT_SET_SCAN_CH_MAP",
    [BLE_VS_EVENT_GET_SCAN_CH_MAP & 0x00FFU]        = "BLE_VS_EVENT_GET_SCAN_CH_MAP",
};

void R_BLE_CMD_VsCb(uint16_t type, ble_status_t result, st_ble_vs_evt_data_t *data)
{
    pf("%s result:0x%04X, param_len:%d\n", vs_evt_name[type&0xFFU], result, data->param_len);
    if(BLE_SUCCESS != result)
    {
        R_BLE_CLI_SetCmdComp();
        return;
    }
    switch( type )
    {
        case BLE_VS_EVENT_SET_TX_POWER:
        {
            st_ble_vs_set_tx_pwr_comp_evt_t *param =
                    (st_ble_vs_set_tx_pwr_comp_evt_t*)data->p_param;

            pf(" conn_hdl:0x%04X\n", param->conn_hdl);
            pf(" curr_tx_pwr:%s%ddBm\n",(param->curr_tx_pwr>=0)?"+":"",param->curr_tx_pwr);
        } break;
        case BLE_VS_EVENT_GET_TX_POWER:
        {
            st_ble_vs_get_tx_pwr_comp_evt_t *param =
                    (st_ble_vs_get_tx_pwr_comp_evt_t*)data->p_param;

            pf(" conn_hdl:0x%04X\n", param->conn_hdl);
            pf(" curr_tx_pwr:%s%ddBm\n", (param->curr_tx_pwr>=0)?"+":"",param->curr_tx_pwr);
            pf(" max_tx_pwr:%s%ddBm\n", (param->max_tx_pwr>=0)?"+":"",param->max_tx_pwr);
        } break;
        case BLE_VS_EVENT_TX_TEST_START:
        {
            pf("BLE_VS_EVENT_TX_TEST_START\n");
            /* do nothing */
        } break;
        case BLE_VS_EVENT_TX_TEST_TERM:
        {
            pf("BLE_VS_EVENT_TX_TEST_TERM\n");
            R_BLE_CLI_SetCmdComp();
            /* do nothing */
        } break;
        case BLE_VS_EVENT_RX_TEST_START:
        {
            pf("BLE_VS_EVENT_RX_TEST_START\n");
            /* do nothing */
        } break;
        case BLE_VS_EVENT_TEST_END:
        {
            st_ble_vs_test_end_evt_t *param =
                    (st_ble_vs_test_end_evt_t*)data->p_param;

            pf(" num of OK  packets:%u\n",param->num_of_packet);
            pf(" num of Err packets:%u\n",param->num_of_crc_err_packet);
            pf(" ave RSSI:%s%ddBm\n",(param->ave_rssi>=0)?"+":"",param->ave_rssi);
            pf(" max RSSI:%s%ddBm\n",(param->max_rssi>=0)?"+":"",param->max_rssi);
            pf(" min RSSI:%s%ddBm\n",(param->min_rssi>=0)?"+":"",param->min_rssi);
            if( param->ave_rssi == 127 )
            {
                pf(" Note: 127 means that RSSI is not available.\n");
            }

        } break;
        case BLE_VS_EVENT_SET_CODING_SCHEME_COMP:
        {
            pf("BLE_VS_EVENT_SET_CODING_SCHEME_COMP\n");
            /* do nothing */
        } break;
        case BLE_VS_EVENT_RF_CONTROL_COMP:
        {
            st_ble_vs_set_rf_ctrl_comp_evt_t *param =
                    (st_ble_vs_set_rf_ctrl_comp_evt_t*)data->p_param;
            pf(" rf power:%s\n", (!param->ctrl)?"off":"on");
        } break;
        case BLE_VS_EVENT_SET_ADDR_COMP:
        {
            if( g_vs_mcu_reset != 0 )
            {
                pf(" !! MCU software reset !!\n");

                volatile uint32_t loop_wait = 0xffff;
                while( loop_wait--)
                {
                    __ASM volatile ("nop");
                }
#ifndef USE_EXTERNAL_CONTROLLER
                r_ble_rf_control_error(0xffffffff);
#endif /* USE_EXTERNAL_CONTROLLER */
            }
        } break;
        case BLE_VS_EVENT_GET_ADDR_COMP:
        {
            st_ble_vs_get_bd_addr_comp_evt_t *param =
                    (st_ble_vs_get_bd_addr_comp_evt_t*)data->p_param;

            pf(" addr:%s on %s\n",
                BLE_BD_ADDR_STR(param->addr.addr,param->addr.type),
                (BLE_VS_ADDR_AREA_REG == param->area)?"current register":"data flash");
        } break;
        case BLE_VS_EVENT_GET_RAND:
        {
            st_ble_vs_get_rand_comp_evt_t *param =
                    (st_ble_vs_get_rand_comp_evt_t*)data->p_param;
            pf(" rand_val: ");
            for( int ii = 0; ii < param->rand_size; ii++ )
            {
                pf("%02X", param->p_rand[ii]);
            }
            pf("\n");
        } break;
        case BLE_VS_EVENT_TX_FLOW_STATE_CHG:
        {
            st_ble_vs_tx_flow_chg_evt_t *param =
                    (st_ble_vs_tx_flow_chg_evt_t*)data->p_param;
            pf(" state:%d (tx flow %s)\n",param->state,(BLE_VS_TX_FLOW_CTL_ON==param->state)?"on":"off");
            pf(" buffer_num:%u\n",param->buffer_num);
        } break;
        case BLE_VS_EVENT_SET_SCAN_CH_MAP:
        {
        } break;
        case BLE_VS_EVENT_GET_SCAN_CH_MAP:
        {
            st_ble_vs_get_scan_ch_map_comp_evt_t *param =
                    (st_ble_vs_get_scan_ch_map_comp_evt_t*)data->p_param;
            pf(" ch_map:0x%02x\n", param->ch_map);
        } break;
        default:
            break;
    }
}

#else /* (BLE_CFG_CMD_LINE_EN == 1) */

void R_BLE_CMD_VsCb(uint16_t type, ble_status_t result, st_ble_vs_evt_data_t *data)
{
    (void)type;
    (void)result;
    (void)&data;
    return;
}

const st_ble_cli_cmd_t g_vs_cmd;

#endif /* (BLE_CFG_CMD_LINE_EN == 1) */
