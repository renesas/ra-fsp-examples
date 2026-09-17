/***********************************************************************************************************************
 * File Name    : adc_app.c
 * Description  : This file contains the user functions to configure the ADC module.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2022 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "user_app_thread.h"
#include "adc_app.h"
#include "common_utils.h"

/*******************************************************************************************************************//**
 * @addtogroup aws_https_client_ep
 * @{
 **********************************************************************************************************************/

/* Function declaration */
static fsp_err_t adc_start_calibration(void);

/*******************************************************************************************************************//**
 * @brief      Initializes the ADC module by opening and start scanning of the internal MCU die temperature sensor
 *             channel.
 * @param[in]  None.
 * @retval     FSP_SUCCESS                  Upon successful ADC Initialization.
 * @retval     Any other error code         Upon unsuccessful ADC Initialization.
 **********************************************************************************************************************/
fsp_err_t hal_adc_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open ADC module */
#if (BSP_PERIPHERAL_ADC_PRESENT)
    err = R_ADC_Open(g_adc.p_ctrl, g_adc.p_cfg);
#elif (BSP_PERIPHERAL_ADC_B_PRESENT)
    err = R_ADC_B_Open(g_adc.p_ctrl, g_adc.p_cfg);
#endif
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("** Failed in R_"ADC_TYPE"_Open API **\r\n");
        return err;
    }

    /* Configure the ADC scan parameters */
#if (BSP_PERIPHERAL_ADC_PRESENT)
    err = R_ADC_ScanCfg(g_adc.p_ctrl, g_adc.p_channel_cfg);
#elif (BSP_PERIPHERAL_ADC_B_PRESENT)
    err = R_ADC_B_ScanCfg(g_adc.p_ctrl, g_adc.p_channel_cfg);
#endif
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* ADC failure message */
        APP_ERR_PRINT("** Failed in R_"ADC_TYPE"_ScanCfg API **\r\n");
        hal_adc_deinit();
        return err;
    }

    /* Calibrate the ADC */
    err = adc_start_calibration();
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* ADC failure message */
        APP_ERR_PRINT("** adc_start_calibration function failed **\r\n");
        return err;
    }

    /* Start the ADC scan */
#if (BSP_PERIPHERAL_ADC_PRESENT)
    err = R_ADC_ScanStart(g_adc.p_ctrl);
#elif (BSP_PERIPHERAL_ADC_B_PRESENT)
    err = R_ADC_B_ScanStart(g_adc.p_ctrl);
#endif
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* ADC failure message */
        APP_ERR_PRINT("** Failed in R_"ADC_TYPE"_ScanStart API **\r\n");
        hal_adc_deinit();
        return err;
    }
    APP_PRINT("\r\nADC module initialization successful");

    /* Disable interrupt */
    R_BSP_IrqDisable((IRQn_Type) ADC_EVENT_SCAN_COMPLETE);

    return err;
}

/*******************************************************************************************************************//**
 * @brief      Reads the internal MCU die temperature value.
 * @param[in]  None.
 * @retval     ADC Temperature value in Celsius.
 **********************************************************************************************************************/
float adc_data_read(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint16_t adc_data = RESET_VALUE;

    /* Read die temperature */
#if (BSP_PERIPHERAL_ADC_PRESENT)
    err = R_ADC_Read(g_adc.p_ctrl, ADC_CHANNEL_TEMPERATURE, &adc_data);
#elif (BSP_PERIPHERAL_ADC_B_PRESENT)
    err = R_ADC_B_Read(g_adc.p_ctrl, ADC_CHANNEL_TEMPERATURE, &adc_data);
#endif
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("** Failed in R_"ADC_TYPE"_Read API **\r\n");
        hal_adc_deinit();
        APP_ERR_TRAP(err);
    }

    /* Return the conversion of MCU die temperature in Celsius */
    return (ADCTEMP_AS_C(adc_data));
}

/*******************************************************************************************************************//**
* @brief      De-initializes the ADC module.
* @param[in]  None.
* @retval     FSP_SUCCESS                  Upon successful ADC Initialization.
* @retval     Any other error code         Upon unsuccessful ADC Initialization.
***********************************************************************************************************************/
void hal_adc_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

#if (BSP_PERIPHERAL_ADC_PRESENT)
    err = R_ADC_Close(g_adc.p_ctrl);
#elif (BSP_PERIPHERAL_ADC_B_PRESENT)
    err = R_ADC_B_Close(g_adc.p_ctrl);
#endif
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("** Failed in R_"ADC_TYPE"_Close API **\r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief       This function initiates the calibration.
 * @param[in]   None.
 * @retval      FSP_SUCCESS                 Upon successful ADC calibration.
 * @retval      Any other error code        Upon unsuccessful ADC calibration.
 **********************************************************************************************************************/
static fsp_err_t adc_start_calibration(void)
{
    fsp_err_t err = FSP_SUCCESS;     /* Error status */
    adc_status_t adc_status;         /* To get ADC status */

    /* Initiate ADC calibration */
#if (BSP_PERIPHERAL_ADC_PRESENT)
    err = R_ADC_Calibrate(&g_adc_ctrl, NULL);
#elif (BSP_PERIPHERAL_ADC_B_PRESENT)
    err = R_ADC_B_Calibrate(&g_adc_ctrl, NULL);
#endif

    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* ADC failure message */
        APP_ERR_PRINT("** R_"ADC_TYPE"_Calibrate API failed **\r\n");
        return err;
    }

    APP_PRINT("\r\nADC Calibration Started\r\n");

    do
    {
        /* To get the ADC status */
#if (BSP_PERIPHERAL_ADC_PRESENT)
        err = R_ADC_StatusGet(&g_adc_ctrl, &adc_status);
#elif (BSP_PERIPHERAL_ADC_B_PRESENT)
        err = R_ADC_B_StatusGet(&g_adc_ctrl, &adc_status);
#endif
        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            /* ADC status failure message */
            APP_ERR_PRINT("** R_"ADC_TYPE"_StatusGet API failed **\r\n");
            return err;
        }
    }
    /* Wait here till the calibration. It takes 24msec to 780msec based on clock */
    while (ADC_STATE_IDLE != adc_status.state);

    APP_PRINT("\r\nADC Calibration Successfully\r\n");

    return err;
}

/*******************************************************************************************************************//**
 * @} (end defgroup aws_https_client_ep)
 **********************************************************************************************************************/
