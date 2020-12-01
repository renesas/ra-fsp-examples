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
 * File Name    : board_mon_thread_entry.c
 * Version      : .
 * Description  : Board status monitor thread.
 *********************************************************************************************************************/
#include "board_mon_thread.h"
#include "common_init.h"
#include "common_data.h"
#include "common_utils.h"
#include "usb_console_main.h"
#include "board_cfg.h"

#define BUTTON_DEBOUNCE_RATE (500)

extern bool g_usb_configured;
extern adc_info_t g_adc_info_rtn;

uint32_t g_pwm_dcs[3] =
{ LED_INTENSITY_10, LED_INTENSITY_50, LED_INTENSITY_90 };
uint32_t g_pwm_rates[3] =
{ BLINK_FREQ_1HZ, BLINK_FREQ_5HZ, BLINK_FREQ_10HZ };

st_board_status_t g_board_status =
{ };

uint8_t g_pwm_dcs_data[] =
{ 10, 50, 90 };
uint8_t g_pwm_rates_data[] =
{ 1, 5, 10 };

static EventBits_t s_ux_bits;
static uint16_t s_new_value = 0;
static uint32_t s_temperature_read_interval = 10;

static const TickType_t s_ticks_to_wait = (50 / portTICK_PERIOD_MS);

static uint16_t s_adc_data = 0;
static uint16_t s_old_adc_data = 0;

static char_t s_print_buffer[128] =
{ };

/**********************************************************************************************************************
 * Function Name: test_temperature_change
 * Description  : Read the Temperature and if it is different form the previous reading, trigger an update.
 * Argument     : None
 * Return Value : None
 *********************************************************************************************************************/
static void test_temperature_change()
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    float32_t mcu_temp_f = 0.00;
    float32_t mcu_temp_c = 0.00;
    float32_t tmp = 0.00;
    uint16_t d = 0;
    uint16_t r = 0;

    /* Read die temperature */
    fsp_err = R_ADC_Read (&g_adc_ctrl, ADC_CHANNEL_TEMPERATURE, &s_adc_data);

    /* Handle error */
    if (FSP_SUCCESS != fsp_err)
    {
        print_to_console ("** R_ADC_Read API failed ** \r\n");

        /* Fatal error */
        SYSTEM_ERROR
    }

    g_board_status.adc_temperature_data = s_adc_data;

    /* Read TSN cal data (value written at manufacture, does not change at runtime) */
    if (0xFFFFFFFF == g_adc_info_rtn.calibration_data)
    {
        /* Unable to read TSN cal value - not supported */
        fsp_err = FSP_ERR_UNSUPPORTED;
    }

    if (FSP_SUCCESS == fsp_err)
    {
        /* Cast, as compiler will assume calc is int */
        mcu_temp_c = (float32_t) ((s_adc_data - (((float32_t) g_adc_info_rtn.calibration_data) -
        TSN_CAL_OFFEST_COUNTS_AT_127DEG_TO_0DEG_C)) / TSN_ADC_COVERSION_SLOPE_COUNTS_PER_DEG_C);

        /* Cast, as compiler will assume calc is int */
        mcu_temp_f = (float32_t) ((s_adc_data - (((float32_t) g_adc_info_rtn.calibration_data) -
        TSN_CAL_OFFEST_COUNTS_AT_260_6DEG_TO_0DEG_F)) / TSN_ADC_COVERSION_SLOPE_COUNTS_PER_DEG_F);
    }
    else
    {
        mcu_temp_f = 0.00;
        mcu_temp_c = 0.00;
    }

    /* As sprintf does not support floating point convert result to d.r */
    tmp = mcu_temp_f * 100.0f;

    /* Truncate the mantissa to leave only the integer part */
    d = (uint16_t) (mcu_temp_f / 1.00f);

    /* Truncate the mantissa to leave only the integer part */
    r = (uint16_t) (tmp / 1.00f);

    /* Cast to maintain siz of uint16_t */
    r = (uint16_t) (r % (d * 100U));

    g_board_status.temperature_f.whole_number = d;
    g_board_status.temperature_f.mantissa = r;

    tmp = mcu_temp_c * 100.0f;

    /* Truncate the mantissa to leave only the integer part */
    d = (uint16_t) (mcu_temp_c / 1.00f);

    /* Truncate the mantissa to leave only the integer part */
    r = (uint16_t) (tmp / 1.00f);

    /* Cast to maintain siz of uint16_t */
    r = (uint16_t) (r % (d * 100U));

    g_board_status.temperature_c.whole_number = d;
    g_board_status.temperature_c.mantissa = r;

    if (s_old_adc_data != s_adc_data)
    {
        s_old_adc_data = s_adc_data;
        xEventGroupSetBits (g_update_console_event, STATUS_UPDATE_TEMP_INFO);
    }
}
/**********************************************************************************************************************
 End of function test_temperature_change
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * Function Name: board_mon_thread_entry
 * Description  : .
 * Argument     : pvParameters
 * Return Value : .
 *********************************************************************************************************************/
