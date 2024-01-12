/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <stdint.h>
#include <stddef.h>

#include "common_init.h"
#include "usb_multiport.h"
#include "tinycrypt/aes.h"
#include "tinycrypt/constants.h"
#include "usb_hmsc_thread.h"
#include "common_utils.h"


#define NUM_OF_NIST_KEYS    (16)
#define NUM_OF_FIXED_KEYS   (128)

void fat_clean_up ();
void format_usb_device ();
fsp_err_t usb_init ();
void usb_safely_eject ();

struct kat_table {
    uint8_t in[NUM_OF_NIST_KEYS];
    uint8_t out[NUM_OF_NIST_KEYS];
};


/*******************************************************************************************************************//**
 * @addtogroup usb_hmsc_ep
 * @{
 **********************************************************************************************************************/
static char_t s_print_buffer[BUFFER_LINE_LENGTH] = {};

/* Global Variables */
uint8_t g_write_data[USB_IO_BUFFER] = {RESET_VALUE};   /* Data(10k) to write to file */
uint8_t g_read_data[USB_IO_BUFFER]  = {RESET_VALUE};    /* Variable to store the data read from file */

size_t g_bytes_to_write = RESET_VALUE;                   /* variable to count bytes to write on file */
size_t g_bytes_read     = RESET_VALUE;                       /* variable to count bytes to read from file */

FF_Disk_t g_my_disk;                                     /* Pointer to store FreeRTOS+FAT disk structure */

/* Variable to check the Safely Eject option */
static bool b_usb_hmsc_close = false;

static volatile bool g_rm_freertos_plus_fat_insertion_events = false;

char_t g_read_fname[32]  = READ_FILE_NAME;
char_t g_write_fname[32] = WRITE_FILE_NAME;

static bool s_b_write_to_usb  = false;                        /* Flag to check write status */
bool_t g_usb_msc_rd_detail_en = false;

bool check_usb_connection(void);

/* Function Name: wait_till_usb_detected */
/*******************************************************************************************************************//**
 * @brief       This function checks status and waits till USB is detected.
 * @param[IN]   None
 * @retval      true if connection detected; false if connection is not detected.
 **********************************************************************************************************************/
bool wait_till_usb_detected(void)
{
    bool status = false;
    status = check_usb_connection();

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
    bool status = false;
    status = check_usb_connection();
    return (status);
}
/**********************************************************************************************************************
 End of function check_usb_connected
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
            if (0 == g_bytes_to_write)
            {
                g_bytes_to_write = (strlen((const char *)&g_write_data[0]));
            }

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

/*******************************************************************************************************************//**
 * @brief     This function performs USB HMSC format operation.
 * @param[IN]   None
 * @retval      None
 ***********************************************************************************************************************/
/**********************************************************************************************************************
 * Function Name: format_usb_device
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void format_usb_device(void)
{
    if ((true == check_usb_connection()) && (true != b_usb_hmsc_close))
    {
        print_to_console((uint8_t *)"\r\n USB Device Formatting will be initiated. Formatting will take time "
                "depending on USB Device capacity.\r\n");
        print_to_console((uint8_t *)" Do not disconnect the USB device while formatting is in progress.   "\
                "Please Wait ...\r\n");

        /* Formatting time varies with USB Device capacity. Might take longer time for higher capacity USB Device */
        FF_Error_t ff_error = FF_Format (&g_my_disk, g_my_disk.xStatus.bPartitionNumber, pdFALSE , pdFALSE);
        if (FF_ERR_NONE != ff_error)
        {
            sprintf (s_print_buffer, " %s\r\n",  FF_GetErrMessage(ff_error));
            print_to_console((uint8_t *)s_print_buffer);
        }
        else
        {
            print_to_console((uint8_t *)"\r\nUSB Device Formatted successfully \r\n");
        }
    }
    else
    {
        print_to_console((uint8_t *)"USB Device disconnected or not initialized after Eject command\n");
    }
}
/**********************************************************************************************************************
 End of function format_usb_device
 *********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief     This function closes USB HMSC on FreeRTOS+FAT and safely ejects.
 * @param[IN]   None
 * @retval      None
 ***********************************************************************************************************************/
