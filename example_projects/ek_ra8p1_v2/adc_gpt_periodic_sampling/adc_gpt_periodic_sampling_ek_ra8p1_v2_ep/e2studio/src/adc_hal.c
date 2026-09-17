/***********************************************************************************************************************
 * File Name    : adc_hal.c
 * Description  : Contains function definition.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
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
#if BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK || BSP_FEATURE_ADC_B_UNIT_1_CHANNELS_MASK
uint16_t g_buffer_adc[ADC_UNIT_NUM][ADC_GROUP_SCAN_NUM][NUM_ADC_CHANNELS][NUM_SAMPLE_BUFFER][NUM_SAMPLES_PER_CHANNEL];
#else
uint16_t g_buffer_adc[ADC_UNIT_1][ADC_GROUP_SCAN_NUM][NUM_ADC_CHANNELS][NUM_SAMPLE_BUFFER][NUM_SAMPLES_PER_CHANNEL];
#endif /* BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK || BSP_FEATURE_ADC_B_UNIT_1_CHANNELS_MASK */
/* Flags to capture ADC 0/1 events */
volatile bool g_adc0_group_a_flag = false;
volatile bool g_adc0_group_b_flag = false;
#if BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK || BSP_FEATURE_ADC_B_UNIT_1_CHANNELS_MASK
volatile bool g_adc1_group_a_flag = false;
volatile bool g_adc1_group_b_flag = false;
#endif /* BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK || BSP_FEATURE_ADC_B_UNIT_1_CHANNELS_MASK */

/* Flag to capture error event */
#if (BSP_PERIPHERAL_ADC_B_PRESENT)
volatile bool g_err_flag_adc_b = false;
#elif (BSP_PERIPHERAL_ADC_PRESENT)
volatile bool g_err_flag_adc0 = false;
#if (BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK)
volatile bool g_err_flag_adc1 = false;
#endif /* BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK */
#endif /* BSP_PERIPHERAL_ADC_B_PRESENT */
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
        .p_src = (void const*) &ADC0_REG->ADDR[ONE],
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
        .p_src = (void const*) &ADC0_REG->ADDR[TWO],
        .num_blocks = ZERO,
        .length = NUM_SAMPLES_PER_CHANNEL,
    },

#if BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK || BSP_FEATURE_ADC_B_UNIT_1_CHANNELS_MASK
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
        .p_src = (void const*) &ADC1_REG->ADDR[SIX],
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
        .p_src = (void const*) &ADC1_REG->ADDR[SEVEN],
        .num_blocks = ZERO,
        .length = NUM_SAMPLES_PER_CHANNEL,
    },
#endif /* BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK || BSP_FEATURE_ADC_B_UNIT_1_CHANNELS_MASK */
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
        .p_src = (void const*) &ADC0_REG->ADDR[FOUR],
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
        .p_src = (void const*) &ADC0_REG->ADDR[FIVE],
        .num_blocks = ZERO,
        .length = NUM_SAMPLES_PER_CHANNEL,
    },

#if BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK || BSP_FEATURE_ADC_B_UNIT_1_CHANNELS_MASK
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
        .p_src = (void const*) &ADC1_REG->ADDR[TEN],
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
        .p_src = (void const*) &ADC1_REG->ADDR[THIRTEEN],
        .num_blocks = ZERO,
        .length = NUM_SAMPLES_PER_CHANNEL,
    },
#endif /* BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK || BSP_FEATURE_ADC_B_UNIT_1_CHANNELS_MASK */
};

/*******************************************************************************************************************//**
 * @brief       Initialize ADC module instances.
 * @param[in]   p_ctrl_adc         ADC instance control structure.
 * @param[in]   p_cfg_adc          ADC instance configure structure.
 * @retval      FSP_SUCCESS        ADC instance opened successfully.
 * @retval      Other error code if opening the ADC instance failed.
 **********************************************************************************************************************/
fsp_err_t init_hal_adc(adc_ctrl_t * p_ctrl_adc, adc_cfg_t const * const p_cfg_adc)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open ADC module */
#if (BSP_PERIPHERAL_ADC_B_PRESENT)
    err = R_ADC_B_Open(p_ctrl_adc, p_cfg_adc);
#else
    err = R_ADC_Open(p_ctrl_adc, p_cfg_adc);
#endif /* BSP_PERIPHERAL_ADC_B_PRESENT */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_"ADC_TYPE"_Open FAILED **\r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       Configure the ADC channel specific settings.
 * @param[in]   p_ctrl_adc                 ADC instance control structure.
 * @param[in]   p_channel_cfg_adc          ADC instance configure channel structure.
 * @retval      FSP_SUCCESS                On successful ADC channel configuration setting.
 * @retval      Other error code if ADC channel configuration failed.
 **********************************************************************************************************************/
