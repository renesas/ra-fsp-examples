/***********************************************************************************************************************
 * File Name    : rtc_ep.c
 * Description  : Contains data structures and functions used in rtc_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "rtc_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup rtc_ep
 * @{
 **********************************************************************************************************************/

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
static rtc_time_t g_present_time =
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
static uint32_t set_time_flag = RESET_FLAG;  /* Flag to be set on successful RTC calendar setting */
#if (BSP_PERIPHERAL_RTC_C_PRESENT)
/* Number of days in each months start from January to December */
static const uint8_t days_in_months[12] = {31U, 28U, 31U, 30U, 31U, 30U, 31U, 31U, 30U, 31U, 30U, 31U};
#endif /* BSP_PERIPHERAL_RTC_C_PRESENT */
static void rtc_date_time_conversion(rtc_time_t * time, unsigned char read_buffer[]);
#if (BSP_PERIPHERAL_RTC_C_PRESENT)
static fsp_err_t set_day_of_week(void);
#endif /* BSP_PERIPHERAL_RTC_C_PRESENT */

extern volatile uint32_t g_alarm_irq_flag;
extern volatile uint32_t g_periodic_irq_flag;
extern volatile uint32_t g_invalid_sequence;

/*******************************************************************************************************************//**
 * @brief       This function initializes RTC module.
 * @param[IN]   None
 * @retval      FSP_SUCCESS                  Upon successful open of RTC module.
 * @retval      Any other error code apart from FSP_SUCCESS  Unsuccessful open.
 **********************************************************************************************************************/
fsp_err_t rtc_init(void)
{
    fsp_err_t err = FSP_SUCCESS;     /* Error status */

    /* Open RTC module */
#if (BSP_PERIPHERAL_RTC_C_PRESENT)
    err = R_RTC_C_Open(&g_rtc_ctrl, &g_rtc_cfg);
#else
    err = R_RTC_Open(&g_rtc_ctrl, &g_rtc_cfg);
#endif /* BSP_PERIPHERAL_RTC_C_PRESENT */
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\nR_"RTC_TYPE"_Open API failed.\r\nRestart the Application\r\n");
    }
    return err;
}

#if (BSP_PERIPHERAL_RTC_C_PRESENT)
/*******************************************************************************************************************//**
 * @brief       This function calculates day of week.
 * @param[IN]   None
 * @retval      FSP_SUCCESS                  Day of the week successfully calculated and set.
 * @retval      FSP_ERR_INVALID_ARGUMENT     The provided date is out of valid range.
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
        if (temp_month < MARCH_MONTH)
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
 * @brief       This function sets the Calendar time provided by user.
 * @param[IN]   None
 * @retval      FSP_SUCCESS                  Upon successful Calendar time set.
 * @retval      Any other error code apart from FSP_SUCCESS  Unsuccessful calendar time set.
 **********************************************************************************************************************/
fsp_err_t set_rtc_calendar_time(void)
{
    fsp_err_t err = FSP_SUCCESS;     /* Error status */
    uint8_t read_time[BUFFER_SIZE_DOWN] = {NULL_CHAR};

    APP_PRINT ("\r\nSetting RTC Date and Time\r\n");

    APP_PRINT("\r\nEnter Date and Time in DD:MM:YYYY HH:MM:SS format.\nSample Input: '04:11:2019 15:23:05'\r\n");

    /* Wait until data is available in RTT Viewer */
    while (BYTES_RECEIVED_ZERO == APP_CHECK_DATA);

    /* Reading Calendar time provided by the user */
    APP_READ(read_time);

    /* Modify user provided date and time to standard format */
    rtc_date_time_conversion(&g_set_time, &read_time[0]);

#if (BSP_PERIPHERAL_RTC_C_PRESENT)
    /* For RTC_C, the user is expected to set the weekday correctly in the application */
    err = set_day_of_week();
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nset_day_of_week failed.\r\n");
        return err;
    }

    /* Set the time provided by user */
    err = R_RTC_C_CalendarTimeSet(&g_rtc_ctrl, &g_set_time);
