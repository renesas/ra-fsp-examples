
/*
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/**********************************************************************************************************************
 * File Name    : face_detection_screen_mipi.c
 * Version      : .
 * Description  : The face detection screen display on mipi lcd.
 *********************************************************************************************************************/
/***************************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 ***************************************************************************************************************************/

#include "ceu_ctl_and_data_processing.h"
#include "dwt.h"
#include "jlink_console.h"
#include "hal_data.h"
#include "display_mipi.h"
#include "graphics.h"
#include "common_util.h"
#include "bg_font_18_full.h"
#include <stdio.h>

/***************************************************************************************************************************
 * Macro definitions
 * Refer to the application note for the physical definition of these values
 ***************************************************************************************************************************/
#define RGB565_RED					(0xFF0000)
#define SIZE_A 						(80)
#define SIZE_B						(107)

/***************************************************************************************************************************
 * Typedef definitions
 ***************************************************************************************************************************/

/***************************************************************************************************************************
 * Imported global variables and functions (from other files)
 ***************************************************************************************************************************/

extern st_ai_detection_point_t g_ai_detection[MAX_DETECTION_NUMS];
extern bool update_face_count;

/***************************************************************************************************************************
 * Exported global variables and functions (to be accessed by other files)
 ***************************************************************************************************************************/

uint32_t face_detection_inference_time;
void  do_face_reconition_screen(void);
uint8_t face_count = 0;

/***************************************************************************************************************************
 * Private global variables and functions
 ***************************************************************************************************************************/
#if defined(ENABLE_STATISTICS_OUTPUT)
	static uint32_t last_cycle_count = 0;
	static void print_statistic(void);
#endif
static d2_point top_right_x[MAX_DETECTION_NUMS];
static d2_point top_right_y[MAX_DETECTION_NUMS];
static d2_point bottom_left_x[MAX_DETECTION_NUMS];
static d2_point bottom_left_y[MAX_DETECTION_NUMS];
static d2_point scaled_h[MAX_DETECTION_NUMS];

static void print_static_text (void);
static void draw_bounding_box(uint8_t i);
static void calculate_and_draw_bounding_box(uint8_t i);
static void display_camera_image(void);

/*********************************************************************************************************************
 *  display_camera_image function
 *  			 This function selects the rotated camera image buffer in QVGA mode 240x320 format and
 *  			 displays in VGA 480x640 format at the center of the mipi lcd.
 *  @param   	None
 *  @retval     None.
***********************************************************************************************************************/
static void display_camera_image(void)
{
	/* QVGA 240x320 as input */
	d2_setblitsrc(d2_handle, camera_out_rot_buffer565, CAM_QVGA_HEIGHT, CAM_QVGA_HEIGHT, CAM_QVGA_WIDTH, d2_mode_rgb565);

	/* display as VGA 640x480 on mipi lcd */
	d2_blitcopy(d2_handle,
						 CAM_QVGA_HEIGHT, CAM_QVGA_WIDTH,  // Source width/height
										(d2_blitpos) 0, 0, // Source position
										(d2_width) ((CAM_VGA_HEIGHT) << 4), (d2_width) ((CAM_VGA_WIDTH) << 4), // Destination size width/height
										(d2_width) (((480 - CAM_VGA_HEIGHT)/2) << 4), (d2_width) (((LCD_VPIX - CAM_VGA_WIDTH)/2) << 4), // Destination offset position
										d2_tm_filter);
}

/*********************************************************************************************************************
 *  draw_bounding_box function
 *  			 This function picks the index of the detection result which has a bounding box and
 *  			 uses DRW to render a red bounding box on the mipi lcd.
 *  @param[IN]   i: index of the detection result which has detected a face
 *  @retval     None
***********************************************************************************************************************/
static void draw_bounding_box(uint8_t i)
{
	d2_setcolor(d2_handle, 0, RGB565_RED);

	d2_renderline(d2_handle, (d2_point) ((top_right_x[i]) << 4), (d2_point) ((top_right_y[i])<< 4), (d2_point) ((bottom_left_x[i]) << 4), (d2_point) ((top_right_y[i]) << 4), (d2_point) (2 << 4), 0);
	d2_renderline(d2_handle, (d2_point) ((bottom_left_x[i]) << 4), (d2_point) ((top_right_y[i]) << 4), (d2_point) ((bottom_left_x[i]) << 4), (d2_point) ((bottom_left_y[i]) << 4), (d2_point) (2 << 4), 0);
	d2_renderline(d2_handle, (d2_point) ((bottom_left_x[i]) << 4), (d2_point) ((bottom_left_y[i]) << 4), (d2_point) ((top_right_x[i]) << 4), (d2_point) ((bottom_left_y[i]) << 4), (d2_point) (2 << 4), 0);
	d2_renderline(d2_handle, (d2_point) ((top_right_x[i]) << 4), (d2_point) ((bottom_left_y[i]) << 4), (d2_point) ((top_right_x[i]) << 4), (d2_point) ((top_right_y[i]) << 4), (d2_point) (2 << 4), 0);

}

