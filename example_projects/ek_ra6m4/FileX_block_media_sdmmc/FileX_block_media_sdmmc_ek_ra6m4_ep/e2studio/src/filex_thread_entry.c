/***********************************************************************************************************************
 * File Name    : filex_thread_entry.c
 * Description  : Contains functions from the filex thread
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
#include "filex_thread.h"
#include "common_utils.h"
#include "filex_block_media_sdmmc_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup FileX_block_media_sdmmc_ep
 * @{
 **********************************************************************************************************************/

/* FileX Media Instance */
static FX_MEDIA g_fx_media0;
static uint8_t g_fx_media0_media_memory[G_FX_MEDIA0_MEDIA_MEMORY_SIZE];
/* File structure */
static FX_FILE g_file;
/* Timeout value */
volatile int32_t g_time_out = INT32_MAX;
ULONG       actual_events   = RESET_VALUE;
/* Variable to store time*/
time_format_t g_set_time = {RESET_VALUE};

/* Global Variables */
uint16_t g_write_data[WRITE_ITEM_SIZE];   /* Data(10k) to write to file */
uint16_t g_read_data[WRITE_ITEM_SIZE] ;

/* Function Declaration */
static UINT filex_blockmedia_sdmmc_operation(void);
static void assign_month_value(time_format_t * time, char * read_buffer);
static void update_buffer(void);
static void deinit_media(void);
/* Writes predefined data into the given file.*/
static UINT filex_blockmedia_sdmmc_write(FX_FILE * p_file, CHAR * p_file_name);

/* Reads written data from the given file.*/
static UINT filex_blockmedia_sdmmc_read(FX_FILE * p_file, CHAR * p_file_name);

