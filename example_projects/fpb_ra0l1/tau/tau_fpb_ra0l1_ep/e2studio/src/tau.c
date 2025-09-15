/***********************************************************************************************************************
 * File Name    : tau.c
 * Description  : Contains declarations of data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "tau.h"

/*******************************************************************************************************************//**
 * @addtogroup tau
 * @{
 **********************************************************************************************************************/

extern bsp_leds_t g_bsp_leds;

/* TAU callback flag */
static volatile _Bool g_led1_status_flag_cur      = false;
static volatile _Bool g_input_capture_detect_flag = false;
static volatile _Bool g_external_counter_flag     = false;
static volatile _Bool g_measure_pulse_width_flag  = false;
static volatile _Bool g_delay_timer_end_flag      = false;

/* Led status flag */
static _Bool g_led2_status_flag     = false;
static _Bool g_led1_status_flag_old = false;

/* Counting valid edge */
static uint32_t g_capture_value = RESET_VALUE;

/* The buffer contains user input */
char user_input[TERM_BUFFER_SIZE + 1] = {RESET_VALUE};

/* Counting variable */
static uint8_t g_led1_blink_times  = RESET_VALUE;
static uint8_t g_interrupt_counter = RESET_VALUE;
static uint8_t g_measure_times     = RESET_VALUE;

/* Timer information */
static timer_info_t periodic_info =
{
 .clock_frequency = RESET_VALUE,
 .count_direction = (timer_direction_t) RESET_VALUE,
 .period_counts = RESET_VALUE,
};

/* TAU instances */
static timer_instance_t const * g_tau_instance [TAU_MAX_MODE] =
{
    [INTERVAL_TIMER]        = &g_interval_timer,
    [DELAY_COUNTER]         = &g_delay_timer,
    [SQUARE_WAVE_SEQUENCE2] = &g_square_wave_output_sequence2,
    [DIVIDER]               = &g_divider_timer,
    [INPUT_CAPTURE]         = &g_input_capture,
    [EXTERNAL_EVENT_COUNT]  = &g_external_event_counter_timer,
    [SQUARE_WAVE_SEQUENCE3] = &g_square_wave_output_sequence3,
    [LOW_LEVEL_MEASUREMENT] = &g_measure_low_level_pulse_width,
};

/* Private function declarations */
static fsp_err_t tau_interval_and_delay_timer_operation(void);
static fsp_err_t tau_external_event_counter_operation(void);
static fsp_err_t tau_measure_low_level_pulse_width_operation(void);
static fsp_err_t led1_on_time_set(void);
static fsp_err_t led1_off_time_set(void);
static uint32_t square_wave_frequency_set(void);
static fsp_err_t square_wave_period_set(void);
static float divider_set(uint32_t square_wave_frequency);
static void tau_deinit(void);
static void handle_error(fsp_err_t err, uint8_t * err_str);
static uint32_t max_timer_calculate(tau_mode_t tau_mode);
static float min_timer_calculate(tau_mode_t tau_mode);
static uint32_t get_user_input(uint32_t min_value, uint32_t max_value);

/*******************************************************************************************************************//**
 *  @brief       This function is used to start the tau example operation.
 *  @param[in]   None
 *  @retval      None
 **********************************************************************************************************************/
void tau_entry(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = { RESET_VALUE };

#if (USE_VIRTUAL_COM == 1)
    /* Initialize UARTA module first to print log to serial terminal */
    err = TERM_INIT();
    if (FSP_SUCCESS != err)
    {
        /* Error trap */
        __asm("BKPT #0\n");
    }
#endif /* USE_VIRTUAL_COM */

    /* Get FSP version */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the terminal */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor,\
              version.version_id_b.patch);
    APP_PRINT(EP_INFO);

    while (true)
    {
        /* Start interval and delay timer modes */
        APP_PRINT(SEQUENCE1_INFO);
        err = tau_interval_and_delay_timer_operation();
        APP_ERR_HANDLE(err, "\r\n**tau_interval_and_delay_timer_operation failed**");

        /* Go to external event counter mode */
        APP_PRINT(SEQUENCE2_INFO);
        err = tau_external_event_counter_operation();
        APP_ERR_HANDLE(err, "\r\n**tau_external_event_counter_operation failed**");

        /* Go to measure low level pulse width mode */
        APP_PRINT(SEQUENCE3_INFO);
        err = tau_measure_low_level_pulse_width_operation();
        APP_ERR_HANDLE(err, "\r\n**tau_measure_low_level_pulse_width_operation failed**");

        APP_PRINT("\r\n=======End of Application. Please enter any key to restart!=======\r\n");
        while (!APP_CHECK_KEY)
        {
            __NOP();
        }

        /* Dummy read to clear input data */
        APP_READ(&user_input[0], TERM_BUFFER_SIZE);
    }
}

