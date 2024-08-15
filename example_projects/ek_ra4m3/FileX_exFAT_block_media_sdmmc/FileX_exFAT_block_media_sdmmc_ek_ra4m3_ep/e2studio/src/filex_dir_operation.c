/***********************************************************************************************************************
 * File Name    : filex_dir_operation.c
 * Description  : Contains data structures and functions used in FileX thread.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "filex_dir_operation.h"
#include "filex_media_operation.h"

/* Private global variable */
static CHAR g_dir_name1 [] = DIR_NAME_ONE;

/* Functions implementation */

/*******************************************************************************************************************//**
 * @brief       This function creates a directory.
 * @param[in]   None
 * @retval      FX_SUCCESS   Upon successful operation
 * @retval      Any Other Error code apart from FX_SUCCESS
 **********************************************************************************************************************/
UINT dir_create(void)
{
    UINT status = FX_SUCCESS;
    entry_info_t entry = {RESET_VALUE};

    /* Verify the current status of the media */
    status = media_verify();
    if (FX_SUCCESS != status)
    {
        return FX_SUCCESS;
    }

    /* Create directory using Azure FileX API */
    status = fx_directory_create (&g_fx_media, g_dir_name1);

    if (FX_ALREADY_CREATED == status)
    {
        PRINT_INFO_STR("Directory already exists\r\n");
        return FX_SUCCESS;
    }

    if (FX_SUCCESS != status)
    {
        RETURN_ERR_STR(status, "fx_directory_create failed\r\n");
    }

    /* Flushes data into the physical media */
    status = fx_media_flush(&g_fx_media);
    RETURN_ERR_STR(status, "fx_media_flush failed\r\n");

    /* Get directory name */
    memcpy(entry.name, g_dir_name1, strlen(g_dir_name1) + ONE_BYTE);

    /* Get directory full information */
    status = fx_directory_information_get(&g_fx_media,
                                          entry.name, &entry.attr, &entry.size,
                                          &entry.time.year, &entry.time.month, &entry.time.date,
                                          &entry.time.hour, &entry.time.min, &entry.time.sec);
    RETURN_ERR_STR(status, "fx_directory_information_get failed\r\n");

    /* Display directory information */
    PRINT_ENTRY_INFO(entry);

    /* Create a directory successful */
    PRINT_INFO_STR("\r\nDirectory created successful\r\n");

    return FX_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief       This function retrieves root directory properties.
 * @param[in]   None
 * @retval      FX_SUCCESS   Upon successful operation
 * @retval      Any Other Error code apart from FX_SUCCESS
 **********************************************************************************************************************/
UINT dir_get_property(void)
{
    UINT status = FX_SUCCESS;
    dir_property_t dir = {RESET_VALUE};
    volume_info_t volume = {RESET_VALUE};
    entry_info_t entry = {RESET_VALUE};

    /* Verify the current status of the media */
    status = media_verify();
    if (FX_SUCCESS != status)
    {
        return FX_SUCCESS;
    }

    /* Get media volume information */
    volume.number = g_fx_media.fx_media_exfat_volume_serial_number;
    status = fx_media_volume_get(&g_fx_media, volume.lable, FX_DIRECTORY_SECTOR);
    RETURN_ERR_STR(status, "fx_media_volume_get failed\r\n");

    /* Print media volume information */
    PRINT_VOLUME_INFO(volume);

    /* Extract first entry details in the current directory */
    status = fx_directory_first_full_entry_find(&g_fx_media,
                                               entry.name, &entry.attr, &entry.size,
                                               &entry.time.year, &entry.time.month, &entry.time.date,
                                               &entry.time.hour, &entry.time.min, &entry.time.sec);
    /* Return in the case of a local directory that is empty. */
    if (FX_NO_MORE_ENTRIES == status)
    {
        PRINT_INFO_STR("Directory empty\r\n");
        return FX_SUCCESS;
    }

    /* Return fails in other failed cases */
    if (FX_SUCCESS != status)
    {
        RETURN_ERR_STR(status, "fx_directory_first_full_entry_find failed\r\n");
    }

    /* Check all entries in root directory */
    do
    {
        /* In case the entry is a directory */
        if(FX_DIRECTORY == (entry.attr & FX_DIRECTORY))
        {
            /* Print entry info */
            PRINT_ENTRY_INFO(entry);
            /* Count number of directories */
            dir.subdir ++;
        }

        /* In case the entry is a file */
        else if (FX_ARCHIVE == (entry.attr & FX_ARCHIVE))
        {
            /* Print entry info */
            PRINT_ENTRY_INFO(entry);
            /* Calculate total files size */
            dir.file_size += (ULONG64)entry.size;
            /* Count number of files */
            dir.file ++;
        }

        /* In other cases */
        else
        {
           /* do not thing */
        }

        /* Extract details of next entry in the local directory */
        status = fx_directory_next_full_entry_find(&g_fx_media,
                                                  entry.name, &entry.attr, &entry.size,
                                                  &entry.time.year, &entry.time.month, &entry.time.date,
                                                  &entry.time.hour, &entry.time.min, &entry.time.sec);
        if ((FX_SUCCESS != status) && (FX_NO_MORE_ENTRIES != status))
        {
            RETURN_ERR_STR(status, "fx_directory_next_full_entry_find failed\r\n");
        }
    }
    while (FX_NO_MORE_ENTRIES != status);

    /* Print number of file and total file size */
    PRINT_DIR_PROPERTY(dir);

    return FX_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief       This function deletes a directory.
 * @param[in]   None
 * @retval      FX_SUCCESS   Upon successful operation
 * @retval      Any Other Error code apart from FX_SUCCESS
 **********************************************************************************************************************/
UINT dir_delete(void)
{
    UINT status = FX_SUCCESS;

    /* Verify the current status of the media */
    status = media_verify();
    if (FX_SUCCESS != status)
    {
        return FX_SUCCESS;
    }

    /* Delete a directory using Azure FileX API */
    status = fx_directory_delete(&g_fx_media, g_dir_name1);

    if (FX_NOT_FOUND == status)
    {
        PRINT_INFO_STR("Directory not exists\r\n");
        return FX_SUCCESS;
    }

    if (FX_DIR_NOT_EMPTY == status)
    {
        PRINT_INFO_STR("Directory not empty\r\n");
        return FX_SUCCESS;
    }

    if (FX_SUCCESS != status)
    {
        RETURN_ERR_STR(status, "fx_directory_delete failed\r\n");
    }

    /* Flush data to physical media */
    status = (ULONG)fx_media_flush(&g_fx_media);
    RETURN_ERR_STR(status, "fx_media_flush failed\r\n");

    /* Delete a directory successful */
    PRINT_INFO_STR("Directory has been deleted\r\n");

    return FX_SUCCESS;
}
