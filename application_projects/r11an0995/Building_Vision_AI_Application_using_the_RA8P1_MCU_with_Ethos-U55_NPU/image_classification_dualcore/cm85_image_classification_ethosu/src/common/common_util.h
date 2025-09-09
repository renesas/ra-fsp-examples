/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : common_utils.h
 * Version      : .
 * Description  : .
 *********************************************************************************************************************/
#ifndef COMMON_UTIL_H__
#define COMMON_UTIL_H__

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "hal_data.h"
#include "ai_application_config.h"

#define EP_SCREEN_MODE d2_mode_rgb565
#define BYTES_PER_PIXEL  2       //16 bit format

#define LED1_PIN (BSP_IO_PORT_06_PIN_00)
#define LED2_PIN (BSP_IO_PORT_03_PIN_03)
#define LED3_PIN (BSP_IO_PORT_10_PIN_07)


#define LED1_ON  R_IOPORT_PinWrite(NULL, LED1_PIN, BSP_IO_LEVEL_HIGH)
#define LED1_OFF R_IOPORT_PinWrite(NULL, LED1_PIN, BSP_IO_LEVEL_LOW)
#define LED2_ON  R_IOPORT_PinWrite(NULL, LED2_PIN, BSP_IO_LEVEL_HIGH)
#define LED2_OFF R_IOPORT_PinWrite(NULL, LED2_PIN, BSP_IO_LEVEL_LOW)
#define LED3_ON  R_IOPORT_PinWrite(NULL, LED3_PIN, BSP_IO_LEVEL_HIGH)
#define LED3_OFF R_IOPORT_PinWrite(NULL, LED3_PIN, BSP_IO_LEVEL_LOW)

#if (ENABLE_INFERENCE_RUNNING_LED == 1)
#define INFERENCE_START_INDICATE_LED LED1_ON
#define INFERENCE_END_INDICATE_LED   LED1_OFF
#else
#define INFERENCE_START_INDICATE_LED
#define INFERENCE_END_INDICATE_LED
#endif

#if (ENABLE_CAMERA_CAPTURE_RUNNING_LED == 1)
#define CAMERA_CAPTURE_END_INDICATE_LED_ON  LED2_ON
#define CAMERA_CAPTURE_END_INDICATE_LED_OFF LED2_OFF
#else
#define CAMERA_CAPTURE_END_INDICATE_LED_ON
#define CAMERA_CAPTURE_END_INDICATE_LED_OFF
#endif

#define ERROR_INDICATE_LED_ON  LED3_ON
#define ERROR_INDICATE_LED_OFF LED3_OFF

#define ERROR_INDICATE ERROR_INDICATE_LED_ON; __BKPT(0)

/* sync events */
#define HARDWARE_DISPLAY_INIT_DONE      (1 << 0)
#define HARDWARE_CAMERA_INIT_DONE       (1 << 1)
#define HARDWARE_ETHOSU_INIT_DONE       (1 << 2)
#define SOFTWARE_AI_INFERENCE_INIT_DONE (1 << 3)
#define GLCDC_VSYNC                     (1 << 10)
#define MIPI_MESSAGE_SENT               (1 << 11)
#define CAMERA_CAPTURE_COMPLETED        (1 << 12)
#define AI_INFERENCE_INPUT_IMAGE_READY  (1 << 13)
#define AI_INFERENCE_RESULT_UPDATED     (1 << 14)
#define DISPLAY_PAUSE                   (1 << 15)
#define CAMERA_AUTO_FOCUS_EXECUTE       (1 << 16)

#define APP_ERROR_TRAP(err)        if(err) { __asm("BKPT #0\n");} /* system execution breaks  */

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

/* The possibilities of the detected category based on 224x224x3 rgb pixel area */
typedef struct ai_classification_point_t {
  unsigned short    category;
  float             prob;
} st_ai_classification_point_t;

typedef enum
{
    CAM_VGA_WIDTH          = 640,
    CAM_VGA_HEIGHT         = 480,
    CAM_QVGA_WIDTH         = 320,
    CAM_QVGA_HEIGHT        = 240,

} camera_size_list_t;

