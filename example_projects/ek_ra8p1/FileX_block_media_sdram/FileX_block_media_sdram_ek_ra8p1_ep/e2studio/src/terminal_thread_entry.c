/***********************************************************************************************************************
 * File Name    : terminal_thread_entry.c
 * Description  : Contains functions from the serial thread
 **********************************************************************************************************************/
/**********************************************************************************************************************
* Copyright (c) 2024 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
**********************************************************************************************************************/
#include "terminal.h"
#include "terminal_thread.h"

/***********************************************************************************************************************
 * @brief        This function performs Terminal Thread entry operation.
 * @param[IN]    None
 * @retval       None
 **********************************************************************************************************************/
void terminal_thread_entry(void)
{
    UINT status = TX_SUCCESS;

#if (USE_VIRTUAL_COM == 1)
    status = serial_init();

    if(TX_SUCCESS != status)
    {
        APP_ERR_TRAP(status);
    }
#endif /* USE_VIRTUAL_COM */

    status = terminal_framework_init();
    if(TX_SUCCESS != status)
    {
        terminal_printf("\r\nERROR: terminal_framework_init failed\r\n");
        terminal_printf("\r\nReturned error code: 0x%x\r\n", status);
        APP_ERR_TRAP(status);
    }

    while(true)
    {
        /* Handle user input from terminal */
        status = terminal_input_handle();
        if(TX_SUCCESS != status)
        {
            terminal_printf("\r\nERROR: terminal_input_hanlde Failed\r\n");
            terminal_printf("\r\nReturned error code: 0x%x\r\n", status);
            APP_ERR_TRAP(status);
        }

        /* Handle other threads that need output to the terminal application via terminal interface */
        status = terminal_output_handle();
        if(TX_SUCCESS != status)
        {
            terminal_printf("\r\nERROR: terminal_output_hanlde failed\r\n");
            terminal_printf("\r\nReturned error code: 0x%x\r\n", status);
            APP_ERR_TRAP(status);
        }

        tx_thread_sleep(TERMINAL_SLEEP_TICK);
    }
}
/***********************************************************************************************************************
* End of function terminal_thread_entry
***********************************************************************************************************************/
