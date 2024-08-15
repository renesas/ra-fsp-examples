/**********************************************************************************************************************
 * File Name    : board_mon_thread_entry.c
 * Version      : .
 * Description  : Board status monitor thread.
 *********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "board_mon_thread.h"
#include "common_utils.h"
#include "common_init.h"
#include "common_data.h"
#include "usb_console_main.h"
#include "board_cfg.h"

#define BUTTON_DEBOUNCE_RATE (500)

extern bool g_usb_configured;

st_board_status_t g_board_status = {};

uint32_t g_pwm_dcs[3] = {LED_INTENSITY_10, LED_INTENSITY_50, LED_INTENSITY_90};
uint32_t g_pwm_rates[3] = {BLINK_FREQ_1HZ, BLINK_FREQ_5HZ, BLINK_FREQ_10HZ};

char_t g_pwm_dcs_data[]   = {10,50, 90};
char_t g_pwm_rates_data[] = {1, 5, 10};

extern adc_info_t  g_adc_info_rtn;

char_t g_print_buffer[BUFFER_LINE_LENGTH]  = {};

static EventBits_t s_ux_bits;
static uint16_t s_new_value      = 0;
static uint32_t s_temperature_read_interval = 10;

static const TickType_t s_ticks_to_wait = (50 / portTICK_PERIOD_MS);

static uint16_t s_adc_data      = 0;
static uint16_t s_old_adc_data  = 0;

/* Constant data for ADC conversion */
#define ADC_EXAMPLE_VCC_MICROVOLT                        (3300000)  // 3.3 Volt reference
#define ADC_EXAMPLE_TEMPERATURE_RESOLUTION               (12U)      // Set ADC to 12-bit resolution
#define ADC_EXAMPLE_REFERENCE_CALIBRATION_TEMPERATURE    (127)      // Reference temperature at calibration

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
    adc_status_t status;

    /* Perform Single scan */
    if (FSP_SUCCESS == fsp_err)
    {
        R_ADC_ScanStart(&g_adc_ctrl);
    }

    if (FSP_SUCCESS == fsp_err)
    {
        R_ADC_StatusGet(&g_adc_ctrl, &status);
        while(ADC_STATE_SCAN_IN_PROGRESS == status.state)
        {
            R_ADC_StatusGet(&g_adc_ctrl, &status);

            /* Free processor for other tasks */
            vTaskDelay(1);
        }
    }

    if (FSP_SUCCESS == fsp_err)
    {
        R_ADC_ScanStop(&g_adc_ctrl);
    }

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
        int32_t reference_calibration_data = (int32_t) g_adc_info_rtn.calibration_data;

        int32_t slope_uv_per_c = BSP_FEATURE_ADC_TSN_SLOPE;
        int32_t v1_uv          = (ADC_EXAMPLE_VCC_MICROVOLT >> ADC_EXAMPLE_TEMPERATURE_RESOLUTION) * reference_calibration_data;
        int32_t vs_uv          = (ADC_EXAMPLE_VCC_MICROVOLT >> ADC_EXAMPLE_TEMPERATURE_RESOLUTION) * s_adc_data;

        float32_t fvs_uv          = (float32_t)vs_uv * (float32_t)1.0;
        float32_t fv1_uv          = (float32_t)v1_uv * (float32_t)1.0;
        float32_t fslope_uv_per_c = (float32_t)slope_uv_per_c * (float32_t)1.0;
        float32_t temperature_cf  = (fvs_uv - fv1_uv) / fslope_uv_per_c + (float32_t)127.0;

        mcu_temp_c = temperature_cf;

        /* Convert °C to °F using formula */
        /* °F = (°C x 9/5) + 32 */
        mcu_temp_f = (temperature_cf * (float32_t)1.8) + (float32_t)32.0;
    }
    else
    {
        mcu_temp_f = 0.00;
        mcu_temp_c = 0.00;
    }

    if (s_old_adc_data != s_adc_data)
    {
        g_board_status.temperature_f_as_f = mcu_temp_f;
        g_board_status.temperature_c_as_f = mcu_temp_c;

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
void board_mon_thread_entry(void * pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);

    while (1)
    {
        s_ux_bits = xEventGroupWaitBits(g_update_console_event,  STATUS_UPDATE_KIS_INFO, pdFALSE, pdTRUE, 1);

        if (true == g_usb_configured)
        {
            if (0 == ( --s_temperature_read_interval))
            {
                s_temperature_read_interval = 10;

                /* Check for change in core temperature */
                /* Event will be set is a change is detected */
                test_temperature_change();
            }

            if ((s_ux_bits & (STATUS_DISPLAY_MENU_KIS | STATUS_UPDATE_TEMP_INFO)) ==
                    (STATUS_DISPLAY_MENU_KIS | STATUS_UPDATE_TEMP_INFO))
            {
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

                /* Fix output as two decimal places */
                size_t len_f = (strcspn(fs, ".") + 3);
                size_t len_c = (strcspn(cs, ".") + 3);

                /* Update temperature to display */
                sprintf(g_print_buffer, "%s%s%s%.*s/%.*s%s  \r\n%s",
                        gp_cursor_store, gp_cursor_temp, gp_green_fg,
                        len_f,fs,len_c,cs,
                        gp_green_fg, gp_cursor_restore );
                print_to_console(g_print_buffer);
            }

            if ((s_ux_bits & (STATUS_DISPLAY_MENU_KIS | STATUS_UPDATE_INTENSE_INFO)) ==
                    (STATUS_DISPLAY_MENU_KIS | STATUS_UPDATE_INTENSE_INFO))
            {
                /* Update Switch SW1 */
                s_new_value = g_board_status.led_intensity;

                sprintf(g_print_buffer, "%s%s%s%d%s\r\n%s",
                        gp_cursor_store, gp_cursor_intensity, gp_green_fg,
                        g_pwm_dcs_data[s_new_value],
                        gp_white_fg, gp_cursor_restore );
                print_to_console(g_print_buffer);
            }

            if ((s_ux_bits & (STATUS_DISPLAY_MENU_KIS | STATUS_UPDATE_FREQ_INFO)) ==
                    (STATUS_DISPLAY_MENU_KIS | STATUS_UPDATE_FREQ_INFO))
            {
                /* Update Switch SW2 */
                s_new_value = g_board_status.led_frequency;

                sprintf(g_print_buffer, "%s%s%s%d%s\r\n%s",
                        gp_cursor_store, gp_cursor_frequency, gp_green_fg,
                        g_pwm_rates_data[s_new_value],
                        gp_white_fg, gp_cursor_restore );
                print_to_console(g_print_buffer);

            }
        }

        if ((s_ux_bits & (STATUS_UPDATE_TEMP_INFO)) == (STATUS_UPDATE_TEMP_INFO))
        {
            /* Clear Event */
            xEventGroupClearBits(g_update_console_event, STATUS_UPDATE_TEMP_INFO);
        }

        if ((s_ux_bits & (STATUS_UPDATE_INTENSE_INFO)) == (STATUS_UPDATE_INTENSE_INFO))
        {
            led_duty_cycle_update();

            /* Clear Event */
            xEventGroupClearBits(g_update_console_event, (STATUS_UPDATE_INTENSE_INFO));
        }

        if ((s_ux_bits & (STATUS_UPDATE_FREQ_INFO)) == (STATUS_UPDATE_FREQ_INFO))
        {
            R_GPT_PeriodSet(g_blinker.p_ctrl, g_pwm_rates[g_board_status.led_frequency]);

            /* Clear Event */
            xEventGroupClearBits(g_update_console_event, (STATUS_UPDATE_FREQ_INFO));
        }

        vTaskDelay(s_ticks_to_wait);
    }
}
/**********************************************************************************************************************
 End of function board_mon_thread_entry
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: button_irq10_ds_callback
 * Description  : SW1 Interrupt handler.
 * Argument     : p_args
 * Return Value : None
 *********************************************************************************************************************/
