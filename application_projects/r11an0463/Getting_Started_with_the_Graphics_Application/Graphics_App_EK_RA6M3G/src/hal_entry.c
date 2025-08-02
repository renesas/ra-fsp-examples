/***********************************************************************************************************************
 * File Name    : hal_entry.c
 * Description  : Contains data structures and initialize application data in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2019 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "hal_data.h"
#include "Application.h"

void R_BSP_WarmStart(bsp_warm_start_event_t event);
/* Pre-set weather forecast*/
static day_info_t forecast[7] =
{
    { "SUN", "Sunny",         "72", "81", "84/73"},
    { "MON", "Partly Cloudy", "50", "74","77/67"},
    { "TUE", "Partly Cloudy", "52", "71","74/65"},
    { "WED", "Rain",          "99", "65", "68/60"},
    { "THU", "Showers",       "85", "73", "76/70"},
    { "FRI", "Partly Cloudy", "50", "80", "83/72"},
    { "SAT", "Sunny",         "45", "89", "92/77"}
};

/*******************************************************************************************************************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart (bsp_warm_start_event_t event)
{
    if (BSP_WARM_START_RESET == event)
    {
#if BSP_FEATURE_FLASH_LP_VERSION != 0

        /* Enable reading from data flash. */
        R_FACI_LP->DFLCTL = 1U;

        /* Would normally have to wait tDSTOP(6us) for data flash recovery. Placing the enable here, before clock and
         * C runtime initialization, should negate the need for a delay since the initialization will typically take more than 6us. */
#endif
    }

    if (BSP_WARM_START_POST_C == event)
    {
        /* C runtime environment and system clocks are setup. */

        /* Configure pins. */
        R_IOPORT_Open(&g_ioport_ctrl, &g_bsp_pin_cfg);

        /* Initialize application data, once after RESET */
        gDataApp.bgrd_darmode           = 0;
        gDataApp.pwm_duty_cycle         = 50;
        gDataApp.text_color             = 0;
        gDataApp.forecast_info          = &forecast[0];
        gDataApp.thermo_target_temp     = 70;
        gDataApp.current_forecast       = WEEK_DAY_SUN;
        gDataApp.sys_weather_type       = SYS_WEATHER_SUNNY;
        gDataApp.total_forecast         = 7;
    }
}
