/***********************************************************************************************************************
 * File Name    : adc_ep.c
 * Description  : Contains data structures and functions used in adc_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "adc_ep.h"

/* User buffer [unit][group][channel][ping_pong_buffer][sample] */
#if BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK || BSP_FEATURE_ADC_B_UNIT_1_CHANNELS_MASK
uint16_t g_buffer_adc[ADC_UNIT_NUM][ADC_GROUP_SCAN_NUM][NUM_ADC_CHANNELS][NUM_SAMPLE_BUFFER][NUM_SAMPLES_PER_CHANNEL];
#else
uint16_t g_buffer_adc[ADC_UNIT_1][ADC_GROUP_SCAN_NUM][NUM_ADC_CHANNELS][NUM_SAMPLE_BUFFER][NUM_SAMPLES_PER_CHANNEL];
#endif /* BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK || BSP_FEATURE_ADC_B_UNIT_1_CHANNELS_MASK */

/* Flags to capture ADC unit 0/1 events */
volatile bool g_adc0_group_a_flag = false;
volatile bool g_adc0_group_b_flag = false;
#if BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK || BSP_FEATURE_ADC_B_UNIT_1_CHANNELS_MASK
volatile bool g_adc1_group_a_flag = false;
volatile bool g_adc1_group_b_flag = false;
#endif /* BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK || BSP_FEATURE_ADC_B_UNIT_1_CHANNELS_MASK */

/* Flags to capture error event */
#if (BSP_PERIPHERAL_ADC_B_PRESENT)
volatile bool g_err_flag_adc_b = false;
#elif (BSP_PERIPHERAL_ADC_PRESENT)
volatile bool g_err_flag_adc0 = false;
#if (BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK)
volatile bool g_err_flag_adc1 = false;
#endif /* BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK */
#endif /* BSP_PERIPHERAL_ADC_B_PRESENT || BSP_PERIPHERAL_ADC_PRESENT */

/* Transfer info for ADC unit 0/1 and group A */
transfer_info_t g_transfer_adc_group_a[] =
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
        .p_dest = (void*) &g_buffer_adc[ADC_UNIT_0][ADC_GROUP_SCAN_A][0U][0U][0U],
        .p_src = (void const*) &ADC0_REG->ADDR[0U], /* Select ADC channel 0 */
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
        .p_dest = (void*) &g_buffer_adc[ADC_UNIT_0][ADC_GROUP_SCAN_A][1U][0U][0U],
        .p_src = (void const*) &ADC0_REG->ADDR[1U], /* Select ADC channel 1 */
        .num_blocks = 0U,
        .length = NUM_SAMPLES_PER_CHANNEL,
    },

#if BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK || BSP_FEATURE_ADC_B_UNIT_1_CHANNELS_MASK
    [2U] =
    {
        .transfer_settings_word_b.dest_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED,
        .transfer_settings_word_b.repeat_area = TRANSFER_REPEAT_AREA_DESTINATION,
        .transfer_settings_word_b.irq = TRANSFER_IRQ_END,
        .transfer_settings_word_b.chain_mode = TRANSFER_CHAIN_MODE_EACH,
        .transfer_settings_word_b.src_addr_mode = TRANSFER_ADDR_MODE_FIXED,
        .transfer_settings_word_b.size = TRANSFER_SIZE_2_BYTE,
        .transfer_settings_word_b.mode = TRANSFER_MODE_NORMAL,
        .p_dest = (void*) &g_buffer_adc[ADC_UNIT_1][ADC_GROUP_SCAN_A][0U][0U][0U],
        .p_src = (void const*) &ADC1_REG->ADDR[0U], /* Select ADC channel 0 */
        .num_blocks = 0U,
        .length = NUM_SAMPLES_PER_CHANNEL,
    },

    [3U] =
    {
        .transfer_settings_word_b.dest_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED,
        .transfer_settings_word_b.repeat_area = TRANSFER_REPEAT_AREA_DESTINATION,
        .transfer_settings_word_b.irq = TRANSFER_IRQ_END,
        .transfer_settings_word_b.chain_mode = TRANSFER_CHAIN_MODE_DISABLED,
        .transfer_settings_word_b.src_addr_mode = TRANSFER_ADDR_MODE_FIXED,
        .transfer_settings_word_b.size = TRANSFER_SIZE_2_BYTE,
        .transfer_settings_word_b.mode = TRANSFER_MODE_NORMAL,
        .p_dest = (void*) &g_buffer_adc[ADC_UNIT_1][ADC_GROUP_SCAN_A][1U][0U][0U],
        .p_src = (void const*) &ADC1_REG->ADDR[1U], /* Select ADC channel 1 */
        .num_blocks = 0U,
        .length = NUM_SAMPLES_PER_CHANNEL,
    },
