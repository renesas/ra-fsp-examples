/***********************************************************************************************************************
 * File Name    : littlefs_app.c
 * Description  : This file contains the User functions to configure the littlefs module
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2022 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "littlefs_app.h"

/*******************************************************************************************************************//**
 * @addtogroup aws_https_client_ep
 * @{
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief      Initializes the Littlefs module by opening, formating and mounting
 *
 * @param[in]  None
 * @retval     FSP_SUCCESS                  Upon successful LittlefS Initialization.
 * @retval     Any other Error Code         Upon unsuccesful LittlefS Initialization.
 **********************************************************************************************************************/
fsp_err_t hal_littlefs_init(void)
{
    fsp_err_t err = FSP_SUCCESS;
    err = RM_LITTLEFS_FLASH_Open(&g_rm_littlefs0_ctrl, &g_rm_littlefs0_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("** Failed in RM_LITTLEFS_FLASH_Open API ** \r\n");
        return err;
    }
    /* configure littlfs flash */
    err = configure_littlefs_flash();
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("** Failed in configure_littlefs_flash function ** \r\n");
        hal_littlefs_deinit();
        return err;
    }

    APP_PRINT("\r\n LittlFS flash module initialization successful");
    return err;
}

fsp_err_t configure_littlefs_flash(void)
{
    fsp_err_t err = FSP_SUCCESS;
    err = lfs_format (&g_rm_littlefs0_lfs, &g_rm_littlefs0_lfs_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("** Failed in lfs_format API ** \r\n");
        hal_littlefs_deinit();
        return err;
    }

    err = lfs_mount (&g_rm_littlefs0_lfs, &g_rm_littlefs0_lfs_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("** Failed in lfs_mount API ** \r\n");
        hal_littlefs_deinit();
        return err;
    }
    return err;
}

void hal_littlefs_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;
    err = RM_LITTLEFS_FLASH_Close(&g_rm_littlefs0_ctrl);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT(" ** Failed in RM_LITTLEFS_FLASH_Close API ** \r\n ");
    }
}
/*******************************************************************************************************************//**
 * @} (end defgroup aws_https_client_ep)
 **********************************************************************************************************************/
