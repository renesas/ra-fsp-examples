/***********************************************************************************************************************
 * File Name    : terminal_thread_entry.c
 * Description  : Contains functions from the serial thread
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
#include "terminal.h"
#include "terminal_thread.h"

/* Terminal Thread entry function */
void terminal_thread_entry(void)
{
    UINT status = TX_SUCCESS;

#if (USE_VIRTUAL_COM == 1)
    status = serial_init();

    if (TX_SUCCESS != status)
    {
        APP_ERR_TRAP(status);
    }
#endif /* USE_VIRTUAL_COM */

    status = terminal_framework_init();
    if (TX_SUCCESS != status)
    {
        terminal_printf("\r\nERROR: terminal_framework_init Failed\r\n");
        terminal_printf("\r\nReturned Error Code: 0x%x  \r\n", status);
        APP_ERR_TRAP(status);
    }

    while (true)
    {
        /* Handle user input from terminal  */
        status = terminal_input_handle();
        if (TX_SUCCESS != status)
        {
            terminal_printf("\r\nERROR: terminal_input_hanlde Failed\r\n");
            terminal_printf("\r\nReturned Error Code: 0x%x  \r\n", status);
            APP_ERR_TRAP(status);
        }

        /* Handle other threads that need output to the terminal application via terminal interface */
        status = terminal_output_handle();
        if (TX_SUCCESS != status)
        {
            terminal_printf("\r\nERROR: terminal_output_hanlde Failed\r\n");
            terminal_printf("\r\nReturned Error Code: 0x%x  \r\n", status);
            APP_ERR_TRAP(status);
        }

        tx_thread_sleep (TERMINAL_SLEEP_TICK);
    }
}
