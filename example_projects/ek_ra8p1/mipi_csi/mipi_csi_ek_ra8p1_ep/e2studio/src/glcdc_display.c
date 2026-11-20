/***********************************************************************************************************************
 * File Name    : glcdc_display.c
 * Description  : Contains data structures and functions used in glcdc_display.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2025 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "glcdc_display.h"
#include "mipi_csi_ep.h"

#if (DISPLAY_OUTPUT == 1U)

/* Vsync event flag */
volatile uint8_t g_vsync_flag = false;

/***********************************************************************************************************************
 * Function Name: glcdc_init.
 * Description  : This function initializes the GLCDC module with the specified image resolution.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Initialized successfully.
 *                Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t glcdc_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close the opened GLCDC module */
    if (DISPLAY_STATE_CLOSED != g_display_ctrl.state)
    {
        err = R_GLCDC_Close(&g_display_ctrl);
        if (FSP_SUCCESS != err)
        {
            return err;
        }
    }

    display_cfg_t display_user_cfg;
    memcpy(&display_user_cfg, &g_display_cfg, sizeof(display_cfg_t));
    display_user_cfg.input[0].hsize = g_image_width;
    display_user_cfg.input[0].vsize = g_image_height;

    /* Initialize GLCDC driver */
    err = R_GLCDC_Open(&g_display_ctrl, &display_user_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Start GLCDC display output */
    err = R_GLCDC_Start(&g_display_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    g_vsync_flag = false;

    /* Wait for a Vsync event */
    while (!g_vsync_flag);

    return err;
}
/***********************************************************************************************************************
* End of function glcdc_init
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: glcdc_buffer_change.
 * Description  : This function updates the GLCDC buffer pointer on the next Vsync.
 * Arguments    : p_buffer                  Pointer to the frame buffer to display.
 * Return Value : FSP_SUCCESS               Updated successfully.
 *                Any other error codes     Failed to update.
 **********************************************************************************************************************/
fsp_err_t glcdc_buffer_change(uint8_t * const p_buffer)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_GLCDC_BufferChange(&g_display_ctrl, p_buffer, DISPLAY_FRAME_LAYER_1);

    return err;
}
/***********************************************************************************************************************
* End of function glcdc_buffer_change
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: glcdc_vsync_isr.
 * Description  : This function is a callback for GLCDC module.
 * Arguments    : p_args                    Pointer to the callback argument.
 * Return Value : None.
 **********************************************************************************************************************/
void glcdc_vsync_isr(display_callback_args_t *p_args)
{
    if (p_args->event == DISPLAY_EVENT_LINE_DETECTION)
    {
        g_vsync_flag = true;
    }
}
/***********************************************************************************************************************
* End of function glcdc_vsync_isr
***********************************************************************************************************************/

#endif /* DISPLAY_OUTPUT */
