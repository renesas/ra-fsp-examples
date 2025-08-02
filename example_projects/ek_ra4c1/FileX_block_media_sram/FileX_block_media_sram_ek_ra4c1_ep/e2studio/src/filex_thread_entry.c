/***********************************************************************************************************************
 * File Name    : filex_thread_entry.c
 * Description  : Contains functions from the FileX thread
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "filex_thread.h"
#include "filex.h"

/* FileX Thread entry function */
void filex_thread_entry(void)
{
    UINT status = FX_SUCCESS;
    UINT len = RESET_VALUE;
    CHAR input [INPUT_MAX_SIZE];
    UINT select = RESET_VALUE;

    /* Suspend until the terminal thread has run at least once */
    terminal_thread_init_check();

    /* Send an output queue to the terminal thread to display EP banner */
    terminal_send_output_queue(TERMINAL_OUTPUT_EP_BANNER, NULL, RESET_VALUE);
    tx_thread_sleep(FILEX_SLEEP_TICK);

    /* Send an output queue to the terminal thread to display EP information */
    terminal_send_output_queue(TERMINAL_OUTPUT_EP_INFO, NULL, RESET_VALUE);
    tx_thread_sleep(FILEX_SLEEP_TICK);

    /* Initialize the FileX file system */
    status = file_system_init();
    ERROR_TRAP(FX_SUCCESS != status, status, "**file_system_init failed**\r\n");

    while (true)
    {
        /* Send an output queue to the terminal thread to display menu option */
        terminal_send_output_queue(TERMINAL_OUTPUT_APP_MENU, NULL, RESET_VALUE);

        /* Suspend until there is an input queue */
        status = terminal_get_input_queue(input, (uint32_t *)&len, TX_WAIT_FOREVER);
        ERROR_TRAP(FX_SUCCESS != status, status, "**terminal_get_input_queue failed**\r\n");

        /* Parse the user's selection and retrieve the network operation argument in string format */
        select = RESET_VALUE;
        sscanf(input, "%d", &select);

        /* Perform file system operations as required by the user */
        status = file_system_operation(select);
        ERROR_TRAP(FX_SUCCESS != status, status, "**file_system_operation failed**\r\n");

        tx_thread_sleep(FILEX_SLEEP_TICK);
    }
}
