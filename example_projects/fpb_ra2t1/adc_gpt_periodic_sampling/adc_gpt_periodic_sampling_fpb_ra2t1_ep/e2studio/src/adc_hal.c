/***********************************************************************************************************************
 * File Name    : adc_hal.c
 * Description  : Contains function definition.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "adc_periodic_scan.h"
#include "adc_hal.h"
#include "dtc_hal.h"

/*******************************************************************************************************************//**
 * @addtogroup adc_hal.c
 * @{
 **********************************************************************************************************************/

/* User buffer */
#if BSP_FEATURE_ADC_UNIT_1_CHANNELS
uint16_t g_buffer_adc[ADC_UNIT_NUM][ADC_GROUP_SCAN_NUM][NUM_ADC_CHANNELS][NUM_SAMPLE_BUFFER][NUM_SAMPLES_PER_CHANNEL];
#else
uint16_t g_buffer_adc[ADC_UNIT_1][ADC_GROUP_SCAN_NUM][NUM_ADC_CHANNELS][NUM_SAMPLE_BUFFER][NUM_SAMPLES_PER_CHANNEL];
#endif
/* Flags to capture ADC 0/1 events */
volatile bool g_adc0_group_a_flag = false ;
volatile bool g_adc0_group_b_flag = false;
#if BSP_FEATURE_ADC_UNIT_1_CHANNELS
volatile bool g_adc1_group_a_flag = false;
volatile bool g_adc1_group_b_flag = false;
#endif

/* Flag to capture error event */
volatile bool g_err_flag_adc0 = false;
#if BSP_FEATURE_ADC_UNIT_1_CHANNELS
volatile bool g_err_flag_adc1 = false;
#endif
/* Transfer info for ADC unit 0/1 and group A */
transfer_info_t g_transfer_adc_group_a[] =
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
  .p_dest = (void*) &g_buffer_adc[ADC_UNIT_0][ADC_GROUP_SCAN_A][ZERO][ZERO][ZERO],
  .p_src = (void const*) &R_ADC0->ADDR[ZERO],
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
  .p_dest = (void*) &g_buffer_adc[ADC_UNIT_0][ADC_GROUP_SCAN_A][ONE][ZERO][ZERO],
  .p_src = (void const*) &R_ADC0->ADDR[ONE],
  .num_blocks = ZERO,
  .length = NUM_SAMPLES_PER_CHANNEL,
 },

#if BSP_FEATURE_ADC_UNIT_1_CHANNELS
[TWO] =
{
 .transfer_settings_word_b.dest_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED,
 .transfer_settings_word_b.repeat_area = TRANSFER_REPEAT_AREA_DESTINATION,
 .transfer_settings_word_b.irq = TRANSFER_IRQ_END,
 .transfer_settings_word_b.chain_mode = TRANSFER_CHAIN_MODE_EACH,
 .transfer_settings_word_b.src_addr_mode = TRANSFER_ADDR_MODE_FIXED,
 .transfer_settings_word_b.size = TRANSFER_SIZE_2_BYTE,
 .transfer_settings_word_b.mode = TRANSFER_MODE_NORMAL,
 .p_dest = (void*) &g_buffer_adc[ADC_UNIT_1][ADC_GROUP_SCAN_A][ZERO][ZERO][ZERO],
 .p_src = (void const*) &R_ADC1->ADDR[ONE],
 .num_blocks = ZERO,
 .length = NUM_SAMPLES_PER_CHANNEL,
},

[THREE] =
{
 .transfer_settings_word_b.dest_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED,
 .transfer_settings_word_b.repeat_area = TRANSFER_REPEAT_AREA_DESTINATION,
 .transfer_settings_word_b.irq = TRANSFER_IRQ_END,
 .transfer_settings_word_b.chain_mode = TRANSFER_CHAIN_MODE_DISABLED,
 .transfer_settings_word_b.src_addr_mode = TRANSFER_ADDR_MODE_FIXED,
 .transfer_settings_word_b.size = TRANSFER_SIZE_2_BYTE,
 .transfer_settings_word_b.mode = TRANSFER_MODE_NORMAL,
 .p_dest = (void*) &g_buffer_adc[ADC_UNIT_1][ADC_GROUP_SCAN_A][ONE][ZERO][ZERO],
 .p_src = (void const*) &R_ADC1->ADDR[TWO],
 .num_blocks = ZERO,
 .length = NUM_SAMPLES_PER_CHANNEL,
},
#endif
};

