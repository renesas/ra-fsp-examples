/***********************************************************************************************************************
 * File Name    : hmi_event_handler.c
 * Description  : Contains event handler for GUIX Thermostat.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
************************************************************************************************************************/

/***********************************************************************************************************************
 * Includes
 ***********************************************************************************************************************/
#include "gx_api.h"

#include "guix_gen/thermostat_resources.h"
#include "guix_gen/thermostat_specifications.h"

#include "system_cfg.h"
#include "system_thread.h"

#include <stdio.h>
#include <math.h>

#include "brightness.h"
#include "system_time.h"
#include "system_api.h"

/***********************************************************************************************************************
Private global variables
***********************************************************************************************************************/
static USHORT cur_settings_screen = ID_SETTINGS_CONTENT_DATE;
static USHORT cur_menu_highlight = ID_SETTINGS_MENUITEM_NONE;


static char g_local_temp_str [5]    = "00.0";
static char g_target_temp_str[5]    = "00.0";
static char g_time_str[12]          = "HH:MM:SS AM";
static char g_hour_str[3]           = "HH";
static char g_min_str[3]            = "MM";
static char g_date_str[19]          = "May 11, 2021";
static char g_year_str[12]           = "2021";
static char g_day_str[3]            = "May";
static char g_bright_str[5]         = "50%";

static GX_RESOURCE_ID const g_months[12] =
{
    GX_STRING_ID_M0, GX_STRING_ID_M1, GX_STRING_ID_M2, GX_STRING_ID_M3, GX_STRING_ID_M4, GX_STRING_ID_M5,
    GX_STRING_ID_M6, GX_STRING_ID_M7, GX_STRING_ID_M8, GX_STRING_ID_M9, GX_STRING_ID_M10, GX_STRING_ID_M11
};

static GX_RESOURCE_ID const g_months_short[12] =
{
    GX_STRING_ID_M0_SHORT, GX_STRING_ID_M1_SHORT, GX_STRING_ID_M2_SHORT, GX_STRING_ID_M3_SHORT,
    GX_STRING_ID_M4_SHORT, GX_STRING_ID_M5_SHORT, GX_STRING_ID_M6_SHORT, GX_STRING_ID_M7_SHORT,
    GX_STRING_ID_M8_SHORT, GX_STRING_ID_M9_SHORT, GX_STRING_ID_M10_SHORT, GX_STRING_ID_M11_SHORT
};

/** Stored GUI state. */
system_state_t g_gui_state;

/***********************************************************************************************************************
Public global variables
***********************************************************************************************************************/
extern GX_WINDOW_ROOT * p_root;
extern GX_WINDOW   *p_splash_screen;
extern GX_WINDOW   *p_settings_screen;
extern GX_WINDOW   *p_thermostat_screen;
extern GX_WINDOW   *p_mainpage_screen;
extern GX_WINDOW   *p_help_screen;

/***********************************************************************************************************************
Private function prototypes
***********************************************************************************************************************/
static float c_to_f(float temp);
static void send_message(system_msg_t * p_message);
static void show_hide_widget(GX_WIDGET * p_widget, GX_RESOURCE_ID id, bool show);
static void update_text(GX_WIDGET * p_widget, GX_RESOURCE_ID id, char * p_text);
static void update_text_id(GX_WIDGET * p_widget, GX_RESOURCE_ID id, UINT string_id);
static void update_pixelmap_button_id(GX_WIDGET * p_widget, GX_RESOURCE_ID id, UINT button);
static void update_time_screen(GX_WIDGET * p_widget, system_state_t * p_state);
static void update_date_screen(GX_WIDGET * p_widget, system_state_t * p_state);
static void update_date(GX_WIDGET * p_widget, system_state_t * p_state);
static void update_time(GX_WIDGET *p_widget, system_state_t * p_state);
static UINT settings_item_show(GX_WINDOW *widget, USHORT new_content, USHORT old_content);
static void update_local_temp_string(void);
static void update_target_temp_string(void);
static void settings_menu_highlight(GX_WINDOW *widget, USHORT menu_item);
static void toggle_screen(GX_WINDOW *new_win, GX_WINDOW *old_win);

/*******************************************************************************************************************//**
 * @brief   Handles all events on the splash screen.
 *
 * @param[in]   widget      Pointer to the widget that caused the event
 * @param[in]   event_ptr   Pointer to event that needs handling
 *
 * @retval      GX_SUCCESS
***********************************************************************************************************************/
UINT splashscreen_event(GX_WINDOW *widget, GX_EVENT *event_ptr)
{
    UINT gx_err = GX_SUCCESS;
    switch (event_ptr->gx_event_type)
    {
        case GXEVENT_MSG_TIME_UPDATE:
            update_time ((GX_WIDGET *) widget, &g_gui_state);
            break;
        case GX_EVENT_TIMER:
            gx_system_timer_stop(widget, 10);
            toggle_screen(p_mainpage_screen,p_splash_screen);
            break;
        case GX_EVENT_SHOW:
            gx_system_timer_start(widget, 10 , SPLASH_TIMEOUT, SPLASH_TIMEOUT);
            gx_err = gx_window_event_process(widget, event_ptr);
            if(GX_SUCCESS != gx_err) {
                APP_ERR_TRAP(FSP_ERR_ASSERTION);
            }
            break;
        default:
            gx_err = gx_window_event_process(widget, event_ptr);
            if(GX_SUCCESS != gx_err) {
                APP_ERR_TRAP(FSP_ERR_ASSERTION);
            }
            break;
    }
    return gx_err;
}

/*******************************************************************************************************************//**
 * @brief   Handles all events on the help screen.
 *
 * @param[in]   widget      Pointer to the widget that caused the event
 * @param[in]   event_ptr   Pointer to event that needs handling
 *
 * @retval      GX_SUCCESS
***********************************************************************************************************************/
UINT help_screen_event(GX_WINDOW *widget, GX_EVENT *event_ptr)
{
    UINT gx_err = GX_SUCCESS;
    switch (event_ptr->gx_event_type)
    {
        case GXEVENT_MSG_UPDATE_TEMPERATURE:
            /** Update temperature text. */
            update_local_temp_string();
            update_text((GX_WIDGET *) widget, ID_TEMP_TEXT, g_local_temp_str);
            update_date((GX_WIDGET *) widget, &g_gui_state);
            break;
        case GXEVENT_MSG_TIME_UPDATE:
            update_time ((GX_WIDGET *) widget, &g_gui_state);
            update_date((GX_WIDGET *) widget, &g_gui_state);
            break;
        case GX_SIGNAL(ID_HELP_CLOSE_BUTTON, GX_EVENT_CLICKED):
            /** Hides the help screen and returns to the thermostat screen. */
            toggle_screen(p_thermostat_screen, p_help_screen);
            break;
        case GX_EVENT_SHOW:
            /** Do default window processing first. */
            gx_err = gx_window_event_process(widget, event_ptr);
            if(GX_SUCCESS != gx_err) {
                APP_ERR_TRAP(FSP_ERR_ASSERTION);
            }
            /** Set the initial temperature units. */
            if(SYSTEM_TEMP_UNITS_F == g_gui_state.temp_units) {
                update_text_id((GX_WIDGET *) widget, ID_TEMP_UNIT_TEXT, GX_STRING_ID_FAHRENHEIT);
            }
            else
            {
                update_text_id((GX_WIDGET *) widget, ID_TEMP_UNIT_TEXT, GX_STRING_ID_CELSIUS);
            }
            /** Show/Hide fan icon. */
            show_hide_widget((GX_WIDGET *) widget, ID_FAN_ICON, g_gui_state.fan_on);
            break;
        default:
            gx_err = gx_window_event_process(widget, event_ptr);
            if(GX_SUCCESS != gx_err) {
                APP_ERR_TRAP(FSP_ERR_ASSERTION);
            }
            break;
    }
    return gx_err;
}


