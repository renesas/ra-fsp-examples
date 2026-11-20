/***********************************************************************************************************************
 * File Name    : sdhi_ep.c
 * Description  : Contains data structures and functions used in sdhi_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2022 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "sdhi_ep.h"

/*******************************************************************************************************************//**
 * @brief     This function opens FreeRTOS+FAT.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Opened successfully.
 * @retval    Any other error codes     Failed to open.
 **********************************************************************************************************************/
fsp_err_t fat_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = RM_FREERTOS_PLUS_FAT_Open(&g_rm_freertos_plus_fat_ctrl, &g_rm_freertos_plus_fat_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function initializes a FreeRTOS+FAT disk structure.
 * @param[in] p_disk                    Pointer to the FreeRTOS+FAT disk structure.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t fat_disk_init(FF_Disk_t * const p_disk)
{
    fsp_err_t err = FSP_SUCCESS;

    err = RM_FREERTOS_PLUS_FAT_DiskInit(&g_rm_freertos_plus_fat_ctrl, &g_rm_freertos_plus_fat_disk_cfg, p_disk);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function de-initializes a FreeRTOS+FAT disk structure.
 * @param[in] p_disk                    Pointer to the FreeRTOS+FAT disk structure.
 * @retval    FSP_SUCCESS               De-initialized successfully.
 * @retval    Any other error codes     Failed to de-initialize.
 **********************************************************************************************************************/
fsp_err_t fat_disk_deinit(FF_Disk_t * const p_disk)
{
    fsp_err_t err = FSP_SUCCESS;

    err = RM_FREERTOS_PLUS_FAT_DiskDeinit(&g_rm_freertos_plus_fat_ctrl, p_disk);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function initializes the media device.
 * @param[in] p_device                  Pointer to the FreeRTOS+FAT media device.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t fat_media_init(rm_freertos_plus_fat_device_t * const p_device)
{
    fsp_err_t err = FSP_SUCCESS;

    err = RM_FREERTOS_PLUS_FAT_MediaInit(&g_rm_freertos_plus_fat_ctrl, p_device);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function closes the opened FreeRTOS+FAT.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void fat_deinit(void)
{
    /* Close the opened FreeRTOS+FAT */
    if (g_rm_freertos_plus_fat_ctrl.open)
    {
        RM_FREERTOS_PLUS_FAT_Close(&g_rm_freertos_plus_fat_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function is a callback for FreeRTOS+FAT.
 * @param[in] p_args                    Pointer to the callback argument.
 * @retval    None.
 **********************************************************************************************************************/
void free_rtos_fat_callback(rm_freertos_plus_fat_callback_args_t *p_args)
{
    /* Set the event bit from ISR context */
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xEventGroupSetBitsFromISR(g_SDHI_EventGroupHandle, p_args->event, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
