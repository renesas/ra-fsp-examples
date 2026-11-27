/***********************************************************************************************************************
 * File Name    : gpt_example_app.c
 * Description  : Example application layer for the GPT EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: gpt_example_app_run().
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
#include "gpt_timer.h"
#include "gpt_example_app.h"
#include "console/uart_console.h"

/* Value to validate open state of timer instance */
extern bool volatile g_one_shot_expired;

/* Private functions */
static void periodic_timer_operation(void);
static void pwm_timer_operation(void);
static void one_shot_timer_operation(void);
static uint32_t process_input_data(uint32_t * value);
static void user_input(char *buf, int len);
static void handle_error(fsp_err_t err, const char *err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the GPT example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void gpt_example_app_run(void)
{
    char rByte[BUF_SIZE] = {INITIAL_VALUE};
    uint32_t read_data = RESET_VALUE;

    fsp_pack_version_t version = {RESET_VALUE};

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

    /* Print menu */
    printf(EP_MENU);

    while (true)
    {
        /* Process input only when the user has provided one */
        if (uart_console_has_char())
        {
            /* Read first byte of data provided by the user */
            user_input(rByte, BUF_SIZE);

            /* Conversion from input string to integer value */
            read_data = (uint32_t) (atoi((char *)rByte));

            switch (read_data)
            {
                case PERIODIC_MODE_TIMER:
                {
                    periodic_timer_operation();
                    break;
                }

                case PWM_MODE_TIMER:
                {
                    pwm_timer_operation();
                    break;
                }

                case ONE_SHOT_MODE_TIMER:
                {
                    one_shot_timer_operation();
                    break;
                }

                default:
                {
                    printf("\r\nInvalid menu option selected\r\n");
                    break;
                }
            }

            read_data = RESET_VALUE;

            /* Reset buffer */
            memset(rByte, INITIAL_VALUE, BUF_SIZE);

            /* Print menu */
            printf(EP_MENU);
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function performs GPT timer in Periodic mode operation.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void periodic_timer_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t gpt_desired_period_ms = RESET_VALUE;
    uint32_t len = RESET_VALUE;
    uint32_t source_freq_hz = RESET_VALUE;
    uint64_t period_counts = RESET_VALUE;

    /* De-initialize the opened active GPT timer instance */
    if (gp_active_timer_ctrl != NULL)
    {
        deinit_gpt_timer((timer_ctrl_t *) gp_active_timer_ctrl);
    }

    printf("\r\n\r\nEnter the desired period in millisecond "PERIOD_RANGE": ");
    len = process_input_data(&gpt_desired_period_ms);
    if (RESET_VALUE == len)
    {
        printf("\r\nInvalid input\r\n");
    }

    /* Get the source clock frequency (in Hz) */
#if (BSP_PERIPHERAL_GPT_GTCLK_PRESENT && !GPT_CFG_GPTCLK_BYPASS)
    /* Calculate the GPTCK Divider */
    const uint8_t divisor = (uint8_t) R_FSP_ClockDividerGet(R_SYSTEM->GPTCKDIVCR_b.GPTCKDIV);

    /* Calculate the GPTCK Frequency */
    source_freq_hz = R_BSP_SourceClockHzGet((fsp_priv_source_clock_t) R_SYSTEM->GPTCKCR_b.GPTCKSEL) / divisor;
#else
    /* Get the PCLKD Frequency */
    source_freq_hz = R_FSP_SystemClockHzGet(FSP_PRIV_CLOCK_PCLKD);
#endif /* BSP_PERIPHERAL_GPT_GTCLK_PRESENT && !GPT_CFG_GPTCLK_BYPASS */

    source_freq_hz >>= (uint32_t)(g_timer_periodic_cfg.source_div);

    /* Convert period to PCLK counts so it can be set in hardware */
    period_counts = (uint64_t)((gpt_desired_period_ms * (source_freq_hz * CLOCK_TYPE_SPECIFIER))\
                    / TIMER_UNITS_MILLISECONDS);

    /* Validate period count based on the user input (time period in millisecond) */
    if (GPT_MAX_PERIOD_COUNT < period_counts)
    {
        printf("\r\n[ERR] ** Invalid input, desired period is out of range **\r\n");
    }
    else
    {
        /* Initialize Periodic timer */
        err = init_gpt_timer(&g_timer_periodic_ctrl, &g_timer_periodic_cfg);
        handle_error(err, "init_gpt_timer for Periodic timer failed\r\n");
        printf("\r\n\r\nOpened timer in Periodic mode\r\n");

        /* Start Periodic timer */
        err = start_gpt_timer(&g_timer_periodic_ctrl);
        handle_error(err, "start_gpt_timer for Periodic timer failed\r\n");
        printf("\r\nStarted timer in Periodic mode\r\n");

        /* Set the desired period counts on the on-board LED */
        err = set_period_gpt_timer(&g_timer_periodic_ctrl, (uint32_t)period_counts);
        handle_error(err, "set_period_gpt_timer for Periodic timer failed\r\n");
        printf("\r\nTimer period: %d\r\n", gpt_desired_period_ms);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function performs GPT timer in PWM mode operation.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void pwm_timer_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t len = RESET_VALUE;
    uint32_t gpt_desired_duty_cycle_percent = RESET_VALUE;

    /* De-initialize the opened active GPT timer instance */
    if (gp_active_timer_ctrl != NULL)
    {
        deinit_gpt_timer((timer_ctrl_t *) gp_active_timer_ctrl);
    }

    printf("\r\n\r\nEnter the desired duty cycle in percentage of range 0 - 100: ");
    len = process_input_data(&gpt_desired_duty_cycle_percent);
    if (RESET_VALUE == len)
    {
        printf("\r\nInvalid input\r\n");
    }

    /* Validate duty cycle percentage */
    if (GPT_MAX_PERCENT < gpt_desired_duty_cycle_percent)
    {
        printf("\r\n[ERR] ** Invalid input, desired duty cycle is out of range **\r\n");
    }
    else
    {
        /* We got valid input, initialize PWM timer */
        err = init_gpt_timer(&g_timer_pwm_ctrl, &g_timer_pwm_cfg);
        handle_error(err, "init_gpt_timer for PWM timer failed\r\n");
        printf("\r\n\r\nOpened timer in PWM mode\r\n");

        /* Start PWM Timer */
        err = start_gpt_timer(&g_timer_pwm_ctrl);
        handle_error(err, "start_gpt_timer for PWM timer failed\r\n");
        printf("\r\nStarted timer in PWM mode\r\n");

        /* Set duty cycle of PWM timer */
        err = set_timer_duty_cycle((uint8_t)gpt_desired_duty_cycle_percent);
        handle_error(err, "set_timer_duty_cycle for PWM timer failed\r\n");
        printf("\r\nDuty cycle: %d\r\n", gpt_desired_duty_cycle_percent);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function performs GPT timer in One-Shot mode operation.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void one_shot_timer_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;
    int32_t one_shot_timeout = INT32_MAX;

    /* De-initialize the opened active GPT timer instance */
    if (gp_active_timer_ctrl != NULL)
    {
        deinit_gpt_timer((timer_ctrl_t *) gp_active_timer_ctrl);
    }

    deinit_gpt_timer(&g_timer_one_shot_mode_ctrl);

    /* Initialize One-Shot timer */
    err = init_gpt_timer(&g_timer_one_shot_mode_ctrl, &g_timer_one_shot_mode_cfg);
    handle_error(err, "init_gpt_timer for One-Shot timer failed\r\n");
    printf("\r\n\r\nOpened timer in One-Shot mode\r\n");

    /* Start One-Shot timer */
    err = start_gpt_timer(&g_timer_one_shot_mode_ctrl);
    handle_error(err, "start_gpt_timer for One-Shot timer failed\r\n");
    printf("\r\nStarted timer in One-Shot mode\r\n");

    /* Wait for One-Shot mode timer to expire */
    while (true != g_one_shot_expired)
    {
        /* Start checking for time out to avoid infinite loop */
        --one_shot_timeout;
        if (RESET_VALUE == one_shot_timeout)
        {
            /* We have reached to a scenario where One-Shot event did not occur */
            printf("[ERR] Callback event not received during One-Shot operation\r\n");
            break;
        }
    }

    if (true == g_one_shot_expired)
    {
        printf("\r\nTimer expired in One-Shot mode\r\n");
        g_one_shot_expired = false;
    }
}

/***********************************************************************************************************************
 *  @brief     This function processes input string to integer value.
 *  @param[in] value                    Pointer to the converted integer value.
 *  @retval    num_bytes                Number of bytes received from the input string.
 **********************************************************************************************************************/
uint32_t process_input_data(uint32_t * value)
{
    char buf[BUF_SIZE] = {INITIAL_VALUE};
    uint32_t num_bytes = RESET_VALUE;

    while (RESET_VALUE == num_bytes)
    {
        if (uart_console_has_char())
        {
            user_input(buf, BUF_SIZE);
            num_bytes = strlen(buf);
        }
    }

    /* Conversion from input string to integer value */
    * value = (uint32_t) (atoi((char *)buf));

    return num_bytes;
}

/*******************************************************************************************************************//**
 * @brief         This function reads a line of user input from stdin and remove trailing newline characters.
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
static void handle_error(fsp_err_t err, const char *err_str)
{
    if (FSP_SUCCESS != err)
    {
        printf("\r\n[ERR] %s", err_str);
        printf("Returned Error Code: 0x%x\r\n", (unsigned int)err);
        uart_console_flush();
        /* De-initialize the timer instance if it opened */
        deinit_gpt_timer((timer_ctrl_t *) gp_active_timer_ctrl);
        __BKPT(0);
    }
}
