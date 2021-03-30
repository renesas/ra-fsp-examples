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
#include <stdlib.h>

#include "r_ble_lsc.h"
#include "r_ble_cli.h"

#define pf R_BLE_CLI_Printf

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

    uint16_t conn_hdl;
    conn_hdl = (uint16_t)strtol(argv[1], NULL, 0);

    long value = strtol(argv[2], NULL, 0);

    ble_status_t ret;
    ret = R_BLE_LSC_WriteSwitchStateCliCnfg(conn_hdl, (uint16_t *)&value);
    if (ret != BLE_SUCCESS)
    {
        pf("lsc %s: failed with 0x%04X\n", argv[0], ret);
		return;
	}
}

static const st_ble_cli_cmd_t lsc_set_switch_state_ntf_cmd =
{
    .p_name = "set_switch_state_ntf",
    .exec   = cmd_lsc_set_switch_state_ntf,
    .p_help = "Usage: lsc set_switch_state_ntf conn_hdl value",
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

    uint16_t conn_hdl;
    conn_hdl = (uint16_t)strtol(argv[1], NULL, 0);

    ble_status_t ret;
    ret = R_BLE_LSC_ReadBlinkRate(conn_hdl);
    if (ret != BLE_SUCCESS)
    {
        pf("lsc %s: failed with 0x%04X\n", argv[0], ret);
    }
}

static const st_ble_cli_cmd_t lsc_read_led_blink_rate_cmd =
{
    .p_name = "read_led_blink_rate",
    .exec   = cmd_lsc_read_led_blink_rate,
    .p_help = "Usage: lsc read_led_blink_rate conn_hdl",
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

    uint16_t conn_hdl;
    conn_hdl = (uint16_t)strtol(argv[1], NULL, 0);

    uint8_t blink_rate;
    blink_rate = (uint8_t)strtol(argv[2], NULL, 0);

    ble_status_t ret;
    ret = R_BLE_LSC_WriteBlinkRate(conn_hdl, &blink_rate);
    if (ret != BLE_SUCCESS)
    {
        pf("lsc %s: failed with 0x%04X\n", argv[0], ret);
    }
}

static const st_ble_cli_cmd_t lsc_write_led_blink_rate_cmd =
{
    .p_name = "write_led_blink_rate",
    .exec   = cmd_lsc_write_led_blink_rate,
    .p_help = "Usage: lsc write_led_blink_rate conn_hdl blink_rate",
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