fsp_err_t adc_channel_config(adc_ctrl_t * p_ctrl_adc, void const * const p_channel_cfg_adc)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Configure ADC channel specific settings */
#if (BSP_PERIPHERAL_ADC_B_PRESENT)
    err = R_ADC_B_ScanCfg(p_ctrl_adc, p_channel_cfg_adc);
#else
    err = R_ADC_ScanCfg(p_ctrl_adc, p_channel_cfg_adc);
#endif /* BSP_PERIPHERAL_ADC_B_PRESENT */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_"ADC_TYPE"_ScanCfg FAILED **\r\n");
    }

#if (defined(BOARD_RA2A1_EK)) || (BSP_PERIPHERAL_ADC_B_PRESENT)

    /* Calibrate the ADC */
    err = adc_start_calibration();
    if (FSP_SUCCESS != err)
    {
        /* ADC failure message */
        APP_ERR_PRINT("** adc_start_calibration FAILED **\r\n");
        return err;
    }
#endif /* BOARD_RA2A1_EK || BSP_PERIPHERAL_ADC_B_PRESENT */
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function closes ADC instance.
 * @param[in]   p_ctrl_adc      ADC control instance.
 * @retval      FSP_SUCCESS     Upon successful close operation.
 * @retval      Other error code if closing the ADC instance failed.
 **********************************************************************************************************************/
void deinit_hal_adc(adc_ctrl_t * p_ctrl_adc)
{
    fsp_err_t err = FSP_SUCCESS;
    
    /* Close opened ADC instance */
#if (BSP_PERIPHERAL_ADC_B_PRESENT)
    err = R_ADC_B_Close(p_ctrl_adc);
#else
    err = R_ADC_Close(p_ctrl_adc);
#endif /* BSP_PERIPHERAL_ADC_B_PRESENT */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_"ADC_TYPE"_Close FAILED **\r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief       This function starts ADC scan operation.
 * @param[in]   p_ctrl_adc      ADC control instance.
 * @retval      FSP_SUCCESS     Upon successful scan start operation.
 * @retval      Other error code if starting the ADC instance failed.
 **********************************************************************************************************************/
fsp_err_t scan_start_adc(adc_ctrl_t * p_ctrl_adc)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Starts ADC scan. API returns success immediately, but scanning begins only when a trigger event occurs */
#if (BSP_PERIPHERAL_ADC_B_PRESENT)
    err = R_ADC_B_ScanStart(p_ctrl_adc);
#else
    err = R_ADC_ScanStart(p_ctrl_adc);
#endif /* BSP_PERIPHERAL_ADC_B_PRESENT */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_"ADC_TYPE"_ScanStart FAILED **\r\n");
    }
    return err;
}

#if (BSP_PERIPHERAL_ADC_B_PRESENT)
/*******************************************************************************************************************//**
 *  @brief        User-defined callback function for ADC_B.
 *  @param[in]    p_args    Pointer to ADC callback arguments.
 *  @retval       None.
 **********************************************************************************************************************/
void g_adc0_callback(adc_callback_args_t *p_args)
{
    adc_event_t event = p_args->event;
    switch(event)
    {
        case ADC_EVENT_SCAN_COMPLETE:
        {
            /* For boards using ADC_B, using group mask to distinguish between groups */
            if (p_args->group_mask & ADC_GROUP_MASK_0)
            {
                /* Set the flag for ADC 0 group A (Group 0) */
                g_adc0_group_a_flag = true;
            }
            else if (p_args->group_mask & ADC_GROUP_MASK_1)
            {
                /* Set the flag for ADC 0 group B (Group 1) */
                g_adc0_group_b_flag = true;
            }
#if BSP_FEATURE_ADC_B_UNIT_1_CHANNELS_MASK
            else if (p_args->group_mask & ADC_GROUP_MASK_2)
            {
                /* Set the flag for ADC 1 group A (Group 2) */
                g_adc1_group_a_flag = true;
            }
            else if (p_args->group_mask & ADC_GROUP_MASK_3)
            {
                /* Set the flag for ADC 1 group B (Group 3) */
                g_adc1_group_b_flag = true;
            }
#endif /* BSP_FEATURE_ADC_B_UNIT_1_CHANNELS_MASK */
            else
            {
                /* Unknown group mask */
                g_err_flag_adc_b = true;
            }
            break;
        }

        case ADC_EVENT_CALIBRATION_COMPLETE:
        {
            /* Do nothing */
            break;
        }

        case ADC_EVENT_OVERFLOW:
        {
            /* ADC overflow occurred - DTC might not be keeping up with scan rate */
            /* This is expected in some cases, just ignore or log for debugging */
            break;
        }

        default:
        {
            /* Set the error flag for ADC_B when an unexpected event is received */
            g_err_flag_adc_b = true;
            break;
        }
    }
}
#elif (BSP_PERIPHERAL_ADC_PRESENT)
/*******************************************************************************************************************//**
 *  @brief        User-defined callback function for ADC unit 0.
 *  @param[in]    p_args    Pointer to ADC callback arguments.
 *  @retval       None.
 **********************************************************************************************************************/
