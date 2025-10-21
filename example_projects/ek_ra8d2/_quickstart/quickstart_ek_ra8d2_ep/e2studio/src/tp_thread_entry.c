/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : tp_thread_entry.c
 * Version      : 1.0
 * Description  : tp_thread_entry() for user defined thread.
 *                This file also contains support functions for handling the touch pannel, eg menu processing
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 20.10.2025 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <stdarg.h>

#include "r_typedefs.h"
#include "r_ioport.h"
#include "board_hw_cfg.h"
#include "common_init.h"
#include "common_utils.h"
#include "jlink_console.h"

#include "tp_thread_entry.h"
#include "display_thread_entry.h"

#include "board_i2c_master.h"

#include "board_cfg_switch.h"
#include "touch_FT5316.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define R_BYTE_UNINITALIZED     (0xFF)

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
static void touchpad_init(void);
static bool touchpad_is_pressed(void);
static void touchpad_get_xy(touch_event_t * touch_event, lv_indev_data_t *data);
static void touchpad_read(lv_indev_data_t * tp_data);
void draw_hitboxes(void);
void lcd_connection_status(e_display_status_t * set);

static lv_indev_data_t s_tp_data;
static lv_indev_data_t g_tp_copy_data;

static bool_t s_output_enabled = false;
static bool_t s_ts_enabled = false;
bool_t g_display_set = true;     // is this needed?

st_bounding_box_t hb_menu     = {{  12,  12}, {  60,  50}};
st_bounding_box_t hb_cancel   = {{ 970,  12}, { 1020,  50}};

st_bounding_box_t menu_cancel = {{ 840,  78}, { 880, 124}}; // over sized hit box as button is small
st_bounding_box_t kis_demo    = {{ 164, 180}, { 280, 305}};
st_bounding_box_t gs_demo     = {{ 314, 180}, { 430, 305}};
st_bounding_box_t led_demo    = {{ 454, 180}, { 570, 305}};
st_bounding_box_t lcd_demo    = {{ 598, 180}, { 712, 305}};
st_bounding_box_t ns_demo     = {{ 742, 180}, { 856, 305}};

static e_display_status_t s_detected_status = DISP_CONNECTION_NONE;

void lcd_connection_status(e_display_status_t * set)
{
    *set = s_detected_status;
}


void draw_hitboxes(void)
{
    if(false == g_overlay_selected)
    {
        if (g_current_backgroud.menu_icon_active)
        {
            d2_setcolor(gp_d2_handle, 0, 0x00FF0000);
            d2_renderbox( gp_d2_handle, (d2_point)hb_menu.top_left.x << 4, (d2_point)hb_menu.top_left.y << 4,
                (d2_point)(hb_menu.bottom_right.x - hb_menu.top_left.x) << 4, (d2_point)(hb_menu.bottom_right.y - hb_menu.top_left.y) << 4);
        }

        if (g_current_backgroud.exit_icon_active)
        {
            d2_setcolor(gp_d2_handle, 0, 0x0000FF00);
            d2_renderbox( gp_d2_handle, (d2_point)hb_cancel.top_left.x << 4, (d2_point)hb_cancel.top_left.y << 4,
                    (d2_point)(hb_cancel.bottom_right.x - hb_cancel.top_left.x) << 4, (d2_point)(hb_cancel.bottom_right.y - hb_cancel.top_left.y) << 4);
        }
    }
    else
    {
        d2_setcolor(gp_d2_handle, 0, 0x0000FF00);
        d2_renderbox( gp_d2_handle, (d2_point)menu_cancel.top_left.x << 4, (d2_point)menu_cancel.top_left.y << 4,
                (d2_point)(menu_cancel.bottom_right.x - menu_cancel.top_left.x) << 4, (d2_point)(menu_cancel.bottom_right.y - menu_cancel.top_left.y) << 4);

        d2_setcolor(gp_d2_handle, 0, 0x00A02020);
        d2_renderbox( gp_d2_handle, (d2_point)kis_demo.top_left.x << 4, (d2_point)kis_demo.top_left.y << 4,
                (d2_point)(kis_demo.bottom_right.x - kis_demo.top_left.x) << 4, (d2_point)(kis_demo.bottom_right.y - kis_demo.top_left.y) << 4);

        d2_setcolor(gp_d2_handle, 0, 0x00B04040);
        d2_renderbox( gp_d2_handle, (d2_point)gs_demo.top_left.x << 4, (d2_point)gs_demo.top_left.y << 4,
                (d2_point)(gs_demo.bottom_right.x - gs_demo.top_left.x) << 4, (d2_point)(gs_demo.bottom_right.y - gs_demo.top_left.y) << 4);

        d2_setcolor(gp_d2_handle, 0, 0x00C06060);
        d2_renderbox( gp_d2_handle, (d2_point)led_demo.top_left.x << 4, (d2_point)led_demo.top_left.y << 4,
                (d2_point)(led_demo.bottom_right.x - led_demo.top_left.x) << 4, (d2_point)(led_demo.bottom_right.y - led_demo.top_left.y) << 4);

        d2_setcolor(gp_d2_handle, 0, 0x00D08080);
        d2_renderbox( gp_d2_handle, (d2_point)lcd_demo.top_left.x << 4, (d2_point)lcd_demo.top_left.y << 4,
                (d2_point)(lcd_demo.bottom_right.x - lcd_demo.top_left.x) << 4, (d2_point)(lcd_demo.bottom_right.y - lcd_demo.top_left.y) << 4);

        d2_setcolor(gp_d2_handle, 0, 0x00E0A0A0);
        d2_renderbox( gp_d2_handle, (d2_point)ns_demo.top_left.x << 4, (d2_point)ns_demo.top_left.y << 4,
                (d2_point)(ns_demo.bottom_right.x - ns_demo.top_left.x) << 4, (d2_point)(ns_demo.bottom_right.y - ns_demo.top_left.y) << 4);
    }
}

