/***********************************************************************************************************************
 * File Name    : filex_thread_entry.c
 * Description  : Contains macros, data structures and functions commonly used in filex_thread_entry.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "filex_thread.h"
#include "filex_media_operation.h"
#include "filex_dir_operation.h"
#include "filex_file_operation.h"

static UINT conv_month_to_int(const CHAR * p_month);
static UINT file_system_init(void);
static UINT file_system_operation(ULONG request);

/* Public global variables */
FX_MEDIA g_fx_media;
uint8_t g_fx_media_media_memory[G_FX_MEDIA_MEDIA_MEMORY_SIZE];
volatile media_status_t g_fx_media_status = (media_status_t)RESET_VALUE;

/* FileX Thread entry function */
void filex_thread_entry(void)
{
    UINT status = RESET_VALUE;
    ULONG actual_event = RESET_VALUE;

    /* Suspend thread until received start file system event flag */
    actual_event = RESET_VALUE;
    status = tx_event_flags_get(&g_request_event, REQUEST_START_FILE_SYSTEM,\
                                 TX_OR_CLEAR, &actual_event, TX_WAIT_FOREVER);
    if(TX_SUCCESS != status)
    {
        PRINT_ERR_STR("tx_event_flags_get for REQUEST_START_FILE_SYSTEM failed\r\n");
        ERROR_TRAP(status);
    }

    /* Initialize the FileX file system */
    status = file_system_init();
    if(FX_SUCCESS != status)
    {
        PRINT_ERR_STR("file_system_init failed\r\n");
        ERROR_TRAP(status);
    }

    while(true)
    {
        /* Wait for a request event from the console thread */
        actual_event = RESET_VALUE;
        status = tx_event_flags_get(&g_request_event, FILE_SYSTEM_REQUEST_MASK,\
                                     TX_OR_CLEAR, &actual_event, TX_WAIT_FOREVER);
        if(TX_SUCCESS != status)
        {
            PRINT_ERR_STR("tx_event_flags_get for user request events failed\r\n");
            ERROR_TRAP(status);
        }

        /* Perform file system operations as required by the user */
        status = file_system_operation(actual_event);
        if(FX_SUCCESS != status)
        {
            PRINT_ERR_STR("file_system_operation failed\r\n");
            ERROR_TRAP(status);
        }

        /* Send a request complete event flag to the console thread */
        status = tx_event_flags_set(&g_request_event, REQUEST_COMPLETED, TX_OR);
        if(TX_SUCCESS != status)
        {
            PRINT_ERR_STR("tx_event_flags_set for REQUEST_COMPLETED event failed\r\n");
            ERROR_TRAP(status);
        }

        tx_thread_sleep(FILEX_THREAD_SLEEP_TICK);
    }
}

/*******************************************************************************************************************//**
 * @brief       Block media callback function.
 * @param[in]   p_args
 * @retval      None
 **********************************************************************************************************************/
void g_rm_filex_block_media_callback(rm_filex_block_media_callback_args_t *p_args)
{
    switch(p_args->event)
    {
        case RM_BLOCK_MEDIA_EVENT_MEDIA_INSERTED:
            g_fx_media_status |= MEDIA_INSERTED;
            break;

        case RM_BLOCK_MEDIA_EVENT_MEDIA_REMOVED:
            g_fx_media_status &= ~(MEDIA_INSERTED);
            break;

        case RM_BLOCK_MEDIA_EVENT_WAIT_END:
            /* FileX driver returns RM_BLOCK_MEDIA_EVENT_WAIT_END event when an operation is completed.
             * It can be read, write or erase operation. If there is an occurrence of error event,
             * the FileX callback will still return the same event and the respective API
             * will return the specific error code. */
            tx_event_flags_set(&g_media_event, RM_BLOCK_MEDIA_EVENT_WAIT_END, TX_OR);
            break;

        case RM_BLOCK_MEDIA_EVENT_POLL_STATUS:
        case RM_BLOCK_MEDIA_EVENT_MEDIA_SUSPEND:
        case RM_BLOCK_MEDIA_EVENT_MEDIA_RESUME:
        case RM_BLOCK_MEDIA_EVENT_WAIT:
        default:
            break;
    }
}

/*******************************************************************************************************************//**
 * @brief       This function converts a string month to an integer month.
 * @param[in]   p_month         Pointer to a string month.
 * @retval      integer month
 **********************************************************************************************************************/
