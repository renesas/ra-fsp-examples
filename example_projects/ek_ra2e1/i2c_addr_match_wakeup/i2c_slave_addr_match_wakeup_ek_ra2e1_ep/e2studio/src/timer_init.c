/***********************************************************************************************************************
 * File Name    : timer_init.c
 * Description  : Contains data structures and functions used in timer_init.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "iic_slave_addr_wu_ep.h"
#include "timer_init.h"

/***********************************************************************************************************************
 * Function Name: timer_init
 * Description  : This function initializes and starts the GPT timer
 * Arguments    : None
 * Return value : FSP_SUCCESS  Upon successful operation
 *                Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t timer_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open GPT Module */
    err = R_GPT_Open(&g_timer_sleep_ctrl, &g_timer_sleep_cfg);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n***R_GPT_Open API failed!!!***\r\n");

    /* Start GPT Module */
    err = R_GPT_Start(&g_timer_sleep_ctrl);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n***R_GPT_Start API failed!!!***\r\n");

    return err;
}
/***********************************************************************************************************************
* End of function timer_init
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: timer_reset
 * Description  : This function resets the counter value to 0 for GPT timer
 * Arguments    : None
 * Return value : FSP_SUCCESS  Upon successful operation
 *                Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t timer_reset(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Reset GPT Module */
    err = R_GPT_Reset(&g_timer_sleep_ctrl);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n***R_GPT_Reset API failed!!!***\r\n");

    return err;
}
/***********************************************************************************************************************
* End of function timer_reset
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: timer_deinit
 * Description  : This function de-initializes the GPT timer
 * Arguments    : None
 * Return value : None
 **********************************************************************************************************************/
void timer_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* De-initialize GPT module if it had already opened */
    if (MODULE_CLOSE != g_timer_sleep_ctrl.open)
    {
        err = R_GPT_Close(&g_timer_sleep_ctrl);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\n***R_GPT_Close API failed!!!***\r\n");
        }
    }
}