/**********************************************************************************************************************
 * Function Name: process_menu_detection
 * Description  : Determines which hitbox (activation zone defined by the st_bounding_box_t structure) conatins supplied 
 *                touch point.
 *                Makes required  modification to g_overlay_selected to prepare demo for a change of activity (demo).  
 * Arguments    : data                  - touch point (only 1 required), 
 *              : enable_tp_read        - re-read touch controller when TRUE.
 *              : active_menu_selection - Current activity (demo) used to determine if a transition to new activity 
 *                                        is required. Don't want to re-draw current activity if its allready active.     
 * Return Value : selection             - control code required matching selected activity.
 *********************************************************************************************************************/
int8_t process_menu_detection (lv_indev_data_t *data, bool_t enable_tp_read, int8_t active_menu_selection)
{
    FSP_PARAMETER_NOT_USED(enable_tp_read);
    int8_t selection = -1;
    lv_indev_data_t local_tp_data;
    lv_point_t local;

    touchpad_get_copy(&local_tp_data);

    if (1 == data->number_of_coordinates)
    {
        local.x = data->point[0].x;
        local.y = data->point[0].y;

        if (!g_overlay_selected)
        {
            /* Process Menu ICON detection */
            if ((local.x > hb_menu.top_left.x) && (local.x < hb_menu.bottom_right.x) &&
               (local.y > hb_menu.top_left.y) && (local.y < hb_menu.bottom_right.y))
            {
                g_overlay_selected = true;
                selection = LCD_FULL_BG_POPUP_MENU;
            }

            /* Process Cancel ICON detection */
            if (g_current_backgroud.exit_icon_active)
            {
                if ((local.x > hb_cancel.top_left.x) && (local.x < hb_cancel.bottom_right.x) &&
                   (local.y > hb_cancel.top_left.y) && (local.y < hb_cancel.bottom_right.y))
                {
                    g_overlay_selected = false;
                    selection = ' ';
                    g_current_backgroud.exit_icon_active = false;

                    g_next_menu_selection = -1;

                    if (g_show_getting_started_page)
                    {
                        selection = 'z';
                    }

                    g_show_getting_started_page = false;
                }
            }
        }
        else
        {
            /* menu_cancel */
            if ((local.x > menu_cancel.top_left.x) && (local.x < menu_cancel.bottom_right.x) &&
               (local.y > menu_cancel.top_left.y) && (local.y < menu_cancel.bottom_right.y))
            {
                g_overlay_selected = false;
                selection = 'c';
                g_next_menu_selection = -1;

                if (g_show_getting_started_page)
                {
                    selection = '9';
                }

                g_show_getting_started_page = false;
            }

            if ((local.x > kis_demo.top_left.x) && (local.x < kis_demo.bottom_right.x) &&
               (local.y > kis_demo.top_left.y) && (local.y < kis_demo.bottom_right.y))
            {
                g_overlay_selected = false;
                selection = '1';
                if('1' == active_menu_selection)
                {
                    /* User is selecting current demo, in effect cancelling popup menu */
                    selection = 'c';
                }
                g_next_menu_selection = -1;
                g_show_getting_started_page = false;
            }

            if ((local.x > gs_demo.top_left.x) && (local.x < gs_demo.bottom_right.x) &&
               (local.y > gs_demo.top_left.y) && (local.y < gs_demo.bottom_right.y))
            {
                g_overlay_selected = false;

                // Special code to re-show getting started
                selection = '9';
                if('0' == active_menu_selection)
                {
                    /* User is selecting current demo, in effect cancelling popup menu */
                    selection = '9';
                }

                g_next_menu_selection = -1;
                g_show_getting_started_page = true;
            }

            if ((local.x > led_demo.top_left.x) && (local.x < led_demo.bottom_right.x) &&
               (local.y > led_demo.top_left.y) && (local.y < led_demo.bottom_right.y))
            {
                g_overlay_selected = false;
                selection = '2';
                if('2' == active_menu_selection)
                {
                    /* User is selecting current demo, in effect cancelling popup menu */
                    selection = 'c';
                }

                g_next_menu_selection = -1;
                g_show_getting_started_page = false;
            }

            if ((local.x > lcd_demo.top_left.x) && (local.x < lcd_demo.bottom_right.x) &&
               (local.y > lcd_demo.top_left.y) && (local.y < lcd_demo.bottom_right.y))
            {
                g_overlay_selected = false;
                selection = '4';
                if('4' == active_menu_selection)
                {
                    /* User is selecting current demo, in effect cancelling popup menu */
                    selection = 'c';
                }

                g_next_menu_selection = -1;
                g_show_getting_started_page = false;
                memset(&g_tp_copy_data, 0, sizeof(lv_indev_data_t));
            }

            if ((local.x > ns_demo.top_left.x) && (local.x < ns_demo.bottom_right.x) &&
               (local.y > ns_demo.top_left.y) && (local.y < ns_demo.bottom_right.y))
            {
                g_overlay_selected = false;
                selection = '5';
                if('5' == active_menu_selection)
                {
                    /* User is selecting current demo, in effect cancelling popup menu */
                    selection = 'c';
                }

                g_next_menu_selection = -1;
                g_show_getting_started_page = false;
            }

        }
    }

    return (selection);
}
/**********************************************************************************************************************
 End of function process_menu_detection
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: tp_thread_entry
 * Description  : Touch Panel Thread entry function
 * Argument     : pvParameters contains TaskHandle_t
 * Return Value : NONE
 *********************************************************************************************************************/
