/***********************************************************************************************************************
 * File Name    : elc_ep.c
 * Description  : Contains declarations of data structures and functions used in elc_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "elc_ep.h"

/* Global variable */
volatile bool g_external_irq_flag = false;

/*******************************************************************************************************************//**
 * @brief     This function initializes all the links in the Event Link Controller.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t elc_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_ELC_Open(&g_elc_ctrl, &g_elc_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function enables the operation of the Event Link Controller.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Enabled successfully.
 * @retval    Any other error codes     Failed to enable.
 **********************************************************************************************************************/
fsp_err_t elc_enable(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_ELC_Enable(&g_elc_ctrl);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function generates a software event in the Event Link Controller.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Generated successfully.
 * @retval    Any other error codes     Failed to generate.
 **********************************************************************************************************************/
fsp_err_t elc_event_gen(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_ELC_SoftwareEventGenerate(&g_elc_ctrl, ELC_SOFTWARE_EVENT_0);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function closes the opened ELC module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void elc_deinit(void)
{
    /* Close the opened ELC module */
    if (g_elc_ctrl.open)
    {
        R_ELC_Close(&g_elc_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function initializes the GPT timer in One-Shot mode or TAU timer in Delay Counter mode.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t oneshot_timer_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = OS_TIMER_INIT(&OS_TIMER_CTRL, &OS_TIMER_CFG);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function initializes the GPT timer in Saw-wave PWM mode or TAU timer in One-Shot Pulse Output mode.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t pwm_timer_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = PWM_TIMER_INIT(&PWM_TIMER_CTRL, &PWM_TIMER_CFG);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function enables external event triggers that start, stop, clear, or capture the counter for
 *            the GPT timer in One-Shot mode or TAU timer in Delay Counter mode.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Enabled successfully.
 * @retval    Any other error codes     Failed to enable.
 **********************************************************************************************************************/
fsp_err_t oneshot_timer_enable(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = OS_TIMER_ENABLE(&OS_TIMER_CTRL);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function enables external event triggers that start, stop, clear, or capture the counter for
 *            the GPT timer in Saw-wave PWM mode or TAU timer in One-Shot Pulse Output mode.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Enabled successfully.
 * @retval    Any other error codes     Failed to enable.
 **********************************************************************************************************************/
fsp_err_t pwm_timer_enable(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = PWM_TIMER_ENABLE(&PWM_TIMER_CTRL);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function de-initializes the GPT timer in One-Shot mode or TAU timer in Delay Counter mode.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void oneshot_timer_deinit(void)
{
    if (OS_TIMER_CTRL.open)
    {
        OS_TIMER_DEINIT(&OS_TIMER_CTRL);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function de-initializes the GPT timer in Saw-wave PWM mode or
 *            TAU timer in One-Shot Pulse Output mode.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void pwm_timer_deinit(void)
{
    if (PWM_TIMER_CTRL.open)
    {
        PWM_TIMER_DEINIT(&PWM_TIMER_CTRL);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function initializes and enables External IRQ module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized and enabled successfully.
 * @retval    Any other error codes     Failed to initialize or enable.
 **********************************************************************************************************************/
fsp_err_t icu_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize External IRQ module */
    err = R_ICU_ExternalIrqOpen(&g_external_irq_ctrl, &g_external_irq_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Enable External IRQ channel interrupt */
    err = R_ICU_ExternalIrqEnable(&g_external_irq_ctrl);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function closes the opened External IRQ module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void icu_deinit(void)
{
    /* Close the opened External IRQ module */
    if (g_external_irq_ctrl.open)
    {
        R_ICU_ExternalIrqClose(&g_external_irq_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function is a callback for the External IRQ module.
 * @param[in] p_args                    Pointer to the callback argument.
 * @retval    None.
 **********************************************************************************************************************/
void external_irq_callback(external_irq_callback_args_t * p_args)
{
    /* Check null pointer and External IRQ channel */
    if ((NULL == p_args) || (g_external_irq_cfg.channel != p_args->channel))
    {
        return;
    }

    /* Set the flag */
    g_external_irq_flag = true;
}
