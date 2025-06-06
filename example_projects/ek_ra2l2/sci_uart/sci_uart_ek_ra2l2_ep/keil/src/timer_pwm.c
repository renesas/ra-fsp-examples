/***********************************************************************************************************************
 * File Name    : timer_pwm.c
 * Description  : Contains timer functions definition.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "timer_pwm.h"
#include "uart_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup r_sci_uart_ep
 * @{
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief       Initialize GPT in PWM mode.
 * @param[in]   None
 * @retval      FSP_SUCCESS         Upon successful open of timer
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful open
 **********************************************************************************************************************/
fsp_err_t gpt_initialize(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open GPT module */
    err = R_GPT_Open (&g_timer_ctrl, &g_timer_cfg);
    if (FSP_SUCCESS != err)
    {
        /* GPT Timer failure message */
        APP_ERR_PRINT ("\r\n** R_GPT_TimerOpen API failed **\r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       Start GPT in PWM mode.
 * @param[in]   None
 * @retval      FSP_SUCCESS         Upon successful start of timer
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful start
 **********************************************************************************************************************/
fsp_err_t gpt_start(void)
{

    fsp_err_t err = FSP_SUCCESS;

    /* Start GPT module */
    err =  R_GPT_Start (&g_timer_ctrl);
    if (FSP_SUCCESS != err)
    {
        /* GPT Start failure message */
        APP_ERR_PRINT ("\r\n ** R_GPT_Start API failed **\r\n");
    }
    return err;

}

/*******************************************************************************************************************//**
 * @brief     Set intensity through duty cycle generation on GPT pin
 * @param[in] raw_count        Raw count value for intensity generation
 *            pin              GPT pin
 * @retval    FSP_SUCCESS      Upon Duty Cycle set returns Successful.
 * @retval    Any Other Error code apart from FSP_SUCCES  Unsuccessful in setting Duty Cycle
 **********************************************************************************************************************/
fsp_err_t set_intensity(uint32_t raw_count, uint8_t pin)
{
    fsp_err_t err = FSP_SUCCESS;
    raw_count *= STEP;
#if defined(BOARD_RA4W1_EK) || defined (BOARD_RA6T1_RSSK) || defined (BOARD_RA6T3_MCK) || defined (BOARD_RA6T2_MCK)\
    || defined (BOARD_RA4T1_MCK)
    raw_count = (MAX_DUTY_CYCLE - raw_count);
#endif
    /* Set GPT timer's DutyCycle as per user input */
    err = R_GPT_DutyCycleSet (&g_timer_ctrl, raw_count, pin);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\n ** R_GPT_DutyCycleSet API failed **\r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       Close the GPT HAL driver before the project ends up in an Error Trap.
 * @param[in]   None
 * @retval      None
 **********************************************************************************************************************/
void timer_gpt_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close the GPT module */
    err = R_GPT_Close (&g_timer_ctrl);
    if (FSP_SUCCESS != err)
    {
        /* GPT Close failure message */
        APP_ERR_PRINT ("\r\n ** R_GPT_Close API failed **\r\n");
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup r_sci_uart_ep)
 **********************************************************************************************************************/
