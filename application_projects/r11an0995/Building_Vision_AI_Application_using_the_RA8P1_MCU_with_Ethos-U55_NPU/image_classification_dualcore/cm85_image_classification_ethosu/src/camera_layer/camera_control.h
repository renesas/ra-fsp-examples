/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/



#ifndef CAMERA_LAYER_CAMERA_CONTROL_H_
#define CAMERA_LAYER_CAMERA_CONTROL_H_

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "hal_data.h"
#include "camera_thread.h"

/***************************************************************************************************************************
 * Macro definitions
 * Refer to the application note for the physical definition of these values
 ***************************************************************************************************************************/
#define OV5640_TEST_IMAGE_ENABLE      (0)
#define OV5640_TEST_IMAGE_ROLLING     (0)

#define OV5640_TEST_IMAGE_TRANSPARENT (0)
#define OV5640_TEST_IMAGE_SQUARE      (0)
#define OV5640_TEST_IMAGE_BAR_STYLE   (1) /* 0: 8-Bar, 1: Gradual Vertical Mode 1, 2: Gradual Horizontal, 3: Gradual vertical Mode 2 */
#define OV5640_TEST_IMAGE_TYPE        (0) /* 0: Color Bar, 1: Random Data, 2, Squares, 3: Black */

/*------------------------------------------------------*/
/*---------- OV5640 MIPI Image Configuration: ----------*/
/*------------------------------------------------------*/
#define OV5640_CONTINUOUS_CLOCK       (1) // Disabling continuous clock makes timing more difficult - Not recommended

#define MIPI_NUM_LANES (2)


#define OV5640_MIPI_GLOBAL_TIMING_VALUE (0x0a)

#define MIPI_XCLK_HZ (24000000) // 20 MHz

#define OV5640_HSCLK_MHZ    (185)
#define OV5640_HSCLK_HZ     (OV5640_HSCLK_MHZ * 1000000)

/*====================================================================================================
 * Camera Config Macros
 ====================================================================================================*/

#define INPUT_FORMAT_YUV422_8_BIT 1  // 0x3034 == 0x18
#define INPUT_FORMAT_YUV422_10_BIT 0 // 0x3034 == 0x1A
#define INPUT_FORMAT_RAW8 0
#define INPUT_FORMAT_RGB888 0

#define delay_ms(x)  R_BSP_SoftwareDelay(x, BSP_DELAY_UNITS_MILLISECONDS)

#define IMAGE_WIDTH         (uint32_t)(g_plcd_display_cfg.input[0].hsize)
#define IMAGE_HEIGHT        (uint32_t)(g_plcd_display_cfg.input[0].vsize)

/***************************************************************************************************************************
 * Exported global variables and functions (to be accessed by other files)
 ***************************************************************************************************************************/
void OV5640_SetMIPIVirtualChannel(uint32_t vchannel);
void OV5640_stream_on(void);
void OV5640_stream_off(void);
uint8_t ov5640_init(void);

extern uint8_t * gp_next_buffer;
#endif /* CAMERA_LAYER_CAMERA_CONTROL_H_ */
