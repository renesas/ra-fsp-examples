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
 *         : 01.01.2025 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdarg.h>

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"

#include "menu_main.h"
#include "common_utils.h"
#include "common_init.h"
#include "menu_main.h"

#include "jlink_console.h"
#include "tp_thread_entry.h"
#include "touch_GT911.h"
#include "board_cfg_switch.h"
#include "board_greenpak.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

#define MODULE_NAME             "\r\n\x1b[2m\x1b[37mWelcome to Quick Start Example Project for %s!\r\n"
#define BANNER_VERSION          ("MP Version 1.01\r\n")

#define BANNER_LCD_ENABLED      ("The Parallel LCD Screen is now enabled.\r\n")
#define BANNER_LCD_DISABLED     ("The Parallel LCD Screen is not detected and not enabled.\r\n")

#define CONNECTION_ABORT_CRTL   (0x00)
#define MENU_RETURN_CRTL        (0x0A)
#define MENU_VTAB_CRTL          (0x0D)
#define MENU_EXIT_CRTL          (0x20)

#define GUI_INSTRUCTIONS        "\r\nEnsure the correct screen and associated configuration has been set.\r\n" \
                                "To use the GUI interface\r\n" \
                                "Tap the Menu on the top left to view the application menu\r\n" \
                                "Tap the icons to launch applications.\r\n" \
                                "Tap X on the top right to return to the home screen\r\n" \
                                "or close the application menu.\r\n\r\n"

#define SUB_OPTIONS             "\r\n> Select from the options in the menu below or using the GUI:\r\n"

#define ENABLED_AS_DEFAULT      (true)
#define DISABLED_AS_DEFAULT     (false)

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

int8_t g_selected_menu = LCD_FULL_BG_GETTING_STARTED;
int8_t g_next_menu_selection = -1;

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
static int8_t g_menu_limit = 0;
static char_t s_print_buffer[BUFFER_LINE_LENGTH] = {};

/* Table of menu functions */
static st_menu_fn_tbl_t s_menu_items[] =
{
    {"Kit Information",                 kis_display_menu},
    {"User LED Control Page",           led_display_menu},
    {"External Memory Read Write Demo", ext_display_menu},
    {"LCD Demonstration",               lcd_display_menu},
    {"Next Steps",                      ns_display_menu},
    {"", NULL }
};

/* Table of menu functions (no display detected */
static st_menu_fn_tbl_t s_menu_items_nd[] =
{
    {"Kit Information",                 kis_display_menu},
    {"External Memory Read Write Demo", ext_display_menu},
    {"Next Steps",                      ns_display_menu},
    {"", NULL }
};

/*  */
static bool_t initialised = false;

/**********************************************************************************************************************
 * Function Name: main_display_menu
 * Description  : The main menu controller.
 *                This function will take input from serial console or LCD touch controller ato determine which demo
 *                to execute.
 *                Some of the code in this function should only be executed once, and is managed by the initialised
 *                flag.
 * Return Value : Menu option executed
 *********************************************************************************************************************/