/* Transfer info for ADC unit 0/1 and group B */
transfer_info_t g_transfer_adc_group_b[] =
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
  .p_dest = (void*) &g_buffer_adc[ADC_UNIT_0][ADC_GROUP_SCAN_B][ZERO][ZERO][ZERO],
  .p_src = (void const*) &R_ADC0->ADDR[EIGHT],
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
  .p_dest = (void*) &g_buffer_adc[ADC_UNIT_0][ADC_GROUP_SCAN_B][ONE][ZERO][ZERO],
  .p_src = (void const*) &R_ADC0->ADDR[NINE],
  .num_blocks = ZERO,
  .length = NUM_SAMPLES_PER_CHANNEL,
 },

#if BSP_FEATURE_ADC_UNIT_1_CHANNELS
[TWO] =
{
 .transfer_settings_word_b.dest_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED,
 .transfer_settings_word_b.repeat_area = TRANSFER_REPEAT_AREA_DESTINATION,
 .transfer_settings_word_b.irq = TRANSFER_IRQ_END,
 .transfer_settings_word_b.chain_mode = TRANSFER_CHAIN_MODE_EACH,
 .transfer_settings_word_b.src_addr_mode = TRANSFER_ADDR_MODE_FIXED,
 .transfer_settings_word_b.size = TRANSFER_SIZE_2_BYTE,
 .transfer_settings_word_b.mode = TRANSFER_MODE_NORMAL,
 .p_dest = (void*) &g_buffer_adc[ADC_UNIT_1][ADC_GROUP_SCAN_B][ZERO][ZERO][ZERO],
 .p_src = (void const*) &R_ADC1->ADDR[FOUR],
 .num_blocks = ZERO,
 .length = NUM_SAMPLES_PER_CHANNEL,
},

[THREE] =
{
 .transfer_settings_word_b.dest_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED,
 .transfer_settings_word_b.repeat_area = TRANSFER_REPEAT_AREA_DESTINATION,
 .transfer_settings_word_b.irq = TRANSFER_IRQ_END,
 .transfer_settings_word_b.chain_mode = TRANSFER_CHAIN_MODE_DISABLED,
 .transfer_settings_word_b.src_addr_mode = TRANSFER_ADDR_MODE_FIXED,
 .transfer_settings_word_b.size = TRANSFER_SIZE_2_BYTE,
 .transfer_settings_word_b.mode = TRANSFER_MODE_NORMAL,
 .p_dest = (void*) &g_buffer_adc[ADC_UNIT_1][ADC_GROUP_SCAN_B][ONE][ZERO][ZERO],
 .p_src = (void const*) &R_ADC1->ADDR[FIVE],
 .num_blocks = ZERO,
 .length = NUM_SAMPLES_PER_CHANNEL,
},
#endif
};

/*******************************************************************************************************************//**
 * @brief       Initializes ADC module instances.
 * @param[IN]   p_ctrl_adc         ADC instance control structure.
 * @param[IN]   p_cfg_adc          ADC instance configure structure.
 * @retval      FSP_SUCCESS        ADC instance opened successfully.
 * @retval      Any other error code apart from FSP_SUCCESS on unsuccessful opening of ADC instance.
 **********************************************************************************************************************/
fsp_err_t init_hal_adc(adc_ctrl_t * p_ctrl_adc, adc_cfg_t const * const p_cfg_adc)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Open ADC module */
    err = R_ADC_Open(p_ctrl_adc, p_cfg_adc);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_ADC_Open FAILED ** \r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       Configures the ADC channel specific settings.
 * @param[IN]   p_ctrl_adc                 ADC instance control structure.
 * @param[IN]   p_channel_cfg_adc          ADC instance configure channel structure.
 * @retval      FSP_SUCCESS                On successful ADC channel configuration setting.
 * @retval      Any other error code apart from FSP_SUCCESS on Unsuccessful channel configuring of ADC instance.
 **********************************************************************************************************************/
