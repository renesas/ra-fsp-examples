/***********************************************************************************************************************
 * File Name    : rtt_input_thread_entry.c
 * Description  : Contains functions from the rtt input thread
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "rtt_input_thread.h"
#include "common_utils.h"
#include "usb_phid_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup usb_phid_ep
 * @{
 **********************************************************************************************************************/

/* Global variable */
bool g_write_flag = false;

/*******************************************************************************************************************//**
 * @brief       rtt input thread entry
 * @param[IN]   pvParameters
 * @retval      None
 **********************************************************************************************************************/
void rtt_input_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);

    uint8_t rtt_buff[BUFFER_SIZE_DOWN] = {RESET_VALUE};
    fsp_pack_version_t version = {RESET_VALUE};

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example Project information printed on the Console */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, \
              version.version_id_b.patch);
    /* Print ep info on RTT Viewer */
    APP_PRINT(EP_INFO);

    APP_PRINT("\r\nEnter any key from RTT Viewer to Write data\r\n");

    while (true)
    {
        /* Check for user input */
        if (APP_CHECK_DATA)
        {
            APP_READ(rtt_buff);

            APP_PRINT("\r\nNOTE: Please Open Notepad \r\n");

            /* Delay for user to open notepad */
            vTaskDelay(DELAY_3_SEC/portTICK_PERIOD_MS);

            /* Set write flag */
            g_write_flag = true;

            APP_PRINT("\r\nEnter any key from RTT Viewer to Write data\r\n");
        }
        vTaskDelay (TASK_DELAY);
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup usb_phid_ep)
 **********************************************************************************************************************/
