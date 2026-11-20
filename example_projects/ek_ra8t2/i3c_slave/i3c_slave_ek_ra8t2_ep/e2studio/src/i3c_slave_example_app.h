/***********************************************************************************************************************
 * File Name    : i3c_slave_example_app.h
 * Description  : Public interface for the I3C Slave example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef I3C_SLAVE_EXAMPLE_APP_H_
#define I3C_SLAVE_EXAMPLE_APP_H_

#include "i3c_slave_ep.h"

#define EP_VERSION      "1.1"

#define MODULE_NAME     "r_i3c"

#define BANNER_INFO     "\r\n******************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" in Slave mode          *"\
                        "\r\n*   Example Project Version %s                                  *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
                        "\r\n******************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n\r\n"

#define EP_INFO         "This example project demonstrates the typical use of the I3C Driver as a Slave\r\n"\
                        "device on Renesas RA MCUs based on Renesas FSP. Once initialization is successful,\r\n"\
                        "the EP waits for DAA, or if DAA is not completed it will initiates Hot-Join request.\r\n"\
                        "If on-board switch is pressed, it will initiate IBI transfer request. Error and info\r\n"\
                        "messages will be printed on the terminal.\r\n"

/* Single entry-point called in hal_entry.c */
void i3c_slave_example_app_run(void);

#endif /* I3C_SLAVE_EXAMPLE_APP_H_ */
