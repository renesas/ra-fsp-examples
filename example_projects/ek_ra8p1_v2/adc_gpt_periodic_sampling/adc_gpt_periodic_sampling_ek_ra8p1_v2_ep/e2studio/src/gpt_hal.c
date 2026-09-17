/***********************************************************************************************************************
 * File Name    : gpt_hal.c
 * Description  : Contains function definition.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
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
 * @brief       Initialize GPT module instance.
 * @param[in]   p_ctrl          Pointer to the GPT instance control structure.
 * @param[in]   p_cfg           Pointer to the GPT instance configure structure.
 * @retval      FSP_SUCCESS     GPT instance opened successfully.
 * @retval      Other error code if opening the GPT instance failed.
 **********************************************************************************************************************/
fsp_err_t init_hal_gpt(timer_ctrl_t * const p_ctrl, timer_cfg_t const * const p_cfg)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open GPT instance */
    err = R_GPT_Open(p_ctrl, p_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_GPT_Open FAILED **\r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function closes GPT instance.
 * @param[in]   p_ctrl_timer    Pointer to the GPT instance control structure.
 * @retval      FSP_SUCCESS     Upon successful close operation.
 * @retval      Other error code if closing the GPT instance failed.
 **********************************************************************************************************************/
void deinit_hal_gpt(timer_ctrl_t * const p_ctrl_timer)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close opened GPT instances */
    err = R_GPT_Close(p_ctrl_timer);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_GPT_Close FAILED **\r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief       This function starts GPT timer.
 * @param[in]   p_ctrl_timer    Pointer to the GPT instance control structure.
 * @retval      FSP_SUCCESS     Upon successful start operation.
 * @retval      Other error code if starting the GPT instance failed.
 **********************************************************************************************************************/
fsp_err_t gpt_timer_start(timer_ctrl_t * const p_ctrl_timer)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Start GPT timer */
    err = R_GPT_Start(p_ctrl_timer);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_GPT_Start FAILED **\r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @} (end addtogroup gpt_hal.c)
 **********************************************************************************************************************/
