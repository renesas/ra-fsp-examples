/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : common_util.c
 * Description  : This file contains implementation of the the error handling function.
 **********************************************************************************************************************/
/***************************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 ***************************************************************************************************************************/

#include "console_output.h"
#include "hal_data.h"
#include "stdio.h"
#include "common_util.h"

/***************************************************************************************************************************
 * Macro definitions
 ***************************************************************************************************************************/

#define ENABLE_BREAK_POINT_AT_FAULT

/***************************************************************************************************************************
 * Typedef definitions
 ***************************************************************************************************************************/

/***************************************************************************************************************************
 * Imported global variables and functions (from other files)
 ***************************************************************************************************************************/
/***************************************************************************************************************************
 * Exported global variables and functions (to be accessed by other files)
 ***************************************************************************************************************************/
processinf_time_info_t application_processing_time;

/***********************************************************************************************************************
 * Private global variables and functions
 ************************************************************************************************************************/

/*********************************************************************************************************************
 * This function is used by the AI for debugging output. Refer to log_macros.h.
 * By default, this function is not being used.
 * @param format
 * @return 0
  **********************************************************************************************************************/
int e_printf(const char *format, ...)
{
	sprintf(sprintf_buffer, format);
	print_to_console(sprintf_buffer);
    return 0;
}

/*********************************************************************************************************************
 *  @brief       This function handle error if error occurred, closes all opened modules, print and traps error.
 *  @param[in]   status    error status
 *  @param[in]   err_str   error string
 *  @retval      None
 **********************************************************************************************************************/
