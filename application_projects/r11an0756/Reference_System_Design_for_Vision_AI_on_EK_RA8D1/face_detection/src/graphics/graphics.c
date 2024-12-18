/*
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : graphics.c
 * Description  : This file defines the graphic system initialization functions and frame update service.
 **********************************************************************************************************************/
/***************************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 ***************************************************************************************************************************/
#include "graphics.h"
#include "hal_data.h"
#include "jlink_console.h"
#include "hal_data.h"
#include "display_mipi.h"
#include "common_util.h"

/***************************************************************************************************************************
 * Macro definitions
 ***************************************************************************************************************************/
/*
 * Draw DIM_FRAME_COUNT_LIMIT frames before enabling backlight to prevent flash of white light on screen and wait for color transition
 */
#define DIM_FRAME_COUNT_LIMIT (120)

/***************************************************************************************************************************
 * Typedef definitions
 ***************************************************************************************************************************/

/***************************************************************************************************************************
 * Imported global variables and functions (from other files)
 ***************************************************************************************************************************/
extern d2_device * d2_handle;

/***************************************************************************************************************************
 * Exported global variables and functions (to be accessed by other files)
 ***************************************************************************************************************************/


/***************************************************************************************************************************
 * Private global variables and functions
 ***************************************************************************************************************************/
static float precision_x = 0;
static float precision_y = 0;

static uint8_t g_draw_count = 0;
static uint32_t g_transition_frame = 3;
static display_runtime_cfg_t glcd_layer_change;
static uint8_t drw_buf = 0;
static face_det_err_t glcdc_init_layer();


/*********************************************************************************************************************
 *  During this transition time, print the static text without updating the camera image
 *  @param[IN]   None
 *  @retval      bool: true is ready for printing
***********************************************************************************************************************/
bool in_transition(void)
{
	if(g_transition_frame > 0)
   {
       g_transition_frame--;
       return (true);
   }
   else
   {
       return (false);
   }

}

/*********************************************************************************************************************
 *  Initialize the Dave/2D, GLCDC and MIPI LCD
 *  @param[IN]   None
 *  @retval      None
***********************************************************************************************************************/
face_det_err_t drw_display_init(void)
{

	face_det_err_t face_det_status = FACE_DET_APP_SUCCESS;


	/* Initialize D/AVE 2D driver */
	d2_handle = d2_opendevice(0);
	d2_inithw(d2_handle, 0);

	/* Clear both buffers */
	d2_framebuffer(d2_handle, fb_background, LCD_HPIX, LCD_STRIDE, LCD_VPIX * LCD_BUF_NUM, SCREEN_MODE);
	d2_clear(d2_handle, 0x00000000);

	/* Set various D2 parameters */
	d2_setblendmode(d2_handle, d2_bm_alpha, d2_bm_one_minus_alpha);
	d2_setalphamode(d2_handle, d2_am_constant);
	d2_setalpha(d2_handle, 0xff);
	d2_setantialiasing(d2_handle, 1);
	d2_setlinecap(d2_handle, d2_lc_butt);
	d2_setlinejoin(d2_handle, d2_lj_none);

	face_det_status = glcdc_init_layer();
	if(FACE_DET_APP_SUCCESS == face_det_status)
	{
		/* Initialize and start display output */
		face_det_status = display_mipi_focuslcd_init(&g_display0);
	}

	return face_det_status;
}
/*********************************************************************************************************************
 *  Initialize the GLCDC, set to use layer_1
 *  @param[IN]   None
 *  @retval      fsp_err_t
***********************************************************************************************************************/
static face_det_err_t glcdc_init_layer()
{
	face_det_err_t face_det_status = FACE_DET_APP_SUCCESS;
	fsp_err_t status = FSP_SUCCESS;
    display_input_cfg_t const *p_input   = &g_display0.p_cfg->input[0];
    display_output_cfg_t const *p_output = &g_display0.p_cfg->output;


    memset(&fb_background[0][0], 0, DISPLAY_BUFFER_STRIDE_BYTES_INPUT0 * DISPLAY_VSIZE_INPUT0);
    memset(&fb_background[1][0], 0, DISPLAY_BUFFER_STRIDE_BYTES_INPUT0 * DISPLAY_VSIZE_INPUT0);

    /* copy the data to runtime - for GLCDC layer change */

    glcd_layer_change.input = g_display0.p_cfg->input[0];

    glcd_layer_change.layer = g_display0.p_cfg->layer[0];

	/* Center image */
	precision_x = (int16_t)(p_output->htiming.display_cyc - p_input->hsize) / 2;
	precision_y = (int16_t)(p_output->vtiming.display_cyc - p_input->vsize) / 2;
	glcd_layer_change.layer.coordinate.x = (int16_t)precision_x;
	glcd_layer_change.layer.coordinate.y = (int16_t)precision_y;

	status = R_GLCDC_LayerChange(&g_display0.p_ctrl, &glcd_layer_change, DISPLAY_FRAME_LAYER_1);
	if(FSP_SUCCESS != status)
	{
		handle_error(FACE_DET_APP_GLCDC_LAYER_CHANGE);
		return FACE_DET_APP_GLCDC_LAYER_CHANGE;
	}
	return face_det_status;
}


