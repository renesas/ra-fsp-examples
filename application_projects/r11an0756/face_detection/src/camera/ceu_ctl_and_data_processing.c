/*
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/**********************************************************************************************************************
 * File Name    : ceu_ctl_and_data_processing.c
 * Description  : This file defines the exported buffers and functions from ceu_ctl_and_data_processing.c.
 **********************************************************************************************************************/
/***************************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 ***************************************************************************************************************************/

#include "ceu_ctl_and_data_processing.h"
#include "hal_data.h"
#include "board_cfg.h"
#include "common_util.h"
#include "dwt.h"


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
 * Exported global variables and functions (to be accessed by other files)
 ***************************************************************************************************************************/

/* QVGA rgb565 initial output buffer. This is a double buffer.*/
uint8_t           camera_out_buffer[2][CAM_QVGA_WIDTH  * CAM_QVGA_HEIGHT * CAM_BYTE_PER_PIXEL ] BSP_PLACE_IN_SECTION(".sdram") BSP_ALIGN_VARIABLE(8);

/*Rotated QVGA rgb565 for mipi display.*/

uint8_t           camera_out_rot_buffer565[ CAM_QVGA_WIDTH  * CAM_QVGA_HEIGHT * CAM_BYTE_PER_PIXEL ] BSP_ALIGN_VARIABLE(8);


/* Inference engine input buffer */
int8_t model_buffer_int8[DET_MODEL_IMG_SIZE_X * DET_MODEL_IMG_SIZE_Y];

uint8_t s_ceu_buffer = 0;


/***************************************************************************************************************************
 * Private global variables and functions
 ***************************************************************************************************************************/

static  uint32_t last_ceu_frame_end = 0;
static face_det_err_t rotate_16bit_image_90_clockwise(uint8_t* input_image, uint8_t* output_image, int input_width, int input_height);


/*********************************************************************************************************************
 *  Crop square, downsample and convert RGB565 to 8bpp grayscale (integer).
 *  @param[IN]   const void * inbuf: input 320x240 RGB565 image buffer
 *  @param[IN]   void * outbuf: output 192x192 int8 image buffer
 *  @param[IN]   uint16_t width: width of the input buffer
 *  @param[IN]   uint16_t height: height of the input buffer
 *  @param[IN]   uint16_t width: width of the output buffer
 *  @param[IN]   uint16_t height: height of the output buffer
***********************************************************************************************************************/

face_det_err_t image_rgb565_to_int8 (const void * inbuf, void * outbuf, uint16_t in_width, uint16_t in_height, uint16_t out_width, uint16_t out_height)
{

    face_det_err_t face_det_status = FACE_DET_APP_SUCCESS;

    if ((NULL == inbuf) || (NULL == outbuf))
    {
        face_det_status = FACE_DET_APP_NULL_POINTER;
        return face_det_status;
    }
    if(in_width < in_height)
    {
        face_det_status = FACE_DET_APP_IMG_PROCESS;
        return face_det_status;
    }
    register uint16_t * p_input  = (uint16_t *) inbuf;
    register int8_t   * p_output = (int8_t *) outbuf;

    /* Pixels to skip at the start of each line to crop to the center square */
    const uint32_t crop_offset = (in_width - in_height) / IMG_CHANNEL;

    for (uint32_t y = 0; y < out_height; y++)
    {
        for (uint32_t x = 0; x < out_width; x++)
        {
            /* Number of pixels in the Y-offset */
            uint32_t y_offset = in_width * ((in_height * y) / out_height);

            /* Pointer to the pixel at the start of the target cropped line */
            uint16_t * p_input_base = p_input + crop_offset + y_offset;

            /* Add X-offset for input pixels in cropped image. */
            uint16_t input = *(p_input_base + ((in_height * x) / out_width) );

            /* Approximate RGB to grayscale weighting of 0.25/0.50/0.125 */
            uint8_t weighted_sum = (uint8_t) (((input >> 11) << 1) + ((input >> 4) & 0x7E) + (input & 0x1F));

            /* Convert to integer range */
            *p_output++ = (int8_t) (weighted_sum - 0x80);
        }
    }
    return face_det_status;
}

