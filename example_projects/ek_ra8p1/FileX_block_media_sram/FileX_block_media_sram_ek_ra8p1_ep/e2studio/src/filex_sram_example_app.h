/***********************************************************************************************************************
 * File Name    : filex_sram_example_app.h
 * Description  : Public interface for the FileX Block Media SRAM example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef FILEX_SRAM_EXAMPLE_APP_H_
#define FILEX_SRAM_EXAMPLE_APP_H_

#include "filex_thread.h"

#define EP_VERSION      "1.0"

#define MODULE_NAME     "FileX Block Media SRAM"

#define BANNER_INFO     "\r\n********************************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module              *"\
                        "\r\n*   Example Project Version "EP_VERSION"                                                *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                                           *"\
                        "\r\n********************************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" module\r\n"

#define EP_INFO         "\r\nThe example project demonstrates how the FileX file system operates on the"\
		                "\r\ninternal SRAM block media of the RA MCU. The project will perform various"\
		                "\r\nFileX file system operations based on the user's selection from the menu"\
		                "\r\noptions, such as media management (format), directory management (create,"\
		                "\r\nget properties, delete), and file management (create, write, read, delete)."\
		                "\r\nThe terminal application is used as the user interface. Errors and"\
		                "\r\ninformation messages will be printed on the terminal application."

#define EP_MENU         "\r\n\r\nMain menu:"\
                        "\r\n1.Format media."\
                        "\r\n2.Create directory."\
                        "\r\n3.Get directory properties."\
                        "\r\n4.Delete directory."\
                        "\r\n5.Create empty file."\
                        "\r\n6.Write to file."\
                        "\r\n7.Read from file."\
                        "\r\n8.Delete file."\
                        "\r\nUser input: "

/* Single entry-point called in filex_thread_entry.c */
void filex_sram_example_app_run(void);

#endif /* FILEX_SRAM_EXAMPLE_APP_H_ */
