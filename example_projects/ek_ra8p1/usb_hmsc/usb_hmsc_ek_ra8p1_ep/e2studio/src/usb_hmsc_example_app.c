/***********************************************************************************************************************
 * File Name    : usb_hmsc_example_app.c
 * Description  : Example application layer for the USB HMSC EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - usb_hmsc_thread_entry.c calls a single function: usb_hmsc_thread_entry_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "usb_hmsc_example_app.h"
#include "console/uart_console.h"

/* Private variables */
static uint8_t s_write_data[WRITE_ITEM_SIZE] = {RESET_VALUE};
static uint8_t s_read_data[WRITE_ITEM_SIZE] = {RESET_VALUE};
static bool s_write_success = false;

/* Private functions */
static void process_usb_operation(uint8_t input_selection);
static void usb_write_operation(void);
static void usb_read_operation(void);
static void usb_format_device(void);
static void usb_safely_eject(void);
static void update_buffer(void);
static void user_input(char * buf, int len);
static void handle_error(fsp_err_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the USB HMSC example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void usb_hmsc_thread_entry_run(void)
{
    fsp_pack_version_t version = {RESET_VALUE};
    fsp_err_t err = FSP_SUCCESS;

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

    err = fat_open();
    handle_error(err, "fat_open failed");

    printf("\r\n\r\nFreeRTOS+FAT opened successfully");
    printf("\r\nConnect a USB storage device...");

    while (true != g_media_inserted)
    {
        /* Do nothing */
    }

    printf("\r\nUSB storage device is connected");

    err = fat_init();
    handle_error(err, "fat_init failed");

    /* Print EP menu */
    printf(EP_MENU);

    while (true)
    {
        /* Get the user input */
        if (uart_console_has_char())
        {
            char input_buf[BUFFER_SIZE] = {RESET_VALUE};
            user_input(input_buf, (int)sizeof(input_buf));
            process_usb_operation((uint8_t)atoi(input_buf));
        }

        if ((true == g_usb_hmsc_opened) && (false == g_media_inserted))
        {
            printf("\r\nUSB storage device disconnected without Safely eject option");
            printf("\r\nReconnect the USB storage device and execute Safely eject option to make sure file"
                   "\r\noperations work correctly");

            /* Wait until USB storage device is connected */
            while (true != g_media_inserted)
            {
                /* Do nothing */
            }
            printf("\r\nUSB storage device is connected");
            printf(EP_MENU);
        }

        vTaskDelay(1);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function initiates the USB operation by calling respective functions.
 * @param[in] input_selection           Menu option selected by the user.
 * @retval    None.
 **********************************************************************************************************************/
static void process_usb_operation(uint8_t input_selection)
{
    fsp_err_t err = FSP_SUCCESS;

    switch (input_selection)
    {
        case USB_WRITE:
        {
            usb_write_operation();
            if (true == s_write_success)
            {
                usb_read_operation();
            }
            break;
        }

        case USB_FORMAT:
        {
            usb_format_device();
            break;
        }

        case USB_SAFELY_EJECT:
        {
            usb_safely_eject();
            break;
        }

        case FAT_INIT:
        {
            if (false == g_usb_hmsc_opened)
            {
                err = fat_open();
                handle_error(err, "fat_open failed");

                printf("\r\n\r\nFreeRTOS+FAT opened successfully");
                printf("\r\nConnect a USB storage device...");

                while (true != g_media_inserted)
                {
                    /* Do nothing */
                }

                printf("\r\nUSB storage device is connected");

                err = fat_init();
                handle_error(err, "fat_init failed");
            }
            else
            {
                printf("\r\nFreeRTOS+FAT is already initialized. Provide any other command.");
            }
            break;
        }

        default:
        {
            printf("\r\nInvalid input!");
            break;
        }
    }

    printf(EP_MENU);
}

/*******************************************************************************************************************//**
 * @brief     This function performs USB HMSC write operation.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void usb_write_operation(void)
{
    FF_FILE *file_pointer = NULL;
    FF_Stat_t file_details;
    FF_Error_t ff_err = FF_ERR_NONE;

    /* Capture the number of bytes written in the variable to check write status */
    size_t bytes_written = RESET_VALUE;

    s_write_success = false;

    memset(&file_details, RESET_VALUE, sizeof(file_details));

    /* Double check the connection again to ensure the USB storage device is still mounted */
    if ((true == g_media_inserted) && (true == g_usb_hmsc_opened))
    {
        printf("\r\n\r\nUSB write operation will be initiated. Please wait...");

        /* Open the file in write mode */
        file_pointer = ff_fopen((const char *)FILE_NAME, WRITE_MODE);
        if (NULL != file_pointer)
        {
            /* Fill write buffer with data */
            update_buffer();

            bytes_written = ff_fwrite(s_write_data, sizeof(s_write_data[RESET_VALUE]), WRITE_ITEM_SIZE, file_pointer);
            if (WRITE_ITEM_SIZE != bytes_written)
            {
                printf("\r\n[ERR] ff_fwrite API failed. Error code: %d", stdioGET_ERRNO());

                ff_err = ff_fclose(file_pointer);
                if (FF_ERR_NONE != ff_err)
                {
                    printf("\r\n[ERR] ff_fclose API failed. Error code: %d", stdioGET_ERRNO());
                }
                return;
            }

            /* Close the file after the write operation */
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
                /* Compare the actual bytes written and file size after the write operation */
                if (bytes_written == file_details.st_size)
                {
                    printf("\r\n%zu bytes successfully written to file %s", bytes_written, FILE_NAME);
                    printf("\r\nWrite operation is successful");
                    s_write_success = true;
                }
                else
                {
                    printf("\r\n[ERR] Written data size does not match the file size");
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
            printf("\r\n[ERR] ff_fopen API failed. Error code: %d", stdioGET_ERRNO());
            return;
        }
    }
    else
    {
        printf("\r\n\r\nUSB storage device is disconnected or FreeRTOS+FAT is not initialized");
    }
}

/*******************************************************************************************************************//**
 * @brief     This function performs USB HMSC read operation.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void usb_read_operation(void)
{
    FF_FILE *file_pointer = NULL;
    FF_Stat_t file_details;
    FF_Error_t ff_err = FF_ERR_NONE;

    memset(&file_details, RESET_VALUE, sizeof(file_details));

    /* Double check the connection again to ensure the USB storage device is still mounted */
    if ((true == g_media_inserted) && (true == g_usb_hmsc_opened))
    {
        printf("\r\n\r\nUSB read operation will be initiated");

        /* Open the file in read mode */
        file_pointer = ff_fopen((const char *)FILE_NAME, READ_MODE);
        if (NULL != file_pointer)
        {
            /* Capture the number of bytes read in the variable to check read status */
            size_t bytes_read = RESET_VALUE;
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
                /* Compare the number of bytes read with the file size */
                if (bytes_read == file_details.st_size)
                {
                    printf("\r\n%zu bytes successfully read from file %s", bytes_read, FILE_NAME);
                    printf("\r\nRead operation is successful");
                }
                else
                {
                    printf("\r\n[ERR] Read data size does not match the file size");
                    return;
                }

                /* Compare write and read data */
                if (DATA_MATCH == memcmp(s_write_data, s_read_data, WRITE_ITEM_SIZE))
                {
                    printf("\r\n\r\nWrite and read data match");
                }
                else
                {
                    printf("\r\n[ERR] Write and read data do not match");
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
            printf("\r\n[ERR] ff_fopen API failed. Error code: %d", stdioGET_ERRNO());
            return;
        }
    }
    else
    {
        printf("\r\nUSB storage device is disconnected or FreeRTOS+FAT is not initialized");
    }
}

/*******************************************************************************************************************//**
 * @brief     This function performs USB HMSC format operation.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void usb_format_device(void)
{
    FF_Error_t ff_err = FF_ERR_NONE;

    if ((true == g_media_inserted) && (true == g_usb_hmsc_opened))
    {
        printf("\r\n\r\nUSB storage device formatting will be initiated. Formatting will take time"
               "\r\ndepending on USB storage device capacity.");
        printf("\r\nDo not disconnect the USB storage device while formatting is in progress. Please wait...");

        /* Format time varies depending on the USB storage device capacity */
        ff_err = FF_Format(&g_my_disk, g_my_disk.xStatus.bPartitionNumber, pdFALSE, pdFALSE);
        if (FF_ERR_NONE != ff_err)
        {
            printf("\r\n[ERR] FF_Format API failed");
            printf("\r\n[ERR] Error: %s", FF_GetErrMessage(ff_err));
        }
        else
        {
            printf("\r\nUSB storage device formatted successfully");
        }
    }
    else
    {
        printf("\r\n\r\nUSB storage device is disconnected or FreeRTOS+FAT is not initialized");
    }
}

/*******************************************************************************************************************//**
 * @brief     This function closes USB HMSC on FreeRTOS+FAT and safely ejects.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void usb_safely_eject(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Check the USB storage device connection */
    if ((true == g_media_inserted) && (true == g_usb_hmsc_opened))
    {
        err = disk_deinit();
        handle_error(err, "disk_deinit failed");

        /* Close the FreeRTOS+FAT instance on safely ejecting */
        err = fat_deinit();
        handle_error(err, "fat_deinit failed");

        /* Update the flags */
        g_usb_hmsc_opened = false;
        g_media_inserted = false;

        printf("\r\n\r\nUSB storage device can be safely removed now");
    }
    else
    {
        printf("\r\n\r\nUSB storage device is disconnected or FreeRTOS+FAT is not initialized");
    }
}

/*******************************************************************************************************************//**
 * @brief     This function updates write buffer with data and clears read buffer.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void update_buffer(void)
{
    for (size_t i = RESET_VALUE; i < WRITE_ITEM_SIZE; i++)
    {
        s_write_data[i] = ASCII_CHAR_A;
        s_read_data[i] = RESET_VALUE;
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
        (void)disk_deinit();
        (void)fat_deinit();

        __BKPT(0);
    }
}
