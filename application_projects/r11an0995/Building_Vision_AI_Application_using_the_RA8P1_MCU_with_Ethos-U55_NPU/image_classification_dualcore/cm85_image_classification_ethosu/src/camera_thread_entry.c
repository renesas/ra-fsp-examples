/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/

#include "hal_data.h"
#include "vector_data.h"
#include "camera_thread.h"
#include "common_data.h"
#include "common_util.h"
#include "time_counter.h"
#include "bsp_api.h"
#include "ov5640_cfg.h"
#include "stdlib.h"
#include "ai_application_config.h"
#include "../ra/fsp/inc/instances/r_mipi_phy.h"
#include "camera_control.h"


/***************************************************************************************************************************
 * Macro definitions
 ***************************************************************************************************************************/

/*====================================================================================================
 * Debug and similar configuration / documentation
 ====================================================================================================*/
/* Validated resolutions:
 *  --- Camera configured for YUV422 format
 *  --- Camera SysClk and MIPIClk both at 280MHz
 *
 *                |  RGB565     | RGB888
 * ---------------|-------------------------------------
 * - 640  x 480   |  Success    |
 * - 1024 x 600   |  Success    |
 * - 1280 x 960   |  Success    | VIN FIFO Overflow
 * - 1280 x 1024  |  Success    | VIN FIFO Overflow
 * - 1920 x 1080  |  Fail       | Fail
 * 0 */


/*====================================================================================================
 * The following types and definitions taken from ov_5640_cfg_mipi.c
 ====================================================================================================*/



// Up to 1 - no conversion if none
#define OUTPUT_FORMAT_YUV 0
#define OUTPUT_FORMAT_RGB565 1
#define OUTPUT_FORMAT_RGB1555 0
#define OUTPUT_FORMAT_RGB888 0
#define OUTPUT_FORMAT_RGB8888 0

#define BYTES_PER_PIXEL_INPUT  (2U) /* YUV422 configuration is checked asserted below */

#if OUTPUT_FORMAT_YUV
    #define BYTES_PER_PIXEL_OUTPUT (2)
#elif OUTPUT_FORMAT_RGB565
    #define BYTES_PER_PIXEL_OUTPUT (2)
#elif OUTPUT_FORMAT_RGB1555
    #define BYTES_PER_PIXEL_OUTPUT (2)
#elif OUTPUT_FORMAT_RGB888
    #define BYTES_PER_PIXEL_OUTPUT (4)
#elif OUTPUT_FORMAT_RGB8888
    #define BYTES_PER_PIXEL_OUTPUT (4)
#else
    #define BYTES_PER_PIXEL_OUTPUT (2)
#endif
#define BYTE_PER_PIXEL      (2U) /* YUV422 configuration is check asserted below */

#if OUTPUT_FORMAT_RGB565 || OUTPUT_FORMAT_RGB1555 || OUTPUT_FORMAT_RGB888 || OUTPUT_FORMAT_RGB8888
    #define OUTPUT_FORMAT_RGB 1
#else
    #define OUTPUT_FORMAT_RGB 0
#endif

#define IMAGE_BYTES (IMAGE_WIDTH*IMAGE_HEIGHT*BYTES_PER_PIXEL_OUTPUT)
#define IMAGE_PADDING (IMAGE_WIDTH*BYTES_PER_PIXEL_OUTPUT*10)  // 10 lines buffer between image frames for better memory view

/***************************************************************************************************************************
 * Imported global variables and functions (from other files)
 ***************************************************************************************************************************/

extern void display_next_buffer_set(uint8_t* next_buffer);

extern int8_t model_buffer_int8[];

/***************************************************************************************************************************
 * Private global variables and functions
 ***************************************************************************************************************************/
static volatile uint32_t time_counter_start = 0;
static volatile uint32_t time_counter_end = 0;
static uint32_t g_pclka_hz = 0;
static uint32_t g_pclka_mhz = 100000000U;
static float    g_pclka_period_ns = 0;


