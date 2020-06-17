/***********************************************************************************************************************
 * File Name    : external_irq_initialise.c
 * Description  : Contains data structures and functions used in external_irq_initialise.c
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
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#include "common_utils.h"
#include "external_irq_initialise.h"

/*******************************************************************************************************************//**
 * @addtogroup external_irq_initialise
 * @{
 **********************************************************************************************************************/

/******************************************************************************************************************//**
 *  @brief    initialise External IRQ module
 *  @param[IN]   None
 *  @retval      FSP_SUCCESS                  Upon successful open and start of timer
 *  @retval      Any Other Error code apart from FSP_SUCCESS is  Unsuccessful open or start
 **********************************************************************************************************************/
fsp_err_t external_irq_init(void)
{
    /* Variable to help handle error codes from functions */
    fsp_err_t fsp_err = FSP_SUCCESS;

    /* Open IRQn (for Switch/user button)
     * Interrupt triggered from user pushbutton, will be used to trigger DMAC
     * transfer_gpt_timer_operation
     */
    fsp_err = R_ICU_ExternalIrqOpen(&g_external_irq_ctrl, &g_external_irq_cfg);
    /* Handle error in-case of failure */
    if (FSP_SUCCESS != fsp_err)
    {
        /* RTT failure message */
        APP_ERR_PRINT("\r\nExternal IRQ open failed. Necessary cleanup is done. \n");
    }
    return fsp_err;
}

/******************************************************************************************************************//**
 *  @brief    Deinitialise External IRQ module
 *  @param[IN]   None
 *  @retval      None
 **********************************************************************************************************************/
void external_irq_deinit(void)
{
    /* Variable to help handle error codes from functions */
    fsp_err_t fsp_err = FSP_SUCCESS;

    fsp_err = R_ICU_ExternalIrqClose(&g_external_irq_ctrl);
    /* Handle error in-case of failure */
    if (FSP_SUCCESS != fsp_err)
    {
        /* RTT failure message */
        APP_ERR_PRINT("\r\nExternal IRQ close failed. Necessary cleanup is done. \n");
    }

}

/*******************************************************************************************************************//**
 * @} (end addtogroup external_irq_initialise)
 **********************************************************************************************************************/
