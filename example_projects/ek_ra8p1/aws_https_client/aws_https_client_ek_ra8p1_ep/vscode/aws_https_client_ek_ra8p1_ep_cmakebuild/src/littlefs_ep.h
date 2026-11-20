/***********************************************************************************************************************
 * File Name    : littlefs_ep.h
 * Description  : Contains macros and functions used in the littlefs_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2022 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef LITTLEFS_EP_H_
#define LITTLEFS_EP_H_

#include "hal_data.h"

/* Macros for LittleFS based on specific flash version */
#if (BSP_PERIPHERAL_FLASH_PRESENT)
    #define LITTLEFS_OPEN(ctrl, cfg)    (RM_LITTLEFS_FLASH_Open((ctrl), (cfg)))
    #define LITTLEFS_CLOSE(ctrl)        (RM_LITTLEFS_FLASH_Close((ctrl)))
#else
    #define LITTLEFS_OPEN(ctrl, cfg)    (RM_LITTLEFS_SPI_FLASH_Open((ctrl), (cfg)))
    #define LITTLEFS_CLOSE(ctrl)        (RM_LITTLEFS_SPI_FLASH_Close((ctrl)))
#endif /* BSP_PERIPHERAL_FLASH_PRESENT */

/* Function declarations */
fsp_err_t littlefs_init(void);
void littlefs_deinit(void);

#endif /* LITTLEFS_EP_H_ */
