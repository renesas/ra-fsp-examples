/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/ 
/**********************************************************************************************************************
 * File Name    : menu_kis.h
 * Description  : Main menu information
 *********************************************************************************************************************/

#ifndef MENU_KIS_H_
#define MENU_KIS_H_

#define MODULE_NAME_VERSION      "\r\n%d. GET FSP VERSION\r\n\r\n"
#define MODULE_NAME_UUID         "\r\n%d. GET UUID\r\n\r\n"
#define MODULE_NAME_HELP         "\r\n%d. HELP\r\n\r\n"
#define MODULE_START_APP         "\r\n%d. STARTING APPLICATION\r\n\r\n"
#define UUID_TAG                 "   RA MCU 128-bit Unique ID (hex) : "
#define HELP_TAG1                "  Please type 1 for FSP version details\r\n"
#define HELP_TAG2                "  Please type 2 for flash memory related operations\r\n"
#define HELP_TAG3                "  Please type 3 for UUID\r\n"
#define HELP_TAG4                "  Please type 4 to configure Wi-Fi\r\n"
#define HELP_TAG5                "  Please type 5 to start application\r\n"

#define CONNECTION_ABORT_CRTL    (0x00)
#define MENU_EXIT_CRTL           (0x20)

extern test_fn kis_display_menu (void);
test_fn get_version(void);
test_fn get_uuid (void);
test_fn help (void);
test_fn start_app (void);

#endif /* MENU_KIS_H_ */
