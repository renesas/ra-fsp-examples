/***********************************************************************************************************************
 * File Name    : flash.h
 * Description  : common flash
 * flash routine related definitions.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef FLASH_H_
#define FLASH_H_

#include "bsp_api.h"
#include "hal_data.h"
/* Provision info storage address ( internal flash ) */
#define CODE_FLASH_WRITE_SIZE           (128)
#define FLASH_BLOCK_SIZE                (8 * 1024)
#define FLASH_HP_CF_BLOCK_SIZE_32KB     (32 * 1024)    /* Block Size 32 KB */
#define DEVICE_CERT_OFFSET              (0x020F0000)

#define DEVICE_CERT_BLOCK_ADDR                      (DEVICE_CERT_OFFSET)
#define DEVICE_CERT_BLOCK_LEN                       (FLASH_HP_CF_BLOCK_SIZE_32KB * 1)

#define NUM_OF_BLOCKS(x)                ((x) / FLASH_HP_CF_BLOCK_SIZE_32KB)

/* Info type IDs */
typedef enum e_int_storage_type {
    DEV_CERT_CFG,           /*  Device Certificate */
}int_storage_type_t;

fsp_err_t flash_init(void);
fsp_err_t flash_deinit(void);
fsp_err_t flash_read(uint8_t *data_buffer, unsigned int data_size,unsigned int info_type);
fsp_err_t flash_write(uint8_t *data_buffer , unsigned int data_size, unsigned int info_type);
int write_file(lfs_t *lfs, const char *path, const void *data, size_t data_size);
int read_file(lfs_t *lfs, const char *path, void *data, size_t data_size);
#endif /* FLASH_H_ */
