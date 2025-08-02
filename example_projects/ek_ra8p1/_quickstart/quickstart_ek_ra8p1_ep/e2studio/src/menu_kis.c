/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/***********************************************************************************************************************
 * File Name    : menu_kis.c
 * Version      : 1.0
 * Description  : Kit Information Screen. Manages demo interface to console and lcd.
 **********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 01.01.2025 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"

#include "common_init.h"
#include "common_data.h"
#include "common_utils.h"
#include "menu_kis.h"
#include "jlink_console.h"

#include "menu_led.h"
#include "tp_thread_entry.h"
#include "display_thread_entry.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define CONNECTION_ABORT_CRTL    (0x00)
#define MENU_EXIT_CRTL           (0x20)

#define MODULE_NAME     "\r\n%d. KIT INFORMATION\r\n"

#define SUB_OPTIONS     "\r\n\x1b[2m\x1b[37m a) Kit name:                           %s "                               \
                        "\r\n\x1b[2m\x1b[37m b) Kit ordering part number:           %s "                               \
                        "\r\n\x1b[2m\x1b[37m c) RA Device part number:              %s"                                \
                        "\r\n\x1b[2m\x1b[37m d) RA MCU 128-bit Unique ID (hex):     "                                  \
                        "\x1b[32m%08x\x1b[37m-\x1b[32m%08x\x1b[37m-\x1b[32m%08x\x1b[37m-\x1b[32m%08x\x1b[37m"          \
                        "\r\n\x1b[2m\x1b[37m e) RA MCU Die temperature (F/C):       \x1b[32m%d.%02d/%d.%02d\x1b[37m"   \
                        "\r\n\x1b[2m\x1b[37m f) Blue LED blinking frequency (Hz):   \x1b[32m%d\x1b[37m"                \
                        "\r\n\x1b[2m\x1b[37m g) Blue LED blinking intensity (%%):    \x1b[32m%d\x1b[37m"                \

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
static char_t s_print_buffer[BUFFER_LINE_LENGTH] = { };

static const TickType_t s_ticks_to_wait = 100 / portTICK_PERIOD_MS;

st_rgb565_text_block_page_t g_kis_page = {
	15, // Number of lines required (including blanks)
	" KIT Information", /* Title */
	{
       {{PAGE_LEFT_EDGE, LINE(1)},"Kit name "},
       {{PAGE_1ST_TAB,   LINE(1)},"%s"},
       {{PAGE_LEFT_EDGE, LINE(2)},"Kit ordering part number "},
       {{PAGE_1ST_TAB,   LINE(2)},"%s"},
       {{PAGE_LEFT_EDGE, LINE(3)},"RA Device part number "},
       {{PAGE_1ST_TAB,   LINE(3)},"%s"},

       {{PAGE_LEFT_EDGE, LINE(4)},"RA MCU 128-bit Unique ID (Hex)"},
       {{PAGE_1ST_TAB,   LINE(4)},""},
       {{PAGE_1ST_TAB,   LINE(5)},""},

       {{PAGE_LEFT_EDGE, LINE(6)},"RA MCU Die temperature (F/C)"},
       {{PAGE_1ST_TAB,   LINE(6)},""},

       {{PAGE_LEFT_EDGE, LINE(7)},"Blue LED blinking frequency (Hz)"},
       {{PAGE_1ST_TAB,   LINE(7)},""},
       {{PAGE_LEFT_EDGE, LINE(8)},"Blue LED blinking intensity (%)"},
       {{PAGE_1ST_TAB,   LINE(8)},""},
	}
};

/**********************************************************************************************************************
 * Function Name: kis_display_menu
 * Description  : The Kit information demo screen.
 * Return Value : Default 0, used for possible expansion.
 *********************************************************************************************************************/
