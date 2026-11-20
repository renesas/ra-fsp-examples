/***********************************************************************************************************************
 * File Name    : terminal_example_app.h
 * Description  : Contains macros and functions used in terminal_example_app.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ***********************************************************************************************************************/

#ifndef TERMINAL_EXAMPLE_APP_H_
#define TERMINAL_EXAMPLE_APP_H_

#include "filex_ep.h"

#define EP_VERSION       "1.0"

#define MODULE_NAME      "FileX block media ospi LevelX"

#define BANNER_INFO      "\r\n********************************************************************************"\
                         "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module       *"\
                         "\r\n*   Example Project Version %s                                                *"\
                         "\r\n*   Flex Software Pack Version %d.%d.%d                                           *"\
                         "\r\n********************************************************************************"\
                         "\r\nRefer to readme.txt file for more details on Example Project and"\
                         "\r\nFSP User's Manual for more information about "MODULE_NAME" module\r\n"

#define EP_INFO          "\r\nThis example project demonstrates the operation of the FileX file system,"\
                         "\r\nincorporating LevelX wear leveling features on the OSPI block media of the"\
                         "\r\nRA MCU. Based on the user's input in terminal, EP performs various FileX"\
                         "\r\nfile system operations, such as erase OSPI Flash, media management (open,"\
                         "\r\nformat), directory management (create, get properties, delete), file management"\
                         "\r\n(create, write, read, delete), and sector level operation (defragment, write,"\
                         "\r\nread, erase). The terminal application is used as the user interface. Errors"\
                         "\r\nand information messages will be printed on the terminal application.\r\n"\

#define MAIN_MENU        "\r\n\r\nMain Menu:"\
                         "\r\n1. Flash Media operation."\
                         "\r\n2. Directory operation."\
                         "\r\n3. File operation."\
                         "\r\n4. Sector operation."\
                         "\r\nUser input: "

#define FLASH_MEDIA_MENU "\r\n\r\n  Main Menu > Flash Media operation:"\
                         "\r\n  1. Erase entire Flash."\
                         "\r\n  2. Format Media."\
                         "\r\n  3. Open Media."\
                         "\r\n  4. Defragment the Flash."\
                         "\r\n  5. Go back main menu"\
                         "\r\n  User input: "

#define DIRECTORY_MENU   "\r\n\r\n  Main Menu > Directory operation:"\
                         "\r\n  1. Create Directory."\
                         "\r\n  2. Get Directory Properties."\
                         "\r\n  3. Delete Directory."\
                         "\r\n  4. Go back Main Menu."\
                         "\r\n  User input: "

#define FILE_MENU        "\r\n\r\n  Main Menu > File operation:"\
                         "\r\n  1. Create Empty File."\
                         "\r\n  2. Write To File."\
                         "\r\n  3. Read From File."\
                         "\r\n  4. Delete File."\
                         "\r\n  5. Go back Main Menu."\
                         "\r\n  User input: "

#define SECTOR_MENU      "\r\n\r\n  Main Menu > Sector operation:"\
                         "\r\n  1. Write to Sector."\
                         "\r\n  2. Read from Sector."\
                         "\r\n  3. Erase Sector."\
                         "\r\n  4. Go back Main Menu."\
                         "\r\n  User input: "

/* Function declarations */
void terminal_thread_entry_run(void);
UINT terminal_output_handle(void);
UINT terminal_input_handle(void);

#endif /* TERMINAL_EXAMPLE_APP_H_ */
