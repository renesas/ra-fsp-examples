/**********************************************************************************************************************
 * File Name    : menu_ns.c
 * Version      : .
 * Description  : The next steps screen display.
 *********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
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
                        "\r\n a) EK-RA6E2 resources:       renesas.com/ra/ek-ra6e2 " \
                        "\r\n b) RA kits information:      renesas.com/ra/kits" \
                        "\r\n c) RA product information:   renesas.com/ra" \
                        "\r\n d) RA product support forum: renesas.com/ra/forum" \
                        "\r\n e) Renesas support:          renesas.com/support" \

#define EP_INFO         "(2)"

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

    // Select text foreground
    sprintf(g_print_buffer, "%s", gp_white_fg);
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


