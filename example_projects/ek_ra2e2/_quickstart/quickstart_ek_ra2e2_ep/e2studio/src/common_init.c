/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name    : common_init.c
 * Description  : Common init function.
 **********************************************************************************************************************/

#include <stdio.h>

#include "r_typedefs.h"
#include "r_console_if.h"

#include "board_cfg.h"
#include "common_init.h"
#include "common_utils.h"

#include "board_cfg.h"

/*setting NUM_RATES as the size of the array s_pwm_rates*/
#define NUM_RATES             (sizeof(s_pwm_rates) / sizeof(s_pwm_rates[0]))
/*setting NUM_DCS as the size of the array s_pwm_dcs*/
#define NUM_DCS               (sizeof(s_pwm_dcs)   / sizeof(s_pwm_dcs[0]))
/*setting NUM_SWITCH as the size of the array s_pwm_pins*/
#define NUM_SWITCH            (sizeof(s_irq_pins)  / sizeof(s_irq_pins[0]))

typedef struct irq_pins
{
    const external_irq_instance_t * const p_irq;
} st_irq_pins_t;


/* Terminal window escape sequences */
const char_t * const gp_clear_screen   = "\x1b[2J";
const char_t * const gp_cursor_home    = "\x1b[H";

const char_t * const gp_cursor_store     = "\x1b[s";
const char_t * const gp_cursor_restore   = "\x1b[u";
const char_t * const gp_cursor_temp      = "\x1b[7;41H\x1b[K";
const char_t * const gp_cursor_frequency = "\x1b[8;41H\x1b[K";
const char_t * const gp_cursor_intensity = "\x1b[9;41H\x1b[K";

const char_t * const gp_green_fg   = "\x1B[2;32m";
const char_t * const gp_white_fg   = "\x1B[2;37m";


extern bsp_leds_t g_bsp_leds;

st_board_status_t g_board_status;
uint8_t g_pwm_dcs_data[]   = {10, 50, 90};
uint8_t g_pwm_rates_data[] = {1, 5, 10};

/* 111 IRQ4_A & 205 IRQ1*/
static st_irq_pins_t s_irq_pins[] =
{
        {&g_external_irq1},
        {&g_external_irq4},
};

static uint32_t s_pwm_dcs[3] = {LED_INTENSITY_10, LED_INTENSITY_50, LED_INTENSITY_90};
static uint32_t s_pwm_rates[3] = {BLINK_FREQ_1HZ, BLINK_FREQ_5HZ, BLINK_FREQ_10HZ};

static uint32_t s_blueled_flashing = OFF;
static uint32_t s_intense = 0;
static uint32_t s_duty = 1;

/* Define the units to be used with the software delay function */
static const bsp_delay_units_t s_bsp_delay_units = BSP_DELAY_UNITS_MILLISECONDS;



/**********************************************************************************************************************
 * Function Name: adc_initialize
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static fsp_err_t adc_initialize(void)
{
    adc_status_t status;

    fsp_err_t fsp_err = FSP_SUCCESS;
    fsp_err           = R_ADC_Open(&g_adc_ctrl, &g_adc_cfg);

    if (FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }


#if BSP_FEATURE_ADC_CALIBRATION_REG_AVAILABLE
    fsp_err = R_ADC_Calibrate (&g_adc_ctrl, NULL);
    if (FSP_SUCCESS != fsp_err)
        return fsp_err;
#endif


    while ((R_ADC_StatusGet(&g_adc_ctrl, &status) == FSP_SUCCESS) && (ADC_STATE_IDLE != status.state))
    {
        R_BSP_SoftwareDelay(20, s_bsp_delay_units);
    }

    fsp_err = R_ADC_ScanCfg(&g_adc_ctrl, &g_adc_channel_cfg);

    if (FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }

    fsp_err = R_ADC_ScanStart(&g_adc_ctrl);

    if (FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }

    return fsp_err;
}
/**********************************************************************************************************************
 End of function adc_initialize
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: icu_initialize
 * Description  : .
 * Return Value : Error/Success result
 *********************************************************************************************************************/
static fsp_err_t icu_initialize(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    for (uint32_t i = 0; i < NUM_SWITCH; i++ )
    {
        fsp_err = R_ICU_ExternalIrqOpen(s_irq_pins[i].p_irq->p_ctrl, s_irq_pins[i].p_irq->p_cfg);

        if (FSP_SUCCESS != fsp_err)
        {
            return fsp_err;
        }

        fsp_err = R_ICU_ExternalIrqEnable(s_irq_pins[i].p_irq->p_ctrl);

        if (FSP_SUCCESS != fsp_err)
        {
            return fsp_err;
        }
    }

    return fsp_err;
}
/**********************************************************************************************************************
 End of function icu_initialize
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: gpt_initialize
 * Description  : .
 * Return Value : Error/Success result
 *********************************************************************************************************************/
static fsp_err_t gpt_initialize(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    fsp_err = R_AGT_Open (g_periodic_timer.p_ctrl, g_periodic_timer.p_cfg);
    if (fsp_err)
    {
        SYSTEM_ERROR
        return fsp_err;
    }

    fsp_err = R_AGT_Open (g_agt_blue.p_ctrl, g_agt_blue.p_cfg);
    if (fsp_err)
    {
        SYSTEM_ERROR
        return fsp_err;
    }
    return fsp_err;
}
/**********************************************************************************************************************
 End of function gpt_initialize
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: common_init
 * Description  : .
 * Return Value : Error/Success result
 *********************************************************************************************************************/
