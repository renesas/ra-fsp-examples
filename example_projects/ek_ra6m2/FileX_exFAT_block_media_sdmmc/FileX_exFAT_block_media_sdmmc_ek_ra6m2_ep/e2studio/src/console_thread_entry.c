#include "console_thread.h"
#include "console.h"

static UINT console_handle(menu_t * p_menu, uint8_t * p_input, ULONG * p_request);

/* Console Thread entry function */
void console_thread_entry(void)
{
    UINT status = TX_SUCCESS;
    ULONG actual_event = RESET_VALUE;
    ULONG request = RESET_VALUE;
    uint8_t input = RESET_VALUE;
    menu_t menu = MENU_MAIN;

    /* Suspend thread until received start console event flag */
    status = tx_event_flags_get (&g_request_event, REQUEST_START_CONSOLE,
                                 TX_OR_CLEAR, &actual_event, TX_WAIT_FOREVER);
    if (TX_SUCCESS != status)
    {
        PRINT_ERR_STR("tx_event_flags_get for REQUEST_START_CONSOLE failed\r\n");
        ERROR_TRAP(status);
    }

    /* Send a output queue to the RTT thread to display EP banner */
    send_data_to_rtt(RTT_OUTPUT_EP_BANNER, RESET_VALUE, NULL);
    tx_thread_sleep (PRINT_EP_BANNER_TICK);

    /* Send a output queue to the RTT thread to display EP information */
    send_data_to_rtt(RTT_OUTPUT_EP_INFO, RESET_VALUE, NULL);
    tx_thread_sleep (PRINT_EP_INFO_TICK);

    /* Send a start file system event to the FileX thread */
    status = tx_event_flags_set (&g_request_event, REQUEST_START_FILE_SYSTEM, TX_OR);
    if (TX_SUCCESS != status)
    {
        PRINT_ERR_STR("tx_event_flags_set for REQUEST_START_FILE_SYSTEM failed\r\n");
        ERROR_TRAP(status);
    }

    tx_thread_sleep (CONSOLE_THREAD_SLEEP_TICK);

    while (true)
    {
        /* Send a output queue to the RTT thread to display menu */
        PRINT_MENU(menu);

        /* Wait for user input  */
        status = get_data_from_rtt(&input);
        if (TX_SUCCESS != status)
        {
            PRINT_ERR_STR("get_data_from_rtt failed\r\n");
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
            /* Send a request event to the FileX Thread */
            status = tx_event_flags_set (&g_request_event, (request & FILE_SYSTEM_REQUEST_MASK), TX_OR);
            if (TX_SUCCESS != status)
            {
                PRINT_ERR_STR("tx_event_flags_set for user request failed\r\n");
                ERROR_TRAP(status);
            }

            /* Wait for request completed event from the FileX Thread */
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


/*******************************************************************************************************************//**
 * @brief       This function takes input data to process the menu state and return the user request.
 * @param[in]   p_menu      pointer to menu state
 * @param[in]   p_input     pointer to user input
 * @param[out]  p_request   pointer to user request
 * @retval      Any Other Error code apart from TX_SUCCESS on Unsuccessful operation.
 **********************************************************************************************************************/
static UINT console_handle(menu_t * p_menu, uint8_t * p_input, ULONG * p_request)
{
    /* Check null pointer */
    if (NULL == p_menu || NULL == p_input || NULL == p_request)
    {
        return TX_PTR_ERROR;
    }

    /* Clean request value */
    *p_request = (request_event_t)RESET_VALUE;

    /* Handle */
    switch (*p_menu)
    {
        case MENU_MAIN:
            switch (*p_input)
            {
                case SELECT_MEDIA_OPERATION:
                    *p_menu = MENU_MEDIA;
                    break;

                case SELECT_DIR_OPERATION:
                    *p_menu = MENU_DIR;
                    break;

                case SELECT_FILE_OPERATION:
                    *p_menu = MENU_FILE;
                    break;

                default:
                    PRINT_INFO_STR("\r\nInvalid selection\r\n");
                    break;
            }
            break;

        case MENU_MEDIA:
            switch (*p_input)
            {
                case SELECT_MEDIA_OPEN:
                    *p_request = REQUEST_MEDIA_OPEN;
                    break;

                case SELECT_MEDIA_PROPERTY:
                    *p_request = REQUEST_MEDIA_PROPERTY;
                    break;

                case SELECT_MEDIA_FORMAT:
                    *p_request = REQUEST_MEDIA_FORMAT;
                    break;

                case SELECT_MEDIA_CLOSE:
                    *p_request = REQUEST_MEDIA_CLOSE;
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

        default:
            PRINT_INFO_STR("\r\nInvalid selection\r\n");
            break;
    }

    return TX_SUCCESS;
}
