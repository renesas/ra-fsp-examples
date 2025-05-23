/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/**********************************************************************************************************************
 * File Name    : cam_dsp_thread_entry.c
 * Version      : .
 * Description  : .
 *********************************************************************************************************************/

#include "cam_dsp_thread.h"

/* Camera Viewer entry function */
/* pvParameters contains TaskHandle_t */

/**********************************************************************************************************************
 * Function Name: cam_dsp_thread_entry
 * Description  : .
 * Argument     : pvParameters
 * Return Value : .
 *********************************************************************************************************************/
void cam_dsp_thread_entry (void * pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);

    /* TODO: add your own code here */
    while (1)
    {
        vTaskDelay(1);
    }
}

/**********************************************************************************************************************
 * End of function cam_dsp_thread_entry
 *********************************************************************************************************************/
