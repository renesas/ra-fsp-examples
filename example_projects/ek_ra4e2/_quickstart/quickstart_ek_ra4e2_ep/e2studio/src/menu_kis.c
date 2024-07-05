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
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : menu_kis.c
 * Version      : .
 * Description  : The Kit information demo.
 *********************************************************************************************************************/

#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"

#include "common_utils.h"
#include "common_init.h"
#include "common_data.h"
#include "menu_kis.h"

#define CONNECTION_ABORT_CRTL    (0x00)
#define MENU_EXIT_CRTL           (0x20)


#define MODULE_NAME     "\r\n%d. KIT INFORMATION\r\n"
#define SUB_OPTIONS     "\r\n a) Kit name:                           %s "              \
                        "\r\n b) Kit ordering part number:           %s "              \
                        "\r\n c) RA Device part number:              %s"               \
                        "\r\n d) RA MCU 128-bit Unique ID (hex):     "                 \
                        "%s%08x%s-%s%08x%s-%s%08x%s-%s%08x%s"                          \
                        "\r\n e) RA MCU Die temperature (F/C):       "                 \
                        "%s%.*s/%.*s%s           "  \
                        "\r\n f) Blue LED blinking frequency (Hz):   %s%d%s "      \
                        "\r\n g) Blue LED blinking intensity (%%):    %s%d%s "
                                                                            /* % needs escaping */

extern char_t g_print_buffer[];

static const TickType_t s_ticks_to_wait = 100 / portTICK_PERIOD_MS;

/**********************************************************************************************************************
 * Function Name: kis_display_menu
 * Description  : .
 * Return Value : The Kit information demo screen
 *********************************************************************************************************************/
test_fn kis_display_menu(void)
{
    int8_t c = -1;
    bsp_unique_id_t const * p_uid = R_BSP_UniqueIdGet();

    sprintf(g_print_buffer, "%s%s%s", gp_clear_screen, gp_cursor_home, gp_white_fg);
    print_to_console(g_print_buffer);

    sprintf(g_print_buffer, MODULE_NAME, g_selected_menu);
    print_to_console(g_print_buffer);

    float32_t mcu_temp_f = g_board_status.temperature_f_as_f;
    float32_t mcu_temp_c = g_board_status.temperature_c_as_f;
    float32_t tmp        = 0.00;

    uint16_t d = 0;
    uint16_t r = 0;

    char_t fs[16] = "";
    char_t cs[16] = "";

    /* As nano_printf is disabled for size convert result to d.r */
    tmp = mcu_temp_f * 100.0f;

    /* Truncate the mantissa to leave only the integer part */
    d = (uint16_t) (mcu_temp_f / 1.00f);

    /* Truncate the mantissa to leave only the integer part */
    r = (uint16_t) (tmp / 1.00f);

    /* Cast to maintain siz of uint16_t */
    r = (uint16_t) (r % (d * 100U));

    sprintf(fs,"%d.%02d",d,r);

    tmp = mcu_temp_c * 100.0f;

    /* Truncate the mantissa to leave only the integer part */
    d = (uint16_t) (mcu_temp_c / 1.00f);

    /* Truncate the mantissa to leave only the integer part */
    r = (uint16_t) (tmp / 1.00f);

    /* Cast to maintain size of uint16_t */
    r = (uint16_t) (r % (d * 100U));

    sprintf(cs,"%d.%02d",d,r);
    size_t len_f = (strcspn(fs, ".") + 3);
    size_t len_c = (strcspn(cs, ".") + 3);

    sprintf(g_print_buffer, SUB_OPTIONS, FULL_NAME, PART_NUMBER, DEVICE_NUMBER,
            gp_green_fg, (size_t)p_uid->unique_id_words[0], gp_white_fg, gp_green_fg, (size_t)p_uid->unique_id_words[1], gp_white_fg,
            gp_green_fg, (size_t)p_uid->unique_id_words[2], gp_white_fg, gp_green_fg, (size_t)p_uid->unique_id_words[3], gp_white_fg,
            gp_green_fg, len_f,fs,len_c,cs, gp_white_fg,
            gp_green_fg, g_pwm_rates_data[g_board_status.led_frequency], gp_white_fg,
            gp_green_fg, g_pwm_dcs_data[g_board_status.led_intensity], gp_white_fg
            );
    print_to_console(g_print_buffer);

    sprintf(g_print_buffer, MENU_RETURN_INFO);
    print_to_console(g_print_buffer);

    /* provide small delay so board_status should be up to date */
    vTaskDelay(s_ticks_to_wait);
    xEventGroupSetBits(g_update_console_event, STATUS_DISPLAY_MENU_KIS);

    while (CONNECTION_ABORT_CRTL != c)
    {
        c = input_from_console();
        if ((MENU_EXIT_CRTL == c) || (CONNECTION_ABORT_CRTL == c))
        {
            break;
        }
    }

    xEventGroupClearBits(g_update_console_event, STATUS_DISPLAY_MENU_KIS);
    return (0);
}
/**********************************************************************************************************************
 End of function kis_display_menu
 *********************************************************************************************************************/


