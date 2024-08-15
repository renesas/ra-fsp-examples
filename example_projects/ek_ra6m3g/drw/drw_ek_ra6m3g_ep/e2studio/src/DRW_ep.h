/***********************************************************************************************************************
 * File Name    : drw_ep.h
 * Description  : Contains declarations of data structures used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2022 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef DRW_EP_H_
#define DRW_EP_H_

#define EP_INFO   "\r\nThe Example Project demonstrates the basic functionalities of the DRW device driver \r\n"\
                  "on Renesas RA MCUs based on Renesas FSP. On successful initialization of the DRW and GLCDC modules,\r\n"\
                  "the EP will be able to draw various shapes and shape fills across the LCD screen, as well as use\r\n"\
                  "Alpha Blending on the images rendered to cover the entire screen of an external LCD connected to the RA MCU.\r\n"\
                  "Additionally, API Failure messages will be displayed on RTT Viewer.\r\n\n "\


#define GLCDC_VSYNC_TIMEOUT     (1000)
#define BYTES_PER_PIXEL         (2U)
#define ARRAY_INDEX             (0U)
#define BITFIELD_VALUE          (0U)
#define RENDER_DELAY            (25U)
#define SW_DELAY                (15U)
#define RESET_FLAG              (0U)
#define SET_FLAG                (1U)

/* Co-ordinates for various shapes drawn on the display */
#define CIRCLE_VAL_X            (120)
#define CIRCLE_VAL_Y            (68)
#define CIRCLE_VAL_R            (25)
#define CIRCLE_VAL_W            (0)
#define TRIANGLE_X1             (300)
#define TRIANGLE_Y1             (90)
#define TRIANGLE_X2             (360)
#define TRIANGLE_Y2             (38)
#define TRIANGLE_X3             (420)
#define TRIANGLE_Y3             (90)
#define BOX_VAL_X1              (60)
#define BOX_VAL_Y1              (164)
#define BOX_VAL_LEN             (120)
#define BOX_VAL_WIDTH           (80)
#define BOX_PARAM               (20)
#define LINE_VAL_X1             (300)
#define LINE_VAL_Y1             (240)
#define LINE_VAL_X2             (400)
#define LINE_VAL_Y2             (180)
#define LINE_VAL_WIDTH          (4)

#define ALPHA_VALUE             (102U)
#define ANTI_ALIASING_VAL       (1U)
#define BUFFER_CLEAR_VAL        (0x000000)
#define RED_COLOR_VAL           (0xFF0000)
#define GREEN_COLOR_VAL         (0x00FF00)
#define BLUE_COLOR_VAL          (0x0000FF)
#define YELLOW_COLOR_VAL        (0xFFFF00)
#define SHIFT_VALUE             (4U)



#define DRW_TEST_IMAGE_RES_H    (480)
#define DRW_TEST_IMAGE_RES_V    (272)


#endif /* DRW_EP_H_ */
