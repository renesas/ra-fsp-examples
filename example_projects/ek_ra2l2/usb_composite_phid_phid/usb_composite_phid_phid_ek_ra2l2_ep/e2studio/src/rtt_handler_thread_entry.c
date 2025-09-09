/***********************************************************************************************************************
 * File Name    : rtt_handler_thread_entry.c
 * Description  : Contains functions from the RTT input thread
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "rtt_handler_thread.h"
#include "common_utils.h"
#include "usb_phid_phid.h"

/*******************************************************************************************************************//**
 * @addtogroup usb_phid_phid
 * @{
 **********************************************************************************************************************/

/* Global variable */
bool g_b_key_flag = false;
bool g_b_mouse_flag = false;

/*******************************************************************************************************************//**
 * @brief       RTT handler thread entry
 * @param[IN]   pvParameters
 * @retval      None
 **********************************************************************************************************************/
void rtt_handler_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);

    char rtt_buff[BUFFER_SIZE_DOWN] = {RESET_VALUE};
    fsp_pack_version_t version = {RESET_VALUE};

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example Project information printed on the Console */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor,\
              version.version_id_b.patch);

    /* Prints EP info on RTT Viewer */
    APP_PRINT(EP_INFO);

    /* Delay for open USB */
    vTaskDelay (DELAY_2_SEC / portTICK_PERIOD_MS);

    APP_PRINT(EP_HINT);

    while (true)
    {
        /* Check for user input */
        if (APP_CHECK_DATA)
        {
            APP_READ(rtt_buff);

            /* Check for run keyboard emulator */
            if ('K' == rtt_buff[0] || 'k' == rtt_buff[0])
            {
                APP_PRINT("\r\nNOTE: Please Open Notepad\r\n");

                /* Delay for the user to open Notepad */
                vTaskDelay (DELAY_3_SEC/portTICK_PERIOD_MS);

                /* Set key flag */
                g_b_key_flag = true;

                APP_PRINT("NOTE: Run Keyboard Emulator\r\n");

                /* Delay for the user check output */
                vTaskDelay (DELAY_1_SEC/portTICK_PERIOD_MS);
            }
            /* Check for run mouse emulator */
            else if ('M' == rtt_buff[0] || 'm' == rtt_buff[0])
            {
                APP_PRINT("\r\nNOTE: See the movement of the mouse pointer on the screen\r\n");

                /* Delay for the user to stand mouse pointer */
                vTaskDelay (DELAY_3_SEC/portTICK_PERIOD_MS);

                /* Set mouse flag */
                g_b_mouse_flag = true;

                APP_PRINT("NOTE: Run Mouse Emulator\r\n");

                /* Delay for the user check output */
                vTaskDelay (DELAY_1_SEC/portTICK_PERIOD_MS);
            }
            else
            {
                ; /* Do nothing */
            }
            APP_PRINT(EP_HINT);
        }
        vTaskDelay (TASK_DELAY);
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup usb_phid_phid)
 **********************************************************************************************************************/