#endif /* BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK || BSP_FEATURE_ADC_B_UNIT_1_CHANNELS_MASK */
};

/* Transfer info for ADC unit 0/1 and group B */
transfer_info_t g_transfer_adc_group_b[] =
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
        .p_dest = (void*) &g_buffer_adc[ADC_UNIT_0][ADC_GROUP_SCAN_B][0U][0U][0U],
        .p_src = (void const*) &ADC0_REG->ADDR[2U], /* Select ADC channel 2 */
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
        .p_dest = (void*) &g_buffer_adc[ADC_UNIT_0][ADC_GROUP_SCAN_B][1U][0U][0U],
        .p_src = (void const*) &ADC0_REG->ADDR[4U], /* Select ADC channel 4 */
        .num_blocks = 0U,
        .length = NUM_SAMPLES_PER_CHANNEL,
    },

#if BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK || BSP_FEATURE_ADC_B_UNIT_1_CHANNELS_MASK
    [2U] =
    {
        .transfer_settings_word_b.dest_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED,
        .transfer_settings_word_b.repeat_area = TRANSFER_REPEAT_AREA_DESTINATION,
        .transfer_settings_word_b.irq = TRANSFER_IRQ_END,
        .transfer_settings_word_b.chain_mode = TRANSFER_CHAIN_MODE_EACH,
        .transfer_settings_word_b.src_addr_mode = TRANSFER_ADDR_MODE_FIXED,
        .transfer_settings_word_b.size = TRANSFER_SIZE_2_BYTE,
        .transfer_settings_word_b.mode = TRANSFER_MODE_NORMAL,
        .p_dest = (void*) &g_buffer_adc[ADC_UNIT_1][ADC_GROUP_SCAN_B][0U][0U][0U],
        .p_src = (void const*) &ADC1_REG->ADDR[2U], /* Select ADC channel 2 */
        .num_blocks = 0U,
        .length = NUM_SAMPLES_PER_CHANNEL,
    },

    [3U] =
    {
        .transfer_settings_word_b.dest_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED,
        .transfer_settings_word_b.repeat_area = TRANSFER_REPEAT_AREA_DESTINATION,
        .transfer_settings_word_b.irq = TRANSFER_IRQ_END,
        .transfer_settings_word_b.chain_mode = TRANSFER_CHAIN_MODE_DISABLED,
        .transfer_settings_word_b.src_addr_mode = TRANSFER_ADDR_MODE_FIXED,
        .transfer_settings_word_b.size = TRANSFER_SIZE_2_BYTE,
        .transfer_settings_word_b.mode = TRANSFER_MODE_NORMAL,
        .p_dest = (void*) &g_buffer_adc[ADC_UNIT_1][ADC_GROUP_SCAN_B][1U][0U][0U],
        .p_src = (void const*) &ADC1_REG->ADDR[4U], /* Select ADC channel 4 */
        .num_blocks = 0U,
        .length = NUM_SAMPLES_PER_CHANNEL,
    },
#endif /* BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK || BSP_FEATURE_ADC_B_UNIT_1_CHANNELS_MASK */
};