/*******************************************************************************************************************//**
 *  @brief       This function is used to demonstrate the interval timer and the delay timer by set cycle blink of LED1
 *  @param[in]   None
 *  @retval      FSP_SUCCESS    Upon successful operation.
 *  @retval      Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
static fsp_err_t tau_interval_and_delay_timer_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open instance TAU interval timer */
    err = R_TAU_Open(&g_interval_timer_ctrl, &g_interval_timer_cfg);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**Open instance TAU interval timer failed**");
    /* Open instance TAU delay timer */
    err = R_TAU_Open(&g_delay_timer_ctrl, &g_delay_timer_cfg);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**Open instance TAU delay timer failed**");

    /* Set LED1 turn-on time */
    err = led1_on_time_set();
    APP_ERR_RET(FSP_SUCCESS != err, err, "Set turn-on time for LED1 failed");
    /* Set LED1 turn-off time */
    err = led1_off_time_set();
    APP_ERR_RET(FSP_SUCCESS != err, err, "Set turn-off time for LED1 failed");

    /* Start instance TAU interval timer after set period blink for LED1 */
    err = R_TAU_Start(&g_interval_timer_ctrl);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**Start instance TAU interval timer failed**");

    /* Enable instance TAU delay timer */
    err = R_TAU_Enable(&g_delay_timer_ctrl);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**Enable instance TAU delay timer failed**");

    APP_PRINT("\r\nPlease observe LED1's behavior to verify\r\n");
    while (true)
    {
        if (g_delay_timer_end_flag)
        {
            g_delay_timer_end_flag = false;
            g_led1_status_flag_cur = false;
            /* Reset interval timer */
            err = R_TAU_Reset(&g_interval_timer_ctrl);
            APP_ERR_RET(FSP_SUCCESS != err, err, "Reset interval timer failed");
        }

        /* Check blink times of LED1 */
        if (g_led1_blink_times < DEMO_TIMES)
        {
            /* Toggle LED1 status if g_led1_status_flag changes */
            if (g_led1_status_flag_old != g_led1_status_flag_cur)
            {
                g_led1_status_flag_old = g_led1_status_flag_cur;
                if (true == g_led1_status_flag_old)
                {
                    R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[LED_SEQUENCE1], BSP_IO_LEVEL_HIGH);
                }
                else
                {
                    R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[LED_SEQUENCE1], BSP_IO_LEVEL_LOW);
                    g_led1_blink_times++;
                }
            }
        }
        else
        {
            /* Close instance TAU interval timer */
            err = R_TAU_Close(&g_interval_timer_ctrl);
            APP_ERR_RET(FSP_SUCCESS != err, err, "Close instance TAU interval timer failed");
            /* Close instance TAU delay timer */
            err = R_TAU_Close(&g_delay_timer_ctrl);
            APP_ERR_RET(FSP_SUCCESS != err, err, "Close instance TAU delay timer failed");

            APP_PRINT("\r\nLED1 blink times: %d\r\n", g_led1_blink_times);
            /* Reset LED1 blink times */
            g_led1_blink_times = RESET_VALUE;

            APP_PRINT("\r\n*** Go to external event counter mode ***\r\n");
            break;
        }
    }
    return err;
}

