/***********************************************************************************************************************
 * File Name    : sdram_example_app.h
 * Description  : Public interface for the SDRAM example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef SDRAM_EXAMPLE_APP_H_
#define SDRAM_EXAMPLE_APP_H_

#define EP_VERSION      "1.0"

#define MODULE_NAME     "SDRAM"

#define BANNER_INFO     "\r\n******************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                 *"\
                        "\r\n*   Example Project Version %s                                  *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
                        "\r\n******************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" BSP\r\n"

#define EP_INFO         "\r\nThe example project demonstrates the basic functionalities of SDRAM on Renesas RA MCUs"\
                        "\r\nbased on Renesas FSP. This example project transfers data to the entire SDRAM using DMAC."\
                        "\r\nIt demonstrates SDRAM settings and data retention by enabling self-refresh in Low Power"\
                        "\r\nMode (LPM). When the Ultra-Low-Power Timer (ULPT) expires, the MCU returns to normal mode"\
                        "\r\nand reads the SDRAM to ensure the data is still retained. The EP information and error"\
                        "\r\nmessages are displayed on the terminal. LED1 is turned ON for 1 second before the MCU"\
                        "\r\nenters LPM, then turned OFF. Additionally, LED3 is turned ON if an error occurs.\r\n\r\n"

/* Single entry-point called in hal_entry.c */
void sdram_example_app_run(void);

#endif /* SDRAM_EXAMPLE_APP_H_ */
