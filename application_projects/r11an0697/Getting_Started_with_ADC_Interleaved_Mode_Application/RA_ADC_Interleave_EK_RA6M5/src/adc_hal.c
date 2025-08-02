/***********************************************************************************************************************
 * File Name    : adc_hal.c
 * Description  : Contains function definition.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2022 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "adc_periodic_scan.h"
#include "adc_hal.h"
#include <dmac_hal.h>

/*******************************************************************************************************************//**
 * @addtogroup adc_hal.c
 * @{
 **********************************************************************************************************************/
/* User buffer */
uint16_t g_buffer_adc[NUM_SAMPLES] = {0};
/* flag to capture error event*/
volatile bool g_err_flag_adc0 = false;
volatile bool g_err_flag_adc1 = false;

/*****************************************************************************************************************
 * @brief       Initializes ADC module instances.
 * @param[IN]   p_ctrl_adc         ADC instance control structure.
 * @param[IN]   p_cfg_adc          ADC instance config  structure.
 * @retval      FSP_SUCCESS        ADC instance opened successfully.
 * @retval      Any Other Error code apart from FSP_SUCCES on Unsuccessful opening of adc instance .
 ****************************************************************************************************************/
fsp_err_t init_hal_adc(adc_ctrl_t * p_ctrl_adc, adc_cfg_t const * const p_cfg_adc)
{
    fsp_err_t err = FSP_SUCCESS;
    /*open adc module */
    err = R_ADC_Open(p_ctrl_adc, p_cfg_adc);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_ADC_Open FAILED ** \r\n");
    }
    return err;
}


/*****************************************************************************************************************
 * @brief       Configures the ADC channel specific settings.
 * @param[IN]   p_ctrl_adc                 ADC instance control structure.
 * @param[IN]   p_channel_cfg_adc          ADC instance  configured channel structure.
 * @retval      FSP_SUCCESS                On successful adc channel configuration setting..
 * @retval      Any Other Error code apart from FSP_SUCCES on Unsuccessful channel configuring of adc instance.
 ****************************************************************************************************************/
fsp_err_t adc_channel_config(adc_ctrl_t * p_ctrl_adc, void const * const p_channel_cfg_adc)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Configure ADC channel specific settings */
    err = R_ADC_ScanCfg(p_ctrl_adc, p_channel_cfg_adc);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_ADC_ScanCfg FAILED ** \r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This functions closes ADC instance.
 * @param[IN]   p_ctrl_adc                  ADC control instance
 * @retval      FSP_SUCCESS                 Upon successful close operation.
 * @retval      Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
void deinit_hal_adc(adc_ctrl_t * p_ctrl_adc)
{
    fsp_err_t err = FSP_SUCCESS;
    /*Close opened ADC instances */
    err = R_ADC_Close(p_ctrl_adc);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_ADC_Close FAILED ** \r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief       This functions start adc scan operation.
 * @param[IN]   p_ctrl_adc                  ADC control instance
 * @retval      FSP_SUCCESS                 Upon successful scan start operation.
 * @retval      Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t scan_start_adc(adc_ctrl_t * p_ctrl_adc)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Scan start adc, API will return success but will start scanning once trigger event are received. */
    err = R_ADC_ScanStart(p_ctrl_adc);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_ADC_Close FAILED ** \r\n");
    }
    return err;
}
/*******************************************************************************************************************//**
 * @} (end addtogroup adc_hal.c)
 **********************************************************************************************************************/
