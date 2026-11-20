/***********************************************************************************************************************
 * File Name    : filex_example_app.c
 * Description  : Contains data structures and functions used in the application layer of the EP.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ***********************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "filex_example_app.h"
#include "board_ospi.h"

/* Private variables */
static uint8_t s_fx_media_media_memory[G_FX_MEDIA_MEDIA_MEMORY_SIZE];
static CHAR s_dir_name[] = DIR_NAME_ONE;
static CHAR s_file_name[] = FILE_NAME_ONE;
static CHAR s_write_data[WRITE_BUFFER_SIZE] = {RESET_VALUE};

/* Private functions */
static void erase_flash_operation(void);
static void media_format_operation(void);
static void media_open_operation(void);
static void defragment_flash_operation(void);
static void dir_create_operation(void);
static void dir_get_property(void);
static void dir_delete_operation(void);
static void file_create_operation(void);
static void file_write_operation(void);
static void file_read_operation(void);
static void file_delete_operation(void);
static void sector_write_operation(void);
static void sector_read_operation(void);
static void sector_erase_operation(void);
static void verify_input_sector(ULONG **select_sector_address, uint16_t *select_sector, bool *is_empty);
static void user_input(char *buf, int len);

/*******************************************************************************************************************//**
 * @brief     App Thread entry-point for the FileX/LevelX example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void app_thread_entry_run(void)
{
    UINT status = FX_SUCCESS;
    fsp_err_t err = FSP_SUCCESS;
    UINT lx_ret_val = LX_SUCCESS;
    ULONG actual_event = RESET_VALUE;

    /* Suspend thread until received start App Thread event flag */
    status = get_tx_flags(REQUEST_START_APP_THREAD, &actual_event);
    APP_ERR_CHECK(status, TX_SUCCESS, "get_tx_flags for REQUEST_START_APP_THREAD failed\r\n");

    /* Initialize the OSPI Flash */
    err = ospi_b_init();
    APP_ERR_CHECK(err, FSP_SUCCESS, "ospi_b_init failed\r\n");

    /* Initialize the FileX file system */
    status = file_system_init();
    APP_ERR_CHECK(status, FX_SUCCESS, "file_system_init failed\r\n");

    /* Initialize the LevelX system */
    lx_ret_val = levelx_init();
    APP_ERR_CHECK(lx_ret_val, LX_SUCCESS, "levelx_init failed\r\n");

    while (true)
    {
        /* Wait for a request event from the console thread */
        status = get_tx_flags(FILE_SYSTEM_REQUEST_MASK, &actual_event);
        APP_ERR_CHECK(status, TX_SUCCESS, "get_tx_flags for FILE_SYSTEM_REQUEST_MASK failed\r\n");

        /* Perform file system operations as required by the user */
        switch (actual_event)
        {
            case REQUEST_ERASE_FLASH:
            {
                erase_flash_operation();
                break;
            }

            case REQUEST_MEDIA_FORMAT:
            {
                media_format_operation();
                break;
            }

            case REQUEST_MEDIA_OPEN:
            {
                media_open_operation();
                break;
            }

            case REQUEST_DEFRAGMENT_FLASH:
            {
                defragment_flash_operation();
                break;
            }

            case REQUEST_DIR_CREATE:
            {
                dir_create_operation();
                break;
            }

            case REQUEST_DIR_PROPERTY:
            {
                dir_get_property();
                break;
            }

            case REQUEST_DIR_DELETE:
            {
                dir_delete_operation();
                break;
            }

            case REQUEST_FILE_CREATE:
            {
                file_create_operation();
                break;
            }

            case REQUEST_FILE_WRITE:
            {
                file_write_operation();
                break;
            }

            case REQUEST_FILE_READ:
            {
                file_read_operation();
                break;
            }

            case REQUEST_FILE_DELETE:
            {
                file_delete_operation();
                break;
            }

            case REQUEST_SECTOR_WRITE:
            {
                sector_write_operation();
                break;
            }

            case REQUEST_SECTOR_READ:
            {
                sector_read_operation();
                break;
            }

            case REQUEST_SECTOR_ERASE:
            {
                sector_erase_operation();
                break;
            }

            default:
            {
                PRINT_INFO_STR("\r\nInvalid input\r\n\r\n");
                break;
            }
        }

        /* Send a request complete event flag to the console thread */
        status = set_tx_flags(REQUEST_COMPLETED);
        APP_ERR_CHECK(status, TX_SUCCESS, "set_tx_flags for REQUEST_COMPLETED failed\r\n");

        thread_sleep(APP_SLEEP_TICK);
    }
}
/***********************************************************************************************************************
* End of function app_thread_entry_run
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: erase_flash_operation
 * Description  : This function performs erasing flash operation.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
static void erase_flash_operation(void)
{
    UINT status = FX_SUCCESS;

    PRINT_INFO_STR("\r\n\r\nErasing OSPI Flash ...\r\n");
    PRINT_INFO_STR(MSG_OPERATION_LONG);

    /* Erase the flash */
    status = (UINT)erase_ospi_flash();
    APP_ERR_CHECK(status, FX_SUCCESS, "erase_ospi_flash failed\r\n");
    PRINT_INFO_STR("\r\nErased OSPI Flash successfully!!!");
    PRINT_INFO_STR("\r\nPlease format media after erased OSPI Flash\r\n");
}
/***********************************************************************************************************************
* End of function erase_flash_operation
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: media_format_operation
 * Description  : This function performs formatting the media operation.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
static void media_format_operation(void)
{
    UINT status = FX_SUCCESS;

    PRINT_INFO_STR("\r\n\r\nErasing the sectors before use for the file system ...\r\n");
    PRINT_INFO_STR(MSG_OPERATION_LONG);

    /* Close media before format */
    status = media_close();
    if (FX_SUCCESS != status && FX_MEDIA_NOT_OPEN != status)
    {
        PRINT_ERR_STR("media_close failed\r\n");
        PRINT_ERROR_TRAP(status);
    }

    /* Clear the media opened flag after close media */
    g_media_opened = false;

    /* Erase the flash sectors prior to usage for file system operations */
    status = (UINT)erase_ospi_flash_range((uint8_t *) FILEX_START_ADDRESS, g_rm_levelx_nor_spi_cfg.size);
    APP_ERR_CHECK(status, FX_SUCCESS, "erase_ospi_flash_range failed\r\n");
    PRINT_INFO_STR("\r\nErased the sectors successfully!!!\r\n");

    /* Format the media */
    PRINT_INFO_STR("\r\nFormatting media ...\r\n");
    PRINT_INFO_STR(MSG_OPERATION_LONG);
    status = media_format(s_fx_media_media_memory);
    APP_ERR_CHECK(status, FX_SUCCESS, "media_format failed\r\n");
    PRINT_INFO_STR("\r\nMedia has been formatted successfully!!!\r\n");
}
/***********************************************************************************************************************
* End of function media_format_operation
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: media_open_operation
 * Description  : This function performs opening the media operation.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
static void media_open_operation(void)
{
    UINT status = FX_SUCCESS;
    media_property_t media =
    {
        .phy_size = RESET_VALUE,
        .total_size = RESET_VALUE,
        .free_size = RESET_VALUE,
        .format_type = RESET_VALUE
    };

    PRINT_INFO_STR("\r\n\r\nOpening media ...\r\n");
    PRINT_INFO_STR(MSG_OPERATION_LONG);

    /* Check media opened flag */
    if (true == g_media_opened)
    {
        PRINT_INFO_STR("Media has already been opened\r\n");
        return;
    }

    /* Open media using the Azure FileX API */
    status = media_open(s_fx_media_media_memory);
    if (FX_SUCCESS != status)
    {
        PRINT_INFO_STR("Media open failed, please format media\r\n");
        return;
    }

    /* Set the media opened status */
    g_media_opened = true;

    /* Get the free space size of the media */
    status = media_extended_space_available(&media.free_size);
    APP_ERR_CHECK(status, FX_SUCCESS, "media_extended_space_available failed\r\n");

    /* Calculate the total size of the media */
    media.total_size = (ULONG64)g_fx_media.fx_media_total_clusters *
                       (ULONG64)g_fx_media.fx_media_sectors_per_cluster *
                       (ULONG64)g_fx_media.fx_media_bytes_per_sector;

    /* Get the format type of the media */
    if (g_fx_media.fx_media_12_bit_FAT)
    {
        media.format_type = "FAT 12";
    }
    else if (g_fx_media.fx_media_32_bit_FAT)
    {
        media.format_type = "FAT 32";
    }
    else
    {
        media.format_type = "FAT 16";
    }

    /* Display media information */
    PRINT_MEDIA_PROPERTY(media);
    PRINT_INFO_STR("\r\nMedia has been opened successfully!!!\r\n");
}
/***********************************************************************************************************************
* End of function media_open_operation
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: defragment_flash_operation
 * Description  : This function defragments the Flash.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
static void defragment_flash_operation(void)
{
    UINT lx_err = LX_SUCCESS;

    /* Check media opened flag */
    if (false == g_media_opened)
    {
        PRINT_INFO_STR(MSG_MEDIA_NOT_OPENED);
        return;
    }

    PRINT_INFO_STR("\r\n\r\nDefragmenting Flash ...\r\n");
    PRINT_INFO_STR(MSG_OPERATION_LONG);

    /* Defragment sectors of the NOR flash */
    lx_err = defragment_flash();
    APP_ERR_CHECK(lx_err, LX_SUCCESS, "defragment_flash failed\r\n");

    /* Defragment flash successfully */
    PRINT_INFO_STR("\r\nFlash has been defragmented successfully!!!\r\n");
}
/***********************************************************************************************************************
* End of function defragment_flash_operation
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: dir_create_operation
 * Description  : This function performs creating a new directory operation.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
static void dir_create_operation(void)
{
    UINT status = FX_SUCCESS;
    entry_info_t entry =
    {
        .name = {NULL_CHAR},
        .attr = RESET_VALUE,
        .size = RESET_VALUE,
        .time =
        {
            .month = RESET_VALUE,
            .date = RESET_VALUE,
            .year = RESET_VALUE,
            .hour = RESET_VALUE,
            .min = RESET_VALUE,
            .sec = RESET_VALUE
        }
    };
    sector_info_t sector =
    {
        .sector_map_entry = NULL,
        .sector_address = NULL,
        .logical_sector = RESET_VALUE
    };

    /* Check media opened flag */
    if (false == g_media_opened)
    {
        PRINT_INFO_STR(MSG_MEDIA_NOT_OPENED);
        return;
    }

    PRINT_INFO_STR("\r\n\r\nCreating a new directory ...\r\n\r\n");

    /* Create directory using Azure FileX API */
    status = dir_create(s_dir_name);
    if (FX_ALREADY_CREATED == status)
    {
        PRINT_INFO_STR("Directory already exists\r\n");
        return;
    }
    APP_ERR_CHECK(status, FX_SUCCESS, "dir_create failed\r\n");

    sector.logical_sector = g_fx_media.fx_media_driver_logical_sector;

    /* Get entry information */
    status = created_entry_info_get(&entry, s_dir_name, &sector);
    APP_ERR_CHECK(status, FX_SUCCESS, "created_entry_info_get failed\r\n");

    PRINT_ENTRY_INFO(entry);
    PRINT_SECTOR_INFO(sector);
    PRINT_INFO_STR("\r\n\r\nDirectory created successfully!!!\r\n");
}
/***********************************************************************************************************************
* End of function dir_create_operation
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: dir_get_property
 * Description  : This function performs getting the properties of a directory operation.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
static void dir_get_property(void)
{
    UINT status = FX_SUCCESS;
    dir_property_t dir =
    {
        .file = RESET_VALUE,
        .subdir = RESET_VALUE,
        .size = RESET_VALUE
    };
    entry_info_t entry =
    {
        .name = {NULL_CHAR},
        .attr = RESET_VALUE,
        .size = RESET_VALUE,
        .time =
        {
            .month = RESET_VALUE,
            .date = RESET_VALUE,
            .year = RESET_VALUE,
            .hour = RESET_VALUE,
            .min = RESET_VALUE,
            .sec = RESET_VALUE
        }
    };

    /* Check media opened flag */
    if (false == g_media_opened)
    {
        PRINT_INFO_STR(MSG_MEDIA_NOT_OPENED);
        return;
    }

    PRINT_INFO_STR("\r\n\r\nGetting directory properties ...\r\n");

    /* Extract first entry details in the current directory */
    status = dir_first_full_entry_find(&entry);
    if (FX_NO_MORE_ENTRIES == status)
    {
        PRINT_INFO_STR("\r\nDirectory empty\r\n");
        return;
    }
    APP_ERR_CHECK(status, FX_SUCCESS, "dir_first_full_entry_find failed\r\n");

    /* Print root directory name */
    PRINT_INFO_STR("\r\nDirectory of :\\ \r\n\r\n");

    /* Check all entries in root directory */
    do
    {
        if (FX_DIRECTORY == (entry.attr & FX_DIRECTORY))
        {
            PRINT_ENTRY_INFO(entry);
            dir.subdir++;               /* Count number of directories */
        }
        else if (FX_ARCHIVE == (entry.attr & FX_ARCHIVE))
        {
            PRINT_ENTRY_INFO(entry);
            dir.size += entry.size;     /* Calculate total files size */
            dir.file++;                 /* Count number of files */
        }
        else
        {
            /* Do nothing */
        }

        /* Extract details of next entry in the root directory */
        status = dir_next_full_entry_find(&entry);
        if ((FX_SUCCESS != status) && (FX_NO_MORE_ENTRIES != status))
        {
            PRINT_ERR_STR("dir_next_full_entry_find failed\r\n");
            PRINT_ERROR_TRAP(status);
        }
    }
    while (FX_NO_MORE_ENTRIES != status);

    PRINT_DIR_PROPERTY(dir);
    PRINT_INFO_STR("Got directory properties successfully!!!\r\n");
}
/***********************************************************************************************************************
* End of function dir_get_property
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: dir_delete_operation
 * Description  : This function performs deleting a directory operation.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
static void dir_delete_operation(void)
{
    UINT status = FX_SUCCESS;

    /* Check media opened flag */
    if (false == g_media_opened)
    {
        PRINT_INFO_STR(MSG_MEDIA_NOT_OPENED);
        return;
    }

    PRINT_INFO_STR("\r\n\r\nDeleting a directory ...\r\n");

    /* Delete a directory using Azure FileX API */
    status = dir_delete(s_dir_name);
    if (FX_NOT_FOUND == status)
    {
        PRINT_INFO_STR("Directory not exists\r\n");
        return;
    }
    if (FX_DIR_NOT_EMPTY == status)
    {
        PRINT_INFO_STR("Directory not empty\r\n");
        return;
    }
    APP_ERR_CHECK(status, FX_SUCCESS, "dir_delete failed\r\n");

    /* Flush data to physical media */
    status = (ULONG) media_flush();
    APP_ERR_CHECK(status, FX_SUCCESS, "media_flush failed\r\n");

    /* Delete a directory successfully */
    PRINT_INFO_STR("\r\nDirectory has been deleted successfully!!!\r\n");
}
/***********************************************************************************************************************
* End of function dir_delete_operation
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: file_create_operation
 * Description  : This function performs creating a new file operation.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
static void file_create_operation(void)
{
    UINT status = FX_SUCCESS;
    entry_info_t entry =
    {
        .name = {NULL_CHAR},
        .attr = RESET_VALUE,
        .size = RESET_VALUE,
        .time =
        {
            .month = RESET_VALUE,
            .date = RESET_VALUE,
            .year = RESET_VALUE,
            .hour = RESET_VALUE,
            .min = RESET_VALUE,
            .sec = RESET_VALUE
        }
    };
    sector_info_t sector =
    {
        .sector_map_entry = NULL,
        .sector_address = NULL,
        .logical_sector = RESET_VALUE
    };

    /* Check media opened flag */
    if (false == g_media_opened)
    {
        PRINT_INFO_STR(MSG_MEDIA_NOT_OPENED);
        return;
    }

    PRINT_INFO_STR("\r\n\r\nCreating a new file ...\r\n\r\n");

    /* Create a new file using the Azure FileX API */
    status = file_create(s_file_name);
    if (FX_ALREADY_CREATED == status)
    {
        PRINT_INFO_STR("File already exists\r\n");
        return;
    }
    APP_ERR_CHECK(status, FX_SUCCESS, "file_create failed\r\n");

    sector.logical_sector = g_fx_media.fx_media_driver_logical_sector;

    /* Get entry information */
    status = created_entry_info_get(&entry, s_file_name, &sector);
    APP_ERR_CHECK(status, FX_SUCCESS, "created_entry_info_get failed\r\n");

    PRINT_ENTRY_INFO(entry);
    PRINT_SECTOR_INFO(sector);
    PRINT_INFO_STR("\r\n\r\nFile created successfully!!!\r\n");
}
/***********************************************************************************************************************
* End of function file_create_operation
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: file_write_operation
 * Description  : This function performs writing fixed data to a file operation.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
static void file_write_operation(void)
{
    UINT status = FX_SUCCESS;
    FX_FILE file;
    memset(&file, RESET_VALUE, sizeof(file));
    time_t time =
    {
        .month = RESET_VALUE,
        .date = RESET_VALUE,
        .year = RESET_VALUE,
        .hour = RESET_VALUE,
        .min = RESET_VALUE,
        .sec = RESET_VALUE
    };
    entry_info_t entry =
    {
        .name = {NULL_CHAR},
        .attr = RESET_VALUE,
        .size = RESET_VALUE,
        .time =
        {
            .month = RESET_VALUE,
            .date = RESET_VALUE,
            .year = RESET_VALUE,
            .hour = RESET_VALUE,
            .min = RESET_VALUE,
            .sec = RESET_VALUE
        }
    };
    sector_info_t sector =
    {
        .sector_map_entry = NULL,
        .sector_address = NULL,
        .logical_sector = RESET_VALUE
    };

    /* Check media opened flag */
    if (false == g_media_opened)
    {
        PRINT_INFO_STR(MSG_MEDIA_NOT_OPENED);
        return;
    }

    PRINT_INFO_STR("\r\n\r\nWriting to a file ...\r\n\r\n");

    /* Open the file for writing by using the Azure FileX API */
    status = file_open(&file, s_file_name, FX_OPEN_FOR_WRITE);
    if (FX_NOT_FOUND == status)
    {
        PRINT_INFO_STR("File does not exist\r\n");
        return;
    }
    APP_ERR_CHECK(status, FX_SUCCESS, "file_open failed\r\n");

    /* Write content to the opened file */
    status = file_write(&file, (VOID *)s_write_data, s_file_name, &time);
    if (FX_SUCCESS != status)
    {
        file_close(&file);
        PRINT_ERR_STR("file_write failed\r\n");
        PRINT_ERROR_TRAP(status);
    }
    file_close(&file);

    sector.logical_sector = g_fx_media.fx_media_driver_logical_sector;

    /* Get entry information */
    status = created_entry_info_get(&entry, s_file_name, &sector);
    APP_ERR_CHECK(status, FX_SUCCESS, "created_entry_info_get failed\r\n");

    PRINT_ENTRY_INFO(entry);
    PRINT_SECTOR_INFO(sector);
    PRINT_INFO_STR("\r\nWrote to a file successfully!!!\r\n");
}
/***********************************************************************************************************************
* End of function file_write_operation
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: file_read_operation
 * Description  : This function performs reading and verifying data from a file operation.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
static void file_read_operation(void)
{
    UINT status = FX_SUCCESS;
    FX_FILE file;
    memset(&file, RESET_VALUE, sizeof(file));
    ULONG len = RESET_VALUE;
    entry_info_t entry =
    {
        .name = {NULL_CHAR},
        .attr = RESET_VALUE,
        .size = RESET_VALUE,
        .time =
        {
            .month = RESET_VALUE,
            .date = RESET_VALUE,
            .year = RESET_VALUE,
            .hour = RESET_VALUE,
            .min = RESET_VALUE,
            .sec = RESET_VALUE
        }
    };
    CHAR read_data[READ_BUFFER_SIZE + ONE_BYTE] = {RESET_VALUE};

    /* Check media opened flag */
    if (false == g_media_opened)
    {
        PRINT_INFO_STR(MSG_MEDIA_NOT_OPENED);
        return;
    }

    PRINT_INFO_STR("\r\n\r\nReading from a file ...\r\n\r\n");

    /* Open the file for reading by using the Azure FileX API */
    status = file_open(&file, s_file_name, FX_OPEN_FOR_READ);
    if (FX_NOT_FOUND == status)
    {
        PRINT_INFO_STR("File does not exist\r\n");
        return;
    }
    APP_ERR_CHECK(status, FX_SUCCESS, "file_open failed\r\n");

    /* Seek to the beginning of the file  */
    status = file_seek(&file);
    if (FX_SUCCESS != status)
    {
       file_close(&file);
       PRINT_ERR_STR("file_seek failed\r\n");
       PRINT_ERROR_TRAP(status);
    }

    /* Read data from the file */
    status = file_read(&file, read_data, &len);
    if (FX_END_OF_FILE != status && FX_SUCCESS != status)
    {
        file_close(&file);
        PRINT_ERR_STR("file_read failed\r\n");
        PRINT_ERROR_TRAP(status);
    }

    /* Close the file using the Azure FileX API */
    file_close(&file);

    /* Retrieve file name */
    strcpy(entry.name, s_file_name);
    status = dir_info(&entry);
    APP_ERR_CHECK(status, FX_SUCCESS, "dir_info failed\r\n");
    PRINT_ENTRY_INFO(entry);

    if (READ_BUFFER_SIZE > len)
    {
        /* Display content of the file */
        PRINT_INFO_STR("\r\nContent of the file\r\n\r\n");
        term_send_output_queue(TERMINAL_OUTPUT_APP_INFO_STR, read_data, len + ONE_BYTE);
    }
    else
    {
        /* Display content of the first 1 KB of the file */
        PRINT_INFO_STR("\r\nContent of the first 1 KB of the file\r\n\r\n");
        term_send_output_queue(TERMINAL_OUTPUT_APP_INFO_STR, read_data, READ_BUFFER_SIZE + ONE_BYTE);
    }

    /* Read a file successfully */
    PRINT_INFO_STR("\r\nRead a file successfully!!!\r\n");
}
/***********************************************************************************************************************
* End of function file_read_operation
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: file_delete_operation
 * Description  : This function performs deleting a file operation.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
static void file_delete_operation(void)
{
    UINT status = FX_SUCCESS;

    /* Check media opened flag */
    if (false == g_media_opened)
    {
        PRINT_INFO_STR(MSG_MEDIA_NOT_OPENED);
        return;
    }

    PRINT_INFO_STR("\r\n\r\nDeleting a file ...\r\n");

    /* Delete the file using the Azure FileX API */
    status = file_delete(s_file_name);
    if (FX_NOT_FOUND == status)
    {
        PRINT_INFO_STR("File does not exist\r\n");
        return;
    }
    APP_ERR_CHECK(status, FX_SUCCESS, "file_delete failed\r\n");

    /* Flush data into the physical media */
    status = media_flush();
    APP_ERR_CHECK(status, FX_SUCCESS, "media_flush failed\r\n");

    /* Delete the file successfully */
    PRINT_INFO_STR("\r\nDeleted a file successfully!!!\r\n");
}
/***********************************************************************************************************************
* End of function file_delete_operation
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: sector_write_operation
 * Description  : This function performs writing data to selected sector operation.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
static void sector_write_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint16_t selected_sector = RESET_VALUE;
    CHAR sector_write_data[SECTOR_WRITE_SIZE];
    sector_info_t sector =
    {
        .sector_map_entry = NULL,
        .sector_address = NULL,
        .logical_sector = RESET_VALUE
    };
    bool is_empty = true;

    /* Check media opened flag */
    if (false == g_media_opened)
    {
        PRINT_INFO_STR(MSG_MEDIA_NOT_OPENED);
        return;
    }

    /* Verify the input sector and retrieve its address */
    verify_input_sector(&sector.sector_address, &selected_sector, &is_empty);

    if (is_empty)
    {
        PRINT_INFO_STR("\r\n\r\nWriting to the selected sector ...\r\n");

        /* Create sector_write_data buffer according to selected sector */
        snprintf(sector_write_data, SECTOR_WRITE_SIZE, "Sector %u: %s", selected_sector, SECTOR_WRITE_TEXT);

        /* Write to selected sector */
        err = sector_write(sector.sector_address, (ULONG *)sector_write_data);
        APP_ERR_CHECK(err, FSP_SUCCESS, "sector_write failed\r\n");
        PRINT_INFO_STR("\r\nWrote to a sector successfully!!!\r\n");
    }
    else
    {
        PRINT_INFO_STR("\r\nThe selected sector is currently in use and must be erased before writing.\r\n");
    }
}
/***********************************************************************************************************************
* End of function sector_write_operation
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: sector_read_operation
 * Description  : This function performs reading data from selected sector operation.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
static void sector_read_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;
    CHAR read_data[SECTOR_READ_SIZE] = {RESET_VALUE};
    sector_info_t sector =
    {
        .sector_map_entry = NULL,
        .sector_address = NULL,
        .logical_sector = RESET_VALUE
    };
    uint16_t selected_sector = RESET_VALUE;
    bool is_empty = true;

    /* Check media opened flag */
    if (false == g_media_opened)
    {
        PRINT_INFO_STR(MSG_MEDIA_NOT_OPENED);
        return;
    }

    /* Verify input sector, convert to address, and check if it's empty */
    verify_input_sector(&sector.sector_address, &selected_sector, &is_empty);

    if (!is_empty)
    {
        PRINT_INFO_STR("\r\n\r\nReading from the selected sector ...\r\n");

        /* Read from selected sector */
        err = sector_read(sector.sector_address, (ULONG *)read_data);
        APP_ERR_CHECK(err, FSP_SUCCESS, "sector_read failed\r\n");

        /* Add NULL char to terminate the read_data */
        read_data[SECTOR_READ_SIZE - 1] = NULL_CHAR;

        PRINT_INFO_STR("\r\nContent of this sector:\r\n");
        PRINT_INFO_STR(read_data);
        PRINT_INFO_STR("\r\nRead from a sector successfully!!!\r\n");
    }
    else
    {
        PRINT_INFO_STR("\r\nThis sector is empty!!!\r\n");
    }

    /* Display sector information */
    PRINT_SECTOR_INFO(sector);
}
/***********************************************************************************************************************
* End of function sector_read_operation
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: sector_erase_operation
 * Description  : This function performs erasing selected sector operation.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
static void sector_erase_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;
    sector_info_t sector =
    {
        .sector_map_entry = NULL,
        .sector_address = NULL,
        .logical_sector = RESET_VALUE
    };
    uint16_t selected_sector = RESET_VALUE;
    bool is_empty = true;

    /* Check media opened flag */
    if (false == g_media_opened)
    {
        PRINT_INFO_STR(MSG_MEDIA_NOT_OPENED);
        return;
    }

    /* Verify input sector, convert to address, and check if it's empty */
    verify_input_sector(&sector.sector_address, &selected_sector, &is_empty);

    if (!is_empty)
    {
        PRINT_INFO_STR("\r\n\r\nErasing the selected sector ...\r\n");

        /* Erase selected sector */
        err = sector_erase(sector.sector_address);
        APP_ERR_CHECK(err, FSP_SUCCESS, "sector_erase failed\r\n");
        PRINT_INFO_STR("\r\nErased a sector successfully!!!\r\n");
    }
    else
    {
        PRINT_INFO_STR("\r\nThis sector is empty!!!\r\n");
    }
}
/***********************************************************************************************************************
* End of function sector_erase_operation
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: verify_input_sector
 * Description  : This function verifies input sector and converts it to corresponding address.
 * Arguments    : select_sector_address     Address of selected sector.
 *                select_sector             Selected sector.
 *                is_empty                  Status of selected sector.
 * Return Value : None.
 **********************************************************************************************************************/
