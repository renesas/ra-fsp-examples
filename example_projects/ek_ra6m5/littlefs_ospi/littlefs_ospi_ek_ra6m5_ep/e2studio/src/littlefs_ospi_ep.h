/***********************************************************************************************************************
 * File Name    : littlefs_ospi_ep.h
 * Description  : Contains macros data structures and functions used in littlefs_ospi_ep.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef LITTLEFS_OSPI_EP_H_
#define LITTLEFS_OSPI_EP_H_

#include <common_utils.h>
#include "board_ospi.h"
#include "lfs.h"

#define EP_VERSION              ("1.0")
#define MODULE_NAME             "littlefs_spi_flash"
#define BANNER_INFO             "\r\n******************************************************************"\
                                "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module    *"\
								"\r\n*   Example Project Version %s                                  *"\
								"\r\n*   Flex Software Pack Version  %d.%d.%d                            *"\
								"\r\n******************************************************************"\
								"\r\nRefer to readme.txt file for more details on Example Project and" \
								"\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define DIR_NAME_ONE            "dir_one"
#define FILE_NAME_ONE           "file_one.txt"
#define TYPE_DIR                "LittleFS type directory"
#define WRITE_LINE_TEXT         "This example project demonstrates the operation of the LittleFS for"\
                                " external flash of the RA MCU\r\n"

#define WRITE_BUFFER_SIZE       (10240U)
#define READ_BUFFER_SIZE        (10240U)
#define PRINT_BUFFER_SIZE       (1024U)
#define ONE_BYTE                (1U)
#define SEEK_VALUE              (0U)
#define WRITE_LINE_SIZE         (strlen(WRITE_LINE_TEXT))

#if (USE_VIRTUAL_COM == 1)
#define BUFFER_SIZE_DOWN                (16)
#define ERR_LED                         (0U)
#endif /* USE_VIRTUAL_COM */

#define SELECT_FLASH_OPERATION          (1U)
#define SELECT_DIR_OPERATION            (2U)
#define SELECT_FILE_OPERATION           (3U)

#define SELECT_ERASE_ENTIRE_FLASH       (1U)
#define SELECT_ERASE_32KB_FLASH         (2U)
#define SELECT_FORMAT_FILE_SYSTEM       (3U)
#define SELECT_FLASH_GO_BACK            (4U)

#define SELECT_CREATE_DIR               (1U)
#define SELECT_GET_DIR_PROPERTY         (2U)
#define SELECT_DELETE_DIR               (3U)
#define SELECT_DIR_GO_BACK              (4U)

#define SELECT_OPEN_FILE                (1U)
#define SELECT_WRITE_FILE               (2U)
#define SELECT_READ_FILE                (3U)
#define SELECT_DELETE_FILE              (4U)
#define SELECT_FILE_GO_BACK             (5U)

#define EP_INFO     "\r\nThis example project demonstrates basic functionalities of LittleFS for external"\
                    "\r\nflash (e.g., SPI, OSPI, QSPI) on RA MCUs based on FSP. The project will perform"\
                    "\r\nvarious file system operations based on the user's selection from menu options,"\
                    "\r\nsuch as erase entire flash, erase 32 KB flash, format file system, directory"\
                    "\r\nmanagement (create, get properties, delete), and file management (create, write,"\
                    "\r\nread, delete). The terminal application is used as the user interface. The menu"\
                    "\r\noptions and system messages (errors and information messages) will be printed"\
                    "\r\non the terminal application during the execution of the project.\r\n"

/* Macro for the menus */
#define MAIN_MENU   "\r\n\r\nMain Menu:"\
                    "\r\n  1. Flash Operations."\
                    "\r\n  2. Directory Operations."\
                    "\r\n  3. File Operations."\
                    "\r\n  User Input:  "

#define FLASH_MENU  "\r\n\r\nMain Menu > Flash Operations:"\
                    "\r\n  1. Erase the entire Flash."\
                    "\r\n  2. Erase the first 32 KB of the Flash."\
                    "\r\n  3. Format the File System."\
                    "\r\n  4. Go back to the Main Menu."\
                    "\r\n  User Input:  "

#define DIR_MENU    "\r\n\r\nMain Menu > Directory Operations:"\
                    "\r\n  1. Create the Directory."\
                    "\r\n  2. Get the Directory Properties."\
                    "\r\n  3. Delete the Directory."\
                    "\r\n  4. Go back to the Main Menu."\
                    "\r\n  User Input:  "

#define FILE_MENU   "\r\n\r\nMain Menu > File Operations:"\
                    "\r\n  1. Open the File."\
                    "\r\n  2. Write to the File."\
                    "\r\n  3. Read from the File."\
                    "\r\n  4. Delete the File."\
                    "\r\n  5. Go back to the Main Menu."\
                    "\r\nUser Input:  "

/* Enumerate for user requests */
typedef enum e_request_event
{
    REQUEST_ERASE_ENTIRE_OSPI = 1,
    REQUEST_ERASE_32KB_OSPI,
    REQUEST_FILE_SYSTEM_FORMAT,
    REQUEST_DIR_CREATE,
    REQUEST_DIR_PROPERTY,
    REQUEST_DIR_DELETE,
    REQUEST_FILE_OPEN,
    REQUEST_FILE_WRITE,
    REQUEST_FILE_READ,
    REQUEST_FILE_DELETE,
} request_event_t;

/* Enumerate for the menus */
typedef enum e_menu
{
    MENU_MAIN = 0,
    MENU_FLASH,
    MENU_DIR,
    MENU_FILE,
    MENU_MAX
} menu_t;

/***********************************************************************************************************************
 * Exported global functions (to be accessed by other files)
 **********************************************************************************************************************/
/***********************************************************************************************************************
 *  Function Name: littlefs_ospi_entry
 *  Description  : This function starts the LittleFS for OSPI flash operation.
 *  Arguments    : None
 *  Return Value : None
 **********************************************************************************************************************/
void littlefs_ospi_entry (void);

#endif /* LITTLEFS_OSPI_EP_H_ */
