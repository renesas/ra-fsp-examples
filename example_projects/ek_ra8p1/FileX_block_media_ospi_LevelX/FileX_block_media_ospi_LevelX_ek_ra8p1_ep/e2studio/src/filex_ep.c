/***********************************************************************************************************************
 * File Name    : filex_ep.c
 * Description  : Contains data structures and functions used in the hardware/middleware initialization layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ***********************************************************************************************************************/

#include <stdlib.h>
#include <string.h>
#include "app_thread.h"
#include "filex_ep.h"

FX_MEDIA g_fx_media;
volatile bool g_media_opened = false;

/* Private functions */
static UINT get_month(const CHAR * p_month);
static void create_fixed_buffer(VOID *buffer_ptr);

/***********************************************************************************************************************
 * Function Name: media_close
 * Description  : This function closes the media.
 * Arguments    : None.
 * Return Value : FX_SUCCESS                Closed successfully.
 *                Any other error codes     Failed to close.
 **********************************************************************************************************************/
UINT media_close(void)
{
    return fx_media_close(&g_fx_media);
}
/***********************************************************************************************************************
* End of function media_close
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: media_format
 * Description  : This function formats the media.
 * Arguments    : None.
 * Return Value : FX_SUCCESS                Formatted successfully.
 *                Any other error codes     Failed to format.
 **********************************************************************************************************************/
UINT media_format(UCHAR * memory_ptr)
{
    return fx_media_format(&g_fx_media,                             /* Pointer to FileX media control block */
                         RM_FILEX_LEVELX_NOR_DeviceDriver,          /* Driver entry */
                         (void *) &g_rm_filex_levelx_nor_instance,  /* Pointer to LevelX NOR Driver */
                         memory_ptr,                                /* Media buffer pointer */
                         G_FX_MEDIA_MEDIA_MEMORY_SIZE,              /* Media buffer size */
                         (char *) G_FX_MEDIA_VOLUME_NAME,           /* Volume Name */
                         G_FX_MEDIA_NUMBER_OF_FATS,                 /* Number of FATs */
                         G_FX_MEDIA_DIRECTORY_ENTRIES,              /* Directory Entries */
                         G_FX_MEDIA_HIDDEN_SECTORS,                 /* Hidden sectors */
                         G_FX_MEDIA_TOTAL_SECTORS,                  /* Total sectors */
                         G_FX_MEDIA_BYTES_PER_SECTOR,               /* Sector size */
                         G_FX_MEDIA_SECTORS_PER_CLUSTER,            /* Sectors per cluster */
                         G_FX_MEDIA_HEADS,                          /* Heads (disk media) */
                         G_FX_MEDIA_SECTORS_PER_TRACK);
}
/***********************************************************************************************************************
* End of function media_format
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: media_open
 * Description  : This function opens the media.
 * Arguments    : None.
 * Return Value : FX_SUCCESS                Opened successfully.
 *                Any other error codes     Failed to open.
 **********************************************************************************************************************/
UINT media_open(VOID * memory_ptr)
{
    return fx_media_open(&g_fx_media,
                       "g_fx_media",
                       RM_FILEX_LEVELX_NOR_DeviceDriver,
                       (void *) &g_rm_filex_levelx_nor_instance,
                       memory_ptr,
                       G_FX_MEDIA_MEDIA_MEMORY_SIZE);
}
/***********************************************************************************************************************
* End of function media_open
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: media_extended_space_available
 * Description  : This function retrieves the available free space of the media.
 * Arguments    : available_bytes_ptr       Pointer to the available space in bytes.
 * Return Value : FX_SUCCESS                Retrieved successfully.
 *                Any other error codes     Failed to retrieve.
 **********************************************************************************************************************/
UINT media_extended_space_available(ULONG64 * available_bytes_ptr)
{
    return fx_media_extended_space_available(&g_fx_media, available_bytes_ptr);
}
/***********************************************************************************************************************
* End of function media_extended_space_available
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: dir_create
 * Description  : This function creates a new directory.
 * Arguments    : directory_name            Pointer to the directory name.
 * Return Value : FX_SUCCESS                Created successfully.
 *                Any other error codes     Failed to create.
 **********************************************************************************************************************/
