/***********************************************************************************************************************
 * File Name    : elc_example_app.c
 * Description  : Example application layer for the ELC EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: elc_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "console/uart_console.h"
#include "elc_example_app.h"

/* Private function */
static void handle_error(fsp_err_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the ELC example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void elc_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};

    /* Get FLEX pack version information */
    R_FSP_VersionGet(&version);

    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        return;     /* UART failed to open — cannot print anything */
    }

    /* Example project information printed on the terminal */
    printf(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    printf(EP_INFO);

    /* Open ELC driver */
    err = elc_init();
    handle_error(err, "elc_init failed\r\n");

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
    handle_error(err, "elc_enable failed\r\n");

    /* Open GPT timer in Saw-wave PWM mode or TAU timer in One-Shot Pulse Output mode */
    err = pwm_timer_init();
    handle_error(err, "pwm_timer_init failed\r\n");

    /* Open GPT timer in One-Shot mode or TAU timer in Delay Counter mode */
    err = oneshot_timer_init();
    handle_error(err, "oneshot_timer_init failed\r\n");

    /* Enable start and stop sources for Saw-wave PWM GPT timer or start source for One-Shot Pulse Output TAU timer */
    err = pwm_timer_enable();
    handle_error(err, "pwm_timer_enable failed\r\n");

    /* Enable start source for One-Shot GPT timer or Delay Counter TAU timer */
    err = oneshot_timer_enable();
    handle_error(err, "oneshot_timer_enable failed\r\n");

    /* On successful operations on ELC and TAU/GPT modules, the user is asked enter any key as terminal input */
    printf(STATUS_DISPLAY);

    /* Wait until data is available in the terminal */
    while (true)
    {
        if (uart_console_has_char())
        {
            /* Clean the user input to avoid an infinite loop */
            getchar();

            err = elc_event_gen();
            handle_error(err, "elc_event_gen failed\r\n");

            printf(STATUS_DISPLAY);
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function handles an error by printing error message and code, de-initializing the related modules,
 *            and entering a trap state.
 * @param[in] err           Error code.
 * @param[in] err_str       Error message.
 * @retval    None.
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err, const char * err_str)
{
    if (FSP_SUCCESS != err)
    {
        printf("\r\n[ERR] %s", err_str);
        printf("Returned Error Code: 0x%x\r\n", (unsigned int)err);
        uart_console_flush();
        oneshot_timer_deinit();
        pwm_timer_deinit();
        elc_deinit();
        __BKPT(0);
    }
}
