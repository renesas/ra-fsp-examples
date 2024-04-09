/***********************************************************************************************************************
 * File Name    : tau_hal.c
 * Description  : Contains function definition.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2024 Renesas Electronics Corporation. All rights reserved.
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
    /*open tau instance */
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
    /*Close opened TAU instances */
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
    /*Start tau timer */
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
