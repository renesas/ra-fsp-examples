
/***********************************************************************************************************************
 * File Name    : system_time.c
 * Description  : Contains functions to initialize and adjust RTC
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
************************************************************************************************************************/
/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#define SYS_TIME_C
#include "system_time.h"
#include <temperature_time_thread.h>
#include "system_api.h"


/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/
static const uint8_t month_table[12] = {0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5};
static const uint8_t ly_month_table[12] = {6, 2, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5};
static const uint8_t days_in_month[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/***********************************************************************************************************************
 * Private functions
 **********************************************************************************************************************/
static int weekday_get(rtc_time_t * p_time);
static void time_validate(int * time, int max, bool day_flag);

/***********************************************************************************************************************
 * Global Variables
 **********************************************************************************************************************/
volatile bool g_time_updated = false;

/** Stored GUI state. */
extern system_state_t g_gui_state;

/*******************************************************************************************************************//**
 * @brief Callback used by the RTC to update the system time.
 *
 * @param[in] p_context User-supplied context carrying any data needed during callback processing.
 **********************************************************************************************************************/
void time_update_callback (rtc_callback_args_t * p_context)
{
    if(RTC_EVENT_PERIODIC_IRQ == p_context->event)
      {
        /* Set Timer semaphore */
        tx_semaphore_put(&g_timer_semaphore);
      }
}

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief   Corrects the time value if it has wrapped above the maximum value or below 0.
 *
 * @param[in]   p_time  Time value
 * @param[in]   max     Maximum value for this time
***********************************************************************************************************************/
static void time_validate(int * p_time, int max, bool day_flag)
{
    if (day_flag == false)
    {
        if (*p_time < 0)
        {
            *p_time = max - 1;
        }
        if (*p_time >= max)
        {
            *p_time = 0;
        }
    }
    else
    {
        if (*p_time <= 0)
        {
            *p_time = max - 1;
        }
        if (*p_time >= max)
        {
            *p_time = 1;
        }
    }
}
/*******************************************************************************************************************//**
 * @brief   Modifies the current time variable by the amount of time requested.  Wrap around events will not affect the
 *
 *
 * @param[in]   years   number of years to adjust by (signed value)
 * @param[in]   months  number of months to adjust by (signed value)
 * @param[in]   days    number of days to adjust by (signed value)
 * @param[in]   hours   number of hours to adjust by (signed value)
 * @param[in]   mins    number of minutes to adjust by (signed value)
 * @param[in]   secs    number of seconds to adjust by (signed value)
***********************************************************************************************************************/
void adjust_time(rtc_time_t * p_time)
{
    rtc_time_t t;
    g_rtc.p_api->calendarTimeGet(g_rtc.p_ctrl, &t);
    t.tm_sec    += p_time->tm_sec;
    t.tm_min    += p_time->tm_min;
    t.tm_hour   += p_time->tm_hour;
    t.tm_mday   += p_time->tm_mday;
    t.tm_mon    += p_time->tm_mon;
    t.tm_year   += p_time->tm_year;

    bool leap = false;
    if (0 == t.tm_year % 4)
    {
        leap = true;
        if (0 == t.tm_year % 100)
        {
            leap = false;
            if (0 != t.tm_year % 400)
            {
                leap = true;
            }
        }
    }

    /** Ensure that all time values are valid. */
    time_validate(&t.tm_sec, 60, false);
    time_validate(&t.tm_min, 60, false);
    time_validate(&t.tm_hour, 24, false);
    time_validate(&t.tm_mon, 12, false);

    /** Calculate the maximum days in the current month. */
    int32_t temp_month = (t.tm_mon + 12) % 12;
    uint8_t days_in_current_month = days_in_month[temp_month];
    if (leap && (1 == temp_month))
    {
        days_in_current_month++;
    }

    time_validate(&t.tm_mday, days_in_current_month+1, true);
    t.tm_wday = weekday_get(&t);

    /* Set time.*/
    R_RTC_CalendarTimeSet(&g_rtc_ctrl, &t);
}

int weekday_get(rtc_time_t * p_time)
{
    int32_t day = p_time->tm_mday;
    bool leap = false;
    if (0 == p_time->tm_year % 4)
    {
        leap = true;
        if (0 == p_time->tm_year % 100)
        {
            leap = false;
            if (0 != p_time->tm_year % 400)
            {
                leap = true;
            }
        }
    }
    int32_t month_offset;
    if (leap)
    {
        month_offset = ly_month_table[p_time->tm_mon];
    }
    else
    {
        month_offset = month_table[p_time->tm_mon];
    }
    int32_t year_offset = p_time->tm_year % 100;
    int32_t century_offset = (3 - ((p_time->tm_year / 100) % 4)) * 2;
    int32_t offset = day + month_offset + year_offset + year_offset / 4 + century_offset;
    int32_t index = offset % 7;

    return index;
}

void system_time_init(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Set up default time */
    rtc_time_t default_time = {
            .tm_sec   = TM_SEC,
            .tm_min   = TM_MIN,
            .tm_hour  = TM_HOUR,
            .tm_mday  = TM_MDAY,
            .tm_mon   = TM_MON,
            .tm_year  = TM_YEAR,
            .tm_wday  = TM_WDAY,
            .tm_yday  = TM_YDAY,
            .tm_isdst = TM_ISDST
    };

    err = R_RTC_Open(&g_rtc_ctrl, &g_rtc_cfg);
    if (FSP_SUCCESS == err)
    {
        /* Enable periodic interrupt */
        err = R_RTC_PeriodicIrqRateSet(&g_rtc_ctrl, RTC_PERIODIC_IRQ_SELECT_1_SECOND);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_TRAP(FSP_ERR_ASSERTION);
        }

        /* Set default calendar */
        err = R_RTC_CalendarTimeSet(&g_rtc_ctrl, &default_time);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_TRAP(FSP_ERR_ASSERTION);
        }

        /** Set default hour mode. */
        g_gui_state.hour_mode = SYSTEM_HOUR_MODE_12;
    } else {
        APP_ERR_TRAP(FSP_ERR_ASSERTION);
    }
}

/** @} (end) */
