/***********************************************************************************************************************
 * File Name    : tml_example_app.c
 * Description  : Example application layer for the TML EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: tml_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "elc_ep.h"
#include "lpm_ep.h"
#include "tml_example_app.h"

/* TML modules name */
static const char * sp_tml_mode_name[APP_TML_MAX_MODE] =
{
    [APP_TML_8_BIT] = NAME_8BIT_MODE,
    [APP_TML_16_BIT] = NAME_16BIT_MODE,
    [APP_TML_32_BIT] = NAME_32BIT_MODE,
    [APP_TML_16_BIT_CAPTURE] = NAME_16BIT_CAPTURE_MODE,
};

/* TML periods in milliseconds */
static uint16_t g_tml_period_in_milli[APP_TML_MAX_MODE] =
{
    [APP_TML_8_BIT] = PERIOD_8BIT_MILLI,
    [APP_TML_16_BIT] = PERIOD_16BIT_MILLI,
    [APP_TML_32_BIT] = PERIOD_32BIT_MILLI,
};

/* LPM modes name */
static const char * gp_lpm_mode_name[APP_LPM_MAX_STATE] =
{
    [APP_LPM_SLEEP_STATE] = NAME_LPM_SLEEP_MODE,
    [APP_LPM_SW_STANDBY_STATE] = NAME_LPM_SW_STANDBY_MODE,
};

