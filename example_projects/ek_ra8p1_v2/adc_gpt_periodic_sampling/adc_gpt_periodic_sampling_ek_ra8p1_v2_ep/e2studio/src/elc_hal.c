/***********************************************************************************************************************
 * File Name    : elc_hal.c
 * Description  : Contains function definition.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "elc_hal.h"

/*******************************************************************************************************************//**
 * @addtogroup elc_hal.c
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * @brief       Initialize ELC module instance.
 * @param[in]   p_ctrl_instance_elc     Pointer to the ELC instance control structure.
 * @param[in]   p_cfg_instance_elc      Pointer to the ELC instance configure structure.
 * @retval      FSP_SUCCESS             ELC instance opened successfully.
 * @retval      Other error code if opening the ELC instance failed.
***********************************************************************************************************************/
fsp_err_t init_hal_elc(elc_ctrl_t * const p_ctrl_instance_elc, elc_cfg_t const * const p_cfg_instance_elc)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Open ELC instance */
    err = R_ELC_Open(p_ctrl_instance_elc, p_cfg_instance_elc);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_ELC_Open FAILED **\r\n");
    }
    return err;
}

/***********************************************************************************************************************
 * @brief       Enable ELC module instance.
 * @param[in]   p_ctrl_instance_elc     Pointer to the ELC instance control structure.
 * @retval      FSP_SUCCESS             ELC instance enabled successfully.
 * @retval      Other error code if enabling the ELC instance failed.
***********************************************************************************************************************/
fsp_err_t elc_enable(elc_ctrl_t * const p_ctrl_instance_elc)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Enable the operation of the Event Link Controller */
    err = R_ELC_Enable(p_ctrl_instance_elc);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_ELC_Enable FAILED **\r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       Close ELC instance.
 * @param[in]   p_ctrl_instance_elc     Pointer to the ELC instance control structure.
 * @retval      FSP_SUCCESS             Upon successful close operation.
 * @retval      Other error code if closing the ELC instance failed.
 **********************************************************************************************************************/
void deinit_hal_elc(elc_ctrl_t * const p_ctrl_instance_elc)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Close opened ELC instance */
    err = R_ELC_Close(p_ctrl_instance_elc);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_ELC_Close FAILED **\r\n");
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup elc_hal.c)
 **********************************************************************************************************************/