/*******************************************************************************************************************//**
 * @brief   Handles all events on the main screen.
 *
 * @param[in]   widget      Pointer to the widget that caused the event
 * @param[in]   event_ptr   Pointer to event that needs handling
 *
 * @retval      GX_SUCCESS
***********************************************************************************************************************/
UINT mainpage_event(GX_WINDOW *widget, GX_EVENT *event_ptr)
{
    UINT gx_err = GX_SUCCESS;
    switch (event_ptr->gx_event_type)
    {
        case GXEVENT_MSG_UPDATE_TEMPERATURE:
            /** Update temperature text. */
            update_local_temp_string();
            update_text((GX_WIDGET *) widget, ID_TEMP_TEXT, g_local_temp_str);
            update_text((GX_WIDGET *) widget, ID_TEMP_TEXT_2, g_local_temp_str);
            break;
        case GXEVENT_MSG_TIME_UPDATE:
            update_time ((GX_WIDGET *) widget, &g_gui_state);
            update_date((GX_WIDGET *) widget, &g_gui_state);
            break;
        case GX_SIGNAL(ID_THERMO_BUTTON, GX_EVENT_CLICKED):
            /** Shows the thermostat control screen. */
            toggle_screen (p_thermostat_screen, p_mainpage_screen);
            break;
        case GX_SIGNAL(ID_SETTINGS_BUTTON, GX_EVENT_CLICKED):
            /** Shows the settings screen and saves which screen the user is currently viewing. */
            toggle_screen (p_settings_screen, widget);
            break;
        case GX_EVENT_SHOW:
            /** Update initial text fields according to system settings before the window shows. */
            /** Do default window processing first. */
            gx_err = gx_window_event_process(widget, event_ptr);
            if(GX_SUCCESS != gx_err) {
                APP_ERR_TRAP(FSP_ERR_ASSERTION);
            }
            /** Update time string on status bar. */
            update_time((GX_WIDGET *)widget, &g_gui_state);

            /** Update date. */
            update_date((GX_WIDGET *) widget, &g_gui_state);

            /** Set the initial temperature units. */
            if(SYSTEM_TEMP_UNITS_F == g_gui_state.temp_units) {
                update_text_id((GX_WIDGET *) widget, ID_TEMP_UNIT_TEXT, GX_STRING_ID_FAHRENHEIT);
                update_text_id((GX_WIDGET *) widget, ID_TEMP_UNIT_TEXT_1, GX_STRING_ID_FAHRENHEIT);
                update_text_id((GX_WIDGET *) widget, ID_TEMP_UNIT_TEXT_2, GX_STRING_ID_FAHRENHEIT);
            } else {
                update_text_id((GX_WIDGET *) widget, ID_TEMP_UNIT_TEXT, GX_STRING_ID_CELSIUS);
                update_text_id((GX_WIDGET *) widget, ID_TEMP_UNIT_TEXT_1, GX_STRING_ID_CELSIUS);
                update_text_id((GX_WIDGET *) widget, ID_TEMP_UNIT_TEXT_2, GX_STRING_ID_CELSIUS);
            }

            /** Update the local temperature string in the status bar. */
            update_local_temp_string();
            update_text((GX_WIDGET *) widget, ID_TEMP_TEXT, g_local_temp_str);
            update_text((GX_WIDGET *) widget, ID_TEMP_TEXT_2, g_local_temp_str);


            /** Set the initial system mode text. */
            switch (g_gui_state.mode)
            {
                case SYSTEM_MODE_OFF:
                    update_text_id((GX_WIDGET *) widget, ID_SYSTEM_MODE_TEXT, GX_STRING_ID_SYSTEM_MODE_OFF);
                    break;
                case SYSTEM_MODE_COOL:
                    update_text_id((GX_WIDGET *) widget, ID_SYSTEM_MODE_TEXT, GX_STRING_ID_SYSTEM_MODE_COOL);
                    break;
                case SYSTEM_MODE_HEAT:
                    update_text_id((GX_WIDGET *) widget, ID_SYSTEM_MODE_TEXT, GX_STRING_ID_SYSTEM_MODE_HEAT);
                    break;
                default:
                    break;
            }

            /** Show/Hide fan icon. */
            show_hide_widget((GX_WIDGET *) widget, ID_FAN_ICON, g_gui_state.fan_on);

            /** Set the initial fan mode text. */
            switch (g_gui_state.fan_mode)
            {
                case SYSTEM_FAN_MODE_AUTO:
                    update_text_id((GX_WIDGET *) widget, ID_FAN_MODE_TEXT, GX_STRING_ID_FAN_MODE_AUTO);
                    break;
                case SYSTEM_FAN_MODE_ON:
                    update_text_id((GX_WIDGET *) widget, ID_FAN_MODE_TEXT, GX_STRING_ID_FAN_MODE_ON);
                    break;
                default:
                    break;
            }

            /** Update "set to" temperature text. */
            if (SYSTEM_TEMP_UNITS_C == g_gui_state.temp_units)
            {
                int integer = (int)floorf(g_gui_state.target_temp);
                int frac    = (int)roundf((g_gui_state.target_temp - (float)integer) * 10.0f);
                snprintf(g_target_temp_str, sizeof(g_target_temp_str), "%2d.%1d", integer, frac);
            }
            else
            {
                float temp_f = c_to_f(g_gui_state.target_temp);
                snprintf(g_target_temp_str, sizeof(g_target_temp_str), "%d", (int) temp_f);
            }
            update_text((GX_WIDGET *) widget, ID_SET_TO_TEXT, g_target_temp_str);
            break;
        default:
            gx_err = gx_window_event_process(widget, event_ptr);
            if(GX_SUCCESS != gx_err) {
                APP_ERR_TRAP(FSP_ERR_ASSERTION);
            }
            break;
    }
    return gx_err;
}