static void verify_input_sector(ULONG **select_sector_address, uint16_t *select_sector, bool *is_empty)
{
    CHAR msg_buf[MSG_SIZE];
    CHAR recv_buf[TERM_BUFFER_SIZE] = {RESET_VALUE};

    /* Assume sector is empty initially */
    *is_empty = true;

    do
    {
        /* Print the valid sector range used in LevelX operation */
        snprintf(msg_buf, MSG_SIZE, "\r\nSelect the sector for operation (from %d to %d): ",
                 LEVELX_SECTOR_RANGE_START, LEVELX_SECTOR_RANGE_END);
        PRINT_INFO_STR(msg_buf);

        /* Wait user input */
        user_input(recv_buf, sizeof(recv_buf));

        /* Conversion from input string to integer value */
        *select_sector = (uint16_t)(atoi(recv_buf));

        /* Verify user input */
        if ((LEVELX_SECTOR_RANGE_START <= *select_sector) && (LEVELX_SECTOR_RANGE_END >= *select_sector))
        {
#if defined (BOARD_RA8D1_EK)
            /* Convert input to sector address */
            *select_sector_address = (ULONG *)(LEVELX_START_ADDRESS + ((*select_sector - LEVELX_SECTOR_RANGE_START) *
                                                                       OSPI_B_SECTOR_256K_SIZE));

            /* Check if sector is empty */
            for (uint32_t n = 0; n < OSPI_B_SECTOR_256K_SIZE; n++)
            {
                UCHAR *byte_address = (UCHAR *)(*select_sector_address) + n;

                if (*byte_address != NOR_SPI_FLASH_CLEARED)
                {
                    *is_empty = false;
                    break;
                }
            }
#elif defined (BOARD_RA8P1_EK) || defined (BOARD_RA8D2_EK) || defined (BOARD_RA8M2_EK) || defined (BOARD_RA8T2_EK)
            /* Convert input to sector address */
            *select_sector_address = (ULONG *)(LEVELX_START_ADDRESS + ((*select_sector - LEVELX_SECTOR_RANGE_START) *
                                                                       OSPI_B_SECTOR_4K_SIZE));

            /* Check if sector is empty */
            for (uint32_t n = 0; n < OSPI_B_SECTOR_4K_SIZE; n++)
            {
                UCHAR *byte_address = (UCHAR *)(*select_sector_address) + n;

                if (*byte_address != NOR_SPI_FLASH_CLEARED)
                {
                    *is_empty = false;
                    break;
                }
            }
#endif /* Specific board */
        }
        else
        {
            *select_sector_address = NULL;
            PRINT_INFO_STR("\r\n\r\nInvalid input. Please select another sector.\r\n");
        }
    } while (*select_sector_address == NULL);
}
/***********************************************************************************************************************
* End of function verify_input_sector
***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief         This function reads a line of user input from stdin and removes trailing newline characters.
 * @param[in,out] buf       Pointer to buffer where the input string is stored.
 * @param[in]     len       Maximum number of characters to read (buffer size).
 * @retval        None.
 **********************************************************************************************************************/
static void user_input(char *buf, int len)
{
    fgets(buf, len, stdin);
    buf[strcspn(buf, "\r\n")] = '\0';
}
/***********************************************************************************************************************
* End of function user_input
***********************************************************************************************************************/
