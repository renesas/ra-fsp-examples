/***********************************************************************************************************************
 * File Name    : temp_sensor.c
 * Description  : Contains data structures and functions used in temp_sensor.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2025 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "temp_sensor.h"

/***********************************************************************************************************************
 * Function Name: adc_init.
 * Description  : This function opens the ADC, configures and starts the scan operation.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Opened, configured, and started successfully.
 *                Any other error codes     Failed to open, configure, or start.
 **********************************************************************************************************************/
fsp_err_t adc_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open ADC module */
    err = R_ADC_D_Open(&g_adc_ctrl, &g_adc_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Configure the ADC scan parameters */
    err = R_ADC_D_ScanCfg(&g_adc_ctrl, &g_adc_channel_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Start the ADC scan */
    err = R_ADC_D_ScanStart(&g_adc_ctrl);

    return err;
}
/***********************************************************************************************************************
* End of function adc_init
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: adc_read_data.
 * Description  : This function reads the ADC output data from the prescribed channel.
 * Arguments    : adc_data                  Pointer to store the read ADC data.
 * Return Value : FSP_SUCCESS               Read successfully.
 *                Any other error codes     Failed to read.
 **********************************************************************************************************************/
fsp_err_t adc_read_data(uint16_t * adc_data)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Read the MCU temperature from ADC */
    err = R_ADC_D_Read(&g_adc_ctrl, ADC_CHANNEL_TEMPERATURE, adc_data);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Wait for conversion time */
    R_BSP_SoftwareDelay(1U, BSP_DELAY_UNITS_SECONDS);

    return err;
}
/***********************************************************************************************************************
* End of function adc_read_data
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: adc_deinit.
 * Description  : This function closes the opened ADC module.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
void adc_deinit(void)
{
    /* Close the opened ADC module */
    if (g_adc_ctrl.opened)
    {
        R_ADC_D_Close(&g_adc_ctrl);
    }
}
/***********************************************************************************************************************
* End of function adc_deinit
***********************************************************************************************************************/