/* FileX_Thread entry function */
void filex_thread_entry(void)
{

    fsp_err_t err = FSP_SUCCESS;
    UINT fx_ret_val = FX_SUCCESS;

    /* Initialize the RTT Thread.*/
    rtt_thread_init_check();

    /* print the banner and EP info. */
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_BANNER, RESET_VALUE, NULL);

    /* Open and configure FileX Block Media interface */
    err = RM_FILEX_BLOCK_MEDIA_Open(&g_rm_filex_block_media_0_ctrl, &g_rm_filex_block_media_0_cfg);
    if (FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("\r\nError in initializing FILEX_BLOCK_MEDIA\r\n");
        ERROR_TRAP(err);
    }

    /* Initialize the FileX system.*/
    fx_system_initialize();

    /* Check if card inserted event is received or not and proceed further. */
    fx_ret_val = tx_event_flags_get(&my_event_flags_group, RM_BLOCK_MEDIA_EVENT_MEDIA_INSERTED, TX_AND_CLEAR, &actual_events, TIMER_TICKS);
    if((TX_SUCCESS == fx_ret_val) && (RM_BLOCK_MEDIA_EVENT_MEDIA_INSERTED == actual_events))
    {
        PRINT_INFO_STR("\r\nSD Card is inserted");
    }
    else
    {
        err = FSP_ERR_TIMEOUT;
        PRINT_ERR_STR("\r\nInsert card and Restart the application.\r\n");
        ERROR_TRAP(err);
    }
    actual_events   = RESET_VALUE;

    /* Extract current Date and Time from compiler MACROS */
    char read_time[MAX_BYTES] = {NULL_CHAR};
    strcat(read_time, __DATE__);
    strcat(read_time," ");
    strcat(read_time, __TIME__);

    /* Date and Time extracted from MACROs are converted to time format so that,
     *  this can be used to add time stamping to the files on SD card */
    g_set_time.date = atoi(&read_time[4]);
    g_set_time.year = atoi(&read_time[7]);
    g_set_time.hour = atoi(&read_time[12]);
    g_set_time.min  = atoi(&read_time[15]);
    g_set_time.sec  = atoi(&read_time[18]);

    if(g_set_time.date > DATE_VALUE_CHECK)
    {
        g_set_time.date -= DATE_VALUE_SUB;
    }

    /* Convert returned month name to month value */
    assign_month_value(&g_set_time, &read_time[0]);

    /* Set system date and time */
    fx_system_date_set(g_set_time.year, g_set_time.month, g_set_time.date);
    fx_system_time_set(g_set_time.hour, g_set_time.min, g_set_time.sec);

    /* Open and Initialize the Block Media SDMMC using Azure FileX */
    fx_ret_val = fx_media_open(&g_fx_media0, "&g_fx_media0", RM_FILEX_BLOCK_MEDIA_BlockDriver,
                               (void *) &g_rm_filex_block_media_0_instance,
                               g_fx_media0_media_memory,
                               G_FX_MEDIA0_MEDIA_MEMORY_SIZE);
    if(FX_SUCCESS != fx_ret_val)
    {
        PRINT_ERR_STR("Media open failed.");
        ERROR_TRAP(fx_ret_val);
    }

    while(true)
    {
        fx_ret_val = tx_event_flags_get(&my_event_flags_group, RM_BLOCK_MEDIA_EVENT_MEDIA_REMOVED, TX_AND_CLEAR, &actual_events, TX_NO_WAIT);
        if((TX_SUCCESS == fx_ret_val) && (RM_BLOCK_MEDIA_EVENT_MEDIA_REMOVED == actual_events))
        {
            PRINT_INFO_STR("\r\nSD Card is removed. Please insert SD card to proceed.");
            /* Check if card inserted event is received or not and proceed further. */
            fx_ret_val = tx_event_flags_get(&my_event_flags_group, RM_BLOCK_MEDIA_EVENT_MEDIA_INSERTED,TX_AND_CLEAR, &actual_events, TX_WAIT_FOREVER);
            if((TX_SUCCESS == fx_ret_val) && (RM_BLOCK_MEDIA_EVENT_MEDIA_INSERTED == actual_events))
            {
                PRINT_INFO_STR("\r\nSD Card is inserted");
            }
        }
        actual_events   = RESET_VALUE;

        /* Perform file operations using blockmedia sdmmc */
        fx_ret_val = filex_blockmedia_sdmmc_operation();
        if(FX_SUCCESS != fx_ret_val)
        {
            PRINT_ERR_STR("Error in filex_blockmedia_sdmmc_operation");
            deinit_media();
            ERROR_TRAP(fx_ret_val);
        }
    }
}

/*******************************************************************************************************************//**
 * @brief       Perform file operations using blockmedia sdmmc
 * @param[IN]   None
 * @retval      FileX API return values.
 **********************************************************************************************************************/
