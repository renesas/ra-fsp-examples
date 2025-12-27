/***********************************************************************************************************************
 * File Name    : oximeter_thread_entry.c
 * Description  : Contains data structures and functions used in Cloud Connectivity application
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2015 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/ 
#include "oximeter_thread.h"
#include "console_menu/console.h"
#include "common_utils.h"
#include "oximeter.h"
#include "user_choice.h"
#include "SPO2/SPO2.h"

#define UNUSED(x)  ((void)(x))

extern TaskHandle_t oximeter_thread; // @suppress("Global (API or Non-API) variable prefix")

/*******************************************************************************************************************//**
 * @brief       Oximeter Thread entry function
 * @param[in]
 * @retval
 ***********************************************************************************************************************/
void oximeter_thread_entry(void *pvParameters){
    FSP_PARAMETER_NOT_USED (pvParameters);

    /* wait for the Sensor thread to initiate the initialization event for oximeter Thread */
    xTaskNotifyWait(pdFALSE, pdFALSE, (uint32_t *)&oximeter_thread, portMAX_DELAY);

#if _OB1203_SENSOR_ENABLE_
    /* Open OB1203 */
    RA_ob1203_init();
#endif

    while (1){
#if _OB1203_SENSOR_ENABLE_
        /* Start reading OB1203 sensor data */
        ob1203_spo2_main();
        vTaskDelay(5);
#else
        vTaskDelay(5);
#endif
    }
}
