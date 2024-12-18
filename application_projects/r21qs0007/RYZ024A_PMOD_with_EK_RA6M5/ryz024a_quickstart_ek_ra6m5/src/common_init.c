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
#define COMMON_INIT_C

#include "common_init.h"

#define NUM_RATES             (sizeof(pwm_rates) / sizeof(pwm_rates[0]))   /*  */
#define NUM_DCS               (sizeof(pwm_dcs) / sizeof(pwm_dcs[0]))       /*  */
#define NUM_SWITCH            (sizeof(s_irq_pins) / sizeof(s_irq_pins[0])) /*  */

/* Enable SW toggling of the LED. ie. PWM via interrupt handler */
#define USE_SW_TIMER          (1)

typedef struct irq_pins
{
    const external_irq_instance_t * const p_irq;
} st_irq_pins_t;

typedef struct pwm_pins
{
    const timer_instance_t * const p_timer;
    const gpt_io_pin_t pin;
} st_pwm_pins_t;

const char_t * const gp_cursor_store = "\x1b[s";
const char_t * const gp_cursor_restore = "\x1b[u";
const char_t * const gp_cursor_temp = "\x1b[8;41H\x1b[K";
const char_t * const gp_cursor_frequency = "\x1b[9;41H\x1b[K";
const char_t * const gp_cursor_intensity = "\x1b[10;41H\x1b[K";

const char_t * const gp_green_fg = "\x1b[32m";
const char_t * const gp_white_fg = "\x1b[37m";

const char_t * const gp_clear_screen = "\x1b[2J";
const char_t * const gp_cursor_home = "\x1b[H";

int32_t g_curr_led_freq = BLINK_FREQ_1HZ;
adc_info_t g_adc_info_rtn;


static volatile uint32_t s_blueled_flashing = OFF;
static uint32_t s_intense = 0;
static uint32_t s_duty = 1;


static st_irq_pins_t s_irq_pins[] =
{
    { &g_external_irq10 },
    { &g_external_irq9 }
};

static st_pwm_pins_t s_pwm_pins[] =
{
    { &g_gpt_blue, GPT_IO_PIN_GTIOCA }
};

/**********************************************************************************************************************
 * Function Name: adc_initialize
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static fsp_err_t adc_initialize(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    fsp_err = R_ADC_Open (&g_adc_ctrl, &g_adc_cfg);
    if (FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }

    fsp_err = R_ADC_ScanCfg (&g_adc_ctrl, &g_adc_channel_cfg);
    if (FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }

    fsp_err = R_ADC_ScanStart (&g_adc_ctrl);
    if (FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }

    /* Read TSN cal data (value written at manufacture, does not change at runtime) */
    fsp_err = R_ADC_InfoGet (&g_adc_ctrl, &g_adc_info_rtn);

    return fsp_err;
}
/**********************************************************************************************************************
 End of function adc_initialize
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: icu_initialize
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static fsp_err_t icu_initialize(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    for (uint32_t i = 0; i < NUM_SWITCH; i++ )
    {
        fsp_err = R_ICU_ExternalIrqOpen (s_irq_pins[i].p_irq->p_ctrl, s_irq_pins[i].p_irq->p_cfg);
        if (FSP_SUCCESS != fsp_err)
        {
            return fsp_err;
        }

        fsp_err = R_ICU_ExternalIrqEnable (s_irq_pins[i].p_irq->p_ctrl);
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
 * Return Value : .
 *********************************************************************************************************************/
