/***********************************************************************************************************************
 * File Name    : timer_initialise.c
 * Description  : Contains data structures and functions used in timer_initialise.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "timer_initialise.h"

/*******************************************************************************************************************//**
 * @addtogroup timer_initialise
 * @{
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 *  @brief       GPT timer initialization function.
 *  @param[IN]   None
 *  @retval      FSP_SUCCESS        Upon successful open and start of timer.
 *  @retval      Any other error code apart from FSP_SUCCESS is Unsuccessful open or start.
 **********************************************************************************************************************/
fsp_err_t gpt_timer_init(void)
{
    /* Variable to help handle error codes from functions */
    fsp_err_t fsp_err = FSP_SUCCESS;

    /* Open the GPT timer */
    fsp_err = R_GPT_Open(&g_gpt_ctrl, &g_gpt_cfg);
    /* Handle error in case of failure */
    if (FSP_SUCCESS != fsp_err)
    {
        APP_ERR_PRINT("\r\nGPT timer open failed. Necessary cleanup is done.\r\n");
        return fsp_err;
    }

    /* Start GPT timer */
    fsp_err = R_GPT_Start(&g_gpt_ctrl);
    /* Handle error in case of failure */
    if (FSP_SUCCESS != fsp_err)
    {
        fsp_err_t err = FSP_SUCCESS;
        err = R_GPT_Close(&g_gpt_ctrl);
        /* Handle error in case of failure */
        if (FSP_SUCCESS != err)
        {
            /* RTT failure message */
            APP_ERR_PRINT("GPT timer close failed.\r\n");
        }
        /* RTT failure message */
        APP_ERR_PRINT("\r\nGPT timer start failed. Necessary cleanup is done.\r\n");
    }
    return fsp_err;
}

/*******************************************************************************************************************//**
 *  @brief       AGT timer initialization function
 *  @param[IN]   None
 *  @retval      FSP_SUCCESS        Upon successful open and start of timer
 *  @retval      Any other error code apart from FSP_SUCCESS is  Unsuccessful open or start
 **********************************************************************************************************************/
fsp_err_t agt_timer_init(void)
{
    /* Variable to help handle error codes from functions */
    fsp_err_t fsp_err = FSP_SUCCESS;

    /* Open and Configure the AGT timer */
    fsp_err = R_AGT_Open(&g_agt_ctrl, &g_agt_cfg);
    /* Handle error in case of failure */
    if (FSP_SUCCESS != fsp_err)
    {
        /* RTT failure message */
        APP_ERR_PRINT("AGT timer open failed. Necessary cleanup is done.\r\n");
        return fsp_err;
    }

    /* Start the AGT timer */
    fsp_err = R_AGT_Start(&g_agt_ctrl);
    /* Handle error in case of failure */
    if (FSP_SUCCESS != fsp_err)
    {
        fsp_err_t err = FSP_SUCCESS;
        err = R_AGT_Close(&g_agt_ctrl);
        /* Handle error in case of failure */
        if (FSP_SUCCESS != err)
        {
            /* RTT failure message */
            APP_ERR_PRINT("AGT timer close failed.\n");
        }

        /* RTT failure message */
        APP_ERR_PRINT("\r\nAGT timer start failed. Necessary cleanup is done.\n");
    }
    return fsp_err;
}

/*******************************************************************************************************************//**
 *  @brief       GPT timer de-initialization function
 *  @param[IN]   None
 *  @retval      None
 **********************************************************************************************************************/
void gpt_timer_deinit(void)
{
    /* Variable to help handle error codes from functions */
    fsp_err_t fsp_err = FSP_SUCCESS;

    /* Close the GPT timer */
    fsp_err = R_GPT_Close(&g_gpt_ctrl);
    /* Handle error in case of failure */
    if (FSP_SUCCESS != fsp_err)
    {
        /* RTT failure message */
        APP_ERR_PRINT("GPT timer close failed.\r\n");
    }
}

/*******************************************************************************************************************//**
 *  @brief       AGT timer de-initialization function
 *  @param[IN]   None
 *  @retval      None
 **********************************************************************************************************************/
void agt_timer_deinit(void)
{
    /* Variable to help handle error codes from functions */
    fsp_err_t fsp_err = FSP_SUCCESS;

    /* Close the AGT timer */
    fsp_err = R_AGT_Close(&g_agt_ctrl);
    /* Handle error in case of failure */
    if (FSP_SUCCESS != fsp_err)
    {
        /* RTT failure message */
        APP_ERR_PRINT("AGT timer close failed.\r\n");
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup timer_initialise)
 **********************************************************************************************************************/
