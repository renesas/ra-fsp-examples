/***********************************************************************************************************************
 * File Name    : tau_example_app.c
 * Description  : Example application layer for the TAU EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: tau_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tau_ep.h"
#include "tau_example_app.h"
#include "console/uart_console.h"

/* LED status flags */
static bool s_led2_status_flag = false;
static bool s_led1_status_flag_old = false;

/* Counting variables */
static uint8_t s_led1_blink_times = RESET_VALUE;
static uint8_t s_measure_times = RESET_VALUE;

/* TAU instances */
static timer_instance_t const * sp_tau_instance[TAU_MAX_MODE] =
{
    [INTERVAL_TIMER] = &g_interval_timer,
    [DELAY_COUNTER] = &g_delay_timer,
    [SQUARE_WAVE_SEQUENCE2] = &g_square_wave_output_sequence2,
    [DIVIDER] = &g_divider_timer,
    [INPUT_CAPTURE] = &g_input_capture,
    [EXTERNAL_EVENT_COUNT] = &g_external_event_counter_timer,
    [SQUARE_WAVE_SEQUENCE3] = &g_square_wave_output_sequence3,
    [LOW_LEVEL_MEASUREMENT] = &g_measure_low_level_pulse_width,
};

/* Private functions */
static void tau_interval_and_delay_timer_operation(void);
static void tau_external_event_counter_operation(void);
static void tau_measure_low_level_pulse_width_operation(void);
static uint32_t get_user_input(uint32_t min_value, uint32_t max_value);
static fsp_err_t led1_on_time_set(void);
static fsp_err_t led1_off_time_set(void);
static uint32_t square_wave_frequency_set(void);
static fsp_err_t square_wave_period_set(void);
static float divider_set(uint32_t square_wave_frequency);
static uint32_t max_timer_calculate(tau_mode_t tau_mode);
static float min_timer_calculate(tau_mode_t tau_mode);
static void user_input(char * buf, int len);
static void handle_error(fsp_err_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the TAU example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void tau_example_app_run(void)
{
    fsp_pack_version_t version = {RESET_VALUE};

    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        return;     /* UART failed to open — cannot print anything */
    }

    /* Get FSP version */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the terminal */
    printf(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    printf(EP_INFO);

    while (true)
    {
        /* Start interval and delay timer modes */
        printf(SEQUENCE1_INFO);
        tau_interval_and_delay_timer_operation();

        /* Go to external event counter mode */
        printf(SEQUENCE2_INFO);
        tau_external_event_counter_operation();

        /* Go to measure low level pulse width mode */
        printf(SEQUENCE3_INFO);
        tau_measure_low_level_pulse_width_operation();

        printf("\r\n*** End of application. Please enter any key to restart! ***\r\n");

        /* Wait until there is any user input */
        while (!uart_console_has_char())
        {
            /* Do nothing */
        }

        /* Read and discard the key that triggered uart_console_has_char() */
        getchar();
    }
}

/*******************************************************************************************************************//**
 * @brief     This function performs interval timer and delay timer operations by blinking LED1.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void tau_interval_and_delay_timer_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open TAU interval timer instance */
    err = tau_init(&g_interval_timer_ctrl, &g_interval_timer_cfg);
    handle_error(err, "tau_init for TAU interval timer instance failed\r\n");

    /* Open TAU delay timer instance */
    err = tau_init(&g_delay_timer_ctrl, &g_delay_timer_cfg);
    handle_error(err, "tau_init for TAU delay timer instance failed\r\n");

    /* Set ON LED1 time */
    err = led1_on_time_set();
    handle_error(err, "led1_on_time_set failed\r\n");

    /* Set OFF LED1 time */
    err = led1_off_time_set();
    handle_error(err, "led1_off_time_set failed\r\n");

    /* Start TAU interval timer instance after set period blink for LED1 */
    err = tau_start(&g_interval_timer_ctrl);
    handle_error(err, "tau_start for TAU interval timer instance failed\r\n");

    /* Enable TAU delay timer instance */
    err = tau_enable(&g_delay_timer_ctrl);
    handle_error(err, "tau_enable for TAU delay timer instance failed\r\n");

    printf("\r\nPlease observe LED1's behavior to verify\r\n");

    while (true)
    {
        if (g_delay_timer_end_flag)
        {
            g_delay_timer_end_flag = false;
            g_led1_status_flag_cur = false;

            /* Reset TAU interval timer instance */
            err = tau_reset(&g_interval_timer_ctrl);
            handle_error(err, "tau_reset for TAU interval timer instance failed\r\n");
        }

        /* Check blink times of LED1 */
        if (DEMO_TIMES > s_led1_blink_times)
        {
            /* Toggle LED1 status if g_led1_status_flag changes */
            if (s_led1_status_flag_old != g_led1_status_flag_cur)
            {
                s_led1_status_flag_old = g_led1_status_flag_cur;
                if (true == s_led1_status_flag_old)
                {
                    R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[LED_SEQUENCE1], BSP_IO_LEVEL_HIGH);
                }
                else
                {
                    R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[LED_SEQUENCE1], BSP_IO_LEVEL_LOW);
                    s_led1_blink_times++;
                }
            }
        }
        else
        {
            /* Close TAU interval timer instance */
            tau_close(&g_interval_timer_ctrl);

            /* Close TAU delay timer instance */
            tau_close(&g_delay_timer_ctrl);
            printf("\r\nLED1 blink times: %d\r\n", s_led1_blink_times);

            /* Reset LED1 blink times */
            s_led1_blink_times = RESET_VALUE;
            printf("\r\n*** Go to external event counter mode ***\r\n");

            break;
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function measures the input signal frequency and performs external event counting using LED2.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void tau_external_event_counter_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;
    float input_pulse_frequency_value = RESET_VALUE;
    uint32_t square_wave_frequency = RESET_VALUE;
    uint32_t external_event_count = RESET_VALUE;
    uint32_t input_pulse_max = RESET_VALUE;
    float input_pulse_min = RESET_VALUE;
    float divided_output = RESET_VALUE;

    /* Open TAU square wave output sequence 2 timer instance */
    err = tau_init(&g_square_wave_output_sequence2_ctrl, &g_square_wave_output_sequence2_cfg);
    handle_error(err, "tau_init for TAU square wave output sequence 2 timer instance failed\r\n");

    /* Open TAU divider timer instance */
    err = tau_init(&g_divider_timer_ctrl, &g_divider_timer_cfg);
    handle_error(err, "tau_init for TAU divider timer instance failed\r\n");

    /* Open TAU input capture timer instance */
    err = tau_init(&g_input_capture_ctrl, &g_input_capture_cfg);
    handle_error(err, "tau_init for TAU input capture timer instance failed\r\n");

    /* Open TAU external event counter timer instance */
    err = tau_init(&g_external_event_counter_timer_ctrl, &g_external_event_counter_timer_cfg);
    handle_error(err, "tau_init for TAU external event counter timer instance failed\r\n");

    /* The limit of measurable input signal is based on the configured clock frequency */
    input_pulse_min = min_timer_calculate(INPUT_CAPTURE);
    input_pulse_max = max_timer_calculate(INPUT_CAPTURE);
    printf("\r\nInput signal range can be measured: %.03fHz to %dHz\r\n", input_pulse_min, input_pulse_max);

    /* Set frequency for square wave */
    square_wave_frequency = square_wave_frequency_set();

    /* Set divider */
    divided_output = divider_set(square_wave_frequency);

    /* Print warning message if the input pulse is out of the measurable range */
    if (input_pulse_min > divided_output || (float)input_pulse_max < divided_output)
    {
        printf(MEASURABLE_WARNING);
    }

    /* Start TAU square wave output sequence 2 timer instance after set frequency */
    err = tau_start(&g_square_wave_output_sequence2_ctrl);
    handle_error(err, "tau_start for TAU square wave output sequence 2 timer instance failed\r\n");

    /* Start TAU divider timer instance */
    err = tau_start(&g_divider_timer_ctrl);
    handle_error(err, "tau_start for TAU divider timer instance failed\r\n");

    /* Enable TAU input capture timer instance to measure pulse width */
    err = tau_enable(&g_input_capture_ctrl);
    handle_error(err, "tau_enable for TAU input capture timer instance failed\r\n");

    /* Enable TAU external event counter timer instance to count the ON times of LED2 */
    err = tau_enable(&g_external_event_counter_timer_ctrl);
    handle_error(err, "tau_enable for TAU external event counter timer instance failed\r\n");

    printf("\r\nPlease observe LED2's behavior to verify\r\n");

    while (true)
    {
        /* Check capture flag and ignore the first measurement since it could have started in the middle of a pulse */
        if (g_input_capture_detect_flag && (g_interrupt_counter > 1U))
        {
            /* Toggle LED2 when a capture event is detected */
            s_led2_status_flag = s_led2_status_flag ? false : true;
            if (s_led2_status_flag)
            {
                /* Turn ON LED2 */
                R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[LED_SEQUENCE2], BSP_IO_LEVEL_HIGH);
                err = tau_info_get(&g_input_capture_ctrl, &g_periodic_info);
                handle_error(err, "tau_info_get for TAU input capture timer instance failed\r\n");

                /* Calculate input pulse width */
                input_pulse_frequency_value = (float) g_periodic_info.clock_frequency / (float) g_capture_value;
                printf("\r\nInput pulse width measurement value: %.03fHz\r\n", input_pulse_frequency_value);
                external_event_count++;

                /* Reset captured timer count value */
                g_capture_value = RESET_VALUE;
            }
            else
            {
                /* Turn OFF LED2 */
                R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[LED_SEQUENCE2], BSP_IO_LEVEL_LOW);
            }

            /* Reset input capture detect flag */
            g_input_capture_detect_flag = false;
        }
        else if (g_external_counter_flag)
        {
            /* Print external event count to confirm with configuration */
            printf("\r\nExternal event count: %d\r\n", external_event_count);
            if (external_event_count == g_external_event_counter_timer_cfg.period_counts)
            {
                printf("\r\n*** External event count operation completed successfully ***\n");
            }
            else
            {
                /* Return error due to external event count value not matching expectation */
                handle_error(FSP_ERR_ASSERTION, "External event count operation did not work as expected\r\n");
            }

            /* Reset external counter flag */
            g_external_counter_flag = false;

            /* Reset interrupt counter */
            g_interrupt_counter = RESET_VALUE;

            /* Reset external event count */
            external_event_count = RESET_VALUE;

            /* Close TAU square wave output sequence 2 timer instance */
            tau_close(&g_square_wave_output_sequence2_ctrl);

            /* Close TAU divider timer instance */
            tau_close(&g_divider_timer_ctrl);

            /* Close TAU input capture timer instance */
            tau_close(&g_input_capture_ctrl);

            /* Close TAU external event counter timer instance */
            tau_close(&g_external_event_counter_timer_ctrl);
            printf("\r\n*** Go to measure low level pulse width mode ***\n");

            break;
        }
        else
        {
            /* Do nothing */
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function measures the low level pulse width of square wave which is generated by
 *            another TAU channel.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void tau_measure_low_level_pulse_width_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;
    float low_level_pulse_width = RESET_VALUE;

    /* Open TAU square wave output sequence 3 timer instance */
    err = tau_init(&g_square_wave_output_sequence3_ctrl, &g_square_wave_output_sequence3_cfg);
    handle_error(err, "tau_init for TAU square wave output sequence 3 timer instance failed\r\n");

    /* Open TAU measure low level pulse width timer instance */
    err = tau_init(&g_measure_low_level_pulse_width_ctrl, &g_measure_low_level_pulse_width_cfg);
    handle_error(err, "tau_init for TAU measure low level pulse width timer instance failed\r\n");

    /* Enable TAU measure low level pulse width of square wave timer instance */
    err = tau_enable(&g_measure_low_level_pulse_width_ctrl);
    handle_error(err, "tau_enable for TAU measure low level pulse width of square wave timer instance failed\r\n");

    /* Set period of square wave */
    err = square_wave_period_set();
    handle_error(err, "square_wave_period_set failed\r\n");

    /* Start TAU square wave output sequence 3 timer instance after set period */
    err = tau_start(&g_square_wave_output_sequence3_ctrl);
    handle_error(err, "tau_start TAU square wave output sequence 3 timer instance failed\r\n");

    while (DEMO_TIMES > s_measure_times)
    {
        /* Check measure flag and ignore the first measurement since it could have started in the middle of a pulse */
        if (g_measure_pulse_width_flag && (g_interrupt_counter > 1U))
        {
            /* Get clock frequency to calculate pulse width */
            err = tau_info_get(&g_measure_low_level_pulse_width_ctrl, &g_periodic_info);
            handle_error(err, "tau_info_get for measure low level pulse width failed\r\n");

            /* Calculate low level pulse width */
            low_level_pulse_width = ((float) g_capture_value / (float) g_periodic_info.clock_frequency) *
                                    CONVERT_SECOND_TO_MILLISECOND;
            printf("\r\nLow level pulse width measurement value: %.03fms\r\n", low_level_pulse_width);

            /* Reset g_measure_pulse_width_flag */
            g_measure_pulse_width_flag = false;

            /* Reset g_capture_value */
            g_capture_value = RESET_VALUE;
            s_measure_times++;
        }
    }

    /* Close TAU square wave output sequence 3 timer instance */
    tau_close(&g_square_wave_output_sequence3_ctrl);

    /* Close TAU measure low level pulse width timer instance */
    tau_close(&g_measure_low_level_pulse_width_ctrl);

    /* Reset low level measurement times */
    s_measure_times = RESET_VALUE;

    /* Reset interrupt counter */
    g_interrupt_counter = RESET_VALUE;
}