/*******************************************************************************************************************//**
 * @brief   Handles all events on the thermostat screen.
 *
 * @param[in]   widget      Pointer to the widget that caused the event
 * @param[in]   event_ptr   Pointer to event that needs handling
 *
 * @retval      GX_SUCCESS
***********************************************************************************************************************/
UINT thermostat_screen_event(GX_WINDOW * p_window, GX_EVENT *event_ptr)
{
    UINT gx_err = GX_SUCCESS;
    GX_WIDGET * p_widget = (GX_WIDGET *) p_window;
    GX_RESOURCE_ID mode_text = GX_STRING_ID_SYSTEM_MODE_OFF;
    bool system_off_text = true;

    switch (event_ptr->gx_event_type)
    {
        case GXEVENT_MSG_UPDATE_TEMPERATURE:
            /** Update temperature text. */
            update_local_temp_string();
            update_text((GX_WIDGET *) p_widget, ID_TEMP_TEXT, g_local_temp_str);
            update_text((GX_WIDGET *) p_widget, ID_TEMP_TEXT_1, g_local_temp_str);
            break;
        case GXEVENT_MSG_TIME_UPDATE:
            update_time ((GX_WIDGET *) p_widget, &g_gui_state);
            update_date(p_widget, &g_gui_state);
            break;
        case GX_SIGNAL(ID_HELP_BUTTON, GX_EVENT_CLICKED):
            /** Shows help screen. */
            toggle_screen(p_help_screen,p_thermostat_screen);
            break;
        case GX_SIGNAL(ID_THERMOSTAT_BACK_BUTTON, GX_EVENT_CLICKED):
            /** Returns to main screen. */
            toggle_screen(p_mainpage_screen,p_thermostat_screen);
            break;
        case GX_SIGNAL(ID_TEMP_UP, GX_EVENT_CLICKED):
            if (SYSTEM_MODE_OFF != g_gui_state.mode)
            {
                /** Increases target system temperature. */
                g_gui_state.target_temp += TEMP_INC_C;
                if (TEMP_MAX_C < g_gui_state.target_temp)
                {
                    g_gui_state.target_temp = TEMP_MAX_C;
                }
            }
            /** Update "set to" temperature text. */
            if (SYSTEM_TEMP_UNITS_C == g_gui_state.temp_units)
            {
                int integer = (int)floorf(g_gui_state.target_temp);
                int frac    = (int)roundf((g_gui_state.target_temp - (float)integer) * 10.0f);
                if (frac == 10u)
                {
                    frac = 0;
                    integer++;
                }
                snprintf(g_target_temp_str, sizeof(g_target_temp_str), "%2d.%1d", integer, frac);
            }
            else
            {
                float temp_f = c_to_f(g_gui_state.target_temp);
                snprintf(g_target_temp_str, sizeof(g_target_temp_str), "%d", (int) temp_f);
            }
            update_text((GX_WIDGET *) p_widget, ID_SET_TO_TEXT, g_target_temp_str);
            break;
        case GX_SIGNAL(ID_TEMP_DOWN, GX_EVENT_CLICKED):
            if (SYSTEM_MODE_OFF != g_gui_state.mode)
            {
                /** Decreases target system temperature. */
                g_gui_state.target_temp -= TEMP_INC_C;
                if (TEMP_MIN_C > g_gui_state.target_temp)
                {
                    g_gui_state.target_temp = TEMP_MIN_C;
                }
            }
            /** Update "set to" temperature text. */
            if (SYSTEM_TEMP_UNITS_C == g_gui_state.temp_units)
            {
                int integer = (int)floorf(g_gui_state.target_temp);
                int frac    = (int)roundf((g_gui_state.target_temp - (float)integer) * 10.0f);
                if (frac == 10u)
                {
                    frac = 0;
                    integer++;
                }
                snprintf(g_target_temp_str, sizeof(g_target_temp_str), "%2d.%1d", integer, frac);
            }
            else
            {
                float temp_f = c_to_f(g_gui_state.target_temp);
                snprintf(g_target_temp_str, sizeof(g_target_temp_str), "%d", (int) temp_f);
            }
            update_text((GX_WIDGET *) p_widget, ID_SET_TO_TEXT, g_target_temp_str);
            break;
        case GX_SIGNAL(ID_FAN_BUTTON, GX_EVENT_CLICKED):
            /** Toggles fan setting. */
            g_gui_state.fan_mode = (system_fan_mode_t)(!g_gui_state.fan_mode);
            if (SYSTEM_FAN_MODE_AUTO == g_gui_state.fan_mode)
            {
                update_pixelmap_button_id((GX_WIDGET *) p_widget, ID_FAN_BUTTON, GX_PIXELMAP_ID_BLACKBUTTON);
                update_text_id((GX_WIDGET *) p_widget, ID_FAN_MODE_TEXT, GX_STRING_ID_FAN_MODE_AUTO);
                g_gui_state.fan_on = false;
            }
            else
            {
                update_pixelmap_button_id((GX_WIDGET *) p_widget, ID_FAN_BUTTON, GX_PIXELMAP_ID_GREENBUTTON);
                update_text_id((GX_WIDGET *) p_widget, ID_FAN_MODE_TEXT, GX_STRING_ID_FAN_MODE_ON);
                g_gui_state.fan_on = true;
            }
            if (g_gui_state.fan_on)
            {
                show_hide_widget((GX_WIDGET *) p_widget, ID_FAN_ICON, 1);
            }
            else
            {
                show_hide_widget((GX_WIDGET *) p_widget, ID_FAN_ICON, 0);
            }
            break;
        case GX_SIGNAL(ID_SYSTEM_BUTTON, GX_EVENT_CLICKED):
            /** Cycles between modes of system operation. */
             g_gui_state.mode = (system_mode_t)((uint32_t)g_gui_state.mode + 1) % (uint32_t)SYSTEM_MODE_MAX;
            /** Update system mode button. */
            switch (g_gui_state.mode)
            {
                case SYSTEM_MODE_OFF:
                    update_pixelmap_button_id ((GX_WIDGET *) p_widget, ID_SYSTEM_BUTTON, GX_PIXELMAP_ID_BLACKBUTTON);
                    mode_text = GX_STRING_ID_SYSTEM_MODE_OFF;
                    system_off_text = true;
                    break;
                case SYSTEM_MODE_COOL:
                    update_pixelmap_button_id ((GX_WIDGET *) p_widget, ID_SYSTEM_BUTTON, GX_PIXELMAP_ID_BLUEBUTTON);
                    mode_text = GX_STRING_ID_SYSTEM_MODE_COOL;
                    system_off_text = false;
                    break;
                case SYSTEM_MODE_HEAT:
                    update_pixelmap_button_id ((GX_WIDGET *) p_widget, ID_SYSTEM_BUTTON, GX_PIXELMAP_ID_REDBUTTON);
                    mode_text = GX_STRING_ID_SYSTEM_MODE_HEAT;
                    system_off_text = false;
                    break;
                default:
                    break;
            }
            /** Update text fields related to system setting. */
            update_text_id((GX_WIDGET *) p_widget, ID_SYSTEM_MODE_TEXT, mode_text);
            show_hide_widget((GX_WIDGET *) p_widget, ID_THERMO_SYSTEM_OFF, system_off_text);
            show_hide_widget((GX_WIDGET *) p_widget, ID_SET_TO_TEXT, !system_off_text);
            show_hide_widget((GX_WIDGET *) p_widget, ID_TEMP_UNIT_TEXT_2, !system_off_text);
            show_hide_widget((GX_WIDGET *) p_widget, ID_THERMO_TARGET_TEMP_DEGREE_TEXT, !system_off_text);
            break;
        case GX_EVENT_SHOW:
            /** Continue with default window processing. */
            gx_err = gx_window_event_process(p_window, event_ptr);
            if(GX_SUCCESS != gx_err) {
                APP_ERR_TRAP(FSP_ERR_ASSERTION);
            }
            /** Update time string on status bar. */
            update_time(p_widget, &g_gui_state);
            update_date(p_widget, &g_gui_state);

            /** Update target (set to) text. */
            update_target_temp_string();
            update_text((GX_WIDGET *) p_widget, ID_SET_TO_TEXT, g_target_temp_str);

            /** Update local temperature text. */
            update_local_temp_string();
            update_text((GX_WIDGET *) p_widget, ID_TEMP_TEXT, g_local_temp_str);
            update_text((GX_WIDGET *) p_widget, ID_TEMP_TEXT_1, g_local_temp_str);

            /** Set the temperature units according to the current settings. */
            if(SYSTEM_TEMP_UNITS_F == g_gui_state.temp_units) {
                update_text_id((GX_WIDGET *) p_widget, ID_TEMP_UNIT_TEXT, GX_STRING_ID_FAHRENHEIT);
                update_text_id((GX_WIDGET *) p_widget, ID_TEMP_UNIT_TEXT_1, GX_STRING_ID_FAHRENHEIT);
                update_text_id((GX_WIDGET *) p_widget, ID_TEMP_UNIT_TEXT_2, GX_STRING_ID_FAHRENHEIT);
            } else {
               update_text_id((GX_WIDGET *) p_widget, ID_TEMP_UNIT_TEXT, GX_STRING_ID_CELSIUS);
               update_text_id((GX_WIDGET *) p_widget, ID_TEMP_UNIT_TEXT_1, GX_STRING_ID_CELSIUS);
               update_text_id((GX_WIDGET *) p_widget, ID_TEMP_UNIT_TEXT_2, GX_STRING_ID_CELSIUS);
            }

            /** Show hide widgets based on the system mode. */
            show_hide_widget((GX_WIDGET *) p_widget, ID_THERMO_SYSTEM_OFF, (bool) !g_gui_state.mode);
            show_hide_widget((GX_WIDGET *) p_widget, ID_SET_TO_TEXT, (bool) g_gui_state.mode);
            show_hide_widget((GX_WIDGET *) p_widget, ID_TEMP_UNIT_TEXT_2, (bool) g_gui_state.mode );
            show_hide_widget((GX_WIDGET *) p_widget, ID_THERMO_TARGET_TEMP_DEGREE_TEXT, (bool) g_gui_state.mode);

            /** Show/Hide fan icon. */
            show_hide_widget((GX_WIDGET *) p_widget, ID_FAN_ICON, g_gui_state.fan_on);
            break;
        default:
            gx_err = gx_window_event_process(p_window, event_ptr);
            if(GX_SUCCESS != gx_err) {
                APP_ERR_TRAP(FSP_ERR_ASSERTION);
            }
            break;
    }
    return gx_err;
}