/*******************************************************************************************************************//**
 *  @brief       This function is used to measure the input pulse width of square wave from another TAU channel
 *               and count blink times of LED2.
 *  @param[in]   None
 *  @retval      FSP_SUCCESS    Upon successful operation.
 *  @retval      Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
static fsp_err_t tau_external_event_counter_operation(void)
{
    fsp_err_t err                         = FSP_SUCCESS;
    float     input_pulse_frequency_value = RESET_VALUE;
    char      timer_buffer[BUFF_SIZE]     = {RESET_VALUE};
    uint32_t  square_wave_frequency       = RESET_VALUE;
    uint32_t  external_event_count        = RESET_VALUE;
    uint32_t  input_pulse_max             = RESET_VALUE;
    float     input_pulse_min             = RESET_VALUE;
    float     divided_output              = RESET_VALUE;

    /* Open instance TAU square wave output sequence 2 */
    err = R_TAU_Open(&g_square_wave_output_sequence2_ctrl, &g_square_wave_output_sequence2_cfg);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**Open instance TAU square wave output sequence 2 failed**");
    /* Open instance TAU divider */
    err = R_TAU_Open(&g_divider_timer_ctrl, &g_divider_timer_cfg);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**Open instance TAU divider failed**");
    /* Open instance TAU input capture */
    err = R_TAU_Open(&g_input_capture_ctrl, &g_input_capture_cfg);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**Open instance TAU input capture failed**");
    /* Open instance TAU external event counter */
    err = R_TAU_Open(&g_external_event_counter_timer_ctrl, &g_external_event_counter_timer_cfg);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**Open instance TAU external event counter failed**");

    /* The limit of measurable input signal is based on the configured clock frequency */
    input_pulse_min = min_timer_calculate(INPUT_CAPTURE);
    input_pulse_max = max_timer_calculate(INPUT_CAPTURE);
    sprintf(timer_buffer, "%.03f", input_pulse_min);
    APP_PRINT("\r\nInput signal range can be measured: from %sHz to %dHz\r\n", timer_buffer, input_pulse_max);

    /* Set frequency for square wave */
    square_wave_frequency = square_wave_frequency_set();
    /* Set divider */
    divided_output = divider_set(square_wave_frequency);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**Set divider failed**");

    /* Start instance TAU square wave output sequence 2 after set frequency */
    err = R_TAU_Start(&g_square_wave_output_sequence2_ctrl);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**Start instance TAU square wave output sequence 2 failed**");
    /* Start divider square wave */
    err = R_TAU_Start(&g_divider_timer_ctrl);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**Start instance TAU divider timer failed**");
    /* Enable instance TAU input capture to measure pulse width */
    err = R_TAU_Enable(&g_input_capture_ctrl);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**Enable instance TAU input capture failed**");
    /* Enable instance TAU external event counter to count turn-on times of LED2 */
    err = R_TAU_Enable(&g_external_event_counter_timer_ctrl);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**Enable instance TAU external event counter failed**");

    /* Print warning message if the input pulse is out of the measurable range */
    if (input_pulse_min > divided_output || (float)input_pulse_max < divided_output)
    {
        APP_PRINT(MEASURABLE_WARNING);
    }

    APP_PRINT("\r\nPlease observe LED2's behavior to verify\r\n");
    while (true)
    {
        /* Check capture flag and ignore the first measurement since it could have started in the middle of a pulse */
        if (g_input_capture_detect_flag && g_interrupt_counter > ONE)
        {
            /* Blink LED2 once rising edge detected */
            g_led2_status_flag = g_led2_status_flag ? false : true;
            if (g_led2_status_flag)
            {
                /* Turn ON LED2 */
                R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[LED_SEQUENCE2], BSP_IO_LEVEL_HIGH);
                err = R_TAU_InfoGet(&g_input_capture_ctrl, &periodic_info);
                APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**Get periodic info of input capture failed**");
                /* Calculate input pulse width */
                input_pulse_frequency_value = (float) ((float) (periodic_info.clock_frequency) / (float)\
                                              (g_capture_value));
                sprintf(timer_buffer, "%.03f", input_pulse_frequency_value);
                APP_PRINT("\r\nInput pulse width measurement value: %sHz\r\n", timer_buffer);
                external_event_count++;
                /* Reset captured valid edge */
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
            APP_PRINT("\r\nExternal event count: %d\n", external_event_count);
            if (external_event_count == g_external_event_counter_timer_cfg.period_counts)
            {
                APP_PRINT("\r\n*** External event count operation completed successfully ***\n");
            }
            else
            {
                err = FSP_ERR_ASSERTION;
                /* Return error due to external event count value not matching expectation */
                APP_ERR_RET(FSP_SUCCESS != err, err, "External event count operation did not work as expected");
            }

            /* Reset external counter flag */
            g_external_counter_flag = false;
            /* Reset interrupt counter */
            g_interrupt_counter = RESET_VALUE;
            /* Reset external event count */
            external_event_count = RESET_VALUE;

            /* Close instance TAU square wave output sequence 2 */
            err = R_TAU_Close(&g_square_wave_output_sequence2_ctrl);
            APP_ERR_RET(FSP_SUCCESS != err, err, "Close instance TAU square wave output sequence 2 failed");
            /* Close instance TAU divider */
            err = R_TAU_Close(&g_divider_timer_ctrl);
            APP_ERR_RET(FSP_SUCCESS != err, err, "Close instance TAU divider failed");
            /* Close instance TAU input capture */
            err = R_TAU_Close(&g_input_capture_ctrl);
            APP_ERR_RET(FSP_SUCCESS != err, err, "Close instance TAU input capture failed");
            /* Close instance TAU external event counter */
            err = R_TAU_Close(&g_external_event_counter_timer_ctrl);
            APP_ERR_RET(FSP_SUCCESS != err, err, "Close instance TAU external event counter failed");

            APP_PRINT("\r\n*** Go to measure low level pulse width mode ***\n");
            break;
        }
        else
        {
            /* Do nothing */
        }
    }
    return err;
}

