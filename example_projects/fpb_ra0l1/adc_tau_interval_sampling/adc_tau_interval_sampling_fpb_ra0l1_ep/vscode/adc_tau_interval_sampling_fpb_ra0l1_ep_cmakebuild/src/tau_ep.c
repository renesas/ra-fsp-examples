/***********************************************************************************************************************
 * File Name    : tau_ep.c
 * Description  : Contains data structures and functions used in tau_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "tau_ep.h"

/*******************************************************************************************************************//**
 * @brief     This function initializes the TAU timer module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t tau_init(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Open TAU timer module */
    err = R_TAU_Open(&g_timer0_ctrl, &g_timer0_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function starts the TAU timer module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Started successfully.
 * @retval    Any other error codes     Failed to start.
 **********************************************************************************************************************/
fsp_err_t tau_start(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Start TAU timer module */
    err = R_TAU_Start(&g_timer0_ctrl);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function closes the opened TAU timer module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void tau_deinit(void)
{
    /* Close the opened TAU timer module */
    if (g_timer0_ctrl.open)
    {
        R_TAU_Close(&g_timer0_ctrl);
    }
}
