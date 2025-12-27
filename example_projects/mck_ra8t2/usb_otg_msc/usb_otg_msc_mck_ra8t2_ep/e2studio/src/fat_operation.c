/***********************************************************************************************************************
* File Name    : fat_operation.c
* Description  : Contains functions for managing FAT file system operations, including disk initialization, mounting,
*                un-mounting, and file handling.
***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "fat_operation.h"
#include "ff_time.h"
#include "common_app.h"

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * @brief      Initializes a FAT disk instance.
 * @param[in]  p_disk               Pointer to the FAT disk structure.
 * @param[in]  p_ff_fs_path         File system path for the disk.
 * @param[in]  required_open_rm_fat Flag indicating whether RM FAT initialization is required.
 * @param[in]  p_rm_fat_instance    Pointer to the RM FAT instance configuration.
 * @param[in]  p_rm_fat_disk_cfg    Pointer to the RM FAT disk configuration.
 * @retval     FSP_SUCCESS on successful operation, other error codes otherwise.
 **********************************************************************************************************************/
fsp_err_t fat_disk_create(fat_disk_t * p_disk, char * p_ff_fs_path, bool required_open_rm_fat,
                          rm_freertos_plus_fat_instance_t const * p_rm_fat_instance,
                          rm_freertos_plus_fat_disk_cfg_t * p_rm_fat_disk_cfg)
{
    /* Check if any of the provided pointers is NULL to avoid dereferencing invalid memory */
    if (NULL == p_disk || NULL == p_ff_fs_path || NULL == p_rm_fat_instance || NULL == p_rm_fat_disk_cfg)
    {
        return FSP_ERR_INVALID_POINTER;
    }

    /* Assign the RM FAT instance to the disk structure */
    p_disk->p_rm_fat_instance = p_rm_fat_instance;

    /* Assign the RM FAT disk configuration to the disk structure */
    p_disk->p_rm_fat_disk_cfg = p_rm_fat_disk_cfg;

    /* Assign the file system path to the disk structure */
    p_disk->p_ff_fs_path = p_ff_fs_path;

    /* Set the flag indicating whether RM FAT initialization is required */
    p_disk->required_open_rm_fat = required_open_rm_fat;

    /* Mark the disk as not open by default */
    p_disk->is_open = false;

    return FSP_SUCCESS;
}

/***********************************************************************************************************************
 * @brief      Opens a FAT disk.
 * @param[in]  p_disk  Pointer to the FAT disk structure.
 * @retval     FSP_SUCCESS on successful operation, other error codes otherwise.
 **********************************************************************************************************************/
fsp_err_t fat_disk_open(fat_disk_t * p_disk)
{
    fsp_err_t status = FSP_SUCCESS;

    /* Check if the provided disk pointer is NULL to prevent invalid memory access */
    if (NULL == p_disk)
    {
        return FSP_ERR_INVALID_POINTER;
    }

    /* Check if the disk is already open */
    if (true == p_disk->is_open)
    {
        return FSP_SUCCESS;
    }

    /* If RM FAT initialization is required, proceed to open the RM FAT instance */
    if (true == p_disk->required_open_rm_fat)
    {
        /* Call the RM_FREERTOS_PLUS_FAT_Open API to initialize the RM FAT instance */
        status = RM_FREERTOS_PLUS_FAT_Open(p_disk->p_rm_fat_instance->p_ctrl, p_disk->p_rm_fat_instance->p_cfg);

        /* If the operation fails, log an error message and return the error code */
        TERM_ERR_RET(FSP_SUCCESS != status, status, "RM_FREERTOS_PLUS_FAT_Open failed\r\n");
    }

    /* Mark the disk as open to indicate it is now ready for operations */
    p_disk->is_open = true;

    return status;
}

/***********************************************************************************************************************
 * @brief      Mounts a FAT disk.
 * @param[in]  p_disk  Pointer to the FAT disk structure.
 * @retval     FSP_SUCCESS on successful operation, other error codes otherwise.
 **********************************************************************************************************************/
