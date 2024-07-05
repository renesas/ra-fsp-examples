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
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : menu_aes.c
 * Version      : .
 * Description  : The encryption / FAT32 demo.
*********************************************************************************************************************/

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"

#include "common_init.h"
#include "common_utils.h"
#include "usb_multiport.h"
#include "menu_aes.h"


#define CONNECTION_ABORT_CRTL    (0x00)
#define MENU_ENTER_RESPONSE_CRTL (0x09)
#define MENU_EXIT_CRTL           (0x20)

#define MODULE_NAME     "\r\n%d. CRYPTOGRAPHY AND USB HIGH SPEED (MSC)\r\n"

#define SUB_OPTIONS     "\r\nPerform encryption and decryption and read/write data from/to USB flash drive"\
                        "\r\nusing USB MSC. AES 256 algorithm and key (below) are used for " \
                        "\r\nencryption and decryption. Using Key [%s]\r\n\r\n"

#define STEP1           "Format a USB flash drive (not provided in the kit) with FAT32 file system.\r\n"   \
                        "Create a file 'source.txt' in the root directory containing the text \r\n"        \
                        "to be encrypted. The file size shall be less than 1 KB.\r\n\r\n"                  \
                        "> Connect the micro AB connector of the USB host cable to the USB High Speed\r\n" \
                        "  port of EK-RA6M5 board. Press tab to continue \r\n"

extern char_t g_write_data[];  /* Data to write to file */
extern char_t g_read_data[];   /* Variable to store the data read from file */
extern size_t g_bytes_read;    /* variable to count bytes to read from file */

/* Encrypted data */
extern uint8_t g_encrypted_data[];
extern size_t g_encrypted_length;
extern uint8_t g_input_data[];
extern size_t  g_input_data_size;
extern uint8_t g_output_data[];
extern size_t g_output_data_size;
extern size_t g_output_length;

extern char_t g_read_fname[32];
extern char_t g_write_fname[32];

extern bool_t g_usb_msc_rd_detail_en;

static char_t s_print_buffer[BUFFER_LINE_LENGTH] = {};

/**********************************************************************************************************************
 * Function Name: wait_to_continue
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static int8_t wait_to_continue(void)
{
    int8_t c = -1;

    while ((CONNECTION_ABORT_CRTL != c))
    {
        c = input_from_console ();

        if ((MENU_EXIT_CRTL == c) || (CONNECTION_ABORT_CRTL == c) || (MENU_ENTER_RESPONSE_CRTL == c))
        {
            break;
        }

    }
    return (c);
}
/**********************************************************************************************************************
 End of function wait_to_continue
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: wait_on_usb_event
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static bool_t wait_on_usb_event(void)
{
    bool_t operation_abort = true;
    EventBits_t uxBits;

    while (true)
    {
        uxBits = xEventGroupWaitBits(g_update_console_event,
                        STATUS_USB_READY | STATUS_USB_MSC_ERROR , pdFALSE, pdTRUE, 1);

        if (((uxBits & (STATUS_USB_READY)) == (STATUS_USB_READY)) ||
            ((uxBits & (STATUS_USB_MSC_ERROR)) == (STATUS_USB_MSC_ERROR)))
        {
            if ((uxBits & (STATUS_USB_READY)) == (STATUS_USB_READY))
            {
                xEventGroupClearBits(g_update_console_event, STATUS_USB_READY);
                operation_abort = false;
                break;
            }

            if ((uxBits & (STATUS_USB_MSC_ERROR)) == (STATUS_USB_MSC_ERROR))
            {
                xEventGroupClearBits(g_update_console_event, STATUS_USB_MSC_ERROR);
                operation_abort = true; // default state
                break;
            }

        }
    }

    return (operation_abort);
}

/**********************************************************************************************************************
 End of function wait_on_usb_event
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: enc_display_menu
 * Description  : .
 * Return Value : The encryption/FAT32 menu screen.
 *********************************************************************************************************************/