void board_mon_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);

    uint16_t wn_mcu_temp_f = 0;
    uint16_t fr_mcu_temp_f = 0;
    uint16_t wn_mcu_temp_c = 0;
    uint16_t fr_mcu_temp_c = 0;

    while (1)
    {
        s_ux_bits = xEventGroupWaitBits (g_update_console_event, STATUS_UPDATE_KIS_INFO, pdFALSE, pdTRUE, 1);

        if (true == g_usb_configured)
        {
            if (0 == ( --s_temperature_read_interval))
            {
                s_temperature_read_interval = 10;

                /* Check for change in core temperature */
                /* Event will be set is a change is detected */
                test_temperature_change ();
            }

            if ((s_ux_bits & (STATUS_DISPLAY_MENU_KIS | STATUS_UPDATE_TEMP_INFO))
                    == (STATUS_DISPLAY_MENU_KIS | STATUS_UPDATE_TEMP_INFO))
            {
                wn_mcu_temp_f = g_board_status.temperature_f.whole_number;
                fr_mcu_temp_f = g_board_status.temperature_f.mantissa;
                wn_mcu_temp_c = g_board_status.temperature_c.whole_number;
                fr_mcu_temp_c = g_board_status.temperature_c.mantissa;

                /* Update temperature to display */
                char_t * p_temp_buffer = pvPortMalloc (128);
                sprintf (p_temp_buffer, "%s%s%s%d.%d/%d.%d%s\r\n%s", gp_cursor_store, gp_cursor_temp, gp_green_fg,
                        wn_mcu_temp_f, fr_mcu_temp_f, wn_mcu_temp_c, fr_mcu_temp_c, gp_green_fg, gp_cursor_restore);

                print_to_console (p_temp_buffer);
                vPortFree (p_temp_buffer);
            }

            if ((s_ux_bits & (STATUS_DISPLAY_MENU_KIS | STATUS_UPDATE_INTENSE_INFO))
                    == (STATUS_DISPLAY_MENU_KIS | STATUS_UPDATE_INTENSE_INFO))
            {
                /* Update Switch SW1 */
                s_new_value = g_board_status.led_intensity;

                sprintf (s_print_buffer, "%s%s%s%d%s\r\n%s", gp_cursor_store, gp_cursor_intensity, gp_green_fg,
                        g_pwm_dcs_data[s_new_value], gp_white_fg, gp_cursor_restore);
                print_to_console (s_print_buffer);
            }

            if ((s_ux_bits & (STATUS_DISPLAY_MENU_KIS | STATUS_UPDATE_FREQ_INFO))
                    == (STATUS_DISPLAY_MENU_KIS | STATUS_UPDATE_FREQ_INFO))
            {
                /* Update Switch SW2 */
                s_new_value = g_board_status.led_frequency;

                sprintf (s_print_buffer, "%s%s%s%d%s\r\n%s", gp_cursor_store, gp_cursor_frequency, gp_green_fg,
                        g_pwm_rates_data[s_new_value], gp_white_fg, gp_cursor_restore);
                print_to_console (s_print_buffer);

            }
        }

        if ((s_ux_bits & (STATUS_UPDATE_TEMP_INFO)) == (STATUS_UPDATE_TEMP_INFO))
        {
            xEventGroupClearBits (g_update_console_event, STATUS_UPDATE_TEMP_INFO);
        }

        if ((s_ux_bits & (STATUS_UPDATE_INTENSE_INFO)) == (STATUS_UPDATE_INTENSE_INFO))
        {
            led_duty_cycle_update ();

            /* Clear Event */
            xEventGroupClearBits (g_update_console_event, (STATUS_UPDATE_INTENSE_INFO));
        }

        if ((s_ux_bits & (STATUS_UPDATE_FREQ_INFO)) == (STATUS_UPDATE_FREQ_INFO))
        {
            R_GPT_PeriodSet (g_blinker.p_ctrl, g_pwm_rates[g_board_status.led_frequency]);

            /* Clear Event */
            xEventGroupClearBits (g_update_console_event, (STATUS_UPDATE_FREQ_INFO));
        }

        vTaskDelay (s_ticks_to_wait);
    }
}
/**********************************************************************************************************************
 End of function board_mon_thread_entry
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: button_irq10_callback
 * Description  : SW1 Interrupt handler.
 * Argument     : p_args
 * Return Value : None
 *********************************************************************************************************************/
