/***********************************************************************************************************************
 * File Name    : console_example_app.c
 * Description  : Contains data structures and functions used in console_example_app.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ***********************************************************************************************************************/

#include <stdlib.h>
#include "console_example_app.h"

/* Private function prototype */
static UINT console_handle(menu_t * p_menu, uint16_t * p_input, ULONG * p_request);

/*******************************************************************************************************************//**
 * @brief     Console Thread entry-point for the FileX/LevelX example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void console_thread_entry_run(void)
{
    UINT status = TX_SUCCESS;
    ULONG actual_event = RESET_VALUE;
    ULONG request = RESET_VALUE;
    uint16_t input = RESET_VALUE;
    menu_t menu = MENU_MAIN;
    char recv_buf[TERM_BUFFER_SIZE] = {RESET_VALUE};

    /* Suspend thread until received start console event flag */
    status = get_tx_flags(REQUEST_START_CONSOLE, &actual_event);
    if (TX_SUCCESS != status)
    {
        PRINT_ERR_STR("get_tx_flags for REQUEST_START_CONSOLE failed\r\n");
        PRINT_ERROR_TRAP(status);
    }

    /* Send an output queue to the terminal thread to display the EP banner */
    term_send_output_queue(TERMINAL_OUTPUT_EP_BANNER, NULL, RESET_VALUE);
    thread_sleep(PRINT_EP_BANNER_TICK);

    /* Send an output queue to the terminal thread to display the EP information */
    term_send_output_queue(TERMINAL_OUTPUT_EP_INFO, NULL, RESET_VALUE);
    thread_sleep(PRINT_EP_INFO_TICK);

    /* Send a start file system event to the App Thread */
    status = set_tx_flags(REQUEST_START_APP_THREAD);
    if (TX_SUCCESS != status)
    {
        PRINT_ERR_STR("set_tx_flags for REQUEST_START_APP_THREAD failed\r\n");
        PRINT_ERROR_TRAP(status);
    }

    thread_sleep(CONSOLE_THREAD_SLEEP_TICK);

    while (true)
    {
        /* Send an output queue to the terminal thread to display the menu */
        term_send_output_queue(TERMINAL_OUTPUT_APP_MENU, &menu, sizeof(menu));

        /* Suspend until there is an input queue */
        status = term_get_input_queue(recv_buf, NULL, TX_WAIT_FOREVER);
        if (TX_SUCCESS != status)
        {
            PRINT_ERR_STR("term_get_input_queue failed\r\n");
            PRINT_ERROR_TRAP(status);
        }

        /* Convert the input string to an integer value */
        input = (uint16_t)(atoi(recv_buf));

        /* Handle the user input based on the current menu */
        status = console_handle(&menu, &input, &request);
        if (TX_SUCCESS != status)
        {
            PRINT_ERR_STR("console_handle failed\r\n");
            PRINT_ERROR_TRAP(status);
        }

        /* Handle the valid user request */
        if (RESET_VALUE != (request & FILE_SYSTEM_REQUEST_MASK))
        {
            /* Send a request event to the App Thread */
            status = set_tx_flags(request & FILE_SYSTEM_REQUEST_MASK);
            if (TX_SUCCESS != status)
            {
                PRINT_ERR_STR("tx_event_flags_set for user request failed\r\n");
                PRINT_ERROR_TRAP(status);
            }

            /* Wait for the request completed event from the App Thread */
            status = get_tx_flags(REQUEST_COMPLETED, &actual_event);
            if (TX_SUCCESS != status)
            {
                PRINT_ERR_STR("tx_event_flags_get for REQUEST_COMPLETED failed\r\n");
                PRINT_ERROR_TRAP(status);
            }
        }
        thread_sleep(CONSOLE_THREAD_SLEEP_TICK);
    }
}
/***********************************************************************************************************************
* End of function console_thread_entry_run
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: console_handle
 * Description  : This function takes input data to process the menu state and returns the user request.
 * Arguments    : p_menu                    Pointer to the menu state.
 *                p_input                   Pointer to the user input.
 *                p_request                 Pointer to the user request.
 * Return Value : TX_SUCCESS                Handled successfully.
 *                Any other error codes     Failed to handle.
 **********************************************************************************************************************/
