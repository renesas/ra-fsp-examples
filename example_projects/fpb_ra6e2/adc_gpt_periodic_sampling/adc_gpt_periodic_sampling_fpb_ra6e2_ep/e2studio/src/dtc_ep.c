/***********************************************************************************************************************
 * File Name    : dtc_ep.c
 * Description  : Contains data structures and functions used in dtc_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "dtc_ep.h"

/*******************************************************************************************************************//**
 * @brief     This function initializes DTC module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t dtc_init(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Open DTC module */
    err = R_DTC_Open(&g_transfer_adc0_group_a_ctrl, &g_transfer_adc0_group_a_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = R_DTC_Open(&g_transfer_adc0_group_b_ctrl, &g_transfer_adc0_group_b_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
#if BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK || BSP_FEATURE_ADC_B_UNIT_1_CHANNELS_MASK
    err = R_DTC_Open(&g_transfer_adc1_group_a_ctrl, &g_transfer_adc1_group_a_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = R_DTC_Open(&g_transfer_adc1_group_b_ctrl, &g_transfer_adc1_group_b_cfg);
#endif /* BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK || BSP_FEATURE_ADC_B_UNIT_1_CHANNELS_MASK */

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function reconfigures DTC module.
 * @param[in] p_api_ctrl_dtc            Pointer to the DTC control structure.
 * @param[in] p_info                    Pointer to transfer info structure.
 * @retval    FSP_SUCCESS               Reconfigured successfully.
 * @retval    Any other error codes     Failed to reconfigure.
 **********************************************************************************************************************/
fsp_err_t dtc_reconfigure(transfer_ctrl_t * const p_api_ctrl_dtc, transfer_info_t * p_info)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Reconfigure DTC module */
    err = R_DTC_Reconfigure(p_api_ctrl_dtc, p_info);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function enables DTC module.
 * @param[in] p_api_ctrl_dtc            Pointer to the DTC control structure.
 * @retval    FSP_SUCCESS               Enabled successfully.
 * @retval    Any other error codes     Failed to enable.
 **********************************************************************************************************************/
fsp_err_t dtc_enable(transfer_ctrl_t * const p_api_ctrl_dtc)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Enable DTC module */
    err = R_DTC_Enable(p_api_ctrl_dtc);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function closes all the opened DTC modules.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void dtc_deinit(void)
{
    /* Close all the opened DTC modules */
    if (g_transfer_adc0_group_a_ctrl.open)
    {
        R_DTC_Close(&g_transfer_adc0_group_a_ctrl);
    }
    if (g_transfer_adc0_group_b_ctrl.open)
    {
        R_DTC_Close(&g_transfer_adc0_group_b_ctrl);
    }
#if BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK || BSP_FEATURE_ADC_B_UNIT_1_CHANNELS_MASK
    if (g_transfer_adc1_group_a_ctrl.open)
    {
        R_DTC_Close(&g_transfer_adc1_group_a_ctrl);
    }
    if (g_transfer_adc1_group_b_ctrl.open)
    {
        R_DTC_Close(&g_transfer_adc1_group_b_ctrl);
    }
#endif /* BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK || BSP_FEATURE_ADC_B_UNIT_1_CHANNELS_MASK */
}
