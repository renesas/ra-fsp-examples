/***********************************************************************************************************************
 * File Name    : terminal_example_app.c
 * Description  : Contains data structures and functions used in terminal_example_app.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ***********************************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include "terminal_example_app.h"
#include "console/uart_console.h"

/* Private variable */
static CHAR * sp_menu_content[MENU_MAX] =
{
    [MENU_MAIN] = MAIN_MENU,
    [MENU_FLASH_MEDIA] = FLASH_MEDIA_MENU,
    [MENU_DIR] = DIRECTORY_MENU,
    [MENU_FILE] = FILE_MENU,
    [MENU_SECTOR] = SECTOR_MENU
};

/* Private function */
static void conv_ul64_to_str(ULONG64 value, CHAR * p_buf, UINT len);

/*******************************************************************************************************************//**
 * @brief     Terminal Thread entry-point for the FileX/LevelX example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void terminal_thread_entry_run(void)
{
    UINT status = TX_SUCCESS;

    /* Initialize the UART console */
    status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        /* Error trap */
        thread_suspend();
    }

    status = term_framework_init();
    if (TX_SUCCESS != status)
    {
        printf("\r\nERROR: term_framework_init failed\r\n");
        printf("\r\nReturned Error Code: 0x%x\r\n", status);
        thread_suspend();
    }

    /* Send a start event to the Console Thread */
    status = (UINT)tx_event_flags_set(&g_request_event, REQUEST_START_CONSOLE, TX_OR);
    if (TX_SUCCESS != status)
    {
        printf("\r\nERROR: tx_event_flags_set failed\r\n");
        thread_suspend();
    }
    thread_sleep(TERMINAL_SLEEP_TICK);

    while (true)
    {
        /* Handle user input from the terminal */
        status = terminal_input_handle();
        if (TX_SUCCESS != status)
        {
            printf("\r\nERROR: terminal_input_handle failed\r\n");
            printf("\r\nReturned Error Code: 0x%x\r\n", status);
            thread_suspend();
        }

        /* Handle other threads that need output to the terminal application via the terminal interface */
        status = terminal_output_handle();
        if (TX_SUCCESS != status)
        {
            printf("\r\nERROR: terminal_output_handle failed\r\n");
            printf("\r\nReturned Error Code: 0x%x\r\n", status);
            thread_suspend();
        }
        thread_sleep(TERMINAL_SLEEP_TICK);
    }
}
/***********************************************************************************************************************
* End of function terminal_thread_entry_run
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: terminal_output_handle
 * Description  : This function transfers a message to the PC based on the terminal frame taken from the terminal
 *                output queue.
 * Arguments    : None.
 * Return Value : TX_SUCCESS                Handled successfully.
 *                Any other error codes     Failed to handle.
 **********************************************************************************************************************/
