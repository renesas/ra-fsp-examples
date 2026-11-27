/***********************************************************************************************************************
 * File Name    : rtc_ep.c
 * Description  : Contains data structures and functions used in rtc_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "rtc_ep.h"

/* Flag to check occurrence of alarm interrupt */
volatile bool g_alarm_irq_flag = false;

/* Flag to check occurrence of periodic interrupt */
volatile bool g_periodic_irq_flag = false;

/* Flag to check if the user provides options in proper sequence */
volatile bool g_invalid_sequence = false;

/* Flag to be set on successful RTC calendar setting */
static volatile bool set_time_flag = false;

/* Variable to store time */
static rtc_time_t g_set_time =
{
    .tm_hour  = RESET_VALUE,
    .tm_isdst = RESET_VALUE,
    .tm_mday  = RESET_VALUE,
    .tm_min   = RESET_VALUE,
    .tm_mon   = RESET_VALUE,
    .tm_sec   = RESET_VALUE,
    .tm_wday  = RESET_VALUE,
    .tm_yday  = RESET_VALUE,
    .tm_year  = RESET_VALUE,
};

/* Private function */
static void rtc_date_time_conversion(rtc_time_t * time, char read_buffer[]);

/*******************************************************************************************************************//**
 * @brief     This function initializes the RTC module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t rtc_init(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Open RTC module */
    err = g_rtc_open_guard(NULL, NULL);

    return err;
}

/*******************************************************************************************************************//**
 * @brief      This function sets the calendar time provided by the user.
 * @param[in]  read_time                Buffer containing date and time string in DD:MM:YYYY HH:MM:SS format.
 * @param[out] time                     Pointer to rtc_time_t structure to store the calendar time read back from RTC.
 * @retval     FSP_SUCCESS              Set calendar time successfully.
 * @retval     Any other error codes    Failed to set calendar time.
 **********************************************************************************************************************/
fsp_err_t set_rtc_calendar_time(char read_time[], rtc_time_t * time)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Modify the user input date and time to standard format */
    rtc_date_time_conversion(&g_set_time, &read_time[0]);

    /* Set the time */
    err = g_rtc_calendar_time_set_guard(NULL, &g_set_time);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Get the current time */
    err = g_rtc_calendar_time_get_guard(NULL, time);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Modify the date in standard format to the user readable format */
    rtc_date_readability_update(time);

    /* Set the set_time_flag */
    set_time_flag = true;

    return err;
}

/*******************************************************************************************************************//**
 * @brief      This function sets the alarm time provided by the user.
 * @param[in]  read_alarm               Buffer containing alarm time string (SS format for RTC).
 * @param[out] alarm_time               Pointer to rtc_alarm_time_t structure to store the alarm time read back
 *                                      from RTC.
 * @retval     FSP_SUCCESS              Set calendar alarm successfully.
 * @retval     Any other error codes    Failed to set calendar alarm.
 **********************************************************************************************************************/
fsp_err_t set_rtc_calendar_alarm(char read_alarm[], rtc_alarm_time_t * alarm_time)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    rtc_alarm_time_t alarm_time_set =
    {
        .sec_match       = RESET_VALUE,
        .min_match       = RESET_VALUE,
        .hour_match      = RESET_VALUE,
        .mday_match      = RESET_VALUE,
        .mon_match       = RESET_VALUE,
        .year_match      = RESET_VALUE,
        .dayofweek_match = RESET_VALUE,
    };

    /* Set alarm only if proper RTC calendar date and time is set */
    if (true == set_time_flag)
    {
        /* Set the flags for which the alarm has to be generated */
        alarm_time_set.hour_match = false;
        alarm_time_set.min_match  = false;
        alarm_time_set.sec_match  = true;
        alarm_time_set.mday_match = false;
        alarm_time_set.mon_match  = false;
        alarm_time_set.year_match = false;

        /* Modify the user input time value to standard format */
        alarm_time_set.time.tm_sec = ((ASCII_TO_DIGIT(read_alarm[0]) * PLACE_VALUE_TEN) +
                                      ASCII_TO_DIGIT(read_alarm[1]));

        /* Set the alarm time provided by the user */
        err = g_rtc_calendar_alarm_set_guard(NULL, &alarm_time_set);
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        /* Get the alarm time */
        err = g_rtc_calendar_alarm_get_guard(NULL, alarm_time);
        if (FSP_SUCCESS != err)
        {
            return err;
        }
    }
    else
    {
        /* Set this flag to notify the user to provide required options */
        g_invalid_sequence = true;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function sets the periodic interrupt rate provided by the user.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Set periodic interrupt rate successfully.
 * @retval    Any other error codes     Failed to set periodic interrupt rate.
 **********************************************************************************************************************/
fsp_err_t set_rtc_periodic_rate(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Enable periodic interrupt rate */
    err = g_rtc_periodic_irq_rate_set_guard(NULL, RTC_PERIODIC_IRQ_SELECT_1_SECOND);

    return err;
}

/*******************************************************************************************************************//**
 * @brief      This function gets the current RTC time.
 * @param[out] time                     Pointer to rtc_time_t structure to store the current calendar time.
 * @retval     FSP_SUCCESS              Got the current RTC time successfully.
 * @retval     Any other error codes    Failed to get the current RTC time.
 **********************************************************************************************************************/
fsp_err_t get_rtc_calendar_time(rtc_time_t * time)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Get the current RTC calendar time */
    err = g_rtc_calendar_time_get_guard(NULL, time);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Modify the date in standard format to the user readable format */
    rtc_date_readability_update(time);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function modifies the date in readable format to the user.
 * @param[in] time                      Date to be modified.
 * @retval    None.
 **********************************************************************************************************************/
void rtc_date_readability_update(rtc_time_t * time)
{
    time->tm_mon  += MON_ADJUST_VALUE;
    time->tm_year += YEAR_ADJUST_VALUE;
}

/*******************************************************************************************************************//**
 * @brief     This function closes the opened RTC module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void rtc_deinit(void)
{
    /* Close the opened RTC module */
    g_rtc_close_guard(NULL);
}

