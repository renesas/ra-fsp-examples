/***********************************************************************************************************************
 * File Name    : filex_thread_entry.c
 * Description  : Contains functions from the filex thread
 ***********************************************************************************************************************/
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
 * Copyright (C) 2024 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
#include "filex_thread.h"
#include "filex.h"
#include "terminal.h"

/* FileX Thread entry function */
void filex_thread_entry(void)
{
    UINT status = FX_SUCCESS;
    uint8_t input = RESET_VALUE;

    /* Suspend until the terminal thread has run at least once */
    terminal_thread_init_check();

    /* Send a output queue to the terminal thread to display EP banner */
    terminal_send_output_queue(TERMINAL_OUTPUT_EP_BANNER, RESET_VALUE, NULL);
    tx_thread_sleep (FILEX_SLEEP_TICK);

    /* Send a output queue to the terminal thread to display EP information */
    terminal_send_output_queue(TERMINAL_OUTPUT_EP_INFO, RESET_VALUE, NULL);
    tx_thread_sleep (FILEX_SLEEP_TICK);

    /* Initialize the FileX file system */
    status = file_system_init();
    if (FX_SUCCESS != status)
    {
        PRINT_ERR_STR("**file_system_init failed**\r\n");
        ERROR_TRAP(status);
    }

    while (true)
    {
        /* Send a output queue to the terminal thread to display menu option */
        terminal_send_output_queue(TERMINAL_OUTPUT_APP_MENU, RESET_VALUE, NULL);

        /* Suspend until there is an input queue */
        status = terminal_get_input_queue(&input);
        if (TX_SUCCESS != status)
        {
            PRINT_ERR_STR("**terminal_get_input_queue failed**\r\n");
            ERROR_TRAP(status);
        }

        /* Perform file system operations as required by the user */
        status = file_system_operation(input);
        if (FX_SUCCESS != status)
        {
            PRINT_ERR_STR("**file_system_operation failed**\r\n");
            ERROR_TRAP(status);
        }

        tx_thread_sleep (FILEX_SLEEP_TICK);
    }
}
