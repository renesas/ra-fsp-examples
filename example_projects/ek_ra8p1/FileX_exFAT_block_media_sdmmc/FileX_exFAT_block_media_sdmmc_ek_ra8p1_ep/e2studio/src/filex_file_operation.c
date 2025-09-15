/***********************************************************************************************************************
 * File Name    : filex_file_operation.c
 * Description  : Contains data structures and functions used in FileX thread.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "filex_file_operation.h"
#include "filex_dir_operation.h"
#include "filex_media_operation.h"

/* Private global variables */
static CHAR g_file_name1[] = FILE_NAME_ONE;
static CHAR g_write_data[WRITE_BUFFER_SIZE] = {RESET_VALUE};

/* Private function declaration */
static void create_fixed_buffer(void);

/* Functions implementation */

/*******************************************************************************************************************//**
 * @brief       This function creates a fixed data buffer.
 * @param[in]   None
 * @retval      None
 **********************************************************************************************************************/
static void create_fixed_buffer(void)
{
    CHAR * p_data = g_write_data;

    /* Clean write buffer */
    memset(p_data, NULL_CHAR, WRITE_BUFFER_SIZE);

    /* Create fixed buffer */
    for(uint16_t i = 0; i < WRITE_BUFFER_SIZE / WRITE_LINE_SIZE ; i ++)
    {
        strncpy(p_data, WRITE_LINE_TEXT, WRITE_LINE_SIZE);
        p_data += WRITE_LINE_SIZE;
    }
}

/*******************************************************************************************************************//**
 * @brief       This function creates a file.
 * @param[in]   None
 * @retval      FX_SUCCESS   Upon successful operation.
 * @retval      Any other error code apart from FX_SUCCESS.
 **********************************************************************************************************************/
