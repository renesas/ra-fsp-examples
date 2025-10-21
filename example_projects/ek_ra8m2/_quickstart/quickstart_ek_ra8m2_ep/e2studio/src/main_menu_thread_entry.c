/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : main_menu_thread_entry.c
 * Version      : 1.0
 * Description  : Contains user entry .
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 14/10/2025 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "main_menu_thread.h"
#include "jlink_console.h"
#include "common_init.h"
#include "common_utils.h"
#include "common_data.h"
#include "menu_main.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#ifdef USE_TO_MANIPLATE_OPTIONATION_AT_FUNCTION_LEVEL

/* Put function between push n pop */
#pragma GCC push_options
#pragma GCC optimize ("O0")
#pragma GCC pop_options

#endif

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
static char_t s_print_buffer[BUFFER_LINE_LENGTH] = {};


/**********************************************************************************************************************
 * Function Name: use_welcome_message
 * Description  : Common function to prepare console of output
 * Argument     : None.
 * Return Value : None.
 *********************************************************************************************************************/
static void use_welcome_message(void)
{
    sprintf (s_print_buffer, "%s%s%s", gp_clear_screen, gp_cursor_home,gp_hide_cursor);
    print_to_console(s_print_buffer);

//    sprintf (s_print_buffer, "\x1b[2m\x1b[37mWelcome to Quick Start Example Project for %s!\r\n", FULL_NAME);
//    print_to_console(s_print_buffer);
}
/**********************************************************************************************************************
 End of function use_welcome_message
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: main_menu_thread_entry
 * Description  : Entry point for thread
 * Argument     : pvParameters
 * Return Value : None.
 *********************************************************************************************************************/
void main_menu_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);

    use_welcome_message();
    common_init();

    /* Each call to main_display_menu() implements the main menu
     * as the code is organised to return to the start after each sub-demo is run. */
    while (1)
    {
        main_display_menu();
        vTaskDelay(100);
    }
}
/**********************************************************************************************************************
 End of function main_menu_thread_entry
 *********************************************************************************************************************/
