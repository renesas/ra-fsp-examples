/***********************************************************************************************************************
 * File Name    : crc_example_app.h
 * Description  : Public interface for the CRC example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef CRC_EXAMPLE_APP_H_
#define CRC_EXAMPLE_APP_H_

#include "hal_data.h"
#include "uart_ep.h"

#define EP_VERSION      "1.1"

#define MODULE_NAME     "r_crc"

#define BANNER_INFO     "\r\n******************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                 *"\
                        "\r\n*   Example Project Version "EP_VERSION"                                  *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
                        "\r\n******************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO         "\r\nThe example project demonstrates the typical use of the CRC HAL module APIs."\
                        "\r\n"CRC_MODE".\r\nOn pressing any key through the terminal, CRC value in normal mode"\
                        "\r\nis calculated for 4 bytes of data. The calculated CRC value and input data"\
                        "\r\nare transmitted and received using the "LOOKBACK"."\
                        "\r\n"TRANS_RESULT""\
                        "\r\nThe LED blinks only if the CRC operation is successful and stays"\
                        "\r\nON if a data mismatch occurs. Failure and status messages are displayed on"\
                        "\r\nthe terminal.\r\n"

#if (BSP_FEATURE_CRC_HAS_SNOOP)
#define CRC_MODE        "It demonstrates CRC operation for data transmission in normal mode and"\
		                "\r\nreception in snoop mode through the "INTERFACE""

#define TRANS_RESULT    "Once the transfer is complete, if the CRC value in snoop mode is zero and"\
                        "\r\nthe transmit and receive buffers are equal, the operation is successful."
#else
#define CRC_MODE        "It demonstrates CRC operation for data transmission and reception in normal"\
		                "\r\nmode through the "INTERFACE""

#define TRANS_RESULT    "Once the transfer is complete, if the CRC values match and the transmit"\
                        "\r\nand receive buffers are equal, the operation is successful."
#endif /* BSP_FEATURE_CRC_HAS_SNOOP */

#define EP_GUIDE        "\r\nPress any key for CRC operation\r\n"

/* Extern variable */
extern bsp_leds_t g_bsp_leds;

/* Single entry-point called in hal_entry.c */
void crc_example_app_run(void);

#endif /* CRC_EXAMPLE_APP_H_ */
