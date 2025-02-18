/*
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/***********************************************************************************************************************
 * File Name    : touch_thread_entry.c
 * Description  : Thread handles sending the capacitive touch control information to emWin.
 **********************************************************************************************************************/
#include "touch_thread.h"
#include "touch_gt911.h"
#include "GUI.h"
#include "Application.h"

static touch_gt911_data_t gt911_touch_coords;
static uint8_t status = 0;

/* Touch Thread entry function */
/* pvParameters contains TaskHandle_t */
void touch_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);

    touch_gt911_ctrl_t gt911_ctrl;
    fsp_err_t err = FSP_SUCCESS;
    uint32_t gt911_err = 0;
    uint16_t gt911_fw_version = 0;

    sync_objects_t gt911_sync =
    {
#if USE_EVENT_FLAGS == 1
     .p_event_flag = &g_event_flags_gt911,f
#elif USE_SEMAPHORES == 1
     .p_semaphore_i2c = g_semaphore_gt911_i2c,
     .p_semaphore_irq = g_semaphore_gt911_irq,
#endif
    };

    /* Initialize touch configuration*/
    touch_gt911_cfg_t gt911_cfg =
    {
     .reset_pin = IOPORT_PORT_10_PIN_01,
     .irq_pin = IOPORT_PORT_05_PIN_10,
     .i2c_sda_pin = IOPORT_PORT_05_PIN_11,
     .i2c_scl_pin = IOPORT_PORT_05_PIN_12,
     .p_i2c_master = &g_i2c_gt911,
     .p_external_irq = &g_int_gt911,
     .p_ioport = &g_ioport,
     .sync = &gt911_sync,
    };

    /* Validate touch configuration */
    err = R_TOUCH_GT911_Validate(&gt911_cfg, &gt911_err);
    APP_ERR_TRAP(err);

    memset(&gt911_ctrl, 0, sizeof(touch_gt911_ctrl_t));

    /* Open the touch controller*/
    err = R_TOUCH_GT911_Open(&gt911_ctrl, &gt911_cfg);
    APP_ERR_TRAP(err);

    /* Reset the touch hardware*/
    err = R_TOUCH_GT911_Reset(&gt911_ctrl);
    APP_ERR_TRAP(err);

    err = R_TOUCH_GT911_VersionGet(&gt911_ctrl, &gt911_fw_version);
    APP_ERR_TRAP(err);

    /* Tell the emWin thread that touch reset has completed */
    BaseType_t xResult = xSemaphoreGive(g_touch_reset_semaphore);
    if(xResult != pdTRUE)
    {
        __asm("BKPT #0\n");
    }

    static GUI_PID_STATE State;
    State.Pressed = 0;
    State.x = 0;
    State.y = 0;


    while(FSP_SUCCESS == err)
    {
        xResult = xSemaphoreTake(g_semaphore_gt911_irq, portMAX_DELAY);
        if(xResult == pdFALSE)
        {
            __asm("BKPT #0\n");
        }

        err = R_TOUCH_GT911_StatusGet(&gt911_ctrl, &status, true);

        if( status & GT911_BUFFER_READY )
        {
            gt911_touch_coords.touch_count = status & GT911_MASK_TOUCH_COUNT;
            err = R_TOUCH_GT911_PointsGet(&gt911_ctrl, &gt911_touch_coords);
            APP_ERR_TRAP(err);

            if(gt911_touch_coords.touch_count)
            {
                State.x =  (int)gt911_touch_coords.point[0].x;
                State.y =  (int)gt911_touch_coords.point[0].y;
                State.Pressed = 1;

                /* Send the touch state to emWin thread */
                GUI_PID_StoreState(&State);
            }
            else if(State.Pressed)
            {
                State.Pressed = 0;
                GUI_PID_StoreState(&State);
            }
        }
        vTaskDelay(16/portTICK_PERIOD_MS);
    }

    err = R_TOUCH_GT911_Close(&gt911_ctrl);
    APP_ERR_TRAP(err);
}