void g_adc0_callback(adc_callback_args_t *p_args)
{
    adc_event_t event = p_args->event;
    switch (event)
    {
        case ADC_EVENT_SCAN_COMPLETE:
        {
            /* Set the flag for ADC0 Group A when ADC_EVENT_SCAN_COMPLETE_GROUP_A occurs */
            g_adc0_group_a_flag = true;
            break;
        }

        case ADC_EVENT_SCAN_COMPLETE_GROUP_B:
        {
            /* Set the flag for ADC0 Group B when ADC_EVENT_SCAN_COMPLETE_GROUP_B occurs */
            g_adc0_group_b_flag = true;
            break;
        }

#ifdef BOARD_RA2A1_EK
        case ADC_EVENT_CALIBRATION_COMPLETE:
        {
            /* Do nothing */
            break;
        }
#endif /* BOARD_RA2A1_EK */
        default:
        {
            /* Set the error flag for ADC0 when an unexpected event is received */
            g_err_flag_adc0 = true;
            break;
        }
    }
}
#if BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK
/*******************************************************************************************************************//**
 *  @brief        User-defined callback function for ADC unit 1.
 *  @param[in]    p_args    Pointer to ADC callback arguments.
 *  @retval       None.
 **********************************************************************************************************************/
void g_adc1_callback(adc_callback_args_t *p_args)
{
    adc_event_t event = p_args->event;
    switch (event)
    {
        case ADC_EVENT_SCAN_COMPLETE:
        {
            /* Set the flag for ADC1 Group A when ADC_EVENT_SCAN_COMPLETE_GROUP_A occurs */
            g_adc1_group_a_flag = true;
            break;
        }

        case ADC_EVENT_SCAN_COMPLETE_GROUP_B:
        {
            /* Set the flag for ADC1 Group B when ADC_EVENT_SCAN_COMPLETE_GROUP_B occurs */
            g_adc1_group_b_flag = true;
            break;
        }

        default:
        {
            /* Set the error flag for ADC1 when an unexpected event is received */
            g_err_flag_adc1 = true;
            break;
        }
    }
}
#endif /* BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK */
#endif /* BSP_PERIPHERAL_ADC_B_PRESENT */

#if (defined(BOARD_RA2A1_EK)) || (BSP_PERIPHERAL_ADC_B_PRESENT)
/*******************************************************************************************************************//**
 * @brief       This function initiates the calibration.
 * @param[in]   None.
 * @retval      FSP_SUCCESS     Upon successful of ADC calibration.
 * @retval      Other error code if calibrating the ADC instance failed.
 **********************************************************************************************************************/
fsp_err_t adc_start_calibration(void)
{
    fsp_err_t err = FSP_SUCCESS;
    adc_status_t adc_status;    /* To get ADC status */

    /* Initiate ADC calibration */
#if (BSP_PERIPHERAL_ADC_B_PRESENT)
    err = R_ADC_B_Calibrate(&g_adc0_ctrl, NULL);
#else
    err = R_ADC_Calibrate(&g_adc0_ctrl, NULL);
#endif /* BSP_PERIPHERAL_ADC_B_PRESENT */
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* ADC failure message */
        APP_ERR_PRINT("** R_"ADC_TYPE"_Calibrate API FAILED **\r\n");
        return err;
    }

    do
    {
        /* To get the ADC status */
#if (BSP_PERIPHERAL_ADC_B_PRESENT)
        err = R_ADC_B_StatusGet(&g_adc0_ctrl, &adc_status);
#else
        err = R_ADC_StatusGet(&g_adc0_ctrl, &adc_status);
#endif /* BSP_PERIPHERAL_ADC_B_PRESENT */
        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            /* ADC status failure message */
            APP_ERR_PRINT("** R_"ADC_TYPE"_StatusGet API FAILED **\r\n");
            return err;
        }
    } while (ADC_STATE_IDLE != adc_status.state); /* Wait here till the calibration.
                                                   * It takes 24msec to 780msec based on clock */

    return err;
}
#endif /* BOARD_RA2A1_EK || BSP_PERIPHERAL_ADC_B_PRESENT */

/*******************************************************************************************************************//**
 * @} (end addtogroup adc_hal.c)
 **********************************************************************************************************************/