/*******************************************************************************************************************//**
 * @brief     This function gets the user input.
 * @param[in] min_value                 Minimum value of input range.
 * @param[in] max_value                 Maximum value of input range.
 * @retval    input_value               Returns input buffer after receive enter key.
 *********************************************************************************************************************/
static uint32_t get_user_input(uint32_t min_value, uint32_t max_value)
{
    uint32_t input_value = RESET_VALUE;
    char input_buff[BUFFER_SIZE] = {NULL_CHAR};

    while (true)
    {
        /* Wait until there is any user input */
        while (!uart_console_has_char())
        {
            /* Do nothing */
        }

        /* Get the user option */
        memset(&input_buff[0], NULL_CHAR, BUFFER_SIZE);
        user_input(input_buff, (int) sizeof(input_buff));
        input_value = (uint32_t) atoi(input_buff);
        if ((input_value >= min_value) && (input_value <= max_value))
        {
            break;
        }
        else
        {
            printf("\r\nInvalid input. Please enter a valid input between %u and %u.\r\n", min_value, max_value);

            /* Reset user input value if it is an invalid input */
            input_value = RESET_VALUE;
        }
    }

    return input_value;
}

/*******************************************************************************************************************//**
 * @brief     This function sets the ON time for LED1.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Set successfully.
 * @retval    Any other error codes     Failed to set.
 **********************************************************************************************************************/
