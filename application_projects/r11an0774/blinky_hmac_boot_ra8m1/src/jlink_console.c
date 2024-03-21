/***********************************************************************************************************************
 * Copyright [2020-2024] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
 *
 * This software and documentation are supplied by Renesas Electronics America Inc. and may only be used with products
 * of Renesas Electronics Corp. and its affiliates ("Renesas").  No other uses are authorized.  Renesas products are
 * sold pursuant to Renesas terms and conditions of sale.  Purchasers are solely responsible for the selection and use
 * of Renesas products and Renesas assumes no liability.  No license, express or implied, to any intellectual property
 * right is granted by Renesas. This software is protected under all applicable laws, including copyright laws. Renesas
 * reserves the right to change or discontinue this software and/or this documentation. THE SOFTWARE AND DOCUMENTATION
 * IS DELIVERED TO YOU "AS IS," AND RENESAS MAKES NO REPRESENTATIONS OR WARRANTIES, AND TO THE FULLEST EXTENT
 * PERMISSIBLE UNDER APPLICABLE LAW, DISCLAIMS ALL WARRANTIES, WHETHER EXPLICITLY OR IMPLICITLY, INCLUDING WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT, WITH RESPECT TO THE SOFTWARE OR
 * DOCUMENTATION.  RENESAS SHALL HAVE NO LIABILITY ARISING OUT OF ANY SECURITY VULNERABILITY OR BREACH.  TO THE MAXIMUM
 * EXTENT PERMITTED BY LAW, IN NO EVENT WILL RENESAS BE LIABLE TO YOU IN CONNECTION WITH THE SOFTWARE OR DOCUMENTATION
 * (OR ANY PERSON OR ENTITY CLAIMING RIGHTS DERIVED FROM YOU) FOR ANY LOSS, DAMAGES, OR CLAIMS WHATSOEVER, INCLUDING,
 * WITHOUT LIMITATION, ANY DIRECT, CONSEQUENTIAL, SPECIAL, INDIRECT, PUNITIVE, OR INCIDENTAL DAMAGES; ANY LOST PROFITS,
 * OTHER ECONOMIC DAMAGE, PROPERTY DAMAGE, OR PERSONAL INJURY; AND EVEN IF RENESAS HAS BEEN ADVISED OF THE POSSIBILITY
 * OF SUCH LOSS, DAMAGES, CLAIMS OR COSTS.
 **********************************************************************************************************************/
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



