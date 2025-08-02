/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/***********************************************************************************************************************
 * File Name    : common_utils.c
 * Version      : 1.0
 * Description  : Shared functions used between modules.
 **********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 01.01.2025 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "common_init.h"
#include "common_utils.h"
#include "board_hw_cfg.h"

#include <ctype.h>

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
const char_t * const gp_cursor_store     = "\x1b[s";
const char_t * const gp_cursor_restore   = "\x1b[u";
const char_t * const gp_cursor_temp      = "\x1b[8;41H\x1b[K";
const char_t * const gp_cursor_frequency = "\x1b[9;41H\x1b[K";
const char_t * const gp_cursor_intensity = "\x1b[10;41H\x1b[K";

const char_t * const gp_red_fg        = "\x1B[91m";
const char_t * const gp_orange_fg     = "\x1B[38;5;208m";
const char_t * const gp_green_fg      = "\x1b[32m";
const char_t * const gp_white_fg      = "\x1b[37m";

const char_t * const gp_clear_screen = "\x1b[2J";
const char_t * const gp_cursor_home  = "\x1b[H";

const char_t * const gp_hide_cursor = "\e[?25l";
const char_t * const gp_show_cursor = "\e[?25h";

bool_t g_overlay_selected = false;

/**********************************************************************************************************************
 * Function Name: tolowerstr
 * Description  : Convert a null terminated string to lower case.
 * Arguments    : ptr - string to convert, is replaced during conversion
 *              : len - length of string to convert
 * Return Value : always returns true in this implementation
 *********************************************************************************************************************/
bool_t tolowerstr(char_t *ptr, uint32_t len)
{
    bool_t   res = true;

    for (uint32_t i = 0; i < len; i++)
    {
        *ptr = (char_t)tolower(*ptr);
        ptr++;
    }

    return res;
}
/**********************************************************************************************************************
 End of function tolowerstr
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: toupperstr
 * Description  : Convert a null terminated string to upper case.
 * Arguments    : ptr - string to convert, is replaced during conversion
 *              : len - length of string to convert
 * Return Value : always returns true in this implementation
 *********************************************************************************************************************/
bool_t toupperstr(char_t *ptr, uint32_t len)
{
    bool_t   res = true;

    for (uint32_t i = 0; i < len; i++)
    {
        *ptr = (char_t)toupper(*ptr);
        ptr++;
    }

    return res;
}
/**********************************************************************************************************************
 End of function toupperstr
 *********************************************************************************************************************/