/*******************************************************************************************************************//**
 *  @brief       This function is used to measure the low level pulse width of square wave, which is generated by
 *               another TAU channel.
 *  @param[in]   None
 *  @retval      FSP_SUCCESS    Upon successful operation.
 *  @retval      Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
static fsp_err_t tau_measure_low_level_pulse_width_operation(void)
{
    fsp_err_t err                     = FSP_SUCCESS;
    float     low_level_pulse_width   = RESET_VALUE;
    char      timer_buffer[BUFF_SIZE] = {RESET_VALUE};

    /* Open instance TAU square wave output sequence 3 */
    err = R_TAU_Open(&g_square_wave_output_sequence3_ctrl, &g_square_wave_output_sequence3_cfg);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**Open instance TAU square wave output sequence 3 failed**");
    /* Open instance TAU measure low level pulse width */
    err = R_TAU_Open(&g_measure_low_level_pulse_width_ctrl, &g_measure_low_level_pulse_width_cfg);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**Open instance TAU measure low level pulse width failed**");

    /* Enable instance TAU to measure low level pulse width of square wave */
    err = R_TAU_Enable(&g_measure_low_level_pulse_width_ctrl);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**Enable instance TAU measure low level pulse width failed**");

    /* Set period of square wave */
    err = square_wave_period_set();
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**Set period of square wave output failed**");

    /* Start instance TAU square wave output sequence 3 after set period */
    err = R_TAU_Start(&g_square_wave_output_sequence3_ctrl);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**Start instance TAU square wave output sequence 3 failed**");

    while (DEMO_TIMES > g_measure_times)
    {
        /* Check measure flag and ignore the first measurement since it could have started in the middle of a pulse */
        if (g_measure_pulse_width_flag && g_interrupt_counter > ONE)
        {
            /* Get clock frequency to calculate pulse width */
            err = R_TAU_InfoGet(&g_measure_low_level_pulse_width_ctrl, &periodic_info);
            APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**Get periodic info of measure low level pulse width failed**");
            /* Calculate low level pulse width */
            low_level_pulse_width = (float) (( (float) (g_capture_value) / (float) (periodic_info.clock_frequency))\
                                    * CONVERT_SECOND_TO_MILISECOND);
            sprintf(timer_buffer, "%.03f", low_level_pulse_width);
            APP_PRINT("\r\nLow level pulse width measurement value: %sms\r\n", timer_buffer);

            /* Reset g_measure_pulse_width_flag */
            g_measure_pulse_width_flag = false;
            /* Reset g_capture_value */
            g_capture_value = RESET_VALUE;
            g_measure_times++;
        }
    }
    /* Close instance TAU square wave output sequence 3 */
    err = R_TAU_Close(&g_square_wave_output_sequence3_ctrl);
    APP_ERR_RET(FSP_SUCCESS != err, err, "Close instance TAU square wave output sequence 3 failed");
    /* Close instance TAU measure low level pulse width */
    err = R_TAU_Close(&g_measure_low_level_pulse_width_ctrl);
    APP_ERR_RET(FSP_SUCCESS != err, err, "Close instance TAU measure low level pulse width failed");
    /* Reset low level measurement times */
    g_measure_times = RESET_VALUE;
    /* Reset interrupt counter */
    g_interrupt_counter = RESET_VALUE;
    return err;
}