/*******************************************************************************************************************//**
 * @brief     This function is a callback for RTC module.
 * @param[in] p_args                    Pointer to the callback argument.
 * @retval    None.
 **********************************************************************************************************************/
void rtc_callback(rtc_callback_args_t *p_args)
{
    if (RTC_EVENT_ALARM_IRQ == p_args->event)
    {
        /* Set g_alarm_irq_flag */
        g_alarm_irq_flag = true;
    }
    else if (RTC_EVENT_PERIODIC_IRQ == p_args->event)
    {
        /* Set g_periodic_irq_flag */
        g_periodic_irq_flag = true;
    }
    else
    {
        /* Do nothing */
    }
}

/*******************************************************************************************************************//**
 * @brief     This function toggles the level of LED pin.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Toggled LED level successfully.
 * @retval    Any other error codes     Failed to toggle LED level.
 **********************************************************************************************************************/
fsp_err_t led_level_toggle(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    bsp_io_level_t led_level = LED_OFF;

    /* Read current LED status */
    err = R_IOPORT_PinRead(&g_ioport_ctrl, (bsp_io_port_pin_t)(g_bsp_leds.p_leds[0]), &led_level);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    led_level ^= BSP_IO_LEVEL_HIGH;

    /* Set LED status */
    err = led_level_set(led_level);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function sets the level of LED pin.
 * @param[in] led_level                 Level to be set.
 * @retval    FSP_SUCCESS               Set LED level successfully.
 * @retval    Any other error codes     Failed to set LED level.
 **********************************************************************************************************************/
fsp_err_t led_level_set(bsp_io_level_t led_level)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Set LED status */
    err = R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[RESET_VALUE], led_level);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function modifies the user input to RTC date and time.
 * @param[in] time                      Date and time to be modified.
 * @param[in] read_buffer               Buffer that contains the user input time values.
 * @retval    None.
 **********************************************************************************************************************/
static void rtc_date_time_conversion(rtc_time_t * time, char read_buffer[])
{
    /* The user is expected to enter date and time in DD:MM:YYYY HH:MM:SS format. The user input in the buffer is
     * processed using the following conversion, where ASCII value is converted to integer value and stored in the
     * RTC time structure. According to DD:MM:YYYY HH:MM:SS format, different array index values are used. */
    time->tm_mday = ((ASCII_TO_DIGIT(read_buffer[0]) * PLACE_VALUE_TEN) + ASCII_TO_DIGIT(read_buffer[1]));
    time->tm_mon  = (((ASCII_TO_DIGIT(read_buffer[3]) * PLACE_VALUE_TEN) + ASCII_TO_DIGIT(read_buffer[4]))) -
                    MON_ADJUST_VALUE;
    time->tm_year = ((ASCII_TO_DIGIT(read_buffer[6]) * PLACE_VALUE_THOUSAND) +
                     (ASCII_TO_DIGIT(read_buffer[7]) * PLACE_VALUE_HUNDRED) +
                     (ASCII_TO_DIGIT(read_buffer[8]) * PLACE_VALUE_TEN) + ASCII_TO_DIGIT(read_buffer[9])) -
                    YEAR_ADJUST_VALUE;
    time->tm_hour = ((ASCII_TO_DIGIT(read_buffer[11]) * PLACE_VALUE_TEN) + ASCII_TO_DIGIT(read_buffer[12]));
    time->tm_min  = ((ASCII_TO_DIGIT(read_buffer[14]) * PLACE_VALUE_TEN) + ASCII_TO_DIGIT(read_buffer[15]));
    time->tm_sec  = ((ASCII_TO_DIGIT(read_buffer[17]) * PLACE_VALUE_TEN) + ASCII_TO_DIGIT(read_buffer[18]));
}
