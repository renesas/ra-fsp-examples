/***********************************************************************************************************************
 * File Name    : touch_thread_entry.c
 * Description  : Contains entry function of touch panel control.
 ***********************************************************************************************************************/
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
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
#include "touch_thread.h"
#include "system_api.h"
#include "guix_gen/thermostat_resources.h"
#include "guix_gen/thermostat_specifications.h"
#include "touch_ft5x06/touch_ft5x06.h"


static touch_data_t touch_data;

/* Touch Thread entry function */
void touch_thread_entry(void)
{
    GX_EVENT gxe = {0};
    UINT status  = 0;
    /*
     * Initialize Touch controller:
     *  FT5X06 driver
     *  IRQ interrupt
     */
    ft5x06_init(&g_i2c_touch, &g_i2c_semaphore, BSP_IO_PORT_03_PIN_04);
    /* Enable touch IRQ */
    R_ICU_ExternalIrqOpen(g_touch_irq.p_ctrl, g_touch_irq.p_cfg);
    R_ICU_ExternalIrqEnable(g_touch_irq.p_ctrl);

    while (1)
    {
        /* Wait for IRQ from touch controller. */
        status = tx_semaphore_get(&g_touch_semaphore, TX_WAIT_FOREVER);
        if(TX_SUCCESS != status)
        {
            APP_ERR_TRAP(FSP_ERR_ASSERTION);
        }

        /* Get touch data from the FT5X06 */
        ft5x06_payload_get(&touch_data);

        /* Send touch data*/
        if(1 == touch_data.num_points)
        {
          gxe.gx_event_payload.gx_event_pointdata.gx_point_x       = touch_data.point[0].x;
          gxe.gx_event_payload.gx_event_pointdata.gx_point_y       = touch_data.point[0].y;
          gxe.gx_event_type = GX_EVENT_PEN_DOWN;
          gx_system_event_send(&gxe);
        }
        else if (GX_EVENT_PEN_DOWN == gxe.gx_event_type)
        {
          gxe.gx_event_type = GX_EVENT_PEN_UP;
          gx_system_event_send(&gxe);
        }

        tx_thread_sleep (1);
    }
}

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
        /* Set touch IRQ semaphore */
        tx_semaphore_ceiling_put(&g_touch_semaphore, (ULONG)1);
    }
}