UINT dir_create(CHAR * directory_name)
{
    return fx_directory_create(&g_fx_media, directory_name);
}
/***********************************************************************************************************************
* End of function dir_create
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: media_flush
 * Description  : This function flushes cached media data to the physical storage media.
 * Arguments    : None.
 * Return Value : FX_SUCCESS                Flushed successfully.
 *                Any other error codes     Failed to flush.
 **********************************************************************************************************************/
UINT media_flush(void)
{
    return fx_media_flush(&g_fx_media);
}
/***********************************************************************************************************************
* End of function media_flush
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: dir_info
 * Description  : This function retrieves full directory information.
 * Arguments    : p_entry                   Pointer to directory information structure.
 * Return Value : FX_SUCCESS                Retrieved successfully.
 *                Any other error codes     Failed to retrieve.
 **********************************************************************************************************************/
UINT dir_info(entry_info_t * p_entry)
{
    return fx_directory_information_get(&g_fx_media,
                                      p_entry->name,
                                      &p_entry->attr,
                                      &p_entry->size,
                                      &p_entry->time.year,
                                      &p_entry->time.month,
                                      &p_entry->time.date,
                                      &p_entry->time.hour,
                                      &p_entry->time.min,
                                      &p_entry->time.sec);
}
/***********************************************************************************************************************
* End of function dir_info
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: dir_first_full_entry_find
 * Description  : This function retrieves the first directory entry information.
 * Arguments    : p_entry                   Pointer to directory entry information.
 * Return Value : FX_SUCCESS                Retrieved successfully.
 *                Any other error codes     Failed to retrieve.
 **********************************************************************************************************************/
UINT dir_first_full_entry_find(entry_info_t * p_entry)
{
    return fx_directory_first_full_entry_find(&g_fx_media,
                                            p_entry->name,
                                            &p_entry->attr,
                                            &p_entry->size,
                                            &p_entry->time.year,
                                            &p_entry->time.month,
                                            &p_entry->time.date,
                                            &p_entry->time.hour,
                                            &p_entry->time.min,
                                            &p_entry->time.sec);
}
/***********************************************************************************************************************
* End of function dir_first_full_entry_find
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: dir_next_full_entry_find
 * Description  : This function retrieves the next directory entry information.
 * Arguments    : p_entry                   Pointer to directory entry information.
 * Return Value : FX_SUCCESS                Retrieved successfully.
 *                Any other error codes     Failed to retrieve.
 **********************************************************************************************************************/
UINT dir_next_full_entry_find(entry_info_t * p_entry)
{
    return fx_directory_next_full_entry_find(&g_fx_media,
                                             p_entry->name,
                                             &p_entry->attr,
                                             &p_entry->size,
                                             &p_entry->time.year,
                                             &p_entry->time.month,
                                             &p_entry->time.date,
                                             &p_entry->time.hour,
                                             &p_entry->time.min,
                                             &p_entry->time.sec);
}
/***********************************************************************************************************************
* End of function dir_next_full_entry_find
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: dir_delete
 * Description  : This function deletes a directory.
 * Arguments    : directory_name            Pointer to the directory name.
 * Return Value : FX_SUCCESS                Deleted successfully.
 *                Any other error codes     Failed to delete.
 **********************************************************************************************************************/
UINT dir_delete(CHAR * directory_name)
{
    return fx_directory_delete(&g_fx_media, directory_name);
}
/***********************************************************************************************************************
* End of function dir_delete
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: file_create
 * Description  : This function creates a new file.
 * Arguments    : file_name                 Pointer to the file name.
 * Return Value : FX_SUCCESS                Created successfully.
 *                Any other error codes     Failed to create.
 **********************************************************************************************************************/
UINT file_create(CHAR * file_name)
{
    return fx_file_create(&g_fx_media, file_name);
}
/***********************************************************************************************************************
* End of function file_create
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: created_entry_info_get
 * Description  : This function flushes media data and retrieves file or directory information.
 * Arguments    : file_name                 Pointer to the file/directory name.
 *                p_entry                   Pointer to file/directory entry information.
 *                p_sector                  Pointer to file/directory sector information.
 * Return Value : FX_SUCCESS                Retrieved successfully.
 *                Any other error codes     Failed to retrieve.
 **********************************************************************************************************************/
