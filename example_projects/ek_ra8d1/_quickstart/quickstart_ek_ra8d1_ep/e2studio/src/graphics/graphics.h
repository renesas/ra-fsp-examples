/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/

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
