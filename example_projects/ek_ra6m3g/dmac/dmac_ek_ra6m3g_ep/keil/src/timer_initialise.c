/***********************************************************************************************************************
 * File Name    : timer_initialise.c
 * Description  : Contains data structures and functions used in timer_initialise.c
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
#include "timer_initialise.h"

/*******************************************************************************************************************//**
 * @addtogroup timer_initialise
 * @{
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 *  @brief    GPT timer initialisation function
 *  @param[IN]   None
 *  @retval      FSP_SUCCESS                  Upon successful open and start of timer
 *  @retval      Any Other Error code apart from FSP_SUCCESS is  Unsuccessful open or start
 **********************************************************************************************************************/
fsp_err_t gpt_timer_init(void)
{
    /* Variable to help handle error codes from functions */
    fsp_err_t fsp_err = FSP_SUCCESS;

    /* Open the gpt timer */
    fsp_err = R_GPT_Open(&g_gpt_ctrl, &g_gpt_cfg);
    /* Handle error in-case of failure */
    if (FSP_SUCCESS != fsp_err)
    {
        APP_ERR_PRINT("\r\nGPT timer open failed. Necessary cleanup is done. \n");
        return fsp_err;
    }

    /* Start GPT timer */
    fsp_err = R_GPT_Start(&g_gpt_ctrl);
    /* Handle error in-case of failure */
    if (FSP_SUCCESS != fsp_err)
    {
        fsp_err_t err = FSP_SUCCESS;
        err = R_GPT_Close(&g_gpt_ctrl);
        /* Handle error in-case of failure */
        if (FSP_SUCCESS != err)
        {
            /* RTT failure message */
            APP_ERR_PRINT("GPT timer close failed.\n\n");
        }
        /* RTT failure message */
        APP_ERR_PRINT("\r\nGPT timer start failed. Necessary cleanup is done. \n");
    }
    return fsp_err;
}


/*******************************************************************************************************************//**
 *  @brief    AGT timer initialisation function
 *  @param[IN]   None
 *  @retval      FSP_SUCCESS                  Upon successful open and start of timer
 *  @retval      Any Other Error code apart from FSP_SUCCESS is  Unsuccessful open or start
 **********************************************************************************************************************/
fsp_err_t agt_timer_init(void)
{
    /* Variable to help handle error codes from functions */
    fsp_err_t fsp_err = FSP_SUCCESS;

    /* Open and Configure the AGT timer  */
    fsp_err = R_AGT_Open(&g_agt_ctrl, &g_agt_cfg);
    /* Handle error in-case of failure */
    if (FSP_SUCCESS != fsp_err)
    {
        /* RTT failure message */
        APP_ERR_PRINT("AGT timer open failed. Necessary cleanup is done. \n\n");
        return fsp_err;
    }

    /* Start the AGT timer. */
    fsp_err = R_AGT_Start(&g_agt_ctrl);
    /* Handle error in-case of failure */
    if (FSP_SUCCESS != fsp_err)
    {
        fsp_err_t err = FSP_SUCCESS;
        err = R_AGT_Close(&g_agt_ctrl);
        /* Handle error in-case of failure */
        if (FSP_SUCCESS != err)
        {
            /* RTT failure message */
            APP_ERR_PRINT("AGT timer close failed.\n");
        }

        /* RTT failure message */
        APP_ERR_PRINT("\r\nAGT timer start failed. Necessary cleanup is done. \n");
    }
    return fsp_err;
}

/*******************************************************************************************************************//**
 *  @brief    GPT timer deinitialisation function
 *  @param[IN]   None
 *  @retval      None
 **********************************************************************************************************************/
void gpt_timer_deinit(void)
{
    /* Variable to help handle error codes from functions */
    fsp_err_t fsp_err = FSP_SUCCESS;

    /* Open and Configure the AGT timer  */
    fsp_err = R_GPT_Close(&g_gpt_ctrl);
    /* Handle error in-case of failure */
    if (FSP_SUCCESS != fsp_err)
    {
        /* RTT failure message */
        APP_ERR_PRINT("GPT timer close failed.\n\n");
    }
}

/*******************************************************************************************************************//**
 *  @brief    AGT timer deinitialisation function
 *  @param[IN]   None
 *  @retval      None
 **********************************************************************************************************************/
void agt_timer_deinit(void)
{
    /* Variable to help handle error codes from functions */
    fsp_err_t fsp_err = FSP_SUCCESS;

    /* Open and Configure the AGT timer  */
    fsp_err = R_AGT_Close(&g_agt_ctrl);
    /* Handle error in-case of failure */
    if (FSP_SUCCESS != fsp_err)
    {
        /* RTT failure message */
        APP_ERR_PRINT("AGT timer close failed.\n\n");
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup timer_initialise)
 **********************************************************************************************************************/
