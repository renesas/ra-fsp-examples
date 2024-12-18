/***********************************************************************************************************************
 * File Name    : temperature_time_thread_entry.c
 * Description  : Contains entry function of temperature and RTC timer thread .
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
#include "temperature_time_thread.h"
#include "brightness.h"
#include "system_time.h"
#include "guix_gen/thermostat_resources.h"
#include "guix_gen/thermostat_specifications.h"

/***********************************************************************************************************************
 Private functions
 ***********************************************************************************************************************/
static fsp_err_t ADC_Setup(void);

/** Stored GUI state. */
extern system_state_t g_gui_state;

/* Temperature Time Thread entry function */
void temperature_time_thread_entry(void)
{
    fsp_err_t     err            = FSP_SUCCESS;
    UINT          status         = TX_SUCCESS;
    uint16_t      adc_temp_data  = 0;
    /* Set up ADC module */
    err = ADC_Setup();
    if(FSP_SUCCESS != err)
    {
        APP_ERR_TRAP(FSP_ERR_ASSERTION);
    }

    while (1)
    {
        /* Wait for RTC interrupt. */
        status = tx_semaphore_get(&g_timer_semaphore, TX_WAIT_FOREVER);
        if(TX_SUCCESS != status)
        {
            APP_ERR_TRAP(FSP_ERR_ASSERTION);
        }

        /* Get date, time */
        /* Lock access to resource*/
        tx_mutex_get(&g_sys_mutex, TX_WAIT_FOREVER);
        R_RTC_CalendarTimeGet(&g_rtc_ctrl, &g_gui_state.time);
        /* Send GUIX event to update time */
        send_hmi_message(GXEVENT_MSG_TIME_UPDATE);;
        tx_mutex_put(&g_sys_mutex);

        /* Delay and update temperature*/
        tx_thread_sleep (10);

        /* Read die temperature */
        err = R_ADC_Read(&g_adc_ctrl, ADC_CHANNEL_TEMPERATURE, &adc_temp_data);
        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            APP_ERR_TRAP(err);
        }

        /* Conversion of ADC temperature in celsius */
        g_gui_state.temp_c = ADCTEMP_AS_C(adc_temp_data);

        /* Send GUIX event to update time */
        send_hmi_message(GXEVENT_MSG_UPDATE_TEMPERATURE);
        tx_thread_sleep (1);
    }
}

/*******************************************************************************************************************//**
 * @brief This function is setting up ADC
 * @param[in] None
 * @retval    fsp_err_t
 **********************************************************************************************************************/
static fsp_err_t ADC_Setup(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    fsp_err = R_ADC_Open(&g_adc_ctrl, &g_adc_cfg);
    if(FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }

    fsp_err = R_ADC_ScanCfg(&g_adc_ctrl, &g_adc_channel_cfg);
    if(FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }

    fsp_err = R_ADC_ScanStart(&g_adc_ctrl);
    if(FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }

    return fsp_err;
}

/*******************************************************************************************************************//**
 * @brief This function sends event to GUIX
 * @param[in] gx_event_message_t
 * @retval    none
 **********************************************************************************************************************/
void send_hmi_message(gx_event_message_t event)
{
    UINT gx_err                    = GX_SUCCESS;
    system_payload_t message_to_gx = {0};

    /** Create message. */
    message_to_gx.gx_event.gx_event_type = event;
    message_to_gx.state = g_gui_state;
    message_to_gx.gx_event.gx_event_target = GX_NULL;
    message_to_gx.gx_event.gx_event_payload.gx_event_ulongdata = (ULONG) &message_to_gx;

    /** Post message. */
    gx_err = gx_system_event_send (&message_to_gx.gx_event);
    if (GX_SUCCESS != gx_err)
    {
        APP_ERR_TRAP(FSP_ERR_ASSERTION);
    }
}