static fsp_err_t led1_on_time_set(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t time_period_ms_led_on = RESET_VALUE;
    uint32_t raw_counts = RESET_VALUE;
    uint32_t time_period_max = RESET_VALUE;

    /* Calculate maximum configurable value of interval timer */
    time_period_max = max_timer_calculate(DELAY_COUNTER);
    printf("\r\nValid range: 1ms to %dms", time_period_max);
    printf("\r\nPlease enter time period values (ms) for LED1 ON\r\nUser input: ");

    /* Wait for user input */
    time_period_ms_led_on = get_user_input(TIME_PERIOD_MIN, time_period_max);
    printf("\r\nTime period for LED1 ON: %dms\r\n", time_period_ms_led_on);

    /* Depend on the user selected clock source, raw counts value can be calculated
       for the user given time-period values */
     err = tau_info_get(&g_delay_timer_ctrl, &g_periodic_info);
     if (FSP_SUCCESS != err)
     {
         return err;
     }
     raw_counts = (uint32_t) ((time_period_ms_led_on * g_periodic_info.clock_frequency) /
                  CONVERT_SECOND_TO_MILLISECOND);

     /* Set period value */
     err = tau_period_set(&g_delay_timer_ctrl, raw_counts);
     if (FSP_SUCCESS != err)
     {
         return err;
     }

     /* Reset LED1 ON time value */
     time_period_ms_led_on = RESET_VALUE;

     return err;
}

