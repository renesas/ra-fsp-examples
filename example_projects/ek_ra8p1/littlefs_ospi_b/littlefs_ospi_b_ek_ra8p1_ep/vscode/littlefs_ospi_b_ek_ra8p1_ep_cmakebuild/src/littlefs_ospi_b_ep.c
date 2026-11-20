/***********************************************************************************************************************
 * File Name    : littlefs_ospi_ep.c
 * Description  : Contains data structures and functions used in littlefs_ospi_ep.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "littlefs_ospi_b_ep.h"

/***********************************************************************************************************************
 * Function Name: littlefs_ospi_b_init
 * Description  : This function initializes LittleFS for OSPI.
 * Arguments    : None.
 * Return Value : LFS_ERR_OK                Initialized successfully.
 *                Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
int32_t littlefs_ospi_b_init(void)
{
    fsp_err_t err = FSP_SUCCESS;
    int32_t lfs_err = LFS_ERR_OK;

    /* Open LittleFS SPI Flash */
    err = RM_LITTLEFS_SPI_FLASH_Open(&g_rm_littlefs_ctrl, &g_rm_littlefs_cfg);
    if (FSP_SUCCESS != err)
    {
        return (int32_t)err;
    }

    /* Mount the file system */
    lfs_err = lfs_mount(&g_rm_littlefs_lfs, &g_rm_littlefs_lfs_cfg);
    if (LFS_ERR_OK != lfs_err)
    {
        /* File system was not formatted or is corrupt, format now */
        lfs_err = lfs_format(&g_rm_littlefs_lfs, &g_rm_littlefs_lfs_cfg);
        if (LFS_ERR_OK != lfs_err)
        {
            return lfs_err;
        }

        /* Mount the file system again */
        lfs_err = lfs_mount(&g_rm_littlefs_lfs, &g_rm_littlefs_lfs_cfg);
        if (LFS_ERR_OK != lfs_err)
        {
            return lfs_err;
        }
    }

    return lfs_err;
}
/***********************************************************************************************************************
* End of function littlefs_ospi_b_init
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: littlefs_ospi_b_deinit
 * Description  : This function de-initializes the opened LittleFS SPI Flash module.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
void littlefs_ospi_b_deinit(void)
{
    /* De-initialize the opened LittleFS SPI Flash module */
    if (g_rm_littlefs_ctrl.open)
    {
        lfs_unmount(&g_rm_littlefs_lfs);
        RM_LITTLEFS_SPI_FLASH_Close(&g_rm_littlefs_ctrl);
    }
}
/***********************************************************************************************************************
* End of function littlefs_ospi_b_deinit
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: g_rm_littlefs_spi_flash_callback
 * Description  : This function is a callback for LittleFS SPI Flash module.
 * Arguments    : p_args                    Pointer to the callback argument.
 * Return Value : None.
 **********************************************************************************************************************/
void g_rm_littlefs_spi_flash_callback(rm_littlefs_spi_flash_callback_args_t *p_args)
{
#ifdef BSP_CFG_DCACHE_ENABLED
    /* Invalidate the data cache */
    SCB_InvalidateDCache();
#endif /* BSP_CFG_DCACHE_ENABLED */

    if (p_args->event == RM_LITTLEFS_SPI_FLASH_EVENT_BUSY)
    {
        /* Do nothing */
    }
}
/***********************************************************************************************************************
* End of function g_rm_littlefs_spi_flash_callback
***********************************************************************************************************************/
