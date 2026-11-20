/***********************************************************************************************************************
 * File Name    : filex_sram_ep.c
 * Description  : Contains data structures and functions used in filex_sram_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "filex_sram_ep.h"
#include "filex_thread.h"

/* Private variables */
FX_MEDIA s_fx_media;
uint8_t s_fx_media_media_memory[G_FX_MEDIA_MEDIA_MEMORY_SIZE];

/* Private functions */
static UINT media_open(void);
static void create_fixed_buffer(VOID *buffer_ptr);

/*******************************************************************************************************************//**
 * @brief     This function initializes the FileX file system.
 * @param[in] None.
 * @retval    FX_SUCCESS                Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
UINT file_system_init(void)
{
    UINT status = FX_SUCCESS;
    time_app_t time = {.date = RESET_VALUE, .hour = RESET_VALUE, .min = RESET_VALUE,
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
    status = (UINT)RM_FILEX_BLOCK_MEDIA_Open(&g_rm_filex_block_media_ctrl, &g_rm_filex_block_media_cfg);
    if (FX_SUCCESS != status)
    {
        return status;
    }

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

    /* Open media using the Azure FileX API */
    return media_open();
}

/*******************************************************************************************************************//**
 * @brief     This function opens the media using the Azure FileX API.
 * @param[in] None.
 * @retval    FX_SUCCESS                Opened successfully.
 * @retval    Any other error codes     Failed to open.
 **********************************************************************************************************************/
static UINT media_open(void)
{
    return fx_media_open(&s_fx_media, "s_fx_media", RM_FILEX_BLOCK_MEDIA_BlockDriver,
                         (void *)&g_rm_filex_block_media_instance, s_fx_media_media_memory,
                         G_FX_MEDIA_MEDIA_MEMORY_SIZE);
}

/*******************************************************************************************************************//**
 * @brief     This function formats the media using the Azure FileX API.
 * @param[in] None.
 * @retval    FX_SUCCESS                Formatted successfully.
 * @retval    Any other error codes     Failed to format.
 **********************************************************************************************************************/
UINT media_format(void)
{
    UINT status = FX_SUCCESS;

    /* Close media before format */
    status = fx_media_close(&s_fx_media);
    if ((FX_SUCCESS != status) && (FX_MEDIA_NOT_OPEN != status))
    {
        return status;
    }

    /* Format the media */
    status = fx_media_format(&s_fx_media,
                             RM_FILEX_BLOCK_MEDIA_BlockDriver,
                             (void *)&g_rm_filex_block_media_instance,
                             s_fx_media_media_memory,
                             G_FX_MEDIA_MEDIA_MEMORY_SIZE,
                             (char *)G_FX_MEDIA_VOLUME_NAME,
                             G_FX_MEDIA_NUMBER_OF_FATS,
                             G_FX_MEDIA_DIRECTORY_ENTRIES,
                             G_FX_MEDIA_HIDDEN_SECTORS,
                             G_FX_MEDIA_TOTAL_SECTORS,
                             G_FX_MEDIA_BYTES_PER_SECTOR,
                             G_FX_MEDIA_SECTORS_PER_CLUSTER,
                             G_FX_MEDIA_HEADS,
                             G_FX_MEDIA_SECTORS_PER_TRACK);
    if (FX_SUCCESS != status)
    {
        return status;
    }

    /* Open media using the Azure FileX API */
    return media_open();
}

/*******************************************************************************************************************//**
 * @brief     This function creates a new directory.
 * @param[in] directory_name            Pointer to the directory name.
 * @retval    FX_SUCCESS                Created successfully.
 * @retval    Any other error codes     Failed to create.
 **********************************************************************************************************************/
UINT dir_create(CHAR * directory_name)
{
    return fx_directory_create(&s_fx_media, directory_name);
}

/*******************************************************************************************************************//**
 * @brief     This function flushes cached media data to the physical storage media.
 * @param[in] None.
 * @retval    FX_SUCCESS                Flushed successfully.
 * @retval    Any other error codes     Failed to flush.
 **********************************************************************************************************************/
UINT media_flush(void)
{
    return fx_media_flush(&s_fx_media);
}

/*******************************************************************************************************************//**
 * @brief     This function retrieves full information for a file or directory.
 * @param[in] p_entry                   Pointer to the entry information structure.
 * @param[in] file_name                 Pointer to the file or directory name.
 * @retval    FX_SUCCESS                Retrieved successfully.
 * @retval    Any other error codes     Failed to retrieve.
 **********************************************************************************************************************/
UINT dir_info(entry_info_t * p_entry, CHAR * file_name)
{
    /* Retrieve a file/directory name */
    strcpy(p_entry->name, file_name);

    /* Retrieve a directory full information */
    return fx_directory_information_get(&s_fx_media, p_entry->name, &p_entry->attr, &p_entry->size,
                                        &p_entry->time.year, &p_entry->time.month, &p_entry->time.date,
                                        &p_entry->time.hour, &p_entry->time.min, &p_entry->time.sec);
}

