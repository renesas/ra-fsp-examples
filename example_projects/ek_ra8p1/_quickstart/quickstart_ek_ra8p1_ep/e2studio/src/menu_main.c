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

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"

#include "menu_main.h"
#include "common_utils.h"
#include "common_init.h"
#include "menu_main.h"
#include "menu_ext.h"
#include "menu_kis.h"
#include "menu_led.h"

#include "jlink_console.h"
#include "tp_thread_entry.h"
#include "display_thread_entry.h"

#include "board_cfg_switch.h"
#include "board_greenpak.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

#define MODULE_NAME    ("\r\n\x1b[2m\x1b[37mWelcome to Quick Start Example Project for %s!\r\n")

#define CONNECTION_ABORT_CRTL    (0x00)
#define MENU_RETURN_CRTL         (0x0A)
#define MENU_VTAB_CRTL           (0x0D)
#define MENU_EXIT_CRTL           (0x20)

#define GUI_INSTRUCTIONS        "\r\nEnsure the correct screen and associated configuration has been set.\r\n" \
                                "To use the GUI interface\r\n" \
                                "Tap the Menu on the top left to view the application menu\r\n" \
                                "Tap the icons to launch applications.\r\n" \
                                "Tap X on the top right to return to the home screen\r\n" \
                                "or close the application menu.\r\n\r\n"

#define SUB_OPTIONS     "\r\n> Select from the options in the menu below or using the GUI:\r\n"

#define ENABLED_AS_DEFAULT  (true)
#define DISABLED_AS_DEFAULT (false)

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

int8_t g_selected_menu = (LCD_FULL_BG_GETTING_STARTED);
int8_t g_next_menu_selection = -1;
bool_t g_request_initialise = false;
char_t g_camera_welcome_message_str[80];

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
    {"Camera and LCD Demonstration",    lcd_display_menu},
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

void set_selected_menu_demo(uint32_t selected);

/* Initial system state */
static bool_t initialised = false;
static uint32_t s_demo_selected = LCD_FULL_BG_DEFAULT;

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
    int8_t c = -1;
    int8_t on_screen_selection = -1;

    lv_indev_data_t data = {};
    bool_t use_limited_options = false;

    check_ov5640_camera_connected();

    /* pop up menu is transitioning */
    if (g_next_menu_selection == -1)
    {
		sprintf (s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);

        /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
		print_to_console((void*) s_print_buffer);

		sprintf (s_print_buffer, "%s%s", gp_hide_cursor, gp_cursor_home);

        /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
		print_to_console((void*) s_print_buffer);

		sprintf (s_print_buffer, MODULE_NAME, FULL_NAME);

        /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
		print_to_console((void*) s_print_buffer);

	#ifndef R_RELEASE_MODE

		 sprintf (s_print_buffer, "%s\r\n", FSP_VERSION_BUILD_STRING);

         /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
		 print_to_console((void*) s_print_buffer);

		 sprintf (s_print_buffer, "%s\r\n", __TIMESTAMP__);

         /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
		 print_to_console((void *) s_print_buffer);

	#endif

		 g_menu_limit = (sizeof(s_menu_items) / sizeof(st_menu_fn_tbl_t)) - 1;
		 sprintf (s_print_buffer,"%s%s",SUB_OPTIONS, GUI_INSTRUCTIONS);

         /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
		 print_to_console((void *) s_print_buffer);

         print_to_console(g_greenpak_warning_str);
         print_to_console(g_switch_warning_str);

		 if(true == ts_configured())
		 {
		     /* Touch screen is configured, camera may not be */
		     if(true == check_ov5640_camera_connected())
		     {
		         print_to_console("\r\nParallel Graphics Expansion Board: Connected and Enabled\r\n");
		         print_to_console("Camera Expansion Board: Connected and Enabled\r\n");

		 	     sprintf(g_camera_welcome_message_str, "Camera and Graphics Demonstration");
		     }
		     else
		     {
                 print_to_console("\r\nParallel Graphics Expansion Board: Connected and Enabled\r\n");
                 print_to_console("Camera Expansion Board: Not Connected\r\n");

                 sprintf(g_camera_welcome_message_str, "Power down board and connect camera");
		     }

	         for (int8_t test_active = 0; NULL != s_menu_items[test_active].p_func; test_active++ )
             {
	             sprintf (s_print_buffer, "\r\n %d. %s", (test_active + 1), s_menu_items[test_active].p_name);

                 /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
                 print_to_console((void *) s_print_buffer);
	         }
		 }
		 else
		 {
		     use_limited_options = true;
             print_to_console("\r\nParallel Graphics Expansion Board: Not Connected\r\n");
             if(true == check_ov5640_camera_connected())
             {
                 print_to_console("Camera Expansion Board: Connected Not in Use\r\n");
             }
             else
             {
                 print_to_console("Camera Expansion Board: Not Connected\r\n");
             }

             for (int8_t test_active = 0; NULL != s_menu_items_nd[test_active].p_func; test_active++ )
             {
                 sprintf (s_print_buffer, "\r\n %d. %s", (test_active + 1), s_menu_items_nd[test_active].p_name);
                 print_to_console((void *) s_print_buffer);
            }
        }

        print_to_console("\r\n\r\n");
        print_to_console("Select Option :\r\n");
		print_to_console("\r\n");
    }

    g_request_initialise = true;

    start_key_check();

    if ((g_next_menu_selection == '9') || (g_next_menu_selection == -1))
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
//    	    configure_led(LED_CONFIGURED_NORMAL);
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
                c = g_next_menu_selection;
                g_next_menu_selection = -1;
            }
            else
            {
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

                 if(false == use_limited_options)
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