test_fn kis_display_menu(void)
{
    int8_t c = -1;
    uint16_t wn_mcu_temp_f = 0;
    uint16_t fr_mcu_temp_f = 0;
    uint16_t wn_mcu_temp_c = 0;
    uint16_t fr_mcu_temp_c = 0;
    bsp_unique_id_t const * p_uid = R_BSP_UniqueIdGet ();

    int8_t on_screen_selection = -1;
    char_t mod_uuid[128] = "";
    lv_indev_data_t data = {};

    dsp_set_background(LCD_FULL_BG_KIT_INFORMATION);

    sprintf (s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);

    print_to_console(s_print_buffer);
    vTaskDelay (s_ticks_to_wait);

    /* provide small delay so board_status should be up to date */
    vTaskDelay (s_ticks_to_wait);
    xEventGroupSetBits (g_update_console_event, STATUS_DISPLAY_MENU_KIS);

    wn_mcu_temp_f = g_board_status.temperature_f.whole_number;
    fr_mcu_temp_f = g_board_status.temperature_f.mantissa;
    wn_mcu_temp_c = g_board_status.temperature_c.whole_number;
    fr_mcu_temp_c = g_board_status.temperature_c.mantissa;

    sprintf(g_kis_page.text_block[1].line, FULL_NAME);
    sprintf(g_kis_page.text_block[3].line, PART_NUMBER);
    sprintf(g_kis_page.text_block[5].line, DEVICE_NUMBER);

    sprintf(mod_uuid, "%08x-%08x-%08x-%08x", (int_t)p_uid->unique_id_words[0], (int_t)p_uid->unique_id_words[1],
                                             (int_t)p_uid->unique_id_words[2], (int_t)p_uid->unique_id_words[3]);
    toupperstr((char_t *) &mod_uuid, (uint32_t) strlen(mod_uuid));
    sprintf(g_kis_page.text_block[7].line,"%s",mod_uuid);
    sprintf(g_kis_page.text_block[10].line, "%d.%02d\'F/%d.%02d\'C",wn_mcu_temp_f, fr_mcu_temp_f, wn_mcu_temp_c, fr_mcu_temp_c);

    /* Read the console terminal to get any characters already pressed. */
    start_key_check();

    while (CONNECTION_ABORT_CRTL != c)
    {
        configure_led(LED_NORMAL_UPDATE);

        /* update LCD (if connected) */
        sprintf(g_kis_page.text_block[10].line, "%d.%02d\'F/%d.%02d\'C",wn_mcu_temp_f, fr_mcu_temp_f, wn_mcu_temp_c, fr_mcu_temp_c);
        sprintf(g_kis_page.text_block[12].line, "%d", g_pwm_rates_data[g_board_status.led_frequency]);
        sprintf(g_kis_page.text_block[14].line, "%d", g_pwm_dcs_data[g_board_status.led_intensity]);

        /* Process touch information */
        touchpad_get_copy(&data);
        on_screen_selection = process_menu_detection(&data, true, '1');

        /* Update console (VCOM) */
        sprintf (s_print_buffer, "%s%s", gp_hide_cursor, gp_cursor_home);
        print_to_console (s_print_buffer);

        sprintf (s_print_buffer, MODULE_NAME, g_selected_menu);

        print_to_console(s_print_buffer);

        wn_mcu_temp_f = g_board_status.temperature_f.whole_number;
        fr_mcu_temp_f = g_board_status.temperature_f.mantissa;
        wn_mcu_temp_c = g_board_status.temperature_c.whole_number;
        fr_mcu_temp_c = g_board_status.temperature_c.mantissa;

        sprintf (s_print_buffer, SUB_OPTIONS, FULL_NAME, PART_NUMBER, DEVICE_NUMBER,
                (int_t)p_uid->unique_id_words[0],  (int_t)p_uid->unique_id_words[1], // typedef uint_t in stdint.h
                (int_t)p_uid->unique_id_words[2],  (int_t)p_uid->unique_id_words[3], // as signed long long int64_t;
                wn_mcu_temp_f, fr_mcu_temp_f, wn_mcu_temp_c, fr_mcu_temp_c,
                g_pwm_rates_data[g_board_status.led_frequency], g_pwm_dcs_data[g_board_status.led_intensity]);

        print_to_console(s_print_buffer);

        sprintf (s_print_buffer, MENU_RETURN_INFO);

        print_to_console(s_print_buffer);

        /* de-bounce switches when KIS screen is used */
        if(g_sw1_updated)
        {
            g_sw1_updated = false;
        }
        if(g_sw2_updated)
        {
            g_sw2_updated = false;
        }

        if (key_pressed() || ((on_screen_selection != LCD_FULL_BG_POPUP_MENU) && (on_screen_selection != -1)))
        {
            if((false == g_overlay_selected) && ((-1) != on_screen_selection) && ('1' != on_screen_selection))
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
                c = (int8_t)get_detected_key();

                if (0 == c)
                {
                    c = -1;
                }
			}

            if ((MENU_EXIT_CRTL == c) || (CONNECTION_ABORT_CRTL == c))
            {
                vTaskDelay(100);
                break;
            }
            start_key_check();
        }
    }

    xEventGroupClearBits (g_update_console_event, STATUS_DISPLAY_MENU_KIS);
    return (0);
}
/**********************************************************************************************************************
 End of function kis_display_menu
 *********************************************************************************************************************/