UINT created_entry_info_get (entry_info_t * p_entry, CHAR * file_name, sector_info_t * p_sector)
{
    UINT status = FX_SUCCESS;

    /* Flush data into the physical media */
    status = media_flush();
    if (FX_SUCCESS != status)
    {
        return status;
    }

    /* Retrieve a directory name */
    strcpy(p_entry->name, file_name);

    status = dir_info(p_entry);
    if (FX_SUCCESS != status)
    {
        return status;
    }

    /* Retrieve sector information */
    return sector_info_get(p_sector->logical_sector,&p_sector->sector_map_entry, &p_sector->sector_address);
}
/***********************************************************************************************************************
* End of function created_entry_info_get
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: file_open
 * Description  : This function opens a file for write access.
 * Arguments    : file_ptr                  Pointer to the file control block.
 *                file_name                 Pointer to the file name.
 *                open_type                 File open type.
 * Return Value : FX_SUCCESS                Opened successfully.
 *                Any other error codes     Failed to open.
 **********************************************************************************************************************/
UINT file_open(FX_FILE * file_ptr, CHAR * file_name, UINT open_type)
{
    return fx_file_open(&g_fx_media, file_ptr, file_name, open_type);
}
/***********************************************************************************************************************
* End of function file_open
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: file_close
 * Description  : This function closes the opened file.
 * Arguments    : file_ptr                  Pointer to the file control block.
 * Return Value : None.
 **********************************************************************************************************************/
void file_close(FX_FILE *file_ptr)
{
    fx_file_close(file_ptr);
}
/***********************************************************************************************************************
* End of function file_close
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: file_write
 * Description  : This function writes data and sets the date and time information.
 * Arguments    : file_ptr                  Pointer to the file control block.
 *                buffer_ptr                Pointer to the data buffer.
 * Return Value : FX_SUCCESS                Written and set successfully.
 *                Any other error codes     Failed to write or set.
 **********************************************************************************************************************/
UINT file_write(FX_FILE *file_ptr, VOID *buffer_ptr, CHAR *file_name, time_t * p_time)
{
    UINT status = FX_SUCCESS;

    /* Clean the file contents */
    status = fx_file_truncate(file_ptr, TRUNCATE_VALUE);
    if (FX_SUCCESS != status)
    {
        return status;
    }
    create_fixed_buffer(buffer_ptr);

    status = fx_file_write(file_ptr, buffer_ptr, WRITE_BUFFER_SIZE);

    if (FX_SUCCESS != status)
    {
        return status;
    }

    /* Get system time */
    status = fx_system_time_get(&p_time->hour,
                             &p_time->min,
                             &p_time->sec);
    if (FX_SUCCESS != status)
    {
        return status;
    }

    /* Get system date */
    status = fx_system_date_get(&p_time->year,
                             &p_time->month,
                             &p_time->date);
    if (FX_SUCCESS != status)
    {
        return status;
    }

    /* Set the date and time information for the opened file */
    status = fx_file_date_time_set(&g_fx_media, file_name,
                                   p_time->year,
                                   p_time->month,
                                   p_time->date,
                                   p_time->hour,
                                   p_time->min,
                                   p_time->sec);

    return status;
}
/***********************************************************************************************************************
* End of function file_write
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: file_seek
 * Description  : This function checks for errors in the file seek call.
 * Arguments    : file_ptr                  Pointer to the file control block.
 * Return Value : FX_SUCCESS                Checked successfully.
 *                Any other error codes     Failed to check.
 **********************************************************************************************************************/
UINT file_seek(FX_FILE *file_ptr)
{
    return fx_file_seek(file_ptr, SEEK_VALUE);
}
/***********************************************************************************************************************
* End of function file_seek
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: file_read
 * Description  : This function reads the file.
 * Arguments    : file_ptr                  Pointer to the file control block.
 *                buffer_ptr                Pointer to the read buffer.
 *                actual_size               Pointer to the number of bytes read.
 * Return Value : FX_SUCCESS                Read successfully.
 *                Any other error codes     Failed to read.
 **********************************************************************************************************************/
UINT file_read(FX_FILE *file_ptr, VOID *buffer_ptr, ULONG *actual_size)
{
    return fx_file_read(file_ptr, buffer_ptr, READ_BUFFER_SIZE, actual_size);
}
/***********************************************************************************************************************
* End of function file_read
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: file_delete
 * Description  : This function deletes a file.
 * Arguments    : file_name                 Pointer to the file name.
 * Return Value : FX_SUCCESS                Deleted successfully.
 *                Any other error codes     Failed to delete.
 **********************************************************************************************************************/
