/***********************************************************************************************************************
 * File Name    : usb_hmsc_ep.h
 * Description  : Contains declarations of data structures and functions used in usb_hmsc_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef USB_HMSC_EP_H_
#define USB_HMSC_EP_H_

#include <stdbool.h>
#include "hal_data.h"

/* Macro for the reset value */
#define RESET_VALUE             (0x00)

/* Macro for the console input buffer size */
#define BUFFER_SIZE             (16U)

/* Macro for the file name */
#define FILE_NAME               "ra_usb.txt"

/* Macro for the file write mode */
#define WRITE_MODE              "w"

/* Macro for the file read mode */
#define READ_MODE               "r"

/* Macros for the user selection */
#define USB_WRITE               (1U)
#define USB_FORMAT              (2U)
#define USB_SAFELY_EJECT        (3U)
#define FAT_INIT                (4U)

/* Macro for the number of elements to read/write */
#define WRITE_ITEM_SIZE         (10240U)

/* Macros for operation results */
#define READ_FAILURE            (0)
#define FF_FS_ADD_FAILURE       (0)
#define DATA_MATCH              (0)

/* Macro for the ASCII value of 'A', used to fill the write buffer */
#define ASCII_CHAR_A            (65)

/* External variables - Defined in usb_hmsc_ep.c */
extern FF_Disk_t g_my_disk;
extern bool g_usb_hmsc_opened;
extern volatile bool g_media_inserted;

/* Function declarations */
fsp_err_t fat_open(void);
fsp_err_t fat_init(void);
fsp_err_t fat_deinit(void);
fsp_err_t disk_deinit(void);

#endif /* USB_HMSC_EP_H_ */