static UINT console_handle(menu_t * p_menu, uint16_t * p_input, ULONG * p_request)
{
    /* Check null pointer */
    if ((NULL == p_menu) || (NULL == p_input) || (NULL == p_request))
    {
        return TX_PTR_ERROR;
    }

    /* Clean the request value */
    *p_request = (ULONG) RESET_VALUE;

    /* Handle the selected menu */
    switch (*p_menu)
    {
        case MENU_MAIN:
        {
            switch (*p_input)
            {
                case SELECT_FLASH_MEDIA_OPERATION:
                {
                    *p_menu = MENU_FLASH_MEDIA;
                    break;
                }

                case SELECT_DIR_OPERATION:
                {
                    *p_menu = MENU_DIR;
                    break;
                }

                case SELECT_FILE_OPERATION:
                {
                    *p_menu = MENU_FILE;
                    break;
                }

                case SELECT_SECTOR_OPERATION:
                {
                    *p_menu = MENU_SECTOR;
                    break;
                }

                default:
                {
                    PRINT_INFO_STR("\r\nInvalid selection\r\n");
                    break;
                }
            }
            break;
        }

        case MENU_FLASH_MEDIA:
        {
            switch (*p_input)
            {
                case SELECT_ENTIRE_FLASH_ERASE:
                {
                    *p_request = REQUEST_ERASE_FLASH;
                    break;
                }

                case SELECT_MEDIA_FORMAT:
                {
                    *p_request = REQUEST_MEDIA_FORMAT;
                    break;
                }

                case SELECT_MEDIA_OPEN:
                {
                    *p_request = REQUEST_MEDIA_OPEN;
                    break;
                }

                case SELECT_FLASH_DEFRAGMENT:
                {
                    *p_request = REQUEST_DEFRAGMENT_FLASH;
                    break;
                }

                case SELECT_MEDIA_GO_BACK:
                {
                    *p_menu = MENU_MAIN;
                    break;
                }

                default:
                {
                    PRINT_INFO_STR("\r\nInvalid selection\r\n");
                    break;
                }
            }
            break;
        }

        case MENU_DIR:
        {
            switch (*p_input)
            {
                case SELECT_DIR_CREATE:
                {
                    *p_request = REQUEST_DIR_CREATE;
                    break;
                }

                case SELECT_DIR_PROPERTY:
                {
                    *p_request = REQUEST_DIR_PROPERTY;
                    break;
                }

                case SELECT_DIR_DELETE:
                {
                    *p_request = REQUEST_DIR_DELETE;
                    break;
                }

                case SELECT_DIR_GO_BACK:
                {
                    *p_menu = MENU_MAIN;
                    break;
                }

                default:
                {
                    PRINT_INFO_STR("\r\nInvalid selection\r\n");
                    break;
                }
            }
            break;
        }

        case MENU_FILE:
        {
            switch (*p_input)
            {
                case SELECT_FILE_CREATE:
                {
                    *p_request = REQUEST_FILE_CREATE;
                    break;
                }

                case SELECT_FILE_WRITE:
                {
                    *p_request = REQUEST_FILE_WRITE;
                    break;
                }

                case SELECT_FILE_READ:
                {
                    *p_request = REQUEST_FILE_READ;
                    break;
                }

                case SELECT_FILE_DELETE:
                {
                    *p_request = REQUEST_FILE_DELETE;
                    break;
                }

                case SELECT_FILE_GO_BACK:
                {
                    *p_menu = MENU_MAIN;
                    break;
                }

                default:
                {
                    PRINT_INFO_STR("\r\nInvalid selection\r\n");
                    break;
                }
            }
            break;
        }

        case MENU_SECTOR:
        {
            switch (*p_input)
            {
                case SELECT_SECTOR_WRITE:
                {
                    *p_request = REQUEST_SECTOR_WRITE;
                    break;
                }

                case SELECT_SECTOR_READ:
                {
                    *p_request = REQUEST_SECTOR_READ;
                    break;
                }

                case SELECT_SECTOR_ERASE:
                {
                    *p_request = REQUEST_SECTOR_ERASE;
                    break;
                }

                case SELECT_SECTOR_GO_BACK:
                {
                    *p_menu = MENU_MAIN;
                    break;
                }

                default:
                {
                    PRINT_INFO_STR("\r\nInvalid selection\r\n");
                    break;
                }
            }
            break;
        }

        default:
        {
            PRINT_INFO_STR("\r\nInvalid selection\r\n");
            break;
        }
    }

    return TX_SUCCESS;
}
/***********************************************************************************************************************
* End of function console_handle
***********************************************************************************************************************/
