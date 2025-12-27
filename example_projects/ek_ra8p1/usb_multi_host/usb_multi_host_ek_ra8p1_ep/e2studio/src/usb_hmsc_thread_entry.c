/***********************************************************************************************************************
 * File Name    : usb_hmsc_thread_entry.c
 * Version      : .
 * Description  : Handle USB HMSC (Host MSC) operations.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "usb_hmsc_thread.h"
#include "usb_hcdc_hmsc_app.h"

/**********************************************************************************************************************
 Private variables and functions
 *********************************************************************************************************************/
static void update_buffer(void);
static void usb_safely_eject(void);
static void format_usb_device(void);

static fsp_err_t usb_init(void);
static void process_usb_msc_operation(uint8_t input_buffer);
static void usb_write_operation(void);
static void usb_read_operation(void);
static void fat_clean_up(void);

static FF_Disk_t my_disk;
static uint8_t g_write_data[WRITE_ITEM_SIZE] = {RESET_VALUE}; /* Data(10k) to write to file */
static uint8_t g_read_data[WRITE_ITEM_SIZE]  = {RESET_VALUE}; /* Variable to store the data read from file */
static bool b_usb_hmsc_close = true;


/**********************************************************************************************************************
 Public variables and functions
 *********************************************************************************************************************/
static bool msc_menu_display_enable = false;
extern int8_t g_new_api_key_str[KEY_SIZE_IN_BYTES + 8];

/**********************************************************************************************************************
 * @brief     USB HMSC Thread entry function.
 * @param[in] Pointer to pvParameters.
 * @retval    None.
 *********************************************************************************************************************/
void usb_hmsc_thread_entry(void *pvParameters)
{
    /* Suspend the USB HMSC thread and wait for resume from the USB common thread entry */
    vTaskSuspend(usb_hmsc_thread);

    FSP_PARAMETER_NOT_USED(pvParameters);
    fsp_err_t freertos_fat_error = FSP_SUCCESS;
    int8_t key_index;

    freertos_fat_error = usb_init();
    if (freertos_fat_error != FSP_SUCCESS)
    {
        APP_ERR_PRINT("\r\nError in initializing FreeRTOS+FAT with USB_HMSC\r\n");
        APP_ERR_TRAP(freertos_fat_error);
    }

    while (true)
    {
        /* Display MSC main menu if not already displayed */
        if (msc_menu_display_enable)
        {
            APP_PRINT(USB_HMSC_MAIN_MENU);
            msc_menu_display_enable = false;
        }

        /* Get user input */
        if (APP_GET_USER_INPUT())
        {
            /* Convert the first character of the input to an integer */
            key_index = (int8_t) g_new_api_key_str[INDEX_ZERO] - '0';
            /* Check if the key index is within the valid range */
            if (key_index < MENU_INDEX_MIN || key_index > MENU_INDEX_MAX)
            {
                APP_PRINT("\r\nInvalid input. Provide a valid input\r\n");
            }
            else
            {
                process_usb_msc_operation((uint8_t) key_index);
            }
        }

        /* Check if the USB MSC device is disconnected without using the eject option */
        if ((false == b_usb_hmsc_close) && (false == check_usb_connection(false,true)))
        {
            APP_PRINT("\r\nUSB Device is disconnected without using the Eject option first.\r\n");
            APP_PRINT("\r\nPlease reconnect the USB device and Execute Safely Eject option\r\n"
                      "to make sure file operations works correctly.\r\n");

            /* Wait indefinitely for MSC connection event */
            xEventGroupWaitBits(g_usb_event_group, IS_MSC_DEVICE_CONNECTED,
                                pdFALSE, pdFALSE, portMAX_DELAY);

            /* Re-initialize the mass storage device */
            freertos_fat_error = usb_init();
            if (freertos_fat_error != FSP_SUCCESS)
            {
                APP_ERR_PRINT("\r\nError in initializing FreeRTOS+FAT with USB_HMSC\r\n");
                APP_ERR_TRAP(freertos_fat_error);
            }
        }

        vTaskDelay(1);
    }
} /* End of function usb_hmsc_thread_entry() */

