/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : tp_thread_entry.h
 * Version      : 1.0
 * Description  : Touch monitoring thread handler.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 20.05.2025 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/

 /**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#ifndef TP_THREAD_ENTRY_H_
#define TP_THREAD_ENTRY_H_

#include "common_utils.h"

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/
typedef struct
{
    lv_point_t top_left;
    lv_point_t bottom_right;
} st_bounding_box_t;

typedef struct
{
    uint16_t x;
    uint16_t y;
    st_bounding_box_t  blinking_box;
    st_bounding_box_t  brightness_box;

    double blinking_tp_angle;          /* Blinking touch point angle */
    double brightness;                 /* Brightness as 0--100 */

    lv_point_t blinking_origin_tp;      /* Blinking origin */
    lv_point_t blinking_destination_tp; /* Blinking touch point */
    lv_point_t brightness_origin_tp;    /* Brightness  */
} st_led_control_block_t;

typedef struct
{
    st_led_control_block_t red;
    st_led_control_block_t green;
    st_led_control_block_t blue;
} st_led_page_t;

/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/
extern d2_device *gp_d2_handle;
extern uint8_t g_gt911_i2c_address;
extern bool_t g_initialise_continue;
extern bool_t g_display_set;

extern bool_t g_show_getting_started_page;
extern st_drv_background_t g_current_backgroud;

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/
extern bool_t ts_thread_enable_output(bool_t state);
extern void ts_show_console(bool_t show_default);
extern bool_t ts_thread_enable_state(void);
extern int8_t process_menu_detection(lv_indev_data_t *data, bool_t enable_tp_read, int8_t active_menu_selection);
extern void touchpad_get_copy(lv_indev_data_t * data);
extern bool_t ts_configured(void);
extern void tp_thread_entry(void *pvParameters);


#endif /* TP_THREAD_ENTRY_H_ */
