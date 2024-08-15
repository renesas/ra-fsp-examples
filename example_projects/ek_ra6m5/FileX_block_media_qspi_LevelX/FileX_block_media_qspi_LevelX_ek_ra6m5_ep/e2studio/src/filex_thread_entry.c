/***********************************************************************************************************************
 * File Name    : filex_thread_entry.c
 * Description  : Contains functions from the filex main thread
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "filex_thread.h"
#include "common_utils.h"
#include "filex_block_media_qspi_levelx_ep.h"
#include "setup_qspi.h"

/*******************************************************************************************************************//**
 * @addtogroup FileX_block_media_qspi_LevelX_ep
 * @{
 **********************************************************************************************************************/

/* FileX Media Instance */
static FX_MEDIA g_fx_media0;
static uint8_t g_fx_media0_media_memory[G_FX_MEDIA0_MEDIA_MEMORY_SIZE];
/* File structure */
static FX_FILE g_file;
/* Variable to store time*/
time_format_t g_set_time = {RESET_VALUE};
/* Buffer to store read/write data */
uint8_t  g_write_data[WRITE_ITEM_SIZE];
uint8_t  g_read_data[WRITE_ITEM_SIZE];

/* Function Declaration */
static UINT filex_qspi_levelx_operation(void);
static fsp_err_t erase_qspi_flash(void);
static void get_current_date_time(time_format_t * time);
static void assign_month_value(time_format_t * time, char * read_buffer);
static void deinit_media(void);

/* FileX_Thread entry function */
void filex_thread_entry(void)
{
    fsp_err_t err = FSP_SUCCESS;
    UINT fx_ret_val = FX_SUCCESS;

    /* Initialize the RTT Thread.*/
    rtt_thread_init_check();

    /* print the banner and EP info. */
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_BANNER, RESET_VALUE, NULL);

    /* Initialize QSPI Flash device */
    err = setup_qspi(&g_qspi0_ctrl, &g_qspi0_cfg);
    if(FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("Error in initializing QSPI Flash device");
        ERROR_TRAP(err);
    }
    else
    {
        PRINT_INFO_STR("Initialized QSPI Flash device");
    }

    /* Initialize FileX system */
    fx_system_initialize();

    /* Initialize LevelX system */
    fx_ret_val = lx_nor_flash_initialize();
    if (LX_SUCCESS != fx_ret_val)
    {
        PRINT_ERR_STR("Failed to initialize NOR flash support");
        ERROR_TRAP(fx_ret_val);
    }

    /* Extract current Date and Time from compiler MACROS */
    get_current_date_time(&g_set_time);

    /* Set system date */
    fx_ret_val = fx_system_date_set(g_set_time.year, g_set_time.month, g_set_time.date);
    if (FX_SUCCESS != fx_ret_val)
    {
        PRINT_ERR_STR("System date set failed");
        ERROR_TRAP(fx_ret_val);
    }

    /* Set system time */
    fx_ret_val = fx_system_time_set(g_set_time.hour, g_set_time.min, g_set_time.sec);
    if (FX_SUCCESS != fx_ret_val)
    {
        PRINT_ERR_STR("System time set failed");
        ERROR_TRAP(fx_ret_val);
    }

    while (true)
    {
        /* Perform file operations using blockmedia qspi with levelx */
        fx_ret_val = filex_qspi_levelx_operation();
        if(FX_SUCCESS != fx_ret_val)
        {
            PRINT_ERR_STR("Error in filex_qspi_levelx operation");
            deinit_media();
            ERROR_TRAP(fx_ret_val);
        }
    }
}

/*******************************************************************************************************************//**
 * @brief       Perform file operations using blockmedia qspi with levelx
 * @param[IN]   None
 * @retval      FileX API return values.
 **********************************************************************************************************************/