#else
    err = R_RTC_CalendarTimeSet(&g_rtc_ctrl, &g_set_time);
#endif /* BSP_PERIPHERAL_RTC_C_PRESENT */

    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_"RTC_TYPE"_CalendarTimeSet API failed.\r\n");
        return err;
    }

    /* Get the current Calendar time */
#if (BSP_PERIPHERAL_RTC_C_PRESENT)
    err = R_RTC_C_CalendarTimeGet(&g_rtc_ctrl, &g_present_time);
#else
    err = R_RTC_CalendarTimeGet(&g_rtc_ctrl, &g_present_time);
#endif /* BSP_PERIPHERAL_RTC_C_PRESENT */
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_"RTC_TYPE"_CalendarTimeGet API failed.\r\n");
        return err;
    }
    /* Modify the date in standard format to user readable format */
    rtc_date_readability_update(&g_present_time);

    APP_PRINT("\r\nRTC calendar set to Date: %d/%d/%d\r\nTime: %d:%d:%d\r\n\n", g_present_time.tm_mday,
              g_present_time.tm_mon, g_present_time.tm_year, g_present_time.tm_hour,
              g_present_time.tm_min, g_present_time.tm_sec);
    set_time_flag = SET_FLAG;
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function sets the alarm time provided by the user.
 * @param[IN]   None
 * @retval      FSP_SUCCESS                  Upon successful Calendar alarm set.
 * @retval      Any other error code apart from FSP_SUCCESS  Unsuccessful calendar alarm set.
 **********************************************************************************************************************/
fsp_err_t set_rtc_calendar_alarm(void)
{
    fsp_err_t err = FSP_SUCCESS;     /* Error status */
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
rtc_alarm_time_t alarm_time_get =
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
    uint8_t read_alarm[BUFFER_SIZE_DOWN] = {NULL_CHAR};

    /* Set alarm only if proper RTC Calendar date and time is set */
    if (SET_FLAG == set_time_flag)
    {
        APP_PRINT("\r\nSetting Calendar Alarm for RTC\r\n");

#if (BSP_PERIPHERAL_RTC_C_PRESENT)
        /* Reset the timer variables */
        alarm_time_set.weekday_match = RESET_VALUE;
        alarm_time_set.time_hour     = RESET_VALUE;
        alarm_time_set.time_minute   = RESET_VALUE;

        APP_PRINT("\r\nEnter hours and minutes value (Format: HH:MM) for which recursive alarm has to be set."\
                  "\r\nSample Input: '15:24'. At 15:24:00, alarm occurred, and LED will be turned ON.\r\n");

        /* Wait until data is available in RTT */
        while (BYTES_RECEIVED_ZERO == APP_CHECK_DATA);

        /* Reading time value for which alarm has to be generated */
        APP_READ(read_alarm);

        /* Modify user provided time value to standard format */
        alarm_time_set.time_hour = (((read_alarm[0] - ASCII_ZERO) * PLACE_VALUE_TEN )+ (read_alarm[1] - ASCII_ZERO));
        alarm_time_set.time_minute = (((read_alarm[3] - ASCII_ZERO) * PLACE_VALUE_TEN )+ (read_alarm[4] - ASCII_ZERO));
        /* Enable ALARMWW */
        alarm_time_set.weekday_match = ALARMWW;

        /* Set the alarm time provided by the user */
        err = R_RTC_C_CalendarAlarmSet(&g_rtc_ctrl, &alarm_time_set);

        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\nCalendar alarm Set failed.\r\n");
            return err;
        }
        /* Get the alarm time */
        err = R_RTC_C_CalendarAlarmGet(&g_rtc_ctrl, &alarm_time_get);

        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\nCalendar alarm get failed.\r\n");
            return err;
        }
        APP_PRINT("\r\nHours and minutes value for which alarm is set: %d:%d", alarm_time_get.time_hour,\
                  alarm_time_get.time_minute);
    }