int8_t main_display_menu(void)
{
    int8_t c = -1;
    int8_t on_screen_selection = -1;

    lv_indev_data_t data = {};

//    switch_set_status_msg();

    if (g_next_menu_selection == -1) /* pop up menu is transitioning */
    {
        sprintf (s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);
        print_to_console((void*) s_print_buffer);

        sprintf (s_print_buffer, "%s%s", gp_hide_cursor, gp_cursor_home);
        print_to_console((void*) s_print_buffer);

        sprintf (s_print_buffer, MODULE_NAME, FULL_NAME);
        print_to_console(s_print_buffer);

#ifndef R_RELEASE_MODE

        sprintf ("%s\r\n", FSP_VERSION_BUILD_STRING);
        print_to_console(s_print_buffer);

        sprintf (s_print_buffer, "%s\r\n", __TIMESTAMP__);
        print_to_console(s_print_buffer);

        print_to_console(BANNER_VERSION);

#endif


        print_to_console(SUB_OPTIONS);
        print_to_console(GUI_INSTRUCTIONS);

        print_to_console(g_greenpak_warning_str);
        print_to_console(g_switch_warning_str);

        if (display_ts_detected())
        {
            print_to_console(BANNER_LCD_ENABLED);

            g_menu_limit = (sizeof(s_menu_items) / sizeof(st_menu_fn_tbl_t)) - 1;

            for (int8_t test_active = 0; NULL != s_menu_items[test_active].p_func; test_active++ )
            {
                sprintf (s_print_buffer, "\r\n %d. %s", (test_active + 1), s_menu_items[test_active].p_name);
                print_to_console((void *) s_print_buffer);
            }
        }
        else
        {

            print_to_console(BANNER_LCD_DISABLED);

            g_menu_limit = (sizeof(s_menu_items_nd) / sizeof(st_menu_fn_tbl_t)) - 1;
            for (int8_t test_active = 0; NULL != s_menu_items_nd[test_active].p_func; test_active++ )
            {
                sprintf (s_print_buffer, "\r\n %d. %s", (test_active + 1), s_menu_items_nd[test_active].p_name);
                print_to_console((void *) s_print_buffer);
            }
        }

        print_to_console("\r\n\r\n");
        print_to_console("Select Option :\r\n");
    }

    start_key_check();

    if (('9' == g_next_menu_selection) || ((-1) == g_next_menu_selection))
    {
        /* do nothing */
    }
    else
    {
        /* re-launch last demo as user selected something from pop-up, or cancelled pop-up */
        c = g_next_menu_selection;
    }

    if (!initialised)
    {
        initialised = true;
    }

     while (0 != c)
    {
      configure_led(LED_NORMAL_UPDATE);
        if (g_next_menu_selection == -1)
        {
            vTaskDelay(20);
            touchpad_get_copy(&data);
            on_screen_selection = process_menu_detection(&data, true, '0');
        }
        if (key_pressed() || (g_next_menu_selection != -1) || ((on_screen_selection != LCD_FULL_BG_POPUP_MENU) && (on_screen_selection != -1)))
        {
            // Screen selection did occur
            if ((on_screen_selection != -1) && (on_screen_selection != LCD_FULL_BG_POPUP_MENU))
            {
                c = on_screen_selection;
            }
            // Screen selection did occur and option changed
            else if ((-1) != g_next_menu_selection) 
            {
                /* Screen selection did occur and option changed */
                c = g_next_menu_selection;
                g_next_menu_selection = -1;
            }
            else
            {
                /* Screen selection occurred */
                c = (int8_t)get_detected_key();
            }

            /* special code to re-show getting started */
            if ('9' == c)
            {
                /* process screen menu */
                set_gs_page();
                g_next_menu_selection = -1;
                break;
            }

            /* exit getting started, show welcome */
            if ('z' == c)
            {
                /* process screen menu */
                dsp_set_background(LCD_FULL_BG_WELCOME);
                g_show_getting_started_page = false;
                break;
            }

            /* Cast, as compiler will assume calc is int */
            c = (int8_t) (c - '0');
            g_selected_menu = c;

            if ((c > 0) && (c <= g_menu_limit))
            {
                 g_overlay_selected = false;

                 if (display_ts_detected())
                 {
                     s_menu_items[c - 1].p_func ();
                 }
                 else
                 {
                     s_menu_items_nd[c - 1].p_func ();
                 }

                if (g_next_menu_selection == -1) // Screen selection will occur as option changed
                {
                    g_selected_menu = LCD_FULL_BG_WELCOME;
                    dsp_set_background(LCD_FULL_BG_WELCOME);
                }
                break;
            }
            else
            {
                 if (on_screen_selection == -1) // Screen selection didn't occur
                {
                    start_key_check();
                }
            }
        }

        if (on_screen_selection == LCD_FULL_BG_POPUP_MENU)
        {
             // Processing popup menu
             vTaskDelay(10);
        }
    }

    /* Cast, as compiler will assume calculation result is an int */
    return ((int8_t) (c - '0'));
}
/**********************************************************************************************************************
 End of function main_display_menu
 *********************************************************************************************************************/
