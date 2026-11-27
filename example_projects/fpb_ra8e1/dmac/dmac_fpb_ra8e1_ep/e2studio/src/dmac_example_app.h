/***********************************************************************************************************************
 * File Name    : dmac_example_app.h
 * Description  : Public interface for the DMAC example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef DMAC_EXAMPLE_APP_H_
#define DMAC_EXAMPLE_APP_H_

#include "hal_data.h"

#define EP_VERSION      ("1.1")

#define MODULE_NAME     "r_dmac"

#define BANNER_INFO     "\r\n******************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                *"\
                        "\r\n*   Example Project Version %s                                  *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
                        "\r\n******************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO         "\r\nThis example project demonstrates basic functionalities of DMAC driver on Renesas RA MCUs"\
                        "\r\nbased on Renesas FSP. The project initializes 3 DMAC transfer instances. Upon successful"\
						"\r\ninitialization, a menu option is displayed. When the user inputs 1, AGT generates 100ms"\
						"\r\ninterrupt to trigger a transfer from the source buffer to the port control register for"\
						"\r\n60 times indicated by LED toggling for 60 times. When the user inputs 2, 32-bit data is"\
						"\r\ntransferred from the GPT counter register to the destination array which is printed on"\
						"\r\nthe terminal. When the user inputs 3, 32-bit data is transferred from predefined source"\
						"\r\narray to the port register and the state of LED changes accordingly.\r\n"

#define EP_MENU         "\r\nPress 1 to transfer source array to LED Port in normal mode."\
                        "\r\nPress 2 to transfer the current GPT timer value."\
						"\r\nPress 3 to transfer source array to LED Port in repeat mode."\
						"\r\nUser input: "

/* Macro for null character */
#define NULL_CHAR       ('\0')

/* Macro for user input buffer */
#define BUFFER_SIZE     (16U)

/* Macro for reset value */
#define RESET_VALUE     (0x00)

/* Single entry-point called in hal_entry.c */
void dmac_example_app_run(void);

#endif /* DMAC_EXAMPLE_APP_H_ */
