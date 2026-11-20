/***********************************************************************************************************************
 * File Name    : filex_sdram_example_app.c
 * Description  : Example application layer for the FileX Block Media SDRAM EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - filex_thread_entry.c calls a single function: filex_sdram_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdint.h>
#include <stdlib.h>
#include "terminal.h"
#include "filex_sdram_ep.h"
#include "filex_sdram_example_app.h"

/* Private variables */
static FX_MEDIA s_fx_media;
static uint8_t s_fx_media_media_memory[G_FX_MEDIA_MEDIA_MEMORY_SIZE];
static CHAR s_dir_name[] = DIR_NAME_ONE;
static CHAR s_file_name[] = FILE_NAME_ONE;
static CHAR s_write_data[WRITE_BUFFER_SIZE] = {RESET_VALUE};

/* Private functions */
static void file_system_init(void);
static void file_system_operation(UINT request);
static UINT media_format(void);
static UINT media_get_property(void);
static UINT dir_create(void);
static UINT dir_get_property(void);
static UINT dir_delete(void);
static UINT file_create(void);
static UINT file_write(void);
static UINT file_read(void);
static UINT file_delete(void);
static void create_fixed_buffer(void);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the FileX Block Media SDRAM example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void filex_sdram_example_app_run(void)
{
    UINT status = FX_SUCCESS;
    char input[TERM_BUFFER_SIZE] = {NULL_CHAR};
    uint32_t input_size = RESET_VALUE;

    /* Suspend until the terminal thread has run at least once */
    status = terminal_thread_init_check();
    TERM_ERR_TRAP(FX_SUCCESS != status, status, "terminal_thread_init_check failed\r\n");

    /* Send an output queue to the terminal thread to display the EP banner */
    status = (UINT)term_send_output_queue(TERMINAL_OUTPUT_EP_BANNER, NULL, RESET_VALUE);
    TERM_ERR_TRAP(FX_SUCCESS != status, status, "term_send_output_queue for the EP banner failed\r\n");
    tx_thread_sleep(FILEX_SLEEP_TICK);

    /* Send an output queue to the terminal thread to display the EP information */
    status = (UINT)term_send_output_queue(TERMINAL_OUTPUT_EP_INFO, NULL, RESET_VALUE);
    TERM_ERR_TRAP(FX_SUCCESS != status, status, "term_send_output_queue for the EP information failed\r\n");
    tx_thread_sleep(FILEX_SLEEP_TICK);

    /* Initialize the FileX file system */
    file_system_init();

    while (true)
    {
        /* Send an output queue to the terminal thread to display the EP menu */
        status = (UINT)term_send_output_queue(TERMINAL_OUTPUT_APP_MENU, NULL, RESET_VALUE);
        TERM_ERR_TRAP(FX_SUCCESS != status, status, "term_send_output_queue for the EP menu failed\r\n");

        /* Suspend until there is an input queue */
        status = (UINT)term_get_input_queue(input, &input_size, TX_WAIT_FOREVER);
        TERM_ERR_TRAP(TX_SUCCESS != status, status, "term_get_input_queue failed\r\n");

        /* Perform file system operations as required by the user */
        file_system_operation((UINT)atoi(input));

        tx_thread_sleep(FILEX_SLEEP_TICK);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function initializes the FileX file system.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void file_system_init(void)
{
    UINT status = FX_SUCCESS;
    date_time_t time = {.date = RESET_VALUE, .hour = RESET_VALUE, .min = RESET_VALUE,
                        .month = RESET_VALUE, .sec = RESET_VALUE, .year = RESET_VALUE};

    /* Extract current date from compiler macros */
    time.month = get_month(&__DATE__[MONTH_INDEX]);
    time.date  = (UINT)atoi(&__DATE__[DATE_INDEX]);
    time.year  = (UINT)atoi(&__DATE__[YEAR_INDEX]);

    /* Extract current time from compiler macros */
    time.hour  = (UINT)atoi(&__TIME__[HOUR_INDEX]);
    time.min   = (UINT)atoi(&__TIME__[MIN_INDEX]);
    time.sec   = (UINT)atoi(&__TIME__[SEC_INDEX]);

    /* Open and configure FileX block media interface */
    status = filex_init();
    TERM_ERR_TRAP(FSP_SUCCESS != status, status, "filex_init failed\r\n");

    /* Initialize the FileX system */
    fx_system_initialize();

    /* Set date for FileX system */
    status = fx_system_date_set(time.year, time.month, time.date);
    TERM_ERR_TRAP(FX_SUCCESS != status, status, "fx_system_date_set failed\r\n");

    /* Set time for FileX system */
    status = fx_system_time_set(time.hour, time.min, time.sec);
    TERM_ERR_TRAP(FX_SUCCESS != status, status, "fx_system_time_set failed\r\n");

    /* Open media using the Azure FileX API */
    status = fx_media_open(&s_fx_media, "s_fx_media", RM_FILEX_BLOCK_MEDIA_BlockDriver,
                           (void *)&g_rm_filex_block_media_instance, s_fx_media_media_memory,
                           G_FX_MEDIA_MEDIA_MEMORY_SIZE);
    TERM_ERR_TRAP(FX_SUCCESS != status, status, "fx_media_open failed\r\n");
}

/*******************************************************************************************************************//**
 * @brief     This function performs file system operations based on the user option.
 * @param[in] option                    The user option for file system operations.
 * @retval    None.
 **********************************************************************************************************************/
static void file_system_operation(UINT option)
{
    UINT status = FX_SUCCESS;

    switch (option)
    {
        case REQUEST_MEDIA_FORMAT:
        {
            status = media_format();
            TERM_ERR_TRAP(FX_SUCCESS != status, status, "media_format failed\r\n");
            break;
        }

        case REQUEST_DIR_CREATE:
        {
            status = dir_create();
            TERM_ERR_TRAP(FX_SUCCESS != status, status, "dir_create failed\r\n");
            break;
        }

        case REQUEST_DIR_PROPERTY:
        {
            status = dir_get_property();
            TERM_ERR_TRAP(FX_SUCCESS != status, status, "dir_get_property failed\r\n");
            break;
        }

        case REQUEST_DIR_DELETE:
        {
            status = dir_delete();
            TERM_ERR_TRAP(FX_SUCCESS != status, status, "dir_delete failed\r\n");
            break;
        }

        case REQUEST_FILE_CREATE:
        {
            status = file_create();
            TERM_ERR_TRAP(FX_SUCCESS != status, status, "file_create failed\r\n");
            break;
        }

        case REQUEST_FILE_WRITE:
        {
            status = file_write();
            TERM_ERR_TRAP(FX_SUCCESS != status, status, "file_write failed\r\n");
            break;
        }

        case REQUEST_FILE_READ:
        {
            status = file_read();
            TERM_ERR_TRAP(FX_SUCCESS != status, status, "file_read failed\r\n");
            break;
        }

        case REQUEST_FILE_DELETE:
        {
            status = file_delete();
            TERM_ERR_TRAP(FX_SUCCESS != status, status, "file_delete failed\r\n");
            break;
        }

        default:
        {
            PRINT_INFO_STR("\r\nInvalid input. Please select a valid option!\r\n");
            break;
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function formats the media.
 * @param[in] None.
 * @retval    FX_SUCCESS                Formatted successfully.
 * @retval    Any other error codes     Failed to format.
 **********************************************************************************************************************/
static UINT media_format(void)
{
    UINT status = FX_SUCCESS;

    PRINT_INFO_STR("\r\n\r\nFormatting media...\r\n\r\n");

    /* Close media before format */
    status = fx_media_close(&s_fx_media);
    if ((FX_SUCCESS != status) && (FX_MEDIA_NOT_OPEN != status))
    {
        return status;
    }

    /* Format the media */
    status = fx_media_format(&s_fx_media,                               /* Pointer to FileX media control block */
                             RM_FILEX_BLOCK_MEDIA_BlockDriver,          /* Driver entry */
                             (void *)&g_rm_filex_block_media_instance,  /* Pointer to FileX block media instance */
                             s_fx_media_media_memory,                   /* Media buffer pointer */
                             G_FX_MEDIA_MEDIA_MEMORY_SIZE,              /* Media buffer size */
                             (char *)G_FX_MEDIA_VOLUME_NAME,            /* Volume name */
                             G_FX_MEDIA_NUMBER_OF_FATS,                 /* Number of FATs */
                             G_FX_MEDIA_DIRECTORY_ENTRIES,              /* Directory entries */
                             G_FX_MEDIA_HIDDEN_SECTORS,                 /* Hidden sectors */
                             G_FX_MEDIA_TOTAL_SECTORS,                  /* Total sectors */
                             G_FX_MEDIA_BYTES_PER_SECTOR,               /* Sector size */
                             G_FX_MEDIA_SECTORS_PER_CLUSTER,            /* Sectors per cluster */
                             G_FX_MEDIA_HEADS,                          /* Heads (disk media) */
                             G_FX_MEDIA_SECTORS_PER_TRACK);             /* Sectors per track (disk media) */
    if (FX_SUCCESS != status)
    {
        return status;
    }

    /* Open media using the Azure FileX API */
    status = fx_media_open(&s_fx_media, "s_fx_media", RM_FILEX_BLOCK_MEDIA_BlockDriver,
                           (void *)&g_rm_filex_block_media_instance, s_fx_media_media_memory,
                           G_FX_MEDIA_MEDIA_MEMORY_SIZE);
    if (FX_SUCCESS != status)
    {
        return status;
    }

    /* Get and display the media properties */
    status = media_get_property();
    if (FX_SUCCESS != status)
    {
        return status;
    }

    PRINT_INFO_STR("\r\nMedia formatted successfully\r\n");

    return status;
}

/*******************************************************************************************************************//**
 * @brief     This function retrieves media properties.
 * @param[in] None.
 * @retval    FX_SUCCESS                Retrieved successfully.
 * @retval    Any other error codes     Failed to retrieve.
 **********************************************************************************************************************/
static UINT media_get_property(void)
{
    UINT status = FX_SUCCESS;
    media_property_t media = {.format_type = RESET_VALUE, .free_size = RESET_VALUE,
                              .phy_size = RESET_VALUE, .total_size = RESET_VALUE};

    /* Get the free space size of the media */
    status = fx_media_extended_space_available(&s_fx_media, &media.free_size);
    if (FX_SUCCESS != status)
    {
        return status;
    }

    /* Calculate the total size of the media */
    media.total_size = ((ULONG64)s_fx_media.fx_media_total_clusters *
                        (ULONG64)s_fx_media.fx_media_sectors_per_cluster *
                        (ULONG64)s_fx_media.fx_media_bytes_per_sector);

    /* Get the format type of the media */
    if (s_fx_media.fx_media_12_bit_FAT)
    {
        media.format_type = "FAT 12";
    }
    else if (s_fx_media.fx_media_32_bit_FAT)
    {
        media.format_type = "FAT 32";
    }
    else
    {
        media.format_type = "FAT 16";
    }

    /* Display media information */
    PRINT_MEDIA_PROPERTY(media);

    return status;
}

/*******************************************************************************************************************//**
 * @brief     This function creates a new directory.
 * @param[in] None.
 * @retval    FX_SUCCESS                Created successfully.
 * @retval    Any other error codes     Failed to create.
 **********************************************************************************************************************/
static UINT dir_create(void)
{
    UINT status = FX_SUCCESS;
    entry_info_t entry = {.name = {RESET_VALUE}, .attr = RESET_VALUE, .size = RESET_VALUE,
                          .time = {.month = RESET_VALUE, .date = RESET_VALUE, .year = RESET_VALUE,
                                   .hour = RESET_VALUE, .min = RESET_VALUE, .sec = RESET_VALUE}
                         };

    PRINT_INFO_STR("\r\n\r\nCreate a new directory...\r\n\r\n");

    /* Create directory using Azure FileX API */
    status = fx_directory_create(&s_fx_media, s_dir_name);
    if (FX_ALREADY_CREATED == status)
    {
        PRINT_INFO_STR("Directory already exists\r\n");
        return FX_SUCCESS;
    }

    if (FX_SUCCESS != status)
    {
        return status;
    }

    /* Flush data into the physical media */
    status = fx_media_flush(&s_fx_media);
    if (FX_SUCCESS != status)
    {
        return status;
    }

    /* Retrieve a directory name */
    strcpy(entry.name, s_dir_name);

    /* Retrieve a directory full information */
    status = fx_directory_information_get(&s_fx_media, entry.name, &entry.attr, &entry.size,
                                          &entry.time.year, &entry.time.month, &entry.time.date,
                                          &entry.time.hour, &entry.time.min, &entry.time.sec);
    if (FX_SUCCESS != status)
    {
        return status;
    }

    /* Display directory information */
    PRINT_ENTRY_INFO(entry);

    PRINT_INFO_STR("\r\nDirectory created successfully\r\n");

    return status;
}

/*******************************************************************************************************************//**
 * @brief     This function gets the properties of a directory.
 * @param[in] None.
 * @retval    FX_SUCCESS                Got successfully.
 * @retval    Any other error codes     Failed to get.
 **********************************************************************************************************************/
static UINT dir_get_property(void)
{
    UINT status = FX_SUCCESS;
    dir_property_t dir = {.file = RESET_VALUE, .size = RESET_VALUE, .subdir = RESET_VALUE};
    entry_info_t entry = {.name = {RESET_VALUE}, .attr = RESET_VALUE, .size = RESET_VALUE,
                          .time = {.month = RESET_VALUE, .date = RESET_VALUE, .year = RESET_VALUE,
                                   .hour = RESET_VALUE, .min = RESET_VALUE, .sec = RESET_VALUE}
                         };

    PRINT_INFO_STR("\r\n\r\nGet directory properties...\r\n");

    /* Extract first entry details in the current directory */
    status = fx_directory_first_full_entry_find(&s_fx_media, entry.name, &entry.attr, &entry.size,
                                                &entry.time.year, &entry.time.month, &entry.time.date,
                                                &entry.time.hour, &entry.time.min, &entry.time.sec);

    /* Return if the current directory is empty */
    if (FX_NO_MORE_ENTRIES == status)
    {
        PRINT_INFO_STR("\r\nDirectory empty\r\n");
        return FX_SUCCESS;
    }

    if (FX_SUCCESS != status)
    {
        return status;
    }

    /* Print root directory name */
    PRINT_INFO_STR("\r\nDirectory contents:\r\n\r\n");

    /* Check all entries in root directory */
    do
    {
        /* If the entry is a directory */
        if (FX_DIRECTORY == (entry.attr & FX_DIRECTORY))
        {
            /* Print entry info */
            PRINT_ENTRY_INFO(entry);

            /* Count number of directories */
            dir.subdir++;
        }

        /* If the entry is a file */
        else if (FX_ARCHIVE == (entry.attr & FX_ARCHIVE))
        {
            /* Print entry info */
            PRINT_ENTRY_INFO(entry);

            /* Calculate total files size */
            dir.size += entry.size;

            /* Count number of files */
            dir.file++;
        }

        /* In other cases */
        else
        {
           /* Do nothing */
        }

        /* Extract details of next entry in the root directory */
        status = fx_directory_next_full_entry_find(&s_fx_media, entry.name, &entry.attr, &entry.size,
                                                   &entry.time.year, &entry.time.month, &entry.time.date,
                                                   &entry.time.hour, &entry.time.min, &entry.time.sec);
        if ((FX_SUCCESS != status) && (FX_NO_MORE_ENTRIES != status))
        {
            return status;
        }
    }
    while (FX_NO_MORE_ENTRIES != status);

    /* Print number of file and total file size */
    PRINT_DIR_PROPERTY(dir);

    PRINT_INFO_STR("Got directory properties successfully\r\n");

    return FX_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief     This function deletes a directory.
 * @param[in] None.
 * @retval    FX_SUCCESS                Deleted successfully.
 * @retval    Any other error codes     Failed to delete.
 **********************************************************************************************************************/
static UINT dir_delete(void)
{
    UINT status = FX_SUCCESS;

    PRINT_INFO_STR("\r\n\r\nDelete a directory...\r\n\r\n");

    /* Delete a directory using Azure FileX API */
    status = fx_directory_delete(&s_fx_media, s_dir_name);
    if (FX_NOT_FOUND == status)
    {
        PRINT_INFO_STR("Directory does not exist\r\n");
        return FX_SUCCESS;
    }

    if (FX_DIR_NOT_EMPTY == status)
    {
        PRINT_INFO_STR("Directory not empty\r\n");
        return FX_SUCCESS;
    }

    if (FX_SUCCESS != status)
    {
        return status;
    }

    /* Flush data to physical media */
    status = fx_media_flush(&s_fx_media);
    if (FX_SUCCESS != status)
    {
        return status;
    }

    PRINT_INFO_STR("Deleted a directory successfully\r\n");

    return status;
}

/*******************************************************************************************************************//**
 * @brief     This function creates a new file.
 * @param[in] None.
 * @retval    FX_SUCCESS                Created successfully.
 * @retval    Any other error codes     Failed to create.
 **********************************************************************************************************************/
static UINT file_create(void)
{
    UINT status = FX_SUCCESS;
    entry_info_t entry = {.name = {RESET_VALUE}, .attr = RESET_VALUE, .size = RESET_VALUE,
                          .time = {.month = RESET_VALUE, .date = RESET_VALUE, .year = RESET_VALUE,
                                   .hour = RESET_VALUE, .min = RESET_VALUE, .sec = RESET_VALUE}
                         };

    PRINT_INFO_STR("\r\n\r\nCreate a new file...\r\n\r\n");

    /* Create a new file using the Azure FileX API */
    status = fx_file_create(&s_fx_media, s_file_name);
    if (FX_ALREADY_CREATED == status)
    {
        PRINT_INFO_STR("File already exists\r\n");
        return FX_SUCCESS;
    }

    if (FX_SUCCESS != status)
    {
        return status;
    }

    /* Flush data into the physical media */
    status = fx_media_flush(&s_fx_media);
    if (FX_SUCCESS != status)
    {
        return status;
    }

    /* Retrieve a file name */
    strcpy(entry.name, s_file_name);

    /* Retrieve a file full information */
    status = fx_directory_information_get(&s_fx_media, entry.name, &entry.attr, &entry.size,
                                          &entry.time.year, &entry.time.month, &entry.time.date,
                                          &entry.time.hour, &entry.time.min, &entry.time.sec);
    if (FX_SUCCESS != status)
    {
        return status;
    }

    /* Display file information */
    PRINT_ENTRY_INFO(entry);

    PRINT_INFO_STR("\r\nFile created successfully\r\n");

    return FX_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief     This function writes fixed data to a file.
 * @param[in] None.
 * @retval    FX_SUCCESS                Wrote successfully.
 * @retval    Any other error codes     Failed to write.
 **********************************************************************************************************************/
static UINT file_write(void)
{
    UINT status = FX_SUCCESS;
    UINT status_temp = FX_SUCCESS;
    ULONG actual_event = RESET_VALUE;
    FX_FILE file = {.fx_file_name = RESET_VALUE};
    date_time_t time = {.date = RESET_VALUE, .hour = RESET_VALUE, .min = RESET_VALUE,
                        .month = RESET_VALUE, .sec = RESET_VALUE, .year = RESET_VALUE};
    entry_info_t entry = {.name = {RESET_VALUE}, .attr = RESET_VALUE, .size = RESET_VALUE,
                          .time = {.month = RESET_VALUE, .date = RESET_VALUE, .year = RESET_VALUE,
                                   .hour = RESET_VALUE, .min = RESET_VALUE, .sec = RESET_VALUE}
                         };

    PRINT_INFO_STR("\r\n\r\nWrite to a file...\r\n\r\n");

    /* Open the file for writing by using the Azure FileX API */
    status = fx_file_open(&s_fx_media, &file, s_file_name, FX_OPEN_FOR_WRITE);

    if (FX_NOT_FOUND == status)
    {
        PRINT_INFO_STR("File does not exist\r\n");
        return FX_SUCCESS;
    }

    if (FX_SUCCESS != status)
    {
        return status;
    }

    /* Clean the file contents */
    status = fx_file_truncate(&file, TRUNCATE_VALUE);
    if (FX_SUCCESS != status)
    {
        /* Close the file using the Azure FileX API */
        status_temp = fx_file_close(&file);
        if (FX_SUCCESS != status_temp)
        {
            return status_temp;
        }

        return status;
    }

    /* Create fixed buffer */
    create_fixed_buffer();

    /* Write content to the opened file */
    for (ULONG i = 0; i < WRITE_TIMES; i++)
    {
        /* Write fixed buffer to a file */
        status = fx_file_write(&file, (VOID *)s_write_data, WRITE_BUFFER_SIZE);
        if (FX_SUCCESS != status)
        {
            /* Close the file using the Azure FileX API */
            status_temp = fx_file_close(&file);
            if (FX_SUCCESS != status_temp)
            {
                return status_temp;
            }

            return status;
        }

        /* Wait until the complete event flag is received */
        status = tx_event_flags_get(&g_media_event_flag, RM_BLOCK_MEDIA_EVENT_WAIT_END,
                                    TX_OR_CLEAR, &actual_event, OPERATION_TIME_OUT);
        if (TX_SUCCESS != status)
        {
            /* Close the file using the Azure FileX API */
            status_temp = fx_file_close(&file);
            if (FX_SUCCESS != status_temp)
            {
                return status_temp;
            }

            return status;
        }

        if (RESET_VALUE == i % WRITE_ONE_PERCENT)
        {
            PRINT_INFO_STR(".");
        }
    }
    PRINT_INFO_STR("\r\n\r\n");

    /* Get system time */
    status = fx_system_time_get(&time.hour, &time.min, &time.sec);
    if (FX_SUCCESS != status)
    {
        /* Close the file using the Azure FileX API */
        status_temp = fx_file_close(&file);
        if (FX_SUCCESS != status_temp)
        {
            return status_temp;
        }

        return status;
    }

    /* Get system date */
    status = fx_system_date_get(&time.year, &time.month, &time.date);
    if (FX_SUCCESS != status)
    {
        /* Close the file using the Azure FileX API */
        status_temp = fx_file_close(&file);
        if (FX_SUCCESS != status_temp)
        {
            return status_temp;
        }

        return status;
    }

    /* Set the date and time information for the opened file */
    status = fx_file_date_time_set(&s_fx_media, s_file_name, time.year, time.month, time.date,
                                   time.hour, time.min, time.sec);
    if (FX_SUCCESS != status)
    {
        /* Close the file using the Azure FileX API */
        status_temp = fx_file_close(&file);
        if (FX_SUCCESS != status_temp)
        {
            return status_temp;
        }

        return status;
    }

    /* Close the file using the Azure FileX API */
    status = fx_file_close(&file);
    if (FX_SUCCESS != status)
    {
        return status;
    }

    /* Flush data into the physical media */
    status = fx_media_flush(&s_fx_media);
    if (FX_SUCCESS != status)
    {
        return status;
    }

    /* Get file name */
    strcpy(entry.name, s_file_name);

    /* Get file full information */
    status = fx_directory_information_get(&s_fx_media, entry.name, &entry.attr, &entry.size,
                                          &entry.time.year, &entry.time.month, &entry.time.date,
                                          &entry.time.hour, &entry.time.min, &entry.time.sec);
    if (FX_SUCCESS != status)
    {
        return status;
    }

    /* Display file information */
    PRINT_ENTRY_INFO(entry);

    PRINT_INFO_STR("\r\nWrote to a file successfully\r\n");

    return FX_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief     This function reads and verifies data from a file.
 * @param[in] None.
 * @retval    FX_SUCCESS                Read and verified successfully.
 * @retval    Any other error codes     Failed to read or verify.
 **********************************************************************************************************************/
static UINT file_read(void)
{
    UINT status = FX_SUCCESS;
    UINT status_temp = FX_SUCCESS;
    ULONG actual_event = RESET_VALUE;
    FX_FILE file = {.fx_file_name = RESET_VALUE};
    ULONG len = RESET_VALUE;
    entry_info_t entry = {.name = {RESET_VALUE}, .attr = RESET_VALUE, .size = RESET_VALUE,
                          .time = {.month = RESET_VALUE, .date = RESET_VALUE, .year = RESET_VALUE,
                                   .hour = RESET_VALUE, .min = RESET_VALUE, .sec = RESET_VALUE}
                         };
    CHAR read_data[READ_BUFFER_SIZE + 1U] = {RESET_VALUE};

    PRINT_INFO_STR("\r\n\r\nRead from a file...\r\n\r\n");

    /* Open the file for reading by using the Azure FileX API */
    status = fx_file_open(&s_fx_media, &file, s_file_name, FX_OPEN_FOR_READ);
    if (FX_NOT_FOUND == status)
    {
        PRINT_INFO_STR("File does not exist\r\n");
        return FX_SUCCESS;
    }

    if (FX_SUCCESS != status)
    {
        return status;
    }

    /* Seek to the beginning of the file */
    status = fx_file_seek(&file, SEEK_VALUE);
    if (FX_SUCCESS != status)
    {
       /* Close the file using the Azure FileX API */
       status_temp = fx_file_close(&file);
       if (FX_SUCCESS != status_temp)
       {
           return status_temp;
       }

       return status;
    }

    /* Read data from the file */
    status = fx_file_read(&file, read_data, READ_BUFFER_SIZE, &len);

    /* If reading the file failed */
    if ((FX_END_OF_FILE != status) && (FX_SUCCESS != status))
    {
        /* Close the file using the Azure FileX API */
        status_temp = fx_file_close(&file);
        if (FX_SUCCESS != status_temp)
        {
            return status_temp;
        }

        return status;
    }

    if (RESET_VALUE == len)
    {
        status = fx_file_close(&file);
        if (FX_SUCCESS != status)
        {
            return status;
        }

        PRINT_INFO_STR("File is empty\r\n");
        return FX_SUCCESS;
    }

    /* Wait until the complete event flag is received */
    status = tx_event_flags_get(&g_media_event_flag, RM_BLOCK_MEDIA_EVENT_WAIT_END,
                                TX_OR_CLEAR, &actual_event, OPERATION_TIME_OUT);
    if (TX_SUCCESS != status)
    {
        /* Close the file using the Azure FileX API */
        status_temp = fx_file_close(&file);
        if (FX_SUCCESS != status_temp)
        {
            return status_temp;
        }

        return status;
    }

    /* Close the file using the Azure FileX API */
    status = fx_file_close(&file);
    if (FX_SUCCESS != status)
    {
        return status;
    }

    /* Retrieve file name */
    strcpy(entry.name, s_file_name);

    /* Retrieve file full information */
    status = fx_directory_information_get(&s_fx_media, entry.name, &entry.attr, &entry.size,
                                          &entry.time.year, &entry.time.month, &entry.time.date,
                                          &entry.time.hour, &entry.time.min, &entry.time.sec);
    if (FX_SUCCESS != status)
    {
        return status;
    }

    /* Display file information */
    PRINT_ENTRY_INFO(entry);

    if (READ_BUFFER_SIZE > len)
    {
        /* Display content of the file */
        PRINT_INFO_STR("\r\nContent of the file:\r\n");
        term_send_output_queue(TERMINAL_OUTPUT_APP_INFO_STR, read_data, len + 1U);
    }
    else
    {
        /* Display content of the first 1 kB of the file */
        PRINT_INFO_STR("\r\nContent of the first 1 kB of the file:\r\n");
        term_send_output_queue(TERMINAL_OUTPUT_APP_INFO_STR, read_data, READ_BUFFER_SIZE + 1U);
    }

    PRINT_INFO_STR("\r\n\r\nRead from a file successfully\r\n");

    return FX_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief     This function deletes a file.
 * @param[in] None.
 * @retval    FX_SUCCESS                Deleted successfully.
 * @retval    Any other error codes     Failed to delete.
 **********************************************************************************************************************/
static UINT file_delete(void)
{
    UINT status = FX_SUCCESS;

    PRINT_INFO_STR("\r\n\r\nDelete a file...\r\n");

    /* Delete the file using the Azure FileX API */
    status = fx_file_delete(&s_fx_media, s_file_name);
    if (FX_NOT_FOUND == status)
    {
        PRINT_INFO_STR("\r\nFile does not exist\r\n");
        return FX_SUCCESS;
    }

    if (FX_SUCCESS != status)
    {
        return status;
    }

    /* Flush data into the physical media */
    status = fx_media_flush(&s_fx_media);
    if (FX_SUCCESS != status)
    {
        return status;
    }

    PRINT_INFO_STR("\r\nDeleted a file successfully\r\n");

    return FX_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief     This function creates a fixed data buffer.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void create_fixed_buffer(void)
{
    CHAR * p_data = s_write_data;

    /* Clean write buffer */
    memset(p_data, NULL_CHAR, WRITE_BUFFER_SIZE);

    /* Create fixed buffer */
    for (uint16_t i = 0; i < WRITE_BUFFER_SIZE / WRITE_LINE_SIZE; i++)
    {
        strncpy(p_data, WRITE_LINE_TEXT, WRITE_LINE_SIZE);
        p_data += WRITE_LINE_SIZE;
    }
}
