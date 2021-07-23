/***********************************************************************************************************************
 * File Name    : adc_app.c
 * Description  : This file contains the User Application code for the ADC initialization and its functions.
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