/*********************************************************************************************************************
 *  glcdc vsync interrupt service routine
 *  @param[IN]   display_callback_args_t *p_args: not used
 *  @retval      None
***********************************************************************************************************************/
__attribute__((section(".itcm_data"))) __attribute__((__used__)) __attribute__((__noinline__))
void glcdc_vsync_callback(display_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);

    BaseType_t xHigherPriorityTaskWoken, xResult;
   	/* xHigherPriorityTaskWoken must be initialised to pdFALSE. */
   	  xHigherPriorityTaskWoken = pdFALSE;
   	if (DISPLAY_EVENT_LINE_DETECTION & p_args->event )
   		{


   			xResult = xEventGroupSetBitsFromISR(g_ai_app_event, GLCDC_VSYNC, &xHigherPriorityTaskWoken);
   			/* Was the message posted successfully? */
   			if( xResult != pdFAIL )
   			{
   			  /* If xHigherPriorityTaskWoken is now set to pdTRUE then a context
   			  switch should be requested.  The macro used is port specific and will
   			  be either portYIELD_FROM_ISR() or portEND_SWITCHING_ISR() - refer to
   			  the documentation page for the port being used. */
   			  portYIELD_FROM_ISR( xHigherPriorityTaskWoken );

   			}

   		}

   	/* wait for 120 v sync cycles before enable mipi backlight */
    if(g_draw_count < DIM_FRAME_COUNT_LIMIT)
    {
        g_draw_count++;
        if(g_draw_count == DIM_FRAME_COUNT_LIMIT)
        {
            mipi_dsi_enable_backlight();
        }
    }
}

/*********************************************************************************************************************
 *  System uses double buffer. Swap the current buffer.
 *  @param[IN]   None
 *  @retval      None
***********************************************************************************************************************/
void graphics_swap_buffer()
{

    drw_buf = drw_buf ? 0 : 1;

    /* Update the layer to display in the GLCDC (will be set on next Vsync) */

    R_GLCDC_BufferChange(g_display0.p_ctrl, fb_background[drw_buf], 0); // double buffered
    R_GLCDC_LayerChange(&g_display0.p_ctrl, &glcd_layer_change, DISPLAY_FRAME_LAYER_1);
}

/*********************************************************************************************************************
 *  Start a new frame with the current draw buffer.
 *  @param[IN]   None
 *  @retval      None
***********************************************************************************************************************/
void graphics_start_frame()
{
    /* Start a new display list */
    d2_startframe(d2_handle);

    /* Set the new buffer to the current draw buffer */
    d2_framebuffer(d2_handle, &(fb_background[drw_buf][0]), LCD_HPIX, LCD_STRIDE, LCD_VPIX, SCREEN_MODE);
}

/*********************************************************************************************************************
 *  Wait the current frame to end and swap the framebuffer
 *  @param[IN]   None
 *  @retval      None
***********************************************************************************************************************/
void graphics_end_frame()
{
    /* End the current display list */
    d2_endframe(d2_handle);

    /* Flip the framebuffer */
    graphics_swap_buffer();

}

