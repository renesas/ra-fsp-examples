/***********************************************************************************************************************
 * File Name    : touch_thread_entry.c
 * Description  : Contains entry function of touch thread.
 ***********************************************************************************************************************/
/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
#include "system_thread.h"
#include "system_api.h"
#include <touch_thread.h>
#include "touch_gt911/touch_gt911.h"
#include "guix_gen/thermostat_resources.h"
#include "guix_gen/thermostat_specifications.h"

typedef enum
{
    ORIENTATION_NONE,
    ORIENTATION_ROTATE_CW,
    ORIENTATION_ROTATE_CCW,
    ORIENTATION_FLIP,
}orientation_t;

#if 1 == BSP_CFG_DCACHE_ENABLED
touch_gt911_data_t gt911_touch_coords BSP_PLACE_IN_SECTION(".ram_noinit_nocache");
#else
touch_gt911_data_t gt911_touch_coords;
#endif

sync_objects_t gt911_sync =
{
#if USE_EVENT_FLAGS == 1
 .p_event_flag = &g_event_flags_gt911,
#elif USE_SEMAPHORES == 1
 .p_semaphore_i2c = &g_semaphore_gt911_i2c,
 .p_semaphore_irq = &g_semaphore_gt911_irq,
 .p_touch_queue = NULL,
#endif
};


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

static void translate_touch_coordinates(coord_t * input, orientation_t direction)
{
    coord_t output =
    {
     .y = input->x,
     .x = 800-input->y,
    };

    switch(direction)
    {
        case ORIENTATION_NONE:
        {
            output.y = input->y;
            output.x = input->x;
        }
        break;

        default:
            break;
    }

    memcpy(input, &output, sizeof(coord_t));

    return;
}

/* New Thread entry function */
void touch_thread_entry(void)
{
    GX_EVENT gxe = {0};
    gxe.gx_event_type = GX_EVENT_PEN_UP;
    uint8_t status = 0;
    /* TODO: add your own code here */


    touch_gt911_ctrl_t gt911_ctrl;

    fsp_err_t err = FSP_SUCCESS;

    uint32_t gt911_err = 0;

    uint16_t gt911_fw_version = 0;

    UINT tx_err = TX_SUCCESS;

#if USE_EVENT_FLAGS == 1
    ULONG actual_flags = 0;
#endif


    err = R_TOUCH_GT911_Validate(&gt911_cfg, &gt911_err);

    if(FSP_SUCCESS != err)
    {
        APP_ERR_TRAP(err);
    }

    memset(&gt911_ctrl, 0, sizeof(touch_gt911_ctrl_t));

    err = R_TOUCH_GT911_Open(&gt911_ctrl, &gt911_cfg);

    if(FSP_SUCCESS != err)
    {
        APP_ERR_TRAP(err);
    }


    err = R_TOUCH_GT911_Reset(&gt911_ctrl);

    if(FSP_SUCCESS != err)
    {
        APP_ERR_TRAP(err);
    }
#if 1
    err = R_TOUCH_GT911_VersionGet(&gt911_ctrl, &gt911_fw_version);

    if(FSP_SUCCESS != err)
    {
        APP_ERR_TRAP(err);
    }
#endif
    while (FSP_SUCCESS == err)
    {

#if USE_EVENT_FLAGS
        tx_err = tx_event_flags_get(&g_event_flags_gt911,
                                            TOUCH_GT911_INT,
                                            TX_OR_CLEAR,
                                            &actual_flags,
                                            TX_WAIT_FOREVER);
#elif USE_SEMAPHORES == 1
        tx_err = tx_semaphore_get(&g_semaphore_gt911_irq, TX_WAIT_FOREVER);
#endif

        if(TX_SUCCESS != tx_err)
        {
            err = FSP_ERR_INVALID_STATE;
            APP_ERR_TRAP(err);
        }
#if USE_EVENT_FLAGS == 1
        if((actual_flags & TOUCH_GT911_INT) != TOUCH_GT911_INT)
        {
            continue;
        }
#endif
        err = R_TOUCH_GT911_StatusGet(&gt911_ctrl, &status, true);

        if(status & GT911_BUFFER_READY)
        {
            gt911_touch_coords.touch_count = status & GT911_MASK_TOUCH_COUNT;
            err = R_TOUCH_GT911_PointsGet(&gt911_ctrl, &gt911_touch_coords);

            if(0U < gt911_touch_coords.touch_count && GX_EVENT_PEN_UP == gxe.gx_event_type)
            {
                /* Send only the TOUCH event of the first finger to the GUIX Model View Controller. */

                translate_touch_coordinates(&gt911_touch_coords.point[0], ORIENTATION_NONE);

                gxe.gx_event_payload.gx_event_pointdata.gx_point_x       = gt911_touch_coords.point[0].x;
                gxe.gx_event_payload.gx_event_pointdata.gx_point_y       = gt911_touch_coords.point[0].y;
                gxe.gx_event_type = GX_EVENT_PEN_DOWN;
                gx_system_event_send(&gxe);

            }

            if ( 0U == gt911_touch_coords.touch_count && GX_EVENT_PEN_DOWN == gxe.gx_event_type)
            {
               gxe.gx_event_type = GX_EVENT_PEN_UP;
               gx_system_event_send(&gxe);
            }
            tx_thread_sleep (1);

            if(FSP_SUCCESS != err)
            {
                APP_ERR_TRAP(err);
            }
        }
    }



    err = R_TOUCH_GT911_Close(&gt911_ctrl);
}
