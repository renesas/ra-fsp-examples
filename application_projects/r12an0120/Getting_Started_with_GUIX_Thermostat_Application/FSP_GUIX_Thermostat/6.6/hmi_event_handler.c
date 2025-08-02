/*
 * These are an event handlers for GUIX graphics framework.
 */

/***********************************************************************************************************************
 * Includes
 ***********************************************************************************************************************/
#include "gx_api.h"

#include "guix_gen/thermostat_resources.h"
#include "guix_gen/thermostat_specifications.h"

#include "system_cfg.h"
#include "system_thread.h"
#include "brightness.h"

#include <stdio.h>
#include <math.h>

/***********************************************************************************************************************
Private global variables
***********************************************************************************************************************/
static USHORT cur_settings_screen = ID_SETTINGS_CONTENT_DATE;
static USHORT cur_menu_highlight = ID_SETTINGS_MENUITEM_NONE;

static char g_local_temp_str [5]    = "00.0";
static char g_time_str[12]          = "HH:MM:SS AM";
static char g_bright_str[5]         = "50%";
static char g_date_str[19]          = "May 11, 2021";

static GX_RESOURCE_ID const g_months[12] =
{
    GX_STRING_ID_M0, GX_STRING_ID_M1, GX_STRING_ID_M2, GX_STRING_ID_M3, GX_STRING_ID_M4, GX_STRING_ID_M5,
    GX_STRING_ID_M6, GX_STRING_ID_M7, GX_STRING_ID_M8, GX_STRING_ID_M9, GX_STRING_ID_M10, GX_STRING_ID_M11
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


/***********************************************************************************************************************
Private function prototypes
***********************************************************************************************************************/
static float c_to_f(float temp);
static void update_text(GX_WIDGET * p_widget, GX_RESOURCE_ID id, char * p_text);
static void settings_menu_highlight(GX_WINDOW *widget, USHORT menu_item);
static void toggle_screen(GX_WINDOW *new_win, GX_WINDOW *old_win);
UINT settings_item_show(GX_WINDOW *widget, USHORT new_content, USHORT old_content);
static void update_local_temp_string(void);
static void update_date(GX_WIDGET * p_widget, system_state_t * p_state);
static void update_time(GX_WIDGET *p_widget, system_state_t * p_state);
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
        case GX_EVENT_SHOW:
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
        case GX_SIGNAL(ID_THERMOSTAT_BACK_BUTTON, GX_EVENT_CLICKED):
            /** Returns to main screen. */
            toggle_screen(p_mainpage_screen,p_thermostat_screen);
            break;
        case GX_EVENT_SHOW:
            gx_err = gx_window_event_process(p_window, event_ptr);
            if(GX_SUCCESS != gx_err) {
                while(1);
            }
            break;
        default:
            gx_err = gx_window_event_process(p_window, event_ptr);
            if(GX_SUCCESS != gx_err) {
                while(1);
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
    UINT gx_err = GX_SUCCESS;
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
        case GX_SIGNAL(ID_SETTINGS_MENUITEM_DISPLAY, GX_EVENT_CLICKED):
            settings_menu_highlight(widget,ID_SETTINGS_MENUITEM_DISPLAY);
            settings_item_show (widget, ID_SETTINGS_CONTENT_DISPLAY, cur_settings_screen);
            cur_settings_screen = ID_SETTINGS_CONTENT_DISPLAY;
           break;
        case GX_SIGNAL(ID_BACK_BUTTON, GX_EVENT_CLICKED):
            /** Returns to main screen. */
            toggle_screen (p_mainpage_screen, widget);
            break;
        case GX_EVENT_SHOW:
            gx_err = gx_window_event_process(widget, event_ptr);
            if(GX_SUCCESS != gx_err) {
                while(1);
            }
            break;
        default:
            gx_err = gx_window_event_process(widget, event_ptr);
            if(GX_SUCCESS != gx_err) {
                while(1);
            }
            break;
    }
    return gx_err;
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
            while(1);
        }

    }
    else
    {
        gx_err = gx_widget_show((GX_WIDGET *) new_win);
        if (GX_SUCCESS != gx_err) {
            while(1);
        }

        /** User defined events are routed to the widget that has the current
            focus. Since gx_widget_show does not claim focus we must manually
            claim focus to insure user defined events (e.g temperature updates)
            will be routed to this window properly. */
        gx_err = gx_system_focus_claim(new_win);
        if (GX_SUCCESS != gx_err && GX_NO_CHANGE != gx_err) {
                while(1);
            }

    }
    gx_err = gx_widget_hide((GX_WIDGET *) old_win);
    if (GX_SUCCESS != gx_err) {
            while(1);
        }
}

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
            while(1);
        }

    } else {
        while(1);
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
UINT settings_item_show(GX_WINDOW *widget, USHORT new_content, USHORT old_content) {

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
            while(1);
        }

    gx_err = gx_widget_find((GX_WIDGET *)widget, new_content, 10, &pNewContent);
    if (GX_SUCCESS != gx_err) {
            while(1);
        }

    gx_err = gx_widget_show((GX_WIDGET *)pNewContent);
    if (GX_SUCCESS != gx_err && GX_NO_CHANGE != gx_err) {
            while(1);
        }

    gx_err = gx_widget_hide((GX_WIDGET *)pOldContent);
    if (GX_SUCCESS != gx_err && GX_NO_CHANGE != gx_err) {
            while(1);
        }

    gx_err = gx_widget_front_move(pNewContent, &return_moved);
    if (GX_SUCCESS != gx_err && GX_NO_CHANGE != gx_err) {
            while(1);
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
        while(1);
    }
    gx_err = gx_widget_style_add(pMenuWidget, GX_STYLE_TRANSPARENT);
    if(GX_SUCCESS != gx_err){
        while(1);
    }

    /** Add the new menu highlight based on menu selection. */
    if (ID_SETTINGS_MENUITEM_NONE != new_highlight) {
        gx_err = gx_widget_find((GX_WIDGET *) widget, new_highlight, 10, (GX_WIDGET**) &pMenuWidget);
        if(GX_SUCCESS != gx_err){
            while(1);
        }
        gx_err = gx_widget_style_remove(pMenuWidget, GX_STYLE_TRANSPARENT);
        if(GX_SUCCESS != gx_err){
            while(1);
        }
    }

    cur_menu_highlight = new_highlight;

}
/** @} (end group events) */