void handle_error (vision_ai_app_err_t err)
{
	switch (err)
	{
	   case VISION_AI_APP_ERR_AI_INIT:
	   {
		   /*Suggest debugging into the code to identify the issue. */
			sprintf(sprintf_buffer, "\r\n[failure: FACE_DET_APP_AI_INIT]: AI init failed.  \r\n");
			print_to_console(sprintf_buffer);

		   break;
	   }
	   case VISION_AI_APP_ERR_AI_INFERENCE:
	   {
		   	sprintf(sprintf_buffer, "\r\n[failure: FACE_DET_APP_INFERENCE]: AI inference failed.  \r\n");
			print_to_console(sprintf_buffer);
			break;
	   }
	   case VISION_AI_APP_ERR_IMG_PROCESS:
	   {
		   	sprintf(sprintf_buffer, "\r\n[failure: FACE_DET_APP_IMG_PROCESS]: Image cropping failed. \r\n");
			print_to_console(sprintf_buffer);
			break;
	   }
	   case VISION_AI_APP_ERR_IMG_ROTATION:
	   {
		  	sprintf(sprintf_buffer, "\r\n[failure: FACE_DET_APP_IMG_ROTATION]: Image rotation failed. \r\n");
			print_to_console(sprintf_buffer);
			break;
	   }
	   case VISION_AI_APP_ERR_NULL_POINTER:
	   {
		   /*Suggest valid the input and output buffer.  */
			sprintf(sprintf_buffer, "\r\n[failure: FACE_DET_APP_NULL_POINTER]: input buffer not allocated. \r\n");
			print_to_console(sprintf_buffer);
			break;
	   }
 	   case VISION_AI_APP_ERR_GLCDC_OPEN:
	   {
		   /*Suggest valid the input and output buffer.  */
			sprintf(sprintf_buffer, "\r\n[failure: FACE_DET_APP_GLCDC_OPEN]: R_GLCDC_Open returned error. \r\n");
			print_to_console(sprintf_buffer);
			break;
	   }
	   case VISION_AI_APP_ERR_MIPI_CMD:
	   {
		   /*Suggest valid the input and output buffer.  */
			sprintf(sprintf_buffer, "\r\n[failure: FACE_DET_APP_MIPI_CMD]: R_MIPI_DSI_Command returned error. \r\n");
			print_to_console(sprintf_buffer);
			break;
	   }
	   case VISION_AI_APP_ERR_GLCDC_START:
	   {
		   /*Suggest valid the input and output buffer.  */
			sprintf(sprintf_buffer, "\r\n[failure: FACE_DET_APP_GLCDC_START]: R_GLCDC_Start returned error. \r\n");
			print_to_console(sprintf_buffer);
			break;
	   }
	   case VISION_AI_APP_ERR_GLCDC_LAYER_CHANGE:
	   {
		   /*Suggest valid the input and output buffer.  */
			sprintf(sprintf_buffer, "\r\n[failure: FACE_DET_APP_GLCDC_LAYER_CHANGE]: R_GLCDC_LayerChange returned error. \r\n");
			print_to_console(sprintf_buffer);
			break;
	   }
	   case VISION_AI_APP_ERR_GRAPHICS_INIT:
	   {
		   /*Suggest valid the input and output buffer.  */
			sprintf(sprintf_buffer, "\r\n[failure: FACE_DET_APP_GRAPHICS_INIT: Graphics system initialization returned error. \r\n");
			print_to_console(sprintf_buffer);
			break;
	   }
	   case VISION_AI_APP_ERR_GPT_OPEN:
	   {
		   /*Suggest valid the input and output buffer.  */
			sprintf(sprintf_buffer, "\r\n[failure: FACE_DET_APP_GPT_OPEN]: R_GPT_Open returned error. \r\n");
			print_to_console(sprintf_buffer);
			break;
	   }
	   case VISION_AI_APP_ERR_CEU_OPEN:
	   {
		   /*Suggest valid the input and output buffer.  */
			sprintf(sprintf_buffer, "\r\n[failure: FACE_DET_APP_CEU_OPEN]: R_CEU_Open returned error. \r\n");
			print_to_console(sprintf_buffer);
			break;
	   }
	   case VISION_AI_APP_ERR_CAMERA_INIT:
	   {
		   /*Suggest valid the input and output buffer.  */
			sprintf(sprintf_buffer, "\r\n[failure: FACE_DET_APP_CAMERA_INIT]: Camera initialization returned error. \r\n");
			print_to_console(sprintf_buffer);
			break;
	   }
	   case VISION_AI_APP_ERR_IIC_MASTER_OPEN:
	   {
		   /*Suggest valid the input and output buffer.  */
			sprintf(sprintf_buffer, "\r\n[failure: FACE_DET_APP_IIC_MASTER_OPEN]: R_IIC_MASTER_Open returned error. \r\n");
			print_to_console(sprintf_buffer);
			break;
	   }
	   case VISION_AI_APP_ERR_WRITE_OV3640_REG:
	   {
		   /*Suggest valid the input and output buffer.  */
			sprintf(sprintf_buffer, "\r\n[failure: FACE_DET_APP_WRITE_OV3640_REG: write OV3640 register returned error. \r\n");
			print_to_console(sprintf_buffer);
			break;
	   }
	   case VISION_AI_APP_ERR_WRITE_SENSOR_ARRAY:
	   {
		   /*Suggest valid the input and output buffer.  */
			sprintf(sprintf_buffer, "\r\n[failure: FACE_DET_APP_WRITE_SENSOR_ARRAY: write OV3640 register array returned error. \r\n");
			print_to_console(sprintf_buffer);
			break;
	   }
	   case VISION_AI_APP_ERR_CONSOLE_OPEN:
	   {
		   /*Suggest valid the input and output buffer.  */
			sprintf(sprintf_buffer, "\r\n[failure: FACE_DET_APP_JLINK_CONSOLE_OPEN]: R_SCI_B_UART_Open returned error. \r\n");
			print_to_console(sprintf_buffer);
			break;
	   }
	   case VISION_AI_APP_ERR_IIC_MASTER_WRITE:
	   {
		   /*Suggest valid the input and output buffer.  */
			sprintf(sprintf_buffer, "\r\n[failure: FACE_DET_APP_IIC_MASTER_WRITE: R_IIC_MASTER_Write returned error. \r\n");
			print_to_console(sprintf_buffer);
			break;
	   }
	   case VISION_AI_APP_ERR_IIC_MASTER_READ:
	   {
		   /*Suggest valid the input and output buffer.  */
			sprintf(sprintf_buffer, "\r\n[failure: FACE_DET_APP_IIC_MASTER_READ: R_IIC_MASTER_Read returned error. \r\n");
			print_to_console(sprintf_buffer);
			break;
	   }
	   case VISION_AI_APP_ERR_CONSOLE_WRITE:
	   {
		   /*Suggest valid the input and output buffer.  */
			sprintf(sprintf_buffer, "\r\n[failure: FACE_DET_APP_JLINK_SONSOLE_WRITE: R_SCI_B_UART_Write returned error. \r\n");
			print_to_console(sprintf_buffer);
			break;
	   }
	   case VISION_AI_APP_ERR_CONSOLE_READ:
	   {
		   /*Suggest valid the input and output buffer.  */
			sprintf(sprintf_buffer, "\r\n[failure: FACE_DET_APP_JLINK_SONSOLE_READ: R_SCI_B_UART_Read returned error. \r\n");
			print_to_console(sprintf_buffer);
			break;
	   }
	   case VISION_AI_APP_ERR_EXTERNAL_IRQ_INIT:
	   {
           /*Suggest valid the input and output buffer.  */
            sprintf(sprintf_buffer, "\r\n[failure: VISION_AI_APP_ERR_EXTERNAL_IRQ_INIT: R_ICU_ExternalIrq returned error. \r\n");
            print_to_console(sprintf_buffer);
            break;
	   }
	   case VISION_AI_APP_ERR_VIN_OPEN:
        {
         /*Suggest valid the input and output buffer.  */
          sprintf(sprintf_buffer, "\r\n[failure: VISION_AI_APP_ERR_VIN_OPEN: R_VIN_Open returned error. \r\n");
          print_to_console(sprintf_buffer);
          break;
        }

	   default:
	   {
		   break;
	   }
	}

	/* if user wants to debug the project, then enable macro ENABLE_DEBUG_OUTPUT */
	#if defined(ENABLE_BREAK_POINT_AT_FAULT)
	 	 __asm("BKPT #0\n"); /* trap upon the error  */
	#else

	/*
	 * This section can be updated to implement a system recovery scheme which is out of scope of this example application.
	 * By default, this application breaks execution at a fault with ENABLE_BREAK_POINT_AT_FAULT defined.
	 */

	 #endif
}


