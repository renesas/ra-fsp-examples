/***********************************************************************************************************************
 * File Name    : smbus_example_app.h
 * Description  : Public interface for the SMBus example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef SMBUS_EXAMPLE_APP_H_
#define SMBUS_EXAMPLE_APP_H_

#define EP_VERSION      "1.0"

#define MODULE_NAME     "rm_comms_smbus"

#define BANNER_INFO     "\r\n******************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module        *"\
                        "\r\n*   Example Project Version %s                                  *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
                        "\r\n******************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO         "\r\nThis example project demonstrates the basic usage of I2C communication conforming to"\
                        "\r\nthe SMBus Specification (version 2.0) on Renesas RA MCUs using the Renesas Flexible"\
                        "\r\nSoftware Package (FSP). The project involves writing commands to the MAX31875 sensor"\
                        "\r\nand reading the resulting temperature values. The SMBus transmission time is monitored"\
                        "\r\nby the GPT module to ensure the MCU acting as a master device complies with the SMBus"\
                        "\r\ntimeout standard. The Packet Error Check (PEC) byte is calculated in CRC-8 format at"\
                        "\r\nboth transmission and reception. These temperature values are continuously printed to a"\
                        "\r\nterminal. Additionally, status information and error messages are displayed on the"\
                        "\r\nhost PC. The error LED will turn on if an error occurs.\r\n\r\n"

/* Single entry-point called in hal_entry.c */
void smbus_example_app_run(void);

#endif /* SMBUS_EXAMPLE_APP_H_ */