fsp_err_t fat_disk_mount(fat_disk_t * p_disk)
{
    fsp_err_t status = FSP_SUCCESS;
    FF_Error_t ff_status = FF_ERR_NONE;
    BaseType_t fs_status = pdTRUE;

    /* Validate that the provided disk pointer is not NULL to prevent invalid memory access */
    if (NULL == p_disk)
    {
        return FSP_ERR_INVALID_POINTER;
    }

    /* Ensure the disk is open before attempting to mount it */
    if (false == p_disk->is_open)
    {
        return FSP_ERR_NOT_OPEN;
    }

    /* If the disk is already mounted, no further action is required */
    if (true == p_disk->is_mount)
    {
        return FSP_SUCCESS;
    }

    /* Initialize the media device for the FAT disk */
    status = RM_FREERTOS_PLUS_FAT_MediaInit(p_disk->p_rm_fat_instance->p_ctrl, &p_disk->rm_device);
    TERM_ERR_RET(FSP_SUCCESS != status, status, "RM_FREERTOS_PLUS_FAT_MediaInit failed\r\n");

    /* Initialize the disk structure for the FAT disk */
    status = RM_FREERTOS_PLUS_FAT_DiskInit(p_disk->p_rm_fat_instance->p_ctrl, p_disk->p_rm_fat_disk_cfg,
                                           &p_disk->ff_disk);
    TERM_ERR_RET(FSP_SUCCESS != status, status, "RM_FREERTOS_PLUS_FAT_DiskInit failed\r\n");

    /* Mount the disk on the FAT file system */
    ff_status = FF_Mount(&p_disk->ff_disk, p_disk->ff_disk.xStatus.bPartitionNumber);
    TERM_ERR_RET(FF_ERR_NONE != ff_status, (uint32_t)ff_status, "FF_Mount failed\r\n");

    /* Add the FAT disk to the FAT file system for file access */
    fs_status = FF_FS_Add(p_disk->p_ff_fs_path, &p_disk->ff_disk);
    TERM_ERR_RET(pdTRUE != fs_status, FSP_ERR_ASSERTION, "FF_FS_Add failed\r\n");

    /* Mark the disk as mounted to indicate it is ready for file operations */
    p_disk->is_mount = true;

    return status;
}

/***********************************************************************************************************************
 * @brief      Un-mounts a FAT disk.
 * @param[in]  p_disk  Pointer to the FAT disk structure.
 * @retval     FSP_SUCCESS on successful operation, other error codes otherwise.
 **********************************************************************************************************************/
fsp_err_t fat_disk_unmount(fat_disk_t * p_disk)
{
    fsp_err_t status = FSP_SUCCESS;
    FF_Error_t ff_status = FF_ERR_NONE;

    /* Validate that the provided disk pointer is not NULL to prevent invalid memory access */
    if (NULL == p_disk)
    {
        return FSP_ERR_INVALID_POINTER;
    }

    /* Ensure the disk is open before attempting to unmount it */
    if (false == p_disk->is_open)
    {
        return FSP_ERR_NOT_OPEN;
    }

    /* If the disk is not mounted, no unmounting action is required */
    if (false == p_disk->is_mount)
    {
        return FSP_SUCCESS;
    }

    /* Remove the FAT disk from the FAT file system. This step detaches the disk from the file system */
    FF_FS_Remove(p_disk->p_ff_fs_path);

    /* Unmount the disk from the FAT file system */
    ff_status = FF_Unmount(&p_disk->ff_disk);
    TERM_ERR_RET(FF_ERR_NONE != ff_status, (uint32_t)ff_status, "FF_Unmount failed\r\n");

    /* De-initialize the disk to release associated resources */
    status = RM_FREERTOS_PLUS_FAT_DiskDeinit(p_disk->p_rm_fat_instance->p_ctrl, &p_disk->ff_disk);
    TERM_ERR_RET(FSP_SUCCESS != status, status, "RM_FREERTOS_PLUS_FAT_DiskDeinit failed\r\n");

    /* Mark the disk as unmounted to update its operational state */
    p_disk->is_mount = false;

    return status;
}

/***********************************************************************************************************************
 * @brief      Closes a FAT disk.
 * @param[in]  p_disk  Pointer to the FAT disk structure.
 * @retval     FSP_SUCCESS on successful operation, other error codes otherwise.
 **********************************************************************************************************************/
