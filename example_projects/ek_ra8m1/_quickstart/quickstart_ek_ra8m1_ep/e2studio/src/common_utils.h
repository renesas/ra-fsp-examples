/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/**********************************************************************************************************************
 * File Name    : common_utils.h
 * Description  : Contains macros, data structures and functions used  common to the EP
 *********************************************************************************************************************/

/* generic headers */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hal_data.h"
#include "r_typedefs.h"

#ifndef COMMON_UTILS_H_
#define COMMON_UTILS_H_

/* SEGGER RTT and error related headers */

#define BIT_SHIFT_8  (8u)
#define SIZE_64      (64u)

#define LVL_ERR      (1u)       /* error conditions   */

#define RESET_VALUE             (0x00)

#define BUFFER_LINE_LENGTH (1024)

/*
 * Use the define below to enable debug message through console
 * that show which files from the embedded file system are loaded
 */


#endif /* COMMON_UTILS_H_ */
