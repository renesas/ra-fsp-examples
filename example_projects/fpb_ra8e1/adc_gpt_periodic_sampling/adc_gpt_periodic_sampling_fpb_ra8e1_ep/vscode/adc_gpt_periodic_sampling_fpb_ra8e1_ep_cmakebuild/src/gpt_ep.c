/***********************************************************************************************************************
 * File Name    : gpt_ep.c
 * Description  : Contains data structures and functions used in gpt_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "gpt_ep.h"

/*******************************************************************************************************************//**
 * @brief     This function initializes GPT timer module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t gpt_init(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Open GPT timer module */
    err = R_GPT_Open(&g_timer0_ctrl, &g_timer0_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function starts GPT timer module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Started successfully.
 * @retval    Any other error codes     Failed to start.
 **********************************************************************************************************************/
fsp_err_t gpt_start(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Start GPT timer module */
    err = R_GPT_Start(&g_timer0_ctrl);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function closes the opened GPT timer module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void gpt_deinit(void)
{
    /* Close the opened GPT timer module */
    if (g_timer0_ctrl.open)
    {
        R_GPT_Close(&g_timer0_ctrl);
    }
}
