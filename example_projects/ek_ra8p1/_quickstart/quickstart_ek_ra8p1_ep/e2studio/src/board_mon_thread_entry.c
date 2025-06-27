/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : board_mon_thread_entry.c
 * Version      : 1.0
 * Description  : Board status monitor thread.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 01.01.2018 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "board_mon_thread.h"
#include "common_init.h"
#include "common_data.h"
#include "common_utils.h"
#include "board_cfg.h"
#include "board_hw_cfg.h"

#include "menu_main.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define BUTTON_DEBOUNCE_RATE (500)

/* Constant data for ADC conversion */
#define ADC_EXAMPLE_VCC_MICROVOLT                       (3300000)   /* 3.3 Volt reference                   */
#define ADC_EXAMPLE_TEMPERATURE_RESOLUTION              (12U)       /* Set ADC to 12-bit resolution         */
#define ADC_EXAMPLE_REFERENCE_CALIBRATION_TEMPERATURE   (105.0)     /* Reference temperature at calibration */
#define TEMPERATURE_SENSOR_SLOPE_MICROVOLT_PER_DEGREE   (2700)      /* temperature sensor slope             */

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

bool g_usb_configured = true;

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


bool_t g_sw1_updated = false;
bool_t g_sw2_updated = false;

/**********************************************************************************************************************
 Private (static) variables
 *********************************************************************************************************************/
static EventBits_t s_ux_bits;

static uint32_t s_temperature_read_interval = 10;

static const TickType_t s_ticks_to_wait = (50 / portTICK_PERIOD_MS);

static uint16_t s_adc_data     = 0;
static uint16_t s_old_adc_data = 0;

static void test_temperature_change();


/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: board_mon_thread_entry
 * Description  : Board Monitor entry function
 *                Note: pvParameters contains TaskHandle_t
 * Argument     : pvParameters - unused but required by I/F
 * Return Value : NONE
 *********************************************************************************************************************/
