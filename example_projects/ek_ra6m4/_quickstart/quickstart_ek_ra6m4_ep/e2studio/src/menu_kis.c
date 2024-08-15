/**********************************************************************************************************************
 * File Name    : menu_kis.c
 * Version      : .
 * Description  : The Kit information demo.
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

#include <stdio.h>
#include <string.h>

#include "common_init.h"
#include "common_data.h"
#include "common_utils.h"
#include "menu_kis.h"

#define CONNECTION_ABORT_CRTL    (0x00)
#define MENU_EXIT_CRTL           (0x20)

#define SHORT_NAME      "RA6M4"
#define FULL_NAME       ("EK-RA6M4")
#define PART_NUMBER     ("RTK7EKA6M4S00001BE")
#define DEVICE_NUMBER   ("R7FA6M4AF3CFB")

#define MODULE_NAME     "\r\n%d. KIT INFORMATION\r\n"

#define SUB_OPTIONS     "\r\n\x1b[2m\x1b[37m a) Kit name:\t\t\t\t%s "                          \
                        "\r\n\x1b[2m\x1b[37m b) Kit ordering part number:\t\t%s "              \
                        "\r\n\x1b[2m\x1b[37m c) RA MCU part number:\t\t\t%s"                   \
                        "\r\n\x1b[2m\x1b[37m d) RA MCU 128-bit Unique ID (hex):\t\x1b[32m%x%x%x%x\x1b[37m-\x1b[32m%x%x%x%x\x1b[37m-\x1b[32m%x%x%x%x\x1b[37m-\x1b[32m%x%x%x%x\x1b[37m" \
                        "\r\n\x1b[2m\x1b[37m e) RA MCU die temperature (F/C):\t\x1b[32m%d.%d/%d.%d\x1b[37m "  \
                        "\r\n\x1b[2m\x1b[37m f) Blue LED blinking frequency (Hz):\t\x1b[32m%d\x1b[37m "      \
                        "\r\n\x1b[2m\x1b[37m g) Blue LED blinking intensity (%%):\t\x1b[32m%d\x1b[37m "      \

/* Terminal window escape sequences */
static const char * const sp_clear_screen   = "\x1b[2J";
static const char * const sp_cursor_home    = "\x1b[H";

static char print_buffer [BUFFER_LINE_LENGTH] = {};
static const TickType_t xTicksToWait = 100 / portTICK_PERIOD_MS;

/**********************************************************************************************************************
 * Function Name: kis_display_menu
 * Description  : .
 * Return Value : The Kit information demo screen
 *********************************************************************************************************************/
test_fn kis_display_menu(void)
{
	int c = -1;
    uint16_t wn_mcu_temp_f  = 0;
    uint16_t fr_mcu_temp_f  = 0;
    uint16_t wn_mcu_temp_c  = 0;
    uint16_t fr_mcu_temp_c  = 0;
    bsp_unique_id_t const * uid = R_BSP_UniqueIdGet();

	sprintf(print_buffer, "%s%s", sp_clear_screen, sp_cursor_home);
	print_to_console(print_buffer);

	sprintf(print_buffer, MODULE_NAME, g_selected_menu);
	print_to_console(print_buffer);

    wn_mcu_temp_f = g_board_status.temperature_f.whole_number;
    fr_mcu_temp_f = g_board_status.temperature_f.mantissa;
    wn_mcu_temp_c = g_board_status.temperature_c.whole_number;
    fr_mcu_temp_c = g_board_status.temperature_c.mantissa;


	sprintf(print_buffer, SUB_OPTIONS, FULL_NAME, PART_NUMBER, DEVICE_NUMBER,
            uid->unique_id_bytes[0],  uid->unique_id_bytes[1],  uid->unique_id_bytes[2],  uid->unique_id_bytes[3],
            uid->unique_id_bytes[4],  uid->unique_id_bytes[5],  uid->unique_id_bytes[6],  uid->unique_id_bytes[7],
            uid->unique_id_bytes[8],  uid->unique_id_bytes[9],  uid->unique_id_bytes[10], uid->unique_id_bytes[11],
            uid->unique_id_bytes[12], uid->unique_id_bytes[13], uid->unique_id_bytes[14], uid->unique_id_bytes[15],
			wn_mcu_temp_f, fr_mcu_temp_f,
			wn_mcu_temp_c, fr_mcu_temp_c,
			g_pwm_rates_data[g_board_status.led_frequency],
			g_pwm_dcs_data[g_board_status.led_intensity]
			);
	print_to_console(print_buffer);

	sprintf(print_buffer, MENU_RETURN_INFO);
	print_to_console(print_buffer);

	/* provide small delay so board_status should be up to date */
	vTaskDelay(xTicksToWait);
	xEventGroupSetBits(g_update_console_event, STATUS_DISPLAY_MENU_KIS);

    while ((c != CONNECTION_ABORT_CRTL))
    {
    	c = input_from_console();
        if ((c == MENU_EXIT_CRTL) || (c == CONNECTION_ABORT_CRTL))
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


