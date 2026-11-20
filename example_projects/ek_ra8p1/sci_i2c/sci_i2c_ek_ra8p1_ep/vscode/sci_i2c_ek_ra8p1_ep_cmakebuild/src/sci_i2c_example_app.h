/***********************************************************************************************************************
 * File Name    : sci_i2c_example_app.h
 * Description  : Public interface for the SCI I2C example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef SCI_I2C_EXAMPLE_APP_H_
#define SCI_I2C_EXAMPLE_APP_H_

#include "sci_i2c_ep.h"

#define EP_VERSION          "1.1"

#define BANNER_INFO         "\r\n******************************************************************"\
                            "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module"MODULE_PADDING"         *"\
                            "\r\n*   Example Project Version "EP_VERSION"                                  *"\
                            "\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
                            "\r\n******************************************************************"\
                            "\r\nRefer to readme.txt file for more details on Example Project and"\
                            "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO             "\r\nThis example project demonstrates SCI I2C Master operation through"\
                            "\r\nloopback with IIC Slave driver. 6 bytes of data will be transmitted"\
                            "\r\nand received continuously on successful initialization. The transmitted"\
                            "\r\ndata is compared with the received data. If the data matches, on-board"\
                            "\r\nLEDs start blinking. On a data mismatch, the LEDs stay ON. Failure"\
                            "\r\nmessages and status are displayed on the terminal.\r\n"

/* Single entry-point called from hal_entry.c */
void sci_i2c_example_app_run(void);

#endif /* SCI_I2C_EXAMPLE_APP_H_ */
