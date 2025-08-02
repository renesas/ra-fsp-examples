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
 *         : 01.01.2018 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "common_init.h"
#include "board_hw_cfg.h"
#include "board_i2c_master.h"
#include "board_cfg_switch.h"
#include "board_greenpak.h"

#include "board_sdram.h"
#include "ospi_b_ep.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define NUM_RATES             (sizeof(pwm_rates) / sizeof(pwm_rates[0]))   /*  */
#define NUM_DCS               (sizeof(pwm_dcs) / sizeof(pwm_dcs[0]))       /*  */
#define NUM_SWITCH            (sizeof(s_irq_pins) / sizeof(s_irq_pins[0])) /*  */

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
bool_t g_show_getting_started_page = true;
adc_info_t g_adc_info_rtn;

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
static uint32_t s_duty    = 1;
static bool_t b_system_up = false;
static st_irq_pins_t s_irq_pins[] =
{
    { &g_external_irq12ds },
    { &g_external_irq13ds }
};

static st_pwm_pins_t s_pwm_pins[] =
{
    { &g_gpt_blue, GPT_IO_PIN_GTIOCB }
};

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
        /* Default operation for this application is to use OSPI so configure the switch */
        if(FSP_SUCCESS == board_cfg_switch_funct_on(SW4_3))
        {

            /* initialise OSPI */
            ospi_b_init();

            /* set to opi mode */
            ospi_b_set_protocol_to_opi();
        }

    }

    if (is_sdram_initialised() == false)
    {
        bsp_sdram_init ();
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

    led_duty_cycle_update ();

    /* Set baseline LED status */
    g_board_status.led_intensity = 0;
    g_board_status.led_frequency = 0;

    R_GPT_PeriodSet(g_blinker_blue.p_ctrl, g_pwm_rates[g_board_status.led_frequency]);

    led_duty_cycle_update ();
    s_duty = g_pwm_dcs[g_board_status.led_intensity];

    /* Start the timers */
    R_GPT_Start(g_blinker_blue.p_ctrl);
    R_GPT_Start(g_gpt_blue.p_ctrl);

	/* Indicate base platform is up and running */
	SYSTEM_OK_CFG_

	b_system_up = true;

    return fsp_err;
}
/**********************************************************************************************************************
 End of function common_init
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: led_duty_cycle_update
 * Description  : Manage pwm for blue led, set duty cycle helper function see R_GPT_DutyCycleSet for details
 *                Note this time is disabled when using led demo screen
 * Return Value : NONE
 *********************************************************************************************************************/
void led_duty_cycle_update(void)
{
    uint32_t required_intensity = 2 - g_board_status.led_intensity;

    R_GPT_DutyCycleSet (s_pwm_pins[0].p_timer->p_ctrl, g_pwm_dcs[required_intensity], s_pwm_pins[0].pin);
    s_duty = g_pwm_dcs[g_board_status.led_intensity];
}
/**********************************************************************************************************************
 End of function led_duty_cycle_update
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: gpt_blinker_blue_callback
 * Description  : user callback assigned to g_blinker_blue stack.
 * Argument     : p_args
 * Return Value : None.
 *********************************************************************************************************************/
void gpt_blinker_blue_callback(timer_callback_args_t *p_args)
{
    UNUSED_PARAM(p_args);
    static bool_t blink_on = true;

    if (blink_on)
    {
        R_GPT_Start (s_pwm_pins[0].p_timer->p_ctrl);
    }
    else
    {
        R_GPT_Stop (s_pwm_pins[0].p_timer->p_ctrl);
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
    fsp_err_t fsp_err = FSP_SUCCESS;

    for (uint32_t i = 0; i < 1; i++)
    {
        fsp_err = R_GPT_Open (s_pwm_pins[i].p_timer->p_ctrl, s_pwm_pins[i].p_timer->p_cfg);

        if (FSP_SUCCESS != fsp_err)
        {
            return fsp_err;
        }
    }

    fsp_err = R_GPT_Open (g_blinker_blue.p_ctrl, g_blinker_blue.p_cfg);
    {
        if (FSP_SUCCESS != fsp_err)
        {
            return fsp_err;
        }
    }

    fsp_err = R_GPT_Open (g_gpt_blue.p_ctrl, g_gpt_blue.p_cfg);

    if (fsp_err)
    {
        SYSTEM_ERROR_CFG_
        return fsp_err;
    }

    if (FSP_SUCCESS != fsp_err)
    {
        /* Fatal error */
        SYSTEM_ERROR_CFG_

        /* Close the GPT timer */
        R_GPT_Close (g_blinker_blue.p_ctrl);

        return fsp_err;
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