/*******************************************************************************************************************//**
 * @brief   Sends messages corresponding to the button touched.
 *
 * @param[in]   widget      Pointer to the widget that caused the event
 * @param[in]   event_ptr   Pointer to event that needs handling
 *
 * @retval      GX_SUCCESS
***********************************************************************************************************************/
UINT settings_screen_event(GX_WINDOW *widget, GX_EVENT *event_ptr)
{
    system_msg_t * p_message;
    system_msg_t sys_message = {0};
    p_message = (system_msg_t*)&sys_message;
    UINT gx_err = GX_SUCCESS;
    UINT period = GX_STRING_ID_PERIOD_AM;
    rtc_time_t time = {0};

    gx_err = gx_window_event_process (widget, event_ptr);
    if (GX_SUCCESS != gx_err) {
        APP_ERR_TRAP(FSP_ERR_ASSERTION);
    }
    switch (event_ptr->gx_event_type)
    {
        case GXEVENT_MSG_UPDATE_TEMPERATURE:
            /** Update temperature text. */
            update_local_temp_string();
            update_text((GX_WIDGET *) widget, ID_TEMP_TEXT, g_local_temp_str);
            break;
        case GXEVENT_MSG_TIME_UPDATE:
            /** Update time and date on status bar. */
            update_time ((GX_WIDGET *) widget, &g_gui_state);
            update_date((GX_WIDGET *) widget, &g_gui_state);
            /** Update time or date screen depending on which screen we're in. */
            switch(cur_settings_screen) {
                case ID_SETTINGS_CONTENT_DATE:
                    update_date_screen((GX_WIDGET *) widget, &g_gui_state);
                    update_date((GX_WIDGET *) widget, &g_gui_state);
                    break;
                case ID_SETTINGS_CONTENT_TIME:
                    update_time_screen((GX_WIDGET *) widget, &g_gui_state);
                    break;
            }
            break;
        case GXEVENT_MSG_AM_PM_TOGGLE:
            /** Update AM/PM hour mode. */
            if (g_gui_state.time.tm_hour < 12)
            {
                period = GX_STRING_ID_PERIOD_AM;
            } else {
                period = GX_STRING_ID_PERIOD_PM;
            }
            update_text_id((GX_WIDGET *) widget, ID_PERIOD_TEXT, period);
            break;
        case GX_SIGNAL(ID_CELSIUS_BUTTON, GX_EVENT_CLICKED):
            /** Update temperature unit. */
            g_gui_state.temp_units = SYSTEM_TEMP_UNITS_C;
            update_pixelmap_button_id((GX_WIDGET *) widget, ID_FAHRENHEIT_BUTTON, GX_PIXELMAP_ID_BLACKBUTTON);
            update_pixelmap_button_id((GX_WIDGET *) widget, ID_CELSIUS_BUTTON, GX_PIXELMAP_ID_GREENBUTTON);
            update_text_id((GX_WIDGET *) widget, ID_TEMP_UNIT_TEXT, GX_STRING_ID_CELSIUS);
            update_local_temp_string();
            update_text((GX_WIDGET *) widget, ID_TEMP_TEXT, g_local_temp_str);
            break;
        case GX_SIGNAL(ID_FAHRENHEIT_BUTTON, GX_EVENT_CLICKED):
            /** Update temperature unit. */
            g_gui_state.temp_units = SYSTEM_TEMP_UNITS_F;
            update_pixelmap_button_id((GX_WIDGET *) widget, ID_FAHRENHEIT_BUTTON, GX_PIXELMAP_ID_GREENBUTTON);
            update_pixelmap_button_id((GX_WIDGET *) widget, ID_CELSIUS_BUTTON, GX_PIXELMAP_ID_BLACKBUTTON);
            update_text_id((GX_WIDGET *) widget, ID_TEMP_UNIT_TEXT, GX_STRING_ID_FAHRENHEIT);
            update_local_temp_string();
            update_text((GX_WIDGET *) widget, ID_TEMP_TEXT, g_local_temp_str);
            break;
        case GX_SIGNAL(ID_BRIGHTNESS_DOWN, GX_EVENT_CLICKED):
            /** Set backlight control's PWM duty cycle. */
            brightness_down(&g_gui_state.brightness);
            /** Update brightness text. */
            snprintf(g_bright_str, sizeof(g_bright_str), "%d%%", g_gui_state.brightness);
            update_text((GX_WIDGET *) widget, ID_BRIGHTNESS_TEXT, g_bright_str);
            break;
        case GX_SIGNAL(ID_BRIGHTNESS_UP, GX_EVENT_CLICKED):
            /** Set backlight control's PWM duty cycle. */
            brightness_up(&g_gui_state.brightness);
            /** Update brightness text. */
            snprintf(g_bright_str, sizeof(g_bright_str), "%d%%", g_gui_state.brightness);
            update_text((GX_WIDGET *) widget, ID_BRIGHTNESS_TEXT, g_bright_str);
            break;
        case GX_SIGNAL(ID_SETTINGS_MENUITEM_THERMOSTAT, GX_EVENT_CLICKED):
            settings_menu_highlight(widget,ID_SETTINGS_MENUITEM_THERMOSTAT);
            settings_item_show (widget, ID_SETTINGS_CONTENT_THERMO, cur_settings_screen);
            cur_settings_screen = ID_SETTINGS_CONTENT_THERMO;
            break;
        case GX_SIGNAL(ID_SETTINGS_MENUITEM_DISPLAY, GX_EVENT_CLICKED):
            settings_menu_highlight(widget,ID_SETTINGS_MENUITEM_DISPLAY);
            settings_item_show (widget, ID_SETTINGS_CONTENT_DISPLAY, cur_settings_screen);
            cur_settings_screen = ID_SETTINGS_CONTENT_DISPLAY;
           break;
        case GX_SIGNAL(ID_SETTINGS_MENUITEM_TIME, GX_EVENT_CLICKED):
            update_time_screen((GX_WIDGET *) widget, &g_gui_state);
            settings_menu_highlight(widget,ID_SETTINGS_MENUITEM_TIME);
            settings_item_show (widget, ID_SETTINGS_CONTENT_TIME, cur_settings_screen);
            cur_settings_screen = ID_SETTINGS_CONTENT_TIME;
            break;
        case GX_SIGNAL(ID_SETTINGS_MENUITEM_DATE, GX_EVENT_CLICKED):
            update_date_screen((GX_WIDGET *) widget, &g_gui_state);
            settings_menu_highlight(widget,ID_SETTINGS_MENUITEM_DATE);
            settings_item_show (widget, ID_SETTINGS_CONTENT_DATE, cur_settings_screen);
            cur_settings_screen = ID_SETTINGS_CONTENT_DATE;
            break;
        case GX_SIGNAL(ID_HOUR_MODE_TOGGLE, GX_EVENT_CLICKED):
            g_gui_state.hour_mode = (system_hour_mode_t)(!g_gui_state.hour_mode);
            break;
        case GX_SIGNAL(ID_AM_PM_TOGGLE, GX_EVENT_CLICKED):

            /** Create message to toggle AM/PM. */
            p_message->msg_id.event_b.class_code = SF_MESSAGE_EVENT_CLASS_TIME;
            p_message->msg_id.event_b.code  = SF_MESSAGE_EVENT_AM_PM_TOGGLE;
            send_message(p_message);
            break;
        case GX_SIGNAL(ID_HOUR_UP, GX_EVENT_CLICKED):
            /** Create message to set hour. */
            p_message->msg_id.event_b.class_code = SF_MESSAGE_EVENT_CLASS_TIME;
            p_message->msg_id.event_b.code  = SF_MESSAGE_EVENT_SET_TIME;
            g_gui_state.time.tm_hour += 1;
            time.tm_hour = +1;
            p_message->msg_payload.time_payload.time = time;
            send_message(p_message);
            break;
        case GX_SIGNAL(ID_HOUR_DOWN, GX_EVENT_CLICKED):
            /** Create message to set hour. */
            p_message->msg_id.event_b.class_code = SF_MESSAGE_EVENT_CLASS_TIME;
            p_message->msg_id.event_b.code  = SF_MESSAGE_EVENT_SET_TIME;
            g_gui_state.time.tm_hour -= 1;
            time.tm_hour = -1;
            p_message->msg_payload.time_payload.time = time;
            send_message(p_message);
            break;
        case GX_SIGNAL(ID_MINUTE_UP, GX_EVENT_CLICKED):
            /** Create message to set minute. */
            p_message->msg_id.event_b.class_code = SF_MESSAGE_EVENT_CLASS_TIME;
            p_message->msg_id.event_b.code  = SF_MESSAGE_EVENT_SET_TIME;
            g_gui_state.time.tm_min += 1;
            time.tm_min = 1;
            p_message->msg_payload.time_payload.time = time;
            send_message(p_message);
            break;
        case GX_SIGNAL(ID_MINUTE_DOWN, GX_EVENT_CLICKED):
            /** Create message to set minute. */
            p_message->msg_id.event_b.class_code = SF_MESSAGE_EVENT_CLASS_TIME;
            p_message->msg_id.event_b.code  = SF_MESSAGE_EVENT_SET_TIME;
            g_gui_state.time.tm_min -= 1;
            time.tm_min = -1;
            p_message->msg_payload.time_payload.time = time;
            send_message(p_message);
            break;
        case GX_SIGNAL(ID_YEAR_UP, GX_EVENT_CLICKED):
            /** Create message to set year. */
            p_message->msg_id.event_b.class_code = SF_MESSAGE_EVENT_CLASS_TIME;
            p_message->msg_id.event_b.code  = SF_MESSAGE_EVENT_SET_DATE;
            g_gui_state.time.tm_year += 1;
            time.tm_year = 1;
            p_message->msg_payload.time_payload.time = time;
            send_message(p_message);
            break;
        case GX_SIGNAL(ID_YEAR_DOWN, GX_EVENT_CLICKED):
            /** Create message to set year. */
            p_message->msg_id.event_b.class_code = SF_MESSAGE_EVENT_CLASS_TIME;
            p_message->msg_id.event_b.code  = SF_MESSAGE_EVENT_SET_DATE;
            g_gui_state.time.tm_year -= 1;
            time.tm_year = -1;
            p_message->msg_payload.time_payload.time = time;
            send_message(p_message);
            break;
        case GX_SIGNAL(ID_MONTH_UP, GX_EVENT_CLICKED):
            /** Create message to set month. */
            p_message->msg_id.event_b.class_code = SF_MESSAGE_EVENT_CLASS_TIME;
            p_message->msg_id.event_b.code  = SF_MESSAGE_EVENT_SET_DATE;
            g_gui_state.time.tm_mon += 1;
            time.tm_mon = 1;
            p_message->msg_payload.time_payload.time = time;
            send_message(p_message);
            break;
        case GX_SIGNAL(ID_MONTH_DOWN, GX_EVENT_CLICKED):
            /** Create message to set month. */
            p_message->msg_id.event_b.class_code = SF_MESSAGE_EVENT_CLASS_TIME;
            p_message->msg_id.event_b.code  = SF_MESSAGE_EVENT_SET_DATE;
            g_gui_state.time.tm_mon -= 1;
            time.tm_mon = -1;
            p_message->msg_payload.time_payload.time = time;
            send_message(p_message);
            break;
        case GX_SIGNAL(ID_DAY_UP, GX_EVENT_CLICKED):
            /** Create message to set date. */
            p_message->msg_id.event_b.class_code = SF_MESSAGE_EVENT_CLASS_TIME;
            p_message->msg_id.event_b.code  = SF_MESSAGE_EVENT_SET_DATE;
            g_gui_state.time.tm_mday += 1;
            time.tm_mday = 1;
            p_message->msg_payload.time_payload.time = time;
            send_message(p_message);
            break;
        case GX_SIGNAL(ID_DAY_DOWN, GX_EVENT_CLICKED):
            /** Create message to set date. */
            p_message->msg_id.event_b.class_code = SF_MESSAGE_EVENT_CLASS_TIME;
            p_message->msg_id.event_b.code  = SF_MESSAGE_EVENT_SET_DATE;
            g_gui_state.time.tm_mday -= 1;
            time.tm_mday = -1;
            p_message->msg_payload.time_payload.time = time;
            send_message(p_message);
            break;
        case GX_SIGNAL(ID_BACK_BUTTON, GX_EVENT_CLICKED):
            /** Returns to main screen. */
            toggle_screen (p_mainpage_screen, widget);
            break;
        case GX_EVENT_SHOW:
            gx_err = gx_window_event_process(widget, event_ptr);
            if(GX_SUCCESS != gx_err) {
                APP_ERR_TRAP(FSP_ERR_ASSERTION);
            }
            settings_menu_highlight(widget,ID_SETTINGS_MENUITEM_NONE);
            settings_item_show (widget, ID_SETTINGS_CONTENT_NONE, cur_settings_screen);
            cur_settings_screen = ID_SETTINGS_CONTENT_NONE;

            /** Update time and date strings on status bar */
            update_time((GX_WIDGET *) widget, &g_gui_state);
            update_date((GX_WIDGET *) widget, &g_gui_state);

            /** Show/Hide fan icon. */
            show_hide_widget((GX_WIDGET *) widget, ID_FAN_ICON, g_gui_state.fan_on);

            if(SYSTEM_TEMP_UNITS_C == g_gui_state.temp_units) {
                update_pixelmap_button_id((GX_WIDGET *) widget, ID_FAHRENHEIT_BUTTON, GX_PIXELMAP_ID_BLACKBUTTON);
                update_pixelmap_button_id((GX_WIDGET *) widget, ID_CELSIUS_BUTTON, GX_PIXELMAP_ID_GREENBUTTON);
            } else {
                update_pixelmap_button_id((GX_WIDGET *) widget, ID_FAHRENHEIT_BUTTON, GX_PIXELMAP_ID_GREENBUTTON);
                update_pixelmap_button_id((GX_WIDGET *) widget, ID_CELSIUS_BUTTON, GX_PIXELMAP_ID_BLACKBUTTON);
            }

            update_local_temp_string();
            update_text((GX_WIDGET *) widget, ID_TEMP_TEXT, g_local_temp_str);

            /** Update brightness text. */
            snprintf(g_bright_str, sizeof(g_bright_str), "%d%%", g_gui_state.brightness);
            update_text((GX_WIDGET *) widget, ID_BRIGHTNESS_TEXT, g_bright_str);
            break;
        default:
            gx_err = gx_window_event_process(widget, event_ptr);
            if(GX_SUCCESS != gx_err) {
                APP_ERR_TRAP(FSP_ERR_ASSERTION);
            }
            break;
    }
    return gx_err;
}

