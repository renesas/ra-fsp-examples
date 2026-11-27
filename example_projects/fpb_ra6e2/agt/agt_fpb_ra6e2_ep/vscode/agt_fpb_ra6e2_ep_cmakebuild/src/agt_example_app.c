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
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "agt_ep.h"
#include "agt_example_app.h"
#include "console/uart_console.h"

/* Global variables */
volatile uint8_t g_one_shot_timer_flag = RESET_FLAG;    /* Flag to check Timer0 is enabled or not */
volatile uint8_t g_periodic_timer_flag = RESET_FLAG;    /* Flag to check Timer1 is enabled or not */
volatile uint32_t g_error_flag = RESET_FLAG;            /* Flag to capture error in ISR's */
volatile uint32_t g_status_check_flag = RESET_FLAG;     /* Flag to capture the status of timers */

/* Private functions */
static fsp_err_t timer_status_check(void);
static void handle_error(fsp_err_t err, const char * err_str);
static void user_input(char *buf, int len);

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
    printf(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor,
           version.version_id_b.patch);
    printf(EP_INFO);

    /* Initialize AGT driver */
    err = agt_init();
    handle_error(err, "agt_init failed\r\n");

    printf(EP_GUIDE);

    while (true)
    {
        if (uart_console_has_char())
        {
            /* Clean buffer */
            memset(&input_buff[0], NULL_CHAR, BUFFER_SIZE);
            user_input(input_buff, (int)sizeof(input_buff));
            time_period_ms_oneshot = (uint32_t)atoi(input_buff);

            if ((TIME_PERIOD_MAX >= time_period_ms_oneshot) && (TIME_PERIOD_MIN < time_period_ms_oneshot))
            {
                printf("\r\nTime period for One-Shot mode timer: %d\r\n", time_period_ms_oneshot);
                break;
            }
            else
            {
                printf("\r\nInvalid input. Please enter valid input\r\n");
            }
        }
    }

    printf("\r\nPeriodic mode: ");

    while (true)
    {
        if (uart_console_has_char())
        {
            /* Clean buffer */
            memset(&input_buff[0], NULL_CHAR, BUFFER_SIZE);
            user_input(input_buff, (int)sizeof(input_buff));
            time_period_ms_periodic = (uint32_t)atoi(input_buff);

            if ((TIME_PERIOD_MAX >= time_period_ms_periodic) && (TIME_PERIOD_MIN < time_period_ms_periodic))
            {
                printf("\r\nTime period for Periodic mode timer: %d\r\n", time_period_ms_periodic);
                break;
            }
            else
            {
                printf("\r\nInvalid input. Please enter valid input\r\n");
            }
        }
    }

    /* Calculation of raw counts value for given milliseconds value */
    err = agt_info_get(&g_timer_one_shot_ctrl, &one_shot_info);
    handle_error(err, "agt_info_get for Timer0 failed\r\n");

    /* Depend on the user selected clock source, raw counts value can be calculated
     * for the user given time-period values */
    clock_freq = one_shot_info.clock_frequency;
    raw_counts_oneshot = (uint32_t)((time_period_ms_oneshot * clock_freq) / 1000);

    /* Set period value */
    err = agt_period_set(&g_timer_one_shot_ctrl, raw_counts_oneshot);
    handle_error(err, "agt_period_set for Timer0 failed\r\n");

    /* Calculation of raw counts value for given milliseconds value */
    err = agt_info_get(&g_timer_periodic_ctrl, &periodic_info);
    handle_error(err, "agt_info_get for Timer1 failed\r\n");

    /* Depend on the user selected clock source, raw counts value can be calculated
     * for the user given time-period values */
    clock_freq = periodic_info.clock_frequency;
    raw_counts_periodic = (uint32_t)((time_period_ms_periodic * clock_freq) / 1000);

    /* Set period value */
    err = agt_period_set(&g_timer_periodic_ctrl, raw_counts_periodic);
    handle_error(err, "agt_period_set for Timer1 failed\r\n");

    printf("\r\nEnter any key to start or stop the timers\r\n");

    while (true)
    {
        if (SET_FLAG == g_error_flag)
        {
            g_error_flag = RESET_FLAG;
            printf("[ERR] \r\nError in one_shot_timer_callback or periodic_timer_callback functions");
            uart_console_flush();
            __BKPT(0);
        }

        if (uart_console_has_char())
        {
            /* Clean buffer */
            memset(&input_buff[0], NULL_CHAR, BUFFER_SIZE);
            user_input(input_buff, (int)sizeof(input_buff));
            g_status_check_flag = SET_FLAG;
        }

        if (SET_FLAG == g_status_check_flag)
        {
            /* Check the status of timers and perform operation accordingly */
            g_status_check_flag = RESET_FLAG;
            err = timer_status_check();
            handle_error(err, "Timer start/stop failed\r\n");
        }

        /* Check if AGT0 is enabled in One-Shot mode */
        if (SET_FLAG == g_one_shot_timer_flag)
        {
            g_one_shot_timer_flag = RESET_FLAG;
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
    err = agt_status_get(&g_timer_periodic_ctrl, &periodic_timer_status);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Retrieve the status of timer running in One-Shot mode */
    err = agt_status_get(&g_timer_one_shot_ctrl, &oneshot_timer_status);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    if (TIMER_STATE_STOPPED != oneshot_timer_status.state)
    {
        err = agt_stop(&g_timer_one_shot_ctrl);
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
        err = agt_stop(&g_timer_periodic_ctrl);
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
        err = agt_start(&g_timer_one_shot_ctrl);
        if (FSP_SUCCESS != err)
        {
            return err;
        }
        else
        {
            /* Set Timer Flag as timer is started */
            g_one_shot_timer_flag = SET_FLAG;
        }
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief         This function reads a line of user input from stdin and remove trailing newline characters.
 * @param[in,out] buf       Pointer to buffer where the input string is stored.
 * @param[in]     len       Maximum number of characters to read (buffer size).
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
 * @param[in] err           Error code.
 * @param[in] err_str       Error message.
 * @retval    None.
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err, const char * err_str)
{
    if (FSP_SUCCESS != err)
    {
        printf("\r\n[ERR]: %s", err_str);
        printf("Returned Error Code: 0x%x\r\n", (unsigned int)err);
        uart_console_flush();
        agt_deinit();
        __BKPT(0);
    }
}