void tp_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);

    uint8_t tp_detected = R_BYTE_UNINITALIZED;

    /* Wait for system up (from manu_thread_entry) */
    while (!system_up())
    {
        vTaskDelay (100);
    }

    check_ov5640_camera_connected();

    touch_panel_reset();
    touchpad_init();

    /* Check FT5316 is connected */
    tp_detected = read_ft5316_tp_status();

    if (tp_detected != R_BYTE_UNINITALIZED)
    {
        s_detected_status = DISP_CONNECTION_PLCD;
    }

    touch_panel_configure();

    while (1)
    {
        vTaskDelay(10);
        touchpad_read(&s_tp_data);
        //switch_set_status_msg();      // WTF?
    }
}
/**********************************************************************************************************************
 End of function tp_thread_entry
 *********************************************************************************************************************/
 
/**********************************************************************************************************************
 * Function Name: tp_thread_enable_ts
 * Description  : Helper function to indicate system wide that the touch controller is configured and available.
 * Return Value : NONE
 *********************************************************************************************************************/
static void tp_thread_enable_ts(void)
{
    s_ts_enabled = true;
    g_display_set = true;     // is this needed? TODO
}
/**********************************************************************************************************************
 End of function tp_thread_enable_ts
 *********************************************************************************************************************/
 
