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
 *         : 01.01.2025 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <stdarg.h>

#include "r_typedefs.h"
#include "r_ioport.h"
#include "board_hw_cfg.h"
#include "board_cfg_switch.h"
#include "common_init.h"
#include "common_utils.h"
#include "jlink_console.h"

#include "board_i2c_master.h"
#include "tp_thread.h"
#include "tp_thread_entry.h"
#include "touch_GT911.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

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
static void touchpad_get_xy(int32_t * x, int32_t * y, touch_event_t * touch_event, lv_indev_data_t *data);
static void touchpad_read(lv_indev_data_t * data);
static lv_indev_data_t s_tp_data;
static bool_t s_output_enabled = false;
static bool_t s_ts_enabled = false;

bool_t ts_configured(void);
static void tp_thread_enable_ts(void);
static void tp_thread_disable_ts(void);
lv_indev_data_t g_tp_copy_data = {};

st_bounding_box_t hb_menu     = {{  12,  12},{  60,  50}};
st_bounding_box_t hb_cancel   = {{ 742,  12},{ 789,  50}};

st_bounding_box_t menu_cancel = {{ 660,  78},{ 706, 124}}; // oversized hitbox as button is small
st_bounding_box_t kis_demo    = {{ 120, 140},{ 214, 230}};
st_bounding_box_t gs_demo     = {{ 238, 140},{ 328, 230}};
st_bounding_box_t led_demo    = {{ 356, 140},{ 450, 230}};
st_bounding_box_t lcd_demo    = {{ 474, 140},{ 566, 230}};
st_bounding_box_t ns_demo     = {{ 592, 140},{ 680, 230}};

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
    UNUSED_PARAM(enable_tp_read);
    UNUSED_PARAM(active_menu_selection);
    int8_t selection = -1;

    lv_indev_data_t local_tp_data = {};

    lv_point_t local;

    /*
    if (enable_tp_read)
    {
        touchpad_read(&s_tp_data);
    }
    */

    touchpad_get_copy(&local_tp_data);

    if (1 == data->number_of_coordinates)
    {
        local.x = data->point[0].x;
        local.y = data->point[0].y;

        if (!g_overlay_selected)
        {
            /* Process Menu ICON detection */
            if ((local.x > hb_menu.top_left.x) && (local.x < hb_menu.bottom_right.x) &&
               (local.y > hb_menu.top_left.y) && (local.y < hb_menu.bottom_right.y) )
            {
                g_overlay_selected = true;
                selection = LCD_FULL_BG_POPUP_MENU;
            }

            /* Process Cancel ICON detection */
            if (g_current_backgroud.exit_icon_active)
            {
                if ((local.x > hb_cancel.top_left.x) && (local.x < hb_cancel.bottom_right.x) &&
                   (local.y > hb_cancel.top_left.y) && (local.y < hb_cancel.bottom_right.y) )
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
            /* Menu cancel? */
            if ((local.x > menu_cancel.top_left.x) && (local.x < menu_cancel.bottom_right.x) &&
               (local.y > menu_cancel.top_left.y) && (local.y < menu_cancel.bottom_right.y))
            {
                g_overlay_selected = false;
                selection = 'c';
                g_next_menu_selection = -1;

                if (g_show_getting_started_page)
                {
                    selection = '9';
                    g_show_getting_started_page = true;
                }
                else
                {
                    g_show_getting_started_page = false;
                }

//                g_current_backgroud.menu_icon_active = true;
//                g_current_backgroud.exit_icon_active = false;
            }

            /* Kit information? */
            if ((local.x > kis_demo.top_left.x) && (local.x < kis_demo.bottom_right.x) &&
               (local.y > kis_demo.top_left.y) && (local.y < kis_demo.bottom_right.y))
            {
                g_overlay_selected = false;
                selection = '1';
                g_next_menu_selection = -1;
                g_show_getting_started_page = false;
//                g_current_backgroud.menu_icon_active = true;
//                g_current_backgroud.exit_icon_active = true;
            }

            /* Getting started? */
            if ((local.x > gs_demo.top_left.x) && (local.x < gs_demo.bottom_right.x) &&
               (local.y > gs_demo.top_left.y) && (local.y < gs_demo.bottom_right.y))
            {
                g_overlay_selected = false;

                // Special code to re-show getting started
                selection = '9';
                g_next_menu_selection = -1;
                g_show_getting_started_page = true;
//                g_current_backgroud.menu_icon_active = true;
//                g_current_backgroud.exit_icon_active = true;
            }

            /* LED control? */
            if ((local.x > led_demo.top_left.x) && (local.x < led_demo.bottom_right.x) &&
               (local.y > led_demo.top_left.y) && (local.y < led_demo.bottom_right.y))
            {
                g_overlay_selected = false;
                selection = '2';
                g_next_menu_selection = -1;
                g_show_getting_started_page = false;
//                g_current_backgroud.menu_icon_active = true;
//                g_current_backgroud.exit_icon_active = true;
            }

            /* Display? */
            if ((local.x > lcd_demo.top_left.x) && (local.x < lcd_demo.bottom_right.x) &&
               (local.y > lcd_demo.top_left.y) && (local.y < lcd_demo.bottom_right.y))
            {
                g_overlay_selected = false;
                selection = '4';
                g_next_menu_selection = -1;
                g_show_getting_started_page = false;
//                g_current_backgroud.menu_icon_active = true;
//                g_current_backgroud.exit_icon_active = true;
            }

            /* Next steps? */
            if ((local.x > ns_demo.top_left.x) && (local.x < ns_demo.bottom_right.x) &&
               (local.y > ns_demo.top_left.y) && (local.y < ns_demo.bottom_right.y))
            {
                g_overlay_selected = false;
                selection = '5';
                g_next_menu_selection = -1;
                g_show_getting_started_page = false;
//                g_current_backgroud.menu_icon_active = true;
//                g_current_backgroud.exit_icon_active = true;
            }
        }
    }
    return (selection);
}
/**********************************************************************************************************************
 End of function process_menu_detection
 *********************************************************************************************************************/

