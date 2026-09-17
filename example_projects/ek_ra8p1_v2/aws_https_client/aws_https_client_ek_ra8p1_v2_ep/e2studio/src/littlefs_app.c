/***********************************************************************************************************************
 * File Name    : littlefs_app.c
 * Description  : This file contains the user functions to configure the LittleFS module.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2022 - 2026 Renesas Electronics Corporation and/or its affiliates
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
 * @brief      Initializes the LittleFS module by opening, formating and mounting.
 * @param[in]  None.
 * @retval     FSP_SUCCESS                  Upon successful LittlefS initialization.
 * @retval     Any other error code         Upon unsuccessful LittlefS initialization.
 **********************************************************************************************************************/
fsp_err_t hal_littlefs_init(void)
{
    fsp_err_t err     = FSP_SUCCESS;
    int       lfs_err = RESET_VALUE;

#if (BSP_PERIPHERAL_FLASH_PRESENT)
    err = RM_LITTLEFS_FLASH_Open(&g_rm_littlefs0_ctrl, &g_rm_littlefs0_cfg);
#else
    err = RM_LITTLEFS_SPI_FLASH_Open(&g_rm_littlefs0_ctrl, &g_rm_littlefs0_cfg);
#endif /* BSP_PERIPHERAL_FLASH_PRESENT */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("** Failed in RM_LITTLEFS_"FLASH_TYPE"_Open API **\r\n");
        return err;
    }

    /* Configure LittleFS flash */
    lfs_err = configure_littlefs_flash();
    if (RESET_VALUE != lfs_err)
    {
        APP_ERR_PRINT("** Failed in configure_littlefs_flash function **\r\n");
        hal_littlefs_deinit();
        return (fsp_err_t) err;
    }

    APP_PRINT("\r\nLittleFS flash module initialized successfully");
    return err;
}

/*******************************************************************************************************************//**
 * @brief      Configures the LittleFS module by formating and mounting.
 * @param[in]  None.
 * @retval     RESET_VALUE                  Upon successful LittlefS configuration.
 * @retval     Any other error code         Upon unsuccessful LittlefS configuration.
 **********************************************************************************************************************/
int configure_littlefs_flash(void)
{
    int lfs_err = RESET_VALUE;

    lfs_err = lfs_format(&g_rm_littlefs0_lfs, &g_rm_littlefs0_lfs_cfg);
    if (RESET_VALUE != lfs_err)
    {
        APP_ERR_PRINT("** Failed in lfs_format API **\r\n");
        hal_littlefs_deinit();
        return lfs_err;
    }

    lfs_err = lfs_mount(&g_rm_littlefs0_lfs, &g_rm_littlefs0_lfs_cfg);
    if (RESET_VALUE != lfs_err)
    {
        APP_ERR_PRINT("** Failed in lfs_mount API **\r\n");
        hal_littlefs_deinit();
        return lfs_err;
    }
    return lfs_err;
}

/*******************************************************************************************************************//**
 * @brief      De-initializes the LittleFS module.
 * @param[in]  None.
 * @retval     None.
 **********************************************************************************************************************/
void hal_littlefs_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

#if (BSP_PERIPHERAL_FLASH_PRESENT)
    err = RM_LITTLEFS_FLASH_Close(&g_rm_littlefs0_ctrl);
#else
    err = RM_LITTLEFS_SPI_FLASH_Close(&g_rm_littlefs0_ctrl);
#endif /* BSP_PERIPHERAL_FLASH_PRESENT */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("** Failed in RM_LITTLEFS_"FLASH_TYPE"_Close API **\r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief      LittleFS SPI flash callback function.
 * @param[in]  p_args       Callback argument.
 * @retval     None.
 **********************************************************************************************************************/
void g_rm_littlefs_spi_flash0_callback(rm_littlefs_spi_flash_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);
    /* Do nothing */
}

/*******************************************************************************************************************//**
 * @} (end defgroup aws_https_client_ep)
 **********************************************************************************************************************/
