/***********************************************************************************************************************
 * File Name    : ulpt_example_app.c
 * Description  : Example application layer for the ULPT EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: ulpt_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ulpt_ep.h"
#include "ulpt_example_app.h"
#include "console/uart_console.h"

/* Private functions */
static void run_periodic_operation(void);
static void run_one_shot_operation(void);
static void apply_period(uint32_t period_in_second);
static uint32_t select_period(void);
static void user_input(char * buf, int len);
static void handle_error(fsp_err_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the ULPT example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void ulpt_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};

    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        return;     /* UART failed to open — cannot print anything */
    }

    /* First time start */
    if (false == g_internal_reset_flag)
    {
        /* Get FSP pack information */
        R_FSP_VersionGet(&version);

        /* Example project information printed on the terminal */
        printf(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor,
               version.version_id_b.patch);
        printf(EP_INFO);
    }

    /* Internal reset */
    else
    {
        g_internal_reset_flag = false;
        printf(BANNER_RESET);
    }

    /* Initialize necessary hardware modules */
    err = ulpt_lpm_open();
    handle_error(err, "ulpt_lpm_open failed\r\n");

    err = ulpt_one_shot_timer_open();
    handle_error(err, "ulpt_one_shot_timer_open failed\r\n");

    err = ulpt_periodic_timer_open();
    handle_error(err, "ulpt_periodic_timer_open failed\r\n");

    /* The user selects the period for both timers */
    apply_period(select_period());

    /* Main loop */
    while (true)
    {
        run_periodic_operation();
        run_one_shot_operation();
    }
}

/*******************************************************************************************************************//**
 * @brief     This function performs Periodic timer operation.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void run_periodic_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t count_value = RESET_VALUE;
    char * count_str[] =
    {
        [0] = NULL,
        [1] = "1st",
        [2] = "2nd",
        [3] = "3rd",
    };

    /* Start Periodic timer */
    err = ulpt_periodic_start();
    handle_error(err, "ulpt_periodic_start failed\r\n");
    printf("\r\nULPT1 in Periodic mode started\r\n");

    /* Wait until ULPT1 timer underflow three times */
    while (TIMES_MAX > count_value)
    {
        if (ulpt_periodic_underflow_consume())
        {
            count_value++;

            printf("\r\nULPT1 underflow for the %s time\r\n", count_str[count_value]);

            if (ulpt_periodic_led_toggle())
            {
                printf("\r\nLED is toggled for the %s time\r\n", count_str[count_value]);
            }
        }
    }

    /* Stop ULPT1 in Periodic mode */
    err = ulpt_periodic_stop();
    handle_error(err, "ulpt_periodic_stop failed\r\n");
    printf("\r\nULPT1 in Periodic mode stopped\r\n");
}

/*******************************************************************************************************************//**
 * @brief     This function performs One-Shot timer operation.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void run_one_shot_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Start One-Shot timer */
    err = ulpt_one_shot_start();
    handle_error(err, "ulpt_one_shot_start failed\r\n");
    printf("\r\nULPT0 in One-Shot mode started\r\n");

    /* Print the enter LPM mode notice and wait until transmission is complete */
    printf("\r\nMCU enter to Deep SW Standby mode\r\n");

    /* Enter to Deep SW Standby mode */
    err = ulpt_low_power_mode_enter();
    handle_error(err, "ulpt_low_power_mode_enter failed\r\n");
}

/*******************************************************************************************************************//**
 * @brief     This function applies the selected period to both the One-Shot and Periodic timers.
 * @param[in] period_in_second          Period value in second.
 * @retval    None.
 **********************************************************************************************************************/
static void apply_period(uint32_t period_in_second)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Set a period value for the One-Shot timer */
    err = ulpt_timer_period_set(&g_timer_one_shot_ctrl, period_in_second);
    handle_error(err, "ulpt_timer_period_set for One-Shot timer failed\r\n");

    /* Set a period value for the Periodic timer */
    err = ulpt_timer_period_set(&g_timer_periodic_ctrl, period_in_second);
    handle_error(err, "ulpt_timer_period_set for Periodic timer failed\r\n");
}

/*******************************************************************************************************************//**
 * @brief     This function gets the timer period from the user.
 * @param[in] None.
 * @retval    period_in_second          Period value in second.
 **********************************************************************************************************************/
static uint32_t select_period(void)
{
    char input_buff[BUFFER_SIZE] = {RESET_VALUE};
    uint8_t user_option = RESET_VALUE;
    uint32_t period_in_second = RESET_VALUE;
    bool select_flag = false;

    /* Print menu */
    printf(EP_MENU);

    /* Console input handle */
    while (false == select_flag)
    {
        if (uart_console_has_char())
        {
            /* Clean input buffer */
            memset(&input_buff[0], NULL_CHAR, BUFFER_SIZE);

            /* Get user option */
            user_input(input_buff, (int)sizeof(input_buff));
            user_option = (uint8_t)atoi(input_buff);

            /* Check user input */
            switch (user_option)
            {
                case SELECT_1S:
                {
                    period_in_second = PERIOD_1S;
                    select_flag = true;
                    printf("\r\n\r\nThe selected period is 1 second\r\n");
                    break;
                }

                case SELECT_2S:
                {
                    period_in_second = PERIOD_2S;
                    select_flag = true;
                    printf("\r\n\r\nThe selected period is 2 seconds\r\n");
                    break;
                }

                case SELECT_3S:
                {
                    period_in_second = PERIOD_3S;
                    select_flag = true;
                    printf("\r\n\r\nThe selected period is 3 seconds\r\n");
                    break;
                }

                default:
                {
                    printf("\r\nInvalid value!\r\n");
                    printf(EP_MENU);
                    break;
                }
            }
        }
    }

    return period_in_second;
}

/*******************************************************************************************************************//**
 * @brief         This function reads a line of user input from stdin and removes trailing newline characters.
 * @param[in,out] buf                   Pointer to buffer where the input string is stored.
 * @param[in]     len                   Maximum number of characters to read (buffer size).
 * @retval        None.
 **********************************************************************************************************************/
static void user_input(char * buf, int len)
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
        hw_module_deinit();
        __BKPT(0);
    }
}
