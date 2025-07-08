/***********************************************************************************************************************
 * File Name    : tsn_control.c
 * Description  : This source file implements functions declared in `tsn_control.h` to handle
 *                operation of ADC-TSN module.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "tsn_control.h"
#include "apn_cpu0.h"

/***********************************************************************************************************************
 * Private Function
 **********************************************************************************************************************/
static fsp_err_t adc_start_calibration(void);

/*******************************************************************************************************************//**
 * @brief     This Function for calibration ADC module
 *
 * @param[in] None.
 * @return    See @ref RENESAS_ERROR_CODES or functions called by this function for other possible
 *            return codes.
 **********************************************************************************************************************/
static fsp_err_t adc_start_calibration(void)
{
    /* Initialize variables */
    fsp_err_t err = FSP_SUCCESS;                     // Error status
    adc_status_t adc_status = {ADC_STATE_IDLE};      // to get adc status
    uint32_t time_out = UINT32_MAX;                  //max value of time_out

    /* Initiate ADC calibration */
    err = R_ADC_B_Calibrate (&g_adc0_ctrl, NULL);

    /* handle error */
    if (FSP_SUCCESS != err)
    {
        /* ADC Failure message */
        APP_ERR_PRINT ("\r\nR_ADC_B_Calibrate failed");
        return err;
    }
    do
    {
        /* To get the adc status*/
        err = R_ADC_B_StatusGet (&g_adc0_ctrl, &adc_status);
        /* handle error */
        if (FSP_SUCCESS != err)
        {
            /* ADC status Failure message */
            APP_ERR_PRINT ("\r\nR_ADC_B_StatusGet failed");
            return err;
        }
    }while ((ADC_STATE_IDLE != adc_status.state) && (RESET_VALUE < --time_out)); //wait here till the calibration.It takes 24msec to 780msec based on clock
    if (RESET_VALUE == time_out)
    {
        APP_ERR_PRINT("ADC Calibration is not successful due to time out");
        err = FSP_ERR_INVALID_HW_CONDITION;
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief     Initializes ADC module for scan temperature.
 *
 * @param[in] None.
 * @return    None.
 **********************************************************************************************************************/
void init_adc_tsn(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Init ADC-TSN */
    err = R_ADC_B_Open(&g_adc0_ctrl, &g_adc0_cfg);
    if (err!=FSP_SUCCESS)
    {
        APP_ERR_PRINT ("\r\nR_ADC_B_Open failed\r\nRestart the Application\r\n");
        APP_ERR_TRAP(err);
    }
    err = R_ADC_B_ScanCfg(&g_adc0_ctrl, &g_adc0_scan_cfg);
    if (err!=FSP_SUCCESS)
    {
        APP_ERR_PRINT ("\r\nR_ADC_B_ScanCfg failed.");
        APP_ERR_TRAP(err);
    }

    /* Calibrate the ADC */
    err = adc_start_calibration();

    /* handle error */
    if (FSP_SUCCESS != err)
    {
        /* ADC Failure message */
        APP_ERR_PRINT ("\r\nADC calibration process failed\r\nRestart the Application\r\n");
        APP_ERR_TRAP(err);
    }
}

/*******************************************************************************************************************//**
 * @brief     Start Scan Temperature
 *
 * @param[in] NONE.
 * @return    NONE.
 **********************************************************************************************************************/
void tsn_scan_start(void)
{
    fsp_err_t err = FSP_SUCCESS;
    err = R_ADC_B_ScanStart(&g_adc0_ctrl);
    if (FSP_SUCCESS != err)
    {
        /* ADC Failure message */
        APP_ERR_PRINT ("\r\nTSN ADC start failed\r\nRestart the Application\r\n");
        APP_ERR_TRAP(err);
    }
}

/*******************************************************************************************************************//**
 * @brief     Stop Scan Temperature
 *
 * @param[in] NONE.
 * @return    NONE
 **********************************************************************************************************************/
void tsn_scan_stop(void)
{
    fsp_err_t err = FSP_SUCCESS;
    err = R_ADC_B_ScanStop(&g_adc0_ctrl);
    if (FSP_SUCCESS != err)
    {
        /* ADC Failure message */
        APP_ERR_PRINT ("\r\nTSN ADC stop failed\r\nRestart the Application\r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief     Read the Temperature value to buffer
 *
 * @param[in] uint16_t * The buffer to store temperature value
 * @return    NONE
 **********************************************************************************************************************/
void tsn_read_data(uint16_t *tsn_buf)
{
    /* Checking for input parameter */
    APP_ASSERT(NULL !=  tsn_buf);

    fsp_err_t err = FSP_SUCCESS;     // Error status

    err = R_ADC_B_Read (&g_adc0_ctrl, ADC_CHANNEL_TEMPERATURE, tsn_buf);
    if (FSP_SUCCESS != err)
    {
        /* ADC Failure message */
        APP_ERR_PRINT("\r\nR_ADC_B_Read failed.\r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief     Close The ADC module
 *
 * @param[in] NONE
 * @return    NONE
 **********************************************************************************************************************/
void adc_tsn_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;
    err = R_ADC_B_Close(&g_adc0_ctrl);
    if (FSP_SUCCESS != err)
    {
        /* ADC Failure message */
        APP_ERR_PRINT("\r\nR_ADC_B_Close failed.\r\n");
        APP_ERR_TRAP(err);
    }
}
