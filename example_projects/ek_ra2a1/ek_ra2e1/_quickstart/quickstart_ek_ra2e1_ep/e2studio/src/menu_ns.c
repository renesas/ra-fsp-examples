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
 * File Name    : menu_ns.c
 * Version      : .
 * Description  : Contains macros, data structures and functions used in the Next Stage screen page
 *********************************************************************************************************************/

#include <stdio.h>

#include "r_typedefs.h"
#include "r_console_if.h"

#include "common_init.h"
#include "common_utils.h"
#include "menu_ns.h"

#define MODULE_NAME     "\r\n\r\n%d. NEXT STEPS\r\n"

#define SUB_OPTIONS     "\r\nVisit the following URLs to learn about the kit and the RA family of" \
        "\r\nMCUs, download tools and documentation, and get support" \
        "\r\n" \
        "\r\n a) EK-RA2E1 resources:       renesas.com/ra/ek-ra2e1 " \
        "\r\n b) EK-RA2E1 resources:       renesas.com/ra" \
        "\r\n c) EK-RA2E1 resources:       renesas.com/ra/forum" \
        "\r\n d) EK-RA2E1 resources:       renesas.com/support" \


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

