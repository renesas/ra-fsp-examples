/***********************************************************************************************************************
 * File Name    : sau_spi_slave_example_app.h
 * Description  : Public interface for the SAU SPI Slave example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef SAU_SPI_SLAVE_EXAMPLE_APP_H_
#define SAU_SPI_SLAVE_EXAMPLE_APP_H_

#define EP_VERSION              "1.0"

#define MODULE_NAME             "r_sau_spi_slave"

#define BANNER_INFO             "\r\n******************************************************************"\
                                "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module       *"\
                                "\r\n*   Example Project Version %s                                  *"\
                                "\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
                                "\r\n******************************************************************"\
                                "\r\nRefer to readme.txt file for more details on Example Project and"\
                                "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO                 "\r\nThis example project demonstrates the basic usage of the SAU SPI"\
                                "\r\ndriver in slave configuration on Renesas RA MCUs using Renesas FSP."\
                                "\r\nThe project follows a command protocol similar to that of the"\
                                "\r\nMAX31723 sensor. During the communication, the MCU#1, which is"\
                                "\r\nrunning the sau_spi_slave_ep project, uses an IRQ to detect the"\
                                "\r\nslave select pin signal of MCU#2, which is running the"\
                                "\r\nsau_spi_master_ep project, and MCU#1 reads commands and data sent"\
                                "\r\nfrom MCU#2 when the signal transitions from LOW to HIGH. When MCU#2"\
                                "\r\nsends a \"read data\" command, MCU#1 reads the internal die temperature"\
                                "\r\nusing the ADC and sends the temperature back to the master MCU#2."\
                                "\r\nThe temperature values are continuously printed to the terminal to"\
                                "\r\nverify correct data transmission. Additionally, status information"\
                                "\r\nand error messages are displayed on the terminal during execution.\r\n"

/* Single entry-point called in hal_entry.c */
void sau_spi_slave_example_app_run(void);

#endif /* SAU_SPI_SLAVE_EXAMPLE_APP_H_ */
