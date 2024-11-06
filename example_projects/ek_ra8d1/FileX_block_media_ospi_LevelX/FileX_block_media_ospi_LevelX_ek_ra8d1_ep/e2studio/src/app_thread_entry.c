/***********************************************************************************************************************
 * File Name    : app_thread_entry.c
 * Description  : Contains functions from the App thread
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#include "app_thread.h"
#include "filex.h"
#include "terminal.h"
#include "board_ospi.h"

/**********************************************************************************************************************
 *  Function Name: app_thread_entry
 *  Description  : This function performs App Thread entry operation.
 *  Arguments    : None
 *  Return Value : None
 *********************************************************************************************************************/
void app_thread_entry(void)
{
    UINT status = FX_SUCCESS;
    fsp_err_t err = FSP_SUCCESS;
    UINT lx_ret_val = LX_SUCCESS;
    ULONG actual_event = RESET_VALUE;

    /* Suspend thread until received start app thread event flag  */
    actual_event = RESET_VALUE;
    status = tx_event_flags_get (&g_request_event, REQUEST_START_APP_THREAD,
                                 TX_OR_CLEAR, &actual_event, TX_WAIT_FOREVER);
    if (TX_SUCCESS != status)
    {
        PRINT_ERR_STR("tx_event_flags_get for REQUEST_START_APP_THREAD failed\r\n");
        ERROR_TRAP(status);
    }

    /* Initialize the OSPI Flash */
    err = ospi_b_init ();
    if (FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("**ospi_b_init failed**\r\n");
        ERROR_TRAP(err);
    }

    /* Initialize the FileX file system */
    status = file_system_init();
    if (FX_SUCCESS != status)
    {
        PRINT_ERR_STR("**file_system_init failed**\r\n");
        ERROR_TRAP(status);
    }

    /* Initialize LevelX system */
    lx_ret_val = lx_nor_flash_initialize();
    if (LX_SUCCESS != lx_ret_val)
    {
        PRINT_ERR_STR("**lx_nor_flash_initialize failed**\r\n");
        ERROR_TRAP(lx_ret_val);
    }

    while (true)
    {
        /* Wait for a request event from the console thread */
        actual_event = RESET_VALUE;
        status = tx_event_flags_get (&g_request_event, FILE_SYSTEM_REQUEST_MASK,
                                     TX_OR_CLEAR, &actual_event, TX_WAIT_FOREVER);
        if (TX_SUCCESS != status)
        {
            PRINT_ERR_STR("tx_event_flags_get for user request events failed\r\n");
            ERROR_TRAP(status);
        }

        /* Perform file system operations as required by the user */
        status = filex_ospi_levelx_operation(actual_event);
        if (FX_SUCCESS != status)
        {
            PRINT_ERR_STR("file_system_operation failed\r\n");
            ERROR_TRAP(status);
        }

        /* Send a request complete event flag to the console thread */
        status = tx_event_flags_set (&g_request_event, REQUEST_COMPLETED, TX_OR);
        if (TX_SUCCESS != status)
        {
            PRINT_ERR_STR("tx_event_flags_set for REQUEST_COMPLETED event failed\r\n");
            ERROR_TRAP(status);
        }

        tx_thread_sleep (APP_SLEEP_TICK);
    }
}
/***********************************************************************************************************************
* End of function app_thread_entry
***********************************************************************************************************************/