UINT terminal_output_handle(void)
{
    UINT status = TX_SUCCESS;
    UINT err_code = RESET_VALUE;
    uint8_t menu = RESET_VALUE;
    terminal_msg_t * p_terminal_msg = NULL;
    CHAR str_size[STR_UL64_MAX_LEN] = {RESET_VALUE};
    CHAR str_size_align[STR_UL64_MAX_LEN] = {RESET_VALUE};
    fsp_pack_version_t version = {RESET_VALUE};
    entry_info_t entry =
    {
        .name = {NULL_CHAR},
        .attr = RESET_VALUE,
        .size = RESET_VALUE,
        .time =
        {
            .month = RESET_VALUE,
            .date = RESET_VALUE,
            .year = RESET_VALUE,
            .hour = RESET_VALUE,
            .min = RESET_VALUE,
            .sec = RESET_VALUE
        }
    };
    dir_property_t dir =
    {
        .file = RESET_VALUE,
        .subdir = RESET_VALUE,
        .size = RESET_VALUE
    };
    media_property_t media =
    {
        .phy_size = RESET_VALUE,
        .total_size = RESET_VALUE,
        .free_size = RESET_VALUE,
        .format_type = RESET_VALUE
    };
    sector_info_t sector =
    {
        .sector_map_entry = NULL,
        .sector_address = NULL,
        .logical_sector = RESET_VALUE
    };

    while (true)
    {
        /* Retrieve a message from the output queue */
        status = term_get_output_queue((void *) &p_terminal_msg, TX_NO_WAIT);
        if (TX_SUCCESS == status)
        {
            switch (p_terminal_msg->id)
            {
                case TERMINAL_OUTPUT_EP_BANNER:
                {
                    /* Get FSP package information */
                    R_FSP_VersionGet(&version);

                    /* Print the EP banner to the terminal application */
                    printf(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor,
                           version.version_id_b.patch);
                    break;
                }

                case TERMINAL_OUTPUT_EP_INFO:
                {
                    /* Print the EP information to the terminal application on the PC */
                    printf(EP_INFO);
                    break;
                }

                case TERMINAL_OUTPUT_APP_INFO_STR:
                {
                    /* Print the application information */
                    printf("%s", p_terminal_msg->msg);
                    break;
                }

                case TERMINAL_OUTPUT_APP_ERR_STR:
                {
                    /* Print the application error */
                    printf("\r\nERROR : %s\r\n", p_terminal_msg->msg);
                    break;
                }

                case TERMINAL_OUTPUT_APP_ERR_TRAP:
                {
                    /* Print error code */
                    err_code = *(UINT *) p_terminal_msg->msg;
                    printf("\r\nReturned Error Code: 0x%x\r\n", err_code);
                    thread_suspend();
                    break;
                }

                case TERMINAL_OUTPUT_APP_MENU:
                {
                    /* Print application menu */
                    menu = *(uint8_t *) p_terminal_msg->msg;
                    printf("%s", sp_menu_content[menu]);
                    break;
                }

                case TERMINAL_OUTPUT_APP_ENTRY_INFO:
                {
                    /* Print entry information */
                    entry = *(entry_info_t *) p_terminal_msg->msg;
                    if (FX_DIRECTORY == (entry.attr & FX_DIRECTORY))
                    {
                        /* Print the directory information */
                        printf("%.2u/%.2u/%.4u  %.2u:%.2u:%.2u    %s %s\r\n",
                               entry.time.month, entry.time.date, entry.time.year,
                               entry.time.hour, entry.time.min, entry.time.sec,
                               "<DIR>          ", entry.name);
                    }
                    else if (FX_ARCHIVE == (entry.attr & FX_ARCHIVE))
                    {
                        /* Print the file information */
                        conv_ul64_to_str(entry.size, str_size, STR_UL64_MAX_LEN);
                        snprintf(str_size_align, STR_UL64_MAX_LEN, "%15s", str_size);
                        printf("%.2u/%.2u/%.4u  %.2u:%.2u:%.2u    %s %s\r\n",
                               entry.time.month, entry.time.date, entry.time.year,
                               entry.time.hour, entry.time.min, entry.time.sec,
                               str_size_align, entry.name);
                    }
                    else
                    {
                        /* Do nothing */
                    }
                    break;
                }

                case TERMINAL_OUTPUT_APP_DIR_PROPERTY:
                {
                    /* Print the directory property */
                    dir = *(dir_property_t *) p_terminal_msg->msg;
                    conv_ul64_to_str(dir.size, str_size, STR_UL64_MAX_LEN);
                    snprintf(str_size_align, STR_UL64_MAX_LEN, "%15s", str_size);
                    printf("         %6u File(s) %s bytes\r\n", dir.file, str_size_align);
                    printf("         %6u Dir(s)\r\n\r\n", dir.subdir);
                    break;
                }

                case TERMINAL_OUTPUT_APP_MEDIA_PROPERTY:
                {
                    /* Print the media property */
                    media = *(media_property_t *) p_terminal_msg->msg;
                    conv_ul64_to_str(media.total_size, str_size, STR_UL64_MAX_LEN);
                    printf("\r\nMedia total size is:   %15s bytes\r\n", str_size);
                    conv_ul64_to_str(media.free_size, str_size, STR_UL64_MAX_LEN);
                    printf("Media free size is:    %15s bytes\r\n", str_size);
                    printf("Format type of flash is:    %s\r\n", media.format_type);
                    break;
                }

                case TERMINAL_OUTPUT_SECTOR_INFO:
                {
                    /* Print the sector information */
                    sector = *(sector_info_t *) p_terminal_msg->msg;
                    printf("\r\nSector allocated at address:            0x%x\r\n", (unsigned int)sector.sector_address);
                    break;
                }

                default:
                {
                    break;
                }
            }
            uart_console_flush();

            /* Release the terminal message allocated block */
            status = tx_byte_release((void *)p_terminal_msg);
            if (TX_SUCCESS != status)
            {
                printf("\r\nERROR : tx_byte_release failed\r\n");
                uart_console_flush();
                return status;
            }
        }
        else if (TX_QUEUE_EMPTY == status)
        {
            /* Output queue is empty */
            return TX_SUCCESS;
        }
        else
        {
            printf("\r\nERROR : tx_queue_receive failed\r\n");
            uart_console_flush();
            return status;
        }
    }
}
/***********************************************************************************************************************
* End of function terminal_output_handle
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: terminal_input_handle
 * Description  : This function checks if there is any data input from the terminal, reads it, and sends it to the
 *                input queue.
 * Arguments    : None.
 * Return Value : TX_SUCCESS                Handled successfully.
 *                Any other error codes     Failed to handle.
 **********************************************************************************************************************/