void button_irq10_callback(external_irq_callback_args_t *p_args)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    BaseType_t xResult = pdFAIL;
    EventBits_t uxBits;

    /* Void the unused args */
    FSP_PARAMETER_NOT_USED(p_args);

    uxBits = xEventGroupGetBitsFromISR (g_update_console_event);

    if ((uxBits & (STATUS_UPDATE_INTENSE_INFO)) != (STATUS_UPDATE_INTENSE_INFO))
    {
        /* Cast, as compiler will assume calc is int */
        g_board_status.led_intensity = (uint16_t) ((g_board_status.led_intensity + 1) % 3);
        xResult = xEventGroupSetBitsFromISR(g_update_console_event, STATUS_UPDATE_INTENSE_INFO,
                                            &xHigherPriorityTaskWoken);

        /* Was the message posted successfully? */
        if (pdFAIL != xResult)
        {
            /* If xHigherPriorityTaskWoken is now set to pdTRUE then a context
             switch should be requested.  The macro used is port specific and will
             be either portYIELD_FROM_ISR() or portEND_SWITCHING_ISR() - refer to
             the documentation page for the port being used. */
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }
}
/**********************************************************************************************************************
 End of function button_irq10_callback
 *********************************************************************************************************************/

/* SW 2 */
/**********************************************************************************************************************
 * Function Name: button_irq11_callback
 * Description  : SW2 interrupt handler.
 * Argument     : p_args
 * Return Value : None
 *********************************************************************************************************************/
void button_irq11_callback(external_irq_callback_args_t *p_args)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    BaseType_t xResult = pdFAIL;
    EventBits_t uxBits;

    /* Void the unused args */
    FSP_PARAMETER_NOT_USED(p_args);

    uxBits = xEventGroupGetBitsFromISR (g_update_console_event);

    if ((uxBits & (STATUS_UPDATE_FREQ_INFO)) != (STATUS_UPDATE_FREQ_INFO))
    {
        /* Cast, as compiler will assume calc is int */
        g_board_status.led_frequency = (uint16_t) ((g_board_status.led_frequency + 1) % 3);
        xResult = xEventGroupSetBitsFromISR(g_update_console_event, STATUS_UPDATE_FREQ_INFO, &xHigherPriorityTaskWoken);

        /* Was the message posted successfully? */
        if (pdFAIL != xResult)
        {
            /* If xHigherPriorityTaskWoken is now set to pdTRUE then a context
             switch should be requested.  The macro used is port specific and will
             be either portYIELD_FROM_ISR() or portEND_SWITCHING_ISR() - refer to
             the documentation page for the port being used. */
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }
}
/**********************************************************************************************************************
 End of function button_irq11_callback
 *********************************************************************************************************************/
