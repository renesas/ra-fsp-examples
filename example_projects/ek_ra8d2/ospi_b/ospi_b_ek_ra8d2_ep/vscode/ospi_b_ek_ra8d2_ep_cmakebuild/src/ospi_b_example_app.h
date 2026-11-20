/***********************************************************************************************************************
 * File Name    : ospi_b_example_app.h
 * Description  : Public interface for the OSPI_B example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef OSPI_B_EXAMPLE_APP_H_
#define OSPI_B_EXAMPLE_APP_H_

#define EP_VERSION      "1.0"

#define MODULE_NAME     "ospi_b"

#define BANNER_INFO     "\r\n******************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                *"\
                        "\r\n*   Example Project Version "EP_VERSION"                                  *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
                        "\r\n******************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO         "\r\nThis example project demonstrates basic functionalities of OSPI driver on"\
                        "\r\nRenesas RA MCUs based on Renesas FSP. EP performs read/write/erase operation"\
						"\r\nin SPI mode (1S-1S-1S) or DOPI mode (8D-8D-8D) based on the user input."\
						"\r\nThe information and error messages are displayed on the terminal.\r\n"

#define EP_MENU         "\r\nProtocol Mode Menu:"\
                        "\r\n1. SPI 1S-1S-1S Protocol Mode"\
						"\r\n2. OPI 8D-8D-8D Protocol Mode"\
						"\r\nUser input: "

#define SUB_MENU        "\r\n\r\n%s Operation Menu:"\
                        "\r\n1. Write Operation"\
						"\r\n2. Read Operation"\
						"\r\n3. Erase Operation"\
						"\r\n4. Back To Main menu"\
						"\r\nUser input: "

/* Single entry-point called in hal_entry.c */
void ospi_b_example_app_run(void);

#endif /* OSPI_B_EXAMPLE_APP_H_ */
