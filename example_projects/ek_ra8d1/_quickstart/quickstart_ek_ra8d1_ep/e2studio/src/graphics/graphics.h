/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/**********************************************************************************************************************
 * File Name    : graphics.h
 * Version      : .
 * Description  : .
 *********************************************************************************************************************/

#ifndef _GRAPHICS_H
#define _GRAPHICS_H

#include <stdint.h>
#include "hal_data.h"
#include "dave_driver.h"

#ifdef NORMAL_SCREEN
 #define LCD_HPIX         (800)
 #define LCD_STRIDE       (800)
 #define LCD_VPIX         (480)
#else
 #define LCD_HPIX         (480)
 #define LCD_STRIDE       (480)
 #define LCD_VPIX         (854)

// #define LCD_VPIX       (858)
#endif

#define LCD_COLOR_SIZE    (2)
#define LCD_BUF_NUM       (2)

extern d2_device * d2_handle;

void     graphics_init(void);
void   * graphics_get_draw_buffer();
void     graphics_start_frame();
void     graphics_end_frame();
void     graphics_swap_buffer();
void     graphics_wait_vsync();
uint32_t graphics_hsv2rgb888(float h, float s, float v);

#endif
