/***********************************************************************************************************************
 * File Name    : usb_multiport.c
 * Description  : Contains multiple functions definitions used in EP
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "board_mon_thread.h"
#include "common_init.h"
#include "common_data.h"
#include "common_utils.h"
#include "usb_console_main.h"
#include "board_cfg.h"
#include "usb_multiport.h"

/*******************************************************************************************************************//**
 * @addtogroup usb_multiport_ep
 * @{
 **********************************************************************************************************************/

#define CONNECTION_ABORT_CRTL    (0x00)
#define MENU_ENTER_RESPONSE_CRTL (0x09)
#define MENU_EXIT_CRTL           (0x20)

/* Global variable */
char_t g_write_data[USB_IO_BUFFER] = {RESET_VALUE};          /* Data to write to file */
char_t g_read_data[USB_IO_BUFFER]  = {RESET_VALUE};          /* Variable to store the data read from file */
size_t g_bytes_to_write = RESET_VALUE;                       /* variable to count bytes to write on file */
size_t g_bytes_read = RESET_VALUE;                           /* variable to count bytes to read from file */

FF_Disk_t g_my_disk;                                         /* USB disk representation */

char_t g_read_fname[32]  = READ_FILE_NAME;
char_t g_write_fname[32] = WRITE_FILE_NAME;

bool_t g_usb_msc_rd_detail_en = false;

static bool s_b_write_to_usb = false;                        /* Flag to check write status */
static char_t s_print_buffer[BUFFER_LINE_LENGTH/4] = {};

/* Function Name: file_system_and_usb_init */
/*******************************************************************************************************************//**
 * @brief       This function Initializes the FreeRTOS+FAT instance.
 * @param[IN]   None
 * @retval      Any Other Error code apart from FSP_SUCCESS on Unsuccessful operation.
 **********************************************************************************************************************/
fsp_err_t file_system_and_usb_init(void)
{
    fsp_err_t freertos_fat_error = FSP_SUCCESS;
    int32_t file_error = SUCCESS;
    rm_freertos_plus_fat_device_t device;
    int8_t c = -1;

    memset (&device, RESET_VALUE, sizeof(device));

    /* Open FreeRTOS PLUS FAT  */
    freertos_fat_error = RM_FREERTOS_PLUS_FAT_Open (&g_rm_freertos_plus_fat_ctrl, &g_rm_freertos_plus_fat_cfg);
    if (FSP_SUCCESS != freertos_fat_error)
    {
        /* ignoring -Wpointer-sign is OK for a constant string */
        print_to_console((uint8_t *)"\r\nFREERTOS PLUS FAT OPEN API failed.\r\n");
        return freertos_fat_error;
    }

    /* ignoring -Wpointer-sign is OK for a constant string */
    print_to_console((uint8_t *)"\r\n");

    /* Wait until USB Device is connected.This is the blocking call*/
    while (true != wait_till_usb_detected ())
    {
        c = input_from_console ();
        if ((MENU_EXIT_CRTL == c) || (CONNECTION_ABORT_CRTL == c))
        {
            /* ignoring -Wpointer-sign is OK for a constant string */
            print_to_console((uint8_t *)"ABORTING OPERATION\r\n\r\n");
            freertos_fat_error = FSP_ERR_NOT_INITIALIZED;
            return freertos_fat_error;
        }
    }

    /* Initialize the mass storage device.  This should not be done until the device is plugged in and initialized. */
    freertos_fat_error = RM_FREERTOS_PLUS_FAT_MediaInit (&g_rm_freertos_plus_fat_ctrl, &device);
    if (FSP_SUCCESS != freertos_fat_error)
    {
        /* ignoring -Wpointer-sign is OK for a constant string */
        print_to_console((uint8_t *)"\r\nFreeRTOS Plus FAT Media Init API failed.\r\n");
        clean_up ();
        return freertos_fat_error;
    }

    /* Initialize one disk for each partition used in the application. */
    freertos_fat_error = RM_FREERTOS_PLUS_FAT_DiskInit (&g_rm_freertos_plus_fat_ctrl, &g_rm_freertos_plus_fat_disk_cfg,
                                                        &g_my_disk);
    if (FSP_SUCCESS != freertos_fat_error)
    {
        /* ignoring -Wpointer-sign is OK for a constant string */
        print_to_console((uint8_t *)"\r\nFreeRTOS Plus FAT Disk Init API failed.\r\n");
        clean_up ();
        return freertos_fat_error;
    }

    /* Mount each disk.  This assumes the disk is already partitioned and formatted. */
    FF_Error_t ff_err = FF_Mount (&g_my_disk, g_my_disk.xStatus.bPartitionNumber);
    if (FSP_SUCCESS != ff_err)
    {
        /* ignoring -Wpointer-sign is OK for a constant string */
        print_to_console((uint8_t *)"\r\nFF_Mount API failed.\r\n");

        /* Close the FREERTOS_PLUS_FAT_Close instance on safely ejecting */
        clean_up ();

        /* As function returns fsp_err_t, ff_err cannot be returned. Hence trapping the error here */
        APP_ERR_TRAP(ff_err);
    }

    /* Add the disk to the file system. */
    file_error = FF_FS_Add ("/", &g_my_disk);
    if (SUCCESS == file_error)
    {
        /* ignoring -Wpointer-sign is OK for a constant string */
        print_to_console((uint8_t *)"\r\nFF_Mount API failed.\r\n");

        /* Close the FREERTOS_PLUS_FAT_Close instance on safely ejecting */
        clean_up ();

        /* As function returns fsp_err_t, ff_err cannot be returned. Hence trapping the error here */
        APP_ERR_TRAP(file_error);
    }

    /* ignoring -Wpointer-sign is OK for a constant string */
    print_to_console((uint8_t *)" USB flash drive mounted successfully\r\n");

    return freertos_fat_error;
}
/**********************************************************************************************************************
 End of function file_system_and_usb_init
 *********************************************************************************************************************/

