/***********************************************************************************************************************
 * File Name    : elc_hal.c
 * Description  : Contains function definition.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 - 2025 Renesas Electronics Corporation and/or its affiliates
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
 * @brief       Initializes ELC module instances.
 * @param[IN]   p_ctrl_instance_elc     ELC instance control structure.
 * @param[IN]   p_cfg_instance_elc      ELC instance configure structure.
 * @retval      FSP_SUCCESS             ELC instance opened successfully.
 * @retval      Any other error code apart from FSP_SUCCESS on Unsuccessful opening of ELC instance.
***********************************************************************************************************************/
fsp_err_t init_hal_elc(elc_ctrl_t * const p_ctrl_instance_elc, elc_cfg_t const * const p_cfg_instance_elc)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Open ELC instance */
    err = R_ELC_Open(p_ctrl_instance_elc, p_cfg_instance_elc);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_ELC_Open FAILED **\r\n");
    }
    return err;
}

/***********************************************************************************************************************
 * @brief       Enable ELC module instances.
 * @param[IN]   p_ctrl_instance_elc     ELC instance control structure.
 * @retval      FSP_SUCCESS             ELC instance enabled successfully.
 * @retval      Any other error code apart from FSP_SUCCESS on Unsuccessful enable of ELC instance.
***********************************************************************************************************************/
fsp_err_t elc_enable(elc_ctrl_t * const p_ctrl_instance_elc)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Enable the operation of the Event Link Controller */
    err = R_ELC_Enable(p_ctrl_instance_elc);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_ELC_Enable FAILED **\r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function closes ELC instance.
 * @param[IN]   p_ctrl_instance_elc     ELC control instance.
 * @retval      FSP_SUCCESS             Upon successful close operation.
 * @retval      Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
void deinit_hal_elc(elc_ctrl_t * const p_ctrl_instance_elc)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Close opened ELC instance */
    err = R_ELC_Close(p_ctrl_instance_elc);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_ELC_Close FAILED **\r\n");
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup elc_hal.c)
 **********************************************************************************************************************/