/*********************************************************************************************************************
 *  calculate_and_draw_bounding_box function
 *  This function takes the ai inference boundary box center of the image and scales it to the 480x640
 *  mipi lcd center area as a bounding box.
 *  @param[IN]   i: index of the detection result which has detected a face
 *  @retval     None
***********************************************************************************************************************/
static void calculate_and_draw_bounding_box(uint8_t i)
{
	/* these are the coordinate in a 480x480 space for AI coordinate*/
	signed short scaled_x = (signed short)((g_ai_detection[i].m_x)*CAM_VGA_WIDTH * IMAGE_INPUT_WIDTH / CAM_QVGA_WIDTH  / DET_MODEL_IMG_SIZE_X);
	signed short scaled_y = (signed short)((g_ai_detection[i].m_y)*CAM_VGA_WIDTH * IMAGE_INPUT_WIDTH / CAM_QVGA_WIDTH  / DET_MODEL_IMG_SIZE_X);
	signed short scaled_w = (signed short)((g_ai_detection[i].m_w)*CAM_VGA_WIDTH * IMAGE_INPUT_WIDTH / CAM_QVGA_WIDTH  / DET_MODEL_IMG_SIZE_X);

	face_count++;

	/* The following code converts the detection result x coordinates to mipi coordinates which is in the opposite direction. */
	/* The mipi x coordinate should reference the detection result y coordinate */
	/* refer to the accompanying application note to understand the geometry meaning of the literals */
	volatile d2_point inf_top_right_x = scaled_y + scaled_h[i];
	volatile d2_point inf_top_right_y = scaled_x + SIZE_A + SIZE_B + scaled_w;
	volatile d2_point inf_bottom_left_x = scaled_y;
	volatile d2_point inf_bottom_left_y = scaled_x + SIZE_A + SIZE_B;


	/*Reverse the inference result mipi coordinate to get the DRW coordinate*/
	top_right_x[i] = LCD_HPIX - inf_bottom_left_x;
	top_right_y[i] = inf_top_right_y;
	bottom_left_x[i] = LCD_HPIX - inf_top_right_x;
	bottom_left_y[i] =  inf_bottom_left_y;

	/* adjust the edge so the bounding box is in the visible area */
	if( top_right_x[i] > CAM_VGA_HEIGHT)
		top_right_x[i] = 478;
	if (top_right_y[i] > LCD_VPIX-107)
		top_right_y[i] = LCD_VPIX-108;
	if(bottom_left_x[i] < 0)
		bottom_left_x[i] = 2;
	if(bottom_left_y[i] < 107)
		bottom_left_y[i] = 109;

	draw_bounding_box(i);
}

#if defined(ENABLE_STATISTICS_OUTPUT)
/*********************************************************************************************************************
 *  print_statistic function
 *  This function prints the statics on the fps and time usage for the major operations in the face detection application.
 *  ToDo: reformat the output
 *  @param   	None
 *  @retval     None
***********************************************************************************************************************/
static void print_statistic(void)
{
	/* print statistic information every other vsync */
	static bool print_now = false;
	if(true == print_now)
	{
		print_now = false;
		uint32_t ms_period = face_det_timing.ceu_capture_t/SCALE_DWT_TO_MS;
		/* Only update the CEU fps if no DWT reset */
		if(ms_period < CONVERT_SEC_TO_MILLISEC)
		{
			/* perform calculation here to reduce operation inside callback */
			face_det_timing.ceu_fps = (uint32_t)(CONVERT_SEC_TO_MILLISEC/(face_det_timing.ceu_capture_t /SCALE_DWT_TO_MS));
		}

		sprintf (sprintf_buffer, "\r\npre Inference Image Processing takes %u ms; image rotation time takes %u ms\r\n", (unsigned int)face_det_timing.pre_inference_image_processing_in_ms, (unsigned int)face_det_timing.image_rotation_in_ms);
						print_to_console(sprintf_buffer);
		sprintf(sprintf_buffer, "\r\nmipi lcd display is refreshed at %u fps;  camera capture is at %u fps\r\n",  (unsigned int)face_det_timing.mipi_lcd_fps, (unsigned int)face_det_timing.ceu_fps);
				print_to_console(sprintf_buffer);

	}
	else
	{
		print_now = true;
	}

}
#endif

