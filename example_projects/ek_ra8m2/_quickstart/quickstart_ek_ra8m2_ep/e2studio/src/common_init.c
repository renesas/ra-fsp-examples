/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/***********************************************************************************************************************
 * File Name    : common_init.c
 * Version      : 1.0
 * Description  : Common init function.
 **********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 14/10/2025 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "common_init.h"
#include "board_hw_cfg.h"
#include "board_i2c_master.h"
#include "board_cfg_switch.h"
#include "board_greenpak.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define NUM_RATES             (sizeof(pwm_rates) / sizeof(pwm_rates[0]))   /*  */
#define NUM_DCS               (sizeof(pwm_dcs) / sizeof(pwm_dcs[0]))       /*  */
#define NUM_SWITCH            (sizeof(s_irq_pins) / sizeof(s_irq_pins[0])) /*  */

#define BLUE_PWM_INDEX      (0)
#define BLUE_PERIODIC_INDEX (1)

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/
typedef struct irq_pins
{
    const external_irq_instance_t * const p_irq;
} st_irq_pins_t;

typedef struct pwm_pins
{
    const timer_instance_t * const p_timer;
    const gpt_io_pin_t             pin;
} st_pwm_pins_t;

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

adc_info_t g_adc_info_rtn;

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
static bool_t b_system_up = false;

static st_irq_pins_t s_irq_pins[] =
{
    { &g_external_irq_sw1 },
    { &g_external_irq_sw2 }
};

static st_pwm_pins_t s_pwm_pins[] =
{
    { &g_pwm_blue, GPT_IO_PIN_GTIOCB },   /* Blue led pwm (intensity) timer      */
    { &g_pwm_periodic_blue, GPT_IO_PIN_GTIOCA_AND_GTIOCB }  /* Blue led periodic (frequency) timer */
};

static uint32_t s_gpt_init_limit;

static fsp_err_t adc_initialize(void);

static fsp_err_t icu_initialize(void);

static fsp_err_t gpt_initialize(void);

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: common_init
 * Description  : perform common initialisation routines required for this application.
 * Return Value : operation success
 *                see R_INTC_IRQ_ExternalIrqOpen for full list of return states.
 *********************************************************************************************************************/
fsp_err_t common_init(void)
{
    fsp_err_t fsp_err = FSP_ERR_NOT_OPEN;

    /* init GreenPak device
     * NOTE When function returns FSP_ERR_INVALID_MODE code will run
     * look for reprogramming Errata on the Renesas website */
    board_greenpak_init();

    /* init sw switch control  */
    if (board_cfg_switch_init() != FSP_SUCCESS)
    {
        return FSP_ERR_NOT_OPEN;
    }
else
    {
    }

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

    if (FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }

    /* Set baseline LED status */
    g_board_status.led_intensity = 0; /* Default intensity 10% */
    g_board_status.led_frequency = 0; /* Default frequency 1Hz */

    led_pwm_periodic_update();

    led_pwm_update();

    /* Start the g_pwm_periodic_blue timer */
    R_GPT_Start(s_pwm_pins[BLUE_PERIODIC_INDEX].p_timer->p_ctrl);
    R_GPT_Start(s_pwm_pins[BLUE_PWM_INDEX].p_timer->p_ctrl);

    /* Indicate base platform is up and running */
	SYSTEM_OK_CFG_

	b_system_up = true;

    return fsp_err;
}
/**********************************************************************************************************************
 End of function common_init
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: led_pwm_update
 * Description  : Manage pwm for blue led, set duty cycle helper function see R_GPT_DutyCycleSet for details
 * Return Value : NONE
 *********************************************************************************************************************/
void led_pwm_update(void)
{
    R_GPT_DutyCycleSet (s_pwm_pins[0].p_timer->p_ctrl, g_pwm_dcs[g_board_status.led_intensity], s_pwm_pins[0].pin);
}
/**********************************************************************************************************************
 End of function led_pwm_update
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: led_pwm_periodic_update
 * Description  : Manage periodic timer for blue led, set period helper function see R_GPT_PeriodSet for details
 * Return Value : NONE
 *********************************************************************************************************************/
