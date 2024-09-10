
/**********************************************************************************************************************
 * File Name    : display_thread_entry.c
 * Version      : .
 * Description  : The display thread operations.
 *********************************************************************************************************************/


/***************************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 ***************************************************************************************************************************/
#include <display_thread.h>
#include <stdio.h>
#include <string.h>
#include "ov3640.h"
#include "display_mipi.h"
#include "graphics.h"
#include "jlink_console.h"
#include "bg_font_18_full.h"
#include "common_util.h"
#include "ceu_ctl_and_data_processing.h"
#include "dwt.h"

/***************************************************************************************************************************
 * Macro definitions
 ***************************************************************************************************************************/
#define SCREEN_MODE 			d2_mode_rgb565
#define DISPLAY_THREAD_YIELD	(20U)

/***************************************************************************************************************************
 * Typedef definitions
 ***************************************************************************************************************************/

/***************************************************************************************************************************
 * Imported global variables and functions (from other files)
 ***************************************************************************************************************************/

extern void  do_face_reconition_screen(void);

/***************************************************************************************************************************
 * Exported global variables and functions (to be accessed by other files)
 ***************************************************************************************************************************/

/***************************************************************************************************************************
 * Private global variables and functions
 ***************************************************************************************************************************/



/*********************************************************************************************************************
 *  display thread entry function
 *  			 This thread initializes all the hardware and display the camera input to the mipi lcd.
 *  @param[IN]   void *pvParameters: not used
 *  @retval      None
***********************************************************************************************************************/
void display_thread_entry(void *pvParameters)
{
	fsp_err_t status = FSP_SUCCESS;
	face_det_err_t face_det_status = FACE_DET_APP_SUCCESS;

    FSP_PARAMETER_NOT_USED(pvParameters);

   	/* Initialize the subsystems other than the camera*/
    bsp_sdram_init();

    status = jlink_console_init();
    if (FSP_SUCCESS != status)
	{
		handle_error(FACE_DET_APP_JLINK_CONSOLE_OPEN);
	}

	face_det_status = drw_display_init();
	if(FACE_DET_APP_SUCCESS != face_det_status)
	{
		handle_error(FACE_DET_APP_GRAPHICS_INIT);
	}

	/* Initialize the camera and start capture*/
	face_det_status = camera_init();
	if (FACE_DET_APP_SUCCESS != face_det_status)
	{
		handle_error(FACE_DET_APP_CAMERA_INIT);
	}

	R_CEU_CaptureStart(&g_ceu_ctrl, (uint8_t * const )&camera_out_buffer);

	xEventGroupSetBits(g_ai_app_event, HARDWARE_INIT_DONE);

	while (true)
	{

		do_face_reconition_screen();

		vTaskDelay(DISPLAY_THREAD_YIELD);

	}


}


