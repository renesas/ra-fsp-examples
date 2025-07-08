/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
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
#include "jlink_console.h"
#include "r_typedefs.h"

#ifndef COMMON_UTILS_H_
#define COMMON_UTILS_H_

/* SEGGER RTT and error related headers */

#define BIT_SHIFT_8  (8u)
#define SIZE_64      (64u)

#define LVL_ERR      (1u)       /* error conditions   */

#define RESET_VALUE             (0x00)


#define PMOD_DISPLY_DELAY_SETTING_CFG (400)

/* Basic Console control codes  */
#define CONNECTION_ABORT_CRTL          (0x00)
#define MENU_EXIT_CRTL                 (0x20)
#define MENU_ENTER_RESPONSE_CRTL       (0x09)
#define INPUT_BUFFER                   (0x05)
#define CARRAGE_RETURN                 (0x0D)
#define BACK_SPACE                     (0x08)


#define BUFFER_LINE_LENGTH (1024)


typedef enum test_err
{
    TEST_SUCCESS = 0,
    TEST_ERROR,
    TEST_SKIP
} e_test_err_t;

#endif /* COMMON_UTILS_H_ */
