/***********************************************************************************************************************
 * File Name    : mipi_csi_ep.c
 * Description  : Contains data structures and functions used in mipi_csi_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "mipi_csi_ep.h"

/* Global variables */
volatile uint8_t * gp_next_buffer;
capture_cfg_t g_vin_cfg_run_time;
uint16_t g_image_width = RESET_VALUE;
uint16_t g_image_height = RESET_VALUE;

/* Private variable */
static vin_extended_cfg_t s_vin_cfg_run_time_extend;

/***********************************************************************************************************************
 * Function Name: vin_camera_start.
 * Description  : This function initializes the VIN module with the provided configuration.
 * Arguments    : p_cfg                     Pointer to selected VIN configuration.
 * Return Value : FSP_SUCCESS               Initialized successfully.
 *                Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t vin_camera_start(capture_cfg_t const * p_cfg)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Sanity-check the input pointer */
    if (p_cfg == NULL)
    {
        return FSP_ERR_INVALID_ARGUMENT;
    }

    /* Stop streaming before de-initializing the VIN module */
    err = camera_stream_off();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* De-initialize VIN module if it was opened */
    if (g_vin_ctrl.open)
    {
        err = R_VIN_Close(&g_vin_ctrl);
        if (FSP_SUCCESS != err)
        {
            return err;
        }
    }

    /* Power down the camera after de-initializing the VIN module */
    err = write_reg_16bit(SYS_CTRL0_REG, SYS_CTRL0_SW_PWDN);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Initialize VIN with the provided configuration */
    err = R_VIN_Open(&g_vin_ctrl, p_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Start VIN capture engine */
    err = R_VIN_CaptureStart(&g_vin_ctrl, NULL);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Start the camera sensor streaming */
    err = camera_stream_on();

    return err;
}
/***********************************************************************************************************************
* End of function vin_camera_start
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: vin_scale_image.
 * Description  : This function updates the VIN run-time configuration for the specified image size.
 * Arguments    : new_width                 Target width of image.
 *                new_height                Target height of image.
 * Return Value : FSP_SUCCESS               Configured successfully.
 *                Any other error codes     Failed to configure.
 **********************************************************************************************************************/
fsp_err_t vin_scale_image(uint16_t new_width, uint16_t new_height)
{
    /* Validate arguments (avoid division by zero later) */
    if ((new_width == 0U) || (new_height == 0U))
    {
        return FSP_ERR_INVALID_ARGUMENT;
    }

    /* Copy the default extended configuration into the runtime copy */
    s_vin_cfg_run_time_extend = g_vin_cfg_extend;

    /* Fetch old output dimensions and masks from the default configuration */
    const uint32_t old_v  = (uint32_t) g_vin_cfg_extend.conversion_data.uds_clipping_bits.cl_vsize;
    const uint32_t old_h  = (uint32_t) g_vin_cfg_extend.conversion_data.uds_clipping_bits.cl_hsize;
    const uint16_t old_vm = (uint16_t) g_vin_cfg_extend.conversion_data.uds_scale_bits.vertical_mask;
    const uint16_t old_hm = (uint16_t) g_vin_cfg_extend.conversion_data.uds_scale_bits.horizontal_mask;

    /* Fetch input dimensions from the default configuration */
    const uint16_t input_height = (uint16_t) g_vin_cfg_extend.input_ctrl.preclip.line_end + 1;
    const uint16_t input_width = (uint16_t) g_vin_cfg_extend.input_ctrl.preclip.pixel_end + 1;

    /* Recalculate the scale masks: new_mask = (old_mask * old_size) / new_size */
    s_vin_cfg_run_time_extend.conversion_data.uds_scale_bits.vertical_mask =
    (uint16_t) (((uint32_t) old_vm * old_v) / new_height);

    s_vin_cfg_run_time_extend.conversion_data.uds_scale_bits.horizontal_mask =
    (uint16_t) (((uint32_t) old_hm * old_h) / new_width);

    /* Update clipping sizes to match the new target dimensions */
    s_vin_cfg_run_time_extend.conversion_data.uds_clipping_bits.cl_vsize = (uint16_t) new_height;
    s_vin_cfg_run_time_extend.conversion_data.uds_clipping_bits.cl_hsize = (uint16_t) new_width;

    /* Update scaling enable bit */
    if ((input_height != new_height) || (input_width != new_width))
    {
        s_vin_cfg_run_time_extend.input_ctrl.cfg_bits.scaling_enable = true;
    }
    else
    {
        s_vin_cfg_run_time_extend.input_ctrl.cfg_bits.scaling_enable = false;
    }

    /* Copy the default main configuration into the runtime copy */
    g_vin_cfg_run_time = g_vin_cfg;

    /* Point p_extend at the freshly updated runtime-extended configuration */
    g_vin_cfg_run_time.p_extend = &s_vin_cfg_run_time_extend;

    return FSP_SUCCESS;
}
/***********************************************************************************************************************
* End of function vin_scale_image
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: mipi_csi_deinit.
 * Description  : This function closes the opened VIN module.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
void mipi_csi_deinit(void)
{
    /* Close the opened VIN module */
    if (g_vin_ctrl.open)
    {
        R_VIN_Close(&g_vin_ctrl);
    }
}
/***********************************************************************************************************************
* End of function mipi_csi_deinit
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: vin_callback.
 * Description  : This function is a callback for VIN module.
 * Arguments    : p_args                    Pointer to the callback argument.
 * Return Value : None.
 **********************************************************************************************************************/
void vin_callback(capture_callback_args_t * p_args)
{
    vin_module_status_t module_status = (vin_module_status_t) p_args->event_status;
    vin_interrupt_status_t interrupt_status = (vin_interrupt_status_t) p_args->interrupt_status;
    FSP_PARAMETER_NOT_USED(module_status);

    switch (p_args->event)
    {
        case VIN_EVENT_NOTIFY:
        {
            if (interrupt_status.bits.frame_complete)
            {
                gp_next_buffer = p_args->p_buffer;
            }

            break;
        }

        case VIN_EVENT_ERROR:
        {
            break;
        }

        default:
        {
            /* Do nothing */
            break;
        }
    }
}
/***********************************************************************************************************************
* End of function vin_callback
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: mipi_csi0_callback.
 * Description  : This function is a callback for MIPI CSI module.
 * Arguments    : p_args                    Pointer to the callback argument.
 * Return Value : None.
 **********************************************************************************************************************/
void mipi_csi0_callback(mipi_csi_callback_args_t * p_args)
{
    switch (p_args->event)
    {
        /* In this project, the application does not rely on any MIPI CSI events.
         * FRAME_DATA event is ignored because VIN automatically handles captured frames.
         * DATA_LANE event status changes are not monitored (stable single-lane setup).
         * VIRTUAL_CHANNEL event is unused as only one channel 0 is active.
         * POWER and SHORT_PACKET_FIFO events are ignored for simplicity. */
        case MIPI_CSI_EVENT_DATA_LANE:
        case MIPI_CSI_EVENT_FRAME_DATA:
        case MIPI_CSI_EVENT_POWER:
        case MIPI_CSI_EVENT_SHORT_PACKET_FIFO:
        case MIPI_CSI_EVENT_VIRTUAL_CHANNEL:
        default:
        {
            break;
        }
    }
}
/***********************************************************************************************************************
* End of function mipi_csi0_callback
***********************************************************************************************************************/
