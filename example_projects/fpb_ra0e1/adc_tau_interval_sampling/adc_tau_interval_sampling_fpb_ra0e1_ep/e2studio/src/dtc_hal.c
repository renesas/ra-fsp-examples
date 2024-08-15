/***********************************************************************************************************************
 * File Name    : dtc_hal.c
 * Description  : Contains function definition.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "dtc_hal.h"

/*******************************************************************************************************************//**
 * @addtogroup dtc_hal.c
 * @{
 **********************************************************************************************************************/

/*****************************************************************************************************************
 * @brief       Initializes DTC module instances.
 * @param[IN]   p_api_ctrl_dtc     DTC instance control structure.
 * @param[IN]   p_cfg_dtc          DTC instance config  structure.
 * @retval      FSP_SUCCESS        DTC instance opened successfully.
 * @retval      Any Other Error code apart from FSP_SUCCES on Unsuccessful opening of dtc instance .
 ****************************************************************************************************************/
fsp_err_t init_hal_dtc(transfer_ctrl_t * const p_api_ctrl_dtc, transfer_cfg_t const * const p_cfg_dtc)
{
    fsp_err_t err = FSP_SUCCESS;
    /* open dtc instance */
    err = R_DTC_Open(p_api_ctrl_dtc, p_cfg_dtc);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_DTC_Open FAILED ** \r\n");
    }
    return err;
}

/*****************************************************************************************************************
 * @brief       Reconfigures DTC transfer and enable transfer.
 * @param[IN]   p_api_ctrl_dtc             DTC instance control structure.
 * @param[IN]   p_info                     config structure to be reconfigured with.
 * @retval      FSP_SUCCESS                On successful dtc re-configuration.
 * @retval      Any Other Error code apart from FSP_SUCCES on Unsuccessful reconfiguring of dtc instance.
 ****************************************************************************************************************/
fsp_err_t dtc_hal_reconfigure(transfer_ctrl_t * const p_api_ctrl_dtc, transfer_info_t * p_info)
{
    fsp_err_t err = FSP_SUCCESS;
    /* reconfigure dtc instance with transfer info parameters */
    err = R_DTC_Reconfigure(p_api_ctrl_dtc, p_info);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_DTC_Reconfigure FAILED ** \r\n");
    }
    return err;
}

/*****************************************************************************************************************
 * @brief       Enable DTC transfer for this activation source.
 * @param[IN]   p_api_ctrl_dtc         DTC instance control structure.
 * @retval      FSP_SUCCESS            DTC instance enabled successfully for activation source.
 * @retval      Any Other Error code apart from FSP_SUCCES on Unsuccessful enabling of dtc instance .
 ****************************************************************************************************************/
fsp_err_t dtc_enable(transfer_ctrl_t * const p_api_ctrl_dtc)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Enable dtc operation */
    err = R_DTC_Enable(p_api_ctrl_dtc);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_DTC_Enable FAILED ** \r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This functions closes DTC instance.
 * @param[IN]   p_api_ctrl_dtc              DTC control instance
 * @retval      FSP_SUCCESS                 Upon successful close operation.
 * @retval      Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
void deinit_hal_dtc(transfer_ctrl_t * const p_api_ctrl_dtc)
{
    fsp_err_t err = FSP_SUCCESS;
    /*Close opened DTC instances */
    err = R_DTC_Close(p_api_ctrl_dtc);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_DTC_Close FAILED ** \r\n");
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup dtc_hal.c)
 **********************************************************************************************************************/
