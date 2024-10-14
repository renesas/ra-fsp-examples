/***********************************************************************************************************************
 * File Name    : filex_thread_entry.c
 * Description  : Contains functions from the filex thread
 ***********************************************************************************************************************/
/**********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
**********************************************************************************************************************/
#include "filex_thread.h"
#include "filex.h"
#include "terminal.h"

/***********************************************************************************************************************
 *  Function Name: filex_thread_entry
 *  Description  : This function performs FileX Thread entry operation.
 *  Arguments    : None
 *  Return Value : None
 **********************************************************************************************************************/
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
/***********************************************************************************************************************
* End of function filex_thread_entry
***********************************************************************************************************************/