/*******************************************************************************************************************//**
 *  @brief       This function is used to set the time-on for LED1.
 *  @param[in]   None
 *  @retval      FSP_SUCCESS    Upon successful operation.
 *  @retval      Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
static fsp_err_t led1_on_time_set(void)
{
    fsp_err_t err                        = FSP_SUCCESS;
    uint32_t time_period_ms_led_on       = RESET_VALUE;
    uint32_t raw_counts                  = RESET_VALUE;
    uint32_t time_period_max             = RESET_VALUE;

    APP_PRINT("\r\nPlease enter time period values (ms) for LED1 ON\r\n");
    /* Calculate maximum configurable value of interval timer */
    time_period_max = max_timer_calculate(DELAY_COUNTER);
    APP_PRINT("\r\nValid range: 1ms to %dms\r\n", time_period_max);

    /* Wait for user input */
    time_period_ms_led_on = get_user_input(TIME_PERIOD_MIN, time_period_max);
    APP_PRINT("\r\nTime period for Led ON: %dms\r\n", time_period_ms_led_on);

    /* Depending on the user selected clock source, raw counts value can be calculated
     * for the user given time-period values */
     err = R_TAU_InfoGet(&g_delay_timer_ctrl, &periodic_info);
     APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**Get periodic info of delay timer failed**");
     raw_counts = (uint32_t)((time_period_ms_led_on * periodic_info.clock_frequency ) / CONVERT_SECOND_TO_MILISECOND);
     /* Set period value */
     err = R_TAU_PeriodSet(&g_delay_timer_ctrl, raw_counts);
     APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**Set delay timer failed**");
     /* Reset LED1 turn-on time value */
     time_period_ms_led_on = RESET_VALUE;

     return err;
}

/*******************************************************************************************************************//**
 *  @brief       This function is used to set the time-off for LED1.
 *  @param[in]   None
 *  @retval      FSP_SUCCESS    Upon successful operation.
 *  @retval      Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
static fsp_err_t led1_off_time_set(void)
{
    fsp_err_t err                    = FSP_SUCCESS;
    uint32_t  time_period_ms_led_off = RESET_VALUE;
    uint32_t  raw_counts             = RESET_VALUE;
    uint32_t  time_period_max        = RESET_VALUE;

    APP_PRINT("\r\nPlease enter time period values (ms) for LED1 OFF\r\n");
    /* Calculate maximum configurable value of delay counter */
    time_period_max = max_timer_calculate(INTERVAL_TIMER) ;
    APP_PRINT("\r\nValid range: 1ms to %dms\r\n", time_period_max);

    /* Wait for user input */
    time_period_ms_led_off = get_user_input(TIME_PERIOD_MIN, time_period_max);
    APP_PRINT("\r\nTime period for Led OFF: %dms\r\n", time_period_ms_led_off);
    /* Calculation of raw counts value for given milliseconds value */
    err = R_TAU_InfoGet(&g_interval_timer_ctrl, &periodic_info);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**Get periodic info of interval timer failed**");

    /* Depending on the user selected clock source, raw counts value can be calculated
     * for the user given time-period values */
    raw_counts = (uint32_t) ((time_period_ms_led_off * periodic_info.clock_frequency) / CONVERT_SECOND_TO_MILISECOND);
    /* Set period value */
    err = R_TAU_PeriodSet(&g_interval_timer_ctrl, raw_counts);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**Set interval timer failed**");
    /* Reset LED1 off time value */
    time_period_ms_led_off = RESET_VALUE;

    return err;
}

/*******************************************************************************************************************//**
 *  @brief       This function is used to set the frequency of square wave of sequence 2.
 *  @param[in]   None
 *  @retval      square_wave_frequency    Frequency is set by the user.
 **********************************************************************************************************************/
static uint32_t square_wave_frequency_set(void)
{
    fsp_err_t err                       = FSP_SUCCESS;
    uint32_t  square_wave_frequency     = RESET_VALUE;
    uint32_t  raw_counts                = RESET_VALUE;
    uint32_t  square_wave_frequency_max = RESET_VALUE;

    APP_PRINT("\r\nPlease enter square wave frequency (Hz)\r\n");
    /* Calculate maximum configurable value of square wave sequence 2 */
    square_wave_frequency_max = max_timer_calculate(SQUARE_WAVE_SEQUENCE2);

    /* Print requirement of frequency input */
    APP_PRINT(FREQUENCY_INPUT, square_wave_frequency_max);
    APP_PRINT("\r\nValid range: 1Hz to %dHz\r\n", square_wave_frequency_max);

    /* Wait for user input */
    square_wave_frequency = get_user_input(SQUARE_WAVE_FREQUENCY_MIN, square_wave_frequency_max);
    APP_PRINT("\r\nSquare wave frequency: %dHz\r\n", square_wave_frequency);

    err = R_TAU_InfoGet(&g_square_wave_output_sequence2_ctrl, &periodic_info);
    APP_ERR_HANDLE(err, "\r\n**Get periodic info of square wave output failed**");

    /* Depending on the user selected clock source, raw counts value can be calculated
     * for the user given square wave frequency values */
    raw_counts = (uint32_t) (periodic_info.clock_frequency / (square_wave_frequency * CONVERT_HALF_TO_ONE_CYCLE));
    /* Set period value */
    err = R_TAU_PeriodSet(&g_square_wave_output_sequence2_ctrl, raw_counts);
    APP_ERR_HANDLE(err, "\r\n**Set frequency for square wave output failed**");

    return square_wave_frequency;
}

