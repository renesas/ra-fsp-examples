/***********************************************************************************************************************
 * File Name    : adc_app.c
 * Description  : This file contains the User Application code for the ADC initialization and its functions.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "https_client_thread.h"
#include "adc_app.h"
#include "common_utils.h"
#include "https_client_app.h"

/*******************************************************************************************************************//**
 * @brief      This function initializes ADC, configures the ADC scan parameters, and starts the ADC scan.
 * @param[in]  None.
 * @retval     FSP_SUCCESS              On successful ADC Initialization.
 * @retval     Any other return value   On unsuccessful ADC Initialization.
 **********************************************************************************************************************/
fsp_err_t hal_adc_init(void)
{
    /* To get return value of the APIs */
    fsp_err_t err = FSP_SUCCESS;

    /* Open ADC module */
#if (BSP_PERIPHERAL_ADC_B_PRESENT)
    err = R_ADC_B_Open(&g_adc0_ctrl, &g_adc0_cfg);
#else
    err = R_ADC_Open(&g_adc0_ctrl, &g_adc0_cfg);
#endif /* BSP_PERIPHERAL_ADC_B_PRESENT */
    if (FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("R_"ADC_TYPE"_Open API failed.");
        return err;
    }

    /* Configure the ADC scan parameters */
#if (BSP_PERIPHERAL_ADC_B_PRESENT)
    err = R_ADC_B_ScanCfg(&g_adc0_ctrl, &g_adc0_scan_cfg);
#else
    err = R_ADC_ScanCfg(&g_adc0_ctrl, &g_adc0_channel_cfg);
#endif /* BSP_PERIPHERAL_ADC_B_PRESENT */
    if (FSP_SUCCESS != err)
    {
        /* ADC failure message */
        PRINT_ERR_STR("R_"ADC_TYPE"_ScanCfg API failed.");
        hal_adc_deinit();
        return err;
    }

#if (BSP_PERIPHERAL_ADC_B_PRESENT)
    err = R_ADC_B_Calibrate(&g_adc0_ctrl, NULL);
    if (FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("R_"ADC_TYPE"_Calibrate API failed.");
        return err;
    }

    /* Wait for calibration to complete */
    adc_status_t status = {.state = ADC_STATE_CALIBRATION_IN_PROGRESS};
    while ((ADC_STATE_IDLE != status.state) && (FSP_SUCCESS == err))
    {
        tx_thread_sleep(1);
        err = R_ADC_B_StatusGet(&g_adc0_ctrl, &status);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("** R_"ADC_TYPE"_StatusGet API failed **\r\n");
            return err;
        }
    }
#endif /* BSP_PERIPHERAL_ADC_B_PRESENT */

    /* Start the ADC scan */
#if (BSP_PERIPHERAL_ADC_B_PRESENT)
    err = R_ADC_B_ScanStart(&g_adc0_ctrl);
#else
    err = R_ADC_ScanStart(&g_adc0_ctrl);
#endif /* BSP_PERIPHERAL_ADC_B_PRESENT */
    if (FSP_SUCCESS != err)
    {
        /* ADC failure message */
        PRINT_ERR_STR("R_"ADC_TYPE"_ScanStart API failed.");
        hal_adc_deinit();
        return err;
    }
    PRINT_INFO_STR("ADC module initialization successful.");
    return err;
}

/*******************************************************************************************************************//**
 * @brief      This function reads the internal MCU die temperature value.
 * @param[in]  None.
 * @retval     ADC temperature value in degrees Celsius.
 **********************************************************************************************************************/
float adc_data_read(void)
{
    /* To get return value of the APIs */
    fsp_err_t err = FSP_SUCCESS;

    /* To get the temperature data from ADC */
    uint16_t adc_data = RESET_VALUE;

    /* Read die temperature */
#if (BSP_PERIPHERAL_ADC_B_PRESENT)
    err = R_ADC_B_Read(&g_adc0_ctrl, ADC_CHANNEL_TEMPERATURE, &adc_data);
#else
    err = R_ADC_Read(&g_adc0_ctrl, ADC_CHANNEL_TEMPERATURE, &adc_data);
#endif /* BSP_PERIPHERAL_ADC_B_PRESENT */
    if (FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("R_"ADC_TYPE"_Read API failed.");
        hal_adc_deinit();
        ERROR_TRAP(err);
    }

    /* Return the conversion of MCU die temperature in degrees Celsius */
    return (float)(ADCTEMP_AS_C(adc_data));
}

/*******************************************************************************************************************//**
 * @brief      This function closes ADC module.
 * @param[in]  None.
 * @retval     None.
 **********************************************************************************************************************/
void hal_adc_deinit(void)
{
    /* To get return value of the APIs */
    fsp_err_t err = FSP_SUCCESS;

    /* Close the ADC module */
#if (BSP_PERIPHERAL_ADC_B_PRESENT)
    err = R_ADC_B_Close(&g_adc0_ctrl);
#else
    err = R_ADC_Close(&g_adc0_ctrl);
#endif /* BSP_PERIPHERAL_ADC_B_PRESENT */
    if (FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("R_"ADC_TYPE"_Close API failed.");
    }
}
