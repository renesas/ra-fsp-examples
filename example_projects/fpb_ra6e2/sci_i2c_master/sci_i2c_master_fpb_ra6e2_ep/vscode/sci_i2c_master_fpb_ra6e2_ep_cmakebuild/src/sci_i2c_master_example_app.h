/***********************************************************************************************************************
 * File Name    : sci_i2c_master_example_app.h
 * Description  : Public interface for the SCI I2C Master example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef SCI_I2C_MASTER_EXAMPLE_APP_H_
#define SCI_I2C_MASTER_EXAMPLE_APP_H_

#include "sci_i2c_ep.h"

#define EP_VERSION          "1.1"

#define BANNER_INFO         "\r\n******************************************************************"\
                            "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module"MODULE_PADDING"         *"\
                            "\r\n*   Example Project Version "EP_VERSION"                                  *"\
                            "\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
                            "\r\n******************************************************************"\
                            "\r\nRefer to readme.txt file for more details on Example Project and"\
                            "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO             "\r\nThe example project demonstrates the typical use of the SCI I2C Master"\
                            "\r\nHAL module APIs. The project initializes SCI I2C Master module with the"\
                            "\r\nstandard rate and interfaces with Pmod ACL for ADXL345. On power up, after"\
                            "\r\ninitializing and configuring the sensor, the RA MCU will read the axis"\
                            "\r\nacceleration data from the sensor and display it on the terminal every time"\
                            "\r\nnew data is detected by an external interrupt.\r\n\r\n"

/* Single entry-point called from hal_entry.c */
void sci_i2c_master_example_app_run(void);

#endif /* SCI_I2C_MASTER_EXAMPLE_APP_H_ */
