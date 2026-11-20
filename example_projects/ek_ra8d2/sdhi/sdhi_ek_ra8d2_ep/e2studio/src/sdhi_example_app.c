/***********************************************************************************************************************
 * File Name    : sdhi_example_app.c
 * Description  : Example application layer for the SDHI EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - sdhi_thread_entry.c calls a single function: sdhi_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "sdhi_ep.h"
#include "sdhi_thread.h"
#include "sdhi_example_app.h"
#include "console/uart_console.h"

/* Buffer used for file write operation */
static uint8_t s_write_data[BUFFER_SIZE] = {RESET_VALUE};

/* Buffer used for file read operation */
static uint8_t s_read_data[BUFFER_SIZE] = {RESET_VALUE};

/* Mount status flag */
static volatile bool s_mount_failed = false;

/* FreeRTOS+FAT disk instance */
static FF_Disk_t s_my_disk;

/* Current event group bit value */
static EventBits_t s_xEventGroupValue = RESET_VALUE;

/* Device structure contains device details */
static rm_freertos_plus_fat_device_t s_device;

/* Private functions */
static void process_sd_operation(uint8_t user_option);
static void sd_write_operation(void);
static void sd_read_operation(void);
static void sd_format(void);
static void sd_safely_remove(void);
static void sd_display_cwd(void);
static void update_buffer(void);
static fsp_err_t sd_init(void);
static void user_input(char * buf, int len);
static void handle_error(fsp_err_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the SDHI example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void sdhi_example_app_run(void)
{
    memset(&s_my_disk, RESET_VALUE, sizeof(s_my_disk));
    fsp_pack_version_t version = {RESET_VALUE};
    fsp_err_t err = FSP_SUCCESS;
    g_SDHI_EventGroupHandle = NULL;

    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        return;     /* UART failed to open — cannot print anything */
    }

    /* Get FSP version information */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the terminal */
    printf(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    printf(EP_INFO);
    printf(EP_NOTE);

    /* Initialize FreeRTOS+FAT */
    err = sd_init();
    handle_error(err, "sd_init failed\r\n");

    if (s_mount_failed)
    {
        printf("\r\n[ERR] FF_Mount API failed indicating the disk is not formatted");
        printf("\r\nPlease format the disk first\r\n");
    }

    /* Print EP menu */
    printf(EP_MENU);

    while (true)
    {
        char input_buff[INPUT_SIZE] = {RESET_VALUE};
        uint8_t user_option = RESET_VALUE;

        if (uart_console_has_char())
        {
            /* Get the user option */
            memset(&input_buff[0], NULL_CHAR, INPUT_SIZE);
            user_input(input_buff, (int)sizeof(input_buff));
            user_option = (uint8_t)atoi(input_buff);

            /* Perform SD operation based on the user option */
            process_sd_operation(user_option);
        }

        s_xEventGroupValue = xEventGroupGetBits(g_SDHI_EventGroupHandle);

        if (0U != (RM_FREERTOS_PLUS_FAT_EVENT_MEDIA_REMOVED & s_xEventGroupValue))
        {
            printf("\r\n\r\n"CARD_TYPE" Card disconnected without Safely remove option.\r\n");
            printf("\r\nConnect the "CARD_TYPE" Card and execute the Safely remove option to ensure file"
                   "\r\noperations work correctly.\r\n");

            /* Wait until SD/MicroSD Card device is connected */
            s_xEventGroupValue = xEventGroupWaitBits(g_SDHI_EventGroupHandle,
                                                     RM_FREERTOS_PLUS_FAT_EVENT_MEDIA_INSERTED,
                                                     pdTRUE,
                                                     pdFALSE,
                                                     portMAX_DELAY);
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function performs SD operation based on the user option.
 * @param[in] user_option               The user option.
 * @retval    None.
 **********************************************************************************************************************/
static void process_sd_operation(uint8_t user_option)
{
    fsp_err_t err = FSP_SUCCESS;

    switch (user_option)
    {
        case SD_WRITE:
        {
            if (false == s_mount_failed)
            {
                sd_write_operation();
                sd_read_operation();
            }
            else
            {
                printf("\r\nDisk is not formatted. Please format the disk first.\r\n");
            }
            break;
        }

        case SD_FORMAT:
        {
            sd_format();
            break;
        }

        case SAFELY_REMOVE:
        {
            sd_safely_remove();
            break;
        }

        case SD_DISPLAY_CWD:
        {
            if (false == s_mount_failed)
            {
                sd_display_cwd();
            }
            else
            {
                printf("\r\nDisk is not formatted. Please format the disk first.\r\n");
            }
            break;
        }

        case SD_INIT:
        {
            if ((RESET_VALUE == s_xEventGroupValue))
            {
                printf(EP_NOTE);
                err = sd_init();
                handle_error(err, "sd_init failed\r\n");
            }
            else
            {
                printf("\r\nFreeRTOS+FAT with SDHI driver is already initialized. Please select another option.\r\n");
            }
            break;
        }

        default:
        {
            printf("\r\nInvalid input. Please provide a valid input.\r\n");
            break;
        }
    }

    printf(EP_MENU);
}

/*******************************************************************************************************************//**
 * @brief     This function performs SD/MicroSD Card write operation.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void sd_write_operation(void)
{
    FF_FILE *file_pointer = NULL;
    FF_Stat_t file_details;
    FF_Error_t ff_err = FF_ERR_NONE;

    /* Number of bytes written */
    size_t bytes_written = RESET_VALUE;

    memset(&file_details, RESET_VALUE, sizeof(file_details));

    /* Verify that the SD/MicroSD Card is still connected */
    if (RM_FREERTOS_PLUS_FAT_EVENT_MEDIA_REMOVED != s_xEventGroupValue)
    {
        /* Open the file in write mode */
        file_pointer = ff_fopen((const char *) FILE_NAME, WRITE_MODE);
        if (NULL != file_pointer)
        {
            /* Fill write buffer with data */
            update_buffer();

            printf("\r\n"CARD_TYPE" Card write operation will be initiated\r\n");

            /* Write API */
            bytes_written = ff_fwrite(s_write_data, sizeof(s_write_data[RESET_VALUE]), BUFFER_SIZE, file_pointer);
            if (BUFFER_SIZE != bytes_written)
            {
                printf("\r\n[ERR] ff_fwrite API failed. Closing opened file.");
                printf("\r\nReturned Error Code: %d\r\n", stdioGET_ERRNO());

                ff_err = ff_fclose(file_pointer);
                if (FF_ERR_NONE != ff_err)
                {
                    printf("\r\n[ERR] ff_fclose API failed");
                    printf("\r\nReturned Error Code: %d\r\n", stdioGET_ERRNO());
                }
                return;
            }

            /* Close the file after writing */
            ff_err = ff_fclose(file_pointer);
            if (FF_ERR_NONE != ff_err)
            {
                printf("\r\n[ERR] ff_fclose API failed");
                printf("\r\nReturned Error Code: %d\r\n", stdioGET_ERRNO());
                return;
            }

            ff_err = ff_stat((const char*)FILE_NAME, &file_details);
            if (FF_ERR_NONE == ff_err)
            {
                /* Compare the actual bytes written and file size */
                if (bytes_written == file_details.st_size)
                {
                    printf("%d bytes data successfully written to file %s\r\n", bytes_written, FILE_NAME);
                    printf("Write operation is successful\r\n");
                }
                else
                {
                    printf("\r\n[ERR] ff_fwrite API failed");
                    printf("\r\nReturned Error Code: %d\r\n", stdioGET_ERRNO());
                }
            }
            else
            {
                printf("\r\n[ERR] ff_stat API failed");
                printf("\r\nReturned Error Code: %d\r\n", stdioGET_ERRNO());
            }
        }
        else
        {
            printf("\r\n[ERR] ff_fopen API failed");
            printf("\r\nReturned Error Code: %d\r\n", stdioGET_ERRNO());
        }
    }
    else
    {
        printf("\r\n[ERR] "CARD_TYPE" Card device is disconnected or not initialized after Safely remove option\r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief     This function performs SD/MicroSD Card read operation.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void sd_read_operation(void)
{
    FF_FILE *file_pointer = NULL;
    FF_Stat_t file_details;
    FF_Error_t ff_err = FF_ERR_NONE;

    memset(&file_details, RESET_VALUE, sizeof(file_details));

    /* Verify that the SD/MicroSD Card is still connected */
    if (RM_FREERTOS_PLUS_FAT_EVENT_MEDIA_REMOVED != s_xEventGroupValue)
    {
        /* Open the file in read mode to verify previously written data */
        file_pointer = ff_fopen((const char *) FILE_NAME, READ_MODE);
        if (NULL != file_pointer)
        {
            printf("\r\n"CARD_TYPE" Card read operation will be initiated\r\n");

            /* Capture the number of bytes read */
            size_t bytes_read = RESET_VALUE;
            bytes_read = ff_fread(s_read_data, sizeof(s_read_data[RESET_VALUE]), BUFFER_SIZE, file_pointer);
            if (BUFFER_SIZE != bytes_read)
            {
                printf("\r\n[ERR] ff_fread API failed. Closing opened file.");
                printf("\r\nReturned Error Code: %d\r\n", stdioGET_ERRNO());

                ff_err = ff_fclose(file_pointer);
                if (FF_ERR_NONE != ff_err)
                {
                    printf("\r\n[ERR] ff_fclose API failed");
                    printf("\r\nReturned Error Code: %d\r\n", stdioGET_ERRNO());
                }
                return;
            }

            ff_err = ff_fclose(file_pointer);
            if (FF_ERR_NONE != ff_err)
            {
                printf("\r\n[ERR] ff_fclose API failed");
                printf("\r\nReturned Error Code: %d\r\n", stdioGET_ERRNO());
                return;
            }

            ff_err = ff_stat((const char*)FILE_NAME, &file_details);
            if (FF_ERR_NONE == ff_err)
            {
                /* Compare the actual bytes read and file size */
                if (bytes_read == file_details.st_size)
                {
                    printf("%d bytes data successfully read from file %s\r\n", bytes_read, FILE_NAME);
                    printf("Read operation is successful\r\n");
                }
                else
                {
                    printf("\r\n[ERR] ff_fread API failed");
                    printf("\r\nReturned Error Code: %d\r\n", stdioGET_ERRNO());
                    return;
                }

                /* Compare write and read data */
                if (RESET_VALUE == memcmp(s_write_data, s_read_data, BUFFER_SIZE))
                {
                    printf("\r\nThe written and read data match.\r\n");
                }
                else
                {
                    printf("\r\n[ERR] The written and read data do not match.\r\n");
                }
            }
            else
            {
                printf("\r\n[ERR] ff_stat API failed");
                printf("\r\nReturned Error Code: %d\r\n", stdioGET_ERRNO());
            }
        }
        else
        {
            printf("\r\n[ERR] ff_fopen API failed");
            printf("\r\nReturned Error Code: %d\r\n", stdioGET_ERRNO());
        }
    }
    else
    {
        printf("\r\n[ERR] "CARD_TYPE" Card device is disconnected or not initialized after Safely remove option\r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief     This function performs SD/MicroSD Card format operation.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void sd_format(void)
{
    FF_Error_t ff_err = FF_ERR_NONE;
    FF_PartitionParameters_t partition_params;
    partition_params.ulSectorCount = s_device.sector_count;
    partition_params.ulHiddenSectors = 1;
    partition_params.ulInterSpace = 0;
    memset(partition_params.xSizes, 0, sizeof(partition_params.xSizes));
    partition_params.xSizes[0] = (BaseType_t)(partition_params.ulSectorCount - 1);
    partition_params.xPrimaryCount = 1;
    partition_params.eSizeType = eSizeIsSectors;

    if (RM_FREERTOS_PLUS_FAT_EVENT_MEDIA_REMOVED != s_xEventGroupValue)
    {
        printf("\r\n"CARD_TYPE" Card formatting will be initiated."
               "\r\nFormatting will take time depending on "CARD_TYPE" Card capacity.");
        printf("\r\nDo not disconnect the "CARD_TYPE" Card while formatting is in progress. Please wait...\r\n");

        /* Partition the SD/MicroSD Card */
        ff_err = FF_Partition(&s_my_disk, &partition_params);
        if (FF_ERR_NONE != ff_err)
        {
            printf("\r\n[ERR] FF_Partition API failed. Check the "CARD_TYPE" Card device.");
            printf("\r\nReturned Error Code: %d\r\n", ff_err);
            return;
        }

        /* Format time varies with SD/MicroSD Card capacity. Might take longer time for higher capacity
         * SD/MicroSD Card. */
        ff_err = FF_Format(&s_my_disk, s_my_disk.xStatus.bPartitionNumber, pdFALSE, pdFALSE);
        if (FF_ERR_NONE != ff_err)
        {
            printf("\r\n[ERR] FF_Format API failed. Check the "CARD_TYPE" Card device.");
            printf("\r\nReturned Error Code: %d\r\n", ff_err);
            return;
        }
        printf("\r\n"CARD_TYPE" Card formatted successfully\r\n");

        /* Check if mount failed previously */
        if (s_mount_failed)
        {
            /* Try mounting the disk again after partition */
            ff_err = FF_Mount(&s_my_disk, s_my_disk.xStatus.bPartitionNumber);
            handle_error((fsp_err_t)ff_err, "FF_Mount API failed\r\n");

            /* Add file system to the disk */
            ff_err = FF_FS_Add("/", &s_my_disk);
            handle_error((fsp_err_t)ff_err, "FF_FS_Add API failed\r\n");

            s_mount_failed = false;
        }

    }
    else
    {
        printf("\r\n[ERR] "CARD_TYPE" Card device is disconnected or not initialized after Safely remove option\r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief     This function closes FreeRTOS+FAT on SDHI and safely removes.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void sd_safely_remove(void)
{
    /* Check the SD/MicroSD Card connection status */
    if (RM_FREERTOS_PLUS_FAT_EVENT_MEDIA_REMOVED != s_xEventGroupValue)
    {
        fsp_err_t err = fat_disk_deinit(&s_my_disk);
        handle_error(err, "fat_disk_deinit failed\r\n");

        /* Close the FreeRTOS+FAT instance */
        fat_deinit();

        /* Clear the EventGroupBit value */
        xEventGroupClearBits(g_SDHI_EventGroupHandle, RM_FREERTOS_PLUS_FAT_EVENT_MEDIA_REMOVED);
        s_xEventGroupValue = xEventGroupGetBits(g_SDHI_EventGroupHandle);

        printf("\r\n"CARD_TYPE" Card can be safely removed now\r\n");
    }
    else
    {
        /* SD/MicroSD Card disconnected */
        printf("\r\n[ERR] "CARD_TYPE" Card device is disconnected or not initialized after Safely remove option\r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief     This function displays the content in the Current Working Directory (CWD).
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void sd_display_cwd(void)
{
    /* Buffer to store CWD content */
    char pcBuffer[50];
    FF_Error_t ff_err = FF_ERR_NONE;
    FF_FindData_t * pxFindStruct = NULL;
    const char * pcAttrib = NULL;
    const char * pcWritableFile = "writable file";
    const char * pcReadOnlyFile = "read only file";
    const char * pcDirectory = "directory";

    memset(pcBuffer, RESET_VALUE, sizeof(pcBuffer));

    /* Check the SD/MicroSD Card connection status */
    if (RM_FREERTOS_PLUS_FAT_EVENT_MEDIA_REMOVED != s_xEventGroupValue)
    {
        /* Allocate the memory */
        pxFindStruct = (FF_FindData_t *)pvPortMalloc(sizeof(FF_FindData_t));
        if (NULL == pxFindStruct)
        {
            printf("\r\n[ERR] Failed to allocate memory for file search operation\r\n");
            return;
        }

        /* Create a directory in current working directory */
        ff_err = ff_mkdir("/ra_dir");
        if (FF_ERR_NONE == ff_err)
        {
            printf("\r\nDirectory ra_dir is created in root directory");
        }
        else if (FF_ERR_DIR_OBJECT_EXISTS == FF_GETERROR(ff_err))
        {
            printf("\r\nDirectory ra_dir already exists in root directory");
        }
        else
        {
            handle_error((fsp_err_t) ff_err, "ff_mkdir API failed\r\n");
        }

        /* pcBuffer is returned, hence return value is not captured */
        ff_getcwd(pcBuffer, sizeof(pcBuffer));
        printf("\r\nCWD is: %s", pcBuffer);
        printf("\r\n%-20s %-25s %-12s\r\n", "File Name", "Attribute", "File Size");

        /* The first parameter to ff_findfirst() is the directory being searched.
         * Do not add wild cards to the end of the directory name. */
        if (RESET_VALUE == ff_findfirst("/", pxFindStruct))
        {
            do
            {
                /* Point pcAttrib to a string that describes the file */
                if (RESET_VALUE != (pxFindStruct->ucAttributes & FF_FAT_ATTR_DIR))
                {
                    pcAttrib = pcDirectory;
                }
                else if (pxFindStruct->ucAttributes & FF_FAT_ATTR_READONLY)
                {
                    pcAttrib = pcReadOnlyFile;
                }
                else
                {
                    pcAttrib = pcWritableFile;
                }

                /* Print the file name, size, and attribute string */
                printf("%-20s %-25s %u bytes\r\n", pxFindStruct->pcFileName, pcAttrib, pxFindStruct->ulFileSize);

            } while (RESET_VALUE == ff_findnext(pxFindStruct));
        }

        /* Release the allocated memory */
        vPortFree((void *)pxFindStruct);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function updates the write buffer with data and clears the read buffer.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void update_buffer(void)
{
    for (uint16_t buf_index = RESET_VALUE; buf_index < BUFFER_SIZE; buf_index++)
    {
        s_write_data[buf_index] = ASCII_CHAR_A;
        s_read_data[buf_index] = RESET_VALUE;
    }
}

/*******************************************************************************************************************//**
 * @brief     This function initializes the FreeRTOS+FAT instance.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
static fsp_err_t sd_init(void)
{
    fsp_err_t err = FSP_SUCCESS;
    FF_Error_t ff_err = FF_ERR_NONE;

    /* Create event flags */
    if (NULL == g_SDHI_EventGroupHandle)
    {
        g_SDHI_EventGroupHandle = xEventGroupCreate();
        if (NULL == g_SDHI_EventGroupHandle)
        {
            return FSP_ERR_OUT_OF_MEMORY;
        }
    }

    memset(&s_device, RESET_VALUE, sizeof(s_device));

    /* Open FreeRTOS+FAT */
    err = fat_init();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    printf("\r\nFreeRTOS+FAT opened successfully");
    printf("\r\nPlease connect "CARD_TYPE" Card...\r\n");

    /* Wait until SD/MicroSD Card is connected */
    s_xEventGroupValue = xEventGroupWaitBits(g_SDHI_EventGroupHandle,
                                             RM_FREERTOS_PLUS_FAT_EVENT_MEDIA_INSERTED,
                                             pdTRUE,
                                             pdFALSE,
                                             portMAX_DELAY);

    printf("\r\n"CARD_TYPE" Card connected\r\n");

    /* Initialize the mass storage device. This should not be done until the device is plugged in and initialized. */
    err = fat_media_init(&s_device);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Initialize one disk for each partition used in the application */
    err = fat_disk_init(&s_my_disk);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Reset mount status flag */
    s_mount_failed = false;

    /* Mount each disk. This assumes the disk is already partitioned and formatted. */
    ff_err = FF_Mount(&s_my_disk, s_my_disk.xStatus.bPartitionNumber);
    if (FF_ERR_NONE != ff_err)
    {
        /* Set mount status flag */
        s_mount_failed = true;
    }
    else
    {
        /* Add the disk to the file system */
        ff_err = FF_FS_Add("/", &s_my_disk);
        if (FF_FS_ADD_FAILURE == ff_err)
        {
            return ((fsp_err_t)FF_ERR_DEVICE_DRIVER_FAILED);
        }
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief         This function reads a line of user input from stdin and removes trailing newline characters.
 * @param[in,out] buf                   Pointer to buffer where the input string is stored.
 * @param[in]     len                   Maximum number of characters to read (buffer size).
 * @retval        None.
 **********************************************************************************************************************/
static void user_input(char * buf, int len)
{
    fgets(buf, len, stdin);
    buf[strcspn(buf, "\r\n")] = '\0';
}

/*******************************************************************************************************************//**
 * @brief     This function handles an error by printing error message and code, de-initializing the related modules,
 *            and entering a trap state.
 * @param[in] err                       Error code.
 * @param[in] err_str                   Error message.
 * @retval    None.
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err, const char * err_str)
{
    if (FSP_SUCCESS != err)
    {
        printf("\r\n[ERR] %s", err_str);
        printf("Returned Error Code: 0x%x\r\n", (unsigned int) err);
        uart_console_flush();
        fat_deinit();
        __BKPT(0);
    }
}