static UINT filex_blockmedia_sdmmc_operation(void)
{
    UINT  status    = FX_SUCCESS;
    UINT  err       = TX_SUCCESS;
    /*Directory Name */
    CHAR dir_name[] = "test_dir";
    /* File names */
    CHAR file_name0[] = "/file_one.txt";
    CHAR file_name1[] = "/file_two.txt";
    CHAR file_name2[] = "/file_three.txt";

    /*Print Menu */
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_MENU, RESET_VALUE, NULL);

    uint8_t read_data = RESET_VALUE;
    /* Wait for user input. */
    do
    {
        err = check_for_RTT_user_IP(&read_data);
    }while (err != TX_SUCCESS);

    switch (read_data)
    {
        case FORMAT_CARD :
        {
            status = tx_event_flags_get(&my_event_flags_group, RM_BLOCK_MEDIA_EVENT_MEDIA_REMOVED, TX_AND_CLEAR, &actual_events, TX_NO_WAIT);
            if((TX_SUCCESS == status) && (RM_BLOCK_MEDIA_EVENT_MEDIA_REMOVED == actual_events))
            {
                status = FX_MEDIA_INVALID;
                actual_events   = RESET_VALUE;
                PRINT_ERR_STR("\r\nSD card is removed. Please insert SD card to proceed.");
                return status;

            }
            else
            {
                PRINT_INFO_STR("\r\nFAT32 formatting of SD card, as per user input.");
                /* Format SD card
                 * Note: Here, exFAT support will be added in future once it is enabled in FileX driver.*/
                status = fx_media_format(&g_fx_media0,                               // Pointer to FileX media control block.
                                         RM_FILEX_BLOCK_MEDIA_BlockDriver,           // Driver entry
                                         (void *) &g_rm_filex_block_media_0_instance,// Pointer to Block Media Driver
                                         g_fx_media0_media_memory,                   // Media buffer pointer
                                         G_FX_MEDIA0_MEDIA_MEMORY_SIZE,              // Media buffer size
                                         (char *) G_FX_MEDIA0_VOLUME_NAME,           // Volume Name
                                         G_FX_MEDIA0_NUMBER_OF_FATS,                 // Number of FATs
                                         G_FX_MEDIA0_DIRECTORY_ENTRIES,              // Directory Entries
                                         G_FX_MEDIA0_HIDDEN_SECTORS,                 // Hidden sectors
                                         G_FX_MEDIA0_TOTAL_SECTORS,                  // Total sectors
                                         G_FX_MEDIA0_BYTES_PER_SECTOR,               // Sector size
                                         G_FX_MEDIA0_SECTORS_PER_CLUSTER,            // Sectors per cluster
                                         MEDIA_SECTOR_HEADS_VALUE,                   // Heads (disk media)
                                         MEDIA_SECTOR_HEADS_VALUE);

                if ((uint8_t)FX_SUCCESS != status)
                {
                    PRINT_ERR_STR("\r\nMedia Format failed\r\n");
                    return status;
                }

                PRINT_INFO_STR("\r\nMedia formatted successfully!\r\n");
            }
        }
        break;
        case FILE_CREATION :
        {
            status = tx_event_flags_get(&my_event_flags_group, RM_BLOCK_MEDIA_EVENT_MEDIA_REMOVED, TX_AND_CLEAR, &actual_events, TX_NO_WAIT);
            if((TX_SUCCESS == status) && (RM_BLOCK_MEDIA_EVENT_MEDIA_REMOVED == actual_events))
            {
                status = FX_MEDIA_INVALID;
                actual_events   = RESET_VALUE;
                PRINT_ERR_STR("\r\nSD card is removed. Please insert SD card to proceed.");
                return status;

            }
            else
            {
                ULONG space_available = RESET_VALUE;

                PRINT_INFO_STR("\r\nChecking for available space in media.\r\n");

                /* Check if space is available on media to proceed */
                status = fx_media_space_available(&g_fx_media0, &space_available);
                if ((uint8_t)FX_SUCCESS != status)
                {
                    PRINT_ERR_STR("\r\nMedia space check failed\r\n");
                    return status;
                }

                app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_MEDIA_SPACE, sizeof(ULONG *), &space_available);

                if (DIR_SPACE_REQUIRED <= space_available)
                {
                    PRINT_INFO_STR("\r\nRequired space for Directory creation is available\r\n");
                }
                else
                {
                    status = FX_NO_MORE_SPACE;
                    PRINT_ERR_STR("\r\nRequired space for Directory/File creation is not available\r\n");
                    return status;
                }
                space_available = RESET_VALUE;

                /* Create directory on media inserted */
                status = fx_directory_create(&g_fx_media0, dir_name);
                if ((uint8_t)FX_ALREADY_CREATED == status)
                {
                    PRINT_INFO_STR("\r\nDirectory is already created.\r\n");
                }
                else if ((uint8_t)FX_SUCCESS != status)
                {
                    PRINT_ERR_STR("\r\nDirectory creation failed.\r\n");
                    return status;
                }
                else
                {
                    PRINT_INFO_STR("\r\nDirectory created successfully.\r\n");
                }

                /* Create necessary files. */
                status = fx_file_create(&g_fx_media0, file_name0);
                if ((uint8_t)FX_ALREADY_CREATED == status)
                {
                    PRINT_INFO_STR("\r\nFile1 is already created.\r\n");
                }
                else if ((uint8_t)FX_SUCCESS != status)
                {
                    PRINT_ERR_STR("\r\nFile1 creation failed.\r\n");
                    return status;
                }
                else
                {
                    PRINT_INFO_STR("\r\nFile1 created successfully.\r\n");
                }

                status = fx_file_create(&g_fx_media0, file_name1);
                if ((uint8_t)FX_ALREADY_CREATED == status)
                {
                    PRINT_INFO_STR("\r\nFile2 is already created.\r\n");
                }
                else if ((uint8_t)FX_SUCCESS != status)
                {
                    PRINT_ERR_STR("\r\nFile2 creation failed.\r\n");
                    return status;
                }
                else
                {
                    PRINT_INFO_STR("\r\nFile2 created successfully.\r\n");
                }

                status = fx_file_create(&g_fx_media0, file_name2);
                if ((uint8_t)FX_ALREADY_CREATED == status)
                {
                    PRINT_INFO_STR("\r\nFile3 is already created.\r\n");
                }
                else if ((uint8_t)FX_SUCCESS != status)
                {
                    PRINT_ERR_STR("\r\nFile3 creation failed.\r\n");
                    return status;
                }
                else
                {
                    PRINT_INFO_STR("\r\nFile3 created successfully.\r\n");
                }

                /* Check if required space to write data is available on media to proceed */
                status = fx_media_space_available(&g_fx_media0, &space_available);
                if ((uint8_t)FX_SUCCESS != status)
                {
                    PRINT_ERR_STR("\r\nMedia space check failed\r\n");
                    return status;
                }

                app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_MEDIA_SPACE, sizeof(ULONG *), &space_available);

                if (WRITE_ITEM_SIZE <= space_available)
                {
                    PRINT_INFO_STR("\r\nRequired space to write pre-defined data is available\r\n");
                }
                else
                {
                    status = FX_NO_MORE_SPACE;
                    PRINT_ERR_STR("\r\nRequired space to write pre-defined data is not available\r\n");
                    return status;
                }

                /* Perform write operations on the file created */
                status = filex_blockmedia_sdmmc_write(&g_file, file_name0);
                if ((uint8_t)FX_SUCCESS != status)
                {
                    PRINT_ERR_STR("\r\nfilex_blockmedia_sdmmc_write failed\r\n");
                    return status;
                }

                /* Read the file contents */
                status = filex_blockmedia_sdmmc_read(&g_file, file_name0);
                if ((uint8_t)FX_SUCCESS != status)
                {
                    PRINT_ERR_STR("\r\nfilex_blockmedia_sdmmc_write failed\r\n");
                    return status;
                }
            }

        }
        break;
        case DISPLAY_CONTENT :
        {
            status = tx_event_flags_get(&my_event_flags_group, RM_BLOCK_MEDIA_EVENT_MEDIA_REMOVED, TX_AND_CLEAR, &actual_events, TX_NO_WAIT);
            if((TX_SUCCESS == status) && (RM_BLOCK_MEDIA_EVENT_MEDIA_REMOVED == actual_events))
            {
                actual_events   = RESET_VALUE;
                status = FX_MEDIA_INVALID;
                PRINT_ERR_STR("\r\nSD card is removed. Please insert SD card to proceed.");
                return status;

            }
            else
            {
                /* Variable to store file details */
                file_info_t file_info = {RESET_VALUE};

                PRINT_INFO_STR("\r\nFile Name           Attribute       File Size   Date & Time of last modification");
                /* Extract first entry details in the current directory */
                status = fx_directory_first_full_entry_find(&g_fx_media0, file_info.directory_name, (UINT *)file_info.attributes, &file_info.size, &file_info.time_stamp.year,
                                                            &file_info.time_stamp.month, &file_info.time_stamp.date, &file_info.time_stamp.hour, &file_info.time_stamp.min, &file_info.time_stamp.sec);
                if ((uint8_t)FX_SUCCESS != status)
                {
                    PRINT_ERR_STR("\r\fx_directory_first_full_entry_find failed\r\n");
                    return status;
                }
                do
                {
                    /* Assign string according to attribute, describing the file */
                    if(RESET_VALUE != (*(file_info.attributes) & FX_DIRECTORY))
                    {
                        strncpy(file_info.attributes, "directory", ARRAY_SIZE );
                    }
                    else if(*(file_info.attributes) & FX_READ_ONLY)
                    {
                        strncpy(file_info.attributes, "read only file", ARRAY_SIZE );
                    }
                    else
                    {
                        strncpy(file_info.attributes, "writable file", ARRAY_SIZE );
                    }
                    app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_FILE_ENTRIES, sizeof(file_info_t), &file_info);
                    /* Extract details of next entry in the current directory */
                    status = fx_directory_next_full_entry_find(&g_fx_media0, file_info.directory_name, (UINT *)file_info.attributes, &file_info.size, &file_info.time_stamp.year,
                                                               &file_info.time_stamp.month, &file_info.time_stamp.date, &file_info.time_stamp.hour, &file_info.time_stamp.min, &file_info.time_stamp.sec);
                    if (((uint8_t)FX_SUCCESS != status) && ((uint8_t)FX_NO_MORE_ENTRIES != status))
                    {
                        PRINT_ERR_STR("\r\nfx_directory_next_full_entry_find failed\r\n");
                        return status;
                    }
                }while((FX_NO_MORE_ENTRIES != (status)));

                /* Re-initializing the status since the error conditions are already handled */
                status = FX_SUCCESS;
            }
        }
        break;
        default :
        {
            PRINT_INFO_STR("\r\nPlease enter a valid input.");
            break;
        }
    }
    return status;
}

