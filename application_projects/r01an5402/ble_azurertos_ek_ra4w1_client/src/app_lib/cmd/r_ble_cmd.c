/**********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/

#include "cli/r_ble_cli.h"
#include "r_ble_cmd.h"

#if (BLE_CFG_CMD_LINE_EN == 1)

void R_BLE_CMD_ParseValues(char *p_str, uint8_t *p_buffer, uint16_t *p_length)
{
    *p_length = 0;

    char *p_tp = strtok(p_str, ",");

    while ((p_tp != NULL) && (*p_length < 50))
    {
        if (p_tp != NULL)
        {
            char *p_endp;
            p_buffer[*p_length] = (uint8_t)strtol(p_tp, &p_endp, 0);
            if((p_buffer[*p_length] == 0) && (p_endp != NULL))
            {
                *p_length = 0;
                return;
            }
            *p_length = (uint16_t)(*p_length+1);
        }

        p_tp = strtok(NULL, ",");
    }
}

void R_BLE_CMD_PrintValues(uint8_t *p_buffer, uint16_t length)
{
    for (int i = 0; i < length; i++)
    {
        R_BLE_CLI_Printf("0x%02x", p_buffer[i]);

        if (i != length - 1)
        {
            R_BLE_CLI_Printf(",", p_buffer[i]);
        }
    }
}

void R_BLE_CMD_ParseAddr(char *p_str, uint8_t *p_addr)
{
    int p = 5;
    char *p_tp = strtok(p_str, ":");

    while ((p_tp != NULL) && (p >= 0))
    {
        if (p_tp != NULL)
        {
            p_addr[p] = (uint8_t)strtol(p_tp, NULL, 16);
            p--;
        }

        p_tp = strtok(NULL, ":");
    }
}

void R_BLE_CMD_PrintAddr(uint8_t *p_addr)
{
    for (uint16_t i = 0; i < 6; i++)
    {
        R_BLE_CLI_Printf("%02x", p_addr[5-i]);
        if (i != 5)
        {
            R_BLE_CLI_Printf(":");
        }
    }
    R_BLE_CLI_Printf("\n");
}

#else /* (BLE_CFG_CMD_LINE_EN == 1) && (BLE_CFG_HCI_MODE_EN == 0) */

void R_BLE_CMD_ParseValues(char *p_str, uint8_t *p_buffer, uint16_t *p_length)
{
    (void)p_str;
    (void)p_buffer;
    (void)p_length;
}

void R_BLE_CMD_PrintValues(uint8_t *p_buffer, uint16_t length)
{
    (void)p_buffer;
    (void)length;
}

void R_BLE_CMD_ParseAddr(char *p_str, uint8_t *p_addr)
{
    (void)p_str;
    (void)p_addr;
}

void R_BLE_CMD_PrintAddr(uint8_t *p_addr)
{
    (void)p_addr;
}

#endif /* (BLE_CFG_CMD_LINE_EN == 1) */