/*********************************************************************************************************************
 *  Rotate an input image of 16 bit per pixel clockwise for 90 degree.
 *  @param[IN]   uint8_t* input_image: input image buffer rgb565
 *  @param[IN]   uint8_t* output_image: output image buffer rgb565
 *  @param[IN]   int input_width: width of the input buffer in pixel
 *  @param[IN]   int input_height: height of the input buffer in pixel
 *  @retval      None
***********************************************************************************************************************/
static face_det_err_t rotate_16bit_image_90_clockwise(uint8_t* input_image, uint8_t* output_image, int input_width, int input_height)
{
    face_det_err_t face_det_status = FACE_DET_APP_SUCCESS;

    if ((NULL == input_image) || (NULL == output_image))
    {
       face_det_status = FACE_DET_APP_NULL_POINTER;
       return face_det_status;
    }

    uint16_t * p_input  = (uint16_t *) input_image;
    uint16_t * p_output = (uint16_t *) output_image;

    for(int32_t y = 0; y < input_height; y++)
    {

        for(int32_t x = 0; x < input_width; x ++)
        {
            *(p_output + ((x * input_height) + (input_height - y - 1))) = *p_input++;
        }

    }

    return face_det_status;
}
/*********************************************************************************************************************
 *  Rotate the image 90 degree clockwise
 *  @param[IN]   None
 *  @retval      None
***********************************************************************************************************************/

face_det_err_t rotate_camera_image(void)
{
	volatile int64_t old_cycle_count =  GetCycleCounter();
	static int64_t new_cycle_count = 0;
	face_det_err_t face_det_status = FACE_DET_APP_SUCCESS;


	xEventGroupWaitBits(g_ai_app_event, CAM_DATA_READY, pdFALSE, pdTRUE, 1);

	SCB_InvalidateDCache_by_Addr(&camera_out_buffer[s_ceu_buffer][0], sizeof(camera_out_buffer) / 2);
	face_det_status = rotate_16bit_image_90_clockwise(camera_out_buffer[s_ceu_buffer], camera_out_rot_buffer565, CAM_QVGA_WIDTH, CAM_QVGA_HEIGHT);


	new_cycle_count = GetCycleCounter();

	/* do nothing if the DWT is reset during the rotation. */
	if(new_cycle_count-old_cycle_count>0)
		face_det_timing.image_rotation_in_ms = (uint32_t)((new_cycle_count - old_cycle_count)/SCALE_DWT_TO_MS);

	return face_det_status;

}

/*********************************************************************************************************************
 *  Camera Interrupt callback
 *  @param[IN]   capture_callback_args_t * p_args: callback context
 *  @retval      None
***********************************************************************************************************************/
__attribute__((section(".itcm_data"))) __attribute__((__used__)) __attribute__((__noinline__))
void g_ceu_user_callback (capture_callback_args_t * p_args)
{
	 BaseType_t xHigherPriorityTaskWoken, xResult;
	/* xHigherPriorityTaskWoken must be initialised to pdFALSE. */
	  xHigherPriorityTaskWoken = pdFALSE;

	if (CEU_EVENT_FRAME_END & p_args->event )
	{


		face_det_timing.ceu_capture_t = (GetCycleCounter()-last_ceu_frame_end);

		last_ceu_frame_end = GetCycleCounter();
		xResult = xEventGroupSetBitsFromISR(g_ai_app_event, CAM_DATA_READY, &xHigherPriorityTaskWoken);
		/* Was the message posted successfully? */
		if( xResult != pdFAIL )
		{
		  /* If xHigherPriorityTaskWoken is now set to pdTRUE then a context
		  switch should be requested.  The macro used is port specific and will
		  be either portYIELD_FROM_ISR() or portEND_SWITCHING_ISR() - refer to
		  the documentation page for the port being used. */
		  portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
		}

		R_CEU_CaptureStart(&g_ceu_ctrl, camera_out_buffer[s_ceu_buffer]);

		s_ceu_buffer = (s_ceu_buffer == 0 ? 1:0);

	}

}
