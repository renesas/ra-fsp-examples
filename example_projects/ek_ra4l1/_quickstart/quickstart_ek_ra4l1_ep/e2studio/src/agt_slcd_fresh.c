/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2025 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : agt_slcd_fresh.c
 * Version      : .
 * Description  : slcd alarm support.
 *********************************************************************************************************************/


#include "common_utils.h"
#include "common_init.h"
#include "jlink_console.h"
#include "menu_main.h"
#include "slcd.h"
#include "lp_mode.h"
#include "agt_slcd_fresh.h"

#define AGT_EXAMPLE_MSEC_PER_SEC           (1000)
#define AGT_EXAMPLE_DESIRED_PERIOD_MSEC    (1000)
/* This example shows how to calculate a new period value at runtime. */

extern rtc_alarm_time_t g_rtc_alarm;
extern rtc_alarm_time_t g_mins_weak;

int16_t g_alarm_flash;



/**********************************************************************************************************************
 * Function Name: agt_period_start
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void agt_period_start(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initializes the module. */
    err = R_AGT_Open (&g_timer0_ctrl, &g_timer0_cfg);

    /* Handle any errors. This function should be defined by the user. */
    assert (FSP_SUCCESS == err);

    /* Start the timer. */
    (void)R_AGT_Start(&g_timer0_ctrl);

    /* Get the source clock frequency (in Hz). There are several ways to do this in FSP:
     *  - If LOCO or subclock is chosen in agt_extended_cfg_t::clock_source
     *      - The source clock frequency is BSP_LOCO_HZ >> timer_cfg_t::source_div
     *  - If PCLKB is chosen in agt_extended_cfg_t::clock_source and the PCLKB frequency has not changed since reset,
     *      - The source clock frequency is BSP_STARTUP_PCLKB_HZ >> timer_cfg_t::source_div
     *  - Use the R_AGT_InfoGet function (it accounts for the clock source and divider).
     *  - Calculate the current PCLKB frequency using R_FSP_SystemClockHzGet(FSP_PRIV_CLOCK_PCLKB) and right shift
     *    by timer_cfg_t::source_div.
     *
     * This example uses the first option but SUB_CLOCK .
     */
    uint32_t timer_freq_hz = BSP_SUB_CLOCK_HZ >> g_timer0_cfg.source_div;

    /* Calculate the desired period based on the current clock. Note that this calculation could overflow if the
     * desired period is larger than UINT32_MAX / pclkb_freq_hz. A cast to uint64_t is used to prevent this. */
    uint32_t period_counts = (uint32_t) (((uint64_t) timer_freq_hz*AGT_EXAMPLE_DESIRED_PERIOD_MSEC)
            / AGT_EXAMPLE_MSEC_PER_SEC);

    /* Set the calculated period. This will return an error if parameter checking is enabled and the calculated
     * period is larger than UINT16_MAX. */
    err = R_AGT_PeriodSet (&g_timer0_ctrl, period_counts);

    assert (FSP_SUCCESS == err);
}
/**********************************************************************************************************************
 End of function agt_period_start
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: agt_alarm_callback
 * Description  : .
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void agt_alarm_callback(timer_callback_args_t * p_args)
{
    static bool_t s_segment_case = true;
    switch (p_args->event)
    {
        case TIMER_EVENT_CYCLE_END:
        {
            if ((g_alarm_flash > 0) || g_s_alarm_reached)
            {
                if (s_segment_case)
                {
                    set_segment_icon_named (ALARM_ICON, ICON_ON);
                    s_segment_case = !s_segment_case;
                }
                else
                {
                    set_segment_icon_named (ALARM_ICON, ICON_OFF);
                    s_segment_case = !s_segment_case;
                }
                if (g_alarm_flash > 0)
                {
                    g_alarm_flash--;
                }
            }
            else
            {
                set_segment_icon_named (ALARM_ICON, !g_rtc_alarm.dayofweek_match);
                if (g_mins_weak.min_match)
                {
                    R_AGT_Stop(&g_timer0_ctrl);
                }
            }
            break;
        }
        default:
            break;
    }
}
/**********************************************************************************************************************
 End of function agt_alarm_callback
 *********************************************************************************************************************/

