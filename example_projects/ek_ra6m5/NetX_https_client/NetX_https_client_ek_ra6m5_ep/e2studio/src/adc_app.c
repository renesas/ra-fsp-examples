/***********************************************************************************************************************
 * File Name    : adc_app.c
 * Description  : This file contains the User Application code for the ADC initialization and its functions.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "https_client_thread.h"
#include "adc_app.h"
#include "common_utils.h"
#include "https_client_app.h"

/*******************************************************************************************************************//**
 * @brief      Function to initialize ADC, configure the ADC scan parameters, and start the ADC scan.
 *
 * @param[in]  None
 * @retval     FSP_SUCCESS                          On successful ADC Initialization.
 * @retval     Any other return value               On unsuccessful ADC Initialization.
 **********************************************************************************************************************/
fsp_err_t hal_adc_init(void)
{
    /* To get return value of the APIs. */
    fsp_err_t err = FSP_SUCCESS;
    /* Open ADC module */
    err = R_ADC_Open(&g_adc0_ctrl, &g_adc0_cfg);
    if (FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("R_ADC_Open API failed.");
        return err;
    }

    /* Configures the ADC scan parameters */
    err = R_ADC_ScanCfg (&g_adc0_ctrl, &g_adc0_channel_cfg);
    if (FSP_SUCCESS != err)
    {
        /* ADC Failure message */
        PRINT_ERR_STR ("R_ADC_ScanCfg API failed.");
        hal_adc_deinit();
        return err;
    }

    /* Start the ADC scan*/
    err = R_ADC_ScanStart (&g_adc0_ctrl);
    if (FSP_SUCCESS != err)
    {
        /* ADC Failure message */
        PRINT_ERR_STR ("R_ADC_ScanStart API failed.");
        hal_adc_deinit();
        return err;
    }
    PRINT_INFO_STR("ADC module initialization successful.");
    return err;
}

/*******************************************************************************************************************//**
 * @brief      Reads the internal mcu die temperature value
 *
 * @param[in]  None
 * @retval     ADC Temperature value in celsius
 **********************************************************************************************************************/
float adc_data_read(void)
{
    /* To get return value of the APIs. */
    fsp_err_t err       = FSP_SUCCESS;
    /* To get the temperature data from ADC. */
    uint16_t adc_data   = RESET_VALUE;

    /* Read die temperature */
    err = R_ADC_Read (&g_adc0_ctrl, ADC_CHANNEL_TEMPERATURE, &adc_data);
    if (FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("R_ADC_Read API failed.");
        hal_adc_deinit();
        ERROR_TRAP(err);
    }
    /* Return the conversion of MCU die temperature in celsius */
    return (float)(ADCTEMP_AS_C(adc_data));
}

/*******************************************************************************************************************//**
 * @brief      Function to close ADC module
 *
 * @param[in]  None
 * @retval     None
 *************************************************************************************************************************/
void hal_adc_deinit(void)
{
    /* To get return value of the APIs. */
    fsp_err_t err = FSP_SUCCESS;
    /* Close the ADC Module. */
    err = R_ADC_Close(&g_adc0_ctrl);
    if(FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("R_ADC_Close API failed.");
    }
}
