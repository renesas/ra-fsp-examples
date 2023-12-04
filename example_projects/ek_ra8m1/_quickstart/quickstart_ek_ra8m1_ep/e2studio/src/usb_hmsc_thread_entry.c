/***********************************************************************************************************************
 * File Name    : usb_hmsc_thread_entry.c
 * Description  : Contains entry function of Freertos Plus FAT with usb hmsc.
 **********************************************************************************************************************/
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
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
 **********************************************************************************************************************/

#include "usb_hmsc_thread.h"
#include "common_data.h"
#include "common_utils.h"
#include "common_init.h"
#include "usb_multiport.h"

extern fsp_err_t usb_init();

/* Function Name: usb_hmsc_thread_entry */
/*******************************************************************************************************************//**
 * @brief     Entry function of usb hmsc thread
 * @param[IN] pointer to pvParameters
 * @retval    None
 **********************************************************************************************************************/
void usb_hmsc_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);
    bool_t initialise   = true;

    EventBits_t uxBits;

    fsp_err_t err       = FSP_SUCCESS;

    while (true)
    {
        uxBits = xEventGroupWaitBits(g_update_console_event,
                                        STATUS_ENABLE_USB_MSC | STATUS_USB_REQUEST_READ | STATUS_USB_REQUEST_WRITE,
                                        pdFALSE, pdTRUE, 1);

        if ((uxBits & (STATUS_ENABLE_USB_MSC)) == (STATUS_ENABLE_USB_MSC))
        {
            xEventGroupClearBits(g_update_console_event, STATUS_ENABLE_USB_MSC);

            if (true == initialise)
            {
                /* Initialize file system and USB */
                err = usb_init();

                if (FSP_SUCCESS != err)
                {
                    xEventGroupSetBits(g_update_console_event, STATUS_USB_MSC_ERROR);
                    initialise = true;
                }
                else
                {
                    xEventGroupSetBits(g_update_console_event, STATUS_USB_READY);
                    initialise = false;
                }
            }
        }

        if ((uxBits & (STATUS_USB_REQUEST_READ)) == (STATUS_USB_REQUEST_READ))
        {
            xEventGroupClearBits(g_update_console_event, STATUS_USB_REQUEST_READ);

            if (check_usb_connected())
            {
                if (usb_read_operation() == SUCCESS)
                {
                    /* File read was possible, file existed and was small enough */
                    xEventGroupSetBits(g_update_console_event, STATUS_USB_READY);
                }
                else
                {
                    /* File read failed, file didn't exist or was too big */
                    xEventGroupSetBits(g_update_console_event, STATUS_USB_MSC_ERROR);
                }
            }
            else
            {
                clean_up();
                initialise = true;

                xEventGroupSetBits(g_update_console_event, STATUS_USB_MSC_ERROR);
            }
        }

        if ((uxBits & (STATUS_USB_REQUEST_WRITE)) == (STATUS_USB_REQUEST_WRITE))
        {
            xEventGroupClearBits(g_update_console_event, STATUS_USB_REQUEST_WRITE);

            usb_write_operation();

            xEventGroupSetBits(g_update_console_event, STATUS_USB_READY);
        }

        if ((uxBits & (STATUS_USB_REQUEST_RESET)) == (STATUS_USB_REQUEST_RESET))
        {
            xEventGroupClearBits(g_update_console_event, STATUS_USB_REQUEST_RESET);
            clean_up();
            initialise = true;

            xEventGroupSetBits(g_update_console_event, STATUS_USB_READY);
        }
        vTaskDelay (1);
    }
}
/**********************************************************************************************************************
 End of function usb_hmsc_thread_entry
 *********************************************************************************************************************/