/**********************************************************************************************************************
 * Function Name: tp_thread_disable_ts
 * Description  : Helper function to indicate system wide that the touch controller is not available.
 *                Function blocks here so should only be called from the TP Thread.
 * Return Value : NONE
 *********************************************************************************************************************/
static void tp_thread_disable_ts(void)
{
    s_ts_enabled = false;
    g_display_set = true;     // is this needed? TODO

    while (1)
    {
        vTaskDelay(1000);
    }
}
/**********************************************************************************************************************
 End of function tp_thread_disable_ts
 *********************************************************************************************************************/
 
/**********************************************************************************************************************
 * Function Name: ts_configured
 * Description  : Helper function to inform to the wider system the status of the touch controller
 * Return Value : Touch controller status
 *                TRUE  - Touch controller is configured and available.
 *                FALSE - Touch controller is not available.
 *********************************************************************************************************************/
bool_t ts_configured(void)
{
    /* Block until state can be determined */
    while (!g_display_set)
    {
        vTaskDelay(100);
    }

    return (s_ts_enabled);
}
/**********************************************************************************************************************
 End of function ts_configured
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: ts_thread_enable_output
 * Description  : Helper function sets requires DISPLAY_FRAME_LAYER_2
 * Argument     : state - TRUE / FALSE.
 * Return Value : NONE
 *********************************************************************************************************************/
bool_t ts_thread_enable_output( bool_t state)
{
    s_output_enabled = state;

    return (true);
}
/**********************************************************************************************************************
 End of function ts_thread_enable_output
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: touchpad_init
 * Description  : Initialize touchpad driver.
 * Return Value : NONE
 *********************************************************************************************************************/
static void touchpad_init(void)
{
    fsp_err_t err;

    /* Need to initialise the Touch Controller before the LCD, as only a Single Reset line shared between them */
    err = R_ICU_ExternalIrqOpen(&g_external_irq19_ctrl, &g_external_irq19_cfg);

    if (FSP_SUCCESS != err)
    {
        tp_thread_disable_ts();
    }

    if (g_board_i2c_master_ctrl.open == 0)
    {
        err = R_IIC_MASTER_Open(&g_board_i2c_master_ctrl, &g_board_i2c_master_cfg);

        if (FSP_SUCCESS != err)
        {
            tp_thread_disable_ts();
        }
    }

    uint8_t status = read_ft5316_tp_status();

    if (R_BYTE_UNINITALIZED == status)
    {
        tp_thread_disable_ts();
    }

    tp_thread_enable_ts();

    R_ICU_ExternalIrqEnable(&g_external_irq19_ctrl);
}
/**********************************************************************************************************************
 End of function touchpad_init
 *********************************************************************************************************************/
 
/**********************************************************************************************************************
 * Function Name: touchpad_is_pressed
 * Description  : Return true is the touchpad is pressed
 * Return Value : NONE
 *********************************************************************************************************************/
static bool touchpad_is_pressed(void)
{
    BaseType_t status;
    bool touch_pressed = false;
    static bool was_pressed = false;

    status = xSemaphoreTake( g_irq_binary_semaphore, 0 );

    if (pdTRUE == status)
    {
        touch_pressed = true;

#ifdef ENABLE_UI_TS_FEEDBACK
        volatile bsp_io_level_t level = BSP_IO_LEVEL_HIGH;
        R_BSP_PinWrite(s_ui_ts_feedback_pin, level);
#endif /* ENABLE_UI_TS_FEEDBACK */
    }

    /* kludge to handle low interrupt rate of FT5316 until we find out how to improve the interrupt rate */
    if (was_pressed && !touch_pressed)
    {
        /* send I2C command to read touch points */
        if (read_ft5316_tp_status() > 0)
        {
            touch_pressed = true;
        }
    }

    was_pressed = touch_pressed;

    return (touch_pressed);
}
/**********************************************************************************************************************
 End of function touchpad_is_pressed
 *********************************************************************************************************************/
 
/**********************************************************************************************************************
 * Function Name: touchpad_get_xy
 * Description  : Get active state of the tp interface
 * Arguments    : touch_event  - active event see touch_event_t
 *              : data         - updated current sate of any touch points detected see lv_indev_data_t
 * Return Value : NONE
 *********************************************************************************************************************/
