/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : common_init.c
 * Version      : 1.00
 * Description  : common initialize functions for EK-RA2L2 Quick Start Example Project
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 01.05.2025 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "hal_data.h"
#include "qsep_setting.h"
#include "board_info.h"
#include "common_init.h"
#include "r_typedefs.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/
typedef struct irq_pins
{
    const external_irq_instance_t * const p_irq;
} st_irq_pins_t;

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/
st_board_status_t g_board_status;


/**********************************************************************************************************************
 Private (static) variables
 *********************************************************************************************************************/
static st_irq_pins_t s_irq_pins[] =
{
        {&g_external_irq3},
        {&g_external_irq7}
};

static uint32_t   s_pwm_dcs[3]    = {LED_INTENSITY_10, LED_INTENSITY_50, LED_INTENSITY_90};
static uint32_t   s_pwm_rates[3]  = {BLINK_FREQ_1HZ, BLINK_FREQ_5HZ, BLINK_FREQ_10HZ};

static uint8_t s_blueled_flashing = OFF;

/**********************************************************************************************************************
 Private (static) functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: adc_initialize
 * Description  : ADC initialize function
 * Arguments    : none
 * Return Value : error code
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
        R_BSP_SoftwareDelay(20, BSP_DELAY_UNITS_MILLISECONDS);
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
 * Description  : ICU initialize function
 * Arguments    : none
 * Return Value : error code
 *********************************************************************************************************************/
static fsp_err_t icu_initialize(void)
{
    fsp_err_t fsp_err      = FSP_SUCCESS;
    uint8_t   num_switches = (sizeof(s_irq_pins)) / (sizeof(s_irq_pins[0]));

    for (uint8_t i = 0; i < num_switches; i++)
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
 * Function Name: agt_initialize
 * Description  : AGT initialize function
 * Arguments    : none
 * Return Value : error code
 *********************************************************************************************************************/
static fsp_err_t agt_initialize(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    fsp_err = R_AGT_Open (g_periodic_timer.p_ctrl, g_periodic_timer.p_cfg);
    if (fsp_err)
    {
        return fsp_err;
    }

    fsp_err = R_AGT_Open (g_agt_blue.p_ctrl, g_agt_blue.p_cfg);
    if (fsp_err)
    {
        return fsp_err;
    }

    return fsp_err;
}
/**********************************************************************************************************************
 End of function agt_initialize
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: common_init
 * Description  : common initialize function
 * Arguments    : none
 * Return Value : error code
 *********************************************************************************************************************/
fsp_err_t common_init(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    fsp_err = adc_initialize();

    if (FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }

    fsp_err = icu_initialize();

    if (FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }

    fsp_err = agt_initialize();

    if (FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }

    /* Set baseline LED status */
    g_board_status.led_intensity = 0;
    g_board_status.led_frequency = 0;
    g_board_status.cur_menu = 0;
    R_AGT_PeriodSet(g_periodic_timer.p_ctrl, s_pwm_rates[g_board_status.led_frequency]);

    /* Start the timers */
    R_AGT_Start(g_periodic_timer.p_ctrl);
    R_AGT_Start(g_agt_blue.p_ctrl);

    TURN_GREEN_ON;

    return fsp_err;
}
/**********************************************************************************************************************
 End of function common_init
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: irq_sw1_callback
 * Description  : IRQ for SW 1 on ICU Instance.
 * Argument     : p_args
 * Return Value : none
 *********************************************************************************************************************/
void irq_sw1_callback(external_irq_callback_args_t *p_args)
{
    /* Cast unused params to void */
    FSP_PARAMETER_NOT_USED(p_args);

    /* Cast as compiler will assume calc is int */
    g_board_status.led_intensity = (uint8_t)((g_board_status.led_intensity + 1) % 3);
}
/**********************************************************************************************************************
 End of function irq_sw1_callback
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: irq_sw2_callback
 * Description  : IRQ for SW 2 on ICU Instance.
 * Argument     : p_args
 * Return Value : none
 *********************************************************************************************************************/
void irq_sw2_callback(external_irq_callback_args_t *p_args)
{
    /* Cast unused params to void */
    FSP_PARAMETER_NOT_USED(p_args);

    /* Cast to uint8_t, as compiler will assume int for the calculation */
    g_board_status.led_frequency = (uint8_t)((g_board_status.led_frequency + 1) % 3);
    R_AGT_PeriodSet(g_periodic_timer.p_ctrl, s_pwm_rates[g_board_status.led_frequency]);
}
/**********************************************************************************************************************
 End of function irq_sw2_callback
 *********************************************************************************************************************/

/** Callback function for driver g_periodic_timer. */
/**********************************************************************************************************************
 * Function Name: periodic_timer_callback
 * Description  : Timer callback function to blink LED1 (blue)
 * Argument     : p_args
 * Return Value : none
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
 * Description  : Timer callback function to change the intensity of LED1 (blue)
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void agt_blue_callback(timer_callback_args_t *p_args)
{
    static uint32_t s_intense = 0;
    static uint32_t s_duty    = 1;

    /* Cast unused params to void */
    FSP_PARAMETER_NOT_USED(p_args);

    if(ON == s_blueled_flashing)
    {
        if ((s_intense++) < s_duty)
        {
            TURN_BLUE_ON;
        }
        else
        {
            TURN_BLUE_OFF;
        }

        if (s_intense >= 100)
        {
            s_intense = 0;
            s_duty    = s_pwm_dcs[g_board_status.led_intensity];
        }
    }
    else
    {
        TURN_BLUE_OFF;
        s_intense = 0;
        s_duty    = s_pwm_dcs[g_board_status.led_intensity];
    }
}
/**********************************************************************************************************************
 End of function agt_blue_callback
 *********************************************************************************************************************/
