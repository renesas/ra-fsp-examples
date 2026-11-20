/***********************************************************************************************************************
 * File Name    : pqc_ep.c
 * Description  : Contains data structures and functions used in pqc_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "pqc_ep.h"

/***********************************************************************************************************************
 * Function Name: littlefs_init.
 * Description  : This function initializes LittleFS module.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Initialized successfully.
 *                Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t littlefs_init(void)
{
    fsp_err_t err = FSP_SUCCESS;
    int lfs_err = RESET_VALUE;

    /* Open LittleFS SPI Flash port */
    err = RM_LITTLEFS_SPI_FLASH_Open(&g_rm_littlefs0_ctrl, &g_rm_littlefs0_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Format the file system */
    lfs_err = lfs_format(&g_rm_littlefs0_lfs, &g_rm_littlefs0_lfs_cfg);
    if (RESET_VALUE != lfs_err)
    {
        return (fsp_err_t)lfs_err;
    }

    /* Mount the file system */
    lfs_err = lfs_mount(&g_rm_littlefs0_lfs, &g_rm_littlefs0_lfs_cfg);

    return (fsp_err_t)lfs_err;
}
/***********************************************************************************************************************
 * End of function littlefs_init.
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: littlefs_deinit.
 * Description  : This function de-initializes the opened LittleFS module.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
void littlefs_deinit(void)
{
    if (g_rm_littlefs0_ctrl.open)
    {
        RM_LITTLEFS_SPI_FLASH_Close(&g_rm_littlefs0_ctrl);
    }
}
/***********************************************************************************************************************
 * End of function littlefs_deinit.
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: cleanup_keys.
 * Description  : This function cleans entire key list.
 * Arguments    : handles                   Pointer to key list.
 *                count                     Key quantity.
 * Return Value : PSA_SUCCESS               Cleaned successfully.
 *                Any other error codes     Failed to clean.
 **********************************************************************************************************************/
psa_status_t cleanup_keys(psa_key_id_t * handles, size_t count)
{
    psa_status_t status = PSA_SUCCESS;

    for (size_t i = 0; i < count; i++)
    {
        if (0 != handles[i])
        {
            status = psa_destroy_key(handles[i]);
            if (PSA_SUCCESS != status)
            {
                return status;
            }

            handles[i] = 0;
        }
    }

    return status;
}
/***********************************************************************************************************************
 * End of function cleanup_keys.
 **********************************************************************************************************************/
