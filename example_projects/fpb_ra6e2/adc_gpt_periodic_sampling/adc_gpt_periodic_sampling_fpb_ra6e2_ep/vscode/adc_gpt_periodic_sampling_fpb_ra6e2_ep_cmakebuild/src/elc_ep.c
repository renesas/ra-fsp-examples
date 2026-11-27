/***********************************************************************************************************************
 * File Name    : elc_ep.c
 * Description  : Contains data structures and functions used in elc_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "elc_ep.h"

/*******************************************************************************************************************//**
 * @brief     This function initializes ELC module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t elc_init(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Open ELC module */
    err = R_ELC_Open(&g_elc_ctrl, &g_elc_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function enables ELC module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Enabled successfully.
 * @retval    Any other error codes     Failed to enable.
 **********************************************************************************************************************/
fsp_err_t elc_enable(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Enable ELC module */
    err = R_ELC_Enable(&g_elc_ctrl);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function closes the opened ELC module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void elc_deinit(void)
{
    /* Close the opened ELC module */
    if (g_elc_ctrl.open)
    {
        R_ELC_Close(&g_elc_ctrl);
    }
}
