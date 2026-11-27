/***********************************************************************************************************************
 * File Name    : sau_i2c_master_example_app.h
 * Description  : Public interface for the SAU I2C Master example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef SAU_I2C_MASTER_EXAMPLE_APP_H_
#define SAU_I2C_MASTER_EXAMPLE_APP_H_

#define EP_VERSION      "1.1"

#define MODULE_NAME     "r_sau_i2c"

#define BANNER_INFO     "\r\n******************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module             *"\
                        "\r\n*   Example Project Version %s                                  *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
                        "\r\n******************************************************************\r\n"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO         "\r\nThe example project demonstrates the basic functionality of the SAU I2C driver"\
                        "\r\non Renesas RA MCUs based on FSP. The project initializes the SAU I2C Master"\
						"\r\nmodule with the standard rate of 100 kHz and communicates with the Pmod ACL"\
						"\r\nBoard for ADXL345. On power-up, after initializing the SAU I2C Master module and"\
						"\r\nconfiguring the ADXL345 sensor, the RA MCU will read the axis acceleration data"\
						"\r\nfrom the sensor and display it on the terminal every time new data is detected"\
						"\r\nby an external interrupt.\r\n\r\n"

/* Single entry-point called in hal_entry.c */
void sau_i2c_master_example_app_run(void);

#endif /* SAU_I2C_MASTER_EXAMPLE_APP_H_ */