/***********************************************************************************************************************
Private Functions
***********************************************************************************************************************/


/*******************************************************************************************************************//**
 * @brief   Converts a Celsius temperature to a Fahrenheit temperature.
 *
 * @param[in]   temp        Celsius temperature to convert
 *
 * @returns The converted temperature
***********************************************************************************************************************/
static float c_to_f(float temp)
{
    return (temp * 9.0f/5.0f) + 32.0f;
}

/*******************************************************************************************************************//**
 * @brief   Helper function to post message.
 *
 * @param[out]   p_message     Pointer to message to send.
***********************************************************************************************************************/
static void send_message(system_msg_t * p_message)
{
    UINT tx_err = 0;

    tx_err = tx_queue_send((TX_QUEUE*)&system_msg_queue, (VOID*)p_message, TX_NO_WAIT);
    /* Trap here if err*/
    if (TX_SUCCESS != tx_err)
    {
        APP_ERR_TRAP(FSP_ERR_ASSERTION);
    }
}

/*******************************************************************************************************************//**
 * @brief   Helper function to show or hide widgets based on ID.
 *
 * @param[in]   p_widget    Parent widget (typically the current screen)
 * @param[in]   id          Resource ID of the widget to show or hide
 * @param[in]   show        Whether to show (true) or hide (false) the widget
***********************************************************************************************************************/
static void show_hide_widget(GX_WIDGET * p_widget, GX_RESOURCE_ID id, bool show)
{
    GX_WIDGET * p_target_widget;

    UINT err = gx_widget_find(p_widget, (USHORT)id, GX_SEARCH_DEPTH_INFINITE, (GX_WIDGET **) &p_target_widget);
    if (GX_SUCCESS == err)
    {
        if (show)
        {
            err = gx_widget_show(p_target_widget);
            if (GX_SUCCESS != err) {
                APP_ERR_TRAP(FSP_ERR_ASSERTION);
            }

        }
        else
        {
            err = gx_widget_hide(p_target_widget);
            if (GX_SUCCESS != err) {
                APP_ERR_TRAP(FSP_ERR_ASSERTION);
            }

        }
    } else {
        APP_ERR_TRAP(FSP_ERR_ASSERTION);
    }
}

