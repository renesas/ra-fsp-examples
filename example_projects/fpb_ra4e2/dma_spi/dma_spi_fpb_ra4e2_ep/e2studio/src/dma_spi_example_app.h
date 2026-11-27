/***********************************************************************************************************************
 * File Name    : dma_spi_example_app.h
 * Description  : Public interface for the DMA SPI example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef DMA_SPI_EXAMPLE_APP_H_
#define DMA_SPI_EXAMPLE_APP_H_

#include "spi_ep.h"

#define EP_VERSION          "1.0"

#define BANNER_INFO         "\r\n******************************************************************"\
		                    "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module "MODULE_PADDING" *"\
							"\r\n*   Example Project Version %s                                  *"\
							"\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
							"\r\n******************************************************************"\
							"\r\nRefer to readme.txt file for more details on Example Project and"\
							"\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO             "\r\nThe example project demonstrates the typical use of the SPI HAL module APIs"\
		                    "\r\nusing SPI and DMAC modules. The project configures DMA support SPI channels"\
							"\r\n(Channel 0 and Channel 1) in Master and Slave modes. Once the modules are"\
							"\r\ninitialized and the SPI channels are configured, Master and Slave can transmit"\
							"\r\nand receive data based on commands from the user sent through the terminal."\
							"\r\nSPI data is transmitted and received via the DMAC module.\r\n"

#define EP_MENU             "\r\n\r\nMain menu:"\
		                    "\r\nPress 1 for Write and Read operations"\
                            "\r\nPress 2 for WriteRead operation"\
                            "\r\nPress 3 for Exit operation"\
                            "\r\nUser input: "

#define ERR_INFO            "\r\nError in configuration or connection"\
                            "\r\nReset the MCU...\r\n"

/* Single entry-point called in hal_entry.c */
void dma_spi_example_app_run(void);

#endif /* DMA_SPI_EXAMPLE_APP_H_ */
