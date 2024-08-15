/***********************************************************************************************************************
 * File Name    : usb_hmsc_ep.h
 * Description  : Contains data structures and functions used in usb_hmsc_ep.h.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2022 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef USB_HMSC_BM_EP_H_
#define USB_HMSC_BM_EP_H_

/* Macros used in usb_hmsc Example Project */
#define EP_INFO        "\r\nThis example project demonstrates basic functionalities of USB Host Mass Storage Class\r\n"\
		               "(HMSC) driver with FreeRTOS+FAT file system on Renesas RA MCUs based on Renesas FSP.\r\n"\
		               "FreeRTOS+FAT uses the underlying Block media driver which utilizes USB HMSC driver to perform\r\n"\
		               "file operations on the USB storage devices.\r\n"\
		               "Error and EP status messages will be printed on J-link RTT Viewer.\r\n"

#define WRITE_MODE              "w"               // File Write mode
#define READ_MODE               "r"               // File Read mode
#define FILE_ENTRY               "/"              // Disk entry

/* USB status */
typedef enum
{
    STATE_DATA_WRITE = 1,
    STATE_FILE_READ,
    STATE_FORMAT,
    STATE_REMOVE,
    STATE_RECONFIGURE,
    STATE_ATTACH,
    STATE_DETACH,
    STATE_USB_CONFIGURED,
} state_t;


#define WRITE_ITEM_SIZE         (1U)          // The number of elements to write
#define SUCCESS                 (0U)
#define READ_WRITE_FAILURE      (0U)
#define PRINT_DELAY             (2U)

/* USB Drive HMSC Menu options */
#define USB_HMSC_MENU           "\r\nUSB HMSC Menu options\r\n"\
		                        "1. Write to File\r\n"\
		                        "2. Read from File\r\n"\
		                        "3. Format USB Drive\r\n"\
		                        "4. Safely Eject the USB Drive\r\n"\
		                        "5. Initialize FreeRTOS+FAT (Valid only after Safely_Eject option is executed)\r\n"

#define RM_FREERTOS_PLUS_FAT_EXAMPLE_FILE_NAME            "TEST_FILE.txt"
#define RM_FREERTOS_PLUS_FAT_EXAMPLE_BUFFER_SIZE_BYTES    (2048)
#define RM_FREERTOS_PLUS_FAT_EXAMPLE_PARTITION_NUMBER     (0)

/* User Defined functions */
void process_usb_operation(state_t * p_usb_state, bool * p_is_usb_ejected);                       /* Processes USB operations */
void usb_media_mount(void);                                                           /* Configures USB media */
void usb_write_operation(void);                                                       /* Performs USB write operation */
void usb_read_operation(void);                                                        /* Performs USB read operation */
void format_usb_device(void);                                                         /* Performs USB format operation */
void usb_safely_eject(void);                                                          /* Performs USB Safely disconnect operation */
void fat_clean_up(void);                                                              /* Clean up of FreeRTOS+FAT instance */
void listing_file(void);                                                              /* Listing all current files */

#endif /* USB_HMSC_BM_EP_H_ */