/**********************************************************************************************************************
 * Function Name: usb_safely_eject
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void usb_safely_eject(void)
{
    /* Check the USB Device Connection before formating */
    if ((true == check_usb_connection()) && (true != b_usb_hmsc_close))
    {
        fsp_err_t freertos_fat_error = RM_FREERTOS_PLUS_FAT_DiskDeinit(&g_rm_freertos_plus_fat_ctrl, &g_my_disk);
        if (FSP_SUCCESS != freertos_fat_error)
        {
            print_to_console((uint8_t *)"\r\nFREERTOS PLUS FAT DiskDeinit API failed\r\n");
            APP_ERR_TRAP (freertos_fat_error);
        }

        /* Closes the FreeRTOS+FAT instance */
        clean_up();

        print_to_console((uint8_t *)"\r\nUSB Device can be safely removed now\r\n");
        /* Update the flag */
        b_usb_hmsc_close = true;
        g_rm_freertos_plus_fat_insertion_events = false;
    }
    else
    {
        /* USB Device disconnected */
        print_to_console((uint8_t *)"USB Device disconnected or not initialized after Eject command\n");
    }
}
/**********************************************************************************************************************
 End of function usb_safely_eject
 *********************************************************************************************************************/
static bool_t s_usb_connection_mon = false;

/*******************************************************************************************************************//**
 * @brief     This function checks the USB HMSC connection status.
 * @param[IN]   None
 * @retval      true if connection detected; false if connection is not detected.
 ***********************************************************************************************************************/
/**********************************************************************************************************************
 * Function Name: check_usb_connection
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
bool check_usb_connection(void)
{
    if (true != g_rm_freertos_plus_fat_insertion_events)
    {
        s_usb_connection_mon = false;
        return false;
    }
    else
    {
        if (false == s_usb_connection_mon)
        {
            print_to_console((uint8_t *)"\r\n USB Device is connected\r\n");
        }
        s_usb_connection_mon = true;
        return true;
    }
}
/**********************************************************************************************************************
 End of function check_usb_connection
 *********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief     This function Initializes the FreeRTOS+FAT instance..
 * @param[IN]   None
 * @retval      Any Other Error code apart from FSP_SUCCESS on Unsuccessful operation.
 ***********************************************************************************************************************/
