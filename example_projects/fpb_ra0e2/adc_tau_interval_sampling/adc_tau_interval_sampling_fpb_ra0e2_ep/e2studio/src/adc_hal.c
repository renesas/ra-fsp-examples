/***********************************************************************************************************************
 * File Name    : adc_hal.c
 * Description  : Contains function definition.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "adc_interval_scan.h"
#include "common_utils.h"
#include "adc_hal.h"
#include "dtc_hal.h"

/*******************************************************************************************************************//**
 * @addtogroup adc_hal.c
 * @{
 **********************************************************************************************************************/

/* User buffer */
uint16_t g_buffer_adc[ADC_UNIT_1][ADC_GROUP_SCAN_NUM][NUM_ADC_CHANNELS][NUM_SAMPLE_BUFFER][NUM_SAMPLES_PER_CHANNEL];
/* Flag to capture ADC group events */
volatile bool g_adc0_group_flag = false ;

/* Flag to capture error event */
volatile bool g_err_flag_adc0 = false;
/* Transfer info for ADC group */
transfer_info_t g_transfer_adc_group[] =
{
 [ZERO] =
 {
  .transfer_settings_word_b.dest_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED,
  .transfer_settings_word_b.repeat_area = TRANSFER_REPEAT_AREA_DESTINATION,
  .transfer_settings_word_b.irq = TRANSFER_IRQ_END,
  .transfer_settings_word_b.chain_mode = TRANSFER_CHAIN_MODE_EACH,
  .transfer_settings_word_b.src_addr_mode = TRANSFER_ADDR_MODE_FIXED,
  .transfer_settings_word_b.size = TRANSFER_SIZE_2_BYTE,
  .transfer_settings_word_b.mode = TRANSFER_MODE_NORMAL,
  .p_dest = (void*) &g_buffer_adc[ADC_UNIT_0][ADC_GROUP_SCAN][ZERO][ZERO][ZERO],
  .p_src = (void const*) &R_ADC_D->ADCR[0],
  .num_blocks = ZERO,
  .length = NUM_SAMPLES_PER_CHANNEL,
 },

 [ONE] =
 {
  .transfer_settings_word_b.dest_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED,
  .transfer_settings_word_b.repeat_area = TRANSFER_REPEAT_AREA_DESTINATION,
  .transfer_settings_word_b.irq = TRANSFER_IRQ_END,
  .transfer_settings_word_b.chain_mode = TRANSFER_CHAIN_MODE_DISABLED,
  .transfer_settings_word_b.src_addr_mode = TRANSFER_ADDR_MODE_FIXED,
  .transfer_settings_word_b.size = TRANSFER_SIZE_2_BYTE,
  .transfer_settings_word_b.mode = TRANSFER_MODE_NORMAL,
  .p_dest = (void*) &g_buffer_adc[ADC_UNIT_0][ADC_GROUP_SCAN][ONE][ZERO][ZERO],
  .p_src = (void const*) &R_ADC_D->ADCR[1],
  .num_blocks = ZERO,
  .length = NUM_SAMPLES_PER_CHANNEL,
 },

};

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
    /* Open ADC module */
    err = R_ADC_D_Open(p_ctrl_adc, p_cfg_adc);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_ADC_D_Open FAILED ** \r\n");
    }
    return err;
}

/*****************************************************************************************************************
 * @brief       Configures the ADC channel specific settings.
 * @param[IN]   p_ctrl_adc                 ADC instance control structure.
 * @param[IN]   p_channel_cfg_adc          ADC instance  configured channel structure.
 * @retval      FSP_SUCCESS                On successful ADC channel configuration setting..
 * @retval      Any Other Error code apart from FSP_SUCCES on Unsuccessful channel configuring of ADC instance.
 ****************************************************************************************************************/
fsp_err_t adc_channel_config(adc_ctrl_t * p_ctrl_adc, void const * const p_channel_cfg_adc)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Configure ADC channel specific settings */
    err = R_ADC_D_ScanCfg(p_ctrl_adc, p_channel_cfg_adc);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_ADC_D_ScanCfg FAILED ** \r\n");
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
    /* Close opened ADC instances */
    err = R_ADC_D_Close(p_ctrl_adc);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_ADC_D_Close FAILED ** \r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief       This functions start ADC scan operation.
 * @param[IN]   p_ctrl_adc                  ADC control instance
 * @retval      FSP_SUCCESS                 Upon successful scan start operation.
 * @retval      Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t scan_start_adc(adc_ctrl_t * p_ctrl_adc)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Scan start ADC, API will return success but will start scanning once trigger event are received. */
    err = R_ADC_D_ScanStart(p_ctrl_adc);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_ADC_D_ScanStart FAILED ** \r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 *  @brief        User defined callback function for ADC
 *  @param[IN]    p_args
 *  @retval       None
 **********************************************************************************************************************/
void g_adc0_callback(adc_callback_args_t *p_args)
{
    adc_event_t event = p_args->event;
    switch(event)
    {
        case ADC_EVENT_SCAN_COMPLETE:
        {
            /* Set the flag for ADC 0 group, when ADC_EVENT_SCAN_COMPLETE_GROUP occurred */
            g_adc0_group_flag = true;
        }
        break;

        default:
        {
            /* Set the err flag for ADC 0, when received event is apart from group scan complete event */
            g_err_flag_adc0 = true;
        }
        break;
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup adc_hal.c)
 **********************************************************************************************************************/