#if (defined(BOARD_RA2A1_EK)) || (BSP_PERIPHERAL_ADC_B_PRESENT)
static fsp_err_t adc_start_calibration(void);
#endif /* BOARD_RA2A1_EK || BSP_PERIPHERAL_ADC_B_PRESENT */

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
    err = ADC_OPEN(&g_adc0_ctrl, &g_adc0_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

#if BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK
    err = ADC_OPEN(&g_adc1_ctrl, &g_adc1_cfg);
#endif /* BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK */

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
    err = ADC_SCANSTART(&g_adc0_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

#if BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK
    err = ADC_SCANSTART(&g_adc1_ctrl);
#endif /* BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK */

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
    err = ADC_SCANCFG(&g_adc0_ctrl, &ADC_CHANNEL_CONFIG);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

#if BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK
    err = ADC_SCANCFG(&g_adc1_ctrl, &g_adc1_channel_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
#endif /* BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK */

#if (defined(BOARD_RA2A1_EK) || (BSP_PERIPHERAL_ADC_B_PRESENT))
    /* Calibrate the ADC */
    err = adc_start_calibration();
#endif /* BOARD_RA2A1_EK || BSP_PERIPHERAL_ADC_B_PRESENT */

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
        ADC_CLOSE(&g_adc0_ctrl);
    }

#if BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK
    if (g_adc1_ctrl.opened)
    {
        ADC_CLOSE(&g_adc1_ctrl);
    }
#endif /* BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK */
}

#if (defined(BOARD_RA2A1_EK)) || (BSP_PERIPHERAL_ADC_B_PRESENT)
/*******************************************************************************************************************//**
 * @brief     This function starts ADC calibration.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Started successfully.
 * @retval    Any other error codes     Failed to start.
 **********************************************************************************************************************/
static fsp_err_t adc_start_calibration(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;
    adc_status_t adc_status;

    /* Initiate ADC calibration */
    err = ADC_CALIBRATE(&g_adc0_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Wait here until the calibration. It takes 24 milliseconds to 780 milliseconds based on clock */
    do
    {
        /* Get the ADC status */
        err = ADC_STATUSGET(&g_adc0_ctrl, &adc_status);
        if (FSP_SUCCESS != err)
        {
            return err;
        }
    } while (ADC_STATE_IDLE != adc_status.state);

    return err;
}
#endif /* BOARD_RA2A1_EK || BSP_PERIPHERAL_ADC_B_PRESENT */

#if (BSP_PERIPHERAL_ADC_B_PRESENT)
/*******************************************************************************************************************//**
 * @brief     The user-defined callback function for ADC_B module.
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
            if (p_args->group_mask & ADC_GROUP_MASK_0)
            {
                g_adc0_group_a_flag = true;
            }
            else if (p_args->group_mask & ADC_GROUP_MASK_1)
            {
                g_adc0_group_b_flag = true;
            }

#if BSP_FEATURE_ADC_B_UNIT_1_CHANNELS_MASK
            else if (p_args->group_mask & ADC_GROUP_MASK_2)
            {
                g_adc1_group_a_flag = true;
            }
            else if (p_args->group_mask & ADC_GROUP_MASK_3)
            {
                g_adc1_group_b_flag = true;
            }
#endif /* BSP_FEATURE_ADC_B_UNIT_1_CHANNELS_MASK */

            else
            {
                g_err_flag_adc_b = true;
            }
            break;
        }

        case ADC_EVENT_CALIBRATION_COMPLETE:
        {
            break;
        }

        case ADC_EVENT_OVERFLOW:
        {
            break;
        }

        default:
        {
            g_err_flag_adc_b = true;
            break;
        }
    }
}

#elif (BSP_PERIPHERAL_ADC_PRESENT)
/*******************************************************************************************************************//**
 * @brief     The user-defined callback function for ADC0.
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
            g_adc0_group_a_flag = true;
            break;
        }

        case ADC_EVENT_SCAN_COMPLETE_GROUP_B:
        {
            g_adc0_group_b_flag = true;
            break;
        }

#ifdef BOARD_RA2A1_EK
        case ADC_EVENT_CALIBRATION_COMPLETE:
        {
            break;
        }
#endif /* BOARD_RA2A1_EK */

        default:
        {
            g_err_flag_adc0 = true;
            break;
        }
    }
}

#if BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK
/*******************************************************************************************************************//**
 * @brief     The user-defined callback function for ADC1.
 * @param[in] p_args                    Callback function parameter data.
 * @retval    None.
 **********************************************************************************************************************/
void g_adc1_callback(adc_callback_args_t * p_args)
{
    adc_event_t event = p_args->event;
    switch (event)
    {
        case ADC_EVENT_SCAN_COMPLETE:
        {
            g_adc1_group_a_flag = true;
            break;
        }

        case ADC_EVENT_SCAN_COMPLETE_GROUP_B:
        {
            g_adc1_group_b_flag = true;
            break;
        }

        default:
        {
            g_err_flag_adc1 = true;
            break;
        }
    }
}
#endif /* BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK */
#endif /* BSP_PERIPHERAL_ADC_B_PRESENT || BSP_PERIPHERAL_ADC_PRESENT */
