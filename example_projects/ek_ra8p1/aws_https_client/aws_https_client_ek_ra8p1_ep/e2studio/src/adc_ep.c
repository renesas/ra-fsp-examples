/***********************************************************************************************************************
 * File Name    : adc_ep.c
 * Description  : Contains data structures and functions used in adc_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2022 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "user_app_thread.h"
#include "adc_ep.h"

/* Private function */
static fsp_err_t adc_start_calibration(void);

/*******************************************************************************************************************//**
 * @brief     This function initializes the ADC module.
 * @param[in] None.
 * @retval    FSP_SUCCESS              Initialized successfully.
 * @retval    Any other error codes    Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t adc_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open ADC module */
    err = ADC_OPEN(g_adc.p_ctrl, g_adc.p_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Configure the ADC scan parameters */
    err = ADC_SCANCFG(g_adc.p_ctrl, g_adc.p_channel_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Calibrate the ADC */
    err = adc_start_calibration();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Start the ADC scan */
    err = ADC_SCANSTART(g_adc.p_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Disable interrupt */
    ADC_IRQ_DISABLE();

    return err;
}

/*******************************************************************************************************************//**
 * @brief      This function reads the internal MCU die temperature value.
 * @param[out] p_temp_c                 Pointer to store the ADC temperature value in degrees Celsius.
 * @retval     FSP_SUCCESS              Read successfully.
 * @retval     Any other error codes    Failed to read.
 **********************************************************************************************************************/
fsp_err_t adc_data_read(float * const p_temp_c)
{
    fsp_err_t err = FSP_SUCCESS;

    /* To get the temperature data from ADC */
    uint16_t adc_data = RESET_VALUE;

    /* Read die temperature */
    err = ADC_READ(g_adc.p_ctrl, ADC_CHANNEL_TEMPERATURE, &adc_data);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Return the conversion of MCU die temperature in degrees Celsius */
    *p_temp_c = (float)(ADCTEMP_AS_C(adc_data));

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function closes the opened ADC module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void adc_deinit(void)
{
    /* Close the opened ADC module */
    if (g_adc_ctrl.opened)
    {
        ADC_CLOSE(g_adc.p_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function starts ADC calibration and waits for it to complete.
 * @param[in] None.
 * @retval    FSP_SUCCESS              Calibrated successfully.
 * @retval    Any other error codes    Failed to calibrate.
 **********************************************************************************************************************/
static fsp_err_t adc_start_calibration(void)
{
    fsp_err_t err = FSP_SUCCESS;
    adc_status_t status = {.state = ADC_STATE_CALIBRATION_IN_PROGRESS};

    /* Initiate ADC calibration */
    err = ADC_CALIBRATE(g_adc.p_ctrl, NULL);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Wait for calibration to complete. It takes 24ms to 780ms on clock */
    while (ADC_STATE_IDLE != status.state)
    {
        vTaskDelay(1);
        err = ADC_STATUSGET(g_adc.p_ctrl, &status);
        if (FSP_SUCCESS != err)
        {
            return err;
        }
    }

    return err;
}
