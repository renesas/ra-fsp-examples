/***********************************************************************************************************************
 * File Name    : ssi_slave_example_app.h
 * Description  : Public interface for the SSI Slave example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef SSI_SLAVE_EXAMPLE_APP_H_
#define SSI_SLAVE_EXAMPLE_APP_H_

#define EP_VERSION              "1.0"

#define MODULE_NAME             "r_ssi"

#define BANNER_INFO             "\r\n******************************************************************"\
                                "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                 *"\
                                "\r\n*   Example Project Version %s                                  *"\
                                "\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
                                "\r\n******************************************************************"\
                                "\r\nRefer to readme.txt file for more details on Example Project and"\
                                "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO                 "\r\nThe example project demonstrates the typical use of the SSI"\
                                "\r\nmodule APIs. The project configures two RA boards, the first"\
                                "\r\nboard with one channel as SSI Master and the second board with"\
                                "\r\none channel as SSI Slave. On selecting Master write - Slave read,"\
                                "\r\nthe Master starts writing data, and the Slave reads the data."\
                                "\r\nOn selecting Slave write - Master read, the Slave starts writing"\
                                "\r\ndata, and the Master reads the data. The result of transfers"\
                                "\r\nbetween SSI Master and SSI Slave is displayed on the terminal.\r\n"

#define EP_MENU                 "\r\n\r\nPlease select the desired operation\n"\
                                "1. Press 1 for Master write - Slave read\r\n"\
                                "2. Press 2 for Master read  - Slave write\r\n"\
                                "User input: "

/* Single entry-point called in hal_entry.c */
void ssi_slave_example_app_run(void);

#endif /* SSI_SLAVE_EXAMPLE_APP_H_ */
