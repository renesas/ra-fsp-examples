/***********************************************************************************************************************
 * File Name    : gpt_input_capture_example_app.c
 * Description  : Example application layer for the GPT Input Capture EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: gpt_input_capture_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "gpt_timer.h"
#include "gpt_input_capture_example_app.h"
#include "console/uart_console.h"

/* Private function */
static void handle_error(fsp_err_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the GPT Input Capture example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void gpt_input_capture_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};
    timer_info_t info = {.clock_frequency = RESET_VALUE, .count_direction = RESET_VALUE, .period_counts = RESET_VALUE};
    float pulse_time = 0.0f;

    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        return;     /* UART failed to open — cannot print anything */
    }

    /* Get FLEX pack version information */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the terminal */
    printf(BANNER_INFO, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    printf(EP_INFO);

    /* Open GPT instance as a periodic timer */
    err = init_gpt(&g_timer_ctrl, &g_timer_cfg);
    handle_error(err, "init_gpt for periodic timer failed\r\n");

    /* Open GPT instance as input capture */
    err = init_gpt(&g_input_capture_ctrl, &g_input_capture_cfg);
    handle_error(err, "init_gpt for input capture failed\r\n");

    /* Enable GPT input capture */
    err = enable_gpt(&g_input_capture_ctrl);
    handle_error(err, "enable_gpt for input capture failed\r\n");

    /* Start GPT timer in periodic mode */
    err = start_gpt(&g_timer_ctrl);
    handle_error(err, "start_gpt for periodic timer failed\r\n");

    while (true)
    {
        /* Check for the flag from ISR callback */
        if (true == g_b_start_measurement)
        {
            /* Reset the flag */
            g_b_start_measurement = false;

            /* Get the period count and clock frequency */
            err = info_get_gpt(&g_input_capture_ctrl, &info);
            handle_error(err, "info_get_gpt failed\r\n");

            /* Calculate the pulse time */
            g_capture_count = (info.period_counts * g_capture_overflow) + g_capture_count;
            pulse_time = (float)(((float)g_capture_count) / ((float)info.clock_frequency));

            /* Reset the variables */
            g_capture_count = RESET_VALUE;
            g_capture_overflow = RESET_VALUE;

            /* Print pulse width measurement values */
            printf("\r\nPulse width measurement value (in second) - %.05f\r\n", pulse_time);
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function handles an error by printing error message and code, de-initializing the related modules,
 *            and entering a trap state.
 * @param[in] err                       Error code.
 * @param[in] err_str                   Error message.
 * @retval    None.
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err, const char *err_str)
{
    if (FSP_SUCCESS != err)
    {
        printf("\r\n[ERR] %s", err_str);
        printf("Returned Error Code: 0x%x\r\n", (unsigned int)err);
        uart_console_flush();
        /* De-initialize the opened timer instances */
        deinit_gpt();
        __BKPT(0);
    }
}