#else
        /* Set the flags for which the alarm has to be generated */
        alarm_time_set.hour_match = false;
        alarm_time_set.min_match  = false;
        alarm_time_set.sec_match  = true;
        alarm_time_set.mday_match = false;
        alarm_time_set.mon_match  = false;
        alarm_time_set.year_match = false;

        APP_PRINT("\r\nEnter seconds value (Format: SS) for which recursive alarm has to be set."\
                  "\r\nPlease input seconds value in the range between 00 and 59."\
                  "\r\nSample input: '35'. At HH:MM:35, alarm occurred, and LED will be turned ON.\r\n");

        /* Wait until data is available in RTT Viewer */
        while (BYTES_RECEIVED_ZERO == APP_CHECK_DATA);

        /* Reading time value for which alarm has to be generated */
        APP_READ(read_alarm);

        /* Modify user provided time value to standard format */
        alarm_time_set.time.tm_sec = (((read_alarm[0] - ASCII_ZERO) * PLACE_VALUE_TEN )+ (read_alarm[1] - ASCII_ZERO));

        /* Set the alarm time provided by the user */
        err = R_RTC_CalendarAlarmSet(&g_rtc_ctrl, &alarm_time_set);
        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\nR_RTC_CalendarAlarmSet API failed.\r\n");
            return err;
        }

        /* Get the alarm time */
        err = R_RTC_CalendarAlarmGet(&g_rtc_ctrl, &alarm_time_get);
        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\nR_RTC_CalendarAlarmGet API failed.\r\n");
            return err;
        }
        APP_PRINT("\r\nSeconds value for which alarm is set: %d",alarm_time_get.time.tm_sec);
    }