/*******************************************************************************************************************//**
 * @brief     This function sets the OFF time for LED1.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Set successfully.
 * @retval    Any other error codes     Failed to set.
 **********************************************************************************************************************/
static fsp_err_t led1_off_time_set(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t time_period_ms_led_off = RESET_VALUE;
    uint32_t raw_counts = RESET_VALUE;
    uint32_t time_period_max = RESET_VALUE;

    /* Calculate maximum configurable value of delay counter */
    time_period_max = max_timer_calculate(INTERVAL_TIMER);
    printf("\r\nValid range: 1ms to %dms", time_period_max);
    printf("\r\nPlease enter time period values (ms) for LED1 OFF\r\nUser input: ");

    /* Wait for user input */
    time_period_ms_led_off = get_user_input(TIME_PERIOD_MIN, time_period_max);
    printf("\r\nTime period for LED1 OFF: %dms\r\n", time_period_ms_led_off);

    /* Calculation of raw counts value for given milliseconds value */
    err = tau_info_get(&g_interval_timer_ctrl, &g_periodic_info);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Depend on the user selected clock source, raw counts value can be calculated
     * for the user given time-period values */
    raw_counts = (uint32_t) ((time_period_ms_led_off * g_periodic_info.clock_frequency) /
                 CONVERT_SECOND_TO_MILLISECOND);

    /* Set period value */
    err = tau_period_set(&g_interval_timer_ctrl, raw_counts);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Reset LED1 OFF time value */
    time_period_ms_led_off = RESET_VALUE;

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function sets the frequency of TAU square wave of sequence 2 timer instance.
 * @param[in] None.
 * @retval    square_wave_frequency     Frequency is set by the user.
 **********************************************************************************************************************/
static uint32_t square_wave_frequency_set(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t square_wave_frequency = RESET_VALUE;
    uint32_t raw_counts = RESET_VALUE;
    uint32_t square_wave_frequency_max = RESET_VALUE;

    /* Calculate maximum configurable value of square wave sequence 2 */
    square_wave_frequency_max = max_timer_calculate(SQUARE_WAVE_SEQUENCE2);

    /* Print requirement of frequency input */
    printf(FREQUENCY_INPUT, square_wave_frequency_max);
    printf("\r\nValid range: 1Hz to %dHz", square_wave_frequency_max);
    printf("\r\nPlease enter square wave frequency (Hz)\r\nUser input: ");

    /* Wait for user input */
    square_wave_frequency = get_user_input(SQUARE_WAVE_FREQUENCY_MIN, square_wave_frequency_max);
    printf("\r\nSquare wave frequency: %dHz\r\n", square_wave_frequency);

    err = tau_info_get(&g_square_wave_output_sequence2_ctrl, &g_periodic_info);
    handle_error(err, "tau_info_get for TAU square wave output sequence 2 timer instance failed\r\n");

    /* Depend on the user selected clock source, raw counts value can be calculated
     * for the user given square wave frequency values */
    raw_counts = (uint32_t) (g_periodic_info.clock_frequency / (square_wave_frequency * CONVERT_HALF_TO_ONE_CYCLE));

    /* Set period value */
    err = tau_period_set(&g_square_wave_output_sequence2_ctrl, raw_counts);
    handle_error(err, "tau_period_set for TAU square wave output sequence 2 timer instance failed\r\n");

    return square_wave_frequency;
}

/*******************************************************************************************************************//**
 * @brief     This function sets the divider.
 * @param[in] square_wave_frequency     Maximum of divider corresponding with selected frequency previous.
 * @retval    divided_output            The frequency of divided square wave.
 **********************************************************************************************************************/
static float divider_set(uint32_t square_wave_frequency)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t divider = RESET_VALUE;
    float divided_output = RESET_VALUE;

    printf("\r\nValid range: 1 to %d", square_wave_frequency);
    printf("\r\nPlease enter clock division\r\nUser input: ");

    /* Get the user input */
    divider = get_user_input(DIVIDER_MIN, square_wave_frequency);
    printf("\r\nSelected divider: %d\r\n", divider);

    /* Print the expected divided frequency of the square wave */
    divided_output = (float) square_wave_frequency / (float) divider;
    printf("\r\nExpected divided frequency of the square wave: %.03fHz\r\n", divided_output);

    /* Set period value for TAU divider timer instance */
    err = tau_period_set(&g_divider_timer_ctrl, divider);
    handle_error(err, "tau_period_set for TAU divider timer instance failed\r\n");

    /* Reset divider */
    divider = RESET_VALUE;

    return divided_output;
}

