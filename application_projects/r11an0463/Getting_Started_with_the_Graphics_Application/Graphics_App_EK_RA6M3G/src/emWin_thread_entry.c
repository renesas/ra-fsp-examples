/***********************************************************************************************************************
 * File Name    : emWin_thread_entry.c
 * Description  : Contains code to initialize emWin.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2019 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#include "emWin_thread.h"
#include "GUI.h"
/* emWin thread entry function */
/* pvParameters contains TaskHandle_t */
void emWin_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);

    MainTask();

    while (1)
    {
        vTaskDelay (1);
    }
}