/*******************************************************************************************************************//**
 * @brief     This function updates write buffer with data and clears read buffer.
 * @param[IN]   None
 * @retval      None.
 ***********************************************************************************************************************/
static void update_buffer(void)
{
    for (uint16_t count = RESET_VALUE ; count < WRITE_ITEM_SIZE ; count++)
    {
        g_write_data[count] = count;
        g_read_data[count] = RESET_VALUE;
    }
}

/*******************************************************************************************************************//**
 * @brief       Performs write operation to file
 * @param[IN]   p_file          Pointer to file
 * @param[IN]   p_file_name     Pointer to file name
 * @param[IN]   number          Number to be written to file
 * @retval      FileX API return values.
 **********************************************************************************************************************/
static UINT filex_blockmedia_sdmmc_write(FX_FILE * p_file, CHAR * p_file_name)
{
    UINT  status    = FX_SUCCESS;

    /* Open the file for write */
    status = fx_file_open(&g_fx_media0, p_file, p_file_name,  FX_OPEN_FOR_WRITE);
    if ((uint8_t)FX_SUCCESS != status)
    {
        PRINT_ERR_STR("\r\nFile open failed\r\n");
        return status;
    }

    /* Clear file contents */
    status = fx_file_truncate(p_file, TRUNCATION_VALUE);
    if ((uint8_t)FX_SUCCESS != status)
    {
        PRINT_ERR_STR("\r\nFile truncation failed\r\n");
        return status;
    }

    /* Fill write buffer with data */
    update_buffer();

    /* Write the fixed data to the opened file */
    status = fx_file_write(p_file, g_write_data, WRITE_ITEM_SIZE);
    if ((uint8_t)FX_SUCCESS != status)
    {
        PRINT_ERR_STR("\r\nFile write failed\r\n");
        return status;
    }

    /* Check if write operation is completed and event is received from callback*/
    status = tx_event_flags_get(&my_event_flags_group, RM_BLOCK_MEDIA_EVENT_WAIT_END,TX_AND_CLEAR, &actual_events, TIMER_TICKS);
    if((TX_SUCCESS == status) && (RM_BLOCK_MEDIA_EVENT_WAIT_END == actual_events))
    {
        PRINT_INFO_STR("\r\nWrite operation completed.");
    }
    actual_events   = RESET_VALUE;

    /* Set date and Time */
    status = fx_file_date_time_set(&g_fx_media0, p_file_name, g_set_time.year, g_set_time.month, g_set_time.date,
                                   g_set_time.hour, g_set_time.min, g_set_time.sec);
    if ((uint8_t)FX_SUCCESS != status)
    {
        PRINT_ERR_STR("\r\nSetting Date and Time failed\r\n");
        return status;
    }

    /* Close the file */
    status = fx_file_close(&g_file);
    if ((uint8_t)FX_SUCCESS != status)
    {
        PRINT_ERR_STR("\r\nFile close failed\r\n");
        return status;
    }

    /* Flush data to physical media */
    status = fx_media_flush(&g_fx_media0);
    if ((uint8_t)FX_SUCCESS != status)
    {
        PRINT_ERR_STR("\r\nMedia flush failed\r\n");
        return status;
    }
    return status;
}

