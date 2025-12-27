/***********************************************************************************************************************
 * File Name    : timer_thread_entry.c
 * Description  : Contains code for RTC and GPT timers.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#include <timer_thread.h>
#include <stdio.h>
#include "ipc_squeue.h"

const char* const weekdays[] = {"SUN","MON","TUE","WED","THU","FRI","SAT"};
const char* const months[]   = {"Jan","Feb","Mar","Apr","May","Jun",
                                 "Jul","Aug","Sep","Oct","Nov","Dec"};

static char rtc_buf[35] BSP_PLACE_IN_SECTION(".shared_mem");

/*******************************************************************************************************************//**
 * @brief This function is callback for RTC timer
 * @param[in] (timer_callback_args_t *)
 * @retval None
 **********************************************************************************************************************/
void g_rtc_timer_cb(rtc_callback_args_t *p_args)
{
    if(RTC_EVENT_PERIODIC_IRQ == p_args->event)
      {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        /* Set Timer semaphore */
        xSemaphoreGiveFromISR(g_timer_semaphore, &xHigherPriorityTaskWoken);

        /* Return to the highest priority available task */
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
      }
}

/*******************************************************************************************************************//**
 * @brief This function is setting up RTC timer
 * @param[in] None
 * @retval    fsp_err_t
 **********************************************************************************************************************/
static fsp_err_t rtc_timer_setup(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Set up default time
     * Default Time: Wednesday 9:30:00, 22 Oct 2025, not daylight saving time */
    rtc_time_t default_time = {
            .tm_sec   = 0,
            .tm_min   = 30,
            .tm_hour  = 9,
            .tm_mday  = 22,
            .tm_mon   = 9,          // Oct => 9 (months start from 0)
            .tm_year  = 125,        // 2025 - 1900 = 125
            .tm_wday  = 3,          // Wed = 3 (0 = Sunday)
            .tm_yday  = 294,        // 22 oct is the 295th day of the year, starting from 0
            .tm_isdst = 0           // Not daylight saving time
    };

    /* Open RTC */
    err = R_RTC_Open(&g_rtc_timer_ctrl, &g_rtc_timer_cfg);
    if(FSP_SUCCESS != err)
    {
        return err;
    }
    /* Enable periodic interrupt */
    err = R_RTC_PeriodicIrqRateSet(&g_rtc_timer_ctrl, RTC_PERIODIC_IRQ_SELECT_1_SECOND);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    /* Set default calendar */
    err = R_RTC_CalendarTimeSet(&g_rtc_timer_ctrl, &default_time);
    if(FSP_SUCCESS != err)
    {
        return err;
    }

    return FSP_SUCCESS;
}


/* Timer Thread entry function */
/* pvParameters contains TaskHandle_t */
void timer_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);
    fsp_err_t err = FSP_SUCCESS;

    /* Store current time info */
    rtc_time_t RtcTimeCurrent = {0};

    /* Set up RTC timer */
    if(rtc_timer_setup())
    {
        __asm("BKPT #0\n");
    }
    while (1)
    {
        /* Wait for interrupt from RTC timer */
        xSemaphoreTake(g_timer_semaphore, portMAX_DELAY);

        /* Get date, time */
        R_RTC_CalendarTimeGet(&g_rtc_timer_ctrl, &RtcTimeCurrent);

        err = R_BSP_IpcSemaphoreTake(&g_ipc_sem_rtc);
        if(FSP_SUCCESS == err)
        {
            snprintf(rtc_buf, sizeof(rtc_buf), "%.3s     %.02d %.3s %.04d    %.02d:%.02d:%.02d %.2s",
                     weekdays[RtcTimeCurrent.tm_wday],
                     RtcTimeCurrent.tm_mday,
                     months[RtcTimeCurrent.tm_mon],
                     1900 + RtcTimeCurrent.tm_year,
                     RtcTimeCurrent.tm_hour,
                     RtcTimeCurrent.tm_min,
                     RtcTimeCurrent.tm_sec,
                     (RtcTimeCurrent.tm_hour < 12) ? "AM" : "PM"
            );

            /* Generate interrupt request to CPU0 to update time on the display */
            R_IPC_EventGenerate(&g_ipc0_ctrl, IPC_GENERATE_EVENT_IRQ3);
        }
        vTaskDelay (1);
    }
}
