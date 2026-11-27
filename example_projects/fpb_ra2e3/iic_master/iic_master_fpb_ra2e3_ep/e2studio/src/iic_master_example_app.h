/***********************************************************************************************************************
 * File Name    : iic_master_example_app.h
 * Description  : Public interface for the IIC Master example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef IIC_MASTER_EXAMPLE_APP_H_
#define IIC_MASTER_EXAMPLE_APP_H_

#include "iic_sensor.h"

#define EP_VERSION      "1.0"

#define BANNER_INFO     "\r\n******************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module"MODULE_PADDING"        *"\
                        "\r\n*   Example Project Version %s                                  *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
                        "\r\n******************************************************************\r\n"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO         "\r\nThe example project demonstrates the typical use of the IIC Master HAL module"\
                        "\r\nAPIs. The project initializes IIC Master module with standard rate and"\
                        "\r\ninterfaces with Pmod ACL Board for ADXL345. On power up after establishing the"\
                        "\r\nconnection of sensor with RA board, it displays accelerometer axis data on the"\
                        "\r\nterminal. Additionally, any error messages will be displayed on the terminal.\r\n\r\n"

/* Single entry-point called in hal_entry.c */
void iic_master_example_app_run(void);

#endif /* IIC_MASTER_EXAMPLE_APP_H_ */
