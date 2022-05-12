/***********************************************************************************************************************
 * File Name    : timer_initialise.c
 * Description  : Contains data structures and functions used in timer_initialise.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright [2020-2021] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
 *
 * This software and documentation are supplied by Renesas Electronics America Inc. and may only be used with products
 * of Renesas Electronics Corp. and its affiliates ("Renesas").  No other uses are authorized.  Renesas products are
 * sold pursuant to Renesas terms and conditions of sale.  Purchasers are solely responsible for the selection and use
 * of Renesas products and Renesas assumes no liability.  No license, express or implied, to any intellectual property
 * right is granted by Renesas. This software is protected under all applicable laws, including copyright laws. Renesas
 * reserves the right to change or discontinue this software and/or this documentation. THE SOFTWARE AND DOCUMENTATION
 * IS DELIVERED TO YOU "AS IS," AND RENESAS MAKES NO REPRESENTATIONS OR WARRANTIES, AND TO THE FULLEST EXTENT
 * PERMISSIBLE UNDER APPLICABLE LAW, DISCLAIMS ALL WARRANTIES, WHETHER EXPLICITLY OR IMPLICITLY, INCLUDING WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT, WITH RESPECT TO THE SOFTWARE OR
 * DOCUMENTATION.  RENESAS SHALL HAVE NO LIABILITY ARISING OUT OF ANY SECURITY VULNERABILITY OR BREACH.  TO THE MAXIMUM
 * EXTENT PERMITTED BY LAW, IN NO EVENT WILL RENESAS BE LIABLE TO YOU IN CONNECTION WITH THE SOFTWARE OR DOCUMENTATION
 * (OR ANY PERSON OR ENTITY CLAIMING RIGHTS DERIVED FROM YOU) FOR ANY LOSS, DAMAGES, OR CLAIMS WHATSOEVER, INCLUDING,
 * WITHOUT LIMITATION, ANY DIRECT, CONSEQUENTIAL, SPECIAL, INDIRECT, PUNITIVE, OR INCIDENTAL DAMAGES; ANY LOST PROFITS,
 * OTHER ECONOMIC DAMAGE, PROPERTY DAMAGE, OR PERSONAL INJURY; AND EVEN IF RENESAS HAS BEEN ADVISED OF THE POSSIBILITY
 * OF SUCH LOSS, DAMAGES, CLAIMS OR COSTS.
 **********************************************************************************************************************/
#include "common_utils.h"
#include "timer_initialise.h"


/*******************************************************************************************************************//**
 *  @brief       AGT timer initialization function
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
 *  @brief       AGT timer deinitialisation function
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
