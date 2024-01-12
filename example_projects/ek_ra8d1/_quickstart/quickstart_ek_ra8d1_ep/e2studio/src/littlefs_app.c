/***********************************************************************************************************************
 * File Name    : littlefs_app.c
 * Description  : This file contains the User functions to configure the littlefs module
 ***********************************************************************************************************************/

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
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
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

/**********************************************************************************************************************
 * Function Name: hal_littlefs_init
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
fsp_err_t hal_littlefs_init (void)
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

/**********************************************************************************************************************
 * End of function hal_littlefs_init
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: configure_littlefs_flash
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
fsp_err_t configure_littlefs_flash (void)
{
    fsp_err_t err = FSP_SUCCESS;
    err = lfs_format(&g_rm_littlefs0_lfs, &g_rm_littlefs0_lfs_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("** Failed in lfs_format API ** \r\n");
        hal_littlefs_deinit();

        return err;
    }

    err = lfs_mount(&g_rm_littlefs0_lfs, &g_rm_littlefs0_lfs_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("** Failed in lfs_mount API ** \r\n");
        hal_littlefs_deinit();

        return err;
    }

    return err;
}

/**********************************************************************************************************************
 * End of function configure_littlefs_flash
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: hal_littlefs_deinit
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void hal_littlefs_deinit (void)
{
    fsp_err_t err = FSP_SUCCESS;
    err = RM_LITTLEFS_FLASH_Close(&g_rm_littlefs0_ctrl);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT(" ** Failed in RM_LITTLEFS_FLASH_Close API ** \r\n ");
    }
}

/**********************************************************************************************************************
 * End of function hal_littlefs_deinit
 *********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @} (end defgroup aws_https_client_ep)
 **********************************************************************************************************************/
