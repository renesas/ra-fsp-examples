/***********************************************************************************************************************
 * File Name    : flash_lp_example_app.h
 * Description  : Public interface for the FLASH LP example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef FLASH_LP_EXAMPLE_APP_H_
#define FLASH_LP_EXAMPLE_APP_H_

#define EP_VERSION          "1.1"

#define MODULE_NAME         "r_flash_lp"

#define BANNER_INFO         "\r\n******************************************************************"\
                            "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module            *"\
                            "\r\n*   Example Project Version "EP_VERSION"                                  *"\
                            "\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
                            "\r\n******************************************************************"\
                            "\r\nRefer to readme.txt file for more details on Example Project and"\
                            "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO             "\r\nThe example project demonstrates basic functionalities of Flash LP driver."\
                            "\r\nThis example project initializes the Flash LP and performs code/data flash"\
                            "\r\noperations (erase, blank check, write, read) on predefined block based on the"\
                            "\r\nuser input. The information and error messages are displayed on the terminal.\r\n"\

#define MAIN_MENU           "\r\nMain menu:"\
                            "\r\nPress 1 for data flash operations"\
                            "\r\nPress 2 for code flash operations"\
                            "\r\nUser input: "

#define SUB_MENU1           "\r\nValid block range for data flash operation is 0 - "DF_BLOCK_RANGE""\
		                    "\r\nEnter block number: "

#define SUB_MENU2           "\r\nValid block range for code flash operation is 10 - "CF_BLOCK_RANGE""\
		                    "\r\nEnter block number: "

#define SUB_MENU3           "\r\n************************************\r\n"\
                            "* Code Flash Access Window Example *"\
                            "\r\n************************************\r\n"\
                            "\r\nValid range for access window operation is 10 - "AW_BLOCK_RANGE": "\
                            "\r\nEnter block number: "

/* Macros for commands */
#define DATA_FLASH          (1U)
#define CODE_FLASH          (2U)

/* Macros for block ranges */
#if defined(BOARD_RA2E1_EK) || defined(BOARD_RA2L2_EK)
#define DF_BLOCK_RANGE      "3"
#define CF_BLOCK_RANGE      "63"
#define AW_BLOCK_RANGE      "62"
#elif defined(BOARD_RA2E2_EK) || defined(BOARD_RA2E3_FPB) || defined(BOARD_RA2T1_FPB)
#define DF_BLOCK_RANGE      "1"
#define CF_BLOCK_RANGE      "31"
#define AW_BLOCK_RANGE      "30"
#elif defined(BOARD_RA4W1_EK) || defined(BOARD_RA2A2_EK)
#define DF_BLOCK_RANGE      "7"
#define CF_BLOCK_RANGE      "255"
#define AW_BLOCK_RANGE      "254"
#elif defined(BOARD_RA0E1_FPB) || defined(BOARD_RA0L1_FPB)
#define DF_BLOCK_RANGE      "3"
#define CF_BLOCK_RANGE      "31"
#define AW_BLOCK_RANGE      "30"
#elif defined(BOARD_RA0E2_FPB)
#define DF_BLOCK_RANGE      "7"
#define CF_BLOCK_RANGE      "63"
#define AW_BLOCK_RANGE      "62"
#else
#define DF_BLOCK_RANGE      "7"
#define CF_BLOCK_RANGE      "127"
#define AW_BLOCK_RANGE      "126"
#endif /* Block ranges */

/* Single entry-point called in hal_entry.c */
void flash_lp_example_app_run(void);

#endif /* FLASH_LP_EXAMPLE_APP_H_ */
