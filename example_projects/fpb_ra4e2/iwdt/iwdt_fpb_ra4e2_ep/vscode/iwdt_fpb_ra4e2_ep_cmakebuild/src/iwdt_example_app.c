/***********************************************************************************************************************
 * File Name    : iwdt_example_app.c
 * Description  : Example application layer for the IWDT EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: iwdt_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "iwdt_ep.h"
#include "iwdt_example_app.h"
#include "timer_setup.h"
#include "console/uart_console.h"

/* Private functions */
static fsp_err_t iwdt_operation(void);
static void handle_error(fsp_err_t err, const char *err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the IWDT example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void iwdt_example_app_run(void)
{
    fsp_pack_version_t version = {RESET_VALUE};
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize UART console before any printing */
    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        return;     /* UART failed to open — cannot print anything */
    }

    /* Get FLEX pack version information */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the Console */
    printf(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    printf(EP_INFO);

    /* Check whether reset is caused by IWDT or NMI and print result */
    reset_cause_t cause = check_reset_status();
    if (RESET_IWDT == cause)
    {
        printf("\r\n************************ IWDT Reset detected ************************\r\n");
    }
    else if (RESET_NMI_SW == cause)
    {
        printf("\r\n******************* NMI Software Reset detected *********************\r\n");
    }
    else
    {
        /* Do nothing */
    }

    printf("\r\nClock source for IWDT is IWDTCLK\r\n");

    /* Initialize timer module (TAU in Interval mode or GPT in Periodic mode, depending on board) */
    err = init_timer_module();
    handle_error(err, "init_timer_module failed\r\n");

    /* Menu for user selection */
    printf(MENU);

    while (true)
    {
        /* Process input only when the user has provided one */
        if (uart_console_has_char())
        {
            err = iwdt_operation();
            handle_error(err, "iwdt_operation failed\r\n");
        }

        /* For every 2 seconds, print IWDT refresh message.
         * This is done to avoid the continuous print message on the terminal */
        if (IWDT_REFRESH_COUNTER_VALUE == g_timer_underflow_counter)
        {
            g_timer_underflow_counter = RESET_VALUE;
            printf("\r\nIWDT counter refreshed\r\n");
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function processes IWDT operations based on user input from the terminal.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Performed operation successfully.
 * @retval    Any other error codes     Failed to perform operation.
 **********************************************************************************************************************/
static fsp_err_t iwdt_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;

    uint8_t user_data = RESET_VALUE;
    static bool b_is_iwdt_enable = false;

    /* Get user input */
    user_data = (uint8_t)(getchar() - '0');

    /* Perform operation on the user input */
    switch (user_data)
    {
        case ENABLE_IWDT:
        {
            /* Set the variable */
            b_is_iwdt_enable = true;

            /* Enable IWDT to count and generate NMI or reset when the debugger (J-Link) is connected */
            enable_iwdt_count_in_debug_mode();

            /* 1. If EP uses TAU timer: Open IWDT, for every TAU timeout, IWDT will get refreshed */
            /* 2. If EP uses GPT timer: Open IWDT, for every GPT timeout, IWDT will get refreshed */
            err = iwdt_open();
            if (FSP_SUCCESS != err)
            {
                return err;
            }

            /* Start timer */
            err = timer_start();
            if (FSP_SUCCESS != err)
            {
                return err;
            }

            /* Print message to indicate the user about application status */
            printf("\r\nIWDT initialized, "USED_TIMER" timer started");
            printf("\r\nPress '2' to stop IWDT counter from refreshing\r\n");
            break;
        }

        case STOP_IWDT_REFRESH:
        {
            if (true == b_is_iwdt_enable)
            {
                /* Reset the variable */
                b_is_iwdt_enable = false;

                /* Stop timer to stop refreshing IWDT */
                err = timer_stop();
                if (FSP_SUCCESS != err)
                {
                    return err;
                }

                printf("\r\n"USED_TIMER" timer stopped");
                printf("\r\nIndependent Watchdog Timer underflow in "IWDT_UNDERFLOW" seconds\r\n");
            }
            else
            {
                printf(MENU);
            }
            break;
        }

        default:
        {
            /* Menu for the user selection */
            printf("\r\nInvalid input");
            printf("\r\nPlease provide input from below option");
            printf(MENU);
            break;
        }
    }

    user_data = RESET_VALUE;
    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function handles an error by printing a message and error code, de-initializing
 *            the timer module, and entering a trap state.
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
        /* De-initialize timer module */
        deinit_timer_module();

        /* Trap here */
        __BKPT(0);
    }
}