/*******************************************************************************************************************//**
 *  @brief       This function is used to set the divider.
 *  @param[in]   divider_limit     Maximum of divider corresponding with selected frequency previous.
 *  @retval      divided_output    The frequency of divided square wave.
 **********************************************************************************************************************/
static float divider_set(uint32_t square_wave_frequency)
{
    fsp_err_t err                     = FSP_SUCCESS;
    char      timer_buffer[BUFF_SIZE] = {RESET_VALUE};
    uint32_t  divider                 = RESET_VALUE;
    float     divided_output          = RESET_VALUE;

    APP_PRINT("\r\nPlease enter clock division\r\n");
    APP_PRINT("Min: 1 and Max: %d\r\n", square_wave_frequency);

    /* Wait for user input */
    divider = get_user_input(DIVIDER_MIN, square_wave_frequency);
    APP_PRINT("\r\nSelected Divider: %d\r\n", divider);
    /* Print the expected divided frequency of the square wave */
    divided_output = (float) ((float) square_wave_frequency / (float) divider);
    sprintf(timer_buffer, "%.03f", divided_output);
    APP_PRINT("\r\nExpected divided frequency of the square wave: %sHz\r\n", timer_buffer);

    /* Set divider value */
    err = R_TAU_PeriodSet(&g_divider_timer_ctrl, divider);
    APP_ERR_HANDLE(err, "\r\n**Set divider failed**");
    /* Reset divider */
    divider = RESET_VALUE;
    return divided_output;
}

/*******************************************************************************************************************//**
 *  @brief       This function is used to set the period of square wave of sequence 3.
 *  @param[in]   None
 *  @retval      FSP_SUCCESS    Upon successful operation.
 *  @retval      Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
static fsp_err_t square_wave_period_set(void)
{
    fsp_err_t err                    = FSP_SUCCESS;
    uint32_t  square_wave_period     = RESET_VALUE;
    uint32_t  raw_counts             = RESET_VALUE;
    uint32_t  square_wave_period_max = RESET_VALUE;

    APP_PRINT("\r\nPlease enter square wave period (ms)\r\n");
    /* Calculate maximum configurable value of square wave sequence 3 */
    square_wave_period_max = max_timer_calculate(SQUARE_WAVE_SEQUENCE3);
    APP_PRINT("\r\nValid range: 1ms to %dms\r\n", square_wave_period_max);

    /* Wait for user input */
    square_wave_period = get_user_input(SQUARE_WAVE_PERIOD_MIN, square_wave_period_max);
    APP_PRINT("\r\nSquare wave period: %dms\r\n", square_wave_period);

    /* Calculation of raw counts value for given milliseconds value */
    err = R_TAU_InfoGet(&g_square_wave_output_sequence3_ctrl, &periodic_info);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**Get periodic info of square wave output sequence 3 failed**");

    /* Depending on the user selected clock source, raw counts value can be calculated
     * for the user given square wave period values */
    raw_counts = (uint32_t) ((periodic_info.clock_frequency * square_wave_period) / (CONVERT_HALF_TO_ONE_CYCLE \
                 * CONVERT_SECOND_TO_MILISECOND));
    /* Set period value */
    err = R_TAU_PeriodSet(&g_square_wave_output_sequence3_ctrl, raw_counts);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**Set frequency for square wave output sequence 3 failed**");
    /* Reset period of square wave */
    square_wave_period = RESET_VALUE;

    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function is used to get the user inputs.
 * @param[in]   min_value   Minimum value.
 *              max_value   Maximum value.
 * @retval      input_value    Returns input buffer after receive enter key.
 **********************************************************************************************************************/
uint32_t get_user_input(uint32_t min_value, uint32_t max_value)
{
    uint32_t input_value = RESET_VALUE;

    while (true)
    {
        /* Clean buffer */
        memset(&user_input[0], NULL_CHAR, sizeof(user_input));

        /* Wait until there is any user input */
        while (!APP_CHECK_DATA)
        {
            __NOP();
        }

        /* Read user input from the terminal */
        APP_READ(&user_input[0], TERM_BUFFER_SIZE);

        /* Convert to integer value */
        input_value = (uint32_t)atoi((char*) &user_input[0]);

        if (input_value >= min_value && input_value <= max_value)
        {
            break;
        }
        else
        {
            APP_PRINT("\r\nInvalid input. Please enter a valid input between %u and %u.\r\n", min_value, max_value);
            /* Reset user input value if it is an invalid input */
            input_value = RESET_VALUE;
        }
    }
    return input_value;
}

