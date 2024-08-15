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

#include "common_init.h"
#include "common_utils.h"
#include "menu_ns.h"

#define CONNECTION_ABORT_CRTL    (0x00)
#define MENU_EXIT_CRTL           (0x20)

#define SHORT_NAME      "RA6M4"
#define FULL_NAME       ("EK-RA6M4")
#define PART_NUMBER     ("RTK7EKA6M4S00001BE")
#define UNIQUE_ID       (0x20192020)
#define EP_VERSION      ("0.1WS")

#define MODULE_NAME     "\r\n%d. NEXT STEPS\r\n"

#define SUB_OPTIONS     "\r\nVisit the following URLs to learn about the kit and the RA family of" \
                        "\r\nMCUs, download tools and documentation, and get support" \
                        "\r\n" \
                        "\r\n a) EK-RA6M4 resources:       renesas.com/ra/ek-ra6m4 " \
                        "\r\n b) EK-RA6M4 resources:       renesas.com/ra" \
                        "\r\n c) EK-RA6M4 resources:       renesas.com/ra/forum" \
                        "\r\n d) EK-RA6M4 resources:       renesas.com/support" \

#define EP_INFO         "(2)"


/*
"<x>. NEXT STEPS"
""
"Visit the following URLs to learn about the kit and the RA family of
 MCUs, download tools and documentation, and get support."
""
"a) EK-RA6M4 resources:       renesas.com/ra/ek-ra6m4 "
"b) EK-RA6M4 resources:       renesas.com/ra"
"c) EK-RA6M4 resources:       renesas.com/ra/forum"
"d) EK-RA6M4 resources:       renesas.com/support"
"> Press enter to return to the menu."
 */

/* Terminal window escape sequences */
static const char * const sp_clear_screen   = "\x1b[2J";
static const char * const sp_cursor_home    = "\x1b[H";


static char print_buffer [BUFFER_LINE_LENGTH] = {};

/**********************************************************************************************************************
 * Function Name: ns_display_menu
 * Description  : .
 * Return Value : The next steps screen.
 *********************************************************************************************************************/
test_fn ns_display_menu(void)
{
    int c = -1;

    sprintf(print_buffer, "%s%s", sp_clear_screen, sp_cursor_home);
    print_to_console(print_buffer);

    sprintf(print_buffer, MODULE_NAME, g_selected_menu);
    print_to_console(print_buffer);

    sprintf(print_buffer, SUB_OPTIONS);
    print_to_console(print_buffer);

    sprintf(print_buffer, MENU_RETURN_INFO);
    print_to_console(print_buffer);

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


