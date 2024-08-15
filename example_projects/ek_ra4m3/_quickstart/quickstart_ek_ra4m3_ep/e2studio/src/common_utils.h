/***********************************************************************************************************************
 * File Name    : common_utils.h
 * Description  : Contains macros, data structures and functions used  common to the EP
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/* generic headers */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hal_data.h"

#ifndef COMMON_UTILS_H_
#define COMMON_UTILS_H_

/* SEGGER RTT and error related headers */

#define BIT_SHIFT_8  (8u)
#define SIZE_64      (64u)

#define LVL_ERR      (1u)       /* error conditions   */

#define RESET_VALUE             (0x00)

#define BUFFER_LINE_LENGTH (512)

/*
 * Typedefs
 * Other typedefs are defined in <stdint.h>
 */
typedef unsigned int bool_t;
typedef int ( * test_fn)(void);
typedef char char_t;

typedef float float32_t;
typedef double float64_t;
typedef long double float128_t;

#endif /* COMMON_UTILS_H_ */
