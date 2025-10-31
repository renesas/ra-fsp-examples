/***********************************************************************************************************************
* File Name    : term_thread_entry.c
* Description  : Implements terminal thread entry, input handling, and output processing for communication with the FAT
*                file system.
***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "term_thread.h"
#include "fat_operation.h"
#include "common_app.h"

/***********************************************************************************************************************
 * Exported Global Variables
 **********************************************************************************************************************/
extern TaskHandle_t fat_thread;
extern volatile bool g_usb_host_active;

/***********************************************************************************************************************
 * Private Function Prototypes
 **********************************************************************************************************************/
static uint32_t term_input_handle(void);
static uint32_t term_output_handle(void);

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief Entry function for the terminal thread.
 *
 * This function initializes the terminal module, prepares necessary resources, and enters a loop to process terminal
 * input and output.
 *
 * @param[in]  pvParameters Unused parameter passed to the thread.
 * @retval     None
 **********************************************************************************************************************/
void term_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);
    uint32_t status = FSP_SUCCESS;

    /* Initialize the terminal module */
    status = TERM_INIT();
    APP_ERR_TRAP(status);

    /* Initialize terminal framework resources */
    status = term_framework_init();
    APP_ERR_TRAP(status);

    /* Main loop for processing terminal input and output */
    while (true)
    {
        /* Handle terminal input */
        status = term_input_handle();
        APP_ERR_TRAP(status);

        /* Handle terminal output */
        status = term_output_handle();
        APP_ERR_TRAP(status);

        /* Yield control to other tasks */
        vTaskDelay(TERM_THREAD_SLEEP_TICK);
    }
}

/*******************************************************************************************************************//**
 * @brief Handles terminal input by reading data, sending it to the input queue, and notifying the FAT thread.
 *
 * @retval FSP_SUCCESS on successful operation, other error codes otherwise.
 **********************************************************************************************************************/
static uint32_t term_input_handle(void)
{
    fsp_err_t status = FSP_SUCCESS;
    BaseType_t os_status = pdTRUE;
    uint32_t r_len = RESET_VALUE;
    char r_buff[TERM_BUFFER_SIZE] = {NULL_CHAR};

    /* Check if there is data available in the terminal */
    if (APP_CHECK_DATA)
    {
        /* Read data from the terminal */
        r_len = APP_READ(r_buff, sizeof(r_buff));
        if ((RESET_VALUE == r_len) || (false == g_usb_host_active))
        {
            /* Return success if no data is read or USB host is inactive */
            return FSP_SUCCESS;
        }

        /* Send input data to the input queue */
        status = term_send_input_queue(TERM_INPUT_MESSAGE, r_buff, r_len);
        if (FSP_SUCCESS != status)
        {
            return status;
        }

        /* Notify the FAT thread about new input */
        os_status = xTaskNotify(fat_thread, FAT_GENERAL_INPUT_COMMAND, eSetBits);
        if (pdTRUE != os_status)
        {
            return FSP_ERR_ASSERTION;
        }
    }

    return status;
}

/*******************************************************************************************************************//**
 * @brief Handles terminal output by retrieving messages from the output queue, processing them, and releasing memory.
 *
 * @retval FSP_SUCCESS on successful operation, other error codes otherwise.
 **********************************************************************************************************************/
static uint32_t term_output_handle(void)
{
    uint32_t status = FSP_SUCCESS;
    term_msg_t *p_term_msg = NULL;

    /* Loop to process all messages in the output queue */
    while (true)
    {
        /* Retrieve a message from the output queue without timeout */
        status = term_get_output_queue((void *)&p_term_msg, 0);
        if (FSP_SUCCESS == status)
        {
            /* Process the retrieved message based on its type */
            switch (p_term_msg->id)
            {
                case TERM_OUTPUT_EP_BANNER:
                {
                    /* Print package version information */
                    fsp_pack_version_t version = {RESET_VALUE};
                    R_FSP_VersionGet(&version);
                    APP_PRINT(BANNER_INFO, EP_VERSION,
                              version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
                    break;
                }
                case TERM_OUTPUT_EP_INFO:
                {
                    /* Print project information */
                    APP_PRINT(EP_INFO);
                    break;
                }
                case TERM_OUTPUT_INFO_STR:
                {
                    /* Print a generic information message */
                    APP_PRINT("\r\n%s", p_term_msg->msg);
                    break;
                }
                case TERM_OUTPUT_ERR_STR:
                {
                    /* Print an error message */
                    APP_PRINT("\r\nERROR: %s", p_term_msg->msg);
                    break;
                }
                case TERM_OUTPUT_ERR_TRAP:
                {
                    uint32_t err = *(uint32_t *)p_term_msg->msg;
                    /* Print an error code and trap */
                    APP_PRINT("\r\nReturned Error Code: 0x%x\r\n", err);
                    APP_ERR_TRAP(err);
                    break;
                }
                case TERM_OUTPUT_EP_MENU:
                {
                    /* Print the terminal menu */
                    APP_PRINT(MENU);
                    break;
                }
                default:
                {
                    /* Handle unsupported message types */
                    APP_PRINT("\r\nUnsupported message type.");
                    break;
                }
            }

            /* Free allocated memory for the message */
            vPortFree((void *)p_term_msg);
        }
        else
        {
            /* Exit the loop if no messages are available */
            return FSP_SUCCESS;
        }
    }

    return FSP_SUCCESS;
}
