/***********************************************************************************************************************
 * File Name    : RA_ZMOD4XXX_Common.c
 * Description  : Contains common data structures and function definitions for ZMOD4XX sensor
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

#include "hal_data.h"
#include "RA_ZMOD4XXX_Common.h"
#include "RA_HS3001.h"
#include "common_utils.h"
#include "ZMOD4510_Thread.h"

/* Interrupt status flag declaration */
volatile demo_callback_status_t g_i2c_callback_status;
volatile demo_callback_status_t g_irq_callback_status;
volatile demo_callback_status_t g_i2c_callback_status1;
volatile demo_callback_status_t g_irq_callback_status1;

/*******************************************************************************************************************//**
 * @brief       Quick snesor setup for ZMOD4410
 * @param[in]
 * @retval
 * @retval
 ***********************************************************************************************************************/
void g_zmod4xxx_sensor0_quick_setup(void)
{
    fsp_err_t err;

    /* Open ZMOD4XXX sensor instance, this must be done before calling any ZMOD4XXX API */
    err = g_zmod4xxx_sensor0.p_api->open (g_zmod4xxx_sensor0.p_ctrl, g_zmod4xxx_sensor0.p_cfg);
    if (err != FSP_SUCCESS)
    {
        APP_DBG_PRINT("\r\nZMOD4410 sensor setup failed:%d\r\n", err);
        APP_ERR_TRAP(err);
    }
    else
    {
        APP_PRINT("\r\nZMOD4410 sensor setup success\r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief       Quick snesor setup for ZMOD4510
 * @param[in]
 * @retval
 * @retval
 ***********************************************************************************************************************/
void g_zmod4xxx_sensor1_quick_setup(void)
{
    fsp_err_t err;

    /* Open ZMOD4XXX sensor instance, this must be done before calling any ZMOD4XXX API */
    err = g_zmod4xxx_sensor1.p_api->open (g_zmod4xxx_sensor1.p_ctrl, g_zmod4xxx_sensor1.p_cfg);
    if (err != FSP_SUCCESS)
    {
        APP_DBG_PRINT("\r\nZMOD4510 sensor setup failed:%d\r\n", err);
        APP_ERR_TRAP(err);
    }
    else
    {
        APP_PRINT("\r\nZMOD4510 sensor setup success\r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief                    I2C callback for ZMOD4410
 * @param[in]   p_args       Callback events
 * @retval
 * @retval
 ***********************************************************************************************************************/
void zmod4xxx_comms_i2c_callback(rm_zmod4xxx_callback_args_t *p_args)
{
    if (RM_ZMOD4XXX_EVENT_SUCCESS == p_args->event)
    {
        g_i2c_callback_status = DEMO_CALLBACK_STATUS_SUCCESS;
    }
    else if (RM_ZMOD4XXX_EVENT_MEASUREMENT_COMPLETE == p_args->event)
    {
        g_i2c_callback_status = DEMO_CALLBACK_STATUS_SUCCESS;
    }
    else
    {
        g_i2c_callback_status = DEMO_CALLBACK_STATUS_REPEAT;
    }
}

/*******************************************************************************************************************//**
 * @brief                    I2C callback for ZMOD4510
 * @param[in]   p_args       Callback events
 * @retval
 * @retval
 ***********************************************************************************************************************/
void zmod4xxx_comms_i2c1_callback(rm_zmod4xxx_callback_args_t *p_args)
{
    if (RM_ZMOD4XXX_EVENT_SUCCESS == p_args->event)
    {
        g_i2c_callback_status1 = DEMO_CALLBACK_STATUS_SUCCESS;
    }
    else if (RM_ZMOD4XXX_EVENT_MEASUREMENT_COMPLETE == p_args->event)
    {
        g_i2c_callback_status1 = DEMO_CALLBACK_STATUS_SUCCESS;
    }
    else
    {
        g_i2c_callback_status1 = DEMO_CALLBACK_STATUS_REPEAT;
    }
}

/*******************************************************************************************************************//**
 * @brief                    ZMOD4410 sensor callback
 * @param[in]   p_args       Callback events
 * @retval
 * @retval
 ***********************************************************************************************************************/
void zmod4xxx_irq0_callback(rm_zmod4xxx_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);

    g_irq_callback_status = DEMO_CALLBACK_STATUS_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief                    ZMOD4510 sensor callback
 * @param[in]   p_args       Callback events
 * @retval
 * @retval
 ***********************************************************************************************************************/
void zmod4xxx_irq1_callback(rm_zmod4xxx_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);

    g_irq_callback_status1 = DEMO_CALLBACK_STATUS_SUCCESS;
}