UINT terminal_input_handle(void)
{
    UINT status = TX_SUCCESS;
    UINT read_len = RESET_VALUE;
    CHAR read_buff[TERM_BUFFER_SIZE];

    if (uart_console_has_char())
    {
        read_len = uart_console_read(read_buff, sizeof(read_buff));
        if (read_len)
        {
            status = term_send_input_queue(TERMINAL_INPUT_MESSAGE, read_buff, read_len);
        }
    }
    return status;
}
/***********************************************************************************************************************
* End of function terminal_input_handle
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: conv_ul64_to_str
 * Description  : This function converts a ULONG64 value to a string.
 * Arguments    : value                     Value of type ULONG64.
 *                p_buf                     Pointer to the string buffer.
 *                len                       Length of the input buffer.
 * Return Value : None.
 **********************************************************************************************************************/
static void conv_ul64_to_str(ULONG64 value, CHAR * p_buf, UINT len)
{
    UINT units = RESET_VALUE;
    UINT thousands = RESET_VALUE;
    UINT millions = RESET_VALUE;
    UINT billions = RESET_VALUE;

    /* Clean the output buffer */
    memset(p_buf, NULL_CHAR, len);

    /* Check the value range */
    if (BILLION <= value)
    {
        billions = (UINT)(value / BILLION);
        millions = (UINT)((value / MILLION) % THOUSAND);
        thousands = (UINT)((value / THOUSAND) % THOUSAND);
        units = (UINT)(value % THOUSAND);
        snprintf(p_buf, len, "%u,%.3u,%.3u,%.3u", billions, millions, thousands, units);
    }
    else if (MILLION <= value)
    {
        millions = (UINT)(value / MILLION);
        thousands = (UINT)((value / THOUSAND) % THOUSAND);
        units = (UINT)(value % THOUSAND);
        snprintf(p_buf, len, "%u,%.3u,%.3u", millions, thousands, units);
    }
    else if (THOUSAND <= value)
    {
        thousands = (UINT)(value / THOUSAND);
        units = (UINT)(value % THOUSAND);
        snprintf(p_buf, len, "%u,%.3u", thousands, units);
    }
    else
    {
        units = (UINT) value;
        snprintf(p_buf, len, "%u", units);
    }
}
/***********************************************************************************************************************
* End of function conv_ul64_to_str
***********************************************************************************************************************/
