/*
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/


/**********************************************************************************************************************
 * File Name    : ai_thread_entry.c
 * Description  : This file defines the entry function of the ai thread and activates the inference. It also includes
 * 				  the image pre processing before inference.
 **********************************************************************************************************************/
/***************************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 ***************************************************************************************************************************/
#include <ai_thread.h>
#include "ceu_ctl_and_data_processing.h"
#include "common_data.h"
#include "dwt.h"
#include <stdio.h>
#include "common_util.h"


/***************************************************************************************************************************
 * Macro definitions
 ***************************************************************************************************************************/
#define AI_THREAD_YIELD						(400U)

/***************************************************************************************************************************
 * Typedef definitions
 ***************************************************************************************************************************/
/***************************************************************************************************************************
 * Imported global variables and functions (from other files)
 ***************************************************************************************************************************/

extern uint8_t s_ceu_buffer;
extern face_det_err_t face_detection (void);
extern face_det_err_t ai_init(void);

/***************************************************************************************************************************
 * Exported global variables and functions (to be accessed by other files)
 ***************************************************************************************************************************/

st_ai_detection_point_t g_ai_detection[MAX_DETECTION_NUMS] = {};
void update_detection_result(uint16_t index, signed short  x, signed short  y, signed short  w, signed short  h);
bool update_face_count = false;

/***************************************************************************************************************************
 * Private global variables and functions
 ***************************************************************************************************************************/


/*********************************************************************************************************************
 *  Read the face detection result to a buffer which will be used by the mipi display function.
 *  @param[IN]   index: index of the image in the result list
 *  @param[IN]   x: x coordinate of the bottom left corner
 *  @param[IN]   y: y coordinate of the bottom left corner
 *  @param[IN]   w: width of the face detected
 *  @param[IN]   h: height of the face detected
 *  @retval      None
***********************************************************************************************************************/
void update_detection_result(uint16_t index, signed short  x, signed short  y, signed short  w, signed short  h)
{
    if(index < MAX_DETECTION_NUMS)
    {
        g_ai_detection[index].m_x = x;
        g_ai_detection[index].m_y = y;
        g_ai_detection[index].m_w = w;
        g_ai_detection[index].m_h = h;
    }

}

/*********************************************************************************************************************
 *  AI thread entry function. The image will be processed prior to inference.
 *  The image processing and inference is repeatedly carried out in this thread.
 *  @param[IN]   	void *pvParameters, contains TaskHandle_t, not used.
 *  @retval      None
***********************************************************************************************************************/

void ai_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);

	face_det_err_t face_det_status = FACE_DET_APP_SUCCESS;


	InitCycleCounter();
	EnableCycleCounter();

	face_det_status = ai_init();
	if(FACE_DET_APP_SUCCESS != face_det_status )
	{
		handle_error(FACE_DET_APP_AI_INIT);
	}

		/* consider check return */
	xEventGroupWaitBits(g_ai_app_event, HARDWARE_INIT_DONE, pdFALSE, pdTRUE, 1);
    while (true)
    {

    	volatile uint64_t t2 = GetCycleCounter();

    	xEventGroupWaitBits(g_ai_app_event, CAM_DATA_READY, pdFALSE, pdTRUE, 1);

    	SCB_InvalidateDCache_by_Addr(&camera_out_buffer[s_ceu_buffer][0], sizeof(camera_out_buffer) / 2);

    	face_det_status = image_rgb565_to_int8((void *) (&(camera_out_buffer[s_ceu_buffer])), model_buffer_int8, CAM_QVGA_WIDTH, CAM_QVGA_HEIGHT, DET_MODEL_IMG_SIZE_X, DET_MODEL_IMG_SIZE_Y);
		if (FACE_DET_APP_SUCCESS != face_det_status)
		{
			handle_error(FACE_DET_APP_IMG_PROCESS);
		}

		face_det_timing.pre_inference_image_processing_in_ms = (uint32_t)((GetCycleCounter() - t2)/SCALE_DWT_TO_MS);

		/* restart face detection statistics for each inference */

		for(int i=0; i<MAX_DETECTION_NUMS; i++)
			memset(&g_ai_detection[i], 0, sizeof(g_ai_detection[i]));

		face_det_status = face_detection();
		if(FACE_DET_APP_INFERENCE == face_det_status)
		{
			handle_error(FACE_DET_APP_INFERENCE);
		}

		update_face_count = true;

		ResetCycleCounter();
		/*
		 * Yield to the display thread. The AI thread does not need to run faster than human reaction and response time,
		 * so a relatively larger delay is used. This value should not be too low; otherwise, the display thread
		 * is negatively influenced. This value should be reevaluated if the system is updated.
		 */
		vTaskDelay(AI_THREAD_YIELD);
    }
}