fsp_err_t fat_disk_close(fat_disk_t * p_disk)
{
    fsp_err_t status = FSP_SUCCESS;

    /* Validate that the provided disk pointer is not NULL to prevent invalid memory access */
    if (NULL == p_disk)
    {
        return FSP_ERR_INVALID_POINTER;
    }

    /* Check if the disk is already closed to avoid redundant operations */
    if (false == p_disk->is_open)
    {
        return FSP_ERR_NOT_OPEN;
    }

    /* If RM FAT initialization was required, close the RM FAT instance to release resources */
    if (true == p_disk->required_open_rm_fat)
    {
        /* Call the RM_FREERTOS_PLUS_FAT_Close API to close the RM FAT instance */
        status = RM_FREERTOS_PLUS_FAT_Close(p_disk->p_rm_fat_instance->p_ctrl);

        /* Handle any errors returned by the close operation */
        TERM_ERR_RET(FSP_SUCCESS != status, status, "RM_FREERTOS_PLUS_FAT_Close failed\r\n");
    }

    /* Mark the disk as closed by updating its operational state */
    p_disk->is_open = false;

    return status;
}

/***********************************************************************************************************************
 * @brief      Formats a FAT disk.
 * @param[in]  p_disk  Pointer to the FAT disk structure.
 * @retval     FSP_SUCCESS on successful operation, other error codes otherwise.
 **********************************************************************************************************************/
fsp_err_t fat_disk_format(fat_disk_t * p_disk)
{
    FF_Error_t ff_status = FF_ERR_NONE;
    BaseType_t fs_status = pdTRUE;
    FF_PartitionParameters_t partition_params;

    /* Validate that the provided disk pointer is not NULL to prevent invalid memory access */
    if (NULL == p_disk)
    {
        return FSP_ERR_INVALID_POINTER;
    }

    /* Ensure the disk is open before attempting to format it */
    if (false == p_disk->is_open)
    {
        return FSP_ERR_NOT_OPEN;
    }

    /* Set up partition parameters, including sector count and partition details */
    partition_params.ulSectorCount = p_disk->rm_device.sector_count;
    partition_params.ulHiddenSectors = 1;
    partition_params.ulInterSpace = 0;
    memset(partition_params.xSizes, 0, sizeof(partition_params.xSizes));
    partition_params.xSizes[0] = (BaseType_t)(partition_params.ulSectorCount - 1);
    partition_params.xPrimaryCount = 1;
    partition_params.eSizeType = eSizeIsSectors;

    /* Remove the FAT disk from the FAT file system to prepare for formatting */
    FF_FS_Remove(p_disk->p_ff_fs_path);
    p_disk->is_mount = false;

    /* Partition the disk using the specified partition parameters */
    ff_status = FF_Partition(&p_disk->ff_disk, &partition_params);
    TERM_ERR_RET(FF_ERR_NONE != ff_status, (uint32_t)ff_status, "FF_Partition failed\r\n");

    /* Format the partition on the disk to create a FAT file system */
    ff_status = FF_FormatDisk(&p_disk->ff_disk, p_disk->ff_disk.xStatus.bPartitionNumber,
                              pdFALSE, pdFALSE, p_disk->p_ff_fs_path);
    TERM_ERR_RET(FF_ERR_NONE != ff_status, (uint32_t)ff_status, "FF_FormatDisk failed\r\n");

    /* Mount the newly formatted disk to make it accessible for file operations */
    ff_status = FF_Mount(&p_disk->ff_disk, p_disk->ff_disk.xStatus.bPartitionNumber);
    TERM_ERR_RET(FF_ERR_NONE != ff_status, (uint32_t)ff_status, "FF_Mount failed\r\n");

    /* Add the formatted disk to the FAT file system for file access */
    fs_status = FF_FS_Add(p_disk->p_ff_fs_path, &p_disk->ff_disk);
    TERM_ERR_RET(pdTRUE != fs_status, FSP_ERR_ASSERTION, "FF_FS_Add failed\r\n");

    /* Mark the disk as mounted to update its operational state */
    p_disk->is_mount = true;

    return FSP_SUCCESS;
}