/*******************************************************************************************************************//**
 *  @brief       This function is used to determine the maximum value of timer.
 *  @param[in]   tau_mode   Select TAU operation function.
 *  @retval      max_val    Maximum value of timer mode.
 **********************************************************************************************************************/
static uint32_t max_timer_calculate(tau_mode_t tau_mode)
{
    fsp_err_t err     = FSP_SUCCESS;
    uint32_t  max_val = RESET_VALUE;

    err = R_TAU_InfoGet(g_tau_instance[tau_mode]->p_ctrl, &periodic_info);
    APP_ERR_HANDLE(err, "\r\n**R_TAU_InfoGet failed**");

    if (INTERVAL_TIMER == tau_mode || DELAY_COUNTER == tau_mode)
    {
        /* Determine the maximum values of interval timer and delay counter corresponding to the
         * selected TAU clock source */
        max_val = ((MAX_PERIOD * CONVERT_SECOND_TO_MILISECOND) / periodic_info.clock_frequency);
    }
    else if (SQUARE_WAVE_SEQUENCE2 == tau_mode || INPUT_CAPTURE == tau_mode)
    {
        /* Determine the maximum frequency of square wave output sequence 2 and the maximum measurable
         * input pulse signal corresponding to the selected TAU clock source */
        max_val = ((periodic_info.clock_frequency / MIN_PERIOD) / CONVERT_HALF_TO_ONE_CYCLE);
    }
    else if (SQUARE_WAVE_SEQUENCE3 == tau_mode)
    {
        /* Determine the maximum period of square wave output sequence 3 corresponding to the selected
         * TAU clock source */
        max_val = (((MAX_PERIOD * CONVERT_HALF_TO_ONE_CYCLE) * CONVERT_SECOND_TO_MILISECOND) /\
                  periodic_info.clock_frequency);
    }
    else
    {
        /* Do nothing */
    }
    return max_val;
}

/*******************************************************************************************************************//**
 *  @brief       This function is used to determine the minimum value of timer.
 *  @param[in]   tau_mode       Select TAU operation function.
 *  @retval      min_val        Minimum measurable value of timer mode.
 **********************************************************************************************************************/
static float min_timer_calculate(tau_mode_t tau_mode)
{
    fsp_err_t err = FSP_SUCCESS;
    float min_val = RESET_VALUE;

    err = R_TAU_InfoGet(g_tau_instance[tau_mode]->p_ctrl, &periodic_info);
    APP_ERR_HANDLE(err, "\r\n**R_TAU_InfoGet failed**");

    if (INPUT_CAPTURE == tau_mode)
    {
        /* Determine the minimum frequency can be measured by input capture operation corresponding to
         * the selected TAU clock source */
        min_val = (float) ((float) periodic_info.clock_frequency / (float) CAPTURE_VALUE_MAX);
    }

    return min_val;
}

/*******************************************************************************************************************//**
 * @brief       Interval timer callback function.
 * @param[in]   p_args  TAU Callback arguments.
 * @return      None
 **********************************************************************************************************************/
void tau_interval_timer_callback(timer_callback_args_t *p_args)
{
    if (TIMER_EVENT_CYCLE_END == p_args->event)
    {
        g_led1_status_flag_cur = true;
    }
}

/*******************************************************************************************************************//**
 *  @brief      Delay timer callback function.
 *  @param[in]  p_args   Callback arguments.
 *  @retval     None
 **********************************************************************************************************************/
void tau_delay_timer_callback(timer_callback_args_t *p_args)
{
    if (TIMER_EVENT_CYCLE_END == p_args->event)
    {
        g_delay_timer_end_flag = true;
    }
}

/*******************************************************************************************************************//**
 *  @brief      Capture input pulse callback function.
 *  @param[in]  p_args   Callback arguments.
 *  @retval     None
 **********************************************************************************************************************/
void tau_input_capture_timer_callback(timer_callback_args_t *p_args)
{
    if (TIMER_EVENT_CAPTURE_EDGE == p_args->event)
    {
        g_input_capture_detect_flag = true;
        g_capture_value = p_args->capture;
        g_interrupt_counter++;
    }
}

