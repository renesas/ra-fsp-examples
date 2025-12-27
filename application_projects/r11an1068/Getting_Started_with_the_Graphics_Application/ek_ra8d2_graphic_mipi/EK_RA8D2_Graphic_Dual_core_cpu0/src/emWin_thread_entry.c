/***********************************************************************************************************************
 * File Name    : emWin_thread_entry.c
 * Description  : Thread handles graphics framework setup and envokes the emWin MainTask thread.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#include "emWin_thread.h"
#include "GUI.h"
#include "Application.h"

/* Pre-set weather forecast */
static day_info_t forecast[7] =
{
    { "SUN", "Sunny",         "72", "81", "84/73"},
    { "MON", "Partly Cloudy", "50", "74", "77/67"},
    { "TUE", "Partly Cloudy", "52", "71", "74/65"},
    { "WED", "Rain",          "99", "65", "68/60"},
    { "THU", "Showers",       "85", "73", "76/70"},
    { "FRI", "Partly Cloudy", "50", "80", "83/72"},
    { "SAT", "Sunny",         "45", "89", "92/77"}
};

/* emWin Thread entry function */
/* pvParameters contains TaskHandle_t */
void emWin_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);

    fsp_err_t err = FSP_SUCCESS;

    /* Initialize application data, once after reset */
    gDataApp.bgrd_darmode           = 0;
    gDataApp.pwm_duty_cycle         = 50;
    gDataApp.text_color             = 0;
    gDataApp.forecast_info          = &forecast[0];
    gDataApp.thermo_target_temp     = 70;
    gDataApp.current_forecast       = WEEK_DAY_WED;
    gDataApp.sys_weather_type       = SYS_WEATHER_RAINY;
    gDataApp.total_forecast         = 7;

    /* Initialize ospi_b controller and setting up external flash device */
    err = ospi_b_init();
    APP_ERR_TRAP(err);

    /* switch to OPI 8D-8D-8D mode */
    err = ospi_b_set_protocol_to_opi();
    APP_ERR_TRAP(err);

    /* Enter XIP mode */
    err = xip_enter();
    APP_ERR_TRAP(err);

    /* Wait for configuration touch controller */
    BaseType_t xResult = xSemaphoreTake(g_touch_reset_semaphore, portMAX_DELAY);
    APP_ERR_TRAP(!xResult);

    MainTask();

    while (1)
    {
        vTaskDelay (1);
    }
}

