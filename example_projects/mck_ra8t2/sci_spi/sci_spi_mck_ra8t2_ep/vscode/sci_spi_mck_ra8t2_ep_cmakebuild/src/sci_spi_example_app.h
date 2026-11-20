/***********************************************************************************************************************
 * File Name    : sci_spi_example_app.h
 * Description  : Public interface for the SCI SPI example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef SCI_SPI_EXAMPLE_APP_H_
#define SCI_SPI_EXAMPLE_APP_H_

#include "sci_spi_ep.h"

#define EP_VERSION          "1.0"

#define BANNER_INFO         "\r\n******************************************************************"\
                            "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module"MODULE_PADDING"         *"\
							"\r\n*   Example Project Version %s                                  *"\
							"\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
							"\r\n******************************************************************"\
							"\r\nRefer to readme.txt file for more details on Example Project and"\
                            "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"


#define EP_INFO             "\r\nThis example project demonstrates the basic usage of SCI SPI driver."\
                            "\r\nThe project writes the commands to the slave sensor and reads the"\
                            "\r\ntemperature value using SCI SPI bus configured as master. The temperature"\
                            "\r\nvalues are continuously printed on the terminal.\r\n"

/* Single entry-point called in hal_entry.c */
void sci_spi_example_app_run(void);

#endif /* SCI_SPI_EXAMPLE_APP_H_ */
