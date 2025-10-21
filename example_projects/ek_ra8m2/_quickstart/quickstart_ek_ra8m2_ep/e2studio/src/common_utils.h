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
 *         : 14/10/2025 1.00     First Release
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

#define CONNECTION_ABORT_CRTL  (0x00)
#define MENU_EXIT_CRTL         (0x20)

/* MACRO encapsulate enable as default action */
#define ENABLED_AS_DEFAULT  (true)

/* MACRO encapsulate disable as default action */
#define DISABLED_AS_DEFAULT (false)

/* Version Revision History */
#define VERSION_STR            ("Version 0.1.")

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/

/* Data structure passed to an input driver to fill */


/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/
extern const char_t * const gp_cursor_store;
extern const char_t * const gp_cursor_restore;
extern const char_t * const gp_cursor_temp;
extern const char_t * const gp_cursor_frequency;
extern const char_t * const gp_cursor_intensity;

extern const char_t * const gp_red_fg;
extern const char_t * const gp_orange_fg;
extern const char_t * const gp_green_fg;
extern const char_t * const gp_white_fg;

extern const char_t * const gp_clear_screen;
extern const char_t * const gp_cursor_home;

extern const char_t * const gp_hide_cursor;
extern const char_t * const gp_show_cursor;

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/
extern bool_t tolowerstr(char_t *ptr, uint32_t len);
extern bool_t toupperstr(char_t *ptr, uint32_t len);

#endif /* COMMON_UTILS_H_ */