/*******************************************************************************************************************//**
 * @brief       Performs read operation to file
 * @param[IN]   p_file          Pointer to file
 * @param[IN]   p_file_name     Pointer to file name
 * @retval      Number of bytes read from file.
 **********************************************************************************************************************/
static UINT filex_blockmedia_sdmmc_read(FX_FILE * p_file, CHAR * p_file_name)
{
    /* Number of read bytes  */
    static ULONG read_size = RESET_VALUE;
    UINT status = FX_SUCCESS;

    /* Open the file for read */
    status = fx_file_open(&g_fx_media0, p_file, p_file_name, FX_OPEN_FOR_READ);
    if ((uint8_t)FX_SUCCESS != status)
    {
        PRINT_ERR_STR("\r\nFile open failed\r\n");
        return status;
    }

    /* Read the file */
    status = fx_file_read(p_file, g_read_data, WRITE_ITEM_SIZE, &read_size);
    if ((uint8_t)FX_SUCCESS != status)
    {
        PRINT_ERR_STR("\r\nFile read failed\r\n");
        return status;
    }

    /* Check if read operation is completed and event is received from callback*/
    status = tx_event_flags_get(&my_event_flags_group, RM_BLOCK_MEDIA_EVENT_WAIT_END,TX_AND_CLEAR, &actual_events, TIMER_TICKS);
    if((TX_SUCCESS == status) && (RM_BLOCK_MEDIA_EVENT_WAIT_END == actual_events))
    {
        PRINT_INFO_STR("\r\nRead operation completed successfully.");
    }
    actual_events   = RESET_VALUE;

    /* Close the file */
    status = fx_file_close(p_file);
    if ((uint8_t)FX_SUCCESS != status)
    {
        PRINT_ERR_STR("\r\nFile close failed\r\n");
        return status;
    }

    /* Compare Write and Read data. */
    if(RESET_VALUE == memcmp(g_write_data, g_read_data, WRITE_ITEM_SIZE))
    {
        PRINT_INFO_STR("\r\nWrite and Read data matched.\r\n");
    }
    else
    {
        status = FX_INVALID_CHECKSUM;
        PRINT_ERR_STR("\r\nWrite and Read data did not match\r\n");
        return status;
    }
    return status;
}