/*********************************************************************************************************************
 *  Crop square, downsample and convert 640x480 RGB565 to 224x224 RGB888 image.
 *  @param[IN]   const void * p_input_image_buff: input RGB565 image
 *  @param[IN]   void * p_output_image_buff: output RGB888 image
 *  @param[IN]   uint16_t in_width: width of the input buffer
 *  @param[IN]   uint16_t in_height: height of the input buffer
 *  @param[IN]   uint16_t out_width: width of the output buffer
 *  @param[IN]   uint16_t out_height: height of the output buffer
***********************************************************************************************************************/
vision_ai_app_err_t image_rgb565_to_rgb888(const void *p_input_image_buff, void *p_output_image_buff,
                                           uint16_t in_width, uint16_t in_height,
                                           uint16_t out_width, uint16_t out_height) {
    if (!p_input_image_buff || !p_output_image_buff) {
        return VISION_AI_APP_ERR_NULL_POINTER;
    }

    if (in_width < in_height) {
        return VISION_AI_APP_ERR_IMG_PROCESS;
    }

    register uint16_t *p_input = (uint16_t *)p_input_image_buff;
    register uint8_t *p_output = (uint8_t *)p_output_image_buff;

    uint32_t crop_offset = (in_width - in_height) / 2;

    for (uint32_t y = 0; y < out_height; y++) {
        for (uint32_t x = 0; x < out_width; x++) {
            uint32_t y_offset = in_width * ((in_height * y) / out_height);
            uint16_t *p_input_base = p_input + crop_offset + y_offset;
            uint16_t input = *(p_input_base + ((in_height * x) / out_width));

            uint8_t r = (input >> 11) & 0x1F;
            uint8_t g = (input >> 5) & 0x3F;
            uint8_t b = input & 0x1F;

            r = (r * 255) / 31;
            g = (g * 255) / 63;
            b = (b * 255) / 31;

            *p_output++ = r;
            *p_output++ = g;
            *p_output++ = b;
        }
    }

    return VISION_AI_APP_SUCCESS;
}

/*
 * Nothing needs to be done in this application project.
 */
void ipc_nmi_callback(void)
{
    ;
}
/* Camera Thread entry function */
/* pvParameters contains TaskHandle_t */
void camera_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);
    fsp_err_t status = FSP_SUCCESS;

    R_BSP_PinAccessEnable();
    // Output low to enable MIPI I/F on the EK-RA8P1
    R_IOPORT_PinWrite(&g_ioport_ctrl, MIPI_IF_EN, BSP_IO_LEVEL_LOW);
    R_BSP_PinAccessDisable();

    if(FSP_SUCCESS != R_IIC_MASTER_Open(&g_i2c_master_board_ctrl, &g_i2c_master_board_cfg))
    {
        handle_error(VISION_AI_APP_ERR_IIC_MASTER_OPEN);

    }

    status = R_VIN_Open(&g_vin0_ctrl, &g_vin0_cfg);
    if(FSP_SUCCESS != status)
    {
        handle_error(VISION_AI_APP_ERR_VIN_OPEN);
    }

    g_pclka_hz = R_FSP_SystemClockHzGet(FSP_PRIV_CLOCK_PCLKA);
    g_pclka_mhz = (uint8_t) (g_pclka_hz / 1000000);
    g_pclka_period_ns = 1000000000.0f / ((float)g_pclka_hz);

    
    TimeCounter_Init();
    TimeCounter_CountReset();
	

    R_LPM_Open(&g_lpm_sleep_ctrl, &g_lpm_sleep_cfg);
   R_IPC_Open(g_ipc0.p_ctrl, g_ipc0.p_cfg);
   R_BSP_IpcNmiEnable(ipc_nmi_callback);

    /** Setup The Camera **/

    ov5640_init();
    OV5640_stream_off();
    ov5640_write_reg(0x3008, 0x42); // software power down

    R_BSP_SoftwareDelay((uint32_t)1, BSP_DELAY_UNITS_MILLISECONDS);
    R_VIN_CaptureStart(&g_vin0_ctrl, NULL);
    R_BSP_SoftwareDelay((uint32_t)1, BSP_DELAY_UNITS_MILLISECONDS);

    ov5640_write_reg(0x3008, 0x02); // wake up
    OV5640_stream_on();
    // Set camera initialization complete flag
    xEventGroupSetBits(g_ai_app_event, HARDWARE_CAMERA_INIT_DONE);
   while (true)
    {
       capture_status_t capture_status;
       R_VIN_StatusGet(&g_vin0_ctrl, &capture_status);

       time_counter_start = TimeCounter_CurrentCountGet();
       image_rgb565_to_rgb888(vin_image_buffer_1, &model_buffer_int8[0],
                                    (uint16_t)IMAGE_WIDTH, (uint16_t)IMAGE_HEIGHT, (uint16_t)AI_INPUT_IMAGE_WIDTH, (uint16_t)AI_INPUT_IMAGE_HEIGHT);
       // Set AI inference input image ready flag. AI inference thread may waiting this flag set.
       xEventGroupSetBits(g_ai_app_event, AI_INFERENCE_INPUT_IMAGE_READY);
       time_counter_end = TimeCounter_CurrentCountGet();
       application_processing_time.ai_inference_pre_processing_time_ms = TimeCounter_CountValueConvertToMs(time_counter_start, time_counter_end);
       vTaskDelay (30);
    }
}





