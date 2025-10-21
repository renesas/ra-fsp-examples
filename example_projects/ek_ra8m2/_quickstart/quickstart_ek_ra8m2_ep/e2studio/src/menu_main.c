/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : menu_main.c
 * Version      : 1.0
 * Description  : Main Menu file. Manages demo interfacce to console and lcd.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 14/10/2025 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"

#include "common_utils.h"
#include "common_init.h"
#include "menu_main.h"
#include "menu_ext.h"
#include "jlink_console.h"

#include "board_cfg_switch.h"
#include "board_greenpak.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/


#ifndef __TIMESTAMP__
#define __TIMESTAMP__ "TIMESTAMP NOT SUPPORTED"
#endif

#define MODULE_NAME    ("\r\n\x1b[2m\x1b[37mWelcome to Quick Start Example Project for %s!")
#define SUB_OPTIONS    ("> Select from the options in the menu below:\r\n")

#define CONNECTION_ABORT_CRTL    (0x00)
#define MENU_RETURN_CRTL         (0x0A)
#define MENU_VTAB_CRTL           (0x0D)
#define MENU_EXIT_CRTL           (0x20)

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/
int8_t g_selected_menu = CONNECTION_ABORT_CRTL;

int8_t g_next_menu_selection = -1;
bool_t g_request_initialise = false;

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
static int8_t g_menu_limit = 0;
static char_t s_print_buffer[BUFFER_LINE_LENGTH] = {};

/* Initial system state */
static bool_t initialised = false;
static uint32_t s_demo_selected = R_QSEP_DEMO_DEFAULT;

/* Table of menu functions */
static st_menu_fn_tbl_t s_menu_items[] =
{
    {"Kit Information",                 kis_display_menu},
    {"External Memory Read Write Demo", ext_display_menu},
    {"Next Steps",                      ns_display_menu},
    {"", NULL }
};

/**********************************************************************************************************************
 * Function Name: main_display_menu
 * Description  : The main menu controller.
 *                This function will take input from serial console or LCD touch controller to determine which demo
 *                to execute.
 *                Some of the code in this function should only be executed once, and is managed by the initialised
 *                flag.
 * Return Value : Menu option executed
 *********************************************************************************************************************/
int8_t main_display_menu(void)
{
    uint8_t c = R_UNINITIALISED_U8;

    /* pop up menu is transitioning */
    if (g_next_menu_selection == (-1))
    {
        sprintf (s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);

        print_to_console(s_print_buffer);

        sprintf (s_print_buffer, MODULE_NAME, FULL_NAME);
        print_to_console(s_print_buffer);

    #ifndef R_RELEASE_MODE
        sprintf (s_print_buffer, "\r\n%s\r\n", FSP_VERSION_BUILD_STRING);

        print_to_console(s_print_buffer);

        sprintf (s_print_buffer, "%s\r\n", __TIMESTAMP__);

        print_to_console(s_print_buffer);
    #endif

        g_menu_limit = ((sizeof(s_menu_items)) / (sizeof(st_menu_fn_tbl_t))) - 1;

        /* Device wont update without running additional applications, no need to re-check */
        print_to_console(g_greenpak_warning_str);

        /* SW4 can be updated without board power cycle, so re-test */
        switch_set_status_msg();
        print_to_console(g_switch_warning_str);

        print_to_console(SUB_OPTIONS);

        for (int8_t test_active = 0; !(s_menu_items[test_active].p_func == NULL); test_active++ )
        {
            sprintf (s_print_buffer, "\r\n %d. %s", (test_active + 1), s_menu_items[test_active].p_name);
            print_to_console(s_print_buffer);
        }

        print_to_console("\r\n\r\n");
        print_to_console("Select Option :\r\n");
        print_to_console("\r\n");
    }

    g_request_initialise = true;

    start_key_check();

    if (!initialised)
    {
        initialised = true;
    }

    while (0 != c)
    {
        if (key_pressed())
        {
            c = get_detected_key();

            /* Cast, as compiler will assume calc is int */
            c = (c - '0');

            if ((c > 0) && (c <= g_menu_limit))
            {
                /* c will be in the range of 0 to  g_menu_limit (which is int8_t) so safe to case into int8_t */
                g_selected_menu = (int8_t)c;

                s_menu_items[c - 1].p_func ();
                break;
            }
            else
            {
                start_key_check();
            }
        }
        else
        {
            vTaskDelay(10);
        }
    }

    /* Cast, as compiler will assume calculation result is an int */
    return (int8_t)(c - '0');
}
/**********************************************************************************************************************
 End of function main_display_menu
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: set_selected_menu_demo
 * Description  : Helper function
 *                Stores the selected current demo screen taken from the list defined in
 *                common_init.h - see comment available option for LCD screens in the demo
 * Return Value : NONE
 *********************************************************************************************************************/
void set_selected_menu_demo(uint32_t selected)
{
    s_demo_selected = selected;
}
/******************************************************************************
 * End of function set_selected_menu_demo
 ******************************************************************************/

/**********************************************************************************************************************
 * Function Name: get_selected_menu_demo
 *                Retrieves the selected current demo screen taken from the list defined in
 *                common_init.h - see comment available option for LCD screens in the demo
 * Return Value : NONE
 *********************************************************************************************************************/
uint32_t get_selected_menu_demo(void)
{
    uint32_t selected = s_demo_selected;
    return selected;
}
/******************************************************************************
 * End of function get_selected_menu_demo
 ******************************************************************************/