static UINT conv_month_to_int(const CHAR * p_month)
{
    const CHAR * p_string[] = {"Nul","Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};

    /* Checks and convert month string to integer */
    for(UINT value = (UINT)JANUARY; value <= (UINT)DECEMBER ; value ++)
    {
        if(RESET_VALUE == strncmp(p_string[value], p_month, MONTH_STR_LEN))
        {
            return (UINT)value;
        }
    }
    /* Set the default month to JANUARY if month is not detected */
    return (UINT)JANUARY;
}

static UINT file_system_init(void)
{
    UINT fx_status = FX_SUCCESS;
    fsp_err_t fsp_err = FSP_SUCCESS;
    app_time_t system_time =
    {
     .month = RESET_VALUE,
     .date  = RESET_VALUE,
     .year  = RESET_VALUE,
     .hour  = RESET_VALUE,
     .min   = RESET_VALUE,
     .sec   = RESET_VALUE
    };

    /* Extract current Date from compiler MACROS */
    system_time.month = conv_month_to_int((const char *)(__DATE__) + MONTH_INDEX);
    system_time.date = atoi((const char *)(__DATE__) + DATE_INDEX);
    system_time.year = atoi((const char *)(__DATE__) + YEAR_INDEX);

    /* Extract current Time from compiler MACROS */
    system_time.hour = atoi((const char *)(__TIME__) + HOUR_INDEX);
    system_time.min  = atoi((const char *)(__TIME__) + MIN_INDEX);
    system_time.sec  = atoi((const char *)(__TIME__) + SEC_INDEX);

    /* Open and configure FileX Block Media interface */
    fsp_err = RM_FILEX_BLOCK_MEDIA_Open(&g_rm_filex_block_media_ctrl, &g_rm_filex_block_media_cfg);
    RETURN_ERR_STR((UINT)fsp_err, "RM_FILEX_BLOCK_MEDIA_Open failed\r\n");

    /* Initialize the FileX system */
    fx_system_initialize();

    /* Set date for FileX system */
    fx_status = fx_system_date_set(system_time.year, system_time.month, system_time.date);
    RETURN_ERR_STR(fx_status, "fx_system_date_set failed\r\n");

    /* Set time for FileX system */
    fx_status = fx_system_time_set(system_time.hour, system_time.min, system_time.sec);
    RETURN_ERR_STR(fx_status, "fx_system_time_set failed\r\n");

    return fx_status;
}

static UINT file_system_operation(ULONG request)
{
    UINT fx_status = FX_SUCCESS;

    switch(request)
    {
        case REQUEST_MEDIA_OPEN:
            PRINT_INFO_STR("Open media ...\r\n\r\n");
            fx_status = media_open();
            break;

        case REQUEST_MEDIA_PROPERTY:
            PRINT_INFO_STR("Get media properties ...\r\n\r\n");
            fx_status = media_get_property();
            break;

        case REQUEST_MEDIA_FORMAT:
            PRINT_INFO_STR("Format media in exFAT-format ...\r\n\r\n");
            fx_status = media_format();
            break;

        case REQUEST_MEDIA_CLOSE:
            PRINT_INFO_STR("Close media ...\r\n\r\n");
            fx_status = media_close();
            break;

        case REQUEST_DIR_CREATE:
            PRINT_INFO_STR("Create a new directory ...\r\n\r\n");
            fx_status = dir_create();
            break;

        case REQUEST_DIR_PROPERTY:
            PRINT_INFO_STR("Get directory properties ...\r\n\r\n");
            fx_status = dir_get_property();
            break;

        case REQUEST_DIR_DELETE:
            PRINT_INFO_STR("Delete a directory ...\r\n\r\n");
            fx_status = dir_delete();
            break;

        case REQUEST_FILE_CREATE:
            PRINT_INFO_STR("Create a new file ...\r\n\r\n");
            fx_status = file_create();
            break;

        case REQUEST_FILE_WRITE:
            PRINT_INFO_STR("Write to a file ...\r\n\r\n");
            fx_status = file_write();
            break;

        case REQUEST_FILE_READ:
            PRINT_INFO_STR("Read from a file ...\r\n\r\n");
            fx_status = file_read();
            break;

        case REQUEST_FILE_DELETE:
            PRINT_INFO_STR("Delete a file ...\r\n\r\n");
            fx_status = file_delete();
            break;

        default:
            PRINT_INFO_STR("This request is not supported\r\n\r\n");
            break;
    }

    return fx_status;
}
