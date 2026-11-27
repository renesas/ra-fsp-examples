/***********************************************************************************************************************
 * File Name    : sau_spi_master_example_app.h
 * Description  : Public interface for the SAU SPI MASTER example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef SAU_SPI_MASTER_EXAMPLE_APP_H_
#define SAU_SPI_MASTER_EXAMPLE_APP_H_

#include "sau_spi_master_ep.h"

#define EP_VERSION      "1.1"

#define MODULE_NAME     "r_sau_spi_master"

#define BANNER_INFO     "\r\n******************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module      *"\
                        "\r\n*   Example Project Version %s                                  *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
                        "\r\n******************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO         "\r\nThis example project demonstrates the basic usage of SAU SPI driver in"\
                        "\r\nmaster configuration on Renesas RA MCUs. The project writes commands"\
                        "\r\nto the slave digital thermometer module, and reads the temperature"\
                        "\r\nvalues using SAU SPI bus configured as master. The temperature values"\
                        "\r\nare continuously printed on the terminal. Additionally, status information"\
                        "\r\nand error messages will be printed on the terminal during the execution of"\
                        "\r\nthe project.\r\n\r\n"

/* Single entry-point called in hal_entry.c */
void sau_spi_master_example_app_run(void);

#endif /* SAU_SPI_MASTER_EXAMPLE_APP_H_ */