/*******************************************************************************************************************//**
 * @brief   Helper function to update strings based on resource ID.
 *
 * @param[in]   p_widget    Parent widget (typically the current screen)
 * @param[in]   id          Resource ID of the widget to show or hide
 * @param[in]   p_text      Pointer to string to display
***********************************************************************************************************************/
static void update_text(GX_WIDGET * p_widget, GX_RESOURCE_ID id, char * p_text)
{
    GX_PROMPT * p_prompt = NULL;

    UINT err = gx_widget_find(p_widget, (USHORT)id, GX_SEARCH_DEPTH_INFINITE, (GX_WIDGET **) &p_prompt);
    if (GX_SUCCESS == err)
    {
        err = gx_prompt_text_set(p_prompt, p_text);
        if (GX_SUCCESS != err) {
            APP_ERR_TRAP(FSP_ERR_ASSERTION);
        }

    } else {
        APP_ERR_TRAP(FSP_ERR_ASSERTION);
    }
}

/*******************************************************************************************************************//**
 * @brief   Helper function to update strings based on ID.
 *
 * @param[in]   p_widget    Parent widget (typically the current screen)
 * @param[in]   id          Resource ID of the widget to show or hide
 * @param[in]   string_id   ID of string to display
***********************************************************************************************************************/
static void update_text_id(GX_WIDGET * p_widget, GX_RESOURCE_ID id, UINT string_id)
{
    GX_PROMPT * p_prompt = NULL;

    UINT err = gx_widget_find(p_widget, (USHORT)id, GX_SEARCH_DEPTH_INFINITE, (GX_WIDGET **) &p_prompt);
    if (GX_SUCCESS == err)
    {
        err = gx_prompt_text_id_set(p_prompt, string_id);
        if (GX_SUCCESS != err) {
            APP_ERR_TRAP(FSP_ERR_ASSERTION);
        }
    } else {
        APP_ERR_TRAP(FSP_ERR_ASSERTION);
    }
}