/**********************************************************************************************************************
 * Function Name: usb_init
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
fsp_err_t usb_init(void)
{
    fsp_err_t freertos_fat_error = FSP_SUCCESS;
    int32_t file_error = SUCCESS;
    rm_freertos_plus_fat_device_t device;

    memset(&device, RESET_VALUE, sizeof(device));

    /* Open FreeRTOS PLUS FAT  */
    freertos_fat_error = RM_FREERTOS_PLUS_FAT_Open(&g_rm_freertos_plus_fat_ctrl, &g_rm_freertos_plus_fat_cfg);
    if (FSP_SUCCESS != freertos_fat_error)
    {
        print_to_console((uint8_t *)"\r\nFREERTOS PLUS FAT OPEN API failed\r\n");
        return freertos_fat_error;
    }

    print_to_console((uint8_t *)"\r\n\nFreeRTOS+FAT Open successful\r\n");

    /* Wait for USB Device connection*/
    print_to_console((uint8_t *)" Connect USB Device...\r\n");

    /* Wait until USB Device is connected */
    while (true != check_usb_connection ())
    {
        ;
    }

    /* Initialize the mass storage device.  This should not be done until the device is plugged in and initialized. */
    freertos_fat_error = RM_FREERTOS_PLUS_FAT_MediaInit(&g_rm_freertos_plus_fat_ctrl, &device);
    if (FSP_SUCCESS != freertos_fat_error)
    {
        print_to_console((uint8_t *)"\r\nFreeRTOS Plus FAT Media Init API failed\r\n");
        fat_clean_up();
        return freertos_fat_error;
    }

    /* Initialize one disk for each partition used in the application. */
    freertos_fat_error = RM_FREERTOS_PLUS_FAT_DiskInit(&g_rm_freertos_plus_fat_ctrl, &g_rm_freertos_plus_fat_disk_cfg,
                                                        &g_my_disk);
    if (FSP_SUCCESS != freertos_fat_error)
    {
        print_to_console((uint8_t *)"\r\nFreeRTOS Plus FAT Disk Init API failed\r\n");
        fat_clean_up();
        return freertos_fat_error;
    }

    /* Mount each disk.  This assumes the disk is already partitioned and formatted. */
    FF_Error_t ff_err = FF_Mount(&g_my_disk, g_my_disk.xStatus.bPartitionNumber);
    if (FSP_SUCCESS != ff_err)
    {
        print_to_console((uint8_t *)"\r\nFF_Mount API failed\r\n");

        /* Close the FREERTOS_PLUS_FAT_Close instance on safely ejecting */
        fat_clean_up();
        /* As function returns fsp_err_t, ff_err cannot be returned. Hence trapping the error here */
        APP_ERR_TRAP (ff_err);
    }

    /* Add the disk to the file system. */
    file_error = FF_FS_Add("/", &g_my_disk);
    if (SUCCESS == file_error)
    {
        print_to_console((uint8_t *)"\r\nFF_Mount API failed\r\n");

        /* Close the FREERTOS_PLUS_FAT_Close instance on safely ejecting */
        fat_clean_up();

        /* As function returns fsp_err_t, ff_err cannot be returned. Hence trapping the error here */
        APP_ERR_TRAP (file_error);
    }

    /* Set this flag to let application know that USB is initialized */
    b_usb_hmsc_close = false;
    return freertos_fat_error;
}
/**********************************************************************************************************************
 End of function usb_init
 *********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief     This function closes the FreeRTOS+FAT instance..
 * @param[IN]   None
 * @retval      Any Other Error code apart from FSP_SUCCESS on Unsuccessful operation.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * Function Name: fat_clean_up
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void fat_clean_up(void)
{
    fsp_err_t freertos_fat_error = FSP_SUCCESS;

    /* Close the FREERTOS_PLUS_FAT_Close instance on any failure */
    freertos_fat_error = RM_FREERTOS_PLUS_FAT_Close (&g_rm_freertos_plus_fat_ctrl);
    if (FSP_SUCCESS != freertos_fat_error)
    {
        print_to_console((uint8_t *)"\r\nFREERTOS PLUS FAT CLOSE API failed\r\n");
    }
    else
    {
        print_to_console((uint8_t *)"\r\nFREERTOS PLUS FAT instance Closed successfully.\r\n");
    }
}
/**********************************************************************************************************************
 End of function fat_clean_up
 *********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief     This function is callback for FreeRTOS+FAT and triggered when USB Pen drive is removed or inserted.
 * @param[IN]   None
 * @retval      None.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * Function Name: free_rtos_fat_callback
 * Description  : .
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void free_rtos_fat_callback(rm_freertos_plus_fat_callback_args_t *p_args)
{
    if (p_args->event & RM_FREERTOS_PLUS_FAT_EVENT_MEDIA_INSERTED)
    {
        g_rm_freertos_plus_fat_insertion_events = true;
    }
    if (p_args->event & RM_FREERTOS_PLUS_FAT_EVENT_MEDIA_REMOVED)
    {
        g_rm_freertos_plus_fat_insertion_events = false;
    }
}
/**********************************************************************************************************************
 End of function free_rtos_fat_callback
 *********************************************************************************************************************/

