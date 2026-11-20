/***********************************************************************************************************************
 * File Name    : i3c_master_example_app.h
 * Description  : Public interface for the I3C Master example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef I3C_MASTER_EXAMPLE_APP_H_
#define I3C_MASTER_EXAMPLE_APP_H_

#include "i3c_master_ep.h"

#ifdef I3C_HDR_DDR_SUPPORT
#include "i3c_master_hdr.h"
#endif /* I3C_HDR_DDR_SUPPORT */

#define EP_VERSION      "1.1"

#define MODULE_NAME     "r_i3c"

#define BANNER_INFO     "\r\n******************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" in Master mode         *"\
                        "\r\n*   Example Project Version %s                                  *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
                        "\r\n******************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n\r\n"

#define EP_INFO         "This example project demonstrates the typical use of the I3C Driver as Master\r\n"\
                        "on Renesas RA MCUs based on Renesas FSP. The I3C Master demonstrates operation\r\n"\
                        "with an I3C Slave connected on another RA board. Once initialization is successful,\r\n"\
                        "it will start assigning Dynamic Addresses to Slave devices present on the bus.\r\n"\
                        "The EP performs write/read operations and displays device information based on\r\n"\
                        "user input. Error and info messages will be printed on the terminal.\r\n"

#ifdef I3C_HDR_DDR_SUPPORT
#define EP_MENU         "\r\nI3C Master operations:\r\n"\
                        "1. Display I3C Slave device information if exists on I3C bus\r\n"\
                        "2. I3C Write Read operation in SDR\r\n"\
                        "3. I3C Write Read operation in HDR-DDR\r\n"
#else
#define EP_MENU         "\r\nI3C Master operations:\r\n"\
                        "1. Display I3C Slave device information if exists on I3C bus\r\n"\
                        "2. I3C Write Read operation\r\n"
#endif /* I3C_HDR_DDR_SUPPORT */

typedef enum menu_options
{
    DISPLAY_I3C_SLAVE_INFO = 1,
    MASTER_WRITE_READ,
#ifdef I3C_HDR_DDR_SUPPORT
    MASTER_WRITE_READ_HDR,
#endif /* I3C_HDR_DDR_SUPPORT */
} menu_options_t;

/* Single entry-point called in hal_entry.c */
void i3c_master_example_app_run(void);

#endif /* I3C_MASTER_EXAMPLE_APP_H_ */
