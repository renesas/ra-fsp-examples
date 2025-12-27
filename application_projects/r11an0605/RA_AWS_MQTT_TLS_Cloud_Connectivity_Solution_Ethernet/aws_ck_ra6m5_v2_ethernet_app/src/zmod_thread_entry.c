/***********************************************************************************************************************
 * File Name    : zmod_thread_entry.c
 * Description  : Contains macros, data structures and functions used in sensing ZMOD4410 sensor data
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/ 

#include "zmod_thread.h"
#include "RA_ZMOD4XXX_Common.h"
#include "common_utils.h"
#include "user_choice.h"

extern TaskHandle_t zmod_thread;

/*******************************************************************************************************************//**
 * @brief       zmod_thread_entry function
 * @param[in]   pvParameters
 * @retval      None
 ***********************************************************************************************************************/
void zmod_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);

    /* Wait till ZMOD4410 and ZMOD4510 sensor initialization is Completed */
    xTaskNotifyWait(pdFALSE, pdFALSE, (uint32_t *)&zmod_thread, portMAX_DELAY);

    while (1)
    {

#if  _ZMOD4410_SENSOR_ENABLE_
        /* Read ZMOD4410 sensor data */
        start_iaq_1st_gen ();
#endif
#if _ZMOD4510_SENSOR_ENABLE_
        /* Read ZMOD4510 sensor data */
        start_oaq_no2_o3 ();
#endif
        vTaskDelay (5);
    }
}
