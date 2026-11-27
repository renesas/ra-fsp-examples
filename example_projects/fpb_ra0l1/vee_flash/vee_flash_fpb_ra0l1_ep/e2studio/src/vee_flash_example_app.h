/***********************************************************************************************************************
 * File Name    : vee_flash_example_app.h
 * Description  : Public interface for the VEE FLASH example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef VEE_FLASH_EXAMPLE_APP_H_
#define VEE_FLASH_EXAMPLE_APP_H_

#include "vee_flash_ep.h"

#define EP_VERSION          "1.0"

#define MODULE_NAME         "vee_flash"

#define BANNER_INFO         "\r\n******************************************************************"\
                            "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module             *"\
                            "\r\n*   Example Project Version %s                                  *"\
                            "\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
                            "\r\n******************************************************************"\
                            "\r\nRefer to readme.txt file for more details on Example Project and"\
                            "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO             "\r\nThis example project demonstrates basic functionalities of Virtual EEPROM (VEEPROM)"\
                            "\r\non Flash of Renesas RA MCUs based on Renesas FSP. Based on the user input, the EP"\
                            "\r\nperforms the following operations:"\
                            "\r\n1. Writing different data types to the different RECORD_IDs of VEEPROM."\
                            "\r\n2. Reading back the data from the different RECORD_IDs."\
                            "\r\n3. Format operation."\
                            "\r\n4. Get the status of VEEPROM."\
                            "\r\n5. Refresh operation."\
                            "\r\nInformation, success, and error messages will be printed on the terminal.\r\n"

/* Main menu */
#define EP_MENU             "\r\nMain Menu:"\
                            "\r\n1. Enter 1 for write operation"\
                            "\r\n2. Enter 2 for read operation"\
                            "\r\n3. Enter 3 for format operation (Format should only be"\
                            "\r\n   used to erase the entire data flash used by VEEPROM)"\
                            "\r\n4. Enter 4 for get status operation"\
                            "\r\n5. Enter 5 for refresh operation"\
                            "\r\nUser input: "

/* Menu for write operations */
#define WRITE_MENU          "\r\n\r\nWrite Operations Menu:"\
                            "\r\n1. Enter 1 to write fixed integer data to RECORD_ID1"\
                            "\r\n2. Enter 2 to write fixed char data to RECORD_ID2"\
                            "\r\n3. Enter 3 to write fixed float data to RECORD_ID3"\
                            "\r\n4. Enter 4 to write fixed structure data to RECORD_ID4"\
                            "\r\nUser input: "

/* Menu for read operations */
#define READ_MENU           "\r\n\r\nRead Operations Menu:"\
                            "\r\n1. Enter 1 to read most recent data associated with RECORD_ID1"\
                            "\r\n2. Enter 2 to read most recent data associated with RECORD_ID2"\
                            "\r\n3. Enter 3 to read most recent data associated with RECORD_ID3"\
                            "\r\n4. Enter 4 to read most recent data associated with RECORD_ID4"\
                            "\r\nUser input: "

/* Single entry-point called in hal_entry.c */
void vee_flash_example_app_run(void);

#endif /* VEE_FLASH_EXAMPLE_APP_H_ */
