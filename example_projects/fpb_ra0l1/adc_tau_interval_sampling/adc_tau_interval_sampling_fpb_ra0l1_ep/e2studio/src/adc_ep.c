/***********************************************************************************************************************
 * File Name    : adc_ep.c
 * Description  : Contains data structures and functions used in adc_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "adc_ep.h"

/* User buffer */
uint16_t g_buffer_adc[ADC_UNIT_1][ADC_GROUP_SCAN_NUM][NUM_ADC_CHANNELS][NUM_SAMPLE_BUFFER][NUM_SAMPLES_PER_CHANNEL];

/* Flag to capture ADC group events */
volatile bool g_adc0_group_flag = false;

/* Flag to capture error event */
volatile bool g_err_flag_adc0 = false;

/* Transfer info for ADC group */
transfer_info_t g_transfer_adc_group[] =
{
    [0U] =
    {
        .transfer_settings_word_b.dest_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED,
        .transfer_settings_word_b.repeat_area = TRANSFER_REPEAT_AREA_DESTINATION,
        .transfer_settings_word_b.irq = TRANSFER_IRQ_END,
        .transfer_settings_word_b.chain_mode = TRANSFER_CHAIN_MODE_EACH,
        .transfer_settings_word_b.src_addr_mode = TRANSFER_ADDR_MODE_FIXED,
        .transfer_settings_word_b.size = TRANSFER_SIZE_2_BYTE,
        .transfer_settings_word_b.mode = TRANSFER_MODE_NORMAL,
        .p_dest = (void *) &g_buffer_adc[ADC_UNIT_0][ADC_GROUP_SCAN][0U][0U][0U],
        .p_src = (void const *) &R_ADC_D->ADCR[0U], /* Select ADC channel 0 */
        .num_blocks = 0U,
        .length = NUM_SAMPLES_PER_CHANNEL,
    },

    [1U] =
    {
        .transfer_settings_word_b.dest_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED,
        .transfer_settings_word_b.repeat_area = TRANSFER_REPEAT_AREA_DESTINATION,
        .transfer_settings_word_b.irq = TRANSFER_IRQ_END,
        .transfer_settings_word_b.chain_mode = TRANSFER_CHAIN_MODE_DISABLED,
        .transfer_settings_word_b.src_addr_mode = TRANSFER_ADDR_MODE_FIXED,
        .transfer_settings_word_b.size = TRANSFER_SIZE_2_BYTE,
        .transfer_settings_word_b.mode = TRANSFER_MODE_NORMAL,
        .p_dest = (void *) &g_buffer_adc[ADC_UNIT_0][ADC_GROUP_SCAN][1U][0U][0U],
        .p_src = (void const *) &R_ADC_D->ADCR[1U], /* Select ADC channel 1 */
        .num_blocks = 0U,
        .length = NUM_SAMPLES_PER_CHANNEL,
    },
};

/*******************************************************************************************************************//**
 * @brief     This function initializes ADC module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t adc_init(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Open ADC module */
    err = R_ADC_D_Open(&g_adc0_ctrl, &g_adc0_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function starts ADC scan operation.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Started successfully.
 * @retval    Any other error codes     Failed to start.
 **********************************************************************************************************************/
fsp_err_t adc_scan_start(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Start ADC scan */
    err = R_ADC_D_ScanStart(&g_adc0_ctrl);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function configures the ADC channel.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Configured successfully.
 * @retval    Any other error codes     Failed to configure.
 **********************************************************************************************************************/
fsp_err_t adc_channel_config(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Configure ADC channel */
    err = R_ADC_D_ScanCfg(&g_adc0_ctrl, &g_adc0_channel_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function closes the opened ADC module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void adc_deinit(void)
{
    /* Close the opened ADC module */
    if (g_adc0_ctrl.opened)
    {
        R_ADC_D_Close(&g_adc0_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function is a callback for the ADC module.
 * @param[in] p_args                    Callback function parameter data.
 * @retval    None.
 **********************************************************************************************************************/
void g_adc0_callback(adc_callback_args_t * p_args)
{
    adc_event_t event = p_args->event;
    switch (event)
    {
        case ADC_EVENT_SCAN_COMPLETE:
        {
            g_adc0_group_flag = true;
            break;
        }

        default:
        {
            g_err_flag_adc0 = true;
            break;
        }
    }
}