/*******************************************************************************************************************//**
 * @brief   Helper function to update pixelmap buttons based on ID.
 *
 * @param[in]   p_widget    Parent widget (typically the current screen)
 * @param[in]   id          Resource ID of the widget to show or hide
 * @param[in]   button      ID of button to display
***********************************************************************************************************************/
static void update_pixelmap_button_id(GX_WIDGET * p_widget, GX_RESOURCE_ID id, UINT button)
{
    GX_PIXELMAP_BUTTON * p_button = NULL;

    UINT err = gx_widget_find(p_widget, (USHORT)id, GX_SEARCH_DEPTH_INFINITE, (GX_WIDGET **) &p_button);
    if (GX_SUCCESS == err)
    {
        err = gx_pixelmap_button_pixelmap_set(p_button, button, button, button);
        if (GX_SUCCESS != err) {
            APP_ERR_TRAP(FSP_ERR_ASSERTION);
        }

    } else {
        APP_ERR_TRAP(FSP_ERR_ASSERTION);
    }
}

/*******************************************************************************************************************//**
 * @brief   Updates the text on the widget to reflect the current time (in the format HH:MM [AM/PM]).
 *
 * @param[in]   p_widget    Pointer to the widget that caused the event
 * @param[in]   p_state     Current state received from system monitor
***********************************************************************************************************************/
static void update_time(GX_WIDGET *p_widget, system_state_t * p_state)
{
    int h = 0;
    int m = 0;
    int s = 0;
    char * p = "";
    h = p_state->time.tm_hour;
    m = p_state->time.tm_min;
    s = p_state->time.tm_sec;

    if (SYSTEM_HOUR_MODE_12 == p_state->hour_mode)
    {
        if (h < 12)
        {
            p = "AM";
        }
        else
        {
            h -= 12;
            p = "PM";
        }

        if (h == 0)
        {
            h = 12;
        }
    }
    snprintf(g_time_str, sizeof(g_time_str), "%02d:%02d:%02d %2s", h, m, s, p);
    update_text(p_widget, ID_TIME_TEXT, g_time_str);
}

/*******************************************************************************************************************//**
 * @brief   Updates the date text on status bar.
 *
 * @param[in]   p_widget    Pointer to the parent widget to search
 * @param[in]   p_state     Current state received from system monitor
***********************************************************************************************************************/
static void update_date(GX_WIDGET *p_widget, system_state_t * p_state)
{
    /** Update date string. */
    char * m;
    gx_system_string_get((GX_RESOURCE_ID)g_months[p_state->time.tm_mon], (GX_CONST GX_CHAR **)&m);
    snprintf(g_date_str, sizeof(g_date_str), "%s %d, %d", m, p_state->time.tm_mday, p_state->time.tm_year + 1900);
    update_text(p_widget, ID_DATE_TEXT, g_date_str);

}

/*******************************************************************************************************************//**
 * @brief   Updates the text on all time and date related widgets.
 *
 * @param[in]   p_widget    Pointer to the parent widget to search
 * @param[in]   p_state     Current state received from system monitor
***********************************************************************************************************************/
static void update_date_screen(GX_WIDGET *p_widget, system_state_t * p_state)
{
    /** Update year text. */
    /** Year is changed to 1900 from 2000, this is because driver will add 100 to it */
    /** Above modification are made w.r.t to c time.h standard */
    snprintf(g_year_str, sizeof(g_year_str), "%d", p_state->time.tm_year + 1900);
    update_text(p_widget, ID_YEAR_TEXT, g_year_str);

    /** Update month text. */
    update_text_id(p_widget, ID_MONTH_TEXT, g_months_short[p_state->time.tm_mon]);

    /** Update day text. */
    snprintf(g_day_str, sizeof(g_day_str), "%d", p_state->time.tm_mday);
    update_text(p_widget, ID_DAY_TEXT, g_day_str);

}

/*******************************************************************************************************************//**
 * @brief   Updates the text on all time and date related widgets.
 *
 * @param[in]   p_widget    Pointer to the parent widget to search
 * @param[in]   p_state     Current state received from system monitor
***********************************************************************************************************************/
static void update_time_screen(GX_WIDGET *p_widget, system_state_t * p_state)
{
    /** Update hour text. */
    char const * fmt = "%02ld";
    int h = p_state->time.tm_hour;

    UINT hour_mode_string = GX_STRING_ID_FORMAT_24;
    if (SYSTEM_HOUR_MODE_12 == p_state->hour_mode)
    {
        hour_mode_string = GX_STRING_ID_FORMAT_12;
        fmt = "%d";
        if (h > 12)
        {
            h -= 12;
        }
        if (h == 0)
        {
            h = 12;
        }
    }
    /** Update hour text */
    snprintf(g_hour_str, sizeof(g_hour_str), fmt, h);
    update_text(p_widget, ID_HOUR_TEXT, g_hour_str);
    update_text_id(p_widget, ID_TIME_FORMAT_TEXT, hour_mode_string);

    /** Update minute text. */
    snprintf(g_min_str, sizeof(g_min_str), "%02d", p_state->time.tm_min);
    update_text(p_widget, ID_MINUTE_TEXT, g_min_str);

    /** Update AM/PM hour mode. */
    UINT period = GX_STRING_ID_PERIOD_PM;
    if (p_state->time.tm_hour < 12)
    {
        period = GX_STRING_ID_PERIOD_AM;
    }
    update_text_id(p_widget, ID_PERIOD_TEXT, period);

    /** Update time string. */
    update_time(p_widget, p_state);
}
/*******************************************************************************************************************//**
 * @brief   Toggles between top level screens defined with .
 *
 * @param[in]   new_win  The pointer to the window to show
 * @param[in]   old_win  The pinter to the old window to hide
 *
 * @retval      GX_SUCCESS
 ***********************************************************************************************************************/
