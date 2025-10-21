/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : display_thread_entry.h
 * Version      : 1.0
 * Description  : I/F for FreeRTOS thread display_thread.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 20.10.2025 1.00     First Release
 *********************************************************************************************************************/


#ifndef DISPLAY_THREAD_ENTRY_H_
#define DISPLAY_THREAD_ENTRY_H_

#include <stdint.h>
#include "hal_data.h"
#include "dave_driver.h"

#define EP_SCREEN_MODE_TP (d2_mode_argb4444)
#define EP_SCREEN_MODE_FG (d2_mode_rgb565)


/* RGB 16-bit color format*/
#ifdef DISPLAY_IN_FORMAT_16BITS_RGB565_0

    #define BYTES_PER_PIXEL  (2)       //16 bit format
    #define GLCD_RED         (0xF8000)
    #define GLCD_GREEN       (0x07E0)
    #define GLCD_BLUE        (0x001F)
    #define GLCD_BLACK       (0x0000)
    #define GLCD_WHITE       (0xFFFF)
    #define GLCD_YELLOW      (0xFFE0)
    #define GLCD_MAGENTA     (0xF81F)
    #define GLCD_CYAN        (0x07FF)

    #define GLCD_CUSTOM      (0x0F22)

#endif

#ifdef DISPLAY_IN_FORMAT_32BITS_RGB888_0
    #define USE_32_BIT (1)
#endif
#ifdef  DISPLAY_IN_FORMAT_32BITS_ARGB8888_0
    #define USE_32_BIT (1)
#endif

#ifdef USE_32_BIT
    #define EP_SCREEN_MODE (d2_mode_argb8888)

    #define BYTES_PER_PIXEL  (4)       //32 bit format
    #define GLCD_RED         (0xFF0000)
    #define GLCD_GREEN       (0x00FF00)
    #define GLCD_BLUE        (0x0000FF)
    #define GLCD_BLACK       (0x000000)
    #define GLCD_WHITE       (0xFFFFFF)
    #define GLCD_YELLOW      (0xFFFF99)
    #define GLCD_MAGENTA     (0xFF00FF)
    #define GLCD_CYAN        (0x00FFFF)

    #define GLCD_CUSTOM      (0x7FFF7F)
#endif

// PLCD DISPLAY
#define LCD_HPIX       (1024)
#define LCD_STRIDE     (1024)
#define LCD_VPIX       (600)

#define LCD_COLOR_SIZE (2)
#define LCD_BUF_NUM    (2)

extern d2_device * gp_d2_handle;
extern st_drv_background_t g_current_backgroud;

extern void graphics_init (void);
extern void * graphics_get_draw_buffer(void);
extern void graphics_wait_vsync(void);
extern void display_next_buffer_set(uint8_t* next_buffer);

extern   uint8_t g_sdram_buffer_welcome[];
extern   uint8_t g_sdram_buffer_led[];

extern void dsp_set_background(uint32_t choice);
extern void set_gs_page(void);

#endif /* DISPLAY_THREAD_ENTRY_H_ */
