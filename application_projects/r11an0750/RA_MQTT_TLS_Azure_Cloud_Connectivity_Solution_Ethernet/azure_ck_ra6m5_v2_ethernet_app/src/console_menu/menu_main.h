/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : menu_main.h
 * Description  : The main menu controller.
 *********************************************************************************************************************/
#include <console_menu/menu_flash.h>
#include "common_utils.h"

#ifndef MENU_MAIN_H_
#define MENU_MAIN_H_

#define SUB_OPTIONS     "\r\n> Select from the options in the menu below:\r\n" \
                        "\r\nMENU"

extern int8_t main_display_menu (void);

#endif /* MENU_MAIN_H_ */
