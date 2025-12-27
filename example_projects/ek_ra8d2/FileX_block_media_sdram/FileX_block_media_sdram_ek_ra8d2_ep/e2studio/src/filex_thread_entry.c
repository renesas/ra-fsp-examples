/***********************************************************************************************************************
 * File Name    : filex_thread_entry.c
 * Description  : Contains functions from the FileX thread.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "filex.h"
#include "filex_thread.h"

/***********************************************************************************************************************
 * @brief        This function performs FileX Thread entry operation.
 * @param[in]    None.
 * @retval       None.
 **********************************************************************************************************************/
void filex_thread_entry(void)
{
    UINT status = FX_SUCCESS;
    char input = RESET_VALUE;

    /* Suspend until the terminal thread has run at least once */
    terminal_thread_init_check();

    /* Send an output queue to the terminal thread to display EP banner */
    term_send_output_queue(TERMINAL_OUTPUT_EP_BANNER, NULL, RESET_VALUE);
    tx_thread_sleep(FILEX_SLEEP_TICK);

    /* Send an output queue to the terminal thread to display EP information */
    term_send_output_queue(TERMINAL_OUTPUT_EP_INFO, NULL, RESET_VALUE);
    tx_thread_sleep(FILEX_SLEEP_TICK);

    /* Initialize the FileX file system */
    status = file_system_init();
    TERM_ERR_TRAP(FX_SUCCESS != status, status, "**file_system_init failed**\r\n");

    while (true)
    {
        /* Send a output queue to the terminal thread to display menu option */
        term_send_output_queue(TERMINAL_OUTPUT_APP_MENU, NULL, RESET_VALUE);

        /* Suspend until there is an input queue */
        status = term_get_input_queue(&input, NULL, TX_WAIT_FOREVER);
        TERM_ERR_TRAP(TX_SUCCESS != status, status, "**term_get_input_queue failed**\r\n");

        /* Perform file system operations as required by the user */
        status = file_system_operation(atoi(&input));
        TERM_ERR_TRAP(FX_SUCCESS != status, status, "**file_system_operation failed**\r\n");

        tx_thread_sleep(FILEX_SLEEP_TICK);
    }
}
/***********************************************************************************************************************
* End of function filex_thread_entry
***********************************************************************************************************************/
