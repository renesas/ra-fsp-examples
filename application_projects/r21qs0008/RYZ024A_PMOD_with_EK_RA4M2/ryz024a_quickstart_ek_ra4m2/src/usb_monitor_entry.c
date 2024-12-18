/**********************************************************************************************************************
 * File Name    : usb_monitor_entry.c
 *********************************************************************************************************************/
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
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 **********************************************************************************************************************/

#include "usb_monitor.h"
#include "usb_console.h"
#include "usb_console_main.h"

extern int16_t g_console_connection_timeout;
extern bool g_usb_configured;

bool g_show_intro = false;

/**********************************************************************************************************************
 * Function Name: usb_monitor_entry
 * Description  : USB Monitor entry function.
 * Argument     : pvParameters contains TaskHandle_t
 * Return Value : .
 *********************************************************************************************************************/
void usb_monitor_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);

    while (1)
    {
        if (g_console_connection_timeout >= 100)
        {
            g_console_connection_timeout = 100;

            g_usb_configured = false;
            g_show_intro = true;
        }

        g_console_connection_timeout++;

        if (0 == g_console_connection_timeout)
        {
            g_console_connection_timeout = -1;

            g_usb_configured = true;
            if (true == g_show_intro)
            {
                /* FreeRTOS function includes cast */
                xSemaphoreGive(g_start_menu_binary_semaphore);
                g_show_intro = false;
            }
        }

        vTaskDelay (1);
    }
}
/**********************************************************************************************************************
 End of function usb_monitor_entry
 *********************************************************************************************************************/

