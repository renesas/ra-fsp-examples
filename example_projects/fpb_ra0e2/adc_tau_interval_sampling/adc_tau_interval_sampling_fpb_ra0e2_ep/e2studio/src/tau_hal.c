/***********************************************************************************************************************
 * File Name    : tau_hal.c
 * Description  : Contains function definition.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "tau_hal.h"

/*******************************************************************************************************************//**
 * @addtogroup tau_hal.c
 * @{
 **********************************************************************************************************************/

/*****************************************************************************************************************
 * @brief       Initializes TAU module instances.
 * @param[IN]   p_ctrl         TAU instance control structure.
 * @param[IN]   p_cfg          TAU instance config  structure.
 * @retval      FSP_SUCCESS    TAU instance opened successfully.
 * @retval      Any Other Error code apart from FSP_SUCCES on Unsuccessful opening of tau instance .
 ****************************************************************************************************************/
fsp_err_t init_hal_tau(timer_ctrl_t * const p_ctrl, timer_cfg_t const * const p_cfg)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Open TAU instance */
    err = R_TAU_Open(p_ctrl, p_cfg);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_TAU_Open FAILED ** \r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This functions closes TAU instance.
 * @param[IN]   p_ctrl_timer                TAU control instance
 * @retval      FSP_SUCCESS                 Upon successful close operation.
 * @retval      Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
void deinit_hal_tau(timer_ctrl_t * const p_ctrl_timer )
{
    fsp_err_t err = FSP_SUCCESS;
    /* Close opened TAU instances */
    err = R_TAU_Close(p_ctrl_timer);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_TAU_Close FAILED ** \r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief       This functions starts TAU timer.
 * @param[IN]   p_ctrl_timer                TAU control instance
 * @retval      FSP_SUCCESS                 Upon successful start operation.
 * @retval      Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t tau_timer_start(timer_ctrl_t * const p_ctrl_timer)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Start TAU timer */
    err = R_TAU_Start(p_ctrl_timer);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_TAU_Start FAILED ** \r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @} (end addtogroup tau_hal.c)
 **********************************************************************************************************************/
