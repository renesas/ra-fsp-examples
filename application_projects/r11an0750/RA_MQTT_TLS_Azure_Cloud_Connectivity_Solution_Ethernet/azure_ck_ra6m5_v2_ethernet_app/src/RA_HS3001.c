/***********************************************************************************************************************
 * File Name    : RA_HS3001.c
 * Description  : Contains data structures and function definations for HS3001 sensor
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
 * Copyright (C) 2020-2023 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
#include "RA_HS3001.h"
#include "common_utils.h"
#include "Sensor_Thread.h"
#include "usr_app.h"
#include "user_choice.h"

/* Utility MACRO to loop 50ms delays until expression becomes false*/
#define WAIT_WHILE_FALSE(e) ({while(!(e)) { __asm(" nop"); }})

/* Variable declarations*/
static volatile fsp_err_t g_err; /* FSP Error variable*/
usr_hs3001_data_t g_hs3001_data_s; /* Data struct*/
volatile bool g_hs300x_completed = false;

/*******************************************************************************************************************//**
 * @brief       Quick sensor setup for HS3001
 * @param[in]
 * @retval
 * @retval
 ***********************************************************************************************************************/
void g_hs300x_sensor0_quick_setup(void)
{
    fsp_err_t err;
    /* Open HS300X sensor instance, this must be done before calling any HS300X API */
    err = g_hs300x_sensor0.p_api->open (g_hs300x_sensor0.p_ctrl, g_hs300x_sensor0.p_cfg);
    if (err != FSP_SUCCESS)
    {
        APP_DBG_PRINT("\r\nHS3001 sensor setup failed:%d\r\n", err);
        APP_ERR_TRAP(err);
    }
    else
    {
        APP_PRINT("\r\nHS3001 sensor setup success\r\n");
    }

}

/*******************************************************************************************************************//**
 * @brief       Read HS3001 sensor data
 * @param[in]
 * @retval
 * @retval
 ***********************************************************************************************************************/
void hs3001_get(void)
{
    rm_hs300x_raw_data_t TH_rawdata;
    rm_hs300x_data_t hs300x_data;
    UINT status = TX_SUCCESS;

    /* start measurement */
    g_hs300x_completed = false;
    tx_thread_sleep (pdMS_TO_TICKS(50));

    g_err = g_hs300x_sensor0.p_api->measurementStart (g_hs300x_sensor0.p_ctrl);
    assert(FSP_SUCCESS == g_err);
    WAIT_WHILE_FALSE(g_hs300x_completed);

    /* Keep attempting to read the data until it stabilises.
     * Section 6.6 of the HS3001 datasheet stipulates a range of
     * measurement times with corresponding to the range of
     * sensor resolutions. */
    do
    {
        /* read the data */
        g_hs300x_completed = false;
        tx_thread_sleep (pdMS_TO_TICKS(50));

        g_err = g_hs300x_sensor0.p_api->read (g_hs300x_sensor0.p_ctrl, &TH_rawdata);
        assert(FSP_SUCCESS == g_err);

        WAIT_WHILE_FALSE(g_hs300x_completed);

        /* Calculate results */
        g_err = g_hs300x_sensor0.p_api->dataCalculate (g_hs300x_sensor0.p_ctrl, &TH_rawdata, &hs300x_data);
        assert((FSP_SUCCESS == g_err) || (g_err == FSP_ERR_SENSOR_INVALID_DATA));
    }
    while (g_err == FSP_ERR_SENSOR_INVALID_DATA);

    /* Fill snesor data in structure */
    g_hs3001_data_s.gs_humidity = (float) hs300x_data.humidity.integer_part
            + (float) hs300x_data.humidity.decimal_part * 0.01F;
    g_hs3001_data_s.gs_temperature = (float) hs300x_data.temperature.integer_part
            + (float) hs300x_data.temperature.decimal_part * 0.01F;

    /* Send ICM sensor data to the ICM queue */
    status = send_data_to_queue(&g_hs3001_queue, &g_hs3001_data_s);
    assert(TX_SUCCESS == status);
    tx_thread_sleep (pdMS_TO_TICKS(40));
}

/*******************************************************************************************************************//**
 * @brief                  HS3001 sensor callback
 * @param[in]   p_args     HS3001 callback event
 * @retval
 * @retval
 ***********************************************************************************************************************/
void hs300x_callback(rm_hs300x_callback_args_t *p_args)
{
    if (RM_HS300X_EVENT_SUCCESS == p_args->event)
    {
        g_hs300x_completed = true;
    }
    else if (RM_HS300X_EVENT_ERROR == p_args->event)
    {
        APP_DBG_PRINT("\r\nRM_HS300X_EVENT_ERROR\r\n");
    }
    else
    {
        /* Do nothing */
    }
    FSP_PARAMETER_NOT_USED(p_args);
}

