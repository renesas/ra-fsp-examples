/***********************************************************************************************************************
* File Name    : fat_operation.h
* Description  : Contains macros, data structures, and function prototypes for FAT file system operations.
***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef FAT_OPERATION_H_
#define FAT_OPERATION_H_

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "common_utils.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define FAT_USB_DISK_PATH           "/usb"
#define FAT_USB_FILE_PATH           "/usb/ra_usb.txt"
#define FAT_USB_FILE_COPY_PATH      "/usb/ra_sd_copy.txt"

#define FAT_INTERNAL_DISK_PATH      "/sd"
#define FAT_INTERNAL_FILE_PATH      "/sd/ra_sd.txt"
#define FAT_INTERNAL_FILE_COPY_PATH "/sd/ra_usb_copy.txt"

#define ENTRY_MODE_STR_LEN          (7U)
#define ENTRY_DATE_STR_LEN          (18U)
#define ENTRY_TIME_STR_LEN          (10U)
#define ENTRY_INFO_STR_LEN          (256U)

#define DIR_HEADER_FORMAT           "\r\n    Directory: %s"\
                                    "\r\nMode                 LastWriteTime         Length Name"\
                                    "\r\n----                 -------------         ------ ----"

#define ENTRY_MODE_FORMAT           "%c%c%c%c%c%c"
#define ENTRY_DATE_FORMAT           "%d/%d/%d"
#define ENTRY_TIME_FORMAT           "%2d:%02d:%02d"
#define ENTRY_INFO_FORMAT           "%6s %17s %9s %14lu %s"

#define FILE_DATA_ZERO              (0U)
#define FILE_BUFF_MAX_SIZE          (1024U)
#define FILE_WRITE_TIMES            (10U)

#define FILE_OPEN_FOR_READ          "r"
#define FILE_OPEN_FOR_WRITE         "w"

/***********************************************************************************************************************
 * Typedefs
 **********************************************************************************************************************/
/* Enumeration for USB and internal disk events */
typedef enum e_fat_event
{
    FAT_USB_DISK_INSERTED      = 1U << 0,
    FAT_USB_DISK_REMOVED       = 1U << 1,
    FAT_INTERNAL_DISK_INSERTED = 1U << 2,
    FAT_INTERNAL_DISK_REMOVED  = 1U << 3,
    FAT_ALL_DISK_REQUEST_OPEN  = 1U << 4,
    FAT_ALL_DISK_REQUEST_CLOSE = 1U << 5,
    FAT_GENERAL_INPUT_COMMAND  = 1U << 6,
} fat_event_t;

/* Structure representing a FAT disk */
typedef struct st_fat_disk
{
    rm_freertos_plus_fat_instance_t const * p_rm_fat_instance;
    rm_freertos_plus_fat_disk_cfg_t * p_rm_fat_disk_cfg;
    char * p_ff_fs_path;
    rm_freertos_plus_fat_device_t rm_device;
    FF_Disk_t ff_disk;
    bool required_open_rm_fat;
    bool is_open;
    bool is_mount;
} fat_disk_t;

/***********************************************************************************************************************
 * Public function prototypes
 **********************************************************************************************************************/
fsp_err_t fat_disk_create(fat_disk_t * p_disk, char * p_ff_fs_path, bool required_open_rm_fat,
                          rm_freertos_plus_fat_instance_t const * p_rm_fat_instance,
                          rm_freertos_plus_fat_disk_cfg_t * p_rm_fat_disk_cfg);
fsp_err_t fat_disk_open(fat_disk_t * p_disk);
fsp_err_t fat_disk_mount(fat_disk_t * p_disk);
fsp_err_t fat_disk_unmount(fat_disk_t * p_disk);
fsp_err_t fat_disk_close(fat_disk_t * p_disk);
fsp_err_t fat_disk_format(fat_disk_t * p_disk);
fsp_err_t fat_disk_delete(fat_disk_t * p_disk);
void fat_dir_list(char * p_path);
fsp_err_t fat_file_copy(char * p_src_path, char * p_dest_path);
fsp_err_t fat_file_remove(char * p_path);
fsp_err_t fat_file_create(char * p_path);

#endif /* FAT_OPERATION_H_ */