UINT file_create(void)
{
    UINT status = FX_SUCCESS;
    entry_info_t entry =
    {
     .name = {NULL_CHAR},
     .attr = RESET_VALUE,
     .size = RESET_VALUE,
     .time = {
              .month = RESET_VALUE,
              .date  = RESET_VALUE,
              .year  = RESET_VALUE,
              .hour  = RESET_VALUE,
              .min   = RESET_VALUE,
              .sec   = RESET_VALUE,
             }
    };

    /* Verify the current state of the media */
    status = media_verify();
    if(FX_SUCCESS != status)
    {
        return FX_SUCCESS;
    }

    /* Create a new file using the Azure FileX API */
    status = fx_file_create(&g_fx_media, g_file_name1);

    if(FX_ALREADY_CREATED == status)
    {
        PRINT_INFO_STR("File already exists\r\n");
        return FX_SUCCESS;
    }

    if(FX_SUCCESS != status)
    {
        RETURN_ERR_STR(status, "fx_file_create failed\r\n");
    }

    /* Flush data into the physical media */
    status = fx_media_flush(&g_fx_media);
    RETURN_ERR_STR(status, "fx_media_flush failed\r\n");

    /* Get file name */
    memcpy(entry.name, g_file_name1, strlen(g_file_name1) + ONE_BYTE);

    /* Get file full information */
    status = fx_directory_information_get(&g_fx_media,\
                                          entry.name, &entry.attr, &entry.size,\
                                          &entry.time.year, &entry.time.month, &entry.time.date,\
                                          &entry.time.hour, &entry.time.min, &entry.time.sec);
    RETURN_ERR_STR(status, "fx_directory_information_get failed\r\n");

    /* Display file information */
    PRINT_ENTRY_INFO(entry);

    /* Create a new file successfully */
    PRINT_INFO_STR("\r\nFile created successfully\r\n");

    return FX_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief       This function writes fixed data to a file.
 * @param[in]   None
 * @retval      FX_SUCCESS   Upon successful operation.
 * @retval      Any other error code apart from FX_SUCCESS.
 **********************************************************************************************************************/
UINT file_write(void)
{
    UINT status = RESET_VALUE;
    UINT status_temp = FX_SUCCESS;
    ULONG actual_event = RESET_VALUE;

    FX_FILE file;
    memset(&file, RESET_VALUE, sizeof(file));

    app_time_t time =
    {
     .month = RESET_VALUE,
     .date  = RESET_VALUE,
     .year  = RESET_VALUE,
     .hour  = RESET_VALUE,
     .min   = RESET_VALUE,
     .sec   = RESET_VALUE,
    };

    entry_info_t entry =
    {
     .name = {NULL_CHAR},
     .attr = RESET_VALUE,
     .size = RESET_VALUE,
     .time = {
              .month = RESET_VALUE,
              .date  = RESET_VALUE,
              .year  = RESET_VALUE,
              .hour  = RESET_VALUE,
              .min   = RESET_VALUE,
              .sec   = RESET_VALUE,
             }
    };

    /* Verify the current state of the media */
    status = media_verify();
    if(FX_SUCCESS != status)
    {
        return FX_SUCCESS;
    }

    /* Open the file for writing by using the Azure FileX API */
    status = fx_file_open(&g_fx_media, &file, g_file_name1, FX_OPEN_FOR_WRITE);

    if(FX_NOT_FOUND == status)
    {
        PRINT_INFO_STR("File does not exist\r\n");
        return FX_SUCCESS;
    }

    if(FX_SUCCESS != status)
    {
        RETURN_ERR_STR(status, "fx_file_open failed\r\n");
    }

    /* Clean the file contents */
    status = fx_file_extended_truncate(&file, TRUNCATE_VALUE);
    if(FX_SUCCESS != status)
    {
        /* Close the file using the Azure FileX API */
        status_temp = fx_file_close(&file);
        RETURN_ERR_STR(status_temp, "fx_file_close failed\r\n");

        /* Return fx_file_extended_truncate failed status */
        RETURN_ERR_STR(status, "fx_file_extended_truncate failed\r\n");
    }

    /* Create fixed buffer */
    create_fixed_buffer();

    /* Write 4GB content to the opened file */
    for(ULONG i = 0; i < WRITE_TIMES ; i++ )
    {
        /* Write fixed buffer to a file */
        status = fx_file_write(&file, (VOID *)g_write_data, WRITE_BUFFER_SIZE);
        if(FX_SUCCESS != status)
        {
            /* Close the file using the Azure FileX API */
            status_temp = fx_file_close(&file);
            RETURN_ERR_STR(status_temp, "fx_file_close failed\r\n");

            /* Return fx_file_write failed status */
            RETURN_ERR_STR(status, "fx_file_write failed\r\n");
        }

        /* Wait until the complete event flag is received */
        status = tx_event_flags_get(&g_media_event,\
                                    RM_BLOCK_MEDIA_EVENT_WAIT_END,\
                                    TX_OR_CLEAR, &actual_event, OPERATION_TIME_OUT);
        if(TX_SUCCESS != status)
        {
            /* Close the file using the Azure FileX API */
            status_temp = fx_file_close(&file);
            RETURN_ERR_STR(status_temp, "fx_file_close failed\r\n");

            /* Return tx_event_flags_get failed status */
            RETURN_ERR_STR(status, "tx_event_flags_get for RM_BLOCK_MEDIA_EVENT_WAIT_END event failed\r\n");
        }

        if(RESET_VALUE == i % WRITE_ONE_PERCENT)
        {
            PRINT_INFO_STR(".");
        }
    }

    PRINT_INFO_STR("\r\n\r\n");

    /* Get system time */
    status = fx_system_time_get(&time.hour, & time.min, &time.sec);
    if(FX_SUCCESS != status)
    {
        /* Close the file using the Azure FileX API */
        status_temp = fx_file_close(&file);
        RETURN_ERR_STR(status_temp, "fx_file_close failed\r\n");

        /* Return fx_system_time_get failed status */
        RETURN_ERR_STR(status, "fx_system_time_get failed\r\n");
    }

    /* Get system date */
    status = fx_system_date_get(&time.year, & time.month, &time.date);
    if(FX_SUCCESS != status)
    {
        /* Close the file using the Azure FileX API */
        status_temp = fx_file_close(&file);
        RETURN_ERR_STR(status_temp, "fx_file_close failed\r\n");

        /* Return fx_system_date_get failed status */
        RETURN_ERR_STR(status, "fx_system_date_get failed\r\n");
    }

    /* Set the date and time information for the opened file */
    status = fx_file_date_time_set(&g_fx_media, g_file_name1,\
                                   time.year, time.month, time.date,\
                                   time.hour, time.min, time.sec);
    if(FX_SUCCESS != status)
    {
        /* Close the file using the Azure FileX API */
        status_temp = fx_file_close(&file);
        RETURN_ERR_STR(status_temp, "fx_file_close failed\r\n");

        /* Return fx_file_date_time_set failed status */
        RETURN_ERR_STR(status, "fx_file_date_time_set failed\r\n");
    }

    /* Close the file using the Azure FileX API */
    status = fx_file_close(&file);
    RETURN_ERR_STR(status, "fx_file_close failed\r\n");

    /* Flush data into the physical media */
    status = fx_media_flush(&g_fx_media);
    RETURN_ERR_STR(status, "fx_media_flush failed\r\n");

    /* Get file name */
    memcpy(entry.name, g_file_name1, strlen(g_file_name1) + ONE_BYTE);

    /* Get file full information */
    status = fx_directory_information_get(&g_fx_media,\
                                          entry.name, &entry.attr, &entry.size,\
                                          &entry.time.year, &entry.time.month, &entry.time.date,\
                                          &entry.time.hour, &entry.time.min, &entry.time.sec);
    RETURN_ERR_STR(status, "fx_directory_information_get failed\r\n");

    /* Display file information */
    PRINT_ENTRY_INFO(entry);

    /* Write to the file successfully */
    PRINT_INFO_STR("\r\nWrite to a file successfully\r\n");

    return FX_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief       This function reads and verifies data from a file.
 * @param[in]   None
 * @retval      FX_SUCCESS   Upon successful operation.
 * @retval      Any other error code apart from FX_SUCCESS.
 **********************************************************************************************************************/
UINT file_read(void)
{
    UINT status = FX_SUCCESS;
    UINT status_temp = FX_SUCCESS;
    ULONG actual_event = RESET_VALUE;

    FX_FILE file;
    memset(&file, RESET_VALUE, sizeof(file));

    ULONG len = RESET_VALUE;

    entry_info_t entry =
    {
     .name = {NULL_CHAR},
     .attr = RESET_VALUE,
     .size = RESET_VALUE,
     .time = {
              .month = RESET_VALUE,
              .date  = RESET_VALUE,
              .year  = RESET_VALUE,
              .hour  = RESET_VALUE,
              .min   = RESET_VALUE,
              .sec   = RESET_VALUE,
             }
    };
    CHAR g_read_data[READ_BUFFER_SIZE + ONE_BYTE] = {RESET_VALUE};

    /* Verify the current state of the media */
    status = media_verify();
    if(FX_SUCCESS != status)
    {
        return FX_SUCCESS;
    }

    /* Open the file for reading by using the Azure FileX API */
    status = fx_file_open(&g_fx_media, &file, g_file_name1, FX_OPEN_FOR_READ);

    if(FX_NOT_FOUND == status)
    {
        PRINT_INFO_STR("File does not exist\r\n");
        return FX_SUCCESS;
    }

    if(FX_SUCCESS != status)
    {
        RETURN_ERR_STR(status, "fx_file_open failed\r\n");
    }

    /* Seek to the beginning of the file */
    status = fx_file_extended_seek(&file, SEEK_VALUE);
    if(FX_SUCCESS != status)
    {
       /* Close the file using the Azure FileX API */
       status_temp = fx_file_close(&file);
       RETURN_ERR_STR(status_temp, "fx_file_close failed\r\n");

       /* Return fx_file_seek failed status */
       RETURN_ERR_STR(status, "fx_file_extended_seek failed\r\n");
    }

    /* Read data from the file */
    status = fx_file_read(&file, g_read_data, READ_BUFFER_SIZE, &len);

    /* In case reading the file failed */
    if(FX_END_OF_FILE != status && FX_SUCCESS != status)
    {
        /* Close the file using the Azure FileX API */
        status_temp = fx_file_close(&file);
        RETURN_ERR_STR(status_temp, "fx_file_close failed\r\n");

        /* Return fx_file_read failed status */
        RETURN_ERR_STR(status, "fx_file_read failed\r\n");
    }

    /* Wait until the complete event flag is received */
    status = tx_event_flags_get(&g_media_event,
                                RM_BLOCK_MEDIA_EVENT_WAIT_END,
                                TX_OR_CLEAR, &actual_event, OPERATION_TIME_OUT);
    if(TX_SUCCESS != status)
    {
        /* Close the file using the Azure FileX API */
        status_temp = fx_file_close(&file);
        RETURN_ERR_STR(status_temp, "fx_file_close failed\r\n");

        /* Return tx_event_flags_get failed status */
        RETURN_ERR_STR(status, "tx_event_flags_get media completed flag failed\r\n");
    }

    /* Close the file using the Azure FileX API */
    status = fx_file_close(&file);
    RETURN_ERR_STR(status, "fx_file_close failed\r\n");

    /* Get file name */
    memcpy(entry.name, g_file_name1, strlen(g_file_name1) + ONE_BYTE);

    /* Get file full information */
    status = fx_directory_information_get(&g_fx_media,\
                                          entry.name, &entry.attr, &entry.size,\
                                          &entry.time.year, &entry.time.month, &entry.time.date,\
                                          &entry.time.hour, &entry.time.min, &entry.time.sec);
    RETURN_ERR_STR(status, "fx_directory_information_get failed\r\n");

    /* Display file information */
    PRINT_ENTRY_INFO(entry);

    if(READ_BUFFER_SIZE > len)
    {
        /* Display content of the file */
        PRINT_INFO_STR("\r\nContent of the file\r\n\r\n");
        send_data_to_rtt(RTT_OUTPUT_APP_INFO_STR, len + ONE_BYTE, g_read_data);
    }
    else
    {
        /* Display content of the first 1 KB of the file */
        PRINT_INFO_STR("\r\nContent of the first 1 kB of the file\r\n\r\n");
        send_data_to_rtt(RTT_OUTPUT_APP_INFO_STR, READ_BUFFER_SIZE + ONE_BYTE, g_read_data);
    }

    PRINT_INFO_STR("\r\nEnd\r\n");

    return FX_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief       This function deletes a file.
 * @param[in]   None
 * @retval      FX_SUCCESS   Upon successful operation.
 * @retval      Any other error code apart from FX_SUCCESS.
 **********************************************************************************************************************/
UINT file_delete(void)
{
    UINT status = FX_SUCCESS;

    /* Verify the current state of the media */
    status = media_verify();
    if(FX_SUCCESS != status)
    {
        return FX_SUCCESS;
    }

    /* Delete the file using the Azure FileX API */
    status = fx_file_delete(&g_fx_media, g_file_name1);

    if(FX_NOT_FOUND == status)
    {
        PRINT_INFO_STR("File does not exist\r\n");
        return FX_SUCCESS;
    }

    if(FX_SUCCESS != status)
    {
        RETURN_ERR_STR(status, "fx_file_delete failed\r\n");
    }

    /* Flush data into the physical media */
    status = fx_media_flush(&g_fx_media);
    RETURN_ERR_STR(status, "fx_media_flush failed\r\n");

    /* Delete the file successfully */
    PRINT_INFO_STR("File has been deleted\r\n");

    return FX_SUCCESS;
}