/*******************************************************************************************************************//**
 * @brief     This function flushes media data and retrieves information for the created file or directory.
 * @param[in] p_entry                   Pointer to the entry information structure.
 * @param[in] file_name                 Pointer to the file or directory name.
 * @retval    FX_SUCCESS                Retrieved successfully.
 * @retval    Any other error codes     Failed to retrieve.
 **********************************************************************************************************************/
UINT created_entry_info_get(entry_info_t * p_entry, CHAR * file_name)
{
    UINT status = FX_SUCCESS;
    status = fx_media_flush(&s_fx_media);
    if (FX_SUCCESS != status)
    {
        return status;
    }
    return dir_info(p_entry, file_name);
}

/*******************************************************************************************************************//**
 * @brief     This function retrieves information for the first directory entry.
 * @param[in] p_entry                   Pointer to the directory entry information.
 * @retval    FX_SUCCESS                Retrieved successfully.
 * @retval    Any other error codes     Failed to retrieve.
 **********************************************************************************************************************/
UINT dir_first_full_entry_find(entry_info_t * p_entry)
{
    return fx_directory_first_full_entry_find(&s_fx_media, p_entry->name, &p_entry->attr, &p_entry->size,
                                               &p_entry->time.year, &p_entry->time.month, &p_entry->time.date,
                                               &p_entry->time.hour, &p_entry->time.min, &p_entry->time.sec);
}

/*******************************************************************************************************************//**
 * @brief     This function retrieves information for the next directory entry.
 * @param[in] p_entry                   Pointer to the directory entry information.
 * @retval    FX_SUCCESS                Retrieved successfully.
 * @retval    Any other error codes     Failed to retrieve.
 **********************************************************************************************************************/
UINT dir_next_full_entry_find(entry_info_t * p_entry)
{
    return fx_directory_next_full_entry_find(&s_fx_media, p_entry->name, &p_entry->attr, &p_entry->size,
                                              &p_entry->time.year, &p_entry->time.month, &p_entry->time.date,
                                              &p_entry->time.hour, &p_entry->time.min, &p_entry->time.sec);
}

/*******************************************************************************************************************//**
 * @brief     This function deletes a directory.
 * @param[in] directory_name            Pointer to the directory name.
 * @retval    FX_SUCCESS                Deleted successfully.
 * @retval    Any other error codes     Failed to delete.
 **********************************************************************************************************************/
UINT dir_delete(CHAR * directory_name)
{
    return fx_directory_delete(&s_fx_media, directory_name);
}

/*******************************************************************************************************************//**
 * @brief     This function creates a new file.
 * @param[in] file_name                 Pointer to the file name.
 * @retval    FX_SUCCESS                Created successfully.
 * @retval    Any other error codes     Failed to create.
 **********************************************************************************************************************/
UINT file_create(CHAR * file_name)
{
    return fx_file_create(&s_fx_media, file_name);
}

/*******************************************************************************************************************//**
 * @brief     This function opens a file with the specified access type.
 * @param[in] file_ptr                  Pointer to the file control block.
 * @param[in] file_name                 Pointer to the file name.
 * @param[in] open_type                 File open type.
 * @retval    FX_SUCCESS                Opened successfully.
 * @retval    Any other error codes     Failed to open.
 **********************************************************************************************************************/
UINT file_open(FX_FILE * file_ptr, CHAR * file_name, UINT open_type)
{
    return fx_file_open(&s_fx_media, file_ptr, file_name, open_type);
}

/*******************************************************************************************************************//**
 * @brief     This function writes fixed data to a file and updates its date and time information.
 * @param[in] file_ptr                  Pointer to the file control block.
 * @param[in] buffer_ptr                Pointer to the write buffer.
 * @param[in] file_name                 Pointer to the file name.
 * @param[in] p_time                    Pointer to the date and time information.
 * @retval    FX_SUCCESS                Written successfully.
 * @retval    Any other error codes     Failed to write.
 **********************************************************************************************************************/
UINT file_write(FX_FILE * file_ptr, VOID * buffer_ptr, CHAR * file_name, time_app_t * p_time)
{
    UINT status = FX_SUCCESS;

    /* Clean the file contents */
    status = fx_file_truncate(file_ptr, TRUNCATE_VALUE);
    if (FX_SUCCESS != status)
    {
        return status;
    }

    /* Create fixed buffer */
    create_fixed_buffer(buffer_ptr);

    /* Write fixed buffer to a file */
    status = fx_file_write(file_ptr, buffer_ptr, WRITE_BUFFER_SIZE);
    if (FX_SUCCESS != status)
    {
        return status;
    }

    /* Wait until the complete event flag is received */
    status = wait_event_get();
    if (TX_SUCCESS != status)
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
    return fx_file_date_time_set(&s_fx_media, file_name,
                                 p_time->year,
                                 p_time->month,
                                 p_time->date,
                                 p_time->hour,
                                 p_time->min,
                                 p_time->sec);
}

