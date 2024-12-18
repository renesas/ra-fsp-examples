/***********************************************************************************************************************
 * File Name    : gpt_hal.c
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
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#include "common_utils.h"
#include "gpt_hal.h"

/*******************************************************************************************************************//**
 * @addtogroup gpt_hal.c
 * @{
 **********************************************************************************************************************/

/* Completion flag for timer */
volatile bool g_timer0_complete_flag = false;

/*****************************************************************************************************************
 * @brief       Initializes GPT module instances.
 * @param[IN]   p_ctrl         GPT instance control structure.
 * @param[IN]   p_cfg          GPT instance config  structure.
 * @retval      FSP_SUCCESS    GPT instance opened successfully.
 * @retval      Any Other Error code apart from FSP_SUCCES on Unsuccessful opening of gpt instance .
 ****************************************************************************************************************/
fsp_err_t init_hal_gpt(timer_ctrl_t * const p_ctrl, timer_cfg_t const * const p_cfg)
{
    fsp_err_t err = FSP_SUCCESS;
    /*open gpt instance */
    err = R_GPT_Open(p_ctrl, p_cfg);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_GPT_Open FAILED ** \r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This functions closes GPT instance.
 * @param[IN]   p_ctrl_timer                GPT control instance
 * @retval      FSP_SUCCESS                 Upon successful close operation.
 * @retval      Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
void deinit_hal_gpt(timer_ctrl_t * const p_ctrl_timer )
{
    fsp_err_t err = FSP_SUCCESS;
    /*Close opened GPT instances */
    err = R_GPT_Close(p_ctrl_timer);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_GPT_Close FAILED ** \r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief       This functions starts GPT timer.
 * @param[IN]   p_ctrl_timer                GPT control instance
 * @retval      FSP_SUCCESS                 Upon successful start operation.
 * @retval      Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t gpt_timer_start(timer_ctrl_t * const p_ctrl_timer)
{
    fsp_err_t err = FSP_SUCCESS;
    /*Start gpt timer */
    err = R_GPT_Start(p_ctrl_timer);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_GPT_Start FAILED ** \r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This functions stops GPT timer.
 * @param[IN]   p_ctrl_timer                GPT control instance
 * @retval      FSP_SUCCESS                 Upon successful start operation.
 * @retval      Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t gpt_timer_stop(timer_ctrl_t * const p_ctrl_timer)
{
    fsp_err_t err = FSP_SUCCESS;
    /*Start gpt timer */
    err = R_GPT_Stop(p_ctrl_timer);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_GPT_Stop FAILED ** \r\n");
    }
    return err;
}
/*******************************************************************************************************************//**
 * @} (end addtogroup gpt_hal.c)
 **********************************************************************************************************************/
