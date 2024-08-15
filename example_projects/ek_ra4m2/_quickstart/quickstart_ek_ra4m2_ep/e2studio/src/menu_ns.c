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

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"

#include "common_utils.h"
#include "common_init.h"
#include "menu_ns.h"

#define CONNECTION_ABORT_CRTL    (0x00)
#define MENU_EXIT_CRTL           (0x20)

#define MODULE_NAME     "\r\n%d. NEXT STEPS\r\n"

#define SUB_OPTIONS     "\r\nVisit the following URLs to learn about the kit and the RA family of" \
                        "\r\nMCUs, download tools and documentation, and get support" \
                        "\r\n" \
                        "\r\n a) EK-RA4M2 resources:       renesas.com/ra/ek-ra4m2 " \
                        "\r\n b) EK-RA4M2 resources:       renesas.com/ra" \
                        "\r\n c) EK-RA4M2 resources:       renesas.com/ra/forum" \
                        "\r\n d) EK-RA4M2 resources:       renesas.com/support" \

#define EP_INFO         "(2)"


/*
"<x>. NEXT STEPS"
""
"Visit the following URLs to learn about the kit and the RA family of
 MCUs, download tools and documentation, and get support."
""
"a) EK-RA4M2 resources:       renesas.com/ra/ek-ra4m2 "
"b) EK-RA4M2 resources:       renesas.com/ra"
"c) EK-RA4M2 resources:       renesas.com/ra/forum"
"d) EK-RA4M2 resources:       renesas.com/support"
"> Press enter to return to the menu."
 */
extern char_t g_print_buffer[];


/**********************************************************************************************************************
 * Function Name: ns_display_menu
 * Description  : .
 * Return Value : The next steps screen.
 *********************************************************************************************************************/
test_fn ns_display_menu(void)
{
    int16_t c = -1;

    sprintf(g_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);
    print_to_console(g_print_buffer);

    sprintf(g_print_buffer, MODULE_NAME, g_selected_menu);
    print_to_console(g_print_buffer);

    sprintf(g_print_buffer, SUB_OPTIONS);
    print_to_console(g_print_buffer);

    sprintf(g_print_buffer, MENU_RETURN_INFO);
    print_to_console(g_print_buffer);

    while ((CONNECTION_ABORT_CRTL != c))
    {
        c = input_from_console();
        if ((MENU_EXIT_CRTL == c) || (CONNECTION_ABORT_CRTL == c))
        {
            break;
        }
    }
    return (0);
}
/**********************************************************************************************************************
 End of function ns_display_menu
 *********************************************************************************************************************/


