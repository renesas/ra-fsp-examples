/***********************************************************************************************************************
 * File Name    : timer_thread_entry.c
 * Description  : Contains code for RTC and GPT timers.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2019 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#include <timer_thread.h>
#include "Resource.h"
#include "Application.h"

/* Store current time info */
extern rtc_time_t RtcTimeCurrent;

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


/*******************************************************************************************************************//**
 * @brief This function is setting up GPT/PWM timer
 * @param[in] None
 * @retval    fsp_err_t
 **********************************************************************************************************************/
static fsp_err_t gpt_timer_PWM_setup(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Open GPT */
    err = R_GPT_Open(&g_timer_PWM_ctrl, &g_timer_PWM_cfg);
    if(FSP_SUCCESS != err)
    {
        return err;
    }
    /* Enable GPT Timer */
    err = R_GPT_Enable(&g_timer_PWM_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    /* Start GPT timer */
    err = R_GPT_Start(&g_timer_PWM_ctrl);;
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

    /* Set up GPT/PWM timer using for LCD back light control */
    if(gpt_timer_PWM_setup())
    {
        APP_ERR_TRAP(FSP_ERR_ASSERTION);
    }

    /* Set up RTC timer */
    if(rtc_timer_setup())
    {
        APP_ERR_TRAP(FSP_ERR_ASSERTION);
    }

    while (1)
    {
        /* Wait for interrupt from RTC timer */
        xSemaphoreTake(g_timer_semaphore, portMAX_DELAY);

        /* Get date, time */
        R_RTC_CalendarTimeGet(&g_rtc_timer_ctrl, &RtcTimeCurrent);

        /* Trigger GUI update*/
        APPW_SetVarData(ID_VAR_TIME_UPDATE, 1);

        vTaskDelay (1);
    }
}
