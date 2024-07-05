/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : menu_main.c
 * Description  : The main menu controller.
 *********************************************************************************************************************/

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"

#include "common_utils.h"
#include "common_init.h"
#include "menu_main.h"


#define MODULE_NAME     "\r\n\x1b[2m\x1b[37mWelcome to Quick Start Example Project for %s!\r\n"

#define SUB_OPTIONS     "\r\n> Select from the options in the menu below:\r\n" \
                        "\r\nMENU"

#define EP_INFO         "(2)"

typedef struct menu_fn_tbl
{
    char_t * p_name;                      /*<! Name of Test */
    test_fn ( * p_func)(void);            /*<! Pointer to Test Function */
} st_menu_fn_tbl_t;

/* Common console formatting for project */
/* Terminal window common display sequences */
const char_t * const gp_cursor_temp      = "\x1b[8;41H\x1b[K";
const char_t * const gp_cursor_frequency = "\x1b[9;41H\x1b[K";
const char_t * const gp_cursor_intensity = "\x1b[10;41H\x1b[K";

/* Terminal window generic escape sequences */
const char_t * const gp_green_fg   = "\x1b[32m";
const char_t * const gp_white_fg   = "\x1b[37m";
const char_t * const gp_clear_screen   = "\x1b[2J";
const char_t * const gp_cursor_home    = "\x1b[H";
const char_t * const gp_cursor_store     = "\x1b[s";
const char_t * const gp_cursor_restore   = "\x1b[u";


extern char_t g_print_buffer[];
int8_t g_selected_menu = 0;


/* Table of menu functions */
static st_menu_fn_tbl_t s_menu_items[] =
{
    {"Kit Information"                        , kis_display_menu},
    {"Quad-SPI FLASH Write/Read"              , ext_display_menu},
    {"Next Steps"                             , ns_display_menu},
    {"", NULL}
};


/**********************************************************************************************************************
 * Function Name: main_display_menu
 * Description  : .
 * Return Value : The main menu controller.
 *********************************************************************************************************************/
int8_t main_display_menu(void)
{
    int8_t c = -1;
    int8_t menu_limit = 0;

    sprintf(g_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);
    print_to_console(g_print_buffer);

    sprintf(g_print_buffer, MODULE_NAME, FULL_NAME);
    print_to_console(g_print_buffer);
    sprintf(g_print_buffer, SUB_OPTIONS);
    print_to_console(g_print_buffer);

    for (int8_t test_active = 0; NULL != s_menu_items[test_active].p_func; test_active++ )
    {
        sprintf(g_print_buffer, "\r\n %d. %s", (test_active + 1), s_menu_items[menu_limit++ ].p_name);
        print_to_console(g_print_buffer);
    }
    print_to_console("\r\n");

    while (0 != c)
    {
        c = input_from_console();
        if (0 != c)
        {
            /* Cast, as calculation will be assumes to return int */
            c = (int8_t)(c - '0');
            g_selected_menu = c;

            if ((c > 0) && (c <= menu_limit))
            {
                s_menu_items[c-1].p_func();
                break;
            }
        }

    }

    /* Cast as calculation will assume int as result */
    return ((int8_t)(c - '0'));

}
/**********************************************************************************************************************
 End of function main_display_menu
 *********************************************************************************************************************/