/**********************************************************************************************************************
 * @brief       This function initiates the USB operation by calling respective functions.
 * @param[in]   Converted RTT input.
 * @retval      None.
 *********************************************************************************************************************/
static void process_usb_msc_operation(uint8_t input_buffer)
{
    int32_t file_error = SUCCESS;
    fsp_err_t freertos_fat_error = FSP_SUCCESS;

    switch (input_buffer)
    {
        case USB_MSC_IGNORE:
        {
            /* Skip this operation as it pertains to another USB device */
        }
        break;

        case USB_MSC_FORMAT:
        {
            /* Format the USB device */
            format_usb_device();
        }
        break;

        case USB_MSC_WRITE:
        {
            /* USB write and read operations */
            APP_PRINT("\r\nPlease Wait ...\r\n");
            usb_write_operation();
            usb_read_operation();
        }
        break;

        case USB_MSC_DELETE:
        {
            /* Remove the specified file */
            APP_PRINT("\r\nPlease Wait ...\r\n");
            file_error = ff_remove(FILE_NAME);
            if (file_error != SUCCESS)
            {
                if (stdioGET_ERRNO() == 2)
                {
                    APP_PRINT(GREEN"\r\nNo such file!\r\n"RESET);
                }
            }
            else
            {
                APP_PRINT(GREEN"\r\nDeleted file: %s\r\n"RESET, FILE_NAME);
            }
        }
        break;

        case USB_MSC_SAFELY_EJECT:
        {
            /* Safely eject the USB device */
            usb_safely_eject();

            /* Get user input and process USB operation */
            if (INPUT_STATUS_HAVE_INPUT == APP_GET_USER_INPUT())
            {
                /* Clear event group bits and initialize FreeRTOS+FAT */
                xEventGroupClearBits(g_usb_event_group, ALL_BIT);
                init_freertos_plus_fat();
                freertos_fat_error = usb_init();
                if (freertos_fat_error != FSP_SUCCESS)
                {
                    APP_ERR_PRINT("\r\nError in initializing FreeRTOS+FAT with USB_HMSC\r\n");
                    APP_ERR_TRAP(freertos_fat_error);
                }
                msc_menu_display_enable = true;
            }
        }
        break;
        default:
        {
            APP_PRINT("\r\nInvalid input. Provide a valid input\r\n");
        }
        break;
    }
} /* End of function process_usb_msc_operation() */

/*******************************************************************************************************************
 * @brief       This function performs USB HMSC format operation.
 * @param[in]   None.
 * @retval      None.
 ******************************************************************************************************************/
static void format_usb_device(void)
{
    /* Check the USB device connection before formatting */
    if ((true == check_usb_connection(false,true)) && (true != b_usb_hmsc_close))
    {
        APP_PRINT("\r\nUSB Device Formatting will be initiated. Formatting will take time "
                  "\r\ndepending on USB Device capacity.\r\n");
        APP_PRINT("\r\nDo not disconnect the USB device while formatting is in progress."
                  " \r\nPlease Wait ...\r\n");

        /* Formatting time varies with USB Device capacity. Might take longer time for higher capacity USB Device */
        FF_Error_t ff_error = FF_Format(&my_disk, my_disk.xStatus.bPartitionNumber, pdFALSE, pdFALSE);

        if (FF_ERR_NONE != ff_error)
        {
            if (my_disk.pxIOManager->xPartition.ulNumClusters < MIN_CLUSTERS_FAT16)
            {
                APP_PRINT("\r\nDisk size insufficient for FAT16/FAT32 format. "
                          "\r\nCurrent number of clusters: %d. Minimum number of clusters required: %d.\r\n",
                          my_disk.pxIOManager->xPartition.ulNumClusters, MIN_CLUSTERS_FAT16);
                APP_PRINT(GREEN"\r\nUSB Device formatting unsuccessful.\r\n"RESET);
            }
            else
            {
                APP_ERR_PRINT("\r\n FF_Format API failed  %d. Check the USB Device.\r\n", FF_GetErrMessage(ff_error));
                APP_PRINT(" %d\r\n", FF_GetErrMessage(ff_error));
            }
        }
        else
        {
            APP_PRINT(GREEN"\r\nUSB Device Formatted successfully \r\n"RESET);
        }
    }
    else
    {
        APP_PRINT(GREEN"USB Device disconnected or not initialized after Eject command\n"RESET);
    }
} /* End of function format_usb_device() */

