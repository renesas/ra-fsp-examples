/***********************************************************************************************************************
 * File Name    : rtt_handler_thread_entry.c
 * Description  : Contains functions from the rtt input thread
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
#include "rtt_handler_thread.h"
#include "common_utils.h"
#include "usb_phid_phid.h"

/*******************************************************************************************************************//**
 * @addtogroup usb_phid_phid
 * @{
 **********************************************************************************************************************/

/* global variable */
bool g_b_key_flag = false;
bool g_b_mouse_flag = false;

/*******************************************************************************************************************//**
 * @brief       rtt handler thread entry
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
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    /* prints ep info on RTTViewer */
    APP_PRINT(EP_INFO);

    /* delay for open USB */
    vTaskDelay (DELAY_2_SEC / portTICK_PERIOD_MS);

    APP_PRINT(EP_HINT);

    while (true)
    {
        /* check for user input */
        if (APP_CHECK_DATA)
        {
            APP_READ(rtt_buff);
            /* check for run keyboard emulator */
            if('K' == rtt_buff[0] || 'k' == rtt_buff[0])
            {
                APP_PRINT("\r\nNOTE: Please Open Notepad \r\n");
                /* delay for user to open notepad */
                vTaskDelay (DELAY_3_SEC/portTICK_PERIOD_MS);
                /* set key flag */
                g_b_key_flag = true;

                APP_PRINT("NOTE: Run Keyboard Emulator \r\n");
                /* delay for user check output */
                vTaskDelay (DELAY_1_SEC/portTICK_PERIOD_MS);
            }
            /* check for run mouse emulator */
            else if('M' == rtt_buff[0] || 'm' == rtt_buff[0])
            {
                APP_PRINT("\r\nNOTE: See the movement of the mouse pointer on the screen \r\n");
                /* delay for user to stand mouse pointer */
                vTaskDelay (DELAY_3_SEC/portTICK_PERIOD_MS);
                /* set mouse flag */
                g_b_mouse_flag = true;

                APP_PRINT("NOTE: Run Mouse Emulator \r\n");
                /* delay for user check output */
                vTaskDelay (DELAY_1_SEC/portTICK_PERIOD_MS);
            }
            else
            {
                /*Do nothing*/
            }
            APP_PRINT(EP_HINT);        }
        vTaskDelay (TASK_DELAY);
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup usb_phid_phid)
 **********************************************************************************************************************/
