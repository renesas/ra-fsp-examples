/***********************************************************************************************************************
 * File Name    : vee_flash_ep.h
 * Description  : Contains macros, data structures, and functions used in vee_flash_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef VEE_FLASH_EP_H_
#define VEE_FLASH_EP_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "hal_data.h"

/* Macro for the reset value */
#define RESET_VALUE             (0x00)

/* Macro for the buffer size */
#define BUFFER_SIZE             (16U)

/* Macro for the length of data to write */
#define DATA_SIZE               (12U)

/* Macro for the null char */
#define NULL_CHAR               ('\0')

/* Macros for the Record ID number */
#define RECORD_ID_1             (1U)
#define RECORD_ID_2             (2U)
#define RECORD_ID_3             (3U)
#define RECORD_ID_4             (4U)

/* Macros for the VEEPROM operations */
#define WRITE                   (1U)
#define READ                    (2U)
#define FORMAT                  (3U)
#define STATUS                  (4U)
#define REFRESH                 (5U)

/* Macros for the Write/Read operations */
#define WRITE_READ_ID_1         (1U)
#define WRITE_READ_ID_2         (2U)
#define WRITE_READ_ID_3         (3U)
#define WRITE_READ_ID_4         (4U)

/* Macro for the default ID */
#define DEFAULT_ID              (65535U)

/* Structure variable */
typedef struct st_adxlsensor
{
    uint8_t x_data;
    uint8_t y_data;
    uint8_t z_data;
    uint8_t xy_data;
} adxl_t;

/* Function declarations */
fsp_err_t vee_flash_init(void);
fsp_err_t vee_format(void);
fsp_err_t vee_status_get(rm_vee_status_t * const p_status);
fsp_err_t vee_refresh(void);
fsp_err_t vee_write(rm_vee_ctrl_t * const p_api_ctrl, uint32_t const rec_id,
                    void const * const p_rec_data, uint32_t const num_bytes);
fsp_err_t vee_read(rm_vee_ctrl_t * const p_api_ctrl, uint32_t const rec_id,
                   uint8_t ** const pp_rec_data, uint32_t * const p_num_bytes);
void vee_flash_deinit(void);

#endif /* VEE_FLASH_EP_H_ */
