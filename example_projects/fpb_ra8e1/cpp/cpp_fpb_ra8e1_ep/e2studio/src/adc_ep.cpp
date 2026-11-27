/***********************************************************************************************************************
 * File Name    : adc_ep.cpp
 * Description  : Contains variables and functions used in adc_ep.cpp.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "adc_ep.h"

/* Flag to check ADC scan is in progress or not */
volatile bool g_ready_to_read = false;

/*******************************************************************************************************************//**
 * @brief     This function initializes ADC module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t adc_ep::init_adc_module(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open the ADC module */
    err = ADC_OPEN(&g_adc_ctrl, &g_adc_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function closes the opened ADC module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void adc_ep::deinit_adc_module(void)
{
    /* Close the opened ADC module */
    if (g_adc_ctrl.opened)
    {
        ADC_CLOSE(&g_adc_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function starts the ADC scan.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Started successfully.
 * @retval    Any other error codes     Failed to start.
 **********************************************************************************************************************/
fsp_err_t adc_ep::adc_scan_start(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = init_adc_module();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Configure the ADC scan parameters */
    err = ADC_SCANCFG(&g_adc_ctrl, &ADC_CHANNEL_CONFIG);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

#if defined(BOARD_RA2A1_EK) || defined(BOARD_RA6T2_MCK)
    /* Set Reference Voltage Circuit Control register */
    R_ADC0->VREFAMPCNT |= ((VREFADCG_VALUE << SHIFT_BY_ONE) | (VREFADCG_ENABLE << SHIFT_BY_THREE));
#endif /* BOARD_RA2A1_EK || BOARD_RA6T2_MCK */
#if defined(BOARD_RA2A1_EK) || (BSP_PERIPHERAL_ADC_B_PRESENT)

    /* Calibrate the ADC */
    err = adc_start_calibration();
    if (FSP_SUCCESS != err)
    {
        return err;
    }
#endif /* BOARD_RA2A1_EK || BSP_PERIPHERAL_ADC_B_PRESENT */

    /* Start the ADC scan */
    err = ADC_SCANSTART(&g_adc_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

#if (BSP_PERIPHERAL_ADC_D_PRESENT)
    /* Setting internal temperature sensor as output, the initial conversion result cannot be used.
     * Scan second time if adc_d scan in One-shot mode */
    if (ADC_D_CONVERSION_MODE_SEQUENTIAL != g_adc_cfg_extend.conversion_operation)
    {
        /* Wait for conversion to complete */
        adc_status_t status;
        status.state = ADC_STATE_SCAN_IN_PROGRESS;
        while (ADC_STATE_SCAN_IN_PROGRESS == status.state)
        {
            ADC_STATUSGET(&g_adc_ctrl, &status);
            if (FSP_SUCCESS != err)
            {
                return err;
            }
        }

        /* Start the ADC scan */
        err = ADC_SCANSTART(&g_adc_ctrl);
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        /* Wait for conversion to complete */
        status.state = ADC_STATE_SCAN_IN_PROGRESS;
        while (ADC_STATE_SCAN_IN_PROGRESS == status.state)
        {
            ADC_STATUSGET(&g_adc_ctrl, &status);
            if (FSP_SUCCESS != err)
            {
                return err;
            }
        }
    }
#endif /* BSP_PERIPHERAL_ADC_D_PRESENT */

    /* Indicate to start reading the ADC data */
    g_ready_to_read = true;

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function stops the ADC scan and closes the ADC.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Stopped successfully.
 * @retval    Any other error codes     Failed to stop.
 **********************************************************************************************************************/
fsp_err_t adc_ep::adc_scan_stop(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = ADC_SCANSTOP(&g_adc_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Reset to indicate stop reading the ADC data */
    g_ready_to_read = false;

    /* Close the ADC module */
    deinit_adc_module();

    return err;
}

/*******************************************************************************************************************//**
 * @brief      This function reads ADC output data from the specified channel.
 * @param[out] p_data                   Pointer to store the ADC data read.
 * @retval     FSP_SUCCESS              Read successfully.
 * @retval     Any other error codes    Failed to read.
 **********************************************************************************************************************/
fsp_err_t adc_ep::adc_read_data(uint16_t * p_data)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Read the result */
    err = ADC_READ(&g_adc_ctrl, ADC_CHANNEL_TEMPERATURE, &g_adc_data);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* In ADC single scan mode (or one-shot mode for ADC_D) the scan stops automatically after one reading.
     * Reset the g_ready_to_read flag to avoid unnecessary reads. Close the ADC module, as it was opened during
     * the scan start command. */
    if (ADC_IS_SINGLE_SCAN())
    {
        g_ready_to_read = false;

        /* Close the ADC module */
        deinit_adc_module();
    }
    else
    {
#ifdef BOARD_RA2A1_EK
    /* Check for deviation in ADC values to initiate the calibration again */
    err = adc_deviation_in_output();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Update the current ADC data to previous */
    g_prev_adc_data = g_adc_data;
#endif /* BOARD_RA2A1_EK */
    }

    /* Store ADC result value */
    *p_data = g_adc_data;

    return err;
}

#ifdef BOARD_RA2A1_EK
/*******************************************************************************************************************//**
 * @brief     This function returns whether a re-calibration was triggered since the last call, clearing the flag.
 * @param[in] None.
 * @retval    true              A re-calibration was triggered since the last call.
 * @retval    false             No re-calibration was triggered since the last call.
 **********************************************************************************************************************/
bool adc_ep::recalibration_was_triggered(void)
{
    bool triggered = g_recalibration_triggered;
    g_recalibration_triggered = false;

    return triggered;
}
#endif /* BOARD_RA2A1_EK */

#if defined(BOARD_RA2A1_EK) || (BSP_PERIPHERAL_ADC_B_PRESENT)
/*******************************************************************************************************************//**
 * @brief     This function starts ADC calibration.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Started successfully.
 * @retval    Any other error codes     Failed to start.
 **********************************************************************************************************************/
fsp_err_t adc_ep::adc_start_calibration(void)
{
    fsp_err_t err = FSP_SUCCESS;
    adc_status_t adc_status = {ADC_STATE_IDLE};
    uint32_t time_out = UINT32_MAX;

    /* Initiate ADC calibration */
    err = ADC_CALIBRATE(&g_adc_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    do
    {
        /* To get the ADC status */
        err = ADC_STATUSGET(&g_adc_ctrl, &adc_status);
        if (FSP_SUCCESS != err)
        {
            return err;
        }
    } while ((ADC_STATE_IDLE != adc_status.state) && (RESET_VALUE < --time_out));

    /* Wait here till the calibration. It takes 24 milliseconds to 780 milliseconds based on clock */
    if (RESET_VALUE == time_out)
    {
        err = FSP_ERR_TIMEOUT;
    }

    return err;
}
#endif /* BOARD_RA2A1_EK || (BSP_PERIPHERAL_ADC_B_PRESENT) */

#ifdef BOARD_RA2A1_EK
/*******************************************************************************************************************//**
 * @brief     This function checks the ADC value deviation and triggers re-calibration if the limit is exceeded.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Checked and triggered successfully.
 * @retval    Any other error codes     Failed to check or trigger.
 **********************************************************************************************************************/
fsp_err_t adc_ep::adc_deviation_in_output(void)
{
    static uint8_t count = RESET_VALUE;
    fsp_err_t err = FSP_SUCCESS;

    if (RESET_VALUE != g_prev_adc_data)
    {
        if (((g_prev_adc_data > g_adc_data) && (g_prev_adc_data - g_adc_data > TEMPERATURE_DEVIATION_LIMIT)) ||\
            ((g_prev_adc_data < g_adc_data) && (g_adc_data - g_prev_adc_data > TEMPERATURE_DEVIATION_LIMIT)))
        {
            /* Increase deviation count */
            count++;
            if (TEMPERATURE_DEVIATION_LIMIT < count)
            {
                /* Reset the error count */
                count = RESET_VALUE;

                /* Notify the caller that a re-calibration was triggered */
                g_recalibration_triggered = true;

                /* Stop the scan to initiate calibration */
                err = adc_scan_stop();
                if (FSP_SUCCESS != err)
                {
                    return err;
                }

                /* Calibrate the ADC again */
                err = adc_start_calibration();
                if (FSP_SUCCESS != err)
                {
                    return err;
                }

                /* Clear the current reading */
                g_adc_data = RESET_VALUE;
            }
        }
        else
        {
            count = RESET_VALUE;
        }
    }

    return err;
}
#endif /* BOARD_RA2A1_EK */
