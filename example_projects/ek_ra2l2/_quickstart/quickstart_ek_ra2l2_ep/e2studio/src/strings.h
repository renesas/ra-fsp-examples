/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : strings.h
 * Version      : 1.00
 * Description  : Text data for  EK-RA2L2 Quick Start Example Project
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 01.05.2025 1.00     First Release
 *********************************************************************************************************************/

#ifndef STRINGS_H_
#define STRINGS_H_

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "board_info.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define STR_WELCOME             "\r\n Welcome to Quick Start Example Project for "\
                                KIT_NAME\
                                "! \r\n"\
                                "\r\n > Select from the options in the menu below: \r\n"\
                                "\r\n MENU \r\n"\
                                " 1. Kit Information \r\n"\
                                " 2. Next Steps \r\n"
#define STR_KITINFO             "\r\n 1. KIT INFORMATION \r\n"\
                                "\r\n a) Kit name: \t\t\t\t"\
                                KIT_NAME\
                                "\r\n b) Kit ordering part number:\t\t"\
                                KIT_PARTNUM\
                                "\r\n c) RA Device part number:   \t\t"\
                                DEVICE_NUMBER
#define STR_NEXTSTEPS           "\r\n 2. NEXT STEPS \r\n"\
                                "\r\n Visit the following URLs to learn about the kit "\
                                "and the RA family \r\n of MCUs, download tools "\
                                "and documentation, and get support \r\n"\
                                "\r\n a) "\
                                KIT_NAME\
                                " resources: \t"\
                                KIT_LANDING_URL\
                                "\r\n b) RA kits information:     \t"\
                                KIT_RA_URL\
                                "\r\n c) RA product information:  \t"\
                                PRODUCT_INFO_URL\
                                "\r\n d) RA product support forum: \t"\
                                PRODUCT_SUPPORT_URL\
                                "\r\n e) Renesas support: \t\t"\
                                RENESAS_SUPPORT_URL\
                                "\r\n\r\n > Press space bar to return to MENU\r\n"
#define STR_MCU_ID              "\r\n d) RA MCU 128-bit Unique ID (hex): "
#define STR_MCU_TEMP            "\r\n e) RA MCU Die temperature (F/C):  "
#define STR_LED_FREQ            "\r\n f) Blue LED blinking frequency (Hz): "
#define STR_LED_INTENSE         "\r\n g) Blue LED blinking intensity (%): "
#define STR_RETURN              "\r\n\r\n > Press space bar to return to MENU\r\n"

#endif /* STRINGS_H_ */
