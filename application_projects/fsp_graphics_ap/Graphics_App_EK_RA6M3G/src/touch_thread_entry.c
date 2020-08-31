/***********************************************************************************************************************
 * File Name    : touch_thread_entry.c
 * Description  : Contains code to handle I2C touch controller.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
#include "touch_thread.h"
#include "touch_ft5x06.h"
#include "GUI.h"

/******************************************************************************
 * Static global variables
 *****************************************************************************/
static touch_data_t touch_data;
/******************************************************************************
 * Function definitions
 *****************************************************************************/

/*******************************************************************************************************************//**
 * Touch IRQ callback function
 *
 * NOTE: This function will return to the highest priority available task.
 *
 * @param      p_args          IRQ callback data
 **********************************************************************************************************************/
void touch_irq_cb(external_irq_callback_args_t * p_args)
{
    if(0 == p_args->channel)
     {
        BaseType_t context_switch = pdFALSE;

        /* Set touch IRQ semaphore */
        xSemaphoreGiveFromISR(g_touch_semaphore, &context_switch);

        /* Return to the highest priority available task */
        portYIELD_FROM_ISR(context_switch);
    }
}

/*******************************************************************************************************************//**
 * Touch Thread entry function
 * @param      pvParameters    Task parameters (contains TaskHandle_t)
 **********************************************************************************************************************/
void touch_thread_entry(void *pvParameters)
{
    static GUI_PID_STATE State;
    FSP_PARAMETER_NOT_USED (pvParameters);

    /* Initialize FT5X06 driver */
    ft5x06_init(&g_i2c_touch, &g_i2c_semaphore, BSP_IO_PORT_03_PIN_04);

    /* Enable touch IRQ */
    R_ICU_ExternalIrqOpen(g_touch_irq.p_ctrl, g_touch_irq.p_cfg);
    R_ICU_ExternalIrqEnable(g_touch_irq.p_ctrl);

    while (1)
    {
        /* Wait for IRQ from touch controller. */
        xSemaphoreTake(g_touch_semaphore, portMAX_DELAY);
        /* Get touch data from the FT5X06 */
        ft5x06_payload_get(&touch_data);

        /* Store touch data in emWin */
        if (touch_data.num_points)
        {
          State.x       = touch_data.point[0].x;
          State.y       = touch_data.point[0].y;
          State.Pressed = 1;
          GUI_PID_StoreState(&State);
        }
        else if (State.Pressed == 1)
        {
          State.Pressed = 0;
          GUI_PID_StoreState(&State);
        }
    }
}
