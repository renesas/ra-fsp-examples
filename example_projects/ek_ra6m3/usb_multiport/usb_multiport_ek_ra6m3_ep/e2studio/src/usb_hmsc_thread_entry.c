/***********************************************************************************************************************
 * File Name    : usb_hmsc_thread_entry.c
 * Description  : Contains entry function of Freertos Plus FAT with usb hmsc.
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

#include "usb_hmsc_thread.h"
#include "common_utils.h"
#include "usb_multiport.h"

/*******************************************************************************************************************//**
 * @addtogroup usb_multiport_ep
 * @{
 **********************************************************************************************************************/

/* extern global variable */
extern uint8_t g_write_data[WRITE_ITEM_SIZE];

/*******************************************************************************************************************//**
 * @brief     Entry function of usb hmsc thread
 * @param[IN] pointer to pvParameters
 * @retval    None
 **********************************************************************************************************************/
void usb_hmsc_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = { RESET_VALUE };
    BaseType_t err_queue = pdFALSE;

    /* version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example Project information printed on the Console */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    APP_PRINT(EP_INFO);

    /* Initialize file system and USB */
    err = file_system_and_usb_init();
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\nError in initializing FreeRTOS+FAT with USB_HMSC.\r\n");
        APP_ERR_TRAP (err);
    }

    /* Print USB operation menu */
    APP_PRINT (MENU);

    while (true)
    {
        uint8_t rtt_input_buf[BUFFER_SIZE_DOWN] = { RESET_VALUE };
        uint8_t converted_rtt_input = RESET_VALUE;
        /*Read RTT input from user*/
        if (APP_CHECK_DATA)
        {
            APP_READ(rtt_input_buf);
            converted_rtt_input = (uint8_t) atoi ((char*) rtt_input_buf);
            process_usb_operation (converted_rtt_input);
        }
        /* check for the usb connection status*/
        if (true == check_usb_connected ())
        {
            /* Data received from pcdc thread through queue */
            err_queue = xQueueReceive (g_queue_data_from_pcdc, g_write_data, (TickType_t) (RESET_VALUE));
            if (pdTRUE == err_queue)
            {
                APP_PRINT("\r\n   Data entered from PCDC is being written to the file.\n");
                usb_write_operation ();
                APP_PRINT (MENU);
            }
        }
        vTaskDelay (1);
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup usb_multiport_ep)
 **********************************************************************************************************************/

