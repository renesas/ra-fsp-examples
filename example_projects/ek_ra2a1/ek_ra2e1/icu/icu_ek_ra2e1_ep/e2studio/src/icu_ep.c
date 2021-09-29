/***********************************************************************************************************************
 * File Name    : icu_ep.c
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
#include "icu_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup icu_ep
 * @{
 **********************************************************************************************************************/

/* Boolean flag to determine switch is pressed or not.*/
volatile bool g_sw_press = false;

/*******************************************************************************************************************//**
 * @brief       This functions initializes ICU module.
 * @param[IN]   None
 * @retval      FSP_SUCCESS                  Upon successful open of ICU module
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful open
 **********************************************************************************************************************/
fsp_err_t icu_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open ICU module */
    err = R_ICU_ExternalIrqOpen(&g_external_irq_ctrl, &g_external_irq_cfg);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* ICU Open failure message */
        APP_ERR_PRINT ("\r\n**R_ICU_ExternalIrqOpen API FAILED**\r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function enables external interrupt for specified channel.
 * @param[IN]   None
 * @retval      FSP_SUCCESS                  Upon successful enable of ICU module
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful open
 **********************************************************************************************************************/
fsp_err_t icu_enable(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Enable ICU module */
    err = R_ICU_ExternalIrqEnable(&g_external_irq_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* ICU Enable failure message */
        APP_ERR_PRINT ("\r\n**R_ICU_ExternalIrqEnable API FAILED**\r\n");
    }
    return err;
}


/*******************************************************************************************************************//**
 * @brief       This function closes opened ICU module before the project ends up in an Error Trap.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
void icu_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close ICU module */
    err = R_ICU_ExternalIrqClose(&g_external_irq_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* ICU Close failure message */
        APP_ERR_PRINT("\r\n**R_ICU_ExternalIrqClose API FAILED**\r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief      User defined external irq callback.
 * @param[IN]  p_args
 * @retval     None
 **********************************************************************************************************************/
void irq_ep_callback(external_irq_callback_args_t *p_args)
{
    /* Make sure it's the right interrupt*/
    if(USER_SW_IRQ_NUMBER == p_args->channel)
    {
        g_sw_press = true;
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup icu_ep)
 **********************************************************************************************************************/