/* TP Thread entry function */
/**********************************************************************************************************************
 * Function Name: tp_thread_entry
 * Description  : TP Thread entry function
 * Argument     : pvParameters contains TaskHandle_t
 * Return Value : NONE
 *********************************************************************************************************************/
void tp_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);
    g_display_set = false;

    /* Keep Thread in this loop until intial setup has completed */
    while (!jlink_configured() || (!g_initialise_continue))
    {
        vTaskDelay(10);
    }

    /* Wait for system up (from manu_thread_entry) */
    while (!system_up())
    {
        vTaskDelay (100);
    }

    touchpad_init();

    while (1)
    {
        vTaskDelay(10);
        touchpad_read(&s_tp_data);
        if(s_tp_data.number_of_coordinates == 0)
        {
            /* Update sw4 status any time tp isn't in use */
            switch_set_status_msg();
        }
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
    g_display_set = true;

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
    if (true == s_ts_enabled)
    {
        return (true);
    }
    else
    {
        return (false);
    }
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
 * Description  : Initialize your touchpad
 * Return Value : .
 *********************************************************************************************************************/
static void touchpad_init(void)
{
    fsp_err_t err;

    /* Need to initialise the Touch Controller before the LCD, as only a Single Reset line shared between them */
    err = R_ICU_ExternalIrqOpen(&g_external_irq3_ctrl, &g_external_irq3_cfg);
    if (FSP_SUCCESS != err)
    {
        tp_thread_disable_ts();
    }

    /* g_i2c_master_board should have been initialised in main_menu_thread_entry
     * The open here is a backup in case that open was not successful
     */
    if (g_board_i2c_master_ctrl.open == 0)
    {
        err = R_IIC_MASTER_Open(&g_board_i2c_master_ctrl, &g_board_i2c_master_cfg);
        if (FSP_SUCCESS != err)
        {
            tp_thread_disable_ts();
        }
    }

    err = enable_ts(&g_board_i2c_master_ctrl, &g_external_irq3_ctrl);

    if (FSP_SUCCESS != err)
    {
        tp_thread_disable_ts();
    }

    tp_thread_enable_ts();
}
/**********************************************************************************************************************
 End of function touchpad_init
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * Function Name: touchpad_is_pressed
 * Description  : Return true is the touchpad is pressed
 * Return Value : .
 *********************************************************************************************************************/
static bool touchpad_is_pressed(void)
{
    BaseType_t status;
    bool touch_pressed = false;

    status = xSemaphoreTake( g_irq_binary_semaphore, 0 );

    if(pdTRUE == status)
    {
        touch_pressed = true;
    }

    return touch_pressed;
}
/**********************************************************************************************************************
 End of function touchpad_is_pressed
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * Function Name: touchpad_get_xy
 * Description  : Get the x and y coordinates if the touchpad is pressed
 * Arguments    : x
 *              : y
 *              : touch_event
 *              : data
 * Return Value : .
 *********************************************************************************************************************/
static void touchpad_get_xy(int32_t * x, int32_t * y, touch_event_t * touch_event, lv_indev_data_t *data)
{
    fsp_err_t err;
    uint32_t number_of_coordinates;
    uint8_t read_data[7];

    err = rd_sensor_reg16_8(&g_board_i2c_master_ctrl, GT911_REG_READ_COORD_ADDR, read_data);
    if (FSP_SUCCESS != err)
    {
        tp_thread_disable_ts();
    }

    if (BUFFER_READY == (BUFFER_READY & read_data[0]))
    {
        number_of_coordinates = (read_data[0] & NUM_TOUCH_POINTS_MASK);
        if (number_of_coordinates != 0)
        {

            data->number_of_coordinates = number_of_coordinates;
            g_tp_copy_data.number_of_coordinates = number_of_coordinates;

            for (uint8_t i = 0; i < number_of_coordinates; i++)
            {
                err = rd_sensor_multi_reg16_8(&g_board_i2c_master_ctrl, (uint16_t)(GT911_REG_POINT1_X_ADDR + (i * 8)), read_data, 7 );
                if (FSP_SUCCESS != err)
                {
                    tp_thread_disable_ts();
                }

                data->point[i].x = (uint16_t)((read_data[2] << 8) | read_data[1]);
                data->point[i].y = (uint16_t)((read_data[4] << 8) | read_data[3]);

                g_tp_copy_data.point[i].x = data->point[i].x;
                g_tp_copy_data.point[i].y = data->point[i].y;
                g_tp_copy_data.state      = data->state;
            }
            *touch_event = TOUCH_EVENT_DOWN;
        }
        else
        {
            *touch_event = TOUCH_EVENT_UP;
        }

        /* Set status to 0, to wait for next touch event */
        err = wr_sensor_reg16_8(&g_board_i2c_master_ctrl, GT911_REG_READ_COORD_ADDR, 0);

        if (FSP_SUCCESS != err)
        {
            tp_thread_disable_ts();
        }
    }

    if(*touch_event == TOUCH_EVENT_MOVE || *touch_event == TOUCH_EVENT_DOWN)
    {
        // Msp to real world from inverted panel
        (*x) = (int32_t)(data->point[0].x);
        (*y) = (int32_t)(data->point[0].y);
    }
}
/**********************************************************************************************************************
 End of function touchpad_get_xy
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * Function Name: touchpad_read
 * Description  : Will be called by the library to read the touchpad
 * Argument     : data
 * Return Value : .
 *********************************************************************************************************************/
static void touchpad_read(lv_indev_data_t * data)
{
    static int32_t last_x = 0;
    static int32_t last_y = 0;

    static touch_event_t touch_event = TOUCH_EVENT_NONE;

    /*Save the pressed coordinates and the state*/
    if (touchpad_is_pressed())
    {
        R_ICU_ExternalIrqDisable(&g_external_irq3_ctrl);

        R_IIC_MASTER_SlaveAddressSet(&g_board_i2c_master_ctrl, g_gt911_i2c_address, I2C_MASTER_ADDR_MODE_7BIT);

        touchpad_get_xy(&last_x, &last_y, &touch_event, data);

        if ((TOUCH_EVENT_DOWN == touch_event) || (TOUCH_EVENT_MOVE == touch_event))
        {
            data->state = LV_INDEV_STATE_PRESSED;
        }
        else
        {
            data->state = LV_INDEV_STATE_RELEASED;
        }

        wr_sensor_reg16_8(&g_board_i2c_master_ctrl, GT911_REG_READ_COORD_ADDR, 0);

        R_ICU_ExternalIrqEnable(&g_external_irq3_ctrl);
    }
    else
    {
        data->state = LV_INDEV_STATE_RELEASED;
        data->number_of_coordinates = 0;
        memset(&g_tp_copy_data, 0, sizeof(lv_indev_data_t));
        vTaskDelay(1);
    }
    vTaskDelay(1);
}
/**********************************************************************************************************************
 End of function touchpad_read
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * Function Name: touchpad_get_copy
 * Description  : .
 * Argument     : data
 * Return Value : .
 *********************************************************************************************************************/
void touchpad_get_copy(lv_indev_data_t * data)
{
    R_ICU_ExternalIrqDisable(&g_external_irq3_ctrl);

    memcpy(data, &g_tp_copy_data, sizeof(lv_indev_data_t));

    R_ICU_ExternalIrqEnable(&g_external_irq3_ctrl);
}
/**********************************************************************************************************************
 End of function touchpad_get_copy
 *********************************************************************************************************************/
