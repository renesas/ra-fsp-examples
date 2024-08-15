/**********************************************************************************************************************
 * File Name    : usb_monitor_entry.c
 *********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "usb_monitor.h"
#include "usb_console.h"
#include "usb_console_main.h"

extern int    g_console_connection_timeout;
extern bool b_usb_configured;

bool g_show_intro = false;

/**********************************************************************************************************************
 * Function Name: usb_monitor_entry
 * Description  : USB Monitor entry function.
 * Argument     : pvParameters contains TaskHandle_t
 * Return Value : .
 *********************************************************************************************************************/
void usb_monitor_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);

    while (1)
    {
        if (g_console_connection_timeout >= 100)
        {
            g_console_connection_timeout = 100;

            b_usb_configured = false;
            g_show_intro = true;
        }

        g_console_connection_timeout++;

        if (0 == g_console_connection_timeout)
        {
            g_console_connection_timeout = -1;

            b_usb_configured = true;
            if (true == g_show_intro)
            {
                xSemaphoreGive(g_start_menu_binary_semaphore);
                g_show_intro = false;
            }

        }

        vTaskDelay(1);
    }
}
/**********************************************************************************************************************
 End of function usb_monitor_entry
 *********************************************************************************************************************/

