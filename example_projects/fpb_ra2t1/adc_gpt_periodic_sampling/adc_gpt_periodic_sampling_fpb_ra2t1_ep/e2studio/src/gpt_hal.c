/***********************************************************************************************************************
 * File Name    : gpt_hal.c
 * Description  : Contains function definition.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "gpt_hal.h"

/*******************************************************************************************************************//**
 * @addtogroup gpt_hal.c
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * @brief       Initializes GPT module instances.
 * @param[IN]   p_ctrl          GPT instance control structure.
 * @param[IN]   p_cfg           GPT instance configure structure.
 * @retval      FSP_SUCCESS     GPT instance opened successfully.
 * @retval      Any other error code apart from FSP_SUCCES on unsuccessful opening of GPT instance.
 **********************************************************************************************************************/
fsp_err_t init_hal_gpt(timer_ctrl_t * const p_ctrl, timer_cfg_t const * const p_cfg)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open GPT instance */
    err = R_GPT_Open(p_ctrl, p_cfg);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_GPT_Open FAILED ** \r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This functions closes GPT instance.
 * @param[IN]   p_ctrl_timer    GPT control instance.
 * @retval      FSP_SUCCESS     Upon successful close operation.
 * @retval      Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
void deinit_hal_gpt(timer_ctrl_t * const p_ctrl_timer )
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close opened GPT instances */
    err = R_GPT_Close(p_ctrl_timer);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_GPT_Close FAILED ** \r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief       This functions starts GPT timer.
 * @param[IN]   p_ctrl_timer    GPT control instance.
 * @retval      FSP_SUCCESS     Upon successful start operation.
 * @retval      Any other error code apart from FSP_SUCCES.
 **********************************************************************************************************************/
fsp_err_t gpt_timer_start(timer_ctrl_t * const p_ctrl_timer)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Start GPT timer */
    err = R_GPT_Start(p_ctrl_timer);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_GPT_Start FAILED ** \r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @} (end addtogroup gpt_hal.c)
 **********************************************************************************************************************/
