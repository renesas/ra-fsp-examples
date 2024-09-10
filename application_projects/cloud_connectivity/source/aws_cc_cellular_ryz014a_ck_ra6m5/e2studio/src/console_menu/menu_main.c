/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/ 
/**********************************************************************************************************************
 * File Name    : menu_main.c
 * Description  : The main menu controller.
 *********************************************************************************************************************/

#include "FreeRTOS.h"
#include "FreeRTOSconfig.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"
#include "menu_main.h"
#include "common_utils.h"
#include "common_init.h"
#include "menu_main.h"
#include "console.h"
#include "menu_kis.h"
#include "menu_catm.h"

typedef struct menu_fn_tbl
{
    char_t * p_name; /*<! Name of Test */
    test_fn ( * p_func)(void); /*<! Pointer to Test Function */
} st_menu_fn_tbl_t;

int8_t g_selected_menu = 0;

static char_t s_print_buffer[BUFFER_LINE_LENGTH] = {};

/* Table of main menu functions */
static st_menu_fn_tbl_t s_menu_items[] =
{
    {"Get FSP version"               , get_version},
    {"Data flash"                    , data_flash_menu},
	{"Get UUID"                      , get_uuid},
	{"Get CATM Info"                 , catm_display_menu},
    {"Validate SIM activation"       , ping_display_menu},
	{"Start Application"             , start_app},
	{"Help"                          , help},
    {"", NULL }
};

/*********************************************************************************************************************
 * @brief   main_display_menu
 *
 * The main menu controller
 * @param       None
 * @retval      int8_t                 Return unsigned int
 *********************************************************************************************************************/
int8_t main_display_menu(void)
{
    int8_t key_pressed = -1;
    int8_t menu_limit = 0;

    sprintf (s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    printf_colour((void*)s_print_buffer);

    sprintf (s_print_buffer, SUB_OPTIONS);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    printf_colour((void*)s_print_buffer);

    for (int8_t test_active = 0; NULL != s_menu_items[test_active].p_func; test_active++ )
    {
        sprintf (s_print_buffer, "\r\n %d. %s", (test_active + 1), s_menu_items[menu_limit++ ].p_name);

        /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
        printf_colour((void*)s_print_buffer);
    }
    printf_colour (MENU_MAIN_SELECT);
    printf_colour("\r\n");

    while ((0 != key_pressed))
    {
        /* Wait for input from console */
        key_pressed = wait_for_keypress ();
        if (0 != key_pressed)
        {
            /* Cast, as compiler will assume calc is int */
            key_pressed = (int8_t) (key_pressed - '0');
            g_selected_menu = key_pressed;

            if ((key_pressed > 0) && (key_pressed <= menu_limit))
            {
                s_menu_items[key_pressed - 1].p_func ();
                break;
            }
        }
        vTaskDelay(200);
    }

    /* Cast, as compiler will assume calc is int */
    return ((int8_t) (key_pressed - '0'));
}
/**********************************************************************************************************************
 End of function main_display_menu
 *********************************************************************************************************************/

