/***********************************************************************************************************************
 * File Name    : agt_example_app.c
 * Description  : Example application layer for the AGT EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: agt_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "agt_ep.h"
#include "agt_example_app.h"
#include "console/uart_console.h"

/* Private variables */
static volatile bool s_one_shot_timer_flag = false;     /* Flag to check Timer0 is enabled or not */
static volatile bool s_status_check_flag = false;       /* Flag to capture the status of timers */
static timer_callback_args_t agt0_callback_args;
static timer_callback_args_t agt1_callback_args;

/* Private functions */
static fsp_err_t timer_status_check(void);
static uint32_t get_timer_period(void);
static void user_input(char * buf, int len);
static void handle_error(fsp_err_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the AGT example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void agt_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};
    char input_buff[BUFFER_SIZE] = {RESET_VALUE};
    uint32_t time_period_ms_oneshot = RESET_VALUE;
    uint32_t time_period_ms_periodic = RESET_VALUE;
    uint32_t raw_counts_oneshot = RESET_VALUE;
    uint32_t raw_counts_periodic = RESET_VALUE;
    timer_info_t one_shot_info =
    {
        .clock_frequency = RESET_VALUE,
        .count_direction = (timer_direction_t) RESET_VALUE,
        .period_counts = RESET_VALUE,
    };
    timer_info_t periodic_info =
    {
        .clock_frequency = RESET_VALUE,
        .count_direction = (timer_direction_t) RESET_VALUE,
        .period_counts = RESET_VALUE,
    };
    uint32_t clock_freq = RESET_VALUE;

    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        return;     /* UART failed to open — cannot print anything */
    }

    /* Get FLEX pack version information */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the terminal */
    printf(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    printf(EP_INFO);

    /* Initialize AGT driver */
    err = agt_init();
    if (FSP_SUCCESS != err)
    {
        printf("\r\n[ERR] agt_init failed\r\n");
        printf("Returned Error Code: 0x%x\r\n", (unsigned int)err);
        uart_console_flush();
        __BKPT(0);
    }

    err = g_timer_one_shot_callback_set_guard(NULL, one_shot_timer_callback, NULL, &agt0_callback_args);
    handle_error(err, "g_timer_one_shot_callback_set_guard failed\r\n");

    err = g_timer_periodic_callback_set_guard(NULL, periodic_timer_callback, NULL, &agt1_callback_args);
    handle_error(err, "g_timer_periodic_callback_set_guard failed\r\n");

    /* Get the period for the One-Shot Timer0 */
    printf(EP_GUIDE);
    time_period_ms_oneshot = get_timer_period();
    printf("\r\nTime period for One-Shot mode timer: %d\r\n", time_period_ms_oneshot);

    /* Get the period for the Periodic Timer1 */
    printf("\r\nPeriodic mode: ");
    time_period_ms_periodic = get_timer_period();
    printf("\r\nTime period for Periodic mode timer: %d\r\n", time_period_ms_periodic);

    /* Calculation of raw counts value for given milliseconds value */
    err = g_timer_one_shot_info_get_guard(NULL, &one_shot_info);
    handle_error(err, "g_timer_one_shot_info_get_guard failed\r\n");

    /* Depend on the user selected clock source, raw counts value can be calculated
     * for the user given time-period values */
    clock_freq = one_shot_info.clock_frequency;
    raw_counts_oneshot = (uint32_t)((time_period_ms_oneshot * clock_freq) / 1000);

    /* Set period value */
    err = g_timer_one_shot_period_set_guard(NULL, raw_counts_oneshot);
    handle_error(err, "g_timer_one_shot_period_set_guard failed\r\n");

    /* Calculation of raw counts value for given milliseconds value */
    err = g_timer_periodic_info_get_guard(NULL, &periodic_info);
    handle_error(err, "g_timer_periodic_info_get_guard failed\r\n");

    /* Depend on the user selected clock source, raw counts value can be calculated
     * for the user given time-period values */
    clock_freq = periodic_info.clock_frequency;
    raw_counts_periodic = (uint32_t)((time_period_ms_periodic * clock_freq) / 1000);

    /* Set period value */
    err = g_timer_periodic_period_set_guard(NULL, raw_counts_periodic);
    handle_error(err, "g_timer_periodic_period_set_guard failed\r\n");

    printf("\r\nEnter any key to start or stop the timers\r\n");

    while (true)
    {
        if (true == g_error_flag)
        {
            g_error_flag = false;
            printf("\r\n[ERR] Error in one_shot_timer_callback or periodic_timer_callback functions");
            uart_console_flush();
            __BKPT(0);
        }

        if (uart_console_has_char())
        {
            /* Clean buffer */
            memset(&input_buff[0], NULL_CHAR, BUFFER_SIZE);
            user_input(input_buff, (int)sizeof(input_buff));
            s_status_check_flag = true;
        }

        if (true == s_status_check_flag)
        {
            /* Check the status of timers and perform operation accordingly */
            s_status_check_flag = false;
            err = timer_status_check();
            handle_error(err, "Timer start/stop failed\r\n");
        }

        /* Check if AGT0 is enabled in One-Shot mode */
        if (true == s_one_shot_timer_flag)
        {
            s_one_shot_timer_flag = false;
            printf("\r\n\r\nAGT0 is enabled in One-Shot mode");
        }

        /* Check if AGT1 is enabled in Periodic mode */
        if (SET_FLAG == g_periodic_timer_flag)
        {
            g_periodic_timer_flag = RESET_FLAG;
            printf("\r\n\r\nOne-Shot mode AGT timer elapsed");
            printf("\r\n\r\nAGT1 is enabled in Periodic mode");
            printf("\r\nLED will toggle for set time period");
            printf("\r\nEnter any key to stop timers\r\n");
        }

        /* Check if AGT1 is already running in Periodic mode */
        if (ALREADY_RUNNING == g_periodic_timer_flag)
        {
            g_periodic_timer_flag = RESET_FLAG;
            printf("\r\n\r\nOne-Shot mode AGT timer elapsed\n");
            printf("\r\n\r\nAGT1 is already running in Periodic mode");
            printf("\r\nEnter any key to stop the timer\r\n");
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function checks the status of both timers and starts/stops the timers.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Operated successfully.
 * @retval    Any other error codes     Failed to operate.
 **********************************************************************************************************************/
static fsp_err_t timer_status_check(void)
{
    fsp_err_t err = FSP_SUCCESS;
    timer_status_t periodic_timer_status =
    {
        .counter = RESET_VALUE,
        .state = (timer_state_t) RESET_VALUE,
    };
    timer_status_t oneshot_timer_status =
    {
        .counter = RESET_VALUE,
        .state = (timer_state_t) RESET_VALUE,
    };

    /* Retrieve the status of timer running in Periodic mode */
    err = g_timer_periodic_status_get_guard(NULL, &periodic_timer_status);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Retrieve the status of timer running in One-Shot mode */
    err = g_timer_one_shot_status_get_guard(NULL, &oneshot_timer_status);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    if (TIMER_STATE_STOPPED != oneshot_timer_status.state)
    {
        err = g_timer_one_shot_stop_guard(NULL);
        if (FSP_SUCCESS != err)
        {
            return err;
        }
        else
        {
            printf("\r\n\r\nOne-Shot timer stopped. Enter any key to start timers.\r\n");
        }
    }
    else if (TIMER_STATE_STOPPED != periodic_timer_status.state)
    {
        err = g_timer_periodic_stop_guard(NULL);
        if (FSP_SUCCESS != err)
        {
            return err;
        }
        else
        {
            printf("\r\n\r\nPeriodic timer stopped. Enter any key to start timers.\r\n");
        }
    }
    else
    {
        err = agt_start_oneshot_timer();
        if (FSP_SUCCESS != err)
        {
            return err;
        }
        else
        {
            /* Set Timer flag as timer is started */
            s_one_shot_timer_flag = true;
        }
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function obtains a valid timer period value from the terminal.
 * @param[in] None.
 * @retval    input_period              Valid timer period value.
 **********************************************************************************************************************/
static uint32_t get_timer_period(void)
{
    char input_buff[BUFFER_SIZE] = {RESET_VALUE};
    uint32_t input_period = RESET_VALUE;

    while (true)
    {
        if (uart_console_has_char())
        {
            memset(input_buff, NULL_CHAR, sizeof(input_buff));
            user_input(input_buff, (int) sizeof(input_buff));
            input_period = (uint32_t) atoi(input_buff);

            if ((TIME_PERIOD_MAX >= input_period) && (TIME_PERIOD_MIN < input_period))
            {
                return input_period;
            }

            printf("\r\nInvalid input. Please enter a valid value.\r\n");
        }
    }
}

/*******************************************************************************************************************//**
 * @brief         This function reads a line of user input from stdin and removes trailing newline characters.
 * @param[in,out] buf                   Pointer to buffer where the input string is stored.
 * @param[in]     len                   Maximum number of characters to read (buffer size).
 * @retval        None.
 **********************************************************************************************************************/
static void user_input(char *buf, int len)
{
    fgets(buf, len, stdin);
    buf[strcspn(buf, "\r\n")] = '\0';
}

/*******************************************************************************************************************//**
 * @brief     This function handles an error by printing error message and code, de-initializing the related modules,
 *            and entering a trap state.
 * @param[in] err                       Error code.
 * @param[in] err_str                   Error message.
 * @retval    None.
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err, const char * err_str)
{
    if (FSP_SUCCESS != err)
    {
        printf("\r\n[ERR] %s", err_str);
        printf("Returned Error Code: 0x%x\r\n", (unsigned int)err);
        uart_console_flush();
        agt_deinit();
        __BKPT(0);
    }
}
