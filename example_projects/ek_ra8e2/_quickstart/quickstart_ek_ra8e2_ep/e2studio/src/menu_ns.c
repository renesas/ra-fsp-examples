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
#include "FreeRTOSConfig.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"

#include "jlink_console.h"
#include "common_init.h"
#include "common_utils.h"
#include "tp_thread_entry.h"
#include "display_thread_entry.h"

#include "menu_main.h"
#include "menu_led.h"
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

st_rgb565_text_block_page_t g_ns_page =
{
    14, // Number of lines required (including blanks)
    " NEXT STEPS", /* Title */
    {
       {{PAGE_LEFT_EDGE, LINE(1)}, "Visit the following URLs to learn about the kit and the RA family of"},
       {{PAGE_LEFT_EDGE, LINE(2)}, "MCUs, download tools and documentation, and get support."},
       {{PAGE_LEFT_EDGE, LINE(3)}, " "},
       {{PAGE_LEFT_EDGE, LINE(4)}, " "},
       {{PAGE_LEFT_EDGE, LINE(5)}, "EK-RA8E2 resources: "},      {{PAGE_1ST_TAB,   LINE(5)}, "renesas.com/ek-ra8e2"},
       {{PAGE_LEFT_EDGE, LINE(6)}, "RA kits information:"},      {{PAGE_1ST_TAB,   LINE(6)}, "renesas.com/ra/kits"},
       {{PAGE_LEFT_EDGE, LINE(7)}, "RA product information:"},   {{PAGE_1ST_TAB,   LINE(7)}, "renesas.com/ra"},
       {{PAGE_LEFT_EDGE, LINE(8)}, "RA product support forum:"}, {{PAGE_1ST_TAB,   LINE(8)}, "renesas.com/ra/forum"},
       {{PAGE_LEFT_EDGE, LINE(9)}, "Renesas support:"},          {{PAGE_1ST_TAB,   LINE(9)}, "renesas.com/support"},
    }
};

/**********************************************************************************************************************
 * Function Name: ns_display_menu
 * Description  : The next steps screen.
 * Return Value : Default 0, used for possible expansion.
 *********************************************************************************************************************/
test_fn ns_display_menu(void)
{
    int8_t c = -1;
    char_t fullname [] = FULL_NAME;
    int8_t on_screen_selection = -1;

    lv_indev_data_t data = {};

    sprintf (s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);
    print_to_console(s_print_buffer);

    sprintf (s_print_buffer, MODULE_NAME, g_selected_menu);
    print_to_console(s_print_buffer);

    tolowerstr(&fullname[0], strlen(FULL_NAME));
    sprintf (s_print_buffer, SUB_OPTIONS, FULL_NAME, fullname);
    print_to_console(s_print_buffer);

    sprintf (s_print_buffer, MENU_RETURN_INFO);
    print_to_console(s_print_buffer);

    volatile bool_t mode = true;
    bool_t new_mode = false;

    start_key_check();

    dsp_set_background(LCD_FULL_BG_NEXT_STEPS);

    new_mode = jlink_set_mode(mode);

    start_key_check();

    configure_led(LED_NORMAL_UPDATE);

    while ((CONNECTION_ABORT_CRTL != c))
    {
        touchpad_get_copy(&data);
        on_screen_selection = process_menu_detection(&data, true, '5');

        if (key_pressed() || ((on_screen_selection != LCD_FULL_BG_POPUP_MENU) && (on_screen_selection != -1)))
        {
            if ((!g_overlay_selected) && (on_screen_selection != -1) && (on_screen_selection != '5'))
            {
                if (on_screen_selection == ' ')
                {
                    g_next_menu_selection = -1;
                }
                else
                {
                    g_next_menu_selection = on_screen_selection;
                }

                /* Pop up menu cancel detected */
                if (on_screen_selection == 'c')
                {
                    g_next_menu_selection = -1;
                }
                else
                {
                    /* Force exit */
                    c = MENU_EXIT_CRTL;
                }
            }
            else
            {
                c = (int8_t) get_detected_key();

                if (0 == c)
                {
                    c = -1;
                }
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

