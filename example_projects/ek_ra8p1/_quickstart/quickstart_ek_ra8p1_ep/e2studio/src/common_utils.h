/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : common_utils.h
 * Version      : 1.0
 * Description  : Contains macros, data structures and functions used common to the EP
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 20.05.2025 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hal_data.h"
#include "r_typedefs.h"

 /**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#ifndef COMMON_UTILS_H_
#define COMMON_UTILS_H_

/* SEGGER RTT and error related headers */
#define BIT_SHIFT_8  (8u)
#define SIZE_64      (64u)

/* Error condition level to be reported   */
#define LVL_ERR      (1u)

/* Generic null value for resetting variables / configurations */
#define RESET_VALUE  (0x00)

/* PRINT Buffer size used throughout this application */
#define BUFFER_LINE_LENGTH  (1024)

/* Basic Console control codes  */
#define CONNECTION_ABORT_CRTL          (0x00)
#define MENU_EXIT_CRTL                 (0x20)
#define MENU_ENTER_RESPONSE_CRTL       (0x09)
#define INPUT_BUFFER                   (0x05)
#define CARRAGE_RETURN                 (0x0D)
#define BACK_SPACE                     (0x08)
#define BACK_SPACE_PUTTY               (0x7F)

#define CONNECTION_ABORT_CRTL  (0x00)
#define MENU_EXIT_CRTL         (0x20)

/* Formating on Display text screens */
#define TITLE_LEFT_EDGE        (80)
#define TITLE_LINE_SPACING     (46)
#define PAGE_LEFT_EDGE         (28)
#define PAGE_1ST_TAB           (PAGE_LEFT_EDGE + 395)
#define PAGE_2ST_TAB           (PAGE_LEFT_EDGE + 695)

/* MACRO Helper to determine vertical position on LCD for text */
#define LINE(l)                (68 + ((l) * 32))

/* MACRO Helper to determine horizontal position on LCD for text */
#define BLOCK(b)               ((b) * 10)

/* Graphics pages */
#define IMAGE_WELCOME          (0)
#define IMAGE_KIT_INFORMATION  (1)
#define IMAGE_LED_DEMO         (2)
#define IMAGE_GETTING_STARTED  (3)
#define IMAGE_ERASE_BACKGROUND (9999)

/* Version Revision History */
#define VERSION_STR            ("Version 0.1.")

#define DRAW_TITLE_FONT_UNDERLINE \
    (user_font_title_draw_line(/* d2_device *d2_handle, */ &cursor_pos, "_________________________________________"))

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/

/** States for input devices*/
typedef enum {
    LV_INDEV_STATE_RELEASED = 0,
    LV_INDEV_STATE_PRESSED
} lv_indev_state_t;

/* Represents a point on the screen. */
typedef struct {
    uint32_t x;
    uint32_t y;
} lv_point_t;

/* Data structure passed to an input driver to fill */
typedef struct {
    lv_point_t point[5];                /**< For LV_INDEV_TYPE_POINTER the currently pressed point*/
    uint32_t   number_of_coordinates;   /**<  */

    lv_indev_state_t state;             /**< LV_INDEV_STATE_RELEASED or LV_INDEV_STATE_PRESSED*/
    bool             continue_reading;  /**< If set to true, the read callback is invoked again,
                                             unless the device is in event-driven mode*/
} lv_indev_data_t;

/* Data structure to represent line of text on LCD */
typedef struct {
    lv_point_t offset;
    char_t     line[128];
} st_text_block_t;

/* Data structure represent text page on LCD */
typedef struct {
    uint32_t          text_lines;
    char_t            title[128];
    st_text_block_t   text_block[24];
} st_rgb565_text_block_page_t;

typedef struct {
    int8_t active_demo;
    bool_t update;
    bool_t menu_icon_active;
    bool_t exit_icon_active;
    st_rgb565_text_block_page_t *p_text_block;
    uint8_t * p_bg_src;
} st_drv_background_t;

/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/
extern const char_t * const gp_cursor_store;
extern const char_t * const gp_cursor_restore;
extern const char_t * const gp_cursor_temp;
extern const char_t * const gp_cursor_frequency;
extern const char_t * const gp_cursor_intensity;

extern const char_t * const gp_red_fg;
extern const char_t * const gp_orange_fg;
extern const char_t * const gp_green_fg;
extern const char_t * const gp_white_fg;

extern const char_t * const gp_clear_screen;
extern const char_t * const gp_cursor_home;

extern d2_device *gp_d2_handle;

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/
extern bool_t tolowerstr(char_t *ptr, uint32_t len);
extern bool_t toupperstr(char_t *ptr, uint32_t len);
extern bool_t set_display_image_full(uint32_t image);
extern bool_t check_ov5640_camera_connected(void);

#endif /* COMMON_UTILS_H_ */
