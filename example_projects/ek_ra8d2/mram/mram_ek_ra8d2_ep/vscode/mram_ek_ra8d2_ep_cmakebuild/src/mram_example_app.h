/***********************************************************************************************************************
 * File Name    : mram_example_app.h
 * Description  : Public interface for the MRAM example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef MRAM_EXAMPLE_APP_H_
#define MRAM_EXAMPLE_APP_H_

#define EP_VERSION      "1.0"

#define MODULE_NAME     "r_mram"

#define BANNER_INFO     "\r\n******************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                *"\
                        "\r\n*   Example Project Version %s                                  *"\
                        "\r\n*   Flex Software Pack Version  %d.%d.%d                            *"\
                        "\r\n******************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO         "\r\nThe example project demonstrates the basic functionality of the MRAM"\
                        "\r\ndriver on Renesas RA MCUs using the Renesas Flexible Software Package (FSP)."\
                        "\r\nThe user can interact with the application via the terminal to execute write,"\
                        "\r\nread, and erase operations, starting at a fixed MRAM address. The operation"\
                        "\r\nstatus and data content are displayed on the terminal."

#define MENU            "\r\n\r\nChoose an option from the menu below:\r\n"\
                        "Press 1 to WRITE to MRAM\r\n"\
                        "Press 2 to READ from MRAM\r\n"\
                        "Press 3 to ERASE MRAM\r\n"\
                        "Press 4 to EXIT"

#define EP_GUIDE        "\r\nUser input: "

/* Single entry-point called in hal_entry.c */
void mram_example_app_run(void);

#endif /* MRAM_EXAMPLE_APP_H_ */
