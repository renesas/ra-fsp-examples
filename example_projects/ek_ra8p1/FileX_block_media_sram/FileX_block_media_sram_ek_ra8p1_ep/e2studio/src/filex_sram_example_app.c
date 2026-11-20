/***********************************************************************************************************************
 * File Name    : filex_sram_example_app.c
 * Description  : Example application layer for the FileX Block Media SRAM EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - filex_thread_entry.c calls a single function: filex_sram_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdint.h>
#include <stdlib.h>
#include "filex_sram_ep.h"
#include "filex_sram_example_app.h"
#include "terminal.h"

/* Private variables */
static CHAR s_dir_name[] = DIR_NAME_ONE;
static CHAR s_file_name[] = FILE_NAME_ONE;
static CHAR s_write_data[WRITE_BUFFER_SIZE] = {RESET_VALUE};

/* Private functions */
static UINT dir_create_operation(void);
static UINT dir_get_property_operation(void);
static UINT dir_delete_operation(void);
static UINT file_create_operation(void);
static UINT file_write_operation(void);
static UINT file_read_operation(void);
static UINT file_delete_operation(void);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the FileX Block Media SRAM example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void filex_sram_example_app_run(void)
{
    UINT status = FX_SUCCESS;
    char input[TERM_IP_MAX_SIZE] = {NULL_CHAR};

    /* Initialize the Terminal Thread */
    term_thread_init_check();

    /* Print EP and banner info */
    term_print_data(TERM_OP_MSG_BANNER, RESET_VALUE, NULL);
    tx_thread_sleep(1U);

    /* Initialize the FileX file system */
    status = file_system_init();
    TERM_ERR_TRAP(FX_SUCCESS != status, status, "\r\nfile_system_init failed");

    while (true)
    {
        /* Print the menu options */
        term_print_data(TERM_OP_MSG_MENU, RESET_VALUE, NULL);

        /* Wait for user input */
        do
        {
            status = term_ip_get(input, sizeof(input));
        } while (TX_SUCCESS != status);

        /* Perform file system operations as required by the user */
        switch ((UINT)atoi(input))
        {
            case REQUEST_MEDIA_FORMAT:
            {
                PRINT_INFO_STR("\r\nFormatting media...");
                status = media_format();
                TERM_ERR_TRAP(FX_SUCCESS != status, status, "\r\nmedia_format failed");
                PRINT_INFO_STR("\r\nMedia formatted successfully");
                break;
            }

            case REQUEST_DIR_CREATE:
            {
                PRINT_INFO_STR("\r\nCreate a new directory...");
                status = dir_create_operation();
                TERM_ERR_TRAP(FX_SUCCESS != status, status, "\r\ndir_create_operation failed");
                break;
            }

            case REQUEST_DIR_PROPERTY:
            {
                PRINT_INFO_STR("\r\nGet directory properties...");
                status = dir_get_property_operation();
                TERM_ERR_TRAP(FX_SUCCESS != status, status, "\r\ndir_get_property_operation failed");
                break;
            }

            case REQUEST_DIR_DELETE:
            {
                PRINT_INFO_STR("\r\nDelete a directory...");
                status = dir_delete_operation();
                TERM_ERR_TRAP(FX_SUCCESS != status, status, "\r\ndir_delete_operation failed");
                break;
            }

            case REQUEST_FILE_CREATE:
            {
                PRINT_INFO_STR("\r\nCreate a new file...");
                status = file_create_operation();
                TERM_ERR_TRAP(FX_SUCCESS != status, status, "\r\nfile_create_operation failed");
                break;
            }

            case REQUEST_FILE_WRITE:
            {
                PRINT_INFO_STR("\r\nWrite to a file...");
                status = file_write_operation();
                TERM_ERR_TRAP(FX_SUCCESS != status, status, "\r\nfile_write_operation failed");
                break;
            }

            case REQUEST_FILE_READ:
            {
                PRINT_INFO_STR("\r\nRead from a file...");
                status = file_read_operation();
                TERM_ERR_TRAP(FX_SUCCESS != status, status, "\r\nfile_read_operation failed");
                break;
            }

            case REQUEST_FILE_DELETE:
            {
                PRINT_INFO_STR("\r\nDelete a file...");
                status = file_delete_operation();
                TERM_ERR_TRAP(FX_SUCCESS != status, status, "\r\nfile_delete_operation failed");
                break;
            }

            default:
            {
                PRINT_INFO_STR("\r\nInvalid input. Please select a valid option!");
                break;
            }
        }
        tx_thread_sleep(1);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function creates a new directory.
 * @param[in] None.
 * @retval    FX_SUCCESS                Created successfully.
 * @retval    Any other error codes     Failed to create.
 **********************************************************************************************************************/
static UINT dir_create_operation(void)
{
    UINT status = FX_SUCCESS;
    entry_info_t entry = {.name = {RESET_VALUE}, .attr = RESET_VALUE, .size = RESET_VALUE,
                          .time = {.month = RESET_VALUE, .date = RESET_VALUE, .year = RESET_VALUE,
                                   .hour = RESET_VALUE, .min = RESET_VALUE, .sec = RESET_VALUE}};

    /* Create directory using Azure FileX API */
    status = dir_create(s_dir_name);
    if (FX_ALREADY_CREATED == status)
    {
        PRINT_INFO_STR("\r\nDirectory already exists");
        return FX_SUCCESS;
    }
    if (FX_SUCCESS != status)
    {
        return status;
    }

    /* Flush data into the physical media and get entry information */
    status = created_entry_info_get(&entry, s_dir_name);
    if (FX_SUCCESS != status)
    {
        return status;
    }
    PRINT_ENTRY_INFO(entry);
    PRINT_INFO_STR("\r\nDirectory created successfully");

    return status;
}

/*******************************************************************************************************************//**
 * @brief     This function gets the properties of a directory.
 * @param[in] None.
 * @retval    FX_SUCCESS                Got successfully.
 * @retval    Any other error codes     Failed to get.
 **********************************************************************************************************************/
static UINT dir_get_property_operation(void)
{
    UINT status = FX_SUCCESS;
    dir_property_t dir = {.file = RESET_VALUE, .size = RESET_VALUE, .subdir = RESET_VALUE};
    entry_info_t entry = {.name = {RESET_VALUE}, .attr = RESET_VALUE, .size = RESET_VALUE,
                          .time = {.month = RESET_VALUE, .date = RESET_VALUE, .year = RESET_VALUE,
                                   .hour = RESET_VALUE, .min = RESET_VALUE, .sec = RESET_VALUE}};

    /* Extract first entry details in the current directory */
    status = dir_first_full_entry_find(&entry);
    if (FX_NO_MORE_ENTRIES == status)
    {
        PRINT_INFO_STR("\r\nDirectory is empty");
        return FX_SUCCESS;
    }
    if (FX_SUCCESS != status)
    {
        return status;
    }

    /* Print root directory name */
    PRINT_INFO_STR("\r\nDirectory contents:");
    do
    {
        if (FX_DIRECTORY == (entry.attr & FX_DIRECTORY))
        {
            PRINT_ENTRY_INFO(entry);
            dir.subdir++; /* Count number of directories */
        }
        else if (FX_ARCHIVE == (entry.attr & FX_ARCHIVE))
        {
            PRINT_ENTRY_INFO(entry);
            dir.size += entry.size; /* Calculate total files size */
            dir.file++;             /* Count number of files */
        }
        else
        {
            /* Do nothing */
        }

        /* Extract details of next entry in the root directory */
        status = dir_next_full_entry_find(&entry);
        if ((FX_SUCCESS != status) && (FX_NO_MORE_ENTRIES != status))
        {
            return status;
        }
    } while (FX_NO_MORE_ENTRIES != status);

    /* Print number of file and total file size */
    PRINT_DIR_PROPERTY(dir);
    PRINT_INFO_STR("\r\nGot directory properties successfully");

    return FX_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief     This function deletes a directory.
 * @param[in] None.
 * @retval    FX_SUCCESS                Deleted successfully.
 * @retval    Any other error codes     Failed to delete.
 **********************************************************************************************************************/
static UINT dir_delete_operation(void)
{
    UINT status = FX_SUCCESS;

    /* Delete a directory using Azure FileX API */
    status = dir_delete(s_dir_name);
    if (FX_NOT_FOUND == status)
    {
        PRINT_INFO_STR("\r\nDirectory does not exist");
        return FX_SUCCESS;
    }
    if (FX_DIR_NOT_EMPTY == status)
    {
        PRINT_INFO_STR("\r\nDirectory is not empty");
        return FX_SUCCESS;
    }
    if (FX_SUCCESS != status)
    {
        return status;
    }

    /* Flush data to physical media */
    status = media_flush();
    if (FX_SUCCESS != status)
    {
        return status;
    }
    PRINT_INFO_STR("\r\nDeleted a directory successfully");

    return status;
}

/*******************************************************************************************************************//**
 * @brief     This function creates a new file.
 * @param[in] None.
 * @retval    FX_SUCCESS                Created successfully.
 * @retval    Any other error codes     Failed to create.
 **********************************************************************************************************************/
static UINT file_create_operation(void)
{
    UINT status = FX_SUCCESS;
    entry_info_t entry = {.name = {RESET_VALUE}, .attr = RESET_VALUE, .size = RESET_VALUE,
                          .time = {.month = RESET_VALUE, .date = RESET_VALUE, .year = RESET_VALUE,
                                   .hour = RESET_VALUE, .min = RESET_VALUE, .sec = RESET_VALUE}};

    /* Create a new file using the Azure FileX API */
    status = file_create(s_file_name);
    if (FX_ALREADY_CREATED == status)
    {
        PRINT_INFO_STR("\r\nFile already exists");
        return FX_SUCCESS;
    }
    if (FX_SUCCESS != status)
    {
        return status;
    }

    /* Flush data into the physical media and retrieve a file full information */
    status = created_entry_info_get(&entry, s_file_name);
    if (FX_SUCCESS != status)
    {
        return status;
    }
    PRINT_ENTRY_INFO(entry);
    PRINT_INFO_STR("\r\nFile created successfully");

    return status;
}

/*******************************************************************************************************************//**
 * @brief     This function writes fixed data to a file.
 * @param[in] None.
 * @retval    FX_SUCCESS                Written successfully.
 * @retval    Any other error codes     Failed to write.
 **********************************************************************************************************************/
static UINT file_write_operation(void)
{
    UINT status = FX_SUCCESS;
    FX_FILE file = {.fx_file_name = RESET_VALUE};
    time_app_t time = {.month = RESET_VALUE, .date = RESET_VALUE, .year = RESET_VALUE,
                       .hour = RESET_VALUE, .min = RESET_VALUE, .sec = RESET_VALUE};
    entry_info_t entry = {.name = {RESET_VALUE}, .attr = RESET_VALUE, .size = RESET_VALUE,
                          .time = {.month = RESET_VALUE, .date = RESET_VALUE, .year = RESET_VALUE,
                                   .hour = RESET_VALUE, .min = RESET_VALUE, .sec = RESET_VALUE}};

    /* Open the file for writing by using the Azure FileX API */
    status = file_open(&file, s_file_name, FX_OPEN_FOR_WRITE);
    if (FX_NOT_FOUND == status)
    {
        PRINT_INFO_STR("\r\nFile does not exist");
        return FX_SUCCESS;
    }
    if (FX_SUCCESS != status)
    {
        return status;
    }

    /* Write content to the opened file */
    status = file_write(&file, (VOID *)s_write_data, s_file_name, &time);
    if (FX_SUCCESS != status)
    {
        file_close(&file);
        return status;
    }
    file_close(&file);

    /* Flush data into the physical media and get file full information */
    status = created_entry_info_get(&entry, s_file_name);
    if (FX_SUCCESS != status)
    {
        return status;
    }
    PRINT_ENTRY_INFO(entry);
    PRINT_INFO_STR("\r\nWrote to a file successfully");

    return status;
}

/*******************************************************************************************************************//**
 * @brief     This function reads data from a file.
 * @param[in] None.
 * @retval    FX_SUCCESS                Read successfully.
 * @retval    Any other error codes     Failed to read.
 **********************************************************************************************************************/
static UINT file_read_operation(void)
{
    UINT status = FX_SUCCESS;
    FX_FILE file = {.fx_file_name = RESET_VALUE};
    ULONG len = RESET_VALUE;
    entry_info_t entry = {.name = {RESET_VALUE}, .attr = RESET_VALUE, .size = RESET_VALUE,
                          .time = {.month = RESET_VALUE, .date = RESET_VALUE, .year = RESET_VALUE,
                                   .hour = RESET_VALUE, .min = RESET_VALUE, .sec = RESET_VALUE}};
    CHAR read_data[READ_BUFFER_SIZE + 1U] = {RESET_VALUE};

    /* Open the file for reading by using the Azure FileX API */
    status = file_open(&file, s_file_name, FX_OPEN_FOR_READ);
    if (FX_NOT_FOUND == status)
    {
        PRINT_INFO_STR("\r\nFile does not exist");
        return FX_SUCCESS;
    }
    if (FX_SUCCESS != status)
    {
        return status;
    }

    /* Seek to the beginning of the file */
    status = file_seek(&file);
    if (FX_SUCCESS != status)
    {
        file_close(&file);
        return status;
    }

    /* Read data from the file */
    status = file_read(&file, read_data, &len);
    if ((FX_END_OF_FILE != status) && (FX_SUCCESS != status))
    {
        file_close(&file);
        return status;
    }
    if (RESET_VALUE == len)
    {
        file_close(&file);
        PRINT_INFO_STR("\r\nFile is empty");
        return FX_SUCCESS;
    }

    /* Wait until the complete event flag is received */
    status = wait_event_get();
    if (TX_SUCCESS != status)
    {
        file_close(&file);
        return status;
    }
    file_close(&file);

    /* Retrieve file full information */
    status = dir_info(&entry, s_file_name);
    if (FX_SUCCESS != status)
    {
        return status;
    }

    /* Display file information */
    PRINT_ENTRY_INFO(entry);

    if (READ_BUFFER_SIZE > len)
    {
        /* Display content of the file */
        PRINT_INFO_STR("\r\nContent of the file:");
        term_print_data(TERM_OP_MSG_INFO_STR, len + 1U, read_data);
    }
    else
    {
        /* Display content of the first 1 kB of the file */
        PRINT_INFO_STR("\r\nContent of the first 1 kB of the file:");
        term_print_data(TERM_OP_MSG_INFO_STR, READ_BUFFER_SIZE + 1U, read_data);
    }

    PRINT_INFO_STR("\r\nRead from a file successfully");

    return status;
}

/*******************************************************************************************************************//**
 * @brief     This function deletes a file.
 * @param[in] None.
 * @retval    FX_SUCCESS                Deleted successfully.
 * @retval    Any other error codes     Failed to delete.
 **********************************************************************************************************************/
static UINT file_delete_operation(void)
{
    UINT status = FX_SUCCESS;

    /* Delete the file using the Azure FileX API */
    status = file_delete(s_file_name);
    if (FX_NOT_FOUND == status)
    {
        PRINT_INFO_STR("\r\nFile does not exist");
        return FX_SUCCESS;
    }
    if (FX_SUCCESS != status)
    {
        return status;
    }

    /* Flush data into the physical media */
    status = media_flush();
    if (FX_SUCCESS != status)
    {
        return status;
    }

    PRINT_INFO_STR("\r\nDeleted a file successfully");

    return status;
}
