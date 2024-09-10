/**********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/
#include <stdlib.h>

#include "r_ble_lsc.h"
#include "r_ble_cli.h"
#include "tx_api.h"
#include "task_function.h"

#define pf R_BLE_CLI_Printf

uint8_t g_blink_rate;
long    g_lsc_ntf_value;
extern void set_lsc_event(unsigned long uxBitsToSet);

#if (BLE_CFG_CMD_LINE_EN == 1)

/*----------------------------------------------------------------------------------------------------------------------
    lsc set_switch_state_ntf command
----------------------------------------------------------------------------------------------------------------------*/
static void cmd_lsc_set_switch_state_ntf(int argc, char *argv[])
{
    if (argc != 3)
    {
        pf("lsc %s: unrecognized operands\n", argv[0]);
		return;
    }

    long value = strtol(argv[2], NULL, 0);
    g_lsc_ntf_value = value;

    if (value)
        set_lsc_event(LSC_WAIT_EN_CCCD);
    else
        set_lsc_event(LSC_WAIT_DIS_CCCD);
}

static const st_ble_cli_cmd_t lsc_set_switch_state_ntf_cmd =
{
    .p_name = "set_switch_state_ntf",
    .exec   = cmd_lsc_set_switch_state_ntf,
    .p_help = "Usage: lsc set_switch_state_ntf conn_hdl value",
};

/*----------------------------------------------------------------------------------------------------------------------
    lsc write_led_blink_rate command
----------------------------------------------------------------------------------------------------------------------*/
static void cmd_lsc_write_led_blink_rate(int argc, char *argv[])
{
    if (argc != 3)
    {
        pf("lsc %s: unrecognized operands\n", argv[0]);
		return;
    }

    uint8_t blink_rate;
    blink_rate = (uint8_t)strtol(argv[2], NULL, 0);
    g_blink_rate = blink_rate;
    set_lsc_event(LSC_WAIT_WR_BLINK);
}

static const st_ble_cli_cmd_t lsc_write_led_blink_rate_cmd =
{
    .p_name = "write_led_blink_rate",
    .exec   = cmd_lsc_write_led_blink_rate,
    .p_help = "Usage: lsc write_led_blink_rate conn_hdl blink_rate",
};

/*----------------------------------------------------------------------------------------------------------------------
    lsc read_led_blink_rate command
----------------------------------------------------------------------------------------------------------------------*/
static void cmd_lsc_read_led_blink_rate(int argc, char *argv[])
{
    if (argc != 2)
    {
        pf("lsc %s: unrecognized operands\n", argv[0]);
        return;
    }

    set_lsc_event(LSC_WAIT_RD_BLINK);
}

static const st_ble_cli_cmd_t lsc_read_led_blink_rate_cmd =
{
    .p_name = "read_led_blink_rate",
    .exec   = cmd_lsc_read_led_blink_rate,
    .p_help = "Usage: lsc read_led_blink_rate conn_hdl",
};

/*----------------------------------------------------------------------------------------------------------------------
    lsc command
----------------------------------------------------------------------------------------------------------------------*/
static const st_ble_cli_cmd_t * const lsc_sub_cmds[] =
{
    &lsc_set_switch_state_ntf_cmd,
    &lsc_write_led_blink_rate_cmd,
    &lsc_read_led_blink_rate_cmd,
};

const st_ble_cli_cmd_t g_lsc_cmd =
{
    .p_name      = "lsc",
    .p_cmds      = lsc_sub_cmds,
    .num_of_cmds = ARRAY_SIZE(lsc_sub_cmds),
    .p_help      = "Sub Command: set_switch_state_ntf, write_led_blink_rate, read_led_blink_rate\n"
                   "Try 'lsc sub-cmd help' for more information",
};
#else   /* (BLE_CFG_CMD_LINE_EN == 1) */
const st_ble_cli_cmd_t g_lsc_cmd;
#endif