/***********************************************************************************************************************
 * @brief      Deletes a FAT disk instance by resetting its parameters.
 * @param[in]  p_disk  Pointer to the FAT disk structure.
 * @retval     FSP_SUCCESS on successful operation, other error codes otherwise.
 **********************************************************************************************************************/
fsp_err_t fat_disk_delete(fat_disk_t * p_disk)
{
    /* Validate that the provided disk pointer is not NULL to prevent invalid memory access */
    if (NULL == p_disk)
    {
        return FSP_ERR_INVALID_POINTER;
    }

    /* Reset the pointer to the RM FAT instance to clear the association */
    p_disk->p_rm_fat_instance = NULL;

    /* Reset the pointer to the RM FAT disk configuration */
    p_disk->p_rm_fat_disk_cfg = NULL;

    /* Reset the file system path associated with the disk */
    p_disk->p_ff_fs_path = NULL;

    /* Set the flag indicating whether RM FAT initialization is required to false */
    p_disk->required_open_rm_fat = false;

    /* Mark the disk as not open */
    p_disk->is_open = false;

    /* Mark the disk as not mounted */
    p_disk->is_mount = false;

    return FSP_SUCCESS;
}

/***********************************************************************************************************************
 * @brief      Lists the files and directories in the specified path.
 * @param[in]  p_path  Path to the directory.
 * @retval     None.
 **********************************************************************************************************************/
void fat_dir_list(char * p_path)
{
    FF_Error_t ff_status = FF_ERR_NONE;
    FF_FindData_t entry;
    char str_mode[ENTRY_MODE_STR_LEN] = {NULL_CHAR};
    char str_date[ENTRY_DATE_STR_LEN] = {NULL_CHAR};
    char str_time[ENTRY_TIME_STR_LEN] = {NULL_CHAR};
    char str_info[ENTRY_INFO_STR_LEN] = {NULL_CHAR};

    /* Check if the provided path pointer is NULL to prevent invalid memory access */
    if (NULL == p_path)
    {
        return;
    }

    /* Format the directory header with the directory path and store it in the info buffer */
    snprintf(str_info, ENTRY_INFO_STR_LEN, DIR_HEADER_FORMAT, p_path);

    /* Print the formatted directory header */
    PRINT_INFO_STR(str_info);

    /* Start searching for the first entry in the directory */
    ff_status = ff_findfirst(p_path, &entry);

    /* If the search was successful, process the directory entries */
    if (FF_ERR_NONE == ff_status)
    {
        /* Loop through all entries in the directory */
        do
        {
            /* Format the entry's attributes */
            memset(str_mode, FILE_DATA_ZERO, sizeof(str_mode));
            snprintf(str_mode, ENTRY_MODE_STR_LEN, ENTRY_MODE_FORMAT,
                     entry.ucAttributes & FF_FAT_ATTR_DIR ? 'd' : '-',
                     entry.ucAttributes & FF_FAT_ATTR_DIR ? '-' : 'a',
                     entry.ucAttributes & FF_FAT_ATTR_READONLY ? 'r' : '-',
                     entry.ucAttributes & FF_FAT_ATTR_HIDDEN ? 'h' : '-',
                     entry.ucAttributes & FF_FAT_ATTR_SYSTEM ? 's' : '-',
                     entry.ucAttributes & FF_FAT_ATTR_VOLID ? 'l' : '-');

            /* Format the last modified date of the directory entry */
            memset(str_date, FILE_DATA_ZERO, sizeof(str_date));
            snprintf(str_date, ENTRY_DATE_STR_LEN, ENTRY_DATE_FORMAT,
                     entry.xDirectoryEntry.xModifiedTime.Month,
                     entry.xDirectoryEntry.xModifiedTime.Day,
                     entry.xDirectoryEntry.xModifiedTime.Year);

            /* Format the last modified time of the directory entry */
            memset(str_time, FILE_DATA_ZERO, sizeof(str_time));
            snprintf(str_time, ENTRY_TIME_STR_LEN, ENTRY_TIME_FORMAT,
                     entry.xDirectoryEntry.xModifiedTime.Hour,
                     entry.xDirectoryEntry.xModifiedTime.Minute,
                     entry.xDirectoryEntry.xModifiedTime.Second);

            /* Format the complete entry information, including attributes, date, time, size, and name */
            memset(str_info, FILE_DATA_ZERO, sizeof(str_info));
            snprintf(str_info, ENTRY_INFO_STR_LEN, ENTRY_INFO_FORMAT,
                     str_mode, str_date, str_time, (unsigned long) entry.ulFileSize, entry.pcFileName);

            /* Print the formatted entry information */
            PRINT_INFO_STR(str_info);

            /* Find the next entry in the directory */
            ff_status = ff_findnext(&entry);
        }
        /* Continue processing entries until there are no more to find */
        while (FF_ERR_NONE == ff_status);
    }
}