void led_pwm_periodic_update(void)
{
    R_GPT_PeriodSet (g_pwm_periodic_blue.p_ctrl, g_pwm_rates[g_board_status.led_frequency]);
}
/**********************************************************************************************************************
 End of function led_duty_cycle_update
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: gpt_periodic_blue_callback
 * Description  : Framework application callback for periodic (slow) Frequency timer g_pwm_periodic_blue.
 * Argument     : p_args - see r_timer_api.h
 * Return Value : None.
 *********************************************************************************************************************/
void gpt_periodic_blue_callback(timer_callback_args_t *p_args)

#define ENABLE_PWM_MANAGEMENT (1)
{
    UNUSED_PARAM(p_args);
    static bool_t blink_on = true;

    if (blink_on)
    {
#ifdef ENABLE_PWM_MANAGEMENT
        /* Manage g_gpt_blue */
        R_GPT_Start (s_pwm_pins[0].p_timer->p_ctrl);

#else
        /* Manage GPIO BLUE */
        TURN_BLUE_ON_CFG_;
#endif
    }
    else
    {
#ifdef ENABLE_PWM_MANAGEMENT
        /* Manage g_gpt_blue */
        R_GPT_Stop (s_pwm_pins[0].p_timer->p_ctrl);
#else
        /* Manage GPIO BLUE */
        TURN_BLUE_OFF_CFG_;
#endif
    }

    blink_on = !blink_on;
}
/**********************************************************************************************************************
 End of function gpt_blinker_blue_callback
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private (static) functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: adc_initialize
 * Description  : Initialize the ADC Controller to read the MCU core temperature.
 * Return Value : NONE
 *********************************************************************************************************************/
static fsp_err_t adc_initialize(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    /* When selecting the temperature sensor output, the internal reference voltage,
     * do not use continuous scan mode or group scan mode. */

    fsp_err = R_ADC_B_Open (&g_adc_ctrl, &g_adc_cfg);

    if (FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }

    fsp_err = R_ADC_B_ScanCfg (&g_adc_ctrl, &g_adc_scan_cfg);

    if (FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }

    fsp_err = R_ADC_B_ScanStart (&g_adc_ctrl);

    if (FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }

    /* Read TSN cal data (value written at manufacture, does not change at runtime) */
    fsp_err = R_ADC_B_InfoGet (&g_adc_ctrl, &g_adc_info_rtn);
    return fsp_err;
}
/**********************************************************************************************************************
 End of function adc_initialize
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: icu_initialize
 * Description  : Initialize the External Interrupts for the User Switches.
 * Return Value :NONE
 *********************************************************************************************************************/
static fsp_err_t icu_initialize(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    for (uint32_t i = 0; i < NUM_SWITCH; i++)
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
 * Description  : Initialise the General Purpose Timer used for blinking LED's.
 * Return Value : NONE.
 *********************************************************************************************************************/
static fsp_err_t gpt_initialize(void)
{
    fsp_err_t fsp_err = FSP_ERR_NOT_OPEN;
    s_gpt_init_limit = ((sizeof(s_pwm_pins)) / sizeof(st_pwm_pins_t));

    for (uint32_t i = 0; i < s_gpt_init_limit; i++)
    {
        fsp_err = R_GPT_Open (s_pwm_pins[i].p_timer->p_ctrl, s_pwm_pins[i].p_timer->p_cfg);

        if (FSP_SUCCESS != fsp_err)
        {
            return fsp_err;
        }
    }
    return fsp_err;
}
/**********************************************************************************************************************
 End of function gpt_initialize
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: system_up
 * Description  : Helper function used in application as a gate to throttle module startup.
 *                when the state is true the system resources cfg_switch, ospi, sdram etc. should be available
 *                see common_init() for configured resources.
 * Return Value : operation success
 *                true  -  reached the state that the rest of the application can continue
 *                false -  system resources may not be configured or initialised.
 *********************************************************************************************************************/
bool_t system_up(void)
{
    return (b_system_up);
}
/**********************************************************************************************************************
 End of function system_up
 *********************************************************************************************************************/
