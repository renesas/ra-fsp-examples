/***********************************************************************************************************************
 * File Name    : adc_ep.c
 * Description  : Contains data structures and functions used in adc_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "https_client_thread.h"
#include "terminal.h"
#include "adc_ep.h"

/* Private function */
#if (BSP_PERIPHERAL_ADC_B_PRESENT)
static fsp_err_t adc_start_calibration(void);
#endif /* BSP_PERIPHERAL_ADC_B_PRESENT */

/*******************************************************************************************************************//**
 * @brief     This function initializes ADC, configures the ADC scan parameters, and starts the ADC scan.
 * @param[in] None.
 * @retval    FSP_SUCCESS              Initialized successfully.
 * @retval    Any other error codes    Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t adc_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open ADC module */
    err = ADC_OPEN(&g_adc0_ctrl, &g_adc0_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Configure the ADC scan parameters */
    err = ADC_SCANCFG(&g_adc0_ctrl, &ADC_CHANNEL_CONFIG);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

#if (BSP_PERIPHERAL_ADC_B_PRESENT)
    err = adc_start_calibration();
    if (FSP_SUCCESS != err)
    {
        return err;
    }
#endif /* BSP_PERIPHERAL_ADC_B_PRESENT */

    /* Start the ADC scan */
    err = ADC_SCANSTART(&g_adc0_ctrl);

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
    err = ADC_READ(&g_adc0_ctrl, ADC_CHANNEL_TEMPERATURE, &adc_data);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Return the conversion of MCU die temperature in degrees Celsius */
    *p_temp_c = (float)(ADCTEMP_AS_C(adc_data));

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function closes the ADC module if it is opened.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void adc_deinit(void)
{
    /* Close the opened ADC module */
    if (g_adc0_ctrl.opened)
    {
        ADC_CLOSE(&g_adc0_ctrl);
    }
}

#if (BSP_PERIPHERAL_ADC_B_PRESENT)
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

    err = R_ADC_B_Calibrate(&g_adc0_ctrl, NULL);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Wait for calibration to complete */
    while (ADC_STATE_IDLE != status.state)
    {
        tx_thread_sleep(1);
        err = R_ADC_B_StatusGet(&g_adc0_ctrl, &status);
        if (FSP_SUCCESS != err)
        {
            return err;
        }
    }

    return err;
}
#endif /* BSP_PERIPHERAL_ADC_B_PRESENT */