UINT file_delete(CHAR *file_name)
{
    return fx_file_delete(&g_fx_media, file_name);
}
/***********************************************************************************************************************
* End of function file_delete
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: g_rm_filex_levelx_nor_callback
 * Description  : This function handles the FileX LevelX NOR callback events.
 * Arguments    : p_args                    Callback function parameter data.
 * Return Value : None.
 **********************************************************************************************************************/
void g_rm_filex_levelx_nor_callback(rm_filex_levelx_nor_callback_args_t * p_args)
{
    if (RM_FILEX_LEVELX_NOR_EVENT_BUSY == p_args->event)
    {
        /* Put the thread to sleep while waiting for operation to complete */
        thread_sleep(APP_SLEEP_TICK);
    }
}
/***********************************************************************************************************************
* End of function g_rm_filex_levelx_nor_callback
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: file_system_init
 * Description  : This function initializes the FileX file system.
 * Arguments    : None.
 * Return Value : FX_SUCCESS                Initialized successfully.
 *                Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
UINT file_system_init(void)
{
    UINT status = FX_SUCCESS;
    time_t time =
    {
        .month = RESET_VALUE,
        .date  = RESET_VALUE,
        .year  = RESET_VALUE,
        .hour  = RESET_VALUE,
        .min   = RESET_VALUE,
        .sec   = RESET_VALUE
    };

    /* Extract current date from compiler macros */
    time.month = get_month((const CHAR *)(__DATE__) + MONTH_INDEX);
    time.date  = (UINT) atoi((const CHAR *)(__DATE__) + DATE_INDEX);
    time.year  = (UINT) atoi((const CHAR *)(__DATE__) + YEAR_INDEX);

    /* Extract current time from compiler macros */
    time.hour = (UINT) atoi((const CHAR *)(__TIME__) + HOUR_INDEX);
    time.min  = (UINT) atoi((const CHAR *)(__TIME__) + MIN_INDEX);
    time.sec  = (UINT) atoi((const CHAR *)(__TIME__) + SEC_INDEX);

    /* Initialize the FileX system */
    fx_system_initialize();

    /* Set date for FileX system */
    status = fx_system_date_set(time.year, time.month, time.date);
    if (FX_SUCCESS != status)
    {
        return status;
    }

    /* Set time for FileX system */
    status = fx_system_time_set(time.hour, time.min, time.sec);
    if (FX_SUCCESS != status)
    {
        return status;
    }

    return status;
}
/***********************************************************************************************************************
* End of function file_system_init
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: get_month
 * Description  : This function converts a string month to an integer month.
 * Arguments    : p_month                   Pointer to a string month.
 * Return Value : Integer month.
 **********************************************************************************************************************/
static UINT get_month(const CHAR * p_month)
{
    const CHAR * p_string[] =
    {
        "Nul", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };

    /* Check and convert month string to integer */
    for (UINT value = (UINT) JANUARY; value <= (UINT) DECEMBER; value++)
    {
        if (RESET_VALUE == strncmp(p_string[value], p_month, MONTH_STR_LEN))
        {
            return (UINT) value;
        }
    }

    /* Set the default month to JANUARY if month is not detected */
    return (UINT) JANUARY;
}
/***********************************************************************************************************************
* End of function get_month
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: create_fixed_buffer
 * Description  : This function creates a fixed data buffer.
 * Arguments    : buffer_ptr                Pointer to the data buffer.
 * Return Value : None.
 **********************************************************************************************************************/
static void create_fixed_buffer(VOID *buffer_ptr)
{
    CHAR * p_data = buffer_ptr;

    /* Clean write buffer */
    memset(p_data, NULL_CHAR, WRITE_BUFFER_SIZE);

    /* Create fixed buffer */
    for (uint16_t i = 0; i < (WRITE_BUFFER_SIZE / WRITE_LINE_SIZE); i++)
    {
        strncpy(p_data, WRITE_LINE_TEXT, WRITE_LINE_SIZE);
        p_data += WRITE_LINE_SIZE;
    }
}
/***********************************************************************************************************************
* End of function create_fixed_buffer
***********************************************************************************************************************/
