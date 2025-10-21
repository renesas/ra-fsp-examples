/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : menu_ns.c
 * Version      : 1.0
 * Description  : The next steps screen display.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 01.01.2025 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "FreeRTOS.h"
#include "FreeRTOSconfig.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"

#include "common_init.h"
#include "common_utils.h"
#include "jlink_console.h"

#include "menu_ns.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define MIPI_OFFSET_IMAGE (0x1c2008)

#define CONNECTION_ABORT_CRTL    (0x00)
#define MENU_EXIT_CRTL           (0x20)

#define MODULE_NAME     "\r\n%d. NEXT STEPS\r\n"

#define SUB_OPTIONS     "\r\nVisit the following URLs to learn about the kit and the RA family of" \
                        "\r\nMCUs, download tools and documentation, and get support." \
                        "\r\n" \
                        "\r\n a) %s resources:       renesas.com/%s " \
                        "\r\n b) RA kits information:      renesas.com/ra/kits" \
                        "\r\n c) RA product information:   renesas.com/ra" \
                        "\r\n d) RA product support forum: renesas.com/ra/forum" \
                        "\r\n e) Renesas support:          renesas.com/support" \

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
 * Function Name: ns_display_menu
 * Description  : The next steps screen.
 * Return Value : Default 0, used for possible expansion.
 *********************************************************************************************************************/
test_fn ns_display_menu(void)
{
    int8_t c = -1;
    char_t fullname [] = FULL_NAME;
    volatile bool_t mode = true;
    bool_t new_mode = false;

    sprintf (s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);
    print_to_console(s_print_buffer);

    sprintf (s_print_buffer, MODULE_NAME, g_selected_menu);
    print_to_console(s_print_buffer);

    tolowerstr(&fullname[0], strlen(FULL_NAME));
    sprintf (s_print_buffer, SUB_OPTIONS, FULL_NAME, fullname);
    print_to_console(s_print_buffer);

    sprintf (s_print_buffer, MENU_RETURN_INFO);
    print_to_console(s_print_buffer);

    new_mode = jlink_set_mode(mode);
    start_key_check();

    while ((CONNECTION_ABORT_CRTL != c))
    {
        if (key_pressed())
        {
            /* c will be in the range of 0 to  g_menu_limit (which is int8_t) so safe to case into int8_t */
            c = (int8_t) get_detected_key();

            if (0 == c)
            {
                c = -1;
            }

            if ((MENU_EXIT_CRTL == c) || (CONNECTION_ABORT_CRTL == c))
            {
                break;
            }
            start_key_check();
        }
        vTaskDelay(10);
    }
    jlink_restore_mode(new_mode);
    return (0);
}
/**********************************************************************************************************************
 End of function ns_display_menu
 *********************************************************************************************************************/