/*******************************************************************************************************************
 * @brief       This function closes the FreeRTOS+FAT instance.
 * @param[in]   None.
 * @retval      None.
 *******************************************************************************************************************/
static void fat_clean_up(void)
{
    fsp_err_t freertos_fat_error = FSP_SUCCESS;

    /* Close the FREERTOS_PLUS_FAT_Close instance on any failure */
    freertos_fat_error = RM_FREERTOS_PLUS_FAT_Close(&g_rm_freertos_plus_fat0_ctrl);
    if (FSP_SUCCESS != freertos_fat_error)
    {
        APP_PRINT("\r\nRM_FREERTOS_PLUS_FAT_Close API failed\r\n");
    }
    else
    {
        APP_PRINT("\r\nFREERTOS PLUS FAT instance Closed successfully.\r\n");
    }
} /* End of function fat_clean_up() */

/**********************************************************************************************************************
 * @brief       This function performs USB HMSC write operation.
 * @param[in]   None.
 * @retval      None.
 *********************************************************************************************************************/
static void usb_write_operation(void)
{
    FF_FILE *file_pointer = NULL;
    FF_Stat_t file_details;
    int32_t file_error = SUCCESS;
    /* Capture the number of bytes written in the variable to check write status */
    size_t bytes_written = RESET_VALUE;

    memset(&file_details, RESET_VALUE, sizeof(file_details));

    /* Double check the connection again to ensure the USB device is still mounted */
    if ((true == check_usb_connection(false,true)) && (true != b_usb_hmsc_close))
    {
        /* Once connection is detected open file is write mode */
        file_pointer = ff_fopen((const char*) FILE_NAME, WRITE_MODE);
        if (NULL != file_pointer)
        {
            /* Fill write buffer with data */
            update_buffer();

            APP_PRINT("\r\nUSB write operation will be initiated.");

            /* Write API writes */
            bytes_written = ff_fwrite(g_write_data, sizeof(g_write_data[RESET_VALUE]),
                                      WRITE_ITEM_SIZE,file_pointer);

            if (WRITE_ITEM_SIZE != bytes_written)
            {
                APP_ERR_PRINT("ff_fwrite API failed. Closing opened file.\r\n");
                /* Adding extra %d as parses string and prints %d as-is */
                APP_PRINT(" %d\r\n", stdioGET_ERRNO());
                file_error = ff_fclose(file_pointer);
                if (SUCCESS != file_error)
                {
                    APP_ERR_PRINT("ff_fclose API failed");
                    /* Adding extra %d as parses string and prints %d as-is */
                    APP_PRINT(" %d\r\n", stdioGET_ERRNO());
                }
                return;
            }

            /* Close the file after write operation and open again in read mode */
            file_error = ff_fclose(file_pointer);
            if (SUCCESS != file_error)
            {
                APP_ERR_PRINT("ff_fclose API failed");
                /* Adding extra %d as parses string and prints %d as-is */
                APP_PRINT(" %d\r\n", stdioGET_ERRNO());
                return;
            }

            file_error = ff_stat((const char*) FILE_NAME, &file_details);
            /* ff_stat returns 0 on success and -1 on error */
            if (SUCCESS == file_error)
            {
                /* Compare the actual bytes written and file size after write operation */
                if (bytes_written == file_details.st_size)
                {
                    APP_PRINT("\r\n%d bytes Data successfully written to file %s\r\n", bytes_written, FILE_NAME);
                    APP_PRINT("Write operation is Successful \r\n");
                }
                else
                {
                    APP_ERR_PRINT("ff_write API failed ");
                    /* Adding extra %d as parses string and prints %d as-is */
                    APP_PRINT(" %d\r\n", stdioGET_ERRNO());
                    return;
                }
            }
            else
            {
                APP_ERR_PRINT("ff_stat API failed");
                /* Adding extra %d as parses string and prints %d as-is */
                APP_PRINT(" %d\r\n", stdioGET_ERRNO());
                return;
            }
        }
        else
        {
            APP_ERR_PRINT("ff_fopen API failed");
            /* Adding extra %d as parses string and prints %d as-is */
            APP_PRINT(" %d\r\n", stdioGET_ERRNO());
            return;
        }
    }
    else
    {
        APP_PRINT(GREEN"USB Device disconnected or not initialized after Eject command\n"RESET);
    }
} /* End of function usb_write_operation() */

