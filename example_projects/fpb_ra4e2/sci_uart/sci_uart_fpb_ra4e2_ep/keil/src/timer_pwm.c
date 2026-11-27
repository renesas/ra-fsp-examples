/***********************************************************************************************************************
 * File Name    : timer_pwm.c
 * Description  : Contains timer function definitions.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "timer_pwm.h"

/*******************************************************************************************************************//**
 * @brief     This function initializes GPT in PWM mode.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t gpt_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open GPT module */
    err = R_GPT_Open(&g_timer_ctrl, &g_timer_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function starts GPT in PWM mode.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Started successfully.
 * @retval    Any other error codes     Failed to start.
 **********************************************************************************************************************/
fsp_err_t gpt_start(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Start GPT module */
    err = R_GPT_Start(&g_timer_ctrl);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function sets intensity through duty cycle generation on GPT pin.
 * @param[in] raw_count                 Raw count value for intensity generation.
 * @param[in] pin                       GPT pin.
 * @retval    FSP_SUCCESS               Set successfully.
 * @retval    Any other error codes     Failed to set.
 **********************************************************************************************************************/
fsp_err_t set_intensity(uint32_t raw_count, uint8_t pin)
{
    fsp_err_t err = FSP_SUCCESS;
    raw_count *= STEP;
#if defined(BOARD_RA4W1_EK) || defined (BOARD_RA6T1_RSSK) || defined (BOARD_RA6T2_MCK) || defined (BOARD_RA4T1_MCK)\
    || defined (BOARD_RA6T3_MCK) || defined (BOARD_RA8T1_MCK) || defined (BOARD_RA8T2_MCK)
    raw_count = (MAX_DUTY_CYCLE - raw_count);
#endif /* Invert PWM duty for boards with active-low output */

    /* Set GPT timer's duty cycle as per user input */
    err = R_GPT_DutyCycleSet(&g_timer_ctrl, raw_count, pin);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function closes the GPT.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void gpt_deinit(void)
{
    /* Close the opened GPT module */
    if (g_timer_ctrl.open)
    {
        R_GPT_Close(&g_timer_ctrl);
    }
}
