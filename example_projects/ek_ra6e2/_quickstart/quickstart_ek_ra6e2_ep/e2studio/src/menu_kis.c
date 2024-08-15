/**********************************************************************************************************************
 * File Name    : menu_kis.c
 * Version      : .
 * Description  : The Kit information demo.
 *********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

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


