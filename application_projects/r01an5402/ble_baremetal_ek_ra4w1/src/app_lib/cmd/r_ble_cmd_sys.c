/**********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/

#include "r_ble_cli.h"
#include "r_ble_cmd.h"
#include "r_ble_cmd_sys.h"

bool g_inhibit_software_standby = true;

#if (BLE_CFG_CMD_LINE_EN == 1)

#define pf R_BLE_CLI_Printf

/*----------------------------------------------------------------------------------------------------------------------
    sys stby command
----------------------------------------------------------------------------------------------------------------------*/

static void exec_sys_stby(int argc, char *argv[])
{
    FSP_PARAMETER_NOT_USED(argc);
    FSP_PARAMETER_NOT_USED(argv);

    if (strcmp(argv[1], "on") == 0)
    {
        g_inhibit_software_standby = false;
        pf("NOTE: This console does not work during Software Standby Mode.\n");
        pf("To exit from the Software Standby Mode, please PUSH the SW1 on the board.\n");
    }
    else if (strcmp(argv[1], "off") == 0)
    {
        g_inhibit_software_standby = true;
    }
    else if (strcmp(argv[1], "get") == 0)
    {
        if (false != g_inhibit_software_standby)
        {
            pf("off\n");
        }
        else
        {
            pf("on\n");
        }
    }
    else
    {
        R_BLE_CLI_PrintUnrecognized();
    }
}

static const st_ble_cli_cmd_t sys_stby_cmd =
{
    .p_name = "stby",
    .exec   = exec_sys_stby,
    .p_help = "Usage: sys stby (on|off|get)\n"
              "Permit to enter software standby or not",
};

/*----------------------------------------------------------------------------------------------------------------------
    sys command
----------------------------------------------------------------------------------------------------------------------*/
static const st_ble_cli_cmd_t * const sys_sub_cmds[] = {
    &sys_stby_cmd,
};

const st_ble_cli_cmd_t g_sys_cmd =
{
    .p_name      = "sys",
    .p_cmds      = sys_sub_cmds,
    .num_of_cmds = sizeof(sys_sub_cmds)/sizeof(sys_sub_cmds[0]),
    .p_help      = "Sub Command: stby\n"
                   "Try 'sys sub-command help' for more information",
};



/*----------------------------------------------------------------------------------------------------------------------
    ble reset command
----------------------------------------------------------------------------------------------------------------------*/
ble_event_cb_t g_ble_host_reset_cb = NULL;

void R_BLE_CMD_SetResetCb(ble_event_cb_t reset_cb)
{
    if(NULL != reset_cb)
    {
        g_ble_host_reset_cb = reset_cb;
    }
}

static void exec_ble_reset(int argc, char *argv[])
{
    FSP_PARAMETER_NOT_USED(argc);
    FSP_PARAMETER_NOT_USED(argv);
    RM_BLE_ABS_Reset(BLE_ABS_INSTANCE.p_ctrl, g_ble_host_reset_cb);
    ((ble_abs_instance_ctrl_t *)(BLE_ABS_INSTANCE.p_ctrl))->open = 0;
}

static const st_ble_cli_cmd_t ble_reset_cmd =
{
    .p_name = "reset",
    .exec   = exec_ble_reset,
    .p_help = "Usage: ble reset\n",
};

/*----------------------------------------------------------------------------------------------------------------------
    ble close command
----------------------------------------------------------------------------------------------------------------------*/
static void exec_ble_close(int argc, char *argv[])
{
    FSP_PARAMETER_NOT_USED(argc);
    FSP_PARAMETER_NOT_USED(argv);
    RM_BLE_ABS_Close(BLE_ABS_INSTANCE.p_ctrl);
}

static const st_ble_cli_cmd_t ble_close_cmd =
{
    .p_name = "close",
    .exec   = exec_ble_close,
    .p_help = "Usage: ble close\n",
};

/*----------------------------------------------------------------------------------------------------------------------
    ble command
----------------------------------------------------------------------------------------------------------------------*/
static const st_ble_cli_cmd_t * const ble_sub_cmds[] = {
    &ble_reset_cmd,
    &ble_close_cmd,
};

const st_ble_cli_cmd_t g_ble_cmd =
{
    .p_name      = "ble",
    .p_cmds      = ble_sub_cmds,
    .num_of_cmds = sizeof(ble_sub_cmds)/sizeof(ble_sub_cmds[0]),
    .p_help      = "Sub Command: reset, close\n"
                   "Try 'ble sub-command help' for more information",
};
#else /* (BLE_CFG_CMD_LINE_EN == 1) */

void R_BLE_CMD_SetResetCb(ble_event_cb_t reset_cb)
{
    (void)reset_cb;
}

const st_ble_cli_cmd_t g_sys_cmd;
const st_ble_cli_cmd_t g_ble_cmd;

#endif /* (BLE_CFG_CMD_LINE_EN == 1) */
