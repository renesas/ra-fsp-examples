/**********************************************************************************************************************
 * File Name    : menu_ns.c
 * Version      : .
 * Description  : The next steps screen display.
 *********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include <stdio.h>

#include "r_typedefs.h"
#include "r_console.h"

#include "common_init.h"
#include "common_utils.h"
#include "menu_ns.h"

#define MODULE_NAME     "\r\n%d. NEXT STEPS\r\n"

#define SUB_OPTIONS     "\r\nVisit the following URLs to learn about the kit and the RA family of" \
        "\r\nMCUs, download tools and documentation, and get support" \
        "\r\n" \
        "\r\n a) EK-RA2L1 resources:       renesas.com/ra/ek-ra2l1 " \
        "\r\n b) EK-RA2L1 resources:       renesas.com/ra" \
        "\r\n c) EK-RA2L1 resources:       renesas.com/ra/forum" \
        "\r\n d) EK-RA2L1 resources:       renesas.com/support" \


static char_t s_print_buffer[BUFFER_LINE_LENGTH] = {};

/**********************************************************************************************************************
 * Function Name: ns_display_menu
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
test_fn ns_display_menu(void)
{
    int8_t c = -1;

    sprintf(s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);
    print_to_console(s_print_buffer);

    sprintf(s_print_buffer, MODULE_NAME, g_selected_menu);
    print_to_console(s_print_buffer);

    sprintf(s_print_buffer, SUB_OPTIONS);
    print_to_console(s_print_buffer);

    sprintf(s_print_buffer, MENU_RETURN_INFO);
    print_to_console(s_print_buffer);

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


