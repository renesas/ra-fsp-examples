/***********************************************************************************************************************
 * File Name    : elc_example_app.c
 * Description  : Example application layer for the ELC EP.
 *                - hal_entry.c calls a single function: elc_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "elc_ep.h"
#include "elc_example_app.h"

/* Private function */
static void handle_error(fsp_err_t err);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the ELC example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void elc_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open ELC driver */
    err = elc_init();
    handle_error(err);

    /* Enable links between modules:
     * For elc_ep running with GPT timers:
     *   1. ELC software event is used as the start source for a target GPT instance (e.g., GPTx, GPTy).
     *   2. Counter overflow event of a reference GPT instance (e.g., GPTy) is used as the stop source
     *   for another GPT instance (e.g., GPTx).
     *
     * For elc_ep running with TAU timers:
     *   1. ELC software event is used as the start source for a TAU instance (e.g., TAUx).
     *   2. Match comparison event of that TAU instance (TAUx) is used as the start source for another
     *   TAU instance (e.g., TAUy). */
    err = elc_enable();
    handle_error(err);

    /* Open GPT timer in Saw-wave PWM mode or TAU timer in One-Shot Pulse Output mode */
    err = pwm_timer_init();
    handle_error(err);

    /* Open GPT timer in One-Shot mode or TAU timer in Delay Counter mode */
    err = oneshot_timer_init();
    handle_error(err);

    /* Enable start and stop sources for Saw-wave PWM GPT timer or start source for One-Shot Pulse Output TAU timer */
    err = pwm_timer_enable();
    handle_error(err);

    /* Enable start source for One-Shot GPT timer or Delay Counter TAU timer */
    err = oneshot_timer_enable();
    handle_error(err);

    /* Initialize and enable the External IRQ module */
    err = icu_init();
    handle_error(err);

    while (true)
    {
        /* Check for an external IRQ event */
        if (true == g_external_irq_flag)
        {
            /* Reset the flag */
            g_external_irq_flag = false;

            err = elc_event_gen();
            handle_error(err);
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function handles an error by de-initializing the related modules and entering a trap state.
 * @param[in] err                       Error code.
 * @retval    None.
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err)
{
    if (FSP_SUCCESS != err)
    {
        oneshot_timer_deinit();
        pwm_timer_deinit();
        elc_deinit();
        icu_deinit();
        __BKPT(0);
    }
}
