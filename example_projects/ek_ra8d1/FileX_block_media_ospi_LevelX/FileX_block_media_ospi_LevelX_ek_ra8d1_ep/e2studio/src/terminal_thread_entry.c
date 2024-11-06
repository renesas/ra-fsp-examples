/***********************************************************************************************************************
 * File Name    : terminal_thread_entry.c
 * Description  : Contains functions from the terminal thread
 ***********************************************************************************************************************/
/**********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
**********************************************************************************************************************/
#include "terminal.h"
#include "terminal_thread.h"
#include "filex.h"

/***********************************************************************************************************************
 *  Function Name: terminal_thread_entry
 *  Description  : This function performs Terminal Thread entry operation.
 *  Arguments    : None
 *  Return Value : None
 **********************************************************************************************************************/
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

    /* Send a start event to the Console thread */
    status = (ULONG)tx_event_flags_set (&g_request_event,
                                           REQUEST_START_CONSOLE,
                                           TX_OR);
    if (TX_SUCCESS != status)
    {
    	terminal_printf("\r\nERROR: tx_event_flags_set failed\r\n");
        APP_ERR_TRAP(status);
    }

    tx_thread_sleep (TERMINAL_SLEEP_TICK);

    while (true)
    {
        /* Send a output queue to the Terminal thread to display menu */

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

