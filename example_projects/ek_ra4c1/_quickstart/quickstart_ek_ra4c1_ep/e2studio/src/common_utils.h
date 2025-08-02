/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : common_utils.h
 * Version      : 1.0
 * Description  : Contains macros, data structures and functions used common to the EP
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 20.05.2025 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hal_data.h"
#include "r_typedefs.h"

 /**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#ifndef COMMON_UTILS_H_
#define COMMON_UTILS_H_

/* SEGGER RTT and error related headers */
#define BIT_SHIFT_8  (8u)
#define SIZE_64      (64u)

/* Error condition level to be reported   */
#define LVL_ERR      (1u)

/* Generic null value for resetting variables / configurations */
#define RESET_VALUE  (0x00)

#define PMOD_DISPLY_DELAY_SETTING_CFG (400)

/* PRINT Buffer size used throughout this application */
#define BUFFER_LINE_LENGTH  (1024)

/* Basic Console control codes  */
#define CONNECTION_ABORT_CRTL          (0x00)
#define MENU_EXIT_CRTL                 (0x20)
#define MENU_ENTER_RESPONSE_CRTL       (0x09)
#define INPUT_BUFFER                   (0x05)
#define CARRAGE_RETURN                 (0x0D)
#define BACK_SPACE                     (0x08)
#define BACK_SPACE_PUTTY               (0x7F)

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/
#endif /* COMMON_UTILS_H_ */