fsp_err_t adc_channel_config(adc_ctrl_t * p_ctrl_adc, void const * const p_channel_cfg_adc)
{
    fsp_err_t err = FSP_SUCCESS;
#ifdef BOARD_RA2A1_EK

    /* Calibrate the ADC */
    err = adc_start_calibration();
    if (FSP_SUCCESS != err)
    {
        /* ADC Failure message */
        APP_ERR_PRINT("** adc_start_calibration failed ** \r\n");
        return err;
    }
#endif
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
 * @param[IN]   p_ctrl_adc      ADC control instance.
 * @retval      FSP_SUCCESS     Upon successful close operation.
 * @retval      Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
void deinit_hal_adc(adc_ctrl_t * p_ctrl_adc)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Close opened ADC instances */
    err = R_ADC_Close(p_ctrl_adc);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_ADC_Close FAILED ** \r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief       This functions start ADC scan operation.
 * @param[IN]   p_ctrl_adc      ADC control instance.
 * @retval      FSP_SUCCESS     Upon successful scan start operation.
 * @retval      Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
fsp_err_t scan_start_adc(adc_ctrl_t * p_ctrl_adc)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Scan start ADC, API will return success but will start scanning once trigger event are received */
    err = R_ADC_ScanStart(p_ctrl_adc);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_ADC_ScanStart FAILED ** \r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 *  @brief        User defined callback function for ADC unit 0
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
            /* Set the flag for ADC 0 group A, when ADC_EVENT_SCAN_COMPLETE_GROUP_A occurred */
            g_adc0_group_a_flag = true;
        }
        break;

        case ADC_EVENT_SCAN_COMPLETE_GROUP_B:
        {
            /* Set the flag for ADC 0 group B, when ADC_EVENT_SCAN_COMPLETE_GROUP_B occurred */
            g_adc0_group_b_flag = true;
        }
        break;
#ifdef BOARD_RA2A1_EK
        case ADC_EVENT_CALIBRATION_COMPLETE:
        {
            /* Do nothing */
        }
        break;
#endif
        default:
        {
            /* Set the err flag for ADC 0, when received event is apart from group A/B scan complete event */
            g_err_flag_adc0 = true;
        }
        break;
    }
}

#if BSP_FEATURE_ADC_UNIT_1_CHANNELS
/*******************************************************************************************************************//**
 *  @brief        User defined callback function for ADC unit 1.
 *  @param[IN]    p_args
 *  @retval       None
 **********************************************************************************************************************/
void g_adc1_callback(adc_callback_args_t *p_args)
{
    adc_event_t event = p_args->event;
    switch(event)
    {
        case ADC_EVENT_SCAN_COMPLETE:
        {
            /* Set the flag for ADC 1 group A, when ADC_EVENT_SCAN_COMPLETE_GROUP_A occurred */
            g_adc1_group_a_flag = true;
        }
        break;

        case ADC_EVENT_SCAN_COMPLETE_GROUP_B:
        {
            /* Set the flag for ADC 1 group B, when ADC_EVENT_SCAN_COMPLETE_GROUP_B occurred */
            g_adc1_group_b_flag = true;
        }
        break;

        default:
        {
            /* Set the error flag for ADC 1, when received event is apart from group A/B scan complete event */
            g_err_flag_adc1 = true;
        }
        break;
    }
}
#endif

#ifdef BOARD_RA2A1_EK
/*******************************************************************************************************************//**
 * @brief       This function initiates the calibration.
 * @param[IN]   None
 * @retval      FSP_SUCCESS     Upon successful of ADC calibration.
 * @retval      Any other error code apart from FSP_SUCCESS on unsuccessful calibration failure.
 **********************************************************************************************************************/
fsp_err_t adc_start_calibration(void)
{
    fsp_err_t err = FSP_SUCCESS;
    adc_status_t adc_status;         /* To get ADC status */

    /* Initiate ADC calibration */
    err = R_ADC_Calibrate (&g_adc0_ctrl, NULL);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* ADC failure message */
        APP_ERR_PRINT("** R_ADC_Calibrate API failed ** \r\n");
        return err;
    }

    do
    {
        /* To get the ADC status */
        err = R_ADC_StatusGet (&g_adc0_ctrl, &adc_status);
        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            /* ADC status failure message */
            APP_ERR_PRINT("** R_ADC_StatusGet API failed ** \r\n");
            return err;
        }
    } while (ADC_STATE_IDLE != adc_status.state); /* Wait here till the calibration.
                                                   * It takes 24msec to 780msec based on clock */

    return err;
}
#endif

/*******************************************************************************************************************//**
 * @} (end addtogroup adc_hal.c)
 **********************************************************************************************************************/