/* Function Name: usb_read_operation */
/*******************************************************************************************************************//**
 * @brief       This function performs USB HMSC read operation.
 * @param[IN]   None
 * @retval      Error code passed up from lower layer
 **********************************************************************************************************************/
int32_t usb_read_operation(void)
{
    FF_FILE * p_file_pointer = NULL;
    FF_Stat_t file_details;
    int32_t file_error = SUCCESS;

    char_t * p_name = &g_read_fname[1];

    /* Resetting the variables */
    memset(&file_details, RESET_VALUE, sizeof(file_details));
    memset(g_read_data, RESET_VALUE, sizeof(g_read_data));

    /* cast to const char * is required on unspecified definition */
    file_error = ff_stat ((const char_t *)g_read_fname, &file_details);

    /* ff_stat returns 0 on success and -1 on error */
    if (SUCCESS != file_error)
    {
        /* ignoring -Wpointer-sign is OK for a constant string */
        print_to_console((uint8_t *)" File not found\r\n");

        sprintf (s_print_buffer, " Confirm file %s is located on the USB flash drive\r\n", p_name);

        /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
        print_to_console((void*)s_print_buffer);

        return (file_error);
    }

    if (true == g_usb_msc_rd_detail_en)
    {
        /* ignoring -Wpointer-sign is OK for a constant string */
        print_to_console((uint8_t *)" File found\r\n\r\n Checking the file size...\r\n");

        /* Compare the actual bytes written and file size after write operation */
        if (WRITE_ITEM_SIZE >= file_details.st_size)
        {
            if (0 >= file_details.st_size)
            {
                if (true == g_usb_msc_rd_detail_en)
                {
                    /* ignoring -Wpointer-sign is OK for a constant string */
                    print_to_console((uint8_t *)" File size needs to be greater than 0 Bytes to encrypt\r\n");
                }
                file_error = SIZE;
                return (file_error);
            }

            if (true == g_usb_msc_rd_detail_en)
            {
                /* ignoring -Wpointer-sign is OK for a constant string */
                print_to_console((uint8_t *)" File size less than 1 KB\r\n");
            }
        }
        else
        {
            sprintf (s_print_buffer, " file %s is too big to process as it contains %lu bytes of data\r\n",
                        p_name, file_details.st_size);

            /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
            print_to_console((void*)s_print_buffer);

            file_error = SIZE;
            return (file_error);
        }
    }


    /* Double check the connection again to ensure the USB device is still mounted */
    if (true == check_usb_connected())
    {
        /* Open the file read mode to read data written previously */
        /* cast to const char * is required on unspecified definition */
        p_file_pointer = ff_fopen ((const char *)READ_FILE_NAME, READ_MODE);

        if (NULL != p_file_pointer)
        {
            if (true == g_usb_msc_rd_detail_en)
            {
                sprintf (s_print_buffer, "\r\n Reading data in \"%s\"......\r\n", p_name);

                /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
                print_to_console((void*)s_print_buffer);
            }

            /* Capture the number of bytes read in the variable to check read status. */
            size_t bytes_read = RESET_VALUE;
            g_bytes_read = RESET_VALUE;

            while (!ff_feof(p_file_pointer))
            {
                /* Read data from file */
                bytes_read = ff_fread (g_read_data, sizeof(g_read_data[RESET_VALUE]), USB_IO_BUFFER, p_file_pointer);
                if (READ_WRITE_FAILURE == bytes_read)
                {
                    sprintf (s_print_buffer, " ff_read API failed. Closing opened file %d\r\n", stdioGET_ERRNO());

                    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
                    print_to_console((void*)s_print_buffer);

                    file_error = ff_fclose (p_file_pointer);
                    if (SUCCESS != file_error)
                    {
                        /* ignoring -Wpointer-sign is OK for a constant string */
                        print_to_console((uint8_t *)"ff_fclose API failed.");

                        /* Adding extra %d as parses string and prints %d as-is. */
                        sprintf (s_print_buffer, " %d\r\n",  stdioGET_ERRNO());

                        /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
                        print_to_console((void*)s_print_buffer);

                    }
                    return (file_error);
                }
                g_bytes_read = g_bytes_read + bytes_read;
            }

            /* Close the file after read */
            file_error = ff_fclose (p_file_pointer);
            if (SUCCESS != file_error)
            {
                /* ignoring -Wpointer-sign is OK for a constant string */
                print_to_console((uint8_t *)"ff_fclose API failed.");

                /* Adding extra %d as parses string and prints %d as-is. */
                sprintf (s_print_buffer, " %d\r\n",  stdioGET_ERRNO());

                /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
                print_to_console((void*)s_print_buffer);

                return (file_error);
            }

            /* cast to const char * is required on unspecified definition */
            file_error = ff_stat ((const char*)READ_FILE_NAME, &file_details);

            /* ff_stat returns 0 on success and -1 on error */
            if (SUCCESS == file_error)
            {
                /* Compare the actual bytes written and file size after write operation */
                if (g_bytes_read != file_details.st_size)
                {
                    /* ignoring -Wpointer-sign is OK for a constant string */
                    print_to_console((uint8_t *)"ff_write API failed.");

                    /* Adding extra %d as parses string and prints %d as-is. */
                    sprintf (s_print_buffer, " %d\r\n",  stdioGET_ERRNO());

                    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
                    print_to_console((void*)s_print_buffer);
                    return (file_error);
                }
            }
            else
            {
                /* ignoring -Wpointer-sign is OK for a constant string */
                print_to_console((uint8_t *)"ff_stat API failed.");

                /* Adding extra %d as parses string and prints %d as-is. */
                sprintf (s_print_buffer, " %d\r\n",  stdioGET_ERRNO());

                /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
                print_to_console((void*)s_print_buffer);
                return (file_error);
            }
        }
        else
        {
            /* ignoring -Wpointer-sign is OK for a constant string */
            print_to_console((uint8_t *)"ff_fopen API failed.");

            /* Adding extra %d as parses string and prints %d as-is. */
            sprintf (s_print_buffer, " %d\r\n",  stdioGET_ERRNO());

            /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
            print_to_console((void*)s_print_buffer);
            return (file_error);
        }
    }
    else
    {
        /* ignoring -Wpointer-sign is OK for a constant string */
        print_to_console((uint8_t *)"USB not detected.\n");
    }

    return (file_error);
}
/**********************************************************************************************************************
 End of function usb_read_operation
 *********************************************************************************************************************/