void board_mon_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);

    while (1)
    {
        s_ux_bits = xEventGroupWaitBits (g_update_console_event, STATUS_UPDATE_KIS_INFO, pdFALSE, pdTRUE, 1);

        if (true == g_usb_configured)
        {
            if (0 == (--s_temperature_read_interval))
            {
                s_temperature_read_interval = 10;

                /* Check for change in core temperature */
                /* Event will be set is a change is detected */
                test_temperature_change ();
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
            R_GPT_PeriodSet (g_blinker_blue.p_ctrl, g_pwm_rates[g_board_status.led_frequency]);

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
 * Function Name: button_irq12_ds_callback
 * Description  : SW1 Interrupt handler.
 * Argument     : p_args
 * Return Value : None
 *********************************************************************************************************************/
void button_irq12_ds_callback(external_irq_callback_args_t *p_args)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    BaseType_t xResult = pdFAIL;
    EventBits_t uxBits;
    bool_t update = false;

    /* Void the unused args */
    FSP_PARAMETER_NOT_USED(p_args);

    uxBits = xEventGroupGetBitsFromISR (g_update_console_event);

    if ((uxBits & (STATUS_UPDATE_INTENSE_INFO)) != (STATUS_UPDATE_INTENSE_INFO))
    {
        /* do not process switch if we are in KIS and waiting to update console */
        if((get_selected_menu_demo() != LCD_FULL_BG_USER_LED))
        {
            /* update only under the following conditions
             * menu KIS in use and switch de-bounced
             * or
             * menu KIS is not in use
             */
            if(get_selected_menu_demo() == LCD_FULL_BG_KIT_INFORMATION)
            {
                if(g_sw2_updated == false)
                {
                    update = true;
                }
            }
            else
            {
                update = true;
            }

            /* Only update when led demo is not in use and de-bounced  */
            if(update == true)
            {
                g_sw2_updated = true;

                /* Cast, as compiler will assume calc is int */
                g_board_status.led_frequency = (uint16_t) ((g_board_status.led_frequency + 1) % 3);

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
    }
}
/**********************************************************************************************************************
 End of function button_irq12_ds_callback
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: button_irq13_ds_callback
 * Description  : SW2 interrupt handler.
 * Argument     : p_args
 * Return Value : None
 *********************************************************************************************************************/
void button_irq13_ds_callback(external_irq_callback_args_t *p_args)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    BaseType_t xResult = pdFAIL;
    EventBits_t uxBits;
    bool_t update = false;

    /* Void the unused args */
    FSP_PARAMETER_NOT_USED(p_args);

    uxBits = xEventGroupGetBitsFromISR (g_update_console_event);

    if ((uxBits & (STATUS_UPDATE_FREQ_INFO)) != (STATUS_UPDATE_FREQ_INFO))
    {
        /* do not process switch if we are in KIS and waiting to update console */
        if((get_selected_menu_demo() != LCD_FULL_BG_USER_LED))
        {
            /* update only under the following conditions
             * menu KIS in use and switch de-bounced
             * or
             * menu KIS is not in use
             */
            if(get_selected_menu_demo() == LCD_FULL_BG_KIT_INFORMATION)
            {
                if(g_sw1_updated == false)
                {
                    update = true;
                }
            }
            else
            {
                update = true;
            }

            /* Only update when led demo is not in use and de-bounced  */
            if(update == true)
            {
                g_sw1_updated = true;

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
    }
}
/**********************************************************************************************************************
 End of function button_irq13_ds_callback
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private (static) functions
 *********************************************************************************************************************/
 
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
    fsp_err = R_ADC_B_Read (&g_adc_ctrl, ADC_CHANNEL_TEMPERATURE, &s_adc_data);

    /* Handle error */
    if (FSP_SUCCESS != fsp_err)
    {
        print_to_console("** R_ADC_Read API failed ** \r\n");

        /* Fatal error */
        SYSTEM_ERROR_CFG_
    }

    g_board_status.adc_temperature_data = s_adc_data;

    /* Read TSN calibration data (value written at manufacture, does not change at runtime) */
    if (ADC_CALIBRATION_NA == g_adc_info_rtn.calibration_data)
    {
        /* Unable to read TSN calibration value - not supported */
        fsp_err = FSP_ERR_UNSUPPORTED;

        g_board_status.temperature_f.whole_number = 0;
        g_board_status.temperature_f.mantissa     = 0;

        g_board_status.temperature_c.whole_number = 0;
        g_board_status.temperature_c.mantissa     = 0;
    }
    else
    {
        int32_t reference_calibration_data = (int32_t) g_adc_info_rtn.calibration_data;

        int32_t slope_uv_per_c    = TEMPERATURE_SENSOR_SLOPE_MICROVOLT_PER_DEGREE;          // g_adc_info_rtn.slope_microvolts;

        /* calibration temperature sample voltage */
        int32_t v1_uv             = (ADC_EXAMPLE_VCC_MICROVOLT >> ADC_EXAMPLE_TEMPERATURE_RESOLUTION) * reference_calibration_data;

		/* current temperature sample voltage */
        int32_t vs_uv             = (ADC_EXAMPLE_VCC_MICROVOLT >> ADC_EXAMPLE_TEMPERATURE_RESOLUTION) * s_adc_data;

        float32_t fvs_uv          = (float32_t) vs_uv * (float32_t) 1.0;
        float32_t fv1_uv          = (float32_t) v1_uv * (float32_t) 1.0;
        float32_t fslope_uv_per_c = (float32_t)slope_uv_per_c * (float32_t)1.0;
        float32_t temperature_cf  = ((fvs_uv - fv1_uv) / fslope_uv_per_c) + (float32_t) ADC_EXAMPLE_REFERENCE_CALIBRATION_TEMPERATURE;

        mcu_temp_c = temperature_cf;

        /* Convert celsius to fahrenheit using formula */
        /* fahrenheit = (celsius x 9/5) + 32 */
        mcu_temp_f = (temperature_cf * (float32_t) 1.8) + (float32_t) 32.0;

        /* As sprintf does not support floating point convert result to d.r */
        tmp = mcu_temp_f * 100.0f;

        /* Truncate the whole number to leave only the integer part */
        d = (uint16_t) (mcu_temp_f / 1.00f);

        /* Truncate the mantissa to leave only the integer part */
        r = (uint16_t) (tmp / 1.00f);

        /* Cast to maintain size of uint16_t */
        r = (uint16_t) (r % (d * 100U));

        g_board_status.temperature_f.whole_number = d;
        g_board_status.temperature_f.mantissa     = r;

        tmp = mcu_temp_c * 100.0f;

        /* Truncate the whole number to leave only the integer part */
        d = (uint16_t) (mcu_temp_c / 1.00f);

        /* Truncate the mantissa to leave only the integer part */
        r = (uint16_t) (tmp / 1.00f);

        /* Cast to maintain size of uint16_t */
        r = (uint16_t) (r % (d * 100U));

        g_board_status.temperature_c.whole_number = d;
        g_board_status.temperature_c.mantissa     = r;
    }

    /* If there's a change in temperature then update the UI */
    if (s_old_adc_data != s_adc_data)
    {
        s_old_adc_data = s_adc_data;

        /* Event driven update no longer used as KIS screen is constly updated */
//        xEventGroupSetBits (g_update_console_event, STATUS_UPDATE_TEMP_INFO);
    }
}
/**********************************************************************************************************************
 End of function test_temperature_change
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