static UINT filex_qspi_levelx_operation(void)
{
    UINT status     = FX_SUCCESS;
    ULONG read_size = RESET_VALUE;
    /* File names */
    CHAR file_name1[MAX_FILE_NAME_SIZE] = "file_1.txt";
    CHAR file_name2[MAX_FILE_NAME_SIZE] = "file_2.txt";

    /*Print Menu */
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_MENU, RESET_VALUE, NULL);

    uint8_t read_data = RESET_VALUE;
    /* Wait for user input. */
    do
    {
        status = check_for_rtt_user_ip(&read_data);
    }while (TX_SUCCESS != status);

    switch(read_data)
    {
        case ERASE_QSPI_FLASH:
            /* Erase QSPI flash device before using LevelX for the first time. */
        {
            PRINT_INFO_STR("Erasing QSPI Flash device...");
            /* Erase QSPI flash device */
            status = erase_qspi_flash();
            if(FX_SUCCESS != status)
            {
                PRINT_ERR_STR("Error in erasing QSPI Flash device");
                return status;
            }

            PRINT_INFO_STR("Erasing QSPI Flash device successful");
        }
        break;
        case FORMAT_MEDIA:
        {
            /* Close media before format */
            status = fx_media_close(&g_fx_media0);
            if (FX_SUCCESS != status && FX_MEDIA_NOT_OPEN != status)
            {
                PRINT_ERR_STR("Media close failed");
                return status;
            }
            else
            {
                PRINT_INFO_STR("formatting QSPI Flash");
                /* Format QSPI Flash */
                status = fx_media_format(&g_fx_media0,                              // Pointer to FileX media control block.
                                         RM_FILEX_LEVELX_NOR_DeviceDriver,          // Driver entry
                                         (void *) &g_rm_filex_levelx_nor_instance,  // Pointer to LevelX NOR Driver
                                         g_fx_media0_media_memory,                  // Media buffer pointer
                                         G_FX_MEDIA0_MEDIA_MEMORY_SIZE,             // Media buffer size
                                         (char *) G_FX_MEDIA0_VOLUME_NAME,          // Volume Name
                                         G_FX_MEDIA0_NUMBER_OF_FATS,                // Number of FATs
                                         G_FX_MEDIA0_DIRECTORY_ENTRIES,             // Directory Entries
                                         G_FX_MEDIA0_HIDDEN_SECTORS,                // Hidden sectors
                                         G_FX_MEDIA0_TOTAL_SECTORS,                 // Total sectors
                                         G_FX_MEDIA0_BYTES_PER_SECTOR,              // Sector size
                                         G_FX_MEDIA0_SECTORS_PER_CLUSTER,           // Sectors per cluster
                                         MEDIA_SECTOR_HEADS_VALUE,                  // Heads (disk media)
                                         MEDIA_SECTORS_PER_HEAD);                   // Sectors per track (disk media)
                if (FX_SUCCESS != status)
                {
                    PRINT_ERR_STR("Media Format failed");
                    return status;
                }

                PRINT_INFO_STR("Format Operation is successful!");
            }
        }
        break;
        case FILE_1_WRITE:
            /* Create a text file and write pre-defined data in it. */
        {
            /* Open media for file operations */
            status = fx_media_open(&g_fx_media0,
                                   "&g_fx_media0",
                                   RM_FILEX_LEVELX_NOR_DeviceDriver,
                                   (void *) &g_rm_filex_levelx_nor_instance,
                                   g_fx_media0_media_memory,
                                   G_FX_MEDIA0_MEDIA_MEMORY_SIZE);
            if(FX_SUCCESS != status)
            {
                PRINT_ERR_STR("Media open failed");
                return status;
            }

            /* Create file1 */
            status = fx_file_create(&g_fx_media0, file_name1);
            if (FX_ALREADY_CREATED == status)
            {
                PRINT_INFO_STR("File1 is already created");
            }
            else if (FX_SUCCESS != status)
            {
                PRINT_ERR_STR("File1 creation failed");
                return status;
            }
            else
            {
                PRINT_INFO_STR("File1 created successfully");
            }

            /* Open the file for write */
            status = fx_file_open(&g_fx_media0, &g_file, file_name1, FX_OPEN_FOR_WRITE);
            if (FX_SUCCESS != status)
            {
                PRINT_ERR_STR("File open failed");
                return status;
            }

            /* Fill write buffer with data */
            UPDATE_BUFFER(g_write_data);

            /* Write the fixed data to the opened file */
            status = fx_file_write(&g_file, g_write_data, WRITE_ITEM_SIZE);
            if (FX_SUCCESS != status)
            {
                PRINT_ERR_STR("File write failed");
                return status;
            }
            else
            {
                PRINT_INFO_STR("Written data to File1 successfully");
            }

            /* Set date and Time */
            status = fx_file_date_time_set(&g_fx_media0, file_name1, g_set_time.year, g_set_time.month, g_set_time.date,
                                           g_set_time.hour, g_set_time.min, g_set_time.sec);
            if (FX_SUCCESS != status)
            {
                PRINT_ERR_STR("Setting Date and Time failed");
                return status;
            }

            /* Close the file */
            status = fx_file_close(&g_file);
            if (FX_SUCCESS != status)
            {
                PRINT_ERR_STR("File close failed");
                return status;
            }

            /* Close the media */
            status = fx_media_close(&g_fx_media0);
            if (FX_SUCCESS != status)
            {
                PRINT_ERR_STR("Media close failed");
                return status;
            }

            PRINT_INFO_STR("File operation 1 is successful");
        }
        break;
        case FILE_2_WRITE_READ_COMPARE:
            /* Create a text file, write pre-defined data in it and read data from it.
             * Compare read data with write data and print result.
             */
        {
            /* Open media for file operations */
            status = fx_media_open(&g_fx_media0,
                                   "&g_fx_media0",
                                   RM_FILEX_LEVELX_NOR_DeviceDriver,
                                   (void *) &g_rm_filex_levelx_nor_instance,
                                   g_fx_media0_media_memory,
                                   G_FX_MEDIA0_MEDIA_MEMORY_SIZE);
            if(FX_SUCCESS != status)
            {
                PRINT_ERR_STR("Media open failed");
                return status;
            }

            /* Delete file2 if already present */
            status = fx_file_delete(&g_fx_media0, file_name2);
            if (status == FX_NOT_FOUND)
            {
                PRINT_INFO_STR("File2 not found");
            }
            else if (FX_SUCCESS != status)
            {
                PRINT_ERR_STR("File2 deletion failed");
                return status;
            }
            else
            {
                PRINT_INFO_STR("File2 deleted successfully");
            }

            /* Create file2 */
            status = fx_file_create(&g_fx_media0, file_name2);
            if (FX_SUCCESS != status)
            {
                PRINT_ERR_STR("File2 creation failed");
                return status;
            }
            else
            {
                PRINT_INFO_STR("File2 created successfully");
            }

            /* Open the file for write */
            status = fx_file_open(&g_fx_media0, &g_file, file_name2, FX_OPEN_FOR_WRITE);
            if (FX_SUCCESS != status)
            {
                PRINT_ERR_STR("File open failed");
                return status;
            }

            /* Fill write buffer with data */
            UPDATE_BUFFER(g_write_data);

            /* Write the fixed data to the opened file */
            status = fx_file_write(&g_file, g_write_data, WRITE_ITEM_SIZE);
            if (FX_SUCCESS != status)
            {
                PRINT_ERR_STR("File write failed");
                return status;
            }
            else
            {
                PRINT_INFO_STR("Written data to File2 successfully");
            }

            /* Set date and Time */
            status = fx_file_date_time_set(&g_fx_media0, file_name2, g_set_time.year, g_set_time.month, g_set_time.date,
                                           g_set_time.hour, g_set_time.min, g_set_time.sec);
            if (FX_SUCCESS != status)
            {
                PRINT_ERR_STR("Setting Date and Time failed");
                return status;
            }

            /* Close the file */
            status = fx_file_close(&g_file);
            if (FX_SUCCESS != status)
            {
                APP_PRINT("File close failed");
                return status;
            }

            /* Open the file for read */
            status = fx_file_open(&g_fx_media0, &g_file, file_name2, FX_OPEN_FOR_READ);
            if (FX_SUCCESS != status)
            {
                PRINT_ERR_STR("File open failed");
                return status;
            }

            /* Fill read buffer with zero */
            memset(&g_read_data, RESET_VALUE, WRITE_ITEM_SIZE);

            /* Read the file */
            status = fx_file_read(&g_file, g_read_data, WRITE_ITEM_SIZE, &read_size);
            if (FX_SUCCESS != status)
            {
                PRINT_ERR_STR("File read failed");
                return status;
            }
            else
            {
                PRINT_INFO_STR("Read data from File2 successfully");
            }

            /* Close the file */
            status = fx_file_close(&g_file);
            if (FX_SUCCESS != status)
            {
                PRINT_ERR_STR("File close failed");
                return status;
            }

            /* Compare Write and Read data. */
            if(RESET_VALUE != memcmp(g_write_data, g_read_data, WRITE_ITEM_SIZE))
            {
                status = FX_INVALID_CHECKSUM;
                PRINT_ERR_STR("Write and Read data did not match");
                return status;
            }
            else
            {
                PRINT_INFO_STR("Write and Read data matched");
            }

            /* Close the media */
            status = fx_media_close(&g_fx_media0);
            if (FX_SUCCESS != status)
            {
                PRINT_ERR_STR("Media close failed");
                return status;
            }

            PRINT_INFO_STR("File operation 2 is successful");
        }
        break;
        default:
        {
            PRINT_INFO_STR("Please enter a valid input");
            break;
        }
    }
    return status;
}

