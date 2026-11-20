/***********************************************************************************************************************
 * File Name    : wdt_example_app.c
 * Description  : Contains data structures and functions used in wdt_example_app.c.
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
#include "ext_irq_setup.h"
#include "timer_setup.h"
#include "wdt_ep.h"
#include "wdt_example_app.h"

/* External global variables */
extern volatile uint32_t g_timer_underflow_counter;
extern volatile uint8_t g_user_button;

/* Private functions */
static fsp_err_t process_input(int32_t cmd);
static void handle_error(fsp_err_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the WDT example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void wdt_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};

    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        return;     /* UART failed to open — cannot print anything */
    }

    /* Get FSP version */
    R_FSP_VersionGet(&version);

    /* Print example project information */
    printf(BANNER_INFO, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    printf(EP_INFO);

    /* Check if reset WDT */
    if (SYSTEM_RSTSR1_WDTRF_DETECT_WDT_RESET == R_SYSTEM->RSTSR1_b.WDTRF)
    {
        check_reset_status();
        printf("\r\n************************ WDT Reset detected ************************\r\n");
    }

    /* Initialize GPT in Periodic mode */
    err = init_gpt_module();
    handle_error(err, "init_gpt_module failed\r\n");

    /* Initialize external IRQ */
    err = init_icu_module();
    handle_error(err, "init_icu_module failed\r\n");

    /* Menu for the user selection */
    printf(EP_MENU);

    while (true)
    {
        /* Process input only when the user has provided one */
        if (uart_console_has_char())
        {
            int selection = getchar();
            err = process_input(selection);
            handle_error(err, "process_input failed\r\n");
        }

        /* For every 3 seconds, terminal prints WDT refresh message.
         * This is done to avoid the continuous print message on terminal */
        if (WDT_REFRESH_COUNTER_VALUE == g_timer_underflow_counter)
        {
            g_timer_underflow_counter = RESET_VALUE;
            printf("\r\nWDT counter refreshed\r\n");
        }

        if (g_user_button)
        {
            g_user_button = RESET_FLAG;
            printf("\r\nPush button is pressed\r\nGPT timer stopped\r\n");
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function handles user input command from the terminal.
 * @param[in] cmd                       Command value received from console input.
 * @retval    FSP_SUCCESS               Handled successfully.
 * @retval    Any other error codes     Failed to handle.
 **********************************************************************************************************************/
static fsp_err_t process_input(int32_t cmd)
{
    fsp_err_t err = FSP_SUCCESS;

    switch (cmd)
    {
        case ENABLE_WDT:
        {
            /* Enable WDT to count and generate NMI or Reset when terminal is connected */
            enable_wdt_count_in_debug_mode();

            /* Open WDT. For every GPT timeout, WDT will get refreshed */
            err = init_wdt_module();
            if (FSP_SUCCESS != err)
            {
                return err;
            }

            /* Start GPT timer in Periodic mode */
            err = timer_start();
            if (FSP_SUCCESS != err)
            {
                return err;
            }

            /* Enable External IRQ */
            err = enable_icu_module();
            if (FSP_SUCCESS != err)
            {
                return err;
            }

            /* Print message to indicate the user about application status */
            printf("\r\n\r\nWDT initialized, GPT timer started");
            printf("\r\nTo stop WDT counter from refreshing, press the push button\r\n");
            break;
        }

        default:
        {
            /* Menu for the user selection */
            printf("\r\nInvalid input\r\n");
            printf(EP_MENU);
            break;
        }
    }

    return err;
}

/***********************************************************************************************************************
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
        deinit_gpt_module();
        deinit_icu_module();
        __BKPT(0);
    }
}