static void toggle_screen(GX_WINDOW *new_win, GX_WINDOW *old_win)
{
    UINT gx_err = GX_SUCCESS;

    if (!new_win->gx_widget_parent)
    {
        gx_err = gx_widget_attach(p_root, (GX_WIDGET *) new_win);
        if (GX_SUCCESS != gx_err) {
            APP_ERR_TRAP(FSP_ERR_ASSERTION);
        }

    }
    else
    {
        gx_err = gx_widget_show((GX_WIDGET *) new_win);
        if (GX_SUCCESS != gx_err) {
            APP_ERR_TRAP(FSP_ERR_ASSERTION);
        }

        /** User defined events are routed to the widget that has the current
            focus. Since gx_widget_show does not claim focus we must manually
            claim focus to insure user defined events (e.g temperature updates)
            will be routed to this window properly. */
        gx_err = gx_system_focus_claim(new_win);
        if (GX_SUCCESS != gx_err && GX_NO_CHANGE != gx_err) {
            APP_ERR_TRAP(FSP_ERR_ASSERTION);
            }

    }
    gx_err = gx_widget_hide((GX_WIDGET *) old_win);
    if (GX_SUCCESS != gx_err) {
        APP_ERR_TRAP(FSP_ERR_ASSERTION);
        }

}


/*******************************************************************************************************************//**
 * @brief   Displays the appropriate settings screen.
 *
 * @param[in]   new_conent  The id of the old settings window being displayed
 * @param[in]   old_content The id of the new settings window to be displayed
 *
 * @retval      GX_SUCCESS
 ***********************************************************************************************************************/
static UINT settings_item_show(GX_WINDOW *widget, USHORT new_content, USHORT old_content) {

    UINT gx_err = GX_SUCCESS;

    GX_WIDGET *pOldContent;
    GX_WIDGET *pNewContent;
    GX_BOOL   return_moved;

    /** If we are already on the selected screen, ignore. */
    if(cur_settings_screen == new_content ){
        return gx_err;
    }

    gx_err = gx_widget_find((GX_WIDGET *)widget, old_content, 10, &pOldContent);
    if (GX_SUCCESS != gx_err) {
        APP_ERR_TRAP(FSP_ERR_ASSERTION);
        }

    gx_err = gx_widget_find((GX_WIDGET *)widget, new_content, 10, &pNewContent);
    if (GX_SUCCESS != gx_err) {
        APP_ERR_TRAP(FSP_ERR_ASSERTION);
        }

    gx_err = gx_widget_show((GX_WIDGET *)pNewContent);
    if (GX_SUCCESS != gx_err && GX_NO_CHANGE != gx_err) {
        APP_ERR_TRAP(FSP_ERR_ASSERTION);
        }

    gx_err = gx_widget_hide((GX_WIDGET *)pOldContent);
    if (GX_SUCCESS != gx_err && GX_NO_CHANGE != gx_err) {
        APP_ERR_TRAP(FSP_ERR_ASSERTION);
        }

    gx_err = gx_widget_front_move(pNewContent, &return_moved);
    if (GX_SUCCESS != gx_err && GX_NO_CHANGE != gx_err) {
        APP_ERR_TRAP(FSP_ERR_ASSERTION);
        }

    return gx_err;

}
/*******************************************************************************************************************//**
 * @brief   Helper function to update local temperature string.
 *
 * @param[in]   void  No arguments to this function
***********************************************************************************************************************/
static void update_local_temp_string(void) {
    if (SYSTEM_TEMP_UNITS_C == g_gui_state.temp_units)
    {
        int integer = (int)floorf(g_gui_state.temp_c);
        int frac    = (int)roundf((g_gui_state.temp_c - (float)integer) * 10.0f);
        snprintf(g_local_temp_str, sizeof(g_local_temp_str), "%2d.%1d", integer, frac);
    }
    else
    {
        int temp_f = (int) c_to_f(g_gui_state.temp_c);
        snprintf(g_local_temp_str, sizeof(g_local_temp_str), "%d", temp_f);
    }

}
/*******************************************************************************************************************//**
 * @brief   Helper function to update target temperature string.
 *
 * @param[in]   void  No arguments to this function
***********************************************************************************************************************/
static void update_target_temp_string(void) {
    /** Update "set to" temperature text. */
    if (SYSTEM_TEMP_UNITS_C == g_gui_state.temp_units)
    {
        int integer = (int)floorf(g_gui_state.target_temp);
        int frac    = (int)roundf((g_gui_state.target_temp - (float)integer) * 10.0f);
        snprintf(g_target_temp_str, sizeof(g_target_temp_str), "%2d.%1d", integer, frac);
    }
    else
    {
        float temp_f = c_to_f(g_gui_state.target_temp);
        snprintf(g_target_temp_str, sizeof(g_target_temp_str), "%d", (int) temp_f);
    }

}
/*******************************************************************************************************************//**
 * @brief   Highlight the appropriate menu selection in the settings screen.
 *
 * @param[in]   widget  Pointer to the window the event occurred in
 * @param[in]   old_highlight The id of the new menu item to highlight
 *
 * @retval      void
 ***********************************************************************************************************************/
static void settings_menu_highlight(GX_WINDOW *widget, USHORT new_highlight) {

    UINT gx_err;
    GX_WIDGET * pMenuWidget;

    /** Remove the current menu highlight. */
    gx_err = gx_widget_find((GX_WIDGET *) widget, cur_menu_highlight, 10, (GX_WIDGET**) &pMenuWidget);
    if(GX_SUCCESS != gx_err){
        APP_ERR_TRAP(FSP_ERR_ASSERTION);
    }
    gx_err = gx_widget_style_add(pMenuWidget, GX_STYLE_TRANSPARENT);
    if(GX_SUCCESS != gx_err){
        APP_ERR_TRAP(FSP_ERR_ASSERTION);
    }

    /** Add the new menu highlight based on menu selection. */
    if (ID_SETTINGS_MENUITEM_NONE != new_highlight) {
        gx_err = gx_widget_find((GX_WIDGET *) widget, new_highlight, 10, (GX_WIDGET**) &pMenuWidget);
        if(GX_SUCCESS != gx_err){
            APP_ERR_TRAP(FSP_ERR_ASSERTION);
        }
        gx_err = gx_widget_style_remove(pMenuWidget, GX_STYLE_TRANSPARENT);
        if(GX_SUCCESS != gx_err){
            APP_ERR_TRAP(FSP_ERR_ASSERTION);
        }
    }

    cur_menu_highlight = new_highlight;

}

/** @} (end group events) */