/*******************************************************************************************************************//**
 * @brief       This functions erases the flash memory using lower level flash APIs.
 * @param[IN]   None
 * @retval      FX_SUCCESS              Upon successful completion of the function
 * @retval      Any other error code    If erase fails
 **********************************************************************************************************************/
static fsp_err_t erase_qspi_flash(void)
{
    fsp_err_t err = FSP_SUCCESS;
    spi_flash_status_t write_status;
    uint32_t status_poll_remaining;

    /* SPI Flash instance for accessing lower level APIs */
    spi_flash_instance_t * p_spi_flash_instance = (spi_flash_instance_t *) g_rm_levelx_nor_spi0_cfg.p_lower_lvl;

    /* Open QSPI flash for erasing  */
    err = p_spi_flash_instance->p_api->open(p_spi_flash_instance->p_ctrl,p_spi_flash_instance->p_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Chip erase */
    err = p_spi_flash_instance->p_api->erase(p_spi_flash_instance->p_ctrl,
                                             (uint8_t *) g_rm_levelx_nor_spi0_cfg.base_address,
                                             SPI_FLASH_ERASE_SIZE_CHIP_ERASE);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Get number of times to poll for operation complete status before returning an error. */
    status_poll_remaining = g_rm_levelx_nor_spi0_cfg.poll_status_count;

    /* Check write status and status poll remaining for timeout error */
    do
    {
        /* Get write status */
        err = p_spi_flash_instance->p_api->statusGet(p_spi_flash_instance->p_ctrl, &write_status);
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        APP_PRINT(RESET_VALUE == (status_poll_remaining % PERIODIC_PRINT_OUT) ? ".":"");
        APP_PRINT(RESET_VALUE == (status_poll_remaining % PERIODIC_LINE_FEED) ? "\n":"");

    }while((true == write_status.write_in_progress) && (RESET_VALUE < status_poll_remaining--));

    /* Check if timeout occurred */
    if (RESET_VALUE == status_poll_remaining)
    {
        err = FSP_ERR_TIMEOUT;
        return err;
    }

    /* Close QSPI flash */
    err = p_spi_flash_instance->p_api->close(p_spi_flash_instance->p_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Format the media */
    err = fx_media_format(&g_fx_media0,                              // Pointer to FileX media control block.
                          RM_FILEX_LEVELX_NOR_DeviceDriver,          // Driver entry
                          (void *) &g_rm_filex_levelx_nor_instance,  // Pointer to LevelX NOR Driver
                          g_fx_media0_media_memory,                  // Media buffer pointer
                          G_FX_MEDIA0_MEDIA_MEMORY_SIZE,             // Media buffer size
                          (char *) G_FX_MEDIA0_VOLUME_NAME,          // Volume Name
                          G_FX_MEDIA0_NUMBER_OF_FATS,                // Number of FATs
                          G_FX_MEDIA0_DIRECTORY_ENTRIES,             // Directory Entries
                          G_FX_MEDIA0_HIDDEN_SECTORS,                // Hidden sectors
                          G_FX_MEDIA0_TOTAL_SECTORS,                 // Total sectors
                          G_FX_MEDIA0_BYTES_PER_SECTOR,              // Sector size
                          G_FX_MEDIA0_SECTORS_PER_CLUSTER,           // Sectors per cluster
                          MEDIA_SECTOR_HEADS_VALUE,                  // Heads (disk media)
                          MEDIA_SECTORS_PER_HEAD);                   // Sectors per track (disk media)

    return err;
}

/*******************************************************************************************************************//**
 * @brief       This functions gets current date and time based on the compiler.
 * @param[OUT]  *time       date and time to be obtained
 * @retval      None
 **********************************************************************************************************************/
static void get_current_date_time(time_format_t * time)
{
    /* Buffer to read Date and Time */
    char read_date_time[DATE_TIME_MAX_LEN] = {NULL_CHAR};

    strcat(read_date_time, __DATE__);
    strcat(read_date_time," ");
    strcat(read_date_time, __TIME__);

    /* Date and Time extracted from MACROs are converted to time format so that,
     *  this can be used to add time stamping to the files on SD card */
    time->date = atoi(&read_date_time[4]);
    time->year = atoi(&read_date_time[7]);
    time->hour = atoi(&read_date_time[12]);
    time->min  = atoi(&read_date_time[15]);
    time->sec  = atoi(&read_date_time[18]);

    if(time->date > DATE_VALUE_CHECK)
    {
        time->date -= DATE_VALUE_SUB;
    }

    /* This compiler MACRO that had date value is processed using the following conversion, where ASCII value is converted to
     * integer value and month value is assigned. */
    assign_month_value(time, &read_date_time[0]);
}

/*******************************************************************************************************************//**
 * @brief       This function assigns month as per the read value.
 * @param[IN]   *time         Pointer to time format structure
 * @param[IN]   read_buffer  buffer to store captured time and data format.
 * @retval      None
 ***********************************************************************************************************************/
static void assign_month_value(time_format_t * time, char * read_buffer)
{

    switch (read_buffer[RESET_VALUE])
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

    /* Close the opened media  */
    status = fx_media_close(&g_fx_media0);
    /* Handle error in-case of failure */
    if (FX_SUCCESS != status)
    {
        PRINT_ERR_STR("Media close failed");
    }
}

/*******************************************************************************************************************//**
 * @brief FileX LevelX NOR callback function.
 * @param[in]  p_args
 * @retval     None
 **********************************************************************************************************************/
void g_rm_filex_levelx_nor_0_callback(rm_filex_levelx_nor_callback_args_t *p_args)
{
    if (p_args->event == RM_FILEX_LEVELX_NOR_EVENT_BUSY)
    {
        /* Put the thread to sleep while waiting for operation to complete. */
        tx_thread_sleep(1);
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup FileX_block_media_qspi_LevelX_ep)
 **********************************************************************************************************************/
