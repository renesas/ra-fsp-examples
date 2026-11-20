/***********************************************************************************************************************
 * File Name    : glcdc_ep.h
 * Description  : Contains macros, variables, and functions used in glcdc_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef GLCDC_EP_H_
#define GLCDC_EP_H_

#include "hal_data.h"

/* Macro for the reset value */
#define RESET_VALUE             (0x00)

/* Macro for the first (and only) GLCDC input layer used by this EP */
#define ARRAY_INDEX              (0U)

/* RGB 16-bit color format */
#ifdef DISPLAY_IN_FORMAT_16BITS_RGB565_0
    #define BYTES_PER_PIXEL     (2U)            /* 16-bit format */
    #define RED                 (0xF800)
    #define GREEN               (0x07E0)
    #define BLUE                (0x001F)
    #define BLACK               (0x0000)
    #define WHITE               (0xFFFF)
    #define YELLOW              (0xFFE0)
    #define MAGENTA             (0xF81F)
    #define CYAN                (0x07FF)
#endif /* DISPLAY_IN_FORMAT_16BITS_RGB565_0 */

/* RGB 32-bit color format */
#ifdef DISPLAY_IN_FORMAT_32BITS_RGB888_0
    #define BYTES_PER_PIXEL     (4U)            /* 32-bit format */
    #define RED                 (0x00FF0000)
    #define GREEN               (0x0000FF00)
    #define BLUE                (0x000000FF)
    #define BLACK               (0x00000000)
    #define WHITE               (0xFFFFFFFF)
    #define YELLOW              (0xFFFFFF00)
    #define MAGENTA             (0x00FF00FF)
    #define CYAN                (0x0000FFFF)
#endif /* DISPLAY_IN_FORMAT_32BITS_RGB888_0 */

/* Coordinate and display configuration macros */
#define X1_CO_ORDINATE          (0U)
#define Y1_CO_ORDINATE          (0U)
#define COLOR_BAND_COUNT        (8U)
#define INC_DEC_VALUE           (1U)

/* External variables - Defined in glcdc_ep.c */
extern uint16_t g_hz_size;
extern uint16_t g_vr_size;
extern uint8_t * g_p_single_buffer;
extern uint8_t * g_p_double_buffer;

/* Function declarations */
fsp_err_t glcdc_init(void);
void glcdc_deinit(void);
void glcdc_draw_color_bands(void);

#endif /* GLCDC_EP_H_ */