/**********************************************************************************************************************
 * @brief       This function performs USB HMSC read operation.
 * @param[in]   None.
 * @retval      None.
 *********************************************************************************************************************/
void usb_read_operation(void)
{
    FF_FILE *file_pointer = NULL;
    FF_Stat_t file_details;
    int32_t file_error = SUCCESS;

    memset(&file_details, RESET_VALUE, sizeof(file_details));

    /* Double check the connection again to ensure the USB device is still mounted */
    if ((true == check_usb_connection(false,true)) && (true != b_usb_hmsc_close))
    {
        /* Open the file read mode to read data written previously */
        file_pointer = ff_fopen((const char *) FILE_NAME, READ_MODE);
        if (file_pointer != NULL)
        {
            APP_PRINT("\r\nUSB read operation will be initiated");

            /* Capture the number of bytes read in the variable to check read status */
            size_t bytes_read = RESET_VALUE;
            bytes_read = ff_fread(g_read_data , sizeof(g_read_data[RESET_VALUE]) , WRITE_ITEM_SIZE , file_pointer);
            if (READ_WRITE_FAILURE == bytes_read)
            {
                APP_ERR_PRINT(" ff_read API failed. Closing opened file \r\n", stdioGET_ERRNO());
                file_error = ff_fclose(file_pointer);
                if(SUCCESS != file_error )
                {
                    APP_ERR_PRINT("ff_fclose API failed");
                    /* Adding extra %d as parses string and prints %d as-is */
                    APP_PRINT(" %d\r\n", stdioGET_ERRNO());
                    return;
                }
                return;
            }

            file_error = ff_fclose(file_pointer);
            if(SUCCESS != file_error)
            {
                APP_ERR_PRINT("ff_fclose API failed");
                /* Adding extra %d as parses string and prints %d as-is */
                APP_PRINT(" %d\r\n", stdioGET_ERRNO());
                return;
            }

            file_error = ff_stat((const char*)FILE_NAME, &file_details);
            /* ff_stat returns 0 on success and -1 on error */
            if (SUCCESS == file_error)
            {
                /* Compare the actual bytes written and file size after write operation */
                if (bytes_read == file_details.st_size)
                {
                    APP_PRINT("\r\n%d bytes Data successfully read from file  %s \r\n", bytes_read, FILE_NAME);
                    APP_PRINT("Read operation is Successful \r\n");
                }
                else
                {
                    APP_ERR_PRINT("ff_write API failed ");
                    /* Adding extra %d as parses string and prints %d as-is */
                    APP_PRINT(" %d\r\n", stdioGET_ERRNO());
                    return;
                }

                /* Compare Write and Read data */
                if(SUCCESS == memcmp(g_write_data, g_read_data, WRITE_ITEM_SIZE))
                {
                    APP_PRINT(GREEN"\r\nWrite and Read data is same\r\n"RESET);
                }
                else
                {
                    APP_ERR_PRINT(GREEN"\r\nWrite and Read data did not match\r\n"RESET);
                }
            }
            else
            {
                APP_ERR_PRINT("ff_stat API failed");
                /* Adding extra %d as parses string and prints %d as-is */
                APP_PRINT(" %d\r\n", stdioGET_ERRNO());
                return;
            }
        }
        else
        {
            APP_ERR_PRINT("ff_fopen API failed");
            /* Adding extra %d as parses string and prints %d as-is */
            APP_PRINT(" %d\r\n", stdioGET_ERRNO());
            return;
        }
    }
    else
    {
        APP_PRINT(GREEN"USB Device disconnected or not initialized after Eject command\n"RESET);
    }
} /* End of function usb_read_operation() */

