/*
 * These are an event handlers for GUIX graphics framework.
 */

/***********************************************************************************************************************
 * Includes
 ***********************************************************************************************************************/
#include "gx_api.h"

#include "guix_gen/thermostat_resources.h"
#include "guix_gen/thermostat_specifications.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define SPLASH_TIMEOUT                  (160)

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
        case GX_EVENT_TIMER:
            gx_system_timer_stop(widget, 10);
            toggle_screen(p_mainpage_screen,p_splash_screen);
            break;
        case GX_EVENT_SHOW:
            gx_system_timer_start(widget, 10 , SPLASH_TIMEOUT, SPLASH_TIMEOUT);
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
    switch (event_ptr->gx_event_type)
    {
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
/** @} (end group events) */
