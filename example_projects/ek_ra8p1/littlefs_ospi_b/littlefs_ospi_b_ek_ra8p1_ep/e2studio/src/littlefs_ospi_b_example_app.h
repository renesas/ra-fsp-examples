/***********************************************************************************************************************
 * File Name    : littlefs_ospi_example_app.h
 * Description  : Public interface for LittleFS OSPI_B example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef LITTLEFS_OSPI_B_EXAMPLE_APP_H_
#define LITTLEFS_OSPI_B_EXAMPLE_APP_H_

#define EP_VERSION          "1.0"

#define MODULE_NAME         "littlefs_spi_flash"

#define BANNER_INFO         "\r\n******************************************************************"\
                            "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module    *"\
                            "\r\n*   Example Project Version %s                                  *"\
                            "\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
                            "\r\n******************************************************************"\
                            "\r\nRefer to readme.txt file for more details on Example Project and"\
                            "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO             "\r\nThis example project demonstrates basic functionalities of LittleFS for external"\
                            "\r\nflash (e.g., SPI, OSPI, QSPI) on RA MCUs based on FSP. The project will perform"\
                            "\r\nvarious file system operations based on the user's selection from menu options,"\
                            "\r\nsuch as erase entire flash, erase 32 KB flash, format file system, directory"\
                            "\r\nmanagement (create, get properties, delete), and file management (create, write,"\
                            "\r\nread, delete). The terminal application is used as the user interface. The menu"\
                            "\r\noptions and system messages (errors and information messages) will be printed"\
                            "\r\non the terminal application during the execution of the project.\r\n"

#define MAIN_MENU           "\r\n\r\nMain menu:"\
                            "\r\n1. Flash operations."\
                            "\r\n2. Directory operations."\
                            "\r\n3. File operations."\
                            "\r\nUser input: "

#define FLASH_MENU          "\r\n\r\nMain menu > Flash operations:"\
                            "\r\n1. Erase the entire flash."\
                            "\r\n2. Erase the first 32 KB of the flash."\
                            "\r\n3. Format the file system."\
                            "\r\n4. Go back to the main menu."\
                            "\r\nUser input: "

#define DIR_MENU            "\r\n\r\nMain menu > Directory operations:"\
                            "\r\n1. Create the directory."\
                            "\r\n2. Get the directory properties."\
                            "\r\n3. Delete the directory."\
                            "\r\n4. Go back to the main menu."\
                            "\r\nUser input: "

#define FILE_MENU           "\r\n\r\nMain menu > File operations:"\
                            "\r\n1. Open the file."\
                            "\r\n2. Write to the file."\
                            "\r\n3. Read from the file."\
                            "\r\n4. Delete the file."\
                            "\r\n5. Go back to the main menu."\
                            "\r\nUser input: "

#define DIR_NAME_ONE        "dir_one"
#define FILE_NAME_ONE       "file_one.txt"
#define TYPE_DIR            "LittleFS type directory"
#define WRITE_LINE_TEXT     "This example project demonstrates the operation of the LittleFS for"\
                            " external flash of the RA MCU\r\n"

/* Single entry-point called in hal_entry.c */
void littlefs_ospi_b_example_app_run(void);

#endif /* LITTLEFS_OSPI_B_EXAMPLE_APP_H_ */
