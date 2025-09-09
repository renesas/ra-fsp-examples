/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/

#include "hal_data.h"
#include "display_layer.h"
#include "common_util.h"
#include "camera_control.h"
#include "time_counter.h"

#include "display_layer_config.h"


/***************************************************************************************************************************
 * Macro definitions
 ***************************************************************************************************************************/

/***************************************************************************************************************************
 * Typedef definitions
 ***************************************************************************************************************************/

/***************************************************************************************************************************
 * Imported global variables and functions (from other files)
 ***************************************************************************************************************************/
/***************************************************************************************************************************
uint8_t camera_buffer_next_layer;

extern display_runtime_cfg_t glcd_layer_change_2;

/* Variables to store resolution information */

/***************************************************************************************************************************
 * Private global variables and functions
 ***************************************************************************************************************************/
 extern display_runtime_cfg_t glcd_layer_change_2;


 /***************************************************************************************************************************
  * Private global variables and functions
  ***************************************************************************************************************************/
 static uint16_t g_hz_size, g_vr_size;
 /* Variables used for buffer usage */
 static uint32_t g_buffer_size;
 static uint8_t * g_p_single_buffer, * g_p_double_buffer;
 static uint32_t last_lcd_glcdc_frame_end = 0;


 /**********************************************************************************************************************
  * Function Name: drw_init
  * Description  : Initialize the DRW
  * Argument     : none
  * Return Value : none
  *********************************************************************************************************************/
fsp_err_t drw_init(void)
{
    /* Initialize D/AVE 2D driver */
     d2_handle = d2_opendevice(0);
     d2_inithw(d2_handle, 0);

     /* Clear both buffers */
     d2_framebuffer(d2_handle, vin_image_buffer_1, 640, 640, 480, DISPLAY_SCREEN_BUFF_D2_COLOR_CODE);
     d2_clear(d2_handle, 0x00000000);

     /* Set various D2 parameters */
     d2_setblendmode(d2_handle, d2_bm_alpha, d2_bm_one_minus_alpha);
     d2_setalphamode(d2_handle, d2_am_constant);
     d2_setalpha(d2_handle, 0xff);
     d2_setantialiasing(d2_handle, 1);
     d2_setlinecap(d2_handle, d2_lc_butt);
     d2_setlinejoin(d2_handle, d2_lj_none);

     return FSP_SUCCESS;
}



/**********************************************************************************************************************
 * Function Name: glcdc_vsync_isr
 * Description  : GLCDC Interrupt Callback
 * Argument     : p_args
 * Return Value : none
 *********************************************************************************************************************/

void glcdc_vsync_isr(display_callback_args_t *p_args)
{

    FSP_PARAMETER_NOT_USED(p_args);

    BaseType_t xHigherPriorityTaskWoken, xResult;
    /* xHigherPriorityTaskWoken must be initialised to pdFALSE. */
     xHigherPriorityTaskWoken = pdFALSE;
     if (DISPLAY_EVENT_LINE_DETECTION & p_args->event )
       {

         application_processing_time.lcd_display_update_refresh_ms = TimeCounter_CountValueConvertToMs(last_lcd_glcdc_frame_end, TimeCounter_CurrentCountGet());
         last_lcd_glcdc_frame_end = TimeCounter_CurrentCountGet();

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

       return;
}

 fsp_err_t initialise_display(void)
{
    /* Fill framebuffers with data for easier debugging */
    memset(fb_foreground, 0x55, DISPLAY_BUFFER_STRIDE_BYTES_INPUT1 * DISPLAY_VSIZE_INPUT1);

    fsp_err_t result;

    /* Get LCDC configuration */
    g_hz_size = (g_plcd_display_cfg.input[0].hsize);
    g_vr_size = (g_plcd_display_cfg.input[0].vsize);

    /* Initialize buffer pointers */
    g_buffer_size = (uint32_t) (g_hz_size * g_vr_size * BYTES_PER_PIXEL);
    g_p_single_buffer = (uint8_t *) g_plcd_display_cfg.input[0].p_base;
    /* Double buffer for drawing color bands with good quality */
    g_p_double_buffer = g_p_single_buffer + g_buffer_size;

    R_BSP_PinAccessEnable();

    /* Reset Display - active low */
    /* Note: Please update wait periods according to LCD controller specification */
    R_IOPORT_PinWrite(&g_ioport_ctrl, DISP_RESET, BSP_IO_LEVEL_LOW);
    R_BSP_SoftwareDelay(10, BSP_DELAY_UNITS_MICROSECONDS);
    R_IOPORT_PinWrite(&g_ioport_ctrl, DISP_RESET, BSP_IO_LEVEL_HIGH);
    R_BSP_SoftwareDelay(120, BSP_DELAY_UNITS_MILLISECONDS);


    /* Initialize GLCDC driver */
    result = R_GLCDC_Open(&g_plcd_display_ctrl, &g_plcd_display_cfg);
    if(FSP_SUCCESS != result)
    {
        handle_error(VISION_AI_APP_ERR_GLCDC_OPEN);
        return result;
    }

    /* Start GLCDC display output */
    result = R_GLCDC_Start(&g_plcd_display_ctrl);

    /* Handle error */
    if(FSP_SUCCESS != result)
    {
        /* GLCDC initialization failed  */
        handle_error(VISION_AI_APP_ERR_GLCDC_START);
        return result;
    }

    R_BSP_SoftwareDelay(40, BSP_DELAY_UNITS_MILLISECONDS);

    R_BSP_PinAccessDisable();

    glcd_layer_change_2.layer = g_plcd_display.p_cfg->layer[1];
    glcd_layer_change_2.input.hsize = 320;
    glcd_layer_change_2.input.vsize = 560;
    glcd_layer_change_2.input.hstride =  ((((320 * DISPLAY_BITS_PER_PIXEL_INPUT0 + 0x1FF) >> 9) << 6)*8/(16));
    glcd_layer_change_2.input.format = DISPLAY_IN_FORMAT_16BITS_RGB565;
    glcd_layer_change_2.input.p_base = (uint32_t *)&fb_foreground[0]; // Initial frame

   /* Center image */

   glcd_layer_change_2.layer.coordinate.x = (int16_t)700;
   glcd_layer_change_2.layer.coordinate.y = (int16_t)40;

   return FSP_SUCCESS;
}


/*********************************************************************************************************************
 *  System uses VIN output buffer. Pick the next buffer.
 *  @param[IN]   None
 *  @retval      None
***********************************************************************************************************************/
void graphics_swap_buffer()
{

    R_GLCDC_BufferChange(g_plcd_display.p_ctrl, gp_next_buffer, DISPLAY_FRAME_LAYER_1);
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

