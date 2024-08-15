/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include <led.h>
#include <bsp_api.h>
#include "r_ble_api.h"
#include "ble_core_task.h"
#include "gpt_timer.h"

/* BLE_CORE_TASK entry function */
/* pvParameters contains TaskHandle_t */


/*******************************************************************************
 Macro definitions
*******************************************************************************/
#define         BLE_TASK_STACK_SIZE (1024)
#define         BLE_TASK_PRIORITY   (configMAX_PRIORITIES - 1)

/*******************************************************************************
 Private global variables
*******************************************************************************/

/*******************************************************************************
 Private global functions
*******************************************************************************/
TaskHandle_t    g_ble_core_task;

extern void     app_main(void);


/*******************************************************************************
* Function Name: ble_core_task_entry
* Description  : BLE_CORE_TASK entry function
* Arguments    : pvParameters contains TaskHandle_t
* Return Value : novoidne
*******************************************************************************/
void ble_core_task_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);

    /* Get packet version */
    fsp_pack_version_t version              = {RESET_VALUE};

    /* version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example Project information printed on the RTT */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    APP_PRINT(EP_INFO);

    /* Turn off the LED */
    LED_LED1OnOff(BSP_IO_LEVEL_HIGH);

    /* Execute GPT timer */
    gpt_timer();


    while (1)
    {
        /* QE_BLE main application */
        app_main();

        vTaskDelay (1000 / portTICK_PERIOD_MS);
    }
}
