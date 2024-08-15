/**********************************************************************************************************************
 * File Name    : menu_ns.c
 * Version      : .
 * Description  : Contains macros, data structures and functions used in the Next Stage screen page
 *********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include <stdio.h>

#include "r_typedefs.h"
#include "r_console_if.h"

#include "common_init.h"
#include "common_utils.h"
#include "menu_ns.h"

#define MODULE_NAME     "\r\n\r\n%d. NEXT STEPS\r\n"

#define SUB_OPTIONS1        "\r\nVisit the following URLs to learn about the kit and the RA family of"
#define SUB_OPTIONS2        "\r\nMCUs, download tools and documentation, and get support"
#define SUB_OPTIONS3        "\r\n"
#define SUB_OPTIONS4        "\r\n a) EK-RA2E2 resources:       renesas.com/ra/ek-ra2e2 "
#define SUB_OPTIONS5        "\r\n b) EK-RA2E2 resources:       renesas.com/ra"
#define SUB_OPTIONS6        "\r\n c) EK-RA2E2 resources:       renesas.com/ra/forum"
#define SUB_OPTIONS7        "\r\n d) EK-RA2E2 resources:       renesas.com/support"


static char_t s_print_buffer[BUFFER_LINE_LENGTH] = {};

/**********************************************************************************************************************
 * Function Name: ns_display_menu
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
test_fn ns_display_menu(void)
{
    int8_t   c      = -1;
    uint32_t sDelay = 800U;

    sprintf(s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);
    print_to_console(s_print_buffer);

    sprintf(s_print_buffer, MODULE_NAME, g_selected_menu);
    print_to_console(s_print_buffer);

    sprintf(s_print_buffer, SUB_OPTIONS1);
    print_to_console(s_print_buffer);
    R_BSP_SoftwareDelay(sDelay, BSP_DELAY_UNITS_MICROSECONDS);

    sprintf(s_print_buffer, SUB_OPTIONS2);
    print_to_console(s_print_buffer);
    R_BSP_SoftwareDelay(sDelay, BSP_DELAY_UNITS_MICROSECONDS);

    sprintf(s_print_buffer, SUB_OPTIONS3);
    print_to_console(s_print_buffer);
    R_BSP_SoftwareDelay(sDelay, BSP_DELAY_UNITS_MICROSECONDS);

    sprintf(s_print_buffer, SUB_OPTIONS4);
    print_to_console(s_print_buffer);
    R_BSP_SoftwareDelay(sDelay, BSP_DELAY_UNITS_MICROSECONDS);

    sprintf(s_print_buffer, SUB_OPTIONS5);
    print_to_console(s_print_buffer);
    R_BSP_SoftwareDelay(sDelay, BSP_DELAY_UNITS_MICROSECONDS);

    sprintf(s_print_buffer, SUB_OPTIONS6);
    print_to_console(s_print_buffer);
    R_BSP_SoftwareDelay(sDelay, BSP_DELAY_UNITS_MICROSECONDS);

    sprintf(s_print_buffer, SUB_OPTIONS7);
    print_to_console(s_print_buffer);
    R_BSP_SoftwareDelay(sDelay, BSP_DELAY_UNITS_MICROSECONDS);

    sprintf(s_print_buffer, MENU_RETURN_INFO);
    print_to_console(s_print_buffer);
    R_BSP_SoftwareDelay(sDelay, BSP_DELAY_UNITS_MICROSECONDS);

    while (0 != c)
    {
        if (check_for_input_from_console())
        {
            c = input_from_console();

            if (' ' == c)
            {
                break;
            }
        }
    }
    return (0);
}
/**********************************************************************************************************************
 End of function ns_display_menu
 *********************************************************************************************************************/

