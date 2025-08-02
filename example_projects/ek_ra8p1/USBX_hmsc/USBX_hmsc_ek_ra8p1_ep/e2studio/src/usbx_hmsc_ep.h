 /**********************************************************************************************************************
 * File Name    : usbx_hmsc_ep.h
 * Description  : Contains data structures and functions used in the EP.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef USBX_HMSC_EP_H_
#define USBX_HMSC_EP_H_

#include "hmsc_thread.h"
#include "ux_api.h"
#include "ux_host_class_storage.h"

/* To display EP info on the RTT Viewer */
#define EP_INFO        "\r\nThis example project demonstrates the basic functionalities of USBX Host Mass Storage"\
                       "\r\nClass (HMSC) driver with Azure RTOS + FileX file system on Renesas RA MCUs based on"\
                       "\r\nRenesas FSP. FileX interface on USBX uses the underlying USB HMSC driver to perform"\
                       "\r\nvarious file operations on the USB storage devices based on user input from the menu."\
                       "\r\nStatus of application will be displayed on J-Link RTT Viewer.\r\n"\

#define USBX_HMSC_MENU          "\r\nUSBX_HMSC Main Menu options:"\
                                "\r\n1. Enter 1 to create a directory and a text file on the disk."\
                                "\r\n2. Enter 2 to Write Pre-defined data on text file."\
                                "\r\n3. Enter 3 to Read the data from file and compare."\
                                "\r\n4. Enter 4 to Display current working directory content."\
                                "\r\n5. Enter 5 to Safely Eject the USB flash drive."\
                                "\r\nUser Input:  "

/* Macros to print info, error and trap the error */
#define PRINT_INFO_STR(str)  ({app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_INFO_STR, sizeof(str), (str));})
#define PRINT_ERR_STR(str)   ({app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_STR, sizeof(str), (str));})
#define ERROR_TRAP(err)      ({app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_TRAP, sizeof(UINT *), &(err));})

#define DATA_LEN                    (32U)
#define TRUNCATION_VALUE            (0U)
#if defined BOARD_RA4L1_EK
#define MEMPOOL_SIZE                (32768)
#else
#define MEMPOOL_SIZE                (63488)
#endif
#define EVENT_USB_PLUG_IN           (1UL << 0)
#define EVENT_USB_PLUG_OUT          (1UL << 1)
#define MAX_BYTES                   (32)
#define READ_LEN                    (64)
#define WAIT_TIME                   (50)
#define VALUE_32                    (32)
#define UX_FSP_DEVICE_INSERTION     (0x01U)
#define UX_FSP_DEVICE_REMOVAL       (0x02U)
#define ARRAY_SIZE                  (20)
#define VALUE_16                    (16)
#define ONE                         (1)
#define TWO                         (2)

/* ASCII code */
#define ASCII_ZERO_CODE             (48)
#define ASCII_CHAR_A                (65)
#define ASCII_LOWER_A               (0x61)
#define ASCII_LOWER_N               (0x6E)
#define ASCII_LOWER_R               (0x72)
#define ASCII_LOWER_P               (0x70)
#define DATE_VALUE_CHECK            (2000U)
#define DATE_VALUE_SUB              (2400U)
#define DIR_SPACE_REQUIRED          (512u)

/* Macro for null character */
#define NULL_CHAR                   ('\0')

#define JAN_JUN_JUL                 (0x4A)
#define FEB                         (0x46)
#define MAR_MAY                     (0x4D)
#define APR_AUG                     (0x41)
#define SEP                         (0x53)
#define OCT                         (0x4F)
#define NOV                         (0x4E)
#define DEC                         (0x44)

#define CREATE                      (1U)          /* RTT input selection for create operation */
#define WRITE                       (2U)          /* RTT input selection for write operation */
#define READ                        (3U)          /* RTT input selection for read operation */
#define DISPLAY                     (4U)          /* RTT input selection to display working directory content */
#define EJECT                       (5U)          /* RTT input selection for write operation */

/* Structure to store time */
typedef struct st_time_format_params
{
    UINT month;
    UINT date;
    UINT year;
    UINT hour;
    UINT min;
    UINT sec;
}time_format_t;

/* Structure to store file details */
typedef struct st_file_info
{
    CHAR directory_name[ARRAY_SIZE];
    CHAR attributes[ARRAY_SIZE];
    ULONG size;
    time_format_t time_stamp;
}file_info_t;

/* Enumeration for month values */
typedef enum e_month_numbers
{
    JANUARY    = 1,
    FEBRUARY   = 2,
    MARCH      = 3,
    APRIL      = 4,
    MAY        = 5,
    JUNE       = 6,
    JULY       = 7,
    AUGUST     = 8,
    SEPTEMBER  = 9,
    OCTOBER    = 10,
    NOVEMBER   = 11,
    DECEMBER   = 12,
}month_numbers_t;

#endif /* USBX_HMSC_EP_H_ */
