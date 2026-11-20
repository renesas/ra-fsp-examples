/***********************************************************************************************************************
 * File Name    : sdhi_example_app.h
 * Description  : Public interface for the SDHI example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef SDHI_EXAMPLE_APP_H_
#define SDHI_EXAMPLE_APP_H_

#define EP_VERSION      "1.0"

#define MODULE_NAME     "SDHI"

#define BANNER_INFO     "\r\n********************************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                                *"\
                        "\r\n*   Example Project Version %s                                                *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                                           *"\
                        "\r\n********************************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO         "\r\nThe example project demonstrates basic functionalities of SDHI driver with"\
                        "\r\nFreeRTOS+FAT file system on Renesas RA MCUs based on Renesas FSP. FreeRTOS+FAT"\
                        "\r\nuses the underlying Block Media driver which utilizes SDHI driver to perform"\
                        "\r\nfile operations on the SD Card (except for MCK-RA8T1, MCK-RA8T2 which use"\
                        "\r\nMicroSD Card). Information and error messages will be printed on the terminal.\r\n"

#if defined (BOARD_RA8T1_MCK) || defined (BOARD_RA8T2_MCK)
#define CARD_TYPE       "MicroSD"
#else
#define CARD_TYPE       "SD"
#endif /* BOARD_RA8T1_MCK, BOARD_RA8T2_MCK */

#define EP_NOTE         "\r\nPlease remove and re-insert the "CARD_TYPE" Card after executing the Safely remove"\
                        "\r\nor Initialize FreeRTOS+FAT option.\r\n"

#define EP_MENU         "\r\nMain menu:"\
                        "\r\n1. Write 10 KB of sample data to ra_sdhi.txt."\
                        "\r\n2. Format the "CARD_TYPE" Card device."\
                        "\r\n3. Safely remove the "CARD_TYPE" Card."\
                        "\r\n4. Display content of current working directory."\
                        "\r\n5. Initialize FreeRTOS+FAT (Valid only after Safely remove option is executed)."\
                        "\r\nUser input: "

/* Single entry-point called in sdhi_thread_entry.c */
void sdhi_example_app_run(void);

#endif /* SDHI_EXAMPLE_APP_H_ */
