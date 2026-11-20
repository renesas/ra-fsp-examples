/***********************************************************************************************************************
 * File Name    : timer_initialise.c
 * Description  : Contains data structures and functions used in timer_initialise.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "timer_initialise.h"

/*******************************************************************************************************************//**
 * @brief     This function initializes the GPT timer.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to open or start.
 **********************************************************************************************************************/
fsp_err_t gpt_timer_init(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Open the GPT timer */
    err = R_GPT_Open(&g_gpt_ctrl, &g_gpt_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Start GPT timer */
    err = R_GPT_Start(&g_gpt_ctrl);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function initializes the AGT timer.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to open or start.
 **********************************************************************************************************************/
fsp_err_t agt_timer_init(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Open the AGT timer */
    err = R_AGT_Open(&g_agt_ctrl, &g_agt_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Start the AGT timer */
    err = R_AGT_Start(&g_agt_ctrl);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function de-initializes the opened GPT timer.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void gpt_timer_deinit(void)
{
    /* Close the opened GPT timer */
    if (g_gpt_ctrl.open)
    {
        R_GPT_Close(&g_gpt_ctrl);
    }
}

/*******************************************************************************************************************//**
 *  @brief      This function de-initializes the opened AGT timer.
 *  @param[in]  None.
 *  @retval     None.
 **********************************************************************************************************************/
void agt_timer_deinit(void)
{
    /* Close the opened AGT timer */
    if (g_agt_ctrl.open)
    {
        R_AGT_Close(&g_agt_ctrl);
    }
}
