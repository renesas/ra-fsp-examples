/***********************************************************************************************************************
 * File Name    : flash_hp_example_app.h
 * Description  : Public interface for the FLASH HP example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef FLASH_HP_EXAMPLE_APP_H_
#define FLASH_HP_EXAMPLE_APP_H_

#define EP_VERSION          "1.1"

#define MODULE_NAME         "r_flash_hp"

#define BANNER_INFO         "\r\n******************************************************************"\
                            "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module            *"\
                            "\r\n*   Example Project Version %s                                  *"\
                            "\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
                            "\r\n******************************************************************"\
                            "\r\nRefer to readme.txt file for more details on Example Project and"\
                            "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"


#define EP_INFO             "\r\nThe example project demonstrates basic functionalities of Flash HP driver."\
                            "\r\nThis example project initializes the Flash HP and performs Code/Data flash"\
                            "\r\noperations (erase, blank check, write, read) on predefined block based on the"\
                            "\r\nuser input. The information and error messages are displayed on the terminal.\r\n"\

#define EP_MENU             "\r\nMain menu:"\
                            "\r\nPress 1 for Code flash operations"\
                            "\r\nPress 2 for Data flash operations"\
                            "\r\nPress 3 for Exit"\
                            "\r\nUser input: "

/* Single entry-point called in hal_entry.c */
void flash_hp_example_app_run(void);

#endif /* FLASH_HP_EXAMPLE_APP_H_ */
