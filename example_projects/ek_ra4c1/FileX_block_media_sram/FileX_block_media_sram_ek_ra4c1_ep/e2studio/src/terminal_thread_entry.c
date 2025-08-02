/***********************************************************************************************************************
* File Name    : terminal_thread_entry.c
* Description  : Contains data structures and functions used in the terminal thread
***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include <terminal_thread.h>
#include "common_app.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/
static UINT terminal_input_handle(void);
static UINT terminal_output_handle(void);

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/
/*******************************************************************************************************************//**
 * @brief     This function initializes the terminal communication, starts the network link thread, and enters a loop
 *            to handle user input and output for terminal communication.
 * @param[in] None
 * @retval    None
 **********************************************************************************************************************/
void terminal_thread_entry(void)
{
    UINT status = TX_SUCCESS;

    /* Initialize the terminal communication */
#if (USE_VIRTUAL_COM == 1U)
    /* Initialize the UART module to use UART as a terminal interface */
    status = terminal_init();
    APP_ERR_TRAP(status);
#else
    /* Initialize the SEGGER RTT to use SEGGER RTT Viewer as a terminal interface */
    terminal_init();
#endif

    /* Initialize the necessary resources for the terminal communication */
    status = terminal_framework_init();
    APP_ERR_TRAP(status);

    while (true)
    {
        /* Handle user input from the terminal */
        status = terminal_input_handle();
        APP_ERR_TRAP(status);

        /* Handle output to the terminal application via the terminal interface */
        status = terminal_output_handle();
        APP_ERR_TRAP(status);

        tx_thread_sleep (TERMINAL_THREAD_SLEEP_TICK);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function takes input data from the PC then puts it into the terminal input queue.
 * @param[in] None
 * @retval    Any other error code apart from TX_SUCCESS on unsuccessful operation.
 **********************************************************************************************************************/
static UINT terminal_input_handle(void)
{
    UINT read_len = RESET_VALUE;
    CHAR read_buff [TERMINAL_BUFFER_SIZE];

    if(terminal_has_data())
    {
        read_len = terminal_read(read_buff, sizeof(read_buff));
        if (read_len)
        {
            terminal_send_input_queue(TERMINAL_INPUT_MESSAGE, read_buff, read_len);
        }
    }

    return TX_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief     This function transfers a message to the PC based on the terminal frame taken from the terminal
 *            output queue.
 * @param[in] None
 * @retval    Any other error code apart from TX_SUCCESS on unsuccessful operation.
 **********************************************************************************************************************/
static UINT terminal_output_handle(void)
{
    UINT status = TX_SUCCESS;

    terminal_msg_t * p_terminal_msg = NULL;

    entry_info_t entry = {.name = {RESET_VALUE}, .attr = RESET_VALUE, .size = RESET_VALUE,\
                          .time = {.month = RESET_VALUE, .date = RESET_VALUE, .year = RESET_VALUE,\
                                   .hour = RESET_VALUE, .min = RESET_VALUE, .sec = RESET_VALUE}\
                         };
    dir_property_t dir = {.file = RESET_VALUE, .size = RESET_VALUE, .subdir = RESET_VALUE};

    while (true)
    {
        /* Retrieve a message from the output queue */
        status = terminal_get_output_queue((void *)&p_terminal_msg, TX_NO_WAIT);
        APP_ERR_RET(TX_SUCCESS != status && TX_QUEUE_EMPTY != status, status);

        if (TX_SUCCESS == status)
        {
            switch (p_terminal_msg->id)
            {
                case TERMINAL_OUTPUT_EP_BANNER:
                    /* Get FSP package information */
                    fsp_pack_version_t version = {RESET_VALUE};
                    R_FSP_VersionGet(&version);

                    /* Print the EP banner to the terminal application on PC */
                    terminal_printf(BANNER_INFO, EP_VERSION,\
                                    version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
                    break;

                case TERMINAL_OUTPUT_EP_INFO:
                    /* Print the EP information to the terminal application on PC */
                    terminal_printf(EP_INFO);
                    break;


                case TERMINAL_OUTPUT_APP_INFO_STR:
                    /* Print a message to the terminal application on PC */
                    terminal_printf("\r\n%s", p_terminal_msg->msg);
                    break;

                case TERMINAL_OUTPUT_APP_ERR_STR:
                    /* Print an error message to the terminal application on PC */
                    terminal_printf("\r\nERROR: %s", p_terminal_msg->msg);
                    break;

                case TERMINAL_OUTPUT_APP_ERR_TRAP:
                    /* Print the error code to the terminal application on PC */
                    UINT err = *(UINT*)p_terminal_msg->msg;
                    terminal_printf("\r\nReturned Error Code: 0x%x\r\n\r\n", err);

                    /* trap upon the error */
                    APP_ERR_TRAP(err);
                    break;

                 case TERMINAL_OUTPUT_APP_MENU:
                    /* Print the menu to the terminal application on PC */
                    terminal_printf(MENU_OPTION);
                    break;

                case TERMINAL_OUTPUT_APP_ENTRY_INFO:
                    entry = *(entry_info_t*)p_terminal_msg->msg;
                    if (FX_DIRECTORY == (entry.attr & FX_DIRECTORY))
                    {
                        /* Print directory information */
                        terminal_printf("%.2u/%.2u/%.4u  %.2u:%.2u:%.2u    %s %s\r\n",
                                  entry.time.month, entry.time.date, entry.time.year,
                                  entry.time.hour, entry.time.min, entry.time.sec,
                                  "<DIR>          ", entry.name);
                    }
                    else if (FX_ARCHIVE == (entry.attr & FX_ARCHIVE))
                    {
                        /* Print file information */
                        terminal_printf("%.2u/%.2u/%.4u  %.2u:%.2u:%.2u    %15d %s\r\n",
                                  entry.time.month, entry.time.date, entry.time.year,
                                  entry.time.hour, entry.time.min, entry.time.sec,
                                  entry.size, entry.name);
                    }
                    else
                    {
                        /* Do nothing */
                    }
                    break;

                case TERMINAL_OUTPUT_APP_DIR_PROPERTY:
                    dir = *(dir_property_t*)p_terminal_msg->msg;
                    /* Print directory property */
                    terminal_printf("         %6u File(s) %16u bytes\r\n", dir.file, dir.size);
                    terminal_printf("         %6u Dir(s)\r\n\r\n", dir.subdir);
                break;

                default:
                    break;
            }

            /*  Release the print message allocated block */
            tx_byte_release((VOID *)p_terminal_msg);
        }

        if (TX_QUEUE_EMPTY == status)
        {
            break;
        }
    }
    return TX_SUCCESS;
}
