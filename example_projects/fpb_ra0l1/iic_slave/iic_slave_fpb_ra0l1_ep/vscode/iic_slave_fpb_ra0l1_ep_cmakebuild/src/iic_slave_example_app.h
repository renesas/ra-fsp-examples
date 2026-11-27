/***********************************************************************************************************************
 * File Name    : iic_slave_example_app.h
 * Description  : Public interface for the IIC Slave example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef IIC_SLAVE_EXAMPLE_APP_H_
#define IIC_SLAVE_EXAMPLE_APP_H_

#include "iic_slave_ep.h"

#define EP_VERSION      "1.1"

#define BANNER_INFO     "\r\n******************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module"MODULE_PADDING"        *"\
                        "\r\n*   Example Project Version "EP_VERSION"                                  *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
                        "\r\n******************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO         "\r\nThe example project demonstrates the typical use of the IIC Slave HAL APIs."\
						"\r\nEP initializes the IIC Slave and IIC Master modules at the standard rate and"\
						"\r\ndemonstrates communication through a loopback connection. After successful"\
						"\r\ninitialization, it performs Slave read (receive) and Slave write (transmit)"\
						"\r\noperations. Following each successful IIC transaction (6 bytes), the"\
						"\r\ntransmitted and received data buffers are compared. LEDs blink when the data"\
						"\r\nmatches and tunrn ON if a failure occurs. Status and error messages are"\
						"\r\ndisplayed on the terminal\r\n\r\n"

/* Single entry-point called in hal_entry.c */
void iic_slave_example_app_run(void);

#endif /* IIC_SLAVE_EXAMPLE_APP_H_ */
