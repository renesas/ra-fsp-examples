/*
 * These are an event handlers for GUIX graphics framework.
 */

/***********************************************************************************************************************
 * Includes
 ***********************************************************************************************************************/
#include "gx_api.h"

#include "guix_gen/thermostat_resources.h"
#include "guix_gen/thermostat_specifications.h"


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

    return gx_err;
}


/** @} (end group events) */
