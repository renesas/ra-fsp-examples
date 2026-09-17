/***********************************************************************************************************************
 * File Name    : littlefs_app.h
 * Description  : Contains macros, data structures and functions used in the LittleFS configuration.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2022 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef LITTLEFS_APP_H_
#define LITTLEFS_APP_H_

#include "hal_data.h"

fsp_err_t hal_littlefs_init(void);
int configure_littlefs_flash(void);
void hal_littlefs_deinit(void);

/* Macro for flash type */
#if (BSP_PERIPHERAL_FLASH_PRESENT)
#define FLASH_TYPE  "FLASH"
#else
#define FLASH_TYPE  "SPI_FLASH"
#endif /* BSP_PERIPHERAL_FLASH_PRESENT */

#endif /* LITTLEFS_APP_H_ */