/*******************************************************************************************************************//**
 *  @brief      External event counter callback function.
 *  @param[in]  p_args   Callback arguments.
 *  @retval     None
 **********************************************************************************************************************/
void tau_external_event_counter_callback(timer_callback_args_t *p_args)
{
    if (TIMER_EVENT_CYCLE_END == p_args->event)
    {
        g_external_counter_flag = true;
    }
}

/*******************************************************************************************************************//**
 *  @brief      Measure low level callback function.
 *  @param[in]  p_args   Callback arguments.
 *  @retval     None
 **********************************************************************************************************************/
void tau_measure_low_level_pulse_width_timer_callback(timer_callback_args_t *p_args)
{
    if (TIMER_EVENT_CAPTURE_EDGE == p_args->event)
    {
        g_measure_pulse_width_flag = true;
        g_capture_value = p_args->capture;
        g_interrupt_counter++;
    }
}

/*******************************************************************************************************************//**
 * @brief       This function handles errors, prints error function and prints errors.
 * @param[IN]   err         Error code.
 *              err_str     Error string.
 * @retval      None
 **********************************************************************************************************************/
void handle_error(fsp_err_t err, uint8_t * err_str)
{
    /* Print error information and error code */
    APP_ERR_PRINT(err_str);
    /* Close the opened TAU modules */
    tau_deinit();
    /* Error trap */
    APP_ERR_TRAP(err);
}

/*******************************************************************************************************************//**
 * @brief       This function closes opened TAU modules.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void tau_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close opened instance TAU interval timer */
    if (MODULE_CLOSE != g_interval_timer_ctrl.open)
    {
        err = R_TAU_Close(&g_interval_timer_ctrl);
        if(FSP_SUCCESS != err)
        {
            APP_ERR_PRINT(err, "R_TAU_Close API failed\r\n");
        }
    }
    /* Close opened instance TAU delay timer */
    if (MODULE_CLOSE != g_delay_timer_ctrl.open)
    {
        err = R_TAU_Close(&g_delay_timer_ctrl);
        if(FSP_SUCCESS != err)
        {
            APP_ERR_PRINT(err, "R_TAU_Close API failed\r\n");
        }
    }
    /* Close opened instance TAU square wave output sequence 2 */
    if (MODULE_CLOSE != g_square_wave_output_sequence2_ctrl.open)
    {
        err = R_TAU_Close(&g_square_wave_output_sequence2_ctrl);
        if(FSP_SUCCESS != err)
        {
            APP_ERR_PRINT(err, "R_TAU_Close API failed\r\n");
        }
    }
    /* Close opened instance TAU divider */
    if (MODULE_CLOSE != g_divider_timer_ctrl.open)
    {
        err = R_TAU_Close(&g_divider_timer_ctrl);
        if(FSP_SUCCESS != err)
        {
            APP_ERR_PRINT(err, "R_TAU_Close API failed\r\n");
        }
    }
    /* Close opened instance TAU input capture */
    if (MODULE_CLOSE != g_input_capture_ctrl.open)
    {
        err = R_TAU_Close(&g_input_capture_ctrl);
        if(FSP_SUCCESS != err)
        {
            APP_ERR_PRINT(err, "R_TAU_Close API failed\r\n");
        }
    }
    /* Close opened instance TAU external event counter */
    if (MODULE_CLOSE != g_external_event_counter_timer_ctrl.open)
    {
        err = R_TAU_Close(&g_external_event_counter_timer_ctrl);
        if(FSP_SUCCESS != err)
        {
            APP_ERR_PRINT(err, "R_TAU_Close API failed\r\n");
        }
    }
    /* Close opened instance TAU square wave output sequence 3 */
    if (MODULE_CLOSE != g_square_wave_output_sequence3_ctrl.open)
    {
        err = R_TAU_Close(&g_square_wave_output_sequence3_ctrl);
        if(FSP_SUCCESS != err)
        {
            APP_ERR_PRINT(err, "R_TAU_Close API failed\r\n");
        }
    }
    /* Close opened instance TAU measure low level pulse width */
    if (MODULE_CLOSE != g_measure_low_level_pulse_width_ctrl.open)
    {
        err = R_TAU_Close(&g_measure_low_level_pulse_width_ctrl);
        if(FSP_SUCCESS != err)
        {
            APP_ERR_PRINT(err, "R_TAU_Close API failed\r\n");
        }
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup tau)
 **********************************************************************************************************************/