static fsp_err_t gpt_initialize(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    for (uint32_t i = 0; i < 1; i++ )
    {
        fsp_err = R_GPT_Open (s_pwm_pins[i].p_timer->p_ctrl, s_pwm_pins[i].p_timer->p_cfg);
        if (FSP_SUCCESS != fsp_err)
        {
            return fsp_err;
        }
    }

    fsp_err = R_GPT_Open (g_blinker.p_ctrl, g_blinker.p_cfg);
    {
        if (FSP_SUCCESS != fsp_err)
        {
            return fsp_err;
        }
    }


    fsp_err = R_GPT_Open (g_gpt_blue.p_ctrl, g_gpt_blue.p_cfg);
    if (fsp_err)
    {
        SYSTEM_ERROR
        return fsp_err;
    }

    fsp_err = R_GPT_Start (g_blinker.p_ctrl);

    if (FSP_SUCCESS != fsp_err)
    {
        /* Fatal error */
        SYSTEM_ERROR

        /* Close the GPT timer */
        R_GPT_Close (g_blinker.p_ctrl);

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
 * Return Value : .
 *********************************************************************************************************************/
fsp_err_t common_init(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    fsp_err = adc_initialize ();
    if (FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }

    fsp_err = icu_initialize ();
    if (FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }

    fsp_err = gpt_initialize ();
#if 1

    if (FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }

    led_duty_cycle_update ();
#endif

    /* Set baseline LED status */
    g_board_status.led_intensity = 0;
    g_board_status.led_frequency = 0;

    R_GPT_PeriodSet(g_blinker.p_ctrl, g_pwm_rates[g_board_status.led_frequency]);

    led_duty_cycle_update ();
    s_duty = g_pwm_dcs[g_board_status.led_intensity];

    /* Start the timers */
    R_GPT_Start(g_blinker.p_ctrl);
    R_GPT_Start(g_gpt_blue.p_ctrl);

    return fsp_err;
}
/**********************************************************************************************************************
 End of function common_init
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: led_duty_cycle_update
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void led_duty_cycle_update(void)
{
    R_GPT_DutyCycleSet (s_pwm_pins[0].p_timer->p_ctrl, g_pwm_dcs[g_board_status.led_intensity], s_pwm_pins[0].pin);
    s_duty = g_pwm_dcs[g_board_status.led_intensity];
}
/**********************************************************************************************************************
 End of function led_duty_cycle_update
 *********************************************************************************************************************/

/*
 * Defined to select whether this project will
 * a. PWM the LED by direct connection to a GPT compare output pin,
 *    where the board is tracked to support this.
 * b. The GPT compare to raise an interrupt. Then the LED is toggled in SW within the interrupt handler
 *    (gpt_blue_callback). For where the LED is not directly tracked to the GPT compare output pin.
 */
#ifdef USE_SW_TIMER

/**********************************************************************************************************************
 * Function Name: periodic_timer_callback
 * Description  : Callback function for driver g_periodic_timer.
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void gpt_blinker_callback(timer_callback_args_t *p_args)
{
    /* Void the unused params */
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
 * Function Name: gpt_blue_callback
 * Description  : Callback function for driver g_gpt_blue.
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void gpt_blue_callback(timer_callback_args_t * p_args)
{
    /* Void the unused params */
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

            if (s_intense >= 100)
            {
                s_intense = 0;
                s_duty = g_pwm_dcs[g_board_status.led_intensity];
            }
            break;
        }
        default:
        {
            TURN_BLUE_OFF
            s_intense = 0;
            s_duty = g_pwm_dcs[g_board_status.led_intensity];
        }
    }
}
/**********************************************************************************************************************
 End of function gpt_blue_callback
 *********************************************************************************************************************/


/* Callback required if using direct HW control of LED, but GPT compare o/p */
#else /* USE_SW_TIMER */

/**********************************************************************************************************************
 * Function Name: gpt_blinker_callback
 * Description  : .
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void gpt_blinker_callback(timer_callback_args_t *p_args)
{
    if (TIMER_EVENT_CYCLE_END == p_args->event)
    {
        timer_status_t status = { 0 };

        R_GPT_StatusGet (s_pwm_pins[0].p_timer->p_ctrl, &status);

        if (TIMER_STATE_COUNTING == status.state)
        {
            R_GPT_Stop (s_pwm_pins[0].p_timer->p_ctrl);
            TURN_BLUE_OFF
        }
        else
        {
            R_GPT_Start (s_pwm_pins[0].p_timer->p_ctrl);
            TURN_BLUE_ON
        }
    }
}
/**********************************************************************************************************************
 End of function gpt_blinker_callback
 *********************************************************************************************************************/
#endif /* USE_SW_TIMER */
