/***********************************************************************************************************************
 * File Name    : usb_multiport_example_app.c
 * Description  : Example application layer for the USB Multiport EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - usb_hmsc_thread_entry.c calls a single function: usb_hmsc_thread_entry_run().
 *                - usb_pcdc_thread_entry.c calls a single function: usb_pcdc_thread_entry_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "usb_multiport_example_app.h"
#include "console/uart_console.h"

/* Private variables */
static uint8_t s_write_data[READ_BUF_SIZE] = {RESET_VALUE};
static uint8_t s_read_data[READ_BUF_SIZE] = {RESET_VALUE};
static uint8_t s_buf[READ_BUF_SIZE] = {RESET_VALUE};
static uint8_t s_rcv_buf[READ_BUF_SIZE] = {RESET_VALUE};

/* Private functions */
static void process_usb_operation(uint8_t input_selection);
static void usb_read_operation(void);
static void usb_write_operation(void);
static void process_usb_pcdc_events(void);
static void user_input(char * buf, int len);
static void handle_error(fsp_err_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the USB HMSC thread of the USB Multiport example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void usb_hmsc_thread_entry_run(void)
{
    fsp_pack_version_t version = {RESET_VALUE};
    fsp_err_t err = FSP_SUCCESS;
    BaseType_t err_queue = pdFALSE;

    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        return;     /* UART failed to open — cannot print anything */
    }

    /* Get FSP version information */
    R_FSP_VersionGet(&version);

    /* Print the EP banner on the terminal */
    printf(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    printf(EP_INFO);

    err = fat_init();
    handle_error(err, "fat_init failed");
    printf("\r\nFreeRTOS+FAT opened successfully");

    printf("\r\nConnect a USB storage device...");

    /* Wait until USB device is connected. This is the blocking call */
    while (true != wait_till_usb_detected())
    {
        /* Do nothing */
    }
    printf("\r\nUSB storage device is connected");

    err = fat_disk_init();
    handle_error(err, "fat_disk_init failed");

    /* Print EP menu */
    printf(EP_MENU);

    while (true)
    {
        /* Get user input */
        if (uart_console_has_char())
        {
            char input_buf[BUFFER_SIZE] = {RESET_VALUE};

            user_input(input_buf, (int)sizeof(input_buf));
            process_usb_operation((uint8_t)atoi(input_buf));
        }

        /* Check for the USB connection status */
        if (check_usb_connected())
        {
            /* Data received from PCDC thread through queue */
            err_queue = xQueueReceive(g_queue_data_from_pcdc, s_write_data, (TickType_t)(RESET_VALUE));
            if (pdTRUE == err_queue)
            {
                printf("\r\nData received from USB PCDC is being written to the file");
                usb_write_operation();
                printf(EP_MENU);
            }
        }

        vTaskDelay(1);
    }
}

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the USB PCDC thread of the USB Multiport example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void usb_pcdc_thread_entry_run(void)
{
    BaseType_t err_queue = pdFALSE;
    fsp_err_t err = FSP_SUCCESS;

    /* The UART console is already opened by the USB HMSC thread, which has a higher priority */
    err = usb_init();
    handle_error(err, "usb_init failed");
    printf("\r\nUSB PCDC opened successfully");

    while (true)
    {
        /* Check if a USB event is received */
        err_queue = xQueueReceive(g_event_queue, &gp_usb_pcdc_event, (TickType_t)(RESET_VALUE));
        if (pdTRUE == err_queue)
        {
            /* Process USB PCDC events */
            process_usb_pcdc_events();
        }

        /* Receive message from queue */
        err_queue = xQueueReceive(g_queue_data_from_hmsc, s_buf, (TickType_t)(RESET_VALUE));
        if (pdTRUE == err_queue)
        {
            /* Write data to Tera Term */
            err = usb_write(&g_basic1_ctrl, s_buf, WRITE_ITEM_SIZE, USB_CLASS_PCDC);
            handle_error(err, "usb_write failed");
        }

        vTaskDelay(1);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function initiates the USB operation by calling the respective functions.
 * @param[in] input_selection           Menu option selected by the user.
 * @retval    None.
 **********************************************************************************************************************/
static void process_usb_operation(uint8_t input_selection)
{
    switch (input_selection)
    {
        case USB_READ:
        {
            /* Read operation from predefined file */
            usb_read_operation();
            printf(EP_MENU);
            break;
        }

        case USB_WRITE:
        {
            if (check_usb_connected())
            {
                /* Set the flag for the write operation */
                g_write_to_usb = true;
                printf("\r\n\r\nGo to Tera Term of the USB PCDC COM port and enter data to write to the file"
                       "\r\nPress Enter to complete the input (up to 512 bytes) and initiate writing to the file %s",
                       FILE_NAME);
            }
            else
            {
                printf("\r\nUSB storage device is not connected");
                printf(EP_MENU);
            }
            break;
        }

        default:
        {
            printf("\r\nInvalid input!");
            printf(EP_MENU);
            break;
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function performs USB HMSC read operation.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void usb_read_operation(void)
{
    FF_FILE * file_pointer = NULL;
    FF_Stat_t file_details;
    FF_Error_t ff_err = FF_ERR_NONE;

    /* Reset the file details */
    memset(&file_details, RESET_VALUE, sizeof(file_details));

    /* Double check the connection again to ensure the USB device is still mounted */
    if (check_usb_connected())
    {
        /* Open the file in read mode to read data written previously */
        file_pointer = ff_fopen((const char *)FILE_NAME, READ_MODE);
        if (NULL != file_pointer)
        {
            printf("\r\n\r\nUSB read operation will be initiated");

            size_t bytes_read = RESET_VALUE;
            size_t bytes_read_total = RESET_VALUE;

            while (!ff_feof(file_pointer))
            {
                /* Clear the buffer so that a shorter last chunk does not carry data of the previous chunk */
                memset(s_read_data, RESET_VALUE, sizeof(s_read_data));

                /* Read data from file */
                bytes_read = ff_fread(s_read_data, sizeof(s_read_data[RESET_VALUE]), WRITE_ITEM_SIZE, file_pointer);
                if (READ_FAILURE == bytes_read)
                {
                    printf("\r\n[ERR] ff_fread API failed. Error code: %d", stdioGET_ERRNO());

                    ff_err = ff_fclose(file_pointer);
                    if (FF_ERR_NONE != ff_err)
                    {
                        printf("\r\n[ERR] ff_fclose API failed. Error code: %d", stdioGET_ERRNO());
                    }
                    return;
                }

                /* Send the data read from file to the PCDC thread through queue */
                if (pdTRUE == xQueueSend(g_queue_data_from_hmsc, s_read_data, (TickType_t)(RESET_VALUE)))
                {
                    bytes_read_total = bytes_read_total + bytes_read;
                }
            }

            /* Close the file after the read operation */
            ff_err = ff_fclose(file_pointer);
            if (FF_ERR_NONE != ff_err)
            {
                printf("\r\n[ERR] ff_fclose API failed. Error code: %d", stdioGET_ERRNO());
                return;
            }

            ff_err = ff_stat((const char *)FILE_NAME, &file_details);
            /* ff_stat returns 0 on success and -1 on error */
            if (FF_ERR_NONE == ff_err)
            {
                /* Compare the actual bytes read and file size after the read operation */
                if (bytes_read_total == file_details.st_size)
                {
                    printf("\r\n%zu bytes successfully read from file %s", bytes_read_total, FILE_NAME);
                    printf("\r\nRead operation is successful");
                }
                else
                {
                    printf("\r\n[ERR] Read data size does not match the file size");
                    return;
                }
            }
            else
            {
                printf("\r\n[ERR] ff_stat API failed. Error code: %d", stdioGET_ERRNO());
                return;
            }
        }
        else
        {
            int32_t error_code = stdioGET_ERRNO();

            if (ENOENT == error_code)
            {
                printf("\r\n[ERR] File %s does not exist", FILE_NAME);
            }
            else
            {
                printf("\r\n[ERR] ff_fopen API failed. Error code: %d", error_code);
            }

            return;
        }
    }
    else
    {
        printf("\r\nUSB storage device is not connected");
    }
}

/*******************************************************************************************************************//**
 * @brief     This function performs USB HMSC write operation.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void usb_write_operation(void)
{
    FF_FILE * file_pointer = NULL;
    FF_Error_t ff_err = FF_ERR_NONE;
    size_t bytes_written = RESET_VALUE;

    /* Double check the connection again to ensure the USB device is still mounted */
    if (check_usb_connected())
    {
        /* Once connection is detected, open the file in write mode */
        file_pointer = ff_fopen((const char *)FILE_NAME, WRITE_MODE);
        if (NULL != file_pointer)
        {
            printf("\r\nUSB write operation will be initiated");

            /* Write data to the file */
            bytes_written = ff_fwrite(s_write_data, sizeof(s_write_data[RESET_VALUE]), g_bytes_to_write,
                                      file_pointer);
            if (g_bytes_to_write != bytes_written)
            {
                printf("\r\n[ERR] ff_fwrite API failed. Error code: %d", stdioGET_ERRNO());

                ff_err = ff_fclose(file_pointer);
                if (FF_ERR_NONE != ff_err)
                {
                    printf("\r\n[ERR] ff_fclose API failed. Error code: %d", stdioGET_ERRNO());
                }

                g_write_to_usb = false;
                return;
            }

            /* Close the file after the write operation */
            ff_err = ff_fclose(file_pointer);
            if (FF_ERR_NONE != ff_err)
            {
                printf("\r\n[ERR] ff_fclose API failed. Error code: %d", stdioGET_ERRNO());
                return;
            }

            g_bytes_to_write = RESET_VALUE;
            printf("\r\n%zu bytes successfully written to file %s", bytes_written, FILE_NAME);
            printf("\r\nWrite operation is successful");
        }
        else
        {
            printf("\r\n[ERR] ff_fopen API failed. Error code: %d", stdioGET_ERRNO());
            return;
        }
    }
    else
    {
        printf("\r\nUSB storage device is not connected");
    }
}

/*******************************************************************************************************************//**
 * @brief     This function processes USB PCDC events.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void process_usb_pcdc_events(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* USB event received */
    switch (gp_usb_pcdc_event->event)
    {
        /* Configured event */
        case USB_STATUS_CONFIGURED:
        {
            err = usb_read(&g_basic1_ctrl, s_rcv_buf, READ_BUF_SIZE, USB_CLASS_PCDC);
            handle_error(err, "usb_read failed");
            break;
        }

        /* Write complete event */
        case USB_STATUS_WRITE_COMPLETE:
        {
            break;
        }

        /* Read complete event */
        case USB_STATUS_READ_COMPLETE:
        {
            /* Read data from Tera Term */
            err = usb_read(&g_basic1_ctrl, s_rcv_buf, READ_BUF_SIZE, USB_CLASS_PCDC);
            handle_error(err, "usb_read failed");

            /* Check for flag status */
            if (g_write_to_usb)
            {
                fill_data_buffer_and_write_to_file(s_rcv_buf, gp_usb_pcdc_event->data_size);
            }

            /* Loop back received data to host */
            err = usb_write(&g_basic1_ctrl, s_rcv_buf, gp_usb_pcdc_event->data_size, USB_CLASS_PCDC);
            handle_error(err, "usb_write failed");
            break;
        }

        /* Receive class request */
        case USB_STATUS_REQUEST:
        {
            err = usb_pcdc_class_request();
            handle_error(err, "usb_pcdc_class_request failed");
            break;
        }

        /* Detach event */
        case USB_STATUS_DETACH:
        {
            printf("\r\nUSB is detached");
            break;
        }

        /* Suspend and resume events */
        case USB_STATUS_SUSPEND:
        case USB_STATUS_RESUME:
        {
            break;
        }

        default:
        {
            /* Do nothing */
            break;
        }
    }
}

/*******************************************************************************************************************//**
 * @brief         This function reads a line of user input from stdin and removes trailing newline characters.
 * @param[in,out] buf                   Pointer to buffer where the input string is stored.
 * @param[in]     len                   Maximum number of characters to read (buffer size).
 * @retval        None.
 **********************************************************************************************************************/
static void user_input(char * buf, int len)
{
    fgets(buf, len, stdin);
    buf[strcspn(buf, "\r\n")] = '\0';
}

/*******************************************************************************************************************//**
 * @brief     This function handles an error by printing error message and code, de-initializing the related modules,
 *            and entering a trap state.
 * @param[in] err                       Error code.
 * @param[in] err_str                   Error message.
 * @retval    None.
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err, const char * err_str)
{
    if (FSP_SUCCESS != err)
    {
        printf("\r\n[ERR] %s", err_str);
        printf("\r\nReturned Error Code: 0x%x", (unsigned int)err);
        uart_console_flush();
        usb_deinit();
        (void)fat_disk_deinit();
        (void)fat_deinit();
        __BKPT(0);
    }
}
