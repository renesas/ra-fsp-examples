/***********************************************************************************************************************
 * File Name    : dtc_ep.c
 * Description  : Contains data structures and functions used in dtc_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
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
    err = R_DTC_Open(&g_transfer_adc0_group_ctrl, &g_transfer_adc0_group_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function reconfigures DTC module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Reconfigured successfully.
 * @retval    Any other error codes     Failed to reconfigure.
 **********************************************************************************************************************/
fsp_err_t dtc_reconfigure(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Reconfigure DTC module */
    err = R_DTC_Reconfigure(&g_transfer_adc0_group_ctrl, &g_transfer_adc_group[0U]);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function enables DTC module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Enabled successfully.
 * @retval    Any other error codes     Failed to enable.
 **********************************************************************************************************************/
fsp_err_t dtc_enable(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Enable DTC module */
    err = R_DTC_Enable(&g_transfer_adc0_group_ctrl);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function closes the opened DTC module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void dtc_deinit(void)
{
    /* Close the opened DTC module */
    if (g_transfer_adc0_group_ctrl.open)
    {
        R_DTC_Close(&g_transfer_adc0_group_ctrl);
    }
}
