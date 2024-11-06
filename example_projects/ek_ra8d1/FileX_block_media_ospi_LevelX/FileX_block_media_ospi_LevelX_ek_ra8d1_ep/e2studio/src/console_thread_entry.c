/***********************************************************************************************************************
 * File Name    : console_thread_entry.c
 * Description  : Contains functions from the console thread
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#include "console_thread.h"
#include "console.h"

static UINT console_handle(menu_t * p_menu, uint16_t * p_input, ULONG * p_request);

/**********************************************************************************************************************
 *  Function Name: console_thread_entry
 *  Description  : This function performs Console Thread entry operation.
 *  Arguments    : None
 *  Return Value : None
 *********************************************************************************************************************/
void console_thread_entry(void)
{
    UINT status = TX_SUCCESS;
    ULONG actual_event = RESET_VALUE;
    ULONG request = RESET_VALUE;
    uint16_t input = RESET_VALUE;
    menu_t menu = MENU_MAIN;

    /* Suspend thread until received start console event flag */
    status = tx_event_flags_get (&g_request_event, REQUEST_START_CONSOLE,
                                 TX_OR_CLEAR, &actual_event, TX_WAIT_FOREVER);
    if (TX_SUCCESS != status)
    {
        PRINT_ERR_STR("tx_event_flags_get for REQUEST_START_CONSOLE failed\r\n");
        ERROR_TRAP(status);
    }

    /* Send a output queue to the terminal thread to display EP banner */
    terminal_send_output_queue(TERMINAL_OUTPUT_EP_BANNER, RESET_VALUE, NULL);
    tx_thread_sleep (PRINT_EP_BANNER_TICK);

    /* Send a output queue to the terminal thread to display EP information */
    terminal_send_output_queue(TERMINAL_OUTPUT_EP_INFO, RESET_VALUE, NULL);
    tx_thread_sleep (PRINT_EP_BANNER_TICK);

    /* Send a start file system event to the APP thread */
    status = tx_event_flags_set (&g_request_event, REQUEST_START_APP_THREAD, TX_OR);
    if (TX_SUCCESS != status)
    {
        PRINT_ERR_STR("tx_event_flags_set for REQUEST_START_APP_THREAD failed\r\n");
        ERROR_TRAP(status);
    }

    tx_thread_sleep (CONSOLE_THREAD_SLEEP_TICK);

    while (true)
    {
        /* Send a output queue to the Terminal thread to display menu */
        PRINT_MENU(menu);

        /* Suspend until there is an input queue */
        status = terminal_get_input_queue(&input);
        if (TX_SUCCESS != status)
        {
            PRINT_ERR_STR("**terminal_get_input_queue failed**\r\n");
            ERROR_TRAP(status);
        }

        /* Handle user input base on the current menu */
        status = console_handle(&menu, &input, &request);
        if (TX_SUCCESS != status)
        {
            PRINT_ERR_STR("console_handle failed\r\n");
            ERROR_TRAP(status);
        }

        /* Handle valid user request */
        if (RESET_VALUE != (request & FILE_SYSTEM_REQUEST_MASK))
        {
            /* Send a request event to the App Thread */
            status = tx_event_flags_set (&g_request_event, (request & FILE_SYSTEM_REQUEST_MASK), TX_OR);
            if (TX_SUCCESS != status)
            {
                PRINT_ERR_STR("tx_event_flags_set for user request failed\r\n");
                ERROR_TRAP(status);
            }

            /* Wait for request completed event from the App Thread */
            status = tx_event_flags_get (&g_request_event, REQUEST_COMPLETED,
                                         TX_OR_CLEAR, &actual_event, TX_WAIT_FOREVER);
            if (TX_SUCCESS != status)
            {
                PRINT_ERR_STR("tx_event_flags_get for REQUEST_START_CONSOLE failed\r\n");
                ERROR_TRAP(status);
            }
        }

        tx_thread_sleep (CONSOLE_THREAD_SLEEP_TICK);
    }
}
/**********************************************************************************************************************
* End of function console_thread_entry
**********************************************************************************************************************/

/**********************************************************************************************************************
 *  Function Name: console_handle
 *  Description  : This function takes input data to process the menu state and return the user request.
 *  Arguments    : p_menu      pointer to menu state
 *  			   p_input     pointer to user input
 *  			   p_request   pointer to user request
 *  Return Value : TX_SUCCESS    Upon successful operation
 *                 Any Other Error code apart from FX_SUCCESS
 *********************************************************************************************************************/
