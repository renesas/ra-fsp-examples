/**********************************************************************************************************************
 * File Name    : menu_kis.c
 * Version      : .
 * Description  : The Kit information demo.
 *********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright [2020] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
 * DISCLAIMER
 * This software and documentation are supplied by Renesas Electronics America Inc. and may only be used with products
 * of Renesas Electronics Corp. and its affiliates ("Renesas").  No other uses are authorized.  Renesas products are
 * sold pursuant to Renesas terms and conditions of sale.  Purchasers are solely responsible for the selection and use
 * of Renesas products and Renesas assumes no liability.  No license, express or implied, to any intellectual property
 * right is granted by Renesas. This software is protected under all applicable laws, including copyright laws. Renesas
 * reserves the right to change or discontinue this software and/or this documentation. THE SOFTWARE AND DOCUMENTATION
 * IS DELIVERED TO YOU "AS IS," AND RENESAS MAKES NO REPRESENTATIONS OR WARRANTIES, AND TO THE FULLEST EXTENT
 * PERMISSIBLE UNDER APPLICABLE LAW, DISCLAIMS ALL WARRANTIES, WHETHER EXPLICITLY OR IMPLICITLY, INCLUDING WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT, WITH RESPECT TO THE SOFTWARE OR
 * DOCUMENTATION.  RENESAS SHALL HAVE NO LIABILITY ARISING OUT OF ANY SECURITY VULNERABILITY OR BREACH.  TO THE MAXIMUM
 * EXTENT PERMITTED BY LAW, IN NO EVENT WILL RENESAS BE LIABLE TO YOU IN CONNECTION WITH THE SOFTWARE OR DOCUMENTATION
 * (OR ANY PERSON OR ENTITY CLAIMING RIGHTS DERIVED FROM YOU) FOR ANY LOSS, DAMAGES, OR CLAIMS WHATSOEVER, INCLUDING,
 * WITHOUT LIMITATION, ANY DIRECT, CONSEQUENTIAL, SPECIAL, INDIRECT, PUNITIVE, OR INCIDENTAL DAMAGES; ANY LOST PROFITS,
 * OTHER ECONOMIC DAMAGE, PROPERTY DAMAGE, OR PERSONAL INJURY; AND EVEN IF RENESAS HAS BEEN ADVISED OF THE POSSIBILITY
 * OF SUCH LOSS, DAMAGES, CLAIMS OR COSTS.
 **********************************************************************************************************************/

#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "FreeRTOSconfig.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"

#include "common_init.h"
#include "common_data.h"
#include "common_utils.h"
#include "menu_kis.h"

#define CONNECTION_ABORT_CRTL    (0x00)
#define MENU_EXIT_CRTL           (0x20)

#define MODULE_NAME     "\r\n%d. KIT INFORMATION\r\n"

#define SUB_OPTIONS     "\r\n\x1b[2m\x1b[37m a) Kit name:\t\t\t\t%s "                                        \
                        "\r\n\x1b[2m\x1b[37m b) Kit ordering part number:\t\t%s "                            \
                        "\r\n\x1b[2m\x1b[37m c) RA Device part number:\t\t%s"                                \
                        "\r\n\x1b[2m\x1b[37m d) RA MCU 128-bit Unique ID (hex):"                             \
                        "\t\x1b[32m%x%x%x%x\x1b[37m-\x1b[32m%x%x%x%x\x1b[37m-"                               \
                        "\x1b[32m%x%x%x%x\x1b[37m-\x1b[32m%x%x%x%x\x1b[37m"                                  \
                        "\r\n\x1b[2m\x1b[37m e) RA MCU die temperature (F/C):\t\x1b[32m%d.%d/%d.%d\x1b[37m " \
                        "\r\n\x1b[2m\x1b[37m f) Blue LED blinking frequency (Hz):\t\x1b[32m%d\x1b[37m "      \
                        "\r\n\x1b[2m\x1b[37m g) Blue LED blinking intensity (%%):\t\x1b[32m%d\x1b[37m "      \


static char_t s_print_buffer[BUFFER_LINE_LENGTH] =
{ };
static const TickType_t s_ticks_to_wait = 100 / portTICK_PERIOD_MS;

/**********************************************************************************************************************
 * Function Name: kis_display_menu
 * Description  : .
 * Return Value : The Kit information demo screen
 *********************************************************************************************************************/
test_fn kis_display_menu(void)
{
    int8_t c = -1;
    uint16_t wn_mcu_temp_f = 0;
    uint16_t fr_mcu_temp_f = 0;
    uint16_t wn_mcu_temp_c = 0;
    uint16_t fr_mcu_temp_c = 0;
    bsp_unique_id_t const * p_uid = R_BSP_UniqueIdGet ();

    sprintf (s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);
    print_to_console (s_print_buffer);

    sprintf (s_print_buffer, MODULE_NAME, g_selected_menu);
    print_to_console (s_print_buffer);

    wn_mcu_temp_f = g_board_status.temperature_f.whole_number;
    fr_mcu_temp_f = g_board_status.temperature_f.mantissa;
    wn_mcu_temp_c = g_board_status.temperature_c.whole_number;
    fr_mcu_temp_c = g_board_status.temperature_c.mantissa;

    sprintf (s_print_buffer, SUB_OPTIONS, FULL_NAME, PART_NUMBER, DEVICE_NUMBER, p_uid->unique_id_bytes[0],
            p_uid->unique_id_bytes[1], p_uid->unique_id_bytes[2], p_uid->unique_id_bytes[3], p_uid->unique_id_bytes[4],
            p_uid->unique_id_bytes[5], p_uid->unique_id_bytes[6], p_uid->unique_id_bytes[7], p_uid->unique_id_bytes[8],
            p_uid->unique_id_bytes[9], p_uid->unique_id_bytes[10], p_uid->unique_id_bytes[11],
            p_uid->unique_id_bytes[12], p_uid->unique_id_bytes[13], p_uid->unique_id_bytes[14],
            p_uid->unique_id_bytes[15], wn_mcu_temp_f, fr_mcu_temp_f, wn_mcu_temp_c, fr_mcu_temp_c,
            g_pwm_rates_data[g_board_status.led_frequency], g_pwm_dcs_data[g_board_status.led_intensity]);
    print_to_console (s_print_buffer);

    sprintf (s_print_buffer, MENU_RETURN_INFO);
    print_to_console (s_print_buffer);

    /* provide small delay so board_status should be up to date */
    vTaskDelay (s_ticks_to_wait);
    xEventGroupSetBits (g_update_console_event, STATUS_DISPLAY_MENU_KIS);

    while (CONNECTION_ABORT_CRTL != c)
    {
        c = input_from_console ();
        if ((MENU_EXIT_CRTL == c) || (CONNECTION_ABORT_CRTL == c))
        {
            break;
        }
    }

    xEventGroupClearBits (g_update_console_event, STATUS_DISPLAY_MENU_KIS);
    return (0);
}
/**********************************************************************************************************************
 End of function kis_display_menu
 *********************************************************************************************************************/

