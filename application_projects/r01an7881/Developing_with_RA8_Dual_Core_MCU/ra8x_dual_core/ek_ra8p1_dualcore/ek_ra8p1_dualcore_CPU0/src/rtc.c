/***********************************************************************************************************************
 * File Name    : rtc.c
 * Description  : This source file implements functions declared in `rtc.h` to handle
 *                operation of RTC module.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#include "rtc.h"

/******************************************************************************
 * Global Variables
 *****************************************************************************/
uint8_t g_alarm_irq_flag = RESET_FLAG;
uint8_t g_periodic_irq_flag = RESET_FLAG;

rtc_time_t g_present_time =  {
     .tm_hour    =  RESET_VALUE,
     .tm_isdst   =  RESET_VALUE,
     .tm_mday    =  RESET_VALUE,
     .tm_min     =  RESET_VALUE,
     .tm_mon     =  RESET_VALUE,
     .tm_sec     =  RESET_VALUE,
     .tm_wday    =  RESET_VALUE,
     .tm_yday    =  RESET_VALUE,
     .tm_year    =  RESET_VALUE,
};

/******************************************************************************
 * Private Global Variables
 *****************************************************************************/
static uint32_t set_time_flag = RESET_FLAG;                 //Flag to be set on successful RTC Calendar setting
static uint32_t g_invalid_sequence = RESET_FLAG;            //Flag check for valid sequence.
static rtc_time_t g_set_time =  {
    .tm_hour    =  RESET_VALUE,
    .tm_isdst   =  RESET_VALUE,
    .tm_mday    =  RESET_VALUE,
    .tm_min     =  RESET_VALUE,
    .tm_mon     =  RESET_VALUE,
    .tm_sec     =  RESET_VALUE,
    .tm_wday    =  RESET_VALUE,
    .tm_yday    =  RESET_VALUE,
    .tm_year    =  RESET_VALUE,
};

/******************************************************************************
 * Private Function
 *****************************************************************************/
static void rtc_date_time_conversion(rtc_time_t * time, unsigned char read_buffer[]);

/*******************************************************************************************************************//**
 * @brief     This function to initialize RTC module
 *
 * @param[in] NONE
 * @return    See @ref RENESAS_ERROR_CODES or functions called by this function for other possible
 *            return codes.
 **********************************************************************************************************************/
void rtc_init(void)
{
    fsp_err_t err = FSP_SUCCESS;     // Error status

    /* Open RTC module */
    err = R_RTC_Open(&g_rtc_ctrl, &g_rtc_cfg);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\nRTC module open failed.\r\nRestart the Application\r\n");
        APP_ERR_TRAP(err);
    }
}

/*******************************************************************************************************************//**
 * @brief     Update Month and Year in readability Format
 *
 * @param[in] rtc_time_t * Buffer contained Date and Time information
 * @return    NONE
 **********************************************************************************************************************/
void rtc_date_readability_update(rtc_time_t * time)
{
    /* Checking for input parameter */
    APP_ASSERT(NULL != time);

    time->tm_mon  +=  MON_ADJUST_VALUE;
    time->tm_year +=  YEAR_ADJUST_VALUE;
}

/*******************************************************************************************************************//**
 * @brief     This function to convert the date and Time in ASCII to integer
 *            and Stored in the RTC time structure.
 * @param[in] rtc_time_t * Buffer store date and time in RTC structure.
 * @param[in] unsigned char * Input buffer in ASCII format.
 * @return    NONE.
 **********************************************************************************************************************/
static void rtc_date_time_conversion(rtc_time_t * time, unsigned char read_buffer[])
{
    /* Checking for input parameter */
    APP_ASSERT(NULL != time);
    APP_ASSERT(NULL != read_buffer);

    /* User is expected to enter Date and Time in DD:MM:YYYY HH:MM:SS format. This User input in the buffer is processed
     * using the following conversion, where ASCII value is converted to integer value and stored in the RTC time structure.
     * According to DD:MM:YYYY HH:MM:SS format, different array index values are used.  */
    time->tm_mday = (((read_buffer[0] - ASCII_ZERO) * PLACE_VALUE_TEN) + (read_buffer[1] - ASCII_ZERO));
    time->tm_mon = ((((read_buffer[3] - ASCII_ZERO) * PLACE_VALUE_TEN) + (read_buffer[4] - ASCII_ZERO))) - MON_ADJUST_VALUE;
    time->tm_year = (((read_buffer[6] - ASCII_ZERO) * PLACE_VALUE_THOUSAND) +
            ((read_buffer[7] - ASCII_ZERO )* PLACE_VALUE_HUNDRED) +
            ((read_buffer[8] - ASCII_ZERO) * PLACE_VALUE_TEN) + (read_buffer[9] - ASCII_ZERO)) - YEAR_ADJUST_VALUE;
    time->tm_hour = (((read_buffer[11] - ASCII_ZERO) * PLACE_VALUE_TEN) + (read_buffer[12] - ASCII_ZERO));
    time->tm_min = (((read_buffer[14] - ASCII_ZERO) * PLACE_VALUE_TEN) + (read_buffer[15] - ASCII_ZERO));
    time->tm_sec = (((read_buffer[17] - ASCII_ZERO) * PLACE_VALUE_TEN )+ (read_buffer[18] - ASCII_ZERO));
}