/***********************************************************************************************************************
 * @brief      Copies a file from source path to destination path.
 * @param[in]  p_src_path   Path to the source file.
 * @param[in]  p_dest_path  Path to the destination file.
 * @retval     FSP_SUCCESS on successful operation, other error codes otherwise.
 **********************************************************************************************************************/
fsp_err_t fat_file_copy(char * p_src_path, char * p_dest_path)
{
    FF_Error_t ff_status = FF_ERR_NONE;
    FF_FILE * p_src_file = NULL;
    FF_FILE * p_dest_file = NULL;
    size_t r_len = FILE_DATA_ZERO;
    size_t w_len = FILE_DATA_ZERO;

    /* Check if the source or destination path is NULL to prevent invalid memory access */
    if (NULL == p_src_path || NULL == p_dest_path)
    {
        return FSP_ERR_INVALID_POINTER;
    }

    /* Open the source file in read mode */
    p_src_file = ff_fopen((const char *)p_src_path, FILE_OPEN_FOR_READ);
    TERM_ERR_RET(NULL == p_src_file, FSP_ERR_INVALID_POINTER, "ff_fopen for the source file failed");

    /* Open the destination file in write mode */
    p_dest_file = ff_fopen((const char *)p_dest_path, FILE_OPEN_FOR_WRITE);
    TERM_ERR_RET(NULL == p_dest_file, FSP_ERR_INVALID_POINTER, "ff_fopen for the destination file failed");

    /* Allocate a buffer for copying data between the source and destination files */
    char *p_buf = (char *)pvPortMalloc(FILE_BUFF_MAX_SIZE);
    if (NULL == p_buf)
    {
        return FSP_ERR_INVALID_POINTER;
    }

    /* Copy the file content in chunks until the entire file is processed */
    do
    {
        /* Clear the buffer before reading data */
        memset(p_buf, FILE_DATA_ZERO, FILE_BUFF_MAX_SIZE);

        /* Read data from the source file into the buffer */
        r_len = ff_fread(p_buf, sizeof(char), FILE_BUFF_MAX_SIZE, p_src_file);

        /* If data is read, write it to the destination file */
        if (FILE_DATA_ZERO != r_len)
        {
            w_len = ff_fwrite(p_buf, sizeof(char), r_len, p_dest_file);

            /* Check if the number of bytes written matches the number read */
            if (w_len != r_len)
            {
                PRINT_ERR_STR("ff_fwrite failed");

                /* Close the destination file to release resources */
                ff_status = ff_fclose(p_dest_file);
                if (FF_ERR_NONE != ff_status)
                {
                    PRINT_ERR_STR("ff_fclose for destination file failed");
                }

                /* Close the source file to release resources */
                ff_status = ff_fclose(p_src_file);
                if (FF_ERR_NONE != ff_status)
                {
                    PRINT_ERR_STR("ff_fclose for source file failed");
                }

                /* Free the allocated buffer */
                vPortFree(p_buf);

                return FSP_ERR_INVALID_SIZE;
            }
        }
    }
    while (FILE_DATA_ZERO != r_len);

    /* Free the allocated buffer after the copy operation */
    vPortFree(p_buf);

    /* Close the destination file */
    ff_status = ff_fclose(p_dest_file);
    TERM_ERR_RET(FF_ERR_NONE != ff_status, (uint32_t)ff_status, "ff_fclose failed");

    /* Close the source file */
    ff_status = ff_fclose(p_src_file);
    TERM_ERR_RET(FF_ERR_NONE != ff_status, (uint32_t)ff_status, "ff_fclose failed");

    /* Retrieve metadata for the source file */
    FF_Stat_t src_details;
    ff_status = ff_stat(p_src_path, &src_details);
    if (FF_ERR_NONE != ff_status)
    {
        PRINT_ERR_STR("ff_stat for the source file failed");
    }

    /* Retrieve metadata for the destination file */
    FF_Stat_t dest_details;
    ff_status = ff_stat(p_dest_path, &dest_details);
    if (FF_ERR_NONE != ff_status)
    {
        PRINT_ERR_STR("ff_stat for the destination file failed");
    }

    /* Verify that the size of the source file matches the destination file */
    TERM_ERR_RET(src_details.st_size != dest_details.st_size, (uint32_t)FSP_ERR_INVALID_SIZE,
                 "The file size is incorrect");

    return FSP_SUCCESS;
}

