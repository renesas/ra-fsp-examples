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

#if (BSP_PERIPHERAL_RTC_C_PRESENT)
/* Number of days in each months start from January to December */
static const uint8_t days_in_months[12] = {31U, 28U, 31U, 30U, 31U, 30U, 31U, 31U, 30U, 31U, 30U, 31U};
#endif /* BSP_PERIPHERAL_RTC_C_PRESENT */

/* Private functions */
static void rtc_date_time_conversion(rtc_time_t * time, char read_buffer[]);

#if (BSP_PERIPHERAL_RTC_C_PRESENT)
static fsp_err_t set_day_of_week(void);
#endif /* BSP_PERIPHERAL_RTC_C_PRESENT */

/*******************************************************************************************************************//**
 * @brief     This function initializes RTC module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t rtc_init(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Open RTC module */
    err = R_RTC_OPEN(&g_rtc_ctrl, &g_rtc_cfg);

    return err;
}

#if (BSP_PERIPHERAL_RTC_C_PRESENT)
/*******************************************************************************************************************//**
 * @brief     This function calculates day of week.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Calculated day of week successfully.
 * @retval    Any other error codes     Failed to calculate day of week.
 **********************************************************************************************************************/
static fsp_err_t set_day_of_week(void)
{
    uint32_t day_of_week;
    uint32_t num_days_month;
    uint32_t day_of_a_month;
    uint32_t temp_month;
    uint32_t temp_year;

    day_of_a_month = (uint32_t) g_set_time.tm_mday;
    temp_month = (uint32_t) (g_set_time.tm_mon + 1);

    /* The valid value of year is between 100 to 199, The RTC has a 100 year calendar from 2000 to 2099
     * to match the starting year 2000, a sample year offset(1900) is added like 117 + 1900 = 2017 */
    temp_year = (uint32_t) (g_set_time.tm_year + YEAR_ADJUST_VALUE);

    /* Checking the error condition for year and months values, here valid value of year is between 100 to 199
     * and for month 0 to 11 */
    if ((g_set_time.tm_year < YEAR_VALUE_MIN) || (g_set_time.tm_year > YEAR_VALUE_MAX) ||
        (g_set_time.tm_mon < 0) || (g_set_time.tm_mon > MONTHS_IN_A_YEAR))
    {
        return FSP_ERR_INVALID_ARGUMENT;
    }

    /* For particular valid month, number of days in a month is updated */
    num_days_month = days_in_months[g_set_time.tm_mon];

    /* Checking for February month and Conditions for Leap year : Every fourth year is a leap year,
     * The RTC has a 100 year calendar from 2000 to 2099 */
    if ((FEBRUARY_MONTH == temp_month) && (0 == (temp_year % 4U)))
    {
        num_days_month = LAST_DAY_OF_LEAP_FEB_MONTH;
    }

    /* Checking for day of a month values for valid range */
    if ((day_of_a_month >= FIRST_DAY_OF_A_MONTH) && (day_of_a_month <= num_days_month))
    {
        /* Adjust month to run from 3 to 14 for March to February */
        if (MARCH_MONTH > temp_month)
        {
            temp_month = (temp_month + 12U);

            /* Adjust year if January or February */
            --temp_year;
        }

        /* For the Gregorian calendar, Zeller's congruence formulas is
         * h = ( q + [13(m+1)/5] + Y + [Y/4] - [Y/100] + [Y/400])mod 7 (mod : modulo)
         * h is the day of the week , q is the day of the month,
         * m is the month (3 = March, 4 = April,..., 14 = February)
         * Y is year, which is Y - 1 during January and February */
        day_of_week  = (uint32_t) g_set_time.tm_mday + ((13 * (temp_month + 1)) / 5) + temp_year + (temp_year / 4);
        day_of_week  = (day_of_week - ZELLER_ALGM_CONST_FIFTEEN) % 7;

        /* Day of week between 0 to 6 :- Sunday to Saturday */
        /* d = (h + 6)mod 7 (mod : modulo) */
        g_set_time.tm_wday = (int16_t) ((day_of_week + 6U) % 7U);
    }
    else
    {
        return FSP_ERR_INVALID_ARGUMENT;
    }

    return FSP_SUCCESS;
}
#endif /* BSP_PERIPHERAL_RTC_C_PRESENT */

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

#if (BSP_PERIPHERAL_RTC_C_PRESENT)
    /* For RTC_C, the user is expected to set the weekday correctly in the application */
    err = set_day_of_week();
    if (FSP_SUCCESS != err)
    {
        return err;
    }