/*******************************************************************************************************************//**
 * @brief     This function sets the Date and Time for the RTC module.
 *
 * @param[in] uint8_t * Date and Time Buffer.
 * @return    See @ref RENESAS_ERROR_CODES or functions called by this function for other possible
 *            return codes.
 **********************************************************************************************************************/
fsp_err_t set_rtc_calendar_time(uint8_t *pBuffer)
{
    APP_ASSERT(NULL != pBuffer);

    fsp_err_t err = FSP_SUCCESS;
    /* Modify user provided date and time to standard format */
    rtc_date_time_conversion(&g_set_time, pBuffer);

    err = R_RTC_CalendarTimeSet(&g_rtc_ctrl, &g_set_time);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Modify the date in standard format to user readable format */
    rtc_date_readability_update(&g_present_time);

    set_time_flag = SET_FLAG;

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function sets RTC in 1 second periodically.
 *
 * @param[in] NONE
 * @return    NONE
 **********************************************************************************************************************/
void set_rtc_periodic_second(void)
{
    fsp_err_t err = FSP_SUCCESS;     // Error status

    /* Enable periodic interrupt rate */
    err = R_RTC_PeriodicIrqRateSet(&g_rtc_ctrl, RTC_PERIODIC_IRQ_SELECT_1_SECOND);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nPeriodic interrupt rate setting failed.\r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief     This function sets RTC alarm
 *
 * @param[in] uint8_t The alarm value in second
 * @return    NONE
 **********************************************************************************************************************/
void set_rtc_calendar_alarm(uint8_t sec_alarm)
{
    fsp_err_t err = FSP_SUCCESS;
    rtc_alarm_time_t alarm_time_set =
    {
         .sec_match        =  RESET_VALUE,
         .min_match        =  RESET_VALUE,
         .hour_match       =  RESET_VALUE,
         .mday_match       =  RESET_VALUE,
         .mon_match        =  RESET_VALUE,
         .year_match       =  RESET_VALUE,
         .dayofweek_match  =  RESET_VALUE,
    };
    rtc_alarm_time_t alarm_time_get =
    {
        .sec_match        =  RESET_VALUE,
        .min_match        =  RESET_VALUE,
        .hour_match       =  RESET_VALUE,
        .mday_match       =  RESET_VALUE,
        .mon_match        =  RESET_VALUE,
        .year_match       =  RESET_VALUE,
        .dayofweek_match  =  RESET_VALUE,
   };

    /*Set alarm only if proper RTC Calendar date and Time is set */
    if (SET_FLAG == set_time_flag)
    {
        /* Set the flags for which the alarm has to be generated */
        alarm_time_set.hour_match  = false;
        alarm_time_set.min_match   = false;
        alarm_time_set.sec_match   = true;
        alarm_time_set.mday_match  = false;
        alarm_time_set.mon_match   = false;
        alarm_time_set.year_match  = false;

        /* Modify user provided time value to standard format */
        alarm_time_set.time.tm_sec = sec_alarm;

        /* Set the alarm time provided by user*/
        err = R_RTC_CalendarAlarmSet(&g_rtc_ctrl, &alarm_time_set);
        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\nCalendar alarm Set failed.\r\n");
        }

        /* Get the alarm time */
        err = R_RTC_CalendarAlarmGet(&g_rtc_ctrl, &alarm_time_get);
        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\nCalendar alarm Get failed.\r\n");
        }
    }
    else
    {
        /* Set this flag to notify the user to provide required options */
        g_invalid_sequence = SET_FLAG;
    }
}

/*******************************************************************************************************************//**
 * @brief     The RTC Callback Function
 *
 * @param[in] rtc_callback_args_t * callback arguments
 * @return    NONE
 **********************************************************************************************************************/
void rtc_callback(rtc_callback_args_t *p_args)
{
    if(RTC_EVENT_ALARM_IRQ == p_args->event)
    {
        g_alarm_irq_flag = SET_FLAG;
    }
    else
    {
        g_periodic_irq_flag = SET_FLAG;
    }
}

/*******************************************************************************************************************//**
 * @brief     This function to Close RTC module
 *
 * @param[in] NONE
 * @return    NONE
 **********************************************************************************************************************/
void rtc_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close RTC module */
    err = R_RTC_Close(&g_rtc_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* RTC Close failure message */
        APP_ERR_PRINT("** RTC module Close failed **  \r\n");
    }
}