/*******************************************************************************************************************//**
 * @brief     This function calculates the maximum configurable value for the selected TAU mode.
 * @param[in] tau_mode                  Selected TAU mode.
 * @retval    max_val                   Maximum value for the selected timer.
 **********************************************************************************************************************/
static uint32_t max_timer_calculate(tau_mode_t tau_mode)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t max_val = RESET_VALUE;

    if (TAU_MAX_MODE <= tau_mode)
    {
        handle_error(FSP_ERR_INVALID_MODE, "Invalid mode\r\n");
    }

    err = tau_info_get(sp_tau_instance[tau_mode]->p_ctrl, &g_periodic_info);
    handle_error(err, "tau_info_get for maximum value failed\r\n");

    if ((INTERVAL_TIMER == tau_mode) || (DELAY_COUNTER == tau_mode))
    {
        /* Determine the maximum values of interval timer and delay counter corresponding to the selected
         * TAU clock source */
        max_val = ((MAX_PERIOD * CONVERT_SECOND_TO_MILLISECOND) / g_periodic_info.clock_frequency);
    }
    else if ((SQUARE_WAVE_SEQUENCE2 == tau_mode) || (INPUT_CAPTURE == tau_mode))
    {
        /* Determine the maximum frequency of square wave output sequence 2 and the maximum measurable
         * input pulse signal corresponding to the selected TAU clock source */
        max_val = ((g_periodic_info.clock_frequency / MIN_PERIOD) / CONVERT_HALF_TO_ONE_CYCLE);
    }
    else if (SQUARE_WAVE_SEQUENCE3 == tau_mode)
    {
        /* Determine the maximum period of square wave output sequence 3 corresponding to the selected
         * TAU clock source */
        max_val = (((MAX_PERIOD * CONVERT_HALF_TO_ONE_CYCLE) * CONVERT_SECOND_TO_MILLISECOND) /
                   g_periodic_info.clock_frequency);
    }
    else
    {
        /* Do nothing */
    }

    return max_val;
}

