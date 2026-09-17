/***********************************************************************************************************************
 * File Name    : dtc_hal.c
 * Description  : Contains function definition.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "dtc_hal.h"

/*******************************************************************************************************************//**
 * @addtogroup dtc_hal.c
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * @brief       Initialize DTC module instances.
 * @param[in]   p_api_ctrl_dtc     Pointer to the DTC instance control structure.
 * @param[in]   p_cfg_dtc          Pointer to the DTC instance configure structure.
 * @retval      FSP_SUCCESS        DTC instance opened successfully.
 * @retval      Other error code if opening the DTC instance failed.
 **********************************************************************************************************************/
fsp_err_t init_hal_dtc(transfer_ctrl_t * const p_api_ctrl_dtc, transfer_cfg_t const * const p_cfg_dtc)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Open DTC instance */
    err = R_DTC_Open(p_api_ctrl_dtc, p_cfg_dtc);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_DTC_Open FAILED **\r\n");
    }
    return err;
}

/***********************************************************************************************************************
 * @brief       Reconfigure DTC transfer and enable it.
 * @param[in]   p_api_ctrl_dtc      Pointer to the DTC instance control structure.
 * @param[in]   p_info              Pointer to transfer info structure used for reconfiguration.
 * @retval      FSP_SUCCESS         On successful DTC re-configuration.
 * @retval      Other error code if reconfiguring the DTC instance failed.
 **********************************************************************************************************************/
fsp_err_t dtc_hal_reconfigure(transfer_ctrl_t * const p_api_ctrl_dtc, transfer_info_t * p_info)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Reconfigure DTC instance using the provided transfer info */
    err = R_DTC_Reconfigure(p_api_ctrl_dtc, p_info);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_DTC_Reconfigure FAILED **\r\n");
    }
    return err;
}

/***********************************************************************************************************************
 * @brief       Enable the DTC transfer for the specified activation source.
 * @param[in]   p_api_ctrl_dtc      Pointer to the DTC instance control structure.
 * @retval      FSP_SUCCESS         DTC instance enabled successfully for activation source.
 * @retval      Other error code if enabling the ADC instance failed.
 **********************************************************************************************************************/
fsp_err_t dtc_enable(transfer_ctrl_t * const p_api_ctrl_dtc)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Enable DTC transfer operation */
    err = R_DTC_Enable(p_api_ctrl_dtc);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_DTC_Enable FAILED **\r\n");
    }
    return err;
}

/***********************************************************************************************************************
 * @brief       Close the DTC instance.
 * @param[in]   p_api_ctrl_dtc      Pointer to the DTC control instance.
 * @retval      FSP_SUCCESS         Upon successful close operation.
 * @retval      Other error code if closing the DTC instance failed.
 **********************************************************************************************************************/
void deinit_hal_dtc(transfer_ctrl_t * const p_api_ctrl_dtc)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Close opened DTC instance */
    err = R_DTC_Close(p_api_ctrl_dtc);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_DTC_Close FAILED **\r\n");
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup dtc_hal.c)
 **********************************************************************************************************************/
