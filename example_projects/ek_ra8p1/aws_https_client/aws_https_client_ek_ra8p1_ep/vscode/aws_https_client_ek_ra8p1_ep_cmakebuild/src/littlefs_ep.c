/***********************************************************************************************************************
 * File Name    : littlefs_ep.c
 * Description  : Contains data structures and functions used in littlefs_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2022 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "littlefs_ep.h"

/* Private function */
static int configure_littlefs_flash(void);

/*******************************************************************************************************************//**
 * @brief     This function initializes the LittleFS module.
 * @param[in] None.
 * @retval    FSP_SUCCESS              Initialized successfully.
 * @retval    Any other error codes    Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t littlefs_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = LITTLEFS_OPEN(&g_rm_littlefs0_ctrl, &g_rm_littlefs0_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Format and mount the LittleFS flash */
    if (0 != configure_littlefs_flash())
    {
        return FSP_ERR_INTERNAL;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function closes the LittleFS module if it is opened.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void littlefs_deinit(void)
{
    /* Close the opened LittleFS module */
    if (g_rm_littlefs0_ctrl.open)
    {
        LITTLEFS_CLOSE(&g_rm_littlefs0_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function formats and mounts the LittleFS flash.
 * @param[in] None.
 * @retval    0                        Formatted and mounted successfully.
 * @retval    Any other error codes    Failed to format or mount.
 **********************************************************************************************************************/
static int configure_littlefs_flash(void)
{
    int lfs_err = lfs_format(&g_rm_littlefs0_lfs, &g_rm_littlefs0_lfs_cfg);
    if (0 != lfs_err)
    {
        return lfs_err;
    }

    return lfs_mount(&g_rm_littlefs0_lfs, &g_rm_littlefs0_lfs_cfg);
}

/*******************************************************************************************************************//**
 * @brief     This function is the LittleFS SPI flash callback required by the stack configuration.
 * @param[in] p_args                    Pointer to the callback arguments.
 * @retval    None.
 **********************************************************************************************************************/
void g_rm_littlefs_spi_flash0_callback(rm_littlefs_spi_flash_callback_args_t * p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);
    /* Do nothing */
}
