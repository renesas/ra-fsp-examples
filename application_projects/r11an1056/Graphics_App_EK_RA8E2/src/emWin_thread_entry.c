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
#include "ospi_b.h"

/* emWin thread entry function */
void emWin_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);

    fsp_err_t err = FSP_SUCCESS;

    /* Initialize ospi_b */
    err = ospi_b_init();
    if (FSP_SUCCESS != err)
    {
        APP_ERR_TRAP(err);
    }
    BaseType_t xResult = xSemaphoreTake(g_touch_reset_semaphore, portMAX_DELAY);

    if(xResult != pdTRUE)
    {
        APP_ERR_TRAP(xResult);
    }

    MainTask();

    while (1)
    {
        vTaskDelay (1);
    }
}