void button_irq10_ds_callback(external_irq_callback_args_t *p_args)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    BaseType_t xResult = pdFAIL;
    EventBits_t uxBitsl;

    FSP_PARAMETER_NOT_USED(p_args);

    uxBitsl = xEventGroupGetBitsFromISR(g_update_console_event);

    if ((uxBitsl & (STATUS_UPDATE_INTENSE_INFO)) != (STATUS_UPDATE_INTENSE_INFO))
    {
        /* Cast, as compiler will assume calc is int */
        g_board_status.led_intensity = (uint16_t)((g_board_status.led_intensity + 1) % 3);
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
 End of function button_irq10_ds_callback
 *********************************************************************************************************************/

/* SW 2 */
/**********************************************************************************************************************
 * Function Name: button_irq9_callback
 * Description  : SW2 interrupt handler.
 * Argument     : p_args
 * Return Value : None
 *********************************************************************************************************************/
void button_irq9_callback(external_irq_callback_args_t *p_args)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    BaseType_t xResult = pdFAIL;
    EventBits_t uxBitsl;

    FSP_PARAMETER_NOT_USED(p_args);

    uxBitsl = xEventGroupGetBitsFromISR(g_update_console_event);

    if ((uxBitsl & (STATUS_UPDATE_FREQ_INFO)) != (STATUS_UPDATE_FREQ_INFO))
    {
        /* Cast, as compiler will assume calc is int */
        g_board_status.led_frequency = (uint16_t)((g_board_status.led_frequency + 1) % 3);
        xResult = xEventGroupSetBitsFromISR(g_update_console_event, STATUS_UPDATE_FREQ_INFO,
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
 End of function button_irq9_callback
 *********************************************************************************************************************/