#endif /* BSP_PERIPHERAL_RTC_C_PRESENT */

    /* Set the time provided by the user */
    err = R_RTC_CALENDAR_TIME_SET(&g_rtc_ctrl, &g_set_time);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Get the current calendar time */
    err = R_RTC_CALENDAR_TIME_GET(&g_rtc_ctrl, time);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Modify the date in standard format to user readable format */
    rtc_date_readability_update(time);

    /* Set the set_time_flag */
    set_time_flag = true;

    return err;
}

/*******************************************************************************************************************//**
 * @brief      This function sets the alarm time provided by the user.
 * @param[in]  read_alarm               Buffer containing alarm time string (HH:MM format for RTC_C, SS format for RTC).
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
#if (BSP_PERIPHERAL_RTC_C_PRESENT)
     .time_minute     = RESET_VALUE,
     .time_hour       = RESET_VALUE,
     .weekday_match   = RESET_VALUE,
#else
     .sec_match       = RESET_VALUE,
     .min_match       = RESET_VALUE,
     .hour_match      = RESET_VALUE,
     .mday_match      = RESET_VALUE,
     .mon_match       = RESET_VALUE,
     .year_match      = RESET_VALUE,
     .dayofweek_match = RESET_VALUE,
#endif /* BSP_PERIPHERAL_RTC_C_PRESENT */
};

    /* Set alarm only if proper RTC calendar date and time is set */
    if (true == set_time_flag)
    {

#if (BSP_PERIPHERAL_RTC_C_PRESENT)
        /* Reset the timer variables */
        alarm_time_set.weekday_match = RESET_VALUE;
        alarm_time_set.time_hour     = RESET_VALUE;
        alarm_time_set.time_minute   = RESET_VALUE;

        /* Modify user provided time value to standard format */
        alarm_time_set.time_hour   = ((ASCII_TO_DIGIT(read_alarm[0]) * PLACE_VALUE_TEN ) +
                                      ASCII_TO_DIGIT(read_alarm[1]));
        alarm_time_set.time_minute = ((ASCII_TO_DIGIT(read_alarm[3]) * PLACE_VALUE_TEN ) +
                                      ASCII_TO_DIGIT(read_alarm[4]));

        /* Enable ALARMWW */
        alarm_time_set.weekday_match = ALARMWW;

        /* Set the alarm time provided by the user */
        err = R_RTC_CALENDAR_ALARM_SET(&g_rtc_ctrl, &alarm_time_set);
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        /* Get the alarm time */
        err = R_RTC_CALENDAR_ALARM_GET(&g_rtc_ctrl, alarm_time);
        if (FSP_SUCCESS != err)
        {
            return err;
        }
    }
#else
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
        err = R_RTC_CALENDAR_ALARM_SET(&g_rtc_ctrl, &alarm_time_set);
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        /* Get the alarm time */
        err = R_RTC_CALENDAR_ALARM_GET(&g_rtc_ctrl, alarm_time);
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        return err;
    }
#endif /* BSP_PERIPHERAL_RTC_C_PRESENT */
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
    err = R_RTC_PERIODIC_IRQ_RATE_SET(&g_rtc_ctrl, RTC_PERIODIC_IRQ_SELECT_1_SECOND);

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
    err = R_RTC_CALENDAR_TIME_GET(&g_rtc_ctrl, time);
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
 * @brief     This function modifies the user input to RTC date and time.
 * @param[in] time                      Date and time to be modified.
 * @param[in] read_buffer               Buffer that contains user input time values.
 * @retval    None.
 **********************************************************************************************************************/
static void rtc_date_time_conversion(rtc_time_t *time, char read_buffer[])
{
    /* The user is expected to enter Date and Time in DD:MM:YYYY HH:MM:SS format. This user input in the buffer is
     * processed using the following conversion, where ASCII value is converted to integer value
     * and stored in the RTC time structure.
     * According to DD:MM:YYYY HH:MM:SS format, different array index values are used. */
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

/*******************************************************************************************************************//**
 * @brief     This function toggles the level of LED pins.
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
 * @brief     This function sets the level of LED pins.
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
 * @brief     This function is a callback for RTC module.
 * @param[in] p_args                    Pointer to callback arguments.
 * @retval    None.
 **********************************************************************************************************************/
void rtc_callback(rtc_callback_args_t *p_args)
{
    if (RTC_EVENT_ALARM_IRQ == p_args->event)
    {
        g_alarm_irq_flag = true;
    }
    else if (RTC_EVENT_PERIODIC_IRQ == p_args->event)
    {
        g_periodic_irq_flag = true;
    }
    else
    {
        /* Do nothing */
    }
}

/*******************************************************************************************************************//**
 * @brief     This function closes the opened RTC module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void rtc_deinit(void)
{
    /* Close the opened RTC module */
    if (g_rtc_ctrl.open)
    {
        R_RTC_CLOSE(&g_rtc_ctrl);
    }
}
