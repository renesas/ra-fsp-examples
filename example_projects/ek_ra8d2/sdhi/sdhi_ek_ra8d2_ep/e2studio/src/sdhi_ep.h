/***********************************************************************************************************************
 * File Name    : sdhi_ep.h
 * Description  : Contains macros and functions used in sdhi_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef SDHI_EP_H_
#define SDHI_EP_H_

#include <stdbool.h>
#include <stddef.h>
#include "hal_data.h"

/* Macro for the reset value */
#define RESET_VALUE             (0x00)

/* Macro for the null char */
#define NULL_CHAR               ('\0')

/* Macro for the input buffer size */
#define INPUT_SIZE              (16U)

/* Macros for file operation */
#define FILE_NAME               "ra_sdhi.txt"
#define WRITE_MODE              "w"
#define READ_MODE               "r"

/* Macros for SD operation */
#define SD_WRITE                (1U)
#define SD_FORMAT               (2U)
#define SAFELY_REMOVE           (3U)
#define SD_DISPLAY_CWD          (4U)
#define SD_INIT                 (5U)

/* Macros for buffer and file system */
#define BUFFER_SIZE             (10240U)
#define FF_FS_ADD_FAILURE       (0)
#define ASCII_CHAR_A            (65)

/* Function declarations */
fsp_err_t fat_init(void);
fsp_err_t fat_disk_init(FF_Disk_t * const p_disk);
fsp_err_t fat_disk_deinit(FF_Disk_t * const p_disk);
fsp_err_t fat_media_init(rm_freertos_plus_fat_device_t * const p_device);
void fat_deinit(void);

#endif /* SDHI_EP_H_ */
