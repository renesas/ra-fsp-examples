/***********************************************************************************************************************
 * File Name    : usb_hmsc_ep.c
 * Description  : Contains data structures and functions used in usb_hmsc_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <string.h>
#include "usb_hmsc_ep.h"

/* Global variables */
FF_Disk_t g_my_disk;
bool g_usb_hmsc_opened = false;
volatile bool g_media_inserted = false;

/* Private variables */
static bool s_fat_opened = false;
static bool s_disk_initialized = false;

/*******************************************************************************************************************//**
 * @brief     This function opens the FreeRTOS+FAT instance.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Opened successfully.
 * @retval    Any other error codes     Failed to open.
 **********************************************************************************************************************/
fsp_err_t fat_open(void)
{
    fsp_err_t err = FSP_SUCCESS;

    memset(&g_my_disk, RESET_VALUE, sizeof(g_my_disk));

    err = RM_FREERTOS_PLUS_FAT_Open(&g_rm_freertos_plus_fat_ctrl, &g_rm_freertos_plus_fat_cfg);
    if (FSP_SUCCESS == err)
    {
        s_fat_opened = true;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function initializes the FreeRTOS+FAT disk and adds it to the file system.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t fat_init(void)
{
    fsp_err_t err = FSP_SUCCESS;
    FF_Error_t ff_err = FF_ERR_NONE;
    rm_freertos_plus_fat_device_t device;

    memset(&device, RESET_VALUE, sizeof(device));

    /* Initialize the mass storage device */
    err = RM_FREERTOS_PLUS_FAT_MediaInit(&g_rm_freertos_plus_fat_ctrl, &device);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Initialize the disk */
    err = RM_FREERTOS_PLUS_FAT_DiskInit(&g_rm_freertos_plus_fat_ctrl, &g_rm_freertos_plus_fat_disk_cfg,
                                        &g_my_disk);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    s_disk_initialized = true;

    /* Mount the disk */
    ff_err = FF_Mount(&g_my_disk, g_my_disk.xStatus.bPartitionNumber);
    if (FF_ERR_NONE != ff_err)
    {
        return FSP_ERR_ABORTED;
    }

    /* Add the disk to the file system */
    ff_err = FF_FS_Add("/", &g_my_disk);
    if (FF_FS_ADD_FAILURE == ff_err)
    {
        return FSP_ERR_ABORTED;
    }

    /* Set the USB HMSC opened flag */
    g_usb_hmsc_opened = true;

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function de-initializes the FreeRTOS+FAT instance.
 * @param[in] None.
 * @retval    FSP_SUCCESS               De-initialized successfully.
 * @retval    Any other error codes     Failed to de-initialize.
 **********************************************************************************************************************/
fsp_err_t fat_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    if (s_fat_opened)
    {
        err = RM_FREERTOS_PLUS_FAT_Close(&g_rm_freertos_plus_fat_ctrl);
        if (FSP_SUCCESS == err)
        {
            s_fat_opened = false;
        }
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function de-initializes the FreeRTOS+FAT disk.
 * @param[in] None.
 * @retval    FSP_SUCCESS               De-initialized successfully.
 * @retval    Any other error codes     Failed to de-initialize.
 **********************************************************************************************************************/
fsp_err_t disk_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    if (s_disk_initialized)
    {
        err = RM_FREERTOS_PLUS_FAT_DiskDeinit(&g_rm_freertos_plus_fat_ctrl, &g_my_disk);
        if (FSP_SUCCESS == err)
        {
            s_disk_initialized = false;
        }
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function is a callback for FreeRTOS+FAT module.
 * @param[in] p_args                    Callback function parameter data.
 * @retval    None.
 **********************************************************************************************************************/
void free_rtos_fat_callback(rm_freertos_plus_fat_callback_args_t *p_args)
{
    if (p_args->event & RM_FREERTOS_PLUS_FAT_EVENT_MEDIA_INSERTED)
    {
        g_media_inserted = true;
    }
    if (p_args->event & RM_FREERTOS_PLUS_FAT_EVENT_MEDIA_REMOVED)
    {
        g_media_inserted = false;
    }
}
