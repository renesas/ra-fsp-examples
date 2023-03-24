/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : common_utils.h
 * Description  : Contains macros, data structures and functions used  common to the EP
 *********************************************************************************************************************/

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