/* Function Name: usb_write_operation */
/*******************************************************************************************************************//**
 * @brief       This function performs USB HMSC write operation.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
void usb_write_operation(void)
{
    FF_FILE * p_file_pointer = NULL;
    FF_Stat_t file_details;
    int32_t file_error = SUCCESS;

    /* Capture the number of bytes written in the variable to check write status. */
    size_t bytes_written = RESET_VALUE;

    memset(&file_details, RESET_VALUE, sizeof(file_details));

    /* Double check the connection again to ensure the USB device is still mounted */
    if (true == check_usb_connected())
    {
        /* Once connection is detected open file is write mode */
        p_file_pointer = ff_fopen ((const char *)g_write_fname, WRITE_MODE);
        if (NULL != p_file_pointer)
        {
            /* Write data to file  */
            g_bytes_to_write = (strlen((const char *)&g_write_data[0]));

            bytes_written = ff_fwrite (g_write_data , sizeof(g_write_data[RESET_VALUE]) ,
                                        g_bytes_to_write , p_file_pointer);
            if (RESET_VALUE != bytes_written)
            {
                g_bytes_to_write = RESET_VALUE;
            }
            else
            {
                /* ignoring -Wpointer-sign is OK for a constant string */
                print_to_console((uint8_t *)" ff_write API failed. Closing opened file.\r\n");

                /* Adding extra %d as parses string and prints %d as-is. */
                sprintf (s_print_buffer, " %d\r\n",  stdioGET_ERRNO());

                /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
                print_to_console((void*)s_print_buffer);
                file_error = ff_fclose (p_file_pointer);
                if (SUCCESS != file_error)
                {
                    /* ignoring -Wpointer-sign is OK for a constant string */
                    print_to_console((uint8_t *)"ff_fclose API failed.");

                    /* Adding extra %d as parses string and prints %d as-is. */
                    sprintf (s_print_buffer, " %d\r\n",  stdioGET_ERRNO());

                    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
                    print_to_console((void*)s_print_buffer);
                }

                /* clear the flag */
                s_b_write_to_usb = false;
                return;
            }

            /* Close the file after write operation and open again in read mode */
            file_error = ff_fclose (p_file_pointer);
            if (SUCCESS != file_error)
            {
                /* ignoring -Wpointer-sign is OK for a constant string */
                print_to_console((uint8_t *)"ff_fclose API failed.");

                /* Adding extra %d as parses string and prints %d as-is. */
                sprintf (s_print_buffer, " %d\r\n",  stdioGET_ERRNO());

                /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
                print_to_console((void*)s_print_buffer);
                return;
            }
        }
        else
        {
            /* ignoring -Wpointer-sign is OK for a constant string */
            print_to_console((uint8_t *)"ff_fopen API failed.");

            /* Adding extra %d as parses string and prints %d as-is. */
            sprintf (s_print_buffer, " %d\r\n",  stdioGET_ERRNO());

            /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
            print_to_console((void*)s_print_buffer);

            return;
        }
    }
    else
    {
        /* ignoring -Wpointer-sign is OK for a constant string */
        print_to_console((uint8_t *)"USB not detected.\r\n");
    }
}
/**********************************************************************************************************************
 End of function usb_write_operation
 *********************************************************************************************************************/