/***********************************************************************************************************************
 * @brief      Creates a file and writes fixed data to it.
 * @param[in]  p_path  Path of the file to be created.
 * @retval     FSP_SUCCESS on successful operation, other error codes otherwise.
 **********************************************************************************************************************/
fsp_err_t fat_file_create(char * p_path)
{
    FF_Error_t ff_status = FF_ERR_NONE;
    FF_FILE *p_file = NULL;
    size_t w_len = FILE_DATA_ZERO;

    /* Validate that the provided file path pointer is not NULL */
    if (NULL == p_path)
    {
        return FSP_ERR_INVALID_POINTER;
    }

    /* Open the file in write mode. If it doesn't exist, it will be created */
    p_file = ff_fopen((const char *)p_path, FILE_OPEN_FOR_WRITE);
    TERM_ERR_RET(NULL == p_file, FSP_ERR_INVALID_POINTER, "ff_fopen for a file failed");

    /* Allocate a buffer for writing data to the file */
    char *p_buf = (char *)pvPortMalloc(FILE_BUFF_MAX_SIZE);
    if (NULL == p_buf)
    {
        return FSP_ERR_INVALID_POINTER;
    }

    /* Fill the buffer with fixed data ('A') to write to the file */
    memset(p_buf, 'A', FILE_BUFF_MAX_SIZE);

    /* Initialize the number of write iterations */
    uint32_t w_times = FILE_WRITE_TIMES;

    /* Write data to the file in chunks for the specified number of iterations */
    do
    {
        w_times--;

        /* Write data from the buffer to the file */
        w_len = ff_fwrite(p_buf, sizeof(char), FILE_BUFF_MAX_SIZE, p_file);

        /* Check if the number of bytes written matches the buffer size */
        if (FILE_BUFF_MAX_SIZE != w_len)
        {
            PRINT_ERR_STR("ff_fwrite failed");

            /* Close the file to release resource */
            ff_status = ff_fclose(p_file);
            if (FF_ERR_NONE != ff_status)
            {
                PRINT_ERR_STR("ff_fclose failed");
            }

            /* Free the allocated buffer before returning */
            vPortFree(p_buf);

            return FSP_ERR_INVALID_SIZE;
        }
    }
    while (FILE_DATA_ZERO != w_times);

    /* Free the allocated memory buffer after the write operation is complete */
    vPortFree(p_buf);

    /* Close the file after writing */
    ff_status = ff_fclose(p_file);
    TERM_ERR_RET(FF_ERR_NONE != ff_status, (uint32_t)ff_status, "ff_fclose failed");

    /* Retrieve the file's metadata after the write operation */
    FF_Stat_t details;
    ff_status = ff_stat(p_path, &details);
    TERM_ERR_RET(FF_ERR_NONE != ff_status, (uint32_t)ff_status, "ff_stat failed");

    /* Verify that the size of the file matches the expected size */
    TERM_ERR_RET((FILE_BUFF_MAX_SIZE * FILE_WRITE_TIMES) != details.st_size,
                 FSP_ERR_INVALID_SIZE, "The file size is incorrect");

    return FSP_SUCCESS;
}
