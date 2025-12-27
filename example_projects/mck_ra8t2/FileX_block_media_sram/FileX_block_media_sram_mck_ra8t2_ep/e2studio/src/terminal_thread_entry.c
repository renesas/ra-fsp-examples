/***********************************************************************************************************************
 * File Name    : terminal_thread_entry.c
 * Description  : Contains functions from the serial thread.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "terminal_thread.h"
#include "filex.h"

extern TX_THREAD terminal_thread;

/* Terminal Thread entry function */
void terminal_thread_entry(void)
{
    UINT status = TX_SUCCESS;

    status = TERM_INIT();
    APP_ERR_TRAP(status);

    status = term_framework_init();
    if (TX_SUCCESS != status)
    {
        APP_PRINT("\r\nERROR: terminal_framework_init failed\r\n");
        APP_PRINT("\r\nReturned error code: 0x%x\r\n", status);
        APP_ERR_TRAP(status);
    }

    while (true)
    {
        /* Handle user input from terminal */
        status = terminal_input_handle();
        if (TX_SUCCESS != status)
        {
            APP_PRINT("\r\nERROR: terminal_input_hanlde failed\r\n");
            APP_PRINT("\r\nReturned error code: 0x%x\r\n", status);
            APP_ERR_TRAP(status);
        }

        /* Handle other threads that need output to the terminal application via terminal interface */
        status = terminal_output_handle();
        if (TX_SUCCESS != status)
        {
            APP_PRINT("\r\nERROR: terminal_output_hanlde failed\r\n");
            APP_PRINT("\r\nReturned error code: 0x%x\r\n", status);
            APP_ERR_TRAP(status);
        }

        tx_thread_sleep(TERMINAL_SLEEP_TICK);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function takes input data from the PC then puts it into the terminal input queue.
 * @param[in] None.
 * @retval    Any other error code apart from TX_SUCCESS on unsuccessful operation.
 **********************************************************************************************************************/
UINT terminal_input_handle(void)
{
    UINT status = TX_SUCCESS;
    uint32_t r_len = RESET_VALUE;
    char r_buff[TERM_BUFFER_SIZE] = {NULL_CHAR};

    if (APP_CHECK_DATA)
    {
        r_len = APP_READ(r_buff, sizeof(r_buff));
        if (RESET_VALUE == r_len)
        {
            /* Return success if no data is read */
            return FSP_SUCCESS;
        }

        /* Send input data to the input queue */
        status = term_send_input_queue(TERMINAL_INPUT_MESSAGE, r_buff, r_len);
        if (FSP_SUCCESS != status)
        {
            return status;
        }
    }

    return status;
}

/*******************************************************************************************************************//**
 * @brief     This function transfers a message to the PC based on the terminal frame taken from the terminal
 *            output queue.
 * @param[in] None.
 * @retval    Any other error code apart from TX_SUCCESS on unsuccessful operation.
 **********************************************************************************************************************/
UINT terminal_output_handle(void)
{
    UINT status = TX_SUCCESS;
    UINT err_code = RESET_VALUE;
    terminal_msg_t * p_terminal_msg = NULL;
    fsp_pack_version_t version = {RESET_VALUE};
    entry_info_t entry = {.name = {RESET_VALUE}, .attr = RESET_VALUE, .size = RESET_VALUE,\
                          .time = {.month = RESET_VALUE, .date = RESET_VALUE, .year = RESET_VALUE,\
                                   .hour = RESET_VALUE, .min = RESET_VALUE, .sec = RESET_VALUE}\
                         };
    dir_property_t dir = {.file = RESET_VALUE, .size = RESET_VALUE, .subdir = RESET_VALUE};

    while (true)
    {
        /* Retrieve a message from output queue */
        status = term_get_output_queue((VOID *)&p_terminal_msg, TX_NO_WAIT);
        if (TX_SUCCESS == status)
        {
            switch (p_terminal_msg->id)
            {
                case TERMINAL_OUTPUT_EP_BANNER:
                    /* Get FSP package information */
                    R_FSP_VersionGet(&version);

                    /* Print the EP banner to terminal application on PC */
                    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major,\
                              version.version_id_b.minor, version.version_id_b.patch);
                    break;

                case TERMINAL_OUTPUT_EP_INFO:
                    /* Print the EP information to terminal application on PC */
                    APP_PRINT(EP_INFO);
                    break;

                case TERMINAL_OUTPUT_APP_INFO_STR:
                    APP_PRINT("%s", p_terminal_msg->msg);
                    break;

                case TERMINAL_OUTPUT_APP_ERR_STR:
                    APP_PRINT("\r\nERROR: %s\r\n", p_terminal_msg->msg);
                    break;

                case TERMINAL_OUTPUT_APP_ERR_TRAP:
                    err_code = *(UINT*)p_terminal_msg->msg;
                    APP_PRINT("\r\nReturned error code: 0x%x\r\n", err_code);
                    APP_ERR_TRAP(err_code);
                    break;

                case TERMINAL_OUTPUT_APP_MENU:
                    APP_PRINT(MENU_OPTION);
                    break;

                case TERMINAL_OUTPUT_APP_ENTRY_INFO:
                    entry = *(entry_info_t*)p_terminal_msg->msg;
                    if (FX_DIRECTORY == (entry.attr & FX_DIRECTORY))
                    {
                        /* Print directory information */
                        APP_PRINT("%.2u/%.2u/%.4u  %.2u:%.2u:%.2u    %s %s\r\n",
                                  entry.time.month, entry.time.date, entry.time.year,
                                  entry.time.hour, entry.time.min, entry.time.sec,
                                  "<DIR>          ", entry.name);
                    }
                    else if (FX_ARCHIVE == (entry.attr & FX_ARCHIVE))
                    {
                        /* Print file information */
                        APP_PRINT("%.2u/%.2u/%.4u  %.2u:%.2u:%.2u    %15d %s\r\n",
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
                    APP_PRINT("         %6u File(s) %16u bytes\r\n", dir.file, dir.size);
                    APP_PRINT("         %6u Dir(s)\r\n\r\n", dir.subdir);
                break;

                default:
                    break;
            }

            /* Release terminal message allocated block */
            status = tx_byte_release((void *)p_terminal_msg);
            TERM_ERR_RET(TX_SUCCESS != status, status, "\r\nERROR: tx_byte_release failed\r\n");
        }

        /* In case queue is empty */
        else if (TX_QUEUE_EMPTY == status)
        {
            return TX_SUCCESS;
        }

        /* In case the API is error */
        else
        {
            /* Print a error message to terminal application on PC */
            APP_PRINT("\r\nERROR: tx_queue_receive failed\r\n");
            return status;
        }
    }

    return status;
}

/*******************************************************************************************************************//**
 * @brief      This function waits for the terminal thread to initialize and sleeps before sending data to it.
 * @param[in]  None.
 * @retval     None.
 **********************************************************************************************************************/
UINT terminal_thread_init_check(void)
{
    UINT status = TX_SUCCESS;
    ULONG run_count = RESET_VALUE;

    /* Get terminal thread run count */
    while (true)
    {
        status = tx_thread_info_get(&terminal_thread,
                                    TX_NULL,
                                    TX_NULL,
                                    &run_count,
                                    TX_NULL,
                                    TX_NULL,
                                    TX_NULL,
                                    TX_NULL,
                                    TX_NULL);
        if (TX_SUCCESS != status)
        {
            return status;
        }

        if (RESET_VALUE < run_count)
        {
            return TX_SUCCESS;
        }

        tx_thread_sleep(ONE_SLEEP_TICK);
    }
}