/* Function Name: wait_till_usb_detected */
/*******************************************************************************************************************//**
 * @brief       This function checks status and waits till USB is detected.
 * @param[IN]   None
 * @retval      true if connection detected; false if connection is not detected.
 **********************************************************************************************************************/
bool wait_till_usb_detected(void)
{
    EventBits_t uxBits;
    bool status = false;
    uxBits = xEventGroupWaitBits (g_event_group, MEDIA_INSERTED | MEDIA_REMOVED, pdFALSE, pdTRUE,
                                    (TickType_t )WAIT_TIME / portTICK_PERIOD_MS);
    if (uxBits & MEDIA_INSERTED)
    {
        status = true;
    }
    else if (uxBits & MEDIA_REMOVED)
    {
        status = false;
    }
    else
    {
        ; /* none */
    }
    return status;
}
/**********************************************************************************************************************
 End of function wait_till_usb_detected
 *********************************************************************************************************************/

/* Function Name: check_usb_connected */
/*******************************************************************************************************************//**
 * @brief       This function checks the USB HMSC connection status.
 * @param[IN]   None
 * @retval      true if connection detected; false if connection is not detected.
 **********************************************************************************************************************/
bool check_usb_connected(void)
{
    EventBits_t uxBits;
    bool status = false;
    uxBits = xEventGroupGetBits(g_event_group);

    if (uxBits & MEDIA_INSERTED)
    {
        status = true;
    }
    else if (uxBits & MEDIA_REMOVED)
    {
        status = false;
    }
    else
    {
        ; /* none */
    }

    return (status);
}
/**********************************************************************************************************************
 End of function check_usb_connected
 *********************************************************************************************************************/

/* Function Name: free_rtos_fat_callback */
/*******************************************************************************************************************//**
 * @brief       This function is callback for FreeRTOS+FAT and triggered when USB Pen drive is removed or inserted.
 * @param[IN]   pointer to p_args
 * @retval      None.
 **********************************************************************************************************************/
void free_rtos_fat_callback(rm_freertos_plus_fat_callback_args_t *p_args)
{
    xEventGroupSetBits (g_event_group, p_args->event);
}
/**********************************************************************************************************************
 End of function free_rtos_fat_callback
 *********************************************************************************************************************/

/* Function Name: clean_up */
/*******************************************************************************************************************//**
 * @brief       This function closes the FreeRTOS+FAT instance..
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
void clean_up(void)
{
    fsp_err_t freertos_fat_error = FSP_SUCCESS;

    if (0 != g_rm_freertos_plus_fat_ctrl.open)
    {
        /* Close the FREERTOS_PLUS_FAT_Close instance on any failure */
        freertos_fat_error = RM_FREERTOS_PLUS_FAT_Close (&g_rm_freertos_plus_fat_ctrl);

        if (FSP_SUCCESS != freertos_fat_error)
        {
            /* ignoring -Wpointer-sign is OK for a constant string */
            print_to_console((uint8_t *)"\r\nFREERTOS PLUS FAT CLOSE API failed.\r\n");
        }
    }
}
/**********************************************************************************************************************
 End of function clean_up
 *********************************************************************************************************************/
