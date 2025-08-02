/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : agt_slcd_flash.c
 * Version      : 1.0
 * Description  : slcd alarm support.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 20.05.2025 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "common_utils.h"
#include "common_init.h"
#include "jlink_console.h"
#include "menu_main.h"
#include "menu_slcd.h"
#include "slcd.h"
#include "lp_mode.h"
#include "agt_slcd_flash.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define AGT_EXAMPLE_MSEC_PER_SEC           (1000)
#define AGT_EXAMPLE_DESIRED_PERIOD_MSEC    (1000)

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/
int16_t g_alarm_flash;

/**********************************************************************************************************************
 * Function Name: agt_period_start
 * Description  : Set the AGT timer to support Alarm Detection
 * Return Value : NONE
 *********************************************************************************************************************/
void agt_period_start(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initializes the module. */
    err = R_AGT_Open (&g_timer1_ctrl, &g_timer1_cfg);

    /* Handle any errors. This function should be defined by the user. */
    assert (FSP_SUCCESS == err);

    /* Start the timer. */
    (void)R_AGT_Start(&g_timer1_ctrl);

    /* Get the source clock frequency (in Hz). There are several ways to do this in FSP:
     *  - If LOCO or subclock is chosen in agt_extended_cfg_t::clock_source
     *      - The source clock frequency is BSP_LOCO_HZ >> timer_cfg_t::source_div
     *  - If PCLKB is chosen in agt_extended_cfg_t::clock_source and the PCLKB frequency has not changed since reset,
     *      - The source clock frequency is BSP_STARTUP_PCLKB_HZ >> timer_cfg_t::source_div
     *  - Use the R_AGT_InfoGet function (it accounts for the clock source and divider).
     *  - Calculate the current PCLKB frequency using R_FSP_SystemClockHzGet(FSP_PRIV_CLOCK_PCLKB) and right shift
     *    by timer_cfg_t::source_div.
     *
     * This example uses the first option by SUB_CLOCK.
     */
    volatile uint32_t timer_freq_hz = BSP_SUB_CLOCK_HZ >> g_timer1_cfg.source_div;

    /* Calculate the desired period based on the current clock. Note that this calculation could overflow if the
     * desired period is larger than UINT32_MAX / pclkb_freq_hz. A cast to uint64_t is used to prevent this. */
    volatile uint32_t period_counts = (uint32_t) (((uint64_t) timer_freq_hz*AGT_EXAMPLE_DESIRED_PERIOD_MSEC)
            / AGT_EXAMPLE_MSEC_PER_SEC);

    /* Set the calculated period. This will return an error if parameter checking is enabled and the calculated
     * period is larger than UINT16_MAX. */

    err = R_AGT_PeriodSet (&g_timer1_ctrl, period_counts);
    assert (FSP_SUCCESS == err);
}
/**********************************************************************************************************************
 End of function agt_period_start
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: agt_alarm_callback
 * Description  : Action the AGT timer Alarm Notification
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
                if (g_mins_wake.min_match)
                {
                    R_AGT_Stop(&g_timer1_ctrl);
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


