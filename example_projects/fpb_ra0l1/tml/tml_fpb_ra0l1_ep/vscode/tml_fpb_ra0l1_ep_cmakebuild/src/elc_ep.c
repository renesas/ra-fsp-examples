/***********************************************************************************************************************
 * File Name    : elc_ep.c
 * Description  : Contains declarations of data structures and functions used in elc_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "elc_ep.h"

/*******************************************************************************************************************//**
 * @brief     This function initializes all the links in the Event Link Controller.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t elc_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_ELC_Open(&g_elc_ctrl, &g_elc_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function enables the operation of the Event Link Controller.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Enabled successfully.
 * @retval    Any other error codes     Failed to enable.
 **********************************************************************************************************************/
fsp_err_t elc_enable(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_ELC_Enable(&g_elc_ctrl);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function generates a software event in the Event Link Controller.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Generated successfully.
 * @retval    Any other error codes     Failed to generate.
 **********************************************************************************************************************/
fsp_err_t elc_event_gen(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_ELC_SoftwareEventGenerate(&g_elc_ctrl, ELC_SOFTWARE_EVENT_0);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function de-initializes the opened ELC module.
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