/**********************************************************************************************************************
 * @brief       This function closes USB HMSC on FreeRTOS+FAT and safely ejects.
 * @param[in]   None.
 * @retval      None.
 *********************************************************************************************************************/
static void usb_safely_eject(void)
{
    /* Check the USB device connection before de-initializing */
    if ((true == check_usb_connection(false,true)) && (true != b_usb_hmsc_close))
    {
        fsp_err_t freertos_fat_error = RM_FREERTOS_PLUS_FAT_DiskDeinit(&g_rm_freertos_plus_fat0_ctrl, &my_disk);
        if (FSP_SUCCESS != freertos_fat_error)
        {
            APP_ERR_PRINT("\r\nRM_FREERTOS_PLUS_FAT_DiskDeinit API failed\r\n");
            APP_ERR_TRAP(freertos_fat_error);
        }

        /* Close the FREERTOS_PLUS_FAT_Close instance on safely ejecting */
        freertos_fat_error = RM_FREERTOS_PLUS_FAT_Close(&g_rm_freertos_plus_fat0_ctrl);
        if (FSP_SUCCESS != freertos_fat_error)
        {
            APP_ERR_PRINT("\r\nRM_FREERTOS_PLUS_FAT_Close API failed\r\n");
            APP_ERR_TRAP(freertos_fat_error);
        }

        APP_PRINT("\r\nUSB Device can be safely removed now or Press any key to re-initialize\r\n");

        /* Update the flag */
        b_usb_hmsc_close = true;
    }
    else
    {
        /* USB Device disconnected */
        APP_PRINT(GREEN"USB Device disconnected or not initialized after Eject command\n"RESET);
    }
} /* End of function usb_safely_eject() */

/**********************************************************************************************************************
 * @brief       This function Initializes the FreeRTOS+FAT instance.
 * @param[in]   None.
 * @retval      Any Other Error code apart from FSP_SUCCESS on Unsuccessful operation.
 *********************************************************************************************************************/
static fsp_err_t usb_init(void)
{
    fsp_err_t freertos_fat_error = FSP_SUCCESS;
    int32_t file_error = SUCCESS;
    rm_freertos_plus_fat_device_t device;

    memset(&device, RESET_VALUE, sizeof(device));
    memset(&my_disk, RESET_VALUE, sizeof(my_disk));

    /* Wait until USB Device is connected */
    while (true != check_usb_connection(false,true))
    {
        vTaskDelay(1);
    }

    /* Initialize the mass storage device. This should not be done until the device is plugged in and initialized */
    freertos_fat_error = RM_FREERTOS_PLUS_FAT_MediaInit(&g_rm_freertos_plus_fat0_ctrl, &device);
    if (FSP_SUCCESS != freertos_fat_error)
    {
        APP_ERR_PRINT("\r\nRM_FREERTOS_PLUS_FAT_MediaInit API failed\r\n");
        fat_clean_up();

        return freertos_fat_error;
    }

    /* Initialize one disk for each partition used in the application */
    freertos_fat_error = RM_FREERTOS_PLUS_FAT_DiskInit(&g_rm_freertos_plus_fat0_ctrl,
                                                       &g_rm_freertos_plus_fat0_disk_cfg, &my_disk);
    if (FSP_SUCCESS != freertos_fat_error)
    {
        APP_ERR_PRINT("\r\nRM_FREERTOS_PLUS_FAT_DiskInit API failed\r\n");
        fat_clean_up();

        return freertos_fat_error;
    }

    /* Mount each disk. This assumes the disk is already partitioned and formatted */
    FF_Error_t ff_err = FF_Mount(&my_disk, my_disk.xStatus.bPartitionNumber);
    if (FSP_SUCCESS != ff_err)
    {
        APP_ERR_PRINT("\r\nFF_Mount API failed\r\n");

        /* Close the FREERTOS_PLUS_FAT_Close instance on safely ejecting */
        fat_clean_up();

        /* As function returns fsp_err_t, ff_err cannot be returned. Hence trapping the error here */
        APP_ERR_TRAP(ff_err);
    }

    /* Add the disk to the file system */
    file_error = FF_FS_Add("/", &my_disk);
    if (SUCCESS == file_error)
    {
        APP_ERR_PRINT("\r\nFF_Mount API failed\r\n");

        /* Close the FREERTOS_PLUS_FAT_Close instance on safely ejecting */
        fat_clean_up();

        /* As function returns fsp_err_t, ff_err cannot be returned. Hence trapping the error here */
        APP_ERR_TRAP(file_error);
    }

    /* Set this flag to let application know that USB is initialized */
    b_usb_hmsc_close = false;

    return freertos_fat_error;
} /* End of function usb_init() */


