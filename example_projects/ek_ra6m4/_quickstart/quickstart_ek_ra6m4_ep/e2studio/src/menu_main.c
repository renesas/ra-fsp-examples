/**********************************************************************************************************************
 * File Name    : menu_main.c
 * Description  : The main menu controller.
 *********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"

#include "common_utils.h"
#include "menu_main.h"

#define SHORT_NAME      "RA6M4"
#define FULL_NAME       ("EK-RA6M4")
#define PART_NUMBER     ("RTK7EKA6M4S00001BE")
#define UNIQUE_ID       (0x20192020)

#define MODULE_NAME     "\r\n\x1b[2m\x1b[37mWelcome to Quick Start Example Project for %s!\r\n"

#define SUB_OPTIONS     "\r\n> Select from the options in the menu below:\r\n" \
	                    "\r\nMENU"

#define EP_INFO         "(2)"

typedef struct menu_fn_tbl
{
    char* name;                         /*<! Name of Test */
    test_fn ( * p_func)(void);            /*<! Pointer to Test Function */
} st_menu_fn_tbl_t;

/* Table of menu functions */
static st_menu_fn_tbl_t s_menu_items[] =
{
    {"Kit Information"                        , kis_display_menu},
    {"Web Server"                             , eth_emb_display_menu},
    {"Network Name Lookup"                    , eth_www_display_menu},
    {"Quad-SPI and Octo-SPI Speed Comparison" , ext_display_menu},
    {"Next Steps"                             , ns_display_menu},
    {"", NULL}
};

/* Terminal window escape sequences */
static const char * const sp_clear_screen   = "\x1b[2J";
static const char * const sp_cursor_home    = "\x1b[H";

int g_selected_menu = 0;
static char print_buffer[BUFFER_LINE_LENGTH] = {};

/**********************************************************************************************************************
 * Function Name: main_display_menu
 * Description  : .
 * Return Value : The main menu controller.
 *********************************************************************************************************************/
int main_display_menu(void)
{
    int c = -1;
    int menu_limit = 0;

    sprintf(print_buffer, "%s%s", sp_clear_screen, sp_cursor_home);
    print_to_console(print_buffer);

    sprintf(print_buffer, MODULE_NAME, FULL_NAME);
    print_to_console(print_buffer);
    sprintf(print_buffer, SUB_OPTIONS);
    print_to_console(print_buffer);

    for (int test_active = 0; NULL != s_menu_items[test_active].p_func; test_active++ )
    {
        sprintf(print_buffer, "\r\n %d. %s", (test_active + 1), s_menu_items[menu_limit++ ].name);
        print_to_console(print_buffer);
    }
    print_to_console("\r\n");

    while ((0 != c))
    {
        c = input_from_console();
        if (0 != c)
        {
            c = c - '0';
            g_selected_menu = c;

            if ((c > 0) && (c <= menu_limit))
            {
                s_menu_items[c-1].p_func();
                break;
            }
        }

    }
    return ((int)(c - '0'));

}
/**********************************************************************************************************************
 End of function main_display_menu
 *********************************************************************************************************************/


