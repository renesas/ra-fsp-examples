/***********************************************************************************************************************
 * File Name    : dmac_hal.c
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
#include <dmac_hal.h>

/*******************************************************************************************************************//**
 * @addtogroup dmac_hal.c
 * @{
 **********************************************************************************************************************/
/* Flags to capture dmac 0/1 completion */
volatile bool g_transfer_adc0_flag = false ;
volatile bool g_transfer_adc1_flag = false;

/*****************************************************************************************************************
 * @brief       Initializes DMAC module instances.
 * @param[IN]   p_api_ctrl_dmac     DMAC instance control structure.
 * @param[IN]   p_cfg_dmac          DMAC instance config  structure.
 * @retval      FSP_SUCCESS        DMAC instance opened successfully.
 * @retval      Any Other Error code apart from FSP_SUCCES on Unsuccessful opening of dmac instance .
 ****************************************************************************************************************/
fsp_err_t init_hal_dmac(transfer_ctrl_t * const p_api_ctrl_dmac, transfer_cfg_t const * const p_cfg_dmac)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Open DMAC instance */
    err = R_DMAC_Open(p_api_ctrl_dmac, p_cfg_dmac);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_DAMC_Open FAILED ** \r\n");
    }
    return err;
}

/*****************************************************************************************************************
 * @brief       Reconfigures DMAC transfer and enable transfer.
 * @param[IN]   p_api_ctrl_dmac             DMAC instance control structure.
 * @param[IN]   p_info                     config structure to be reconfigured with.
 * @retval      FSP_SUCCESS                On successful dmac re-configuration.
 * @retval      Any Other Error code apart from FSP_SUCCES on Unsuccessful reconfiguring of dmac instance.
 ****************************************************************************************************************/
fsp_err_t dmac_hal_reconfigure(transfer_ctrl_t * const p_api_ctrl_dmac, transfer_info_t * p_info)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Reconfigure DMAC instance with transfer info parameters */
    err = R_DMAC_Reconfigure(p_api_ctrl_dmac, p_info);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_DMAC_Reconfigure FAILED ** \r\n");
    }
    return err;
}

/*****************************************************************************************************************
 * @brief       Enable DMAC transfer for this activation source.
 * @param[IN]   p_api_ctrl_dmac         DMAC instance control structure.
 * @retval      FSP_SUCCESS            DMAC instance enabled successfully for activation source.
 * @retval      Any Other Error code apart from FSP_SUCCES on Unsuccessful enabling of dmac instance .
 ****************************************************************************************************************/
fsp_err_t dmac_enable(transfer_ctrl_t * const p_api_ctrl_dmac)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Enable DMAC operation */
    err = R_DMAC_Enable(p_api_ctrl_dmac);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_DMAC_Enable FAILED ** \r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This functions closes DMAC instance.
 * @param[IN]   p_api_ctrl_dmac              DMAC control instance
 * @retval      FSP_SUCCESS                 Upon successful close operation.
 * @retval      Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
void deinit_hal_dmac(transfer_ctrl_t * const p_api_ctrl_dmac)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Close opened DMAC instances */
    err = R_DMAC_Close(p_api_ctrl_dmac);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_DMAC_Close FAILED ** \r\n");
    }
}

/*******************************************************************************************************************//**
 *  @brief        User defined callback function for DMAC channel 0
 *  @param[IN]    p_args
 *  @retval       None
 **********************************************************************************************************************/
void g_transfer_adc0_cb(dmac_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);
    /* Set the flag for DMAC channel 1 when transfer completed */
    g_transfer_adc0_flag = true;
}

/*******************************************************************************************************************//**
 *  @brief        User defined callback function for DMAC channel 1.
 *  @param[IN]    p_args
 *  @retval       None
 **********************************************************************************************************************/
void g_transfer_adc1_cb(dmac_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);
    /* Set the flag for DMAC channel 0 when transfer completed */
    g_transfer_adc1_flag = true;
}

/*******************************************************************************************************************//**
 * @} (end addtogroup dmac_hal.c)
 **********************************************************************************************************************/