fsp_err_t common_init(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    if (STATUS_GET(STATUS_UPDATE_TEMP_INFO))
    {
        STATUS_SET(STATUS_UPDATE_INTENSE_INFO);
    }


    fsp_err = adc_initialize();

    if (FSP_SUCCESS != fsp_err)
    {
        SYSTEM_ERROR
        return fsp_err;
    }

    fsp_err = icu_initialize();

    if (FSP_SUCCESS != fsp_err)
    {
        SYSTEM_ERROR
        return fsp_err;
    }

    fsp_err = gpt_initialize();

    if (FSP_SUCCESS != fsp_err)
    {
        SYSTEM_ERROR
        return fsp_err;
    }

    /* Set baseline die temperature */
    fsp_err = R_ADC_Read (&g_adc_ctrl, ADC_CHANNEL_TEMPERATURE, &g_board_status.adc_temperature_data);

    /* Handle error */
    if (FSP_SUCCESS != fsp_err)
    {
        print_to_console ("** R_ADC_Read API failed ** \r\n");

        SYSTEM_ERROR
    }

    /* Set baseline LED status */
    g_board_status.led_intensity = 0;
    g_board_status.led_frequency = 0;
    R_AGT_PeriodSet(g_periodic_timer.p_ctrl, s_pwm_rates[g_board_status.led_frequency]);

    /* Start the timers */
    R_AGT_Start(g_periodic_timer.p_ctrl);
    R_AGT_Start(g_agt_blue.p_ctrl);

    return fsp_err;
}
/**********************************************************************************************************************
 End of function common_init
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: print_to_console
 * Description  : .
 * Argument     : p_data contains address of buffer to be printed
 * Return Value : Error/Success result
 *********************************************************************************************************************/
fsp_err_t print_to_console(char_t *p_data)
{
    printf(p_data);
    return (0);
}
/**********************************************************************************************************************
 End of function print_to_console
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: check_for_input_from_console
 * Description  : .
 * Return Value : Error/Success result
 *********************************************************************************************************************/
bool_t check_for_input_from_console(void)
{
    if (STATUS_GET(STATUS_DISPLAY_MENU_KIS) == 0)
    {
        STATUS_CLEAR(STATUS_UPDATE_FREQ_INFO);
        STATUS_CLEAR(STATUS_UPDATE_INTENSE_INFO);
    }

    return (R_CONSOLE_IsInputReady());
}
/**********************************************************************************************************************
 End of function check_for_input_from_console
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: input_from_console
 * Description  : Gets single byte input from console, use function check_for_input_from_console()
 *                to confirm input is available
 * Return Value : Read char
 *********************************************************************************************************************/
int8_t input_from_console(void)
{
    char_t read_byte_buffer;

    R_CONSOLE_ReadByte(&read_byte_buffer);
    return (read_byte_buffer);
}
/**********************************************************************************************************************
 End of function input_from_console
 *********************************************************************************************************************/

/** External IRQ for SW 1 on ICU Instance. */
/**********************************************************************************************************************
 * Function Name: button_irq4_callback
 * Description  : .
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void button_irq4_callback(external_irq_callback_args_t *p_args)
{
    /* Cast unused params to void */
    FSP_PARAMETER_NOT_USED(p_args);

    if (STATUS_GET(STATUS_UPDATE_INTENSE_INFO) == 0)
    {
        /* Cast calculation which compiler may perform as int */
        g_board_status.led_intensity = (uint16_t)((g_board_status.led_intensity + 1) % 3);
        STATUS_SET(STATUS_UPDATE_INTENSE_INFO);
    }
}
/**********************************************************************************************************************
 End of function button_irq4_callback
 *********************************************************************************************************************/

/** External IRQ for SW 2 on ICU Instance. */
/**********************************************************************************************************************
 * Function Name: button_irq1_callback
 * Description  : .
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void button_irq1_callback(external_irq_callback_args_t *p_args)
{
    /* Cast unused params to void */
    FSP_PARAMETER_NOT_USED(p_args);

    if (STATUS_GET(STATUS_UPDATE_FREQ_INFO) == 0)
    {
        /* Cast to uint16_6, as compiler will assume int for the calculation */
        g_board_status.led_frequency = (uint16_t)((g_board_status.led_frequency + 1) % 3);
        R_AGT_PeriodSet(g_periodic_timer.p_ctrl, s_pwm_rates[g_board_status.led_frequency]);
        STATUS_SET(STATUS_UPDATE_FREQ_INFO);
    }
}
/**********************************************************************************************************************
 End of function button_irq1_callback
 *********************************************************************************************************************/


/** Callback function for driver g_periodic_timer. */
/**********************************************************************************************************************
 * Function Name: periodic_timer_callback
 * Description  : .
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void periodic_timer_callback(timer_callback_args_t *p_args)
{
    /* Cast unused params to void */
    FSP_PARAMETER_NOT_USED(p_args);

    if (OFF == s_blueled_flashing)
    {
        s_blueled_flashing = ON;
    }
    else
    {
        s_blueled_flashing = OFF;
    }
}
/**********************************************************************************************************************
 End of function periodic_timer_callback
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: agt_blue_callback
 * Description  : .
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void agt_blue_callback(timer_callback_args_t *p_args)
{
    /* Cast unused params to void */
    FSP_PARAMETER_NOT_USED(p_args);

    switch (s_blueled_flashing)
    {
        case ON:
        {
            if ((s_intense++ ) < s_duty)
            {
                TURN_BLUE_ON
            }
            else
            {
                TURN_BLUE_OFF
            }

            if ((s_intense) >= 100)
            {
                s_intense = 0;
                s_duty    = s_pwm_dcs[g_board_status.led_intensity];
            }
            break;
        }
        default:
        {
            TURN_BLUE_OFF
            s_intense = 0;
            s_duty = s_pwm_dcs[g_board_status.led_intensity];
        }
    }
}
/**********************************************************************************************************************
 End of function agt_blue_callback
 *********************************************************************************************************************/
