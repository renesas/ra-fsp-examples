/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : update_slcd.c
 * Version      : .
 * Description  : .
 *********************************************************************************************************************/
#include "common_init.h"
#include "slcd.h"
#include "r_rtc_api.h"
#include "r_rtc.h"
#include "lp_mode.h"
#include "agt_slcd_fresh.h"
#include "update_slcd.h"


extern adc_info_t       g_adc_info_rtn;
extern rtc_alarm_time_t g_rtc_alarm;
extern rtc_alarm_time_t g_mins_weak;

extern uint32_t g_alarm_cal;
extern bool_t   g_alarm_ring;
extern bool_t   g_s_alarm_reached;


static rtc_time_t s_latest_update_time =
{ };

static rtc_time_t s_current_time =
{ };


const uint16_t g_day_of_week_list[7] =
{ SUNDAY_ICON, MONDAY_ICON, TUESDAY_ICON, WEDNESDAY_ICON, THURSDAY_ICON, FRIDAY_ICON, SATURDAY_ICON };






/**********************************************************************************************************************
 * Function Name: rtc_update_callback
 * Description  : .
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void rtc_update_callback(rtc_callback_args_t *p_args)
{
    switch (p_args->event)
    {
        case RTC_EVENT_ALARM_IRQ:
            if (false == g_mins_weak.min_match)
            {
                g_s_alarm_reached = true;

            }
            else
            {
                /*Run if the interrupt from Low power mode*/
                g_alarm_ring = true;
                g_alarm_cal--;
            }
            break;

        case RTC_EVENT_ALARM1_IRQ:

            break;

        case RTC_EVENT_PERIODIC_IRQ:
            R_RTC_CalendarTimeGet (&g_rtc_ctrl, &s_current_time);

            if (s_latest_update_time.tm_min != s_current_time.tm_min)
            {
                set_time_on_slcd (&s_current_time);
                s_latest_update_time.tm_min = s_current_time.tm_min;
            }
            if ((s_current_time.tm_wday != g_rtc_alarm.time.tm_wday)
                || (s_current_time.tm_hour != g_rtc_alarm.time.tm_hour)
                || (s_current_time.tm_min != g_rtc_alarm.time.tm_min))
            {
                g_s_alarm_reached = false;
            }
            break;
        default:
            break;
    }
}
/**********************************************************************************************************************
 End of function rtc_update_callback
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: set_time_on_slcd
 * Description  : .
 * Argument     : p_time
 * Return Value : .
 *********************************************************************************************************************/
void set_time_on_slcd(rtc_time_t *p_time)
{
    uint32_t value = 0;

    uint8_t  time_h      = (uint8_t) p_time->tm_hour; /* Cast to uint8_t */
    uint16_t day_of_week = 0;

    if (0 == time_h)
    {
        /* special case 12AM */
        time_h += 12;
        set_segment_icon_named (AM_ICON, ICON_ON);
        set_segment_icon_named (PM_ICON, ICON_OFF);
    }
    else if (12 == time_h)
    {
        /* special case 12PM */
        set_segment_icon_named (AM_ICON, ICON_OFF);
        set_segment_icon_named (PM_ICON, ICON_ON);
    }
    else if (time_h > 12)
    {
        /* special case PM */
        time_h -= 12;
        set_segment_icon_named (AM_ICON, ICON_OFF);
        set_segment_icon_named (PM_ICON, ICON_ON);
    }
    else
    {
        /* Normal case */
        set_segment_icon_named (AM_ICON, ICON_ON);
        set_segment_icon_named (PM_ICON, ICON_OFF);
    }

    /* Current Time */
    value  = (uint32_t) (time_h * 100); // set digits 2-3 to the hour
    value += (uint32_t) (p_time->tm_min);        // set digits 0-1 to the minute

    set_segments_medium_digits (value, SET_COLON);

    /* clear all day of week segments */
    for (uint8_t i = 0; i <= 6; i++)
    {
        set_segment_icon_named (g_day_of_week_list[i], ICON_OFF);
    }

    /* day of week */
    day_of_week = g_day_of_week_list[p_time->tm_wday];
    set_segment_icon_named (day_of_week, ICON_ON);
}
/**********************************************************************************************************************
 End of function set_time_on_slcd
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: set_temp_on_slcd
 * Description  : .
 * Argument     : p_status
 * Return Value : .
 *********************************************************************************************************************/
void set_temp_on_slcd(st_board_status_t *p_status)
{
    int16_t value = 0;

    /* Check if Calibration of ADC is enabled and temperature is in valid range */
    if ((ADC_CALIBRATION_NA != g_adc_info_rtn.calibration_data)
            && (p_status->temperature_c.whole_number < ADC_CALIBRATION_ERROR))
    {
        /* Current temperature */
        value  = (int16_t) (p_status->temperature_c.whole_number * 10);     // set digits 2 - 3 to integer part
        value += (int16_t) (p_status->temperature_c.mantissa / 10);          // set digits 0 - 1 to mantissa

        set_segments_big_digits (value, WITH_DP);
        set_segment_icon_named (DEG_C_ICON, ICON_ON);
    }
    else
    {
        value = 0x7FFF;
        set_segments_big_digits (value, NO_DP);
        set_segment_icon_named (DEG_C_ICON, ICON_OFF);
        set_segment_icon_named (DEG_F_ICON, ICON_OFF);
        set_segment_icon_named (RENESAS_ICON, ICON_ON);
    }
}
/**********************************************************************************************************************
 End of function set_temp_on_slcd
 *********************************************************************************************************************/