/* Private functions */
static void tml_counter_operation(uint16_t duration_time, app_tml_mode_t tml_mode);
static void tml_capture_operation(void);
static void tml_counter_operation_in_lpm(app_lpm_states_t lpm_mode);
static uint16_t tml_get_count_duration(void);
static uint16_t get_user_input(void);
static void user_input(char * buf, int len);
static void handle_error(fsp_err_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the TML example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void tml_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};
    uint16_t duration_time_s = RESET_VALUE;
    uint16_t duration_time_ms = RESET_VALUE;
    uint16_t selection = RESET_VALUE;

    /* Turn OFF all LEDs on board at start up */
    for (int i = 0; i < g_bsp_leds.led_count; i++)
    {
        set_led(i, LED_OFF);
    }

    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        for (int i = 0; i < g_bsp_leds.led_count; i++)
        {
            set_led(i, LED_ON);
        }
        return;     /* UART failed to open — cannot print anything */
    }

    /* Get FLEX pack version information */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the terminal */
    printf(BANNER_INFO, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    printf(EP_INFO);

    while (true)
    {
        printf(EP_MENU);
        selection = get_user_input();
        switch (selection)
        {
            case TML_COUNTER:
            {
                printf("\r\n\r\nDemonstrate TML counter mode\r\n");

                /* Get duration time for each TML counter mode */
                duration_time_s = tml_get_count_duration();
                printf("\r\nDuration time: %d seconds\r\n", duration_time_s);
                duration_time_ms = duration_time_s * SECOND_TO_MILLISECOND;

                /* The TML operates in 8-bit counter mode */
                tml_counter_operation(duration_time_ms, APP_TML_8_BIT);

                /* The TML operates in 16-bit counter mode */
                tml_counter_operation(duration_time_ms, APP_TML_16_BIT);

                /* The TML operates in 32-bit counter mode */
                tml_counter_operation(duration_time_ms, APP_TML_32_BIT);

                break;
            }

            case TML_CAPTURE:
            {
                printf("\r\n\r\nDemonstrate TML capture mode\r\n");

                /* The TML operates in 16-bit capture mode */
                tml_capture_operation();

                break;
            }

            case TML_COUNTER_IN_LPM:
            {
                printf("\r\n\r\nDemonstrate TML counter mode in LPM (Sleep and Software Standby modes)\r\n");

                /* Initialize TML 32-bit counter operates in LPM modes */
                err = tml_init(APP_TML_32_BIT_COUNTER_LPM);
                handle_error(err, "tml_init failed\r\n");

                /* The 32-bit counter TML operates in LPM Sleep mode */
                tml_counter_operation_in_lpm(APP_LPM_SLEEP_STATE);

                /* The 32-bit counter TML operates in LPM SW Standby mode */
                tml_counter_operation_in_lpm(APP_LPM_SW_STANDBY_STATE);

                /* De-initialize TML 32-bit counter operates in LPM modes */
                tml_close(APP_TML_32_BIT_COUNTER_LPM);
                break;
            }

            default:
            {
                printf("\r\nInvalid input!\r\n");
                break;
            }

        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function performs TML operation in counter modes.
 * @param[in] duration_time             Duration time value.
 * @param[in] tml_mode                  TML mode.
 * @retval    None.
 **********************************************************************************************************************/
static void tml_counter_operation(uint16_t duration_time, app_tml_mode_t tml_mode)
{
    fsp_err_t err = FSP_SUCCESS;
    uint16_t max_number_cycles = duration_time / g_tml_period_in_milli[tml_mode];

    /* Reset number cycles end */
    g_number_cycles = RESET_VALUE;

    /* Open the timer */
    err = tml_init(tml_mode);
    handle_error(err, "tml_init failed\r\n");
    printf("\r\n------------%s is running------------\r\n", sp_tml_mode_name[tml_mode]);
    printf("LED will be toggled after end of each cycle\r\n");

    /* Start the timer */
    err = tml_start(tml_mode);
    handle_error(err, "tml_start failed\r\n");

    while (true)
    {
        if (true == g_timer_flag[tml_mode])
        {
            /* Clear g_timer_flag flag */
            g_timer_flag[tml_mode] = false;

            /* Toggle on-board LED */
            /* In the TML 8-bit timer counter operation, the LED may be blink so quickly so we cannot see the LED operation */
            toggle_led();

            if (g_number_cycles >= max_number_cycles)
            {
                printf("Number cycles end of %s: %d\r\n", sp_tml_mode_name[tml_mode], g_number_cycles);

                /* Stop the timer */
                err = tml_stop(tml_mode);
                handle_error(err, "tml_stop failed\r\n");

                /* Close the timer */
                if ((APP_TML_8_BIT == tml_mode) || (APP_TML_16_BIT == tml_mode))
                {
                    printf("Close %s and open %s\r\n", sp_tml_mode_name[tml_mode], sp_tml_mode_name[tml_mode + 1]);
                }
                else
                {
                    printf("Close %s\r\n", sp_tml_mode_name[tml_mode]);
                }
                tml_close(tml_mode);

                break;
            }
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function performs TML operation in capture mode.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void tml_capture_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint16_t user_input = RESET_VALUE;

    /* Initialize timer 16-bit capture */
    err = tml_init(APP_TML_16_BIT_CAPTURE);
    handle_error(err, "tml_init failed\r\n");

    /* Start timer 16-bit capture */
    err = tml_start(APP_TML_16_BIT_CAPTURE);
    handle_error(err, "tml_start failed\r\n");

    /* Initialize ELC module */
    err = elc_init();
    handle_error(err, "elc_init failed\r\n");

    /* Enable ELC module */
    err = elc_enable();
    handle_error(err, "elc_enable failed\r\n");

    while (true)
    {
        printf(SUB_MENU2);
        user_input = get_user_input();
        if (CAPTURE == user_input)
        {
            /* Generate an ELC software event to trigger capture */
            err = elc_event_gen();
            handle_error(err, "elc_event_gen failed\r\n");
            while (false == g_timer_flag[APP_TML_16_BIT_CAPTURE])
            {
                /* Wait for capture flag */
            }
            g_timer_flag[APP_TML_16_BIT_CAPTURE] = false;
            printf("\r\nCapture times: %d\r\n", (unsigned int)g_captured_time);
        }
        else if (RETURN_MAIN_MENU == user_input)
        {
            /* De-initialize ELC module */
            elc_deinit();

            /* De-initialize timer 16-bit capture */
            tml_close(APP_TML_16_BIT_CAPTURE);
            break;
        }
        else
        {
            printf("\r\nInvalid input!\r\n");
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function performs TML operation in counter modes with low power modes.
 * @param[in] lpm_mode                  LPM mode.
 * @retval    None.
 **********************************************************************************************************************/
static void tml_counter_operation_in_lpm(app_lpm_states_t lpm_mode)
{
    fsp_err_t err = FSP_SUCCESS;
    uint16_t time_cancel_lpm = RESET_VALUE;

    /* Open this LPM instance */
    err = lpm_init(lpm_mode);
    handle_error(err, "lpm_init failed\r\n");

    /* Procedure before entering to LPM */
    /* Set the timer period to cancel from LPM mode */
    if (APP_LPM_SLEEP_STATE == lpm_mode)
    {
        printf("\r\n------------TML counter operation in Sleep mode------------\r\n");
    }
    else if (APP_LPM_SW_STANDBY_STATE == lpm_mode)
    {
        printf("\r\n------------TML counter operation in SW Standby mode------------\r\n");
    }
    else
    {
        /* Do nothing */
    }

    time_cancel_lpm = tml_get_count_duration();

    err = tml_set_timer_cancel_lpm(&time_cancel_lpm);
    handle_error(err, "tml_set_timer_cancel_lpm failed\r\n");

    /* Print LPM mode name */
    printf("%s. Please wait %d seconds to return to normal mode\r\n", gp_lpm_mode_name[lpm_mode], time_cancel_lpm);

    err = lpm_enter_procedure();
    handle_error(err, "lpm_enter_procedure failed\r\n");

    /* Enter LPM */
    err = lpm_mode_enter(lpm_mode);
    handle_error(err, "lpm_mode_enter failed\r\n");

    /* Procedure after exiting LPM */
    err = lpm_exit_procedure();
    handle_error(err, "lpm_exit_procedure failed\r\n");
    printf("MCU returned to the normal mode\r\n");

    /* Close this LPM instance */
    lpm_deinit();
}

/*******************************************************************************************************************//**
 * @brief     This function gets duration time for each TML counter mode.
 * @param[in] None
 * @retval    duration_time             Duration time value.
 **********************************************************************************************************************/
static uint16_t tml_get_count_duration(void)
{
    uint16_t duration_time = RESET_VALUE;

    while (true)
    {
        printf(SUB_MENU1);
        duration_time = get_user_input();
        if ((MAX_DURATION >= duration_time) && (MIN_DURATION < duration_time))
        {
            break;
        }
        else
        {
            printf("\r\nInvalid input!\r\n");
        }
    }

    return duration_time;
}

/*********************************************************************************************************************
 * @brief     This function gets the user input.
 * @param[in] None.
 * @retval    input_value               The user input.
 *********************************************************************************************************************/
static uint16_t get_user_input(void)
 {
    uint16_t input_value = RESET_VALUE;
    char input_buff[BUFFER_SIZE] = {RESET_VALUE};

    /* Wait until user data is available */
    while (!uart_console_has_char())
    {
        /* Do nothing */
    }

    /* Read user input from the terminal */
    user_input(input_buff, BUFFER_SIZE);

    /* Convert to integer value */
    input_value = (uint16_t)atoi((char*)input_buff);

    return input_value;
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
        tml_deinit();
        elc_deinit();
        lpm_deinit();
        __BKPT(0);
    }
}
