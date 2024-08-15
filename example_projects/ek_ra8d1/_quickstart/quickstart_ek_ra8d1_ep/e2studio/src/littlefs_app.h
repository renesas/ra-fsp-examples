/***********************************************************************************************************************
 * File Name    : littlefs_app.h
 * Description  : Contains macros, data structures and functions used in the littlefs configuration
 ***********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef LITTLEFS_APP_H_
#define LITTLEFS_APP_H_

#include "hal_data.h"

fsp_err_t hal_littlefs_init(void);
fsp_err_t configure_littlefs_flash(void);
void      hal_littlefs_deinit(void);

#endif                                 /* LITTLEFS_APP_H_ */
