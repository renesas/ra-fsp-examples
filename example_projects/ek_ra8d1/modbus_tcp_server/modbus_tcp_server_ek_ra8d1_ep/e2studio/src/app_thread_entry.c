/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name    : app_thread_entry.c
 * Description  : Modbus TCP Server Implementation.
 **********************************************************************************************************************/

#include "app_thread.h"
#include "modbus_tcp_ep.h"
/* App Thread entry function */
/* pvParameters contains TaskHandle_t */
void app_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);

    modbus_tcp_entry();

    while (1)
    {
        vTaskDelay (1);
    }
}
