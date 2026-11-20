/***********************************************************************************************************************
 * File Name    : lin_slave_example_app.h
 * Description  : Public interface for the LIN Slave example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef LIN_SLAVE_EXAMPLE_APP_H_
#define LIN_SLAVE_EXAMPLE_APP_H_

#include "lin_slave_ep.h"

#define EP_VERSION          "1.1"

#define BANNER_INFO         "\r\n****************************************************************"\
                            "\r\n*   Renesas FSP Example Project for "MODULE_NAME"             *"\
                            "\r\n*   Example Project Version "EP_VERSION"                                  *"\
                            "\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
                            "\r\n****************************************************************"\
                            "\r\nRefer to readme.txt file for more details on Example Project and"\
                            "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#if BSP_PERIPHERAL_SAU_PRESENT

#define MODULE_NAME         "sau_lin Module  "

#define EP_INFO             "\r\nThis project demonstrates the basic functionalities of LIN on Renesas"\
                            "\r\nRA MCUs based on Renesas FSP. The Master defines four distinct messages,"\
                            "\r\neach assigned a unique ID, allowing the user to select and transmit"\
                            "\r\na specific message to the Slave. The Master sends a start frame with"\
                            "\r\nthe selected ID to retrieve data, and the Slave responds accordingly."\
                            "\r\nAdditionally, users can configure the baud rate to other supported values."\
                            "\r\nFor the Slave, users can enter Low Power Mode (Software Standby) via"\
                            "\r\nthe EP menu and wake up when triggered by the Master.\r\n"

#define MAIN_MENU           "\r\n\r\n=== LIN Slave Main Menu ==="\
                            "\r\n1. Configure baud rate"\
                            "\r\n2. Enter software standby mode"\
                            "\r\nSelect an option or wait for master communication: "

#elif BSP_PERIPHERAL_SCI_B_PRESENT || BSP_PERIPHERAL_SCI_PRESENT

#define EP_INFO             "\r\nThis project demonstrates the basic functionalities of LIN on Renesas"\
                            "\r\nRA MCUs based on Renesas FSP. The Master defines four distinct messages,"\
                            "\r\neach assigned a unique ID, allowing the user to select and transmit"\
                            "\r\na specific message to the Slave. The Master sends a start frame with"\
                            "\r\nthe selected ID to retrieve data, and the Slave responds accordingly."\
                            "\r\nAdditionally, users can configure the baud rate to other supported values.\r\n"

#define MAIN_MENU           "\r\n\r\n=== LIN Slave Main Menu ==="\
                            "\r\n1. Configure baud rate"\
                            "\r\nSelect an option or wait for master communication: "

#if BSP_PERIPHERAL_SCI_PRESENT
#define MODULE_NAME         "sci_lin Module  "
#elif BSP_PERIPHERAL_SCI_B_PRESENT
#define MODULE_NAME         "sci_b_lin Module"
#endif

#endif /* BSP_PERIPHERAL_SAU_PRESENT || BSP_PERIPHERAL_SCI_PRESENT || BSP_PERIPHERAL_SCI_B_PRESENT */

#define BAUDRATE_OPTION     "\r\n\r\n=== LIN Slave Baud Rate Selection ==="\
                            "\r\n[1]. 2400  bps"\
                            "\r\n[2]. 4800  bps"\
                            "\r\n[3]. 9600  bps"\
                            "\r\n[4]. 10400 bps"\
                            "\r\n[5]. 14400 bps"\
                            "\r\n[6]. 19200 bps (Default)"\
                            "\r\nSelect an option: "

/* Single entry-point called in hal_entry.c */
void lin_slave_example_app_run(void);

#endif /* LIN_SLAVE_EXAMPLE_APP_H_ */