/*******************************************************************************************************************//**
 * @brief     This function waits for the FileX Block Media operation completion event.
 * @param[in] None.
 * @retval    TX_SUCCESS                Event received successfully.
 * @retval    Any other error codes     Failed to receive the event.
 **********************************************************************************************************************/
UINT wait_event_get(void)
{
    ULONG actual_event = RESET_VALUE;
    return tx_event_flags_get(&g_media_event_flag, RM_BLOCK_MEDIA_EVENT_WAIT_END,
                              TX_OR_CLEAR, &actual_event, OPERATION_TIME_OUT);
}

/*******************************************************************************************************************//**
 * @brief     This function closes the opened file.
 * @param[in] file_ptr                  Pointer to the file control block.
 * @retval    None.
 **********************************************************************************************************************/
void file_close(FX_FILE * file_ptr)
{
    fx_file_close(file_ptr);
}

/*******************************************************************************************************************//**
 * @brief     This function moves the file read/write pointer to the specified position.
 * @param[in] file_ptr                  Pointer to the file control block.
 * @retval    FX_SUCCESS                Sought successfully.
 * @retval    Any other error codes     Failed to seek.
 **********************************************************************************************************************/
UINT file_seek(FX_FILE * file_ptr)
{
    return fx_file_seek(file_ptr, SEEK_VALUE);
}

/*******************************************************************************************************************//**
 * @brief     This function reads data from a file.
 * @param[in] file_ptr                  Pointer to the file control block.
 * @param[in] buffer_ptr                Pointer to the read buffer.
 * @param[in] actual_size               Pointer to the number of bytes read.
 * @retval    FX_SUCCESS                Read successfully.
 * @retval    Any other error codes     Failed to read.
 **********************************************************************************************************************/
UINT file_read(FX_FILE * file_ptr, VOID * buffer_ptr, ULONG * actual_size)
{
    return fx_file_read(file_ptr, buffer_ptr, READ_BUFFER_SIZE, actual_size);
}

/*******************************************************************************************************************//**
 * @brief     This function deletes a file.
 * @param[in] file_name                 Pointer to the file name.
 * @retval    FX_SUCCESS                Deleted successfully.
 * @retval    Any other error codes     Failed to delete.
 **********************************************************************************************************************/
UINT file_delete(CHAR * file_name)
{
    return fx_file_delete(&s_fx_media, file_name);
}

/*******************************************************************************************************************//**
 * @brief     This function creates a fixed data buffer for file write operations.
 * @param[in] buffer_ptr                Pointer to the data buffer.
 * @retval    None.
 **********************************************************************************************************************/
static void create_fixed_buffer(VOID * buffer_ptr)
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

/*******************************************************************************************************************//**
 * @brief     This function de-initializes the opened FileX Block Media module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void filex_deinit(void)
{
    if (g_rm_filex_block_media_ctrl.open)
    {
        RM_FILEX_BLOCK_MEDIA_Close(&g_rm_filex_block_media_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function converts a string month to an integer month.
 * @param[in] p_month                   Pointer to a string month.
 * @retval    value                     Integer value representing the month.
 **********************************************************************************************************************/
UINT get_month(const CHAR * p_month)
{
    static const CHAR * sp_string[] = {"Nul", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct",
                                      "Nov", "Dec"};

    /* Check and convert month string to integer */
    for (UINT value = (UINT)JANUARY; value <= (UINT)DECEMBER; value++)
    {
        if (RESET_VALUE == strncmp(sp_string[value], p_month, MONTH_STR_LEN))
        {
            return (UINT)value;
        }
    }

    /* Set the default month to JANUARY if month is not detected */
    return (UINT)JANUARY;
}

/*******************************************************************************************************************//**
 * @brief     This function is a callback for FileX Block Media module.
 * @param[in] p_args                    Pointer to the callback argument.
 * @retval    None.
 **********************************************************************************************************************/
void rm_filex_block_media_callback(rm_filex_block_media_callback_args_t * p_args)
{
    switch (p_args->event)
    {
        case RM_BLOCK_MEDIA_EVENT_MEDIA_INSERTED:
        {
            /* Do nothing */
            break;
        }

        case RM_BLOCK_MEDIA_EVENT_MEDIA_REMOVED:
        {
            /* Do nothing */
            break;
        }

        case RM_BLOCK_MEDIA_EVENT_WAIT_END:
        {
            /* FileX driver returns RM_BLOCK_MEDIA_EVENT_WAIT_END event when an operation is completed.
             * It can be read, write or erase operation. If there is an occurrence of error event,
             * the FileX callback will still return the same event and the respective API
             * will return the specific error code. */
            tx_event_flags_set(&g_media_event_flag, RM_BLOCK_MEDIA_EVENT_WAIT_END, TX_OR);
            break;
        }

        case RM_BLOCK_MEDIA_EVENT_POLL_STATUS:
        case RM_BLOCK_MEDIA_EVENT_MEDIA_SUSPEND:
        case RM_BLOCK_MEDIA_EVENT_MEDIA_RESUME:
        case RM_BLOCK_MEDIA_EVENT_WAIT:
        default:
        {
            /* Do nothing */
            break;
        }
    }
}