static UINT console_handle(menu_t * p_menu, uint16_t * p_input, ULONG * p_request)
{
    /* Check null pointer */
    if (NULL == p_menu || NULL == p_input || NULL == p_request)
    {
        return TX_PTR_ERROR;
    }

    /* Clean request value */
    *p_request = (request_event_t)RESET_VALUE;

    /* Handle selected menu*/
    switch (*p_menu)
    {
        case MENU_MAIN:
            switch (*p_input)
            {
                case SELECT_FLASH_MEDIA_OPERATION:
                    *p_menu = MENU_FLASH_MEDIA;
                    break;

                case SELECT_DIR_OPERATION:
                    *p_menu = MENU_DIR;
                    break;

                case SELECT_FILE_OPERATION:
                    *p_menu = MENU_FILE;
                    break;

                case SELECT_SECTOR_OPERATION:
                    *p_menu = MENU_SECTOR;
                    break;

                default:
                    PRINT_INFO_STR("\r\nInvalid selection\r\n");
                    break;
            }
            break;

        case MENU_FLASH_MEDIA:
            switch (*p_input)
            {
                case SELECT_ENTIRE_FLASH_ERASE:
                    *p_request = REQUEST_ERASE_FLASH;
                    break;

                case SELECT_MEDIA_FORMAT:
                    *p_request = REQUEST_MEDIA_FORMAT;
                    break;

                case SELECT_MEDIA_OPEN:
                    *p_request = REQUEST_MEDIA_OPEN;
                    break;

                case SELECT_FLASH_DEFRAGMENT:
                    *p_request = REQUEST_DEFRAGMENT_FLASH;
                    break;

                case SELECT_MEDIA_GO_BACK:
                    *p_menu = MENU_MAIN;
                    break;

                default:
                    PRINT_INFO_STR("\r\nInvalid selection\r\n");
                    break;
            }
            break;

        case MENU_DIR:
            switch (*p_input)
            {
                case SELECT_DIR_CREATE:
                    *p_request = REQUEST_DIR_CREATE;
                    break;

                case SELECT_DIR_PROPERTY:
                    *p_request = REQUEST_DIR_PROPERTY;
                    break;

                case SELECT_DIR_DELETE:
                    *p_request = REQUEST_DIR_DELETE;
                    break;

                case SELECT_DIR_GO_BACK:
                    *p_menu = MENU_MAIN;
                    break;

                default:
                    PRINT_INFO_STR("\r\nInvalid selection\r\n");
                    break;
            }
            break;

        case MENU_FILE:
            switch (*p_input)
            {
                case SELECT_FILE_CREATE:
                    *p_request = REQUEST_FILE_CREATE;
                    break;

                case SELECT_FILE_WRITE:
                    *p_request = REQUEST_FILE_WRITE;
                    break;

                case SELECT_FILE_READ:
                    *p_request = REQUEST_FILE_READ;
                    break;

                case SELECT_FILE_DELETE:
                    *p_request = REQUEST_FILE_DELETE;
                    break;

                case SELECT_FILE_GO_BACK:
                    *p_menu = MENU_MAIN;
                    break;

                default:
                    PRINT_INFO_STR("\r\nInvalid selection\r\n");
                    break;
            }
            break;

            case MENU_SECTOR:
                switch (*p_input)
                {
                    case SELECT_SECTOR_WRITE:
                        *p_request = REQUEST_SECTOR_WRITE;
                        break;

                    case SELECT_SECTOR_READ:
                        *p_request = REQUEST_SECTOR_READ;
                        break;

                    case SELECT_SECTOR_ERASE:
                        *p_request = REQUEST_SECTOR_ERASE;
                        break;

                    case SELECT_SECTOR_GO_BACK:
                        *p_menu = MENU_MAIN;
                        break;

                    default:
                        PRINT_INFO_STR("\r\nInvalid selection\r\n");
                        break;
                }
                break;

        default:
            PRINT_INFO_STR("\r\nInvalid selection\r\n");
            break;
    }

    return TX_SUCCESS;
}
/**********************************************************************************************************************
* End of function console_handle
**********************************************************************************************************************/