/*********************************************************************************************************************
 *  print_static_text function
 *  This function prints the static text which does not change based on inference result.
 *  @param   	None
 *  @retval     None
***********************************************************************************************************************/
static void print_static_text(void)
{
	/* show model information */
	print_bg_font_18(d2_handle, 380, 16,  (char*)"Model:");

	print_bg_font_18(d2_handle, 330, 24,  (char*)"Yolo-");
	print_bg_font_18(d2_handle, 300, 24,  (char*)"fastest");
	/*show inference time in ms*/
	print_bg_font_18(d2_handle, 200, 16,  (char*)"Inference");
	print_bg_font_18(d2_handle, 170, 16,  (char*)"time:");

	/*instruct user to look into the camera*/
	print_bg_font_18(d2_handle, 360, 780,  (char*)"Please ");
	print_bg_font_18(d2_handle, 330, 780,  (char*)"look ");
	print_bg_font_18(d2_handle, 300, 780,  (char*)"into ");
	print_bg_font_18(d2_handle, 270, 780,  (char*)"the ");
	print_bg_font_18(d2_handle, 240, 780,  (char*)"camera.");

	/*print the number of faces detected */
	print_bg_font_18(d2_handle, 190, 780,  (char*)"No of ");
	print_bg_font_18(d2_handle, 160, 780,  (char*)"Faces:");

}

/*********************************************************************************************************************
 *  print_inf_time_and_number_faces function
 *  This function prints the time used in the previously finished inference and the number of faces detected.
 *  @param   	None
 *  @retval     None
***********************************************************************************************************************/
static void print_inf_time_and_number_faces(void)
{
	/* The face_detection_inference_time is acquired in UseCaseHandler_obj.cc.
	 * This time does not include the pre and post processing routine.
	 * It is the time used for inference only.
	 */

	uint32_t time = (uint32_t)(face_detection_inference_time); // ms

	char time_str[8] = {'0', '0', '0', '0', ' ', 'm', 's', '\0'};
	print_bg_font_18(d2_handle, 120, 30,  "          ");
	time_str[0] += (char)(time / 1000);
	time_str[1] += (char)((time / 100) % 10);
	time_str[2] += (char)((time / 10) % 10);
	time_str[3] += (char)(time % 10);
	print_bg_font_18(d2_handle, 120, 30,  (char*)time_str);

	// update string on display
	char num_str[3] = {'0', '0', '\0'};
	print_bg_font_18(d2_handle, 120, 780,  "    ");
	num_str[0] += (char) (face_count / 10);
	num_str[1] +=  (char) (face_count % 10);
	print_bg_font_18(d2_handle, 120, 780,  (char*)num_str);

}

/*********************************************************************************************************************
 *  do_face_reconition_screen function: display the camera image and face detection result on the mipi lcd
 *  @param       None
 *  @retval      None
***********************************************************************************************************************/

void  do_face_reconition_screen(void)
{
#if defined(ENABLE_STATISTICS_OUTPUT)
    volatile uint32_t t1 = 0;
    volatile uint32_t time_elapsed_in_ms = 0;
#endif
    face_det_err_t face_det_status = FACE_DET_APP_SUCCESS;

    xEventGroupWaitBits(g_ai_app_event, HARDWARE_INIT_DONE, pdFALSE, pdTRUE, 1);

    face_det_status = rotate_camera_image();
    if (FACE_DET_APP_SUCCESS != face_det_status)
    {
    	handle_error(FACE_DET_APP_IMG_ROTATION);
    }

	/* Wait for vertical blanking period */
	xEventGroupWaitBits(g_ai_app_event, GLCDC_VSYNC, pdFALSE, pdTRUE, 1);
	graphics_start_frame();

	/* show static information */
	if(in_transition())
	{
		print_static_text();
	}
	else
	{
		#if defined(ENABLE_STATISTICS_OUTPUT)
		{
			t1 = GetCycleCounter();

			time_elapsed_in_ms = (t1-last_cycle_count)/SCALE_DWT_TO_MS;
			/* This is to skip report if DWT is reset. */
			if(time_elapsed_in_ms < CONVERT_SEC_TO_MILLISEC)
			{
				face_det_timing.mipi_lcd_fps =  (uint32_t)(CONVERT_SEC_TO_MILLISEC/time_elapsed_in_ms);
			}

			last_cycle_count = GetCycleCounter();
			print_statistic();
		}
		#endif

		display_camera_image();

		/* if a new inference has finished, update the detection result: bounding box and number of faces */
		if(true == update_face_count)
		{
		    face_count = 0;
			for(uint8_t i=0; i<MAX_DETECTION_NUMS; i++)
			{
				scaled_h[i] = (d2_point)((g_ai_detection[i].m_h)*CAM_VGA_WIDTH * IMAGE_INPUT_WIDTH / CAM_QVGA_WIDTH  / DET_MODEL_IMG_SIZE_X);
				/* only handle the result if there is a face detected */
				if(scaled_h[i] != 0)
				{
					calculate_and_draw_bounding_box(i);
				}
			}

			update_face_count = false;
		}
		else
		{
			/* Human movement is slower than the mipi lcd refresh rate. Keep the previous bounding box until a new inference is finished. */
			for(uint8_t i=0; i<MAX_DETECTION_NUMS; i++)
			{
				if (scaled_h[i] != 0)
					draw_bounding_box(i);
			}

		}

		print_inf_time_and_number_faces();
	}
	/* Wait for previous frame rendering to finish, then finalize this frame and flip the buffers */
	graphics_end_frame();
}