#define CAM_BYTE_PER_PIXEL              (2)
#define RGB888_BYTE_PER_PIXEL           (3)

/* image intermediate input size to convert to MIPI display size */
#define IMAGE_INPUT_WIDTH       CAM_QVGA_HEIGHT
#define IMAGE_INPUT_HEIGHT      CAM_QVGA_HEIGHT

/** Common error codes */
typedef enum e_vision_ai_app_err
{
    VISION_AI_APP_SUCCESS                = 0,

    VISION_AI_APP_ERR_AI_INIT            = 1,  ///< AI init failed
    VISION_AI_APP_ERR_AI_INFERENCE       = 2,  ///< AI inference failed
    VISION_AI_APP_ERR_IMG_PROCESS        = 3,  ///< Image crop failed
    VISION_AI_APP_ERR_IMG_ROTATION       = 4,  ///< Image rotation failed
    VISION_AI_APP_ERR_NULL_POINTER       = 5,  ///< null pointer
    VISION_AI_APP_ERR_GLCDC_OPEN         = 6,  ///< glcdc open failed
    VISION_AI_APP_ERR_MIPI_CMD           = 7,  ///< mipi command failed
    VISION_AI_APP_ERR_GLCDC_START        = 8,  ///< glcdc start failed
    VISION_AI_APP_ERR_GLCDC_LAYER_CHANGE = 9,  ///< graphics layer change failed
    VISION_AI_APP_ERR_GRAPHICS_INIT      = 10, ///< One of the graphics system initialization failed
    VISION_AI_APP_ERR_GPT_OPEN           = 11, ///< GPT open failed
    VISION_AI_APP_ERR_CEU_OPEN           = 12, ///< CEU open failed
    VISION_AI_APP_ERR_WRITE_OV3640_REG   = 13, ///< Write OV3640 register failed
    VISION_AI_APP_ERR_WRITE_SENSOR_ARRAY = 14, ///< Write OV3640 register array failed
    VISION_AI_APP_ERR_CAMERA_INIT        = 15, ///< Camera init failed
    VISION_AI_APP_ERR_IIC_MASTER_OPEN    = 16, ///< IIC master open failed
    VISION_AI_APP_ERR_IIC_MASTER_WRITE   = 17, ///< IIC master write failed
    VISION_AI_APP_ERR_IIC_MASTER_READ    = 18, ///< IIC master read failed
    VISION_AI_APP_ERR_CONSOLE_OPEN       = 19, ///< jlink uart open error
    VISION_AI_APP_ERR_CONSOLE_WRITE      = 20, ///< JLink console write failed
    VISION_AI_APP_ERR_CONSOLE_READ       = 21, ///< JLink console read failed
    VISION_AI_APP_ERR_EXTERNAL_IRQ_INIT  = 22, ///< External IRQ init failed
    VISION_AI_APP_ERR_VIN_OPEN           = 23, ///< VIN Open Error
} vision_ai_app_err_t;

/** process_time report */
typedef struct st_processing_time_info_t
{
    uint32_t camera_image_capture_time_ms;          ///< Camera frame capture time
    uint32_t camera_post_processing_time_ms;        ///< Post processing time for captured camera image
    uint32_t lcd_display_update_refresh_ms;         ///< LCD display refresh time
    uint32_t ai_inference_pre_processing_time_ms;   ///< Pre processing time for AI inference
    uint32_t ai_inference_time_ms;                  ///< AI inference processing time
} processinf_time_info_t;

/**********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/

extern char sprintf_buffer[];
extern processinf_time_info_t application_processing_time;
extern st_ai_detection_point_t g_ai_detection[AI_MAX_DETECTION_NUM];
extern st_ai_classification_point_t g_ai_classification[AI_MAX_DETECTION_NUM];

FSP_CPP_HEADER
void handle_error (vision_ai_app_err_t err);

FSP_CPP_FOOTER

#endif /* COMMON_UTIL_H__ */
