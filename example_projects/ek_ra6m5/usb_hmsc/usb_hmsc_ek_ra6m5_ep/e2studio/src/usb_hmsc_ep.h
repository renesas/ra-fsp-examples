/***********************************************************************************************************************
 * File Name    : usb_hmsc_ep.h
 * Description  : Contains data structures and functions used in usb_hmsc_ep.h.
 **********************************************************************************************************************/
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

#ifndef USB_HMSC_EP_H_
#define USB_HMSC_EP_H_

/* Macros used in usb_hmsc Example Project */
#define EP_INFO        "\r\nThis example project demonstrates basic functionalities of USB Host Mass Storage Class\r\n"\
                       "(HMSC) driver with FreeRTOS+FAT file system on Renesas RA MCUs based on Renesas FSP.\r\n"\
                       "FreeRTOS+FAT uses the underlying Block media driver which utilizes USB HMSC driver to perform\r\n"\
                       "file operations on the USB storage devices.\r\n"\
                       "Error and info messages will be printed on JlinkRTTViewer.\r\n"

#define FILE_NAME               "ra_usb.txt"      // File name
#define WRITE_MODE              "w"               // File Write mode
#define READ_MODE               "r"               // File Read mode

#define USB_WRITE               (1u)              // RTT Input selection for write operation
#define USB_FORMAT              (2u)              // RTT Input selection for format operation
#define USB_SAFELY_EJECT        (3u)              // RTT Input selection for safely eject operation
#define USB_INIT                (4u)              // RTT Input selection for Re-initialization of USB
                                                  // Valid only after Safely_Eject option is executed.

#define WRITE_ITEM_SIZE         (10240u)          // The number of elements to read
#define SUCCESS                 (0)
#define READ_WRITE_FAILURE      (0)
#define ASCII_CHAR_A            (65)

/* USB Drive HMSC Menu options */
#define USB_HMSC_MENU           "\r\nUSB HMSC Menu options\r\n"\
                                "\r\n 1. Write 10k data from app_buffer to ra_usb.txt file\r\n"\
                                "\r\n 2. Format USB Drive\r\n"\
                                "\r\n 3. Safely Eject the USB Drive\r\n"\
                                "\r\n 4. Initialize FreeRTOS+FAT (Valid only after Safely_Eject option is executed)\r\n"

/* Global Variables */
extern uint8_t g_write_data[WRITE_ITEM_SIZE];   /* Data(10k) to write to file */
extern uint8_t g_read_data[WRITE_ITEM_SIZE];   /* Variable to store the data read from file */
extern FF_Disk_t my_disk;                                      /* Pointer to store FreeRTOS+FAT disk structure */
/* Function Declarations */
void process_usb_operation(uint8_t p_input_buffer);     /* Processes USB operations */
void usb_write_operation(void);                         /* Performs USB write operation */
void usb_read_operation(void);                          /* Performs USB read operation */
void format_usb_device(void);                           /* Performs USB format operation */
bool check_usb_connection(void);                        /* Checks the USB connection status */
void usb_safely_eject(void);                            /* Performs USB Safely disconnect operation */
void update_buffer(void);                               /* Update write buffer with data and clear read buffer */
fsp_err_t usb_init(void);                               /* Initialize USB HMSC device */
void fat_clean_up(void);                                /* Closes FreeRTOS+FAT */

#endif /* USB_HMSC_EP_H_ */
