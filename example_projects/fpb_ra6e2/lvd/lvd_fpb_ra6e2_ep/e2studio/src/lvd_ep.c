/***********************************************************************************************************************
 * File Name    : lvd_ep.c
 * Description  : Contains data structures and functions used in lvd_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "lvd_ep.h"

/* Flag to check occurrence of LVD1 threshold raise */
volatile bool g_lvd1_status = false;

/*******************************************************************************************************************//**
 * @brief     This function checks whether the last MCU reset was caused by the LVD0 monitor, clearing the
 *            flag once read.
 * @param[in] None.
 * @retval    true                      The last reset was caused by the LVD0 monitor.
 * @retval    false                     The last reset was not caused by the LVD0 monitor.
 **********************************************************************************************************************/
bool lvd0_reset(void)
{
#if defined (BOARD_RA0E1_FPB) || defined (BOARD_RA0E2_FPB) || defined (BOARD_RA0L1_FPB)|| defined (BOARD_RA0E3_FPB)
    if (SYSTEM_RSTSR0_DETECT_LVD0_RESET != R_SYSTEM->RESF_b.LVIRF)
    {
        return false;
    }

    /* Flag clears automatically on these boards */
#else
    if (SYSTEM_RSTSR0_DETECT_LVD0_RESET != R_SYSTEM->RSTSR0_b.LVD0RF)
    {
        return false;
    }

    /* Clear the flag once the value is read */
    R_SYSTEM->RSTSR0_b.LVD0RF = RESET_VALUE;
#endif /* Specific board */

    return true;
}

/*******************************************************************************************************************//**
 * @brief     This function initializes LVD1 module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t lvd_init(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Open LVD1 */
    err = R_LVD_Open(&g_lvd_ctrl, &g_lvd_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function clears the LVD1 status flag.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Cleared successfully.
 * @retval    Any other error codes     Failed to clear.
 **********************************************************************************************************************/
fsp_err_t lvd_status_clear(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Clear LVD1 status */
    err = R_LVD_StatusClear(&g_lvd_ctrl);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function closes the opened LVD1 module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void lvd_deinit(void)
{
    /* Close opened LVD1 */
    if (g_lvd_ctrl.open)
    {
        R_LVD_Close(&g_lvd_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function is a callback for LVD1 and notifies the user about a voltage increase.
 * @param[in] p_args                    Callback function parameter data.
 * @retval    None.
 **********************************************************************************************************************/
void lvd1_callback(lvd_callback_args_t *p_args)
{
    /* Voltage raising detected */
    if (LVD_CURRENT_STATE_ABOVE_THRESHOLD == p_args->current_state)
    {
        /* Set the LVD1 status flag to notify the user about a voltage increase */
        g_lvd1_status = true;
    }
}
