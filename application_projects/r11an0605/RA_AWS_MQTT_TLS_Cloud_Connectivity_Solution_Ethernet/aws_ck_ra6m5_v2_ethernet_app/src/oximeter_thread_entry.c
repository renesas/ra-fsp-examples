/***********************************************************************************************************************
 * File Name    : oximeter_thread_entry.c
 * Description  : Contains data structures and functions used in Cloud Connectivity application
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright [2015-2023] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
 *
 * The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
 * and/or its licensors ("Renesas") and subject to statutory and contractual protections.
 *
 * This file is subject to a Renesas FSP license agreement. Unless otherwise agreed in an FSP license agreement with
 * Renesas: 1) you may not use, copy, modify, distribute, display, or perform the contents; 2) you may not use any name
 * or mark of Renesas for advertising or publicity purposes or in connection with your use of the contents; 3) RENESAS
 * MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED
 * "AS IS" WITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, AND NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR
 * CONSEQUENTIAL DAMAGES, INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF
 * CONTRACT OR TORT, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents
 * included in this file may be subject to different terms.
 **********************************************************************************************************************/
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
