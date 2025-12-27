/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
 * File Name    : hmi_event_handler.c
 * Description  : Contains event handler for GUIX Thermostat.
 ***********************************************************************************************************************/
 
/***********************************************************************************************************************
 * Includes
 ***********************************************************************************************************************/
#include <guix_gen/thermostat_resources.h>
#include <guix_gen/thermostat_specifications.h>
#include "gx_api.h"


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
    return (gx_err);
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
    FSP_PARAMETER_NOT_USED(*widget);
    FSP_PARAMETER_NOT_USED(*event_ptr);
    UINT gx_err = GX_SUCCESS;

    return (gx_err);
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
    FSP_PARAMETER_NOT_USED(*widget);
    FSP_PARAMETER_NOT_USED(*event_ptr);
    UINT gx_err = GX_SUCCESS;

    return (gx_err);
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
    FSP_PARAMETER_NOT_USED(* p_window);
    FSP_PARAMETER_NOT_USED(*event_ptr);
    UINT gx_err = GX_SUCCESS;

    return (gx_err);
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
    FSP_PARAMETER_NOT_USED(*widget);
    FSP_PARAMETER_NOT_USED(*event_ptr);
    UINT gx_err = GX_SUCCESS;

    return (gx_err);
}


/** @} (end group events) */