test_fn enc_display_menu(void)
{
    int8_t c = -1;
    bool_t operation_abort = false;
    char_t key_str[33] = {}; // 32 for key + '\0'

    sprintf (s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    print_to_console((void*)s_print_buffer);

    sprintf (s_print_buffer, MODULE_NAME, g_selected_menu);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    print_to_console((void*)s_print_buffer);

    initialise_crypto_key();

    get_key_as_str(&key_str[0], 33);
    sprintf (s_print_buffer, SUB_OPTIONS, key_str);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    print_to_console((void*)s_print_buffer);

    sprintf (s_print_buffer, STEP1);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    print_to_console((void*)s_print_buffer);

    c = wait_to_continue();

    if (MENU_EXIT_CRTL == c)
    {
        /* ignoring -Wpointer-sign is OK for a constant string */
        print_to_console((uint8_t *)"\r\nABORTING OPERATION\r\n");
        operation_abort = true;
    }

    if (false == operation_abort)
    {
        /* ignoring -Wpointer-sign is OK for a constant string */
        print_to_console((uint8_t *)"> Do not remove USB Device until instructed that it is safe to do so\r\n");

        memset(&g_read_data[0], 0, USB_IO_BUFFER);
        memset(&g_write_data[0], 0, USB_IO_BUFFER);

        xEventGroupSetBits(g_update_console_event, STATUS_ENABLE_USB_MSC);
        operation_abort = wait_on_usb_event();
    }

    if (false == operation_abort)
    {
        /* set source filename for read */
        char_t * p_name = &g_read_fname[1];
        g_usb_msc_rd_detail_en = true;

        sprintf(g_read_fname, "%s", READ_FILE_NAME);
        sprintf (s_print_buffer, "\r\n Looking for \"%s\" in the root directory...\r\n", p_name);

        /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
        print_to_console((void*)s_print_buffer);

        xEventGroupSetBits(g_update_console_event, STATUS_USB_REQUEST_READ);
        operation_abort = wait_on_usb_event();
    }

    if (false == operation_abort)
    {
        /* Copy data from USB to crypto systems */
        memcpy(g_input_data, (uint8_t *) &g_read_data[0], g_bytes_read);
        g_input_data_size = g_bytes_read;

        initialise_crypto_demo();
        encrypt_demo();

        /* Copy data from crypto to USB systems */
        memcpy((uint8_t *) &g_write_data[0], (uint8_t *) &g_encrypted_data[0], g_encrypted_length);

        sprintf(g_write_fname, "%s", ENCRYPT_FILE_NAME);
        xEventGroupSetBits(g_update_console_event, STATUS_USB_REQUEST_WRITE);
        operation_abort = wait_on_usb_event();
    }

    if (false == operation_abort)
    {
        char_t * p_name = &g_read_fname[1];
        g_usb_msc_rd_detail_en = false;

        sprintf(g_read_fname, "%s", ENCRYPT_FILE_NAME);
        sprintf(s_print_buffer, " Created \"%s\" in the flash drive\r\n", p_name);

        /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
        print_to_console((void*)s_print_buffer);

        /* Open the encrypted file and read */
        sprintf(s_print_buffer, "\r\n Reading data in \"%s\"......\r\n", p_name);

        /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
        print_to_console((void*)s_print_buffer);

        xEventGroupSetBits(g_update_console_event, STATUS_USB_REQUEST_READ);
        operation_abort = wait_on_usb_event();

    }

    if (false == operation_abort)
    {
        decrypt_demo();
        sprintf(g_write_fname, "%s", DECRYPT_FILE_NAME);

        /* Copy data from crypto to USB systems */
        memcpy((uint8_t *) &g_write_data[0], (uint8_t *) &g_output_data[0], g_output_data_size);

        xEventGroupSetBits(g_update_console_event, STATUS_USB_REQUEST_WRITE);
        operation_abort = wait_on_usb_event();

    }

    if (false == operation_abort)
    {
        char_t * p_name = &g_read_fname[1];
        g_usb_msc_rd_detail_en = false;

        sprintf(g_read_fname, "%s", DECRYPT_FILE_NAME);
        sprintf (s_print_buffer, " Created \"%s\" in the flash drive\r\n", p_name);

        /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
        print_to_console((void*)s_print_buffer);
    }

    /* Re-open the original source file */
    if (false == operation_abort)
    {
        sprintf(g_read_fname, "%s", READ_FILE_NAME);
        g_usb_msc_rd_detail_en = false;
        xEventGroupSetBits(g_update_console_event, STATUS_USB_REQUEST_READ);
        operation_abort = wait_on_usb_event();

    }

    /* The original source.txt file contents is in g_read_fname */
    /* The decrypted file contents is in g_write_fname */
    if (false == operation_abort)
    {
        char_t * p_rd_name = &g_read_fname[1];
        char_t * p_wr_name = &g_write_fname[1];

        sprintf(s_print_buffer, "\r\n Comparing data in \"%s\" and \"%s\"...\r\n", p_rd_name, p_wr_name);

        /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
        print_to_console((void*)s_print_buffer);

        if ((g_output_length == g_input_data_size) && (memcmp(g_write_data, g_read_data, g_input_data_size) == 0))
        {
            sprintf (s_print_buffer, " Data matches. File operation successful\r\n");

            /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
            print_to_console((void*)s_print_buffer);
        }

        deinitialise_crypto_demo();
    }
    xEventGroupSetBits(g_update_console_event, STATUS_USB_REQUEST_RESET);

    wait_on_usb_event();

    /* ignoring -Wpointer-sign is OK for a constant string */
    print_to_console((uint8_t *)"\r\n> Its now safe to remove the USB Device");

    if (false == operation_abort)
    {

        /* ignoring -Wpointer-sign is OK for a constant string */
        print_to_console((uint8_t *)"\r\n> Remove the flash drive from the EK-RA6M5 kit \r\n" \
                            "  and connect to the PC to view the contents of the text files");
    }

    sprintf (s_print_buffer, MENU_RETURN_INFO);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    print_to_console((void*)s_print_buffer);

    while ((CONNECTION_ABORT_CRTL != c))
    {
        c = input_from_console ();
        if ((MENU_EXIT_CRTL == c) || (CONNECTION_ABORT_CRTL == c))
        {
            break;
        }
    }
    return (0);
}
/**********************************************************************************************************************
 End of function enc_display_menu
 *********************************************************************************************************************/

