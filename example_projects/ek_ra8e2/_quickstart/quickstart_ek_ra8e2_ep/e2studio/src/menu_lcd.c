/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : menu_lcd.c
 * Version      : 1.0
 * Description  : The lcd demo console and lcd display file. Manages demo interfacce to console and lcd.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 01.01.2025 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <stdarg.h>

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"

#include "tp_thread_entry.h"

#include "common_init.h"
#include "common_utils.h"

#include "jlink_console.h"
#include "menu_lcd.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define MIPI_OFFSET_IMAGE       (0x1c2008)

#define CONNECTION_ABORT_CRTL   (0x00)
#define MENU_EXIT_CRTL          (0x20)

#define MODULE_NAME             "\r\n%d. GUI Demo\r\n\r\n"
#define PRINT_COORDINATES       "TP %d: %4ld, %4ld     \r\n"
#define PRINT_NO_TOUCH          "TP %d:     ,        \r\n"
#define LCD_HELP_TEXT           "A sample image is displayed on the LCD screen.\r\n" \
                                "Touch the screen and the position will be marked by a coloured circle.\r\n" \
                                "Up to Five touch points can be shown, on the display each one will be colored in order of the touch detection.\r\n" \
                                "For each active touch point the position in pixels shall be shown below.\r\n\r\n"

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
static char_t tp_array[5][120] = {};

static const d2_color lcd_demo_clut[] = {
    0x00FF0000,
    0x000000FF,
    0x0000FF00,
    0x00007F7F,
    0x007F00FF
};

void menu_lcd_update_screen(d2_device *d2_handle);

/**********************************************************************************************************************
 * Function Name: lcd_display_menu
 * Description  : The touch screen demonstration presenting up to 5 touch points as squares on the screen..
 * Return Value : Default 0, used for possible expansion.
 *********************************************************************************************************************/
test_fn lcd_display_menu(void)
{
    int8_t c = -1;
    lv_indev_data_t data = {};
    int8_t on_screen_selection = -1;

    sprintf (s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);
    print_to_console((void*)s_print_buffer);

    volatile bool_t mode = true;
    bool_t new_mode = false;

    ts_thread_enable_output(true);

    dsp_set_background(LCD_FULL_BG_USER_LCD);

    new_mode = jlink_set_mode(mode);
    start_key_check();

    while ((CONNECTION_ABORT_CRTL != c))
    {
//        configure_led(LED_NORMAL_UPDATE);

        touchpad_get_copy(&data);
        on_screen_selection = process_menu_detection(&data, false, '4');

        if (key_pressed() || ((on_screen_selection != LCD_FULL_BG_POPUP_MENU) && (on_screen_selection != -1)))
        {
             if ((!g_overlay_selected) && (on_screen_selection != -1) && (on_screen_selection != '4'))
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
                vTaskDelay(100);
                break;
            }

            start_key_check();
        }

        sprintf (s_print_buffer, "%s%s", gp_hide_cursor, gp_cursor_home);
        print_to_console (s_print_buffer);

        sprintf (s_print_buffer, MODULE_NAME, g_selected_menu);
        print_to_console(s_print_buffer);

        print_to_console(LCD_HELP_TEXT);

        uint32_t limit = data.number_of_coordinates;

        for (uint8_t i = 0; i < 5; i++)
        {
            if (i < limit)
            {
                sprintf (tp_array[i], PRINT_COORDINATES, i + 1, data.point[i].x, data.point[i].y);
            }
            else
            {
                sprintf (tp_array[i], PRINT_NO_TOUCH, i + 1);
            }
        }

        sprintf(s_print_buffer,"%s%s%s%s%s", tp_array[0], tp_array[1], tp_array[2], tp_array[3], tp_array[4]);
        print_to_console (s_print_buffer);

        sprintf (s_print_buffer, MENU_RETURN_INFO);
        print_to_console(s_print_buffer);
    }

    jlink_restore_mode(new_mode);

    return (0);
}
/**********************************************************************************************************************
 End of function lcd_display_menu
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * Function Name: menu_lcd_update_screen
 * Description  : Function called by call_user_draw in display_thread_display.c whenever the screen for
 *                LCD_FULL_BG_USER_LCD is required.
 * Argument     : d2_handle - device pointer (see: <d2_opendevice>)
 * Return Value : None.
 *********************************************************************************************************************/
void menu_lcd_update_screen(d2_device *d2_handle)
{
    /* Only if the overlay menu is not displayed */
    if(false == g_overlay_selected)
    {
        lv_indev_data_t data = {};
        touchpad_get_copy(&data);

        uint32_t limit = data.number_of_coordinates;
        for (uint8_t i = 0; i < 5; i++)
        {
            if (i < limit)
            {
                if(((data.point[i].x != 0) || (data.point[i].y != 0)) &&
                    (data.point[i].x < FULL_IMAGE_WIDTH) && (data.point[i].y < FULL_IMAGE_HEIGHT))
                {
                    d2_setcolor(d2_handle, 0, lcd_demo_clut[i]);
                    d2_rendercircle( d2_handle, (d2_point)data.point[i].x << 4,
                            (d2_point)data.point[i].y << 4, 20 << 4, 50 << 4);
                }
            }
        }
    }
}
/**********************************************************************************************************************
 End of function menu_lcd_update_screen
 *********************************************************************************************************************/
