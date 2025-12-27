/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/ 
/**********************************************************************************************************************
 * File Name    : jlink_console.c
 * Description  : This file contains the definitions of jlink console usage functions.
 *********************************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include "hal_data.h"
#include "board_cfg.h"



uint32_t g_transfer_complete = 0;
uint32_t g_receive_complete  = 0;

static void Jlink_console_write(const char *buffer);
fsp_err_t print_to_console(char * p_data);
void jlink_console_init (void);

void jlink_console_init (void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    fsp_err = R_SCI_B_UART_Open(&g_jlink_console_ctrl, &g_jlink_console_cfg);

    assert(FSP_SUCCESS == fsp_err);

    return;
}

fsp_err_t print_to_console(char * p_data)
{
    fsp_err_t err = FSP_SUCCESS;

    Jlink_console_write(p_data);

    return (err);
}


static void Jlink_console_write(const char *buffer)
{
    fsp_err_t err = FSP_SUCCESS;

    g_transfer_complete = false;

    err = R_SCI_B_UART_Write(&g_jlink_console_ctrl, (uint8_t *)buffer, strlen(buffer));

    assert(FSP_SUCCESS == err);

    while (!g_transfer_complete)
    {
        R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MILLISECONDS);
    }
}


/* callback from driver */
void jlink_console_callback(uart_callback_args_t *p_args)
{
    /* Handle the UART event */
    switch (p_args->event)
    {

        /* Receive complete */
        case UART_EVENT_RX_COMPLETE:
        {
            g_receive_complete = 1;
            break;
        }
        /* Transmit complete */
        case UART_EVENT_TX_COMPLETE:
        {
            g_transfer_complete = 1;
            break;
        }

        default:
        {
        }
    }
}



