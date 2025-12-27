/***********************************************************************************************************************
 * File Name    : windows_handler.c
 * Description  : Contains event handler for GUIX Hellow World.
 ***********************************************************************************************************************/
/*
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
 * Includes
 ***********************************************************************************************************************/
#include "gx_api.h"

#include "guix_gen/Hello_World_resources.h"
#include "guix_gen/Hello_World_specifications.h"

#include "system_thread.h"
#include <stdio.h>
#include <math.h>
#include "touch_thread.h"


#define APP_ERR_TRAP(err)           if(err) { __asm("BKPT #0\n");} /* trap upon the error  */

/***********************************************************************************************************************
Public global variables
***********************************************************************************************************************/
extern GX_WINDOW_ROOT * p_root;
extern GX_WINDOW   *p_window1_screen;
extern GX_WINDOW   *p_window2_screen;


static void toggle_screen(GX_WINDOW *new_win, GX_WINDOW *old_win);
static void update_text_id(GX_WIDGET * p_widget, GX_RESOURCE_ID id, UINT string_id);
static bool button_enabled = false;


/*******************************************************************************************************************//**
 * @brief   Handles all events on the main screen.
 *
 * @param[in]   widget      Pointer to the widget that caused the event
 * @param[in]   event_ptr   Pointer to event that needs handling
 *
 * @retval      GX_SUCCESS
***********************************************************************************************************************/


UINT window1_screen_event(GX_WINDOW *widget, GX_EVENT *event_ptr)
{
    UINT gx_err = GX_SUCCESS;
    switch (event_ptr->gx_event_type)
    {
        case GX_SIGNAL(ID_BUTTONENABLER, GX_EVENT_TOGGLE_OFF):

            button_enabled = false;

            update_text_id((GX_WIDGET *) widget, ID_WINDOWCHANGER_TEXT, GX_STRING_ID_BUTTON_DISABLED);
            update_text_id((GX_WIDGET *) widget, ID_INSTRUCTIONS, GX_STRING_ID_INSTRUCT_CHECKBOX);
            break;

        case GX_SIGNAL(ID_BUTTONENABLER, GX_EVENT_TOGGLE_ON):

            button_enabled = true;

            update_text_id((GX_WIDGET *) widget, ID_WINDOWCHANGER_TEXT, GX_STRING_ID_BUTTON_ENABLED);
            update_text_id((GX_WIDGET *) widget, ID_INSTRUCTIONS, GX_STRING_ID_INSTRUCT_BUTTON);
            break;


        case GX_SIGNAL(ID_WINDOW_1_CHANGER, GX_EVENT_CLICKED):
            //* Shows the settings screen and saves which screen the user is currently viewing. */

                    if(button_enabled == false)
                    {

                        break;
                    }
                        toggle_screen (p_window2_screen, widget);
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
 * @brief   Sends messages corresponding to the button touched.
 *
 * @param[in]   widget      Pointer to the widget that caused the event
 * @param[in]   event_ptr   Pointer to event that needs handling
 *
 * @retval      GX_SUCCESS
***********************************************************************************************************************/

UINT window2_screen_event(GX_WINDOW *widget, GX_EVENT *event_ptr)
{

    UINT gx_err = GX_SUCCESS;

    gx_err = gx_window_event_process (widget, event_ptr);
    if (GX_SUCCESS != gx_err) {
        APP_ERR_TRAP(FSP_ERR_ASSERTION);
    }
    switch (event_ptr->gx_event_type)
    {
        case GX_SIGNAL(ID_WINDOW_2_CHANGER, GX_EVENT_CLICKED):
            /** Returns to main screen. */
        toggle_screen (p_window1_screen, widget);

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



/** @} (end group events) */