/*******************************************************************************************************************//**
 * @brief       This functions assigns month value based on compiler returned month.
 * @param[IN]   time                    date and time to be modified
 * @param[IN]   read_buffer             Buffer that contains user input time values
 * @retval      None
 **********************************************************************************************************************/
static void assign_month_value(time_format_t * time, char * read_buffer)
{
    /* This compiler MACRO that had date value is processed using the following conversion, where ASCII value is converted to
     * integer value and month value is assigned. */
    switch (read_buffer[0])
    {
        case JAN_JUN_JUL:
        {
            if(ASCII_LOWER_A == read_buffer[1])
            {
                time->month = JANUARY;
            }
            else if(ASCII_LOWER_N == read_buffer[2])
            {
                time->month = JUNE;
            }
            else
            {
                time->month = JULY;
            }
        }
        break;
        case FEB:
        {
            time->month = FEBRUARY;
        }
        break;
        case MAR_MAY:
        {
            if(ASCII_LOWER_R == read_buffer[2])
            {
                time->month = MARCH;
            }
            else
            {
                time->month = MAY;
            }
        }
        break;
        case APR_AUG:
        {
            if(ASCII_LOWER_P == read_buffer[1])
            {
                time->month = APRIL;
            }
            else
            {
                time->month = AUGUST;
            }
        }
        break;
        case SEP:
        {
            time->month = SEPTEMBER;
        }
        break;
        case OCT:
        {
            time->month = OCTOBER;
        }
        break;
        case NOV:
        {
            time->month = NOVEMBER;
        }
        break;
        case DEC:
        {
            time->month = DECEMBER;
        }
        break;
        default :
        {

        }
        break;
    }
}

