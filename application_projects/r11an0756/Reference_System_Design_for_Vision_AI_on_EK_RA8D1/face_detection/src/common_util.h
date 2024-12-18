/**********************************************************************************************************************
 * File Name    : common_util.h
 * Description  : This file contains the error handling related micro definition and exported function.
 **********************************************************************************************************************/


#ifndef COMMON_UTIL_H_
#define COMMON_UTIL_H_

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "hal_data.h"

/**********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/* sync events */
#define CAM_DATA_READY    	(1 << 0)   /* One frame is captured */
#define HARDWARE_INIT_DONE	(2 << 0)
#define GLCDC_VSYNC 		(3 << 0)
#define MIPI_MESSAGE_SENT 	(4 << 0)


#define ENABLE_STATISTICS_OUTPUT

/* AI model input size */
#define DET_MODEL_IMG_SIZE_X 192
#define DET_MODEL_IMG_SIZE_Y 192

/* This definition selects how many faces will be presented in the detection result */
#define MAX_DETECTION_NUMS 20

#define APP_ERROR_TRAP(err)        if(err) { __asm("BKPT #0\n");} /* system execution breaks  */

#define CPU_CLOCK	R_FSP_SystemClockHzGet(FSP_PRIV_CLOCK_CPUCLK)
#define CONVERT_SEC_TO_MILLISEC	(1000)
#define SCALE_DWT_TO_MS	(CPU_CLOCK/CONVERT_SEC_TO_MILLISEC)

/**********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/* The coordinate of the bounding box corner, bounding box width and height based on 192x192 gray pixel area */
typedef struct ai_detection_point_t {
  signed short      m_x;
  signed short      m_y;
  signed short      m_w;
  signed short      m_h;
} st_ai_detection_point_t;

typedef enum
{
    CAM_VGA_WIDTH          = 640,
    CAM_VGA_HEIGHT         = 480,
    CAM_QVGA_WIDTH         = 320,
    CAM_QVGA_HEIGHT        = 240,

} camera_size_list_t;

#define CAM_BYTE_PER_PIXEL   			(2)
#define RGB888_BYTE_PER_PIXEL			(3)

/* image intermediate input size to convert to MIPI display size */
#define IMAGE_INPUT_WIDTH		CAM_QVGA_HEIGHT
#define IMAGE_INPUT_HEIGHT		CAM_QVGA_HEIGHT

/** Common error codes */
typedef enum e_face_det_app_err
{
    FACE_DET_APP_SUCCESS 				= 0,

    FACE_DET_APP_AI_INIT             	= 1,                    ///< AI init failed
	FACE_DET_APP_INFERENCE			 	= 2,					///< AI inference failed
	FACE_DET_APP_IMG_PROCESS		 	= 3,					///< Image crop failed
	FACE_DET_APP_IMG_ROTATION			= 4,					///< Image rotation failed
	FACE_DET_APP_NULL_POINTER		 	= 5,					///< null pointer
	FACE_DET_APP_GLCDC_OPEN				= 6,					///< glcdc open failed
	FACE_DET_APP_MIPI_CMD			 	= 7,					///< mipi command failed
	FACE_DET_APP_GLCDC_START		 	= 8, 					///< glcdc start failed
	FACE_DET_APP_GLCDC_LAYER_CHANGE		= 9, 					///< graphics layer change failed
	FACE_DET_APP_GRAPHICS_INIT			= 10, 					///< One of the graphics system initialization failed
	FACE_DET_APP_GPT_OPEN				= 11,					///< GPT open failed
	FACE_DET_APP_CEU_OPEN				= 12,					///< CEU open failed
	FACE_DET_APP_WRITE_OV3640_REG		= 13, 					///< Write OV3640 register failed
	FACE_DET_APP_WRITE_SENSOR_ARRAY	    = 14,					///< Write OV3640 register array failed
	FACE_DET_APP_CAMERA_INIT			= 15,					///< Camera init failed
	FACE_DET_APP_IIC_MASTER_OPEN		= 16,					///< IIC master open failed
	FACE_DET_APP_IIC_MASTER_WRITE		= 17,					///< IIC master write failed
	FACE_DET_APP_IIC_MASTER_READ		= 18,					///< IIC master read failed
	FACE_DET_APP_JLINK_CONSOLE_OPEN		= 19,					///< jlink uart open error
	FACE_DET_APP_JLINK_SONSOLE_WRITE	= 20,					///< JLink console write failed
	FACE_DET_APP_JLINK_SONSOLE_READ		= 21,					///< JLink console read failed


} face_det_err_t;

/** timing report */
typedef struct st_timing_info_t
{
	volatile uint32_t ceu_capture_t;				///< time in cpu cycle between two camera frame end event
	uint32_t ceu_fps;                 				///< CEU fps.
	uint32_t mipi_lcd_fps;                			///< mipi lcd refresh fps.
	uint32_t image_rotation_in_ms;					///< image rotation time usage in ms.
	uint32_t pre_inference_image_processing_in_ms; 	///< time usage of image processing prior to inference

} timing_info_t;


/**********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/

extern char sprintf_buffer[];
extern timing_info_t face_det_timing;
extern void handle_error (face_det_err_t err);

#endif /* COMMON_UTIL_H_ */