/*******************************************************************************************************************//**
 * @brief     This function calculates the minimum measurable value for the selected TAU mode.
 * @param[in] tau_mode                  Selected TAU mode.
 * @retval    min_val                   Minimum value for the selected timer.
 **********************************************************************************************************************/
static float min_timer_calculate(tau_mode_t tau_mode)
{
    fsp_err_t err = FSP_SUCCESS;
    float min_val = RESET_VALUE;

    if (TAU_MAX_MODE <= tau_mode)
    {
        handle_error(FSP_ERR_INVALID_MODE, "Invalid mode\r\n");
    }

    err = tau_info_get(sp_tau_instance[tau_mode]->p_ctrl, &g_periodic_info);
    handle_error(err, "tau_info_get for minimum value failed\r\n");

    if (INPUT_CAPTURE == tau_mode)
    {
        /* Determine the minimum frequency can be measured by input capture operation corresponding to the selected
         * TAU clock source */
        min_val = (float) g_periodic_info.clock_frequency / (float) CAPTURE_VALUE_MAX;
    }

    return min_val;
}

/*******************************************************************************************************************//**
 * @brief     This function sets the period of TAU square wave output sequence 3 timer instance.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Set successfully.
 * @retval    Any other error codes     Failed to set.
 **********************************************************************************************************************/
static fsp_err_t square_wave_period_set(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t square_wave_period = RESET_VALUE;
    uint32_t raw_counts = RESET_VALUE;
    uint32_t square_wave_period_max = RESET_VALUE;

    /* Calculate maximum configurable value of TAU square wave output sequence 3 timer instance */
    square_wave_period_max = max_timer_calculate(SQUARE_WAVE_SEQUENCE3);
    printf("\r\nValid range: 1ms to %dms", square_wave_period_max);
    printf("\r\nPlease enter square wave period (ms)\r\nUser input: ");

    /* Get the user input */
    square_wave_period = get_user_input(SQUARE_WAVE_PERIOD_MIN, square_wave_period_max);
    printf("\r\nSquare wave period: %dms\r\n", square_wave_period);

    /* Calculation of raw counts value for given milliseconds value */
    err = tau_info_get(&g_square_wave_output_sequence3_ctrl, &g_periodic_info);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Depend on the user selected clock source, raw counts value can be calculated
     * for the user given square wave period values */
    raw_counts = (uint32_t) ((g_periodic_info.clock_frequency * square_wave_period) /
                 (CONVERT_HALF_TO_ONE_CYCLE * CONVERT_SECOND_TO_MILLISECOND));

    /* Set period value */
    err = tau_period_set(&g_square_wave_output_sequence3_ctrl, raw_counts);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Reset period of square wave */
    square_wave_period = RESET_VALUE;

    return err;
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
        printf("Returned Error Code: 0x%x\r\n", (unsigned int) err);
        uart_console_flush();
        tau_deinit();
        __BKPT(0);
    }
}