/*******************************************************************************************************************//**
 * @brief       Deinitializes opened media
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void deinit_media(void)
{
    UINT status = FX_SUCCESS;
    fsp_err_t err = FSP_SUCCESS;

    /* Close the opened media  */
    status = fx_media_close(&g_fx_media0);
    /* Handle error in-case of failure */
    if (FX_SUCCESS != status)
    {
        PRINT_ERR_STR("\r\nMedia close failed\r\n");;
    }

    /* Close opened FileX Block Media interface */
    err = RM_FILEX_BLOCK_MEDIA_Close(&g_rm_filex_block_media_0_ctrl);
    /* Handle error in-case of failure */
    if (FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("\r\nError in closing FILEX_BLOCK_MEDIA\r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief Block media callback function.
 * @param[in]  p_args
 * @retval     None
 **********************************************************************************************************************/
void g_rm_filex_block_media_0_callback(rm_filex_block_media_callback_args_t *p_args)
{

    switch(p_args->event)
    {
        case RM_BLOCK_MEDIA_EVENT_MEDIA_INSERTED:
        {
            tx_event_flags_set(&my_event_flags_group, RM_BLOCK_MEDIA_EVENT_MEDIA_INSERTED, TX_OR);
        }
        break;
        case RM_BLOCK_MEDIA_EVENT_MEDIA_REMOVED:
        {
            tx_event_flags_set(&my_event_flags_group, RM_BLOCK_MEDIA_EVENT_MEDIA_REMOVED, TX_OR);
        }
        break;
        case RM_BLOCK_MEDIA_EVENT_WAIT_END:
        {
            /* FileX driver returns RM_BLOCK_MEDIA_EVENT_WAIT_END event when an operation is completed.
             * It can be read, write or erase operation. If there is an occurrence of error event,
             * the FileX callback will still return the same event and the respective API
             * will return the specific error code. */
            tx_event_flags_set(&my_event_flags_group, RM_BLOCK_MEDIA_EVENT_WAIT_END, TX_OR);
        }
        break;
        case RM_BLOCK_MEDIA_EVENT_POLL_STATUS:
        case RM_BLOCK_MEDIA_EVENT_MEDIA_SUSPEND:
        case RM_BLOCK_MEDIA_EVENT_MEDIA_RESUME:
        case RM_BLOCK_MEDIA_EVENT_WAIT:
        default:
        {
            break;
        }
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup FileX_block_media_sdmmc_ep)
 **********************************************************************************************************************/
