/***********************************************************************************************************************
 * File Name    : usb_thread_entry.c
 * Description  : Contains data structures and functions used in usb_thread_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "usb_thread.h"
#include "usb_hcdc_app.h"

/* USB Thread entry function */
/* pvParameters contains TaskHandle_t */
void usb_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);
    usb_hcdc_main_task();
    while (true)
    {
        vTaskDelay(1);
    }
}