static void touchpad_get_xy(touch_event_t * touch_event, lv_indev_data_t *data)
{
    fsp_err_t err;
    uint8_t tp_detected;
    uint8_t reg_val_x_lsb;
    uint8_t reg_val_y_lsb;
    uint8_t reg_val_x_msb;
    uint8_t reg_val_y_msb;
    uint16_t reg_val_x;
    uint16_t reg_val_y;
    memset(data->point, 0, sizeof(data->point));

    /* read the current number of touch points */
    R_IIC_MASTER_SlaveAddressSet (&g_board_i2c_master_ctrl, g_ft5316_i2c_address, I2C_MASTER_ADDR_MODE_7BIT);
    err = read_reg8 (TD_STATUS, &tp_detected, &g_board_i2c_master_ctrl, 1);

    if (FSP_SUCCESS == err)
    {
        data->number_of_coordinates = tp_detected;

        if (tp_detected > 0)
        {
            /* read coordinates of touch points for all that are detected */
            for (uint8_t i = 0; i < tp_detected; i++)
            {
                read_reg8 ((uint8_t) (TOUCHn_XL + 6 * (i)), &reg_val_x_lsb, &g_board_i2c_master_ctrl, 1);
                read_reg8 ((uint8_t) (TOUCHn_YL + 6 * (i)), &reg_val_y_lsb, &g_board_i2c_master_ctrl, 1);
                read_reg8 ((uint8_t) (TOUCHn_XH + 6 * (i)), &reg_val_x_msb, &g_board_i2c_master_ctrl, 1);
                read_reg8 ((uint8_t) (TOUCHn_YH + 6 * (i)), &reg_val_y_msb, &g_board_i2c_master_ctrl, 1);

                /* extract touch point MSB and merge with LSB */
                reg_val_x = ((reg_val_x_msb & 0xF) << 8) | reg_val_x_lsb;
                reg_val_y = ((reg_val_y_msb & 0xF) << 8) | reg_val_y_lsb;

                data->point[i].x = reg_val_x;
                data->point[i].y = reg_val_y;
            }

            *touch_event = TOUCH_EVENT_DOWN;
        }
        else
        {
            *touch_event = TOUCH_EVENT_UP;
        }

        if (FSP_SUCCESS != err)
        {
            tp_thread_disable_ts();
        }
    }
}
/**********************************************************************************************************************
 End of function touchpad_get_xy
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * Function Name: touchpad_read
 * Description  : Will be called by the library to read the touchpad
 *              : data         - updated current sate of any touch points detected see lv_indev_data_t
 * Return Value : NONE
 *********************************************************************************************************************/
static void touchpad_read(lv_indev_data_t * tp_data)
{
    lv_indev_data_t data;

    static touch_event_t touch_event = TOUCH_EVENT_NONE;

    /* Save the pressed coordinates and the state */
    if (touchpad_is_pressed())
    {
        touchpad_get_xy(&touch_event, &data);

        if ((TOUCH_EVENT_DOWN == touch_event) || (TOUCH_EVENT_MOVE == touch_event))
        {
            data.state = LV_INDEV_STATE_PRESSED;
        }
        else
        {
            data.state = LV_INDEV_STATE_RELEASED;
        }
    }
    else
    {
        data.number_of_coordinates = 0;
        vTaskDelay(1);
    }

    /* update the touch pad data atomically */
    taskENTER_CRITICAL();
    memcpy(tp_data, &data, sizeof(lv_indev_data_t));
    memcpy(&g_tp_copy_data, &data, sizeof(lv_indev_data_t));
    taskEXIT_CRITICAL();

    vTaskDelay(1);
}
/**********************************************************************************************************************
 End of function touchpad_read
 *********************************************************************************************************************/
 
/**********************************************************************************************************************
 * Function Name: touchpad_get_copy
 * Description  : Helper function to get a copy of the touch data.
 * Argument     : data         - updated current sate of any touch points detected see lv_indev_data_t
 * Return Value : NONE
 *********************************************************************************************************************/
void touchpad_get_copy(lv_indev_data_t * data)
{
    taskENTER_CRITICAL();
    memcpy(data, &g_tp_copy_data, sizeof(lv_indev_data_t));
    taskEXIT_CRITICAL();
}
/**********************************************************************************************************************
 End of function touchpad_get_copy
 *********************************************************************************************************************/