#endif /* BSP_PERIPHERAL_RTC_C_PRESENT */
    else
    {
        /* Set this flag to notify the user to provide required options */
        g_invalid_sequence = SET_FLAG;
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function sets the periodic interrupt rate provided by the user.
 * @param[IN]   None
 * @retval      FSP_SUCCESS                  Upon successful periodic interrupt rate set.
 * @retval      Any other error code apart from FSP_SUCCESS  Unsuccessful periodic interrupt rate set.
 **********************************************************************************************************************/
fsp_err_t set_rtc_periodic_rate(void)
{
    fsp_err_t err = FSP_SUCCESS;     /* Error status */
    APP_PRINT("\r\nSetting Periodic IRQ rate for RTC\r\n");

    /* Enable periodic interrupt rate */
#if (BSP_PERIPHERAL_RTC_C_PRESENT)
    err = R_RTC_C_PeriodicIrqRateSet(&g_rtc_ctrl, RTC_PERIODIC_IRQ_SELECT_1_SECOND);
#else
    err = R_RTC_PeriodicIrqRateSet(&g_rtc_ctrl, RTC_PERIODIC_IRQ_SELECT_1_SECOND);
#endif /* BSP_PERIPHERAL_RTC_C_PRESENT */
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_"RTC_TYPE"_PeriodicIrqRateSet API failed.\r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function gets the current RTC time.
 * @param[IN]   None
 * @retval      FSP_SUCCESS                  Upon successful current RTC time get.
 * @retval      Any other error code apart from FSP_SUCCESS  Unsuccessful current RTC time get.
 **********************************************************************************************************************/
fsp_err_t get_rtc_calendar_time(void)
{
    fsp_err_t err = FSP_SUCCESS;     /* Error status */

#if (BSP_PERIPHERAL_RTC_C_PRESENT)
    /* Get the current RTC Calendar time */
    err = R_RTC_C_CalendarTimeGet(&g_rtc_ctrl, &g_present_time);
#else
    /* Get the current RTC Calendar time */
    err = R_RTC_CalendarTimeGet(&g_rtc_ctrl, &g_present_time);
#endif /* BSP_PERIPHERAL_RTC_C_PRESENT */
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_"RTC_TYPE"_CalendarTimeGet API failed.\r\n");
        return err;
    }

    /* Modify the date in standard format to user readable format */
    rtc_date_readability_update(&g_present_time);

    APP_PRINT("\r\nRTC Date: %d/%d/%d\r\nTime: %d:%d:%d\r\n\n", g_present_time.tm_mday,
              g_present_time.tm_mon, g_present_time.tm_year, g_present_time.tm_hour,
              g_present_time.tm_min, g_present_time.tm_sec);
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function modifies the date in readable format to the user.
 * @param[IN]   time                    Date to be modified.
 * @retval      None
 **********************************************************************************************************************/
void rtc_date_readability_update(rtc_time_t * time)
{
    time->tm_mon  += MON_ADJUST_VALUE;
    time->tm_year += YEAR_ADJUST_VALUE;
}

/*******************************************************************************************************************//**
 * @brief       This function modifies the user input to RTC date and time.
 * @param[IN]   time                    Date and time to be modified.
 * @param[IN]   read_buffer             Buffer that contains user input time values.
 * @retval      None
 **********************************************************************************************************************/
static void rtc_date_time_conversion(rtc_time_t * time, unsigned char read_buffer[])
{
    /* The user is expected to enter Date and Time in DD:MM:YYYY HH:MM:SS format. This user input in the buffer is
     * processed using the following conversion, where ASCII value is converted to integer value
     * and stored in the RTC time structure.
     * According to DD:MM:YYYY HH:MM:SS format, different array index values are used. */
    time->tm_mday = (((read_buffer[0] - ASCII_ZERO) * PLACE_VALUE_TEN) + (read_buffer[1] - ASCII_ZERO));
    time->tm_mon  = ((((read_buffer[3] - ASCII_ZERO) * PLACE_VALUE_TEN) + (read_buffer[4] - ASCII_ZERO)))\
                    - MON_ADJUST_VALUE;
    time->tm_year = (((read_buffer[6] - ASCII_ZERO) * PLACE_VALUE_THOUSAND) +\
                    ((read_buffer[7] - ASCII_ZERO )* PLACE_VALUE_HUNDRED) +\
                    ((read_buffer[8] - ASCII_ZERO) * PLACE_VALUE_TEN) + (read_buffer[9] - ASCII_ZERO))\
                    - YEAR_ADJUST_VALUE;
    time->tm_hour = (((read_buffer[11] - ASCII_ZERO) * PLACE_VALUE_TEN) + (read_buffer[12] - ASCII_ZERO));
    time->tm_min  = (((read_buffer[14] - ASCII_ZERO) * PLACE_VALUE_TEN) + (read_buffer[15] - ASCII_ZERO));
    time->tm_sec  = (((read_buffer[17] - ASCII_ZERO) * PLACE_VALUE_TEN )+ (read_buffer[18] - ASCII_ZERO));
}

/*******************************************************************************************************************//**
 * @brief      RTC callback function.
 * @param[in]  p_args
 * @retval     None
 **********************************************************************************************************************/
void rtc_callback(rtc_callback_args_t *p_args)
{
    if (RTC_EVENT_ALARM_IRQ == p_args->event)
    {
        g_alarm_irq_flag = SET_FLAG;
    }
    else if (RTC_EVENT_PERIODIC_IRQ == p_args->event)
    {
        g_periodic_irq_flag = SET_FLAG;
    }
    else
    {
        /* Do nothing */
    }
}

/*******************************************************************************************************************//**
 * @brief       This function closes opened RTC module before the project ends up in an error trap.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
void rtc_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close RTC module */
#if (BSP_PERIPHERAL_RTC_C_PRESENT)
    err = R_RTC_C_Close(&g_rtc_ctrl);
#else
    err = R_RTC_Close(&g_rtc_ctrl);
#endif /* BSP_PERIPHERAL_RTC_C_PRESENT */
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* RTC Close failure message */
        APP_ERR_PRINT("** R_"RTC_TYPE"_Close API failed **\r\n");
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup rtc_ep)
 **********************************************************************************************************************/
