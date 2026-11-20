/***********************************************************************************************************************
 * File Name    : drw_ep.h
 * Description  : Contains macros, variables, and functions used in drw_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2022 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef DRW_EP_H_
#define DRW_EP_H_

#include <stddef.h>
#include <stdbool.h>
#include "hal_data.h"
#if defined(BOARD_RA8D1_EK)
#include "r_mipi_dsi_api.h"
#endif /* BOARD_RA8D1_EK */

/* Macro for the reset value */
#define RESET_VALUE             (0x00)

/* Macro for GLCDC VSYNC timeout */
#define GLCDC_VSYNC_TIMEOUT     (1000U)

/* Macro for frame buffer pixel size configuration */
#if defined(BOARD_RA8E2_EK) || defined(BOARD_RA8P1_EK) || defined(BOARD_RA8D2_EK)
#define BYTES_PER_PIXEL         (4U)
#else
#define BYTES_PER_PIXEL         (2U)
#endif /* Specific board */

/* Macros for general DRW configuration */
#define ARRAY_INDEX             (0U)
#define BITFIELD_VALUE          (0U)

/* Macros for shape position and dimension */
#if defined(BOARD_RA8D1_EK)
#define CIRCLE_VAL_X            (360)
#define CIRCLE_VAL_Y            (165)
#define CIRCLE_VAL_R            (45)
#define CIRCLE_VAL_W            (0)
#define TRIANGLE_X1             (285)
#define TRIANGLE_Y1             (560)
#define TRIANGLE_X2             (390)
#define TRIANGLE_Y2             (660)
#define TRIANGLE_X3             (285)
#define TRIANGLE_Y3             (760)
#define BOX_VAL_X1              (65)
#define BOX_VAL_Y1              (80)
#define BOX_VAL_LEN             (135)
#define BOX_VAL_WIDTH           (200)
#define BOX_PARAM               (270)
#define LINE_VAL_X1             (175)
#define LINE_VAL_Y1             (760)
#define LINE_VAL_X2             (65)
#define LINE_VAL_Y2             (580)
#define LINE_VAL_WIDTH          (6)
#else
#define CIRCLE_VAL_X            (SCALE_X(120))
#define CIRCLE_VAL_Y            (SCALE_Y(68))
#define CIRCLE_VAL_R            (SCALE_DIM(25))
#define CIRCLE_VAL_W            (0)
#define TRIANGLE_X1             (SCALE_X(300))
#define TRIANGLE_Y1             (SCALE_Y(90))
#define TRIANGLE_X2             (SCALE_X(360))
#define TRIANGLE_Y2             (SCALE_Y(38))
#define TRIANGLE_X3             (SCALE_X(420))
#define TRIANGLE_Y3             (SCALE_Y(90))
#define BOX_VAL_X1              (SCALE_X(60))
#define BOX_VAL_Y1              (SCALE_Y(164))
#define BOX_VAL_LEN             (SCALE_X(120))
#define BOX_VAL_WIDTH           (SCALE_Y(80))
#define BOX_PARAM               (SCALE_DIM(20))
#define LINE_VAL_X1             (SCALE_X(300))
#define LINE_VAL_Y1             (SCALE_Y(240))
#define LINE_VAL_X2             (SCALE_X(400))
#define LINE_VAL_Y2             (SCALE_Y(180))
#define LINE_VAL_WIDTH          (SCALE_DIM(4))
#endif /* BOARD_RA8D1_EK */

/* Macros for DRW rendering configuration */
#define ALPHA_VALUE             (155U)
#define ANTI_ALIASING_VAL       (1U)
#define BUFFER_CLEAR_VAL        (0x000000)
#define RED_COLOR_VAL           (0xFF0000)
#define GREEN_COLOR_VAL         (0x00FF00)
#define BLUE_COLOR_VAL          (0x0000FF)
#define YELLOW_COLOR_VAL        (0xFFFF00)
#define SHIFT_VALUE             (4U)

#if (!defined(BOARD_RA8D1_EK))
/* Macros to calculate scaled values for LCDs other than Graphics Expansion Board P/N RTK7EK6M3B00001BU */
#define SCALE_X(val)            ((val) * DRW_TEST_IMAGE_RES_H / 480)
#define SCALE_Y(val)            ((val) * DRW_TEST_IMAGE_RES_V / 272)
#define SCALE_DIM(val)          (((val) * (DRW_TEST_IMAGE_RES_H + DRW_TEST_IMAGE_RES_V)) / (480 + 272))

/* Scaling factors for step size of animation */
#define STEP_X                  (DRW_TEST_IMAGE_RES_H / 480)
#define STEP_Y                  (DRW_TEST_IMAGE_RES_V / 272)
#endif /* BOARD_RA8D1_EK */

/* Macros for display resolution configuration */
#if defined(BOARD_RA8P1_EK) || defined(BOARD_RA8D2_EK)
#define DRW_TEST_IMAGE_RES_H    (1024)
#define DRW_TEST_IMAGE_RES_V    (600)
#elif defined(BOARD_RA8E2_EK)
#define DRW_TEST_IMAGE_RES_H    (800)
#define DRW_TEST_IMAGE_RES_V    (480)
#elif defined(BOARD_RA8D1_EK)
#define DRW_TEST_IMAGE_RES_H    (480)
#define DRW_TEST_IMAGE_RES_V    (854)
#else
#define DRW_TEST_IMAGE_RES_H    (480)
#define DRW_TEST_IMAGE_RES_V    (272)
#endif /* Specific board */

#if defined(BOARD_RA8D1_EK)
/* Macros for MIPI DSI LCD initialization */
#define MIPI_DSI_DISPLAY_CONFIG_DATA_DELAY_FLAG     ((mipi_dsi_cmd_id_t) 0xFE)
#define MIPI_DSI_DISPLAY_CONFIG_DATA_END_OF_TABLE   ((mipi_dsi_cmd_id_t) 0xFD)

/* LCD initialization command table entry */
typedef struct
{
    unsigned char size;
    unsigned char buffer[10];
    mipi_dsi_cmd_id_t cmd_id;
    mipi_dsi_cmd_flag_t flags;
} lcd_table_setting_t;
#endif /* BOARD_RA8D1_EK */

/* External variables - Defined in drw_ep.c */
extern volatile uint8_t g_vsync_flag;
extern d2_device * gp_davey;

/* Function declarations */
fsp_err_t glcdc_init(void);
fsp_err_t glcdc_buffer_change(display_ctrl_t const * const p_api_ctrl, uint8_t * const framebuffer,
                              display_frame_layer_t layer);
fsp_err_t glcdc_stop(void);
void glcdc_deinit(void);
d2_s32 drw_init(void);
void drw_deinit(void);
#if defined(DISPLAY_IN_FORMAT_32BITS_RGB888_0)
d2_s32 display_draw(uint32_t * framebuffer);
#else
d2_s32 display_draw(uint8_t * framebuffer);
#endif /* DISPLAY_IN_FORMAT_32BITS_RGB888_0 */

#endif /* DRW_EP_H_ */
