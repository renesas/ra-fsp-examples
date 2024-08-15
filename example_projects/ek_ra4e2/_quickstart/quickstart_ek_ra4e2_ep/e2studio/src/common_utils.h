/**********************************************************************************************************************
 * File Name    : common_utils.h
 * Description  : Contains macros, data structures and functions used  common to the EP
 *********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef COMMON_UTILS_H_
#define COMMON_UTILS_H_

/* generic headers */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hal_data.h"
#include "r_typedefs.h"

#define BUFFER_LINE_LENGTH (512)

/* SEGGER RTT and error related headers */
#define BIT_SHIFT_8  (8u)
#define SIZE_64      (64u)

#define LVL_ERR      (1u)       /* error conditions   */

#define SHORT_NAME      "RA4E2"
#define FULL_NAME       ("EK-RA4E2")
#define PART_NUMBER     ("RTK7EKA4E2S00001BE")
#define DEVICE_NUMBER   ("R7FA4E2B93CFM")

#define RESET_VALUE     (0x00)
#define KIT_NAME        "EK-RA4E2"
#define EP_VERSION      ("2.00")

/* Terminal window common display sequences */
extern const char_t * const gp_cursor_temp;
extern const char_t * const gp_cursor_frequency;
extern const char_t * const gp_cursor_intensity;

/* Terminal window generic escape sequences */
extern const char_t * const gp_green_fg;
extern const char_t * const gp_white_fg;
extern const char_t * const gp_clear_screen;
extern const char_t * const gp_cursor_home;
extern const char_t * const gp_cursor_store;
extern const char_t * const gp_cursor_restore;


#endif /* COMMON_UTILS_H_ */