/**********************************************************************************************************************
 * @brief       This function updates write buffer with data and clears read buffer.
 * @param[in]   None.
 * @retval      None.
 *********************************************************************************************************************/
static void update_buffer(void)
{
    for (uint16_t i = RESET_VALUE; i < WRITE_ITEM_SIZE; i++)
    {
        g_write_data[i] = ASCII_CHAR_A;
        g_read_data[i] = RESET_VALUE;
    }
} /* End of function update_buffer() */

/**********************************************************************************************************************
 * @brief       This function Initializes the FreeRTOS+FAT instance.
 * @param[in]   None.
 * @retval      Any other error code apart from FSP_SUCCESS on Unsuccessful operation.
 *********************************************************************************************************************/
void init_freertos_plus_fat(void)
{
    fsp_err_t freertos_fat_error = FSP_SUCCESS;

    /* Open FreeRTOS PLUS FAT */
    freertos_fat_error = RM_FREERTOS_PLUS_FAT_Open(&g_rm_freertos_plus_fat0_ctrl, &g_rm_freertos_plus_fat0_cfg);
    if (FSP_SUCCESS != freertos_fat_error)
    {
        APP_ERR_PRINT("\r\nRM_FREERTOS_PLUS_FAT_Open API failed\r\n");
        handle_error(freertos_fat_error, "\r\nInitialize the media device failed\r\n");
    }

    APP_PRINT("\r\n\nFreeRTOS+FAT Open successful\r\n");

    /* Wait for USB Device connection */
    APP_PRINT("\r\nConnect USB Device...\r\n");

} /* End of function init_freertos_plus_fat() */


/**********************************************************************************************************************
 * @brief       This function is callback for FreeRTOS+FAT and triggered when USB device is removed or inserted.
 * @param[in]   p_args    Parameter data.
 * @retval      None.
 *********************************************************************************************************************/
void free_rtos_fat_callback(rm_freertos_plus_fat_callback_args_t *p_args)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE, xResult;

    if (p_args->event & RM_FREERTOS_PLUS_FAT_EVENT_MEDIA_INSERTED)
    {
        msc_menu_display_enable = true;
        xResult = xEventGroupSetBitsFromISR(g_usb_event_group, IS_MSC_DEVICE_CONNECTED, &xHigherPriorityTaskWoken);
        if (xResult != pdFAIL)
        {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }

    if (p_args->event & RM_FREERTOS_PLUS_FAT_EVENT_MEDIA_REMOVED)
    {
        msc_menu_display_enable = false;
        xEventGroupClearBits(g_usb_event_group, ALL_BIT & ~IS_CDC_DEVICE_CONNECTED);
    }

} /* End of function free_rtos_fat_callback() */

/*******************************************************************************************************************//**
 * @brief     This function is called to close the USB module handle error trapping.
 * @param[in] err       Return values of APIs.
 * @param[in] err_str   Print message from the failed API call.
 * @retval    None.
 **********************************************************************************************************************/
void handle_error(fsp_err_t err, char *err_str)
{
        APP_ERR_PRINT(err_str);
        fat_clean_up();
        APP_ERR_TRAP(err);

} /* End of function handle_error() */

/*******************************************************************************************************************
 * End of file
 ******************************************************************************************************************/
