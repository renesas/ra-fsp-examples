/***********************************************************************************************************************
 * File Name    : lin_master_example_app.h
 * Description  : Public interface for the SCI_B LIN MASTER example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef LIN_MASTER_EXAMPLE_APP_H_
#define LIN_MASTER_EXAMPLE_APP_H_

#include "lin_master_ep.h"

#define EP_VERSION          "1.1"

#define BANNER_INFO         "\r\n******************************************************************"\
                            "\r\n*   Renesas FSP Example Project for "MODULE_NAME"             *"\
                            "\r\n*   Example Project Version "EP_VERSION"                                  *"\
                            "\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
                            "\r\n******************************************************************"\
                            "\r\nRefer to readme.txt file for more details on Example Project and" \
                            "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#if BSP_PERIPHERAL_SAU_PRESENT
#define MODULE_NAME         "sau_lin Module  "

#define EP_INFO             "\r\nThis project demonstrates the basic functionalities of Local Interconnect"\
                            "\r\nNetwork (LIN) on Renesas RA MCUs based on the Renesas FSP. The LIN"\
                            "\r\nmodules communicate with transceivers that satisfy the ISO9141 protocol."\
                            "\r\nThe Master defines four distinct messages, each assigned a unique ID,"\
                            "\r\nallowing the user to select and transmit a specific message to the Slave."\
                            "\r\nThe Master sends a start frame with the selected ID to retrieve data,"\
                            "\r\nand the Slave responds accordingly. The user can select a baud rate"\
                            "\r\n(2400, 4800, 9600, 10400, 14400, 19200) from the application menu."\
                            "\r\nAdditionally, the LIN module's baud rate can be configured to other"\
                            "\r\nsupported values, as specified in the markdown file, by modifying the"\
                            "\r\nconfiguration.xml. For the SAU LIN Slave, users can enter Low Power"\
                            "\r\nMode (Software Standby) via the EP menu and wake up when triggered"\
                            "\r\nby the Master.\r\n"

#define MAIN_MENU           "\r\n\r\n=== LIN Master Main Menu ==="\
                            "\r\n1. Configure baud rate"\
                            "\r\n2. Transmit LIN data"\
                            "\r\n3. Transmit and receive LIN data"\
                            "\r\n4. Send a wake-up signal"\
                            "\r\nSelect an option: "

#elif BSP_PERIPHERAL_SCI_B_PRESENT || BSP_PERIPHERAL_SCI_PRESENT

#define EP_INFO             "\r\nThis project demonstrates the basic functionalities of Local Interconnect"\
                            "\r\nNetwork (LIN) on Renesas RA MCUs based on the Renesas FSP. The LIN"\
                            "\r\nmodules communicate with transceivers that satisfy the ISO9141 protocol."\
                            "\r\nThe Master defines four distinct messages, each assigned a unique ID,"\
                            "\r\nallowing the user to select and transmit a specific message to the Slave."\
                            "\r\nThe Master sends a start frame with the selected ID to retrieve data,"\
                            "\r\nand the Slave responds accordingly. The user can select a baud rate"\
                            "\r\n(2400, 4800, 9600, 10400, 14400, 19200) from the application menu."\
                            "\r\nAdditionally, the LIN module's baud rate can be configured to other"\
                            "\r\nsupported values, as specified in the markdown file, by modifying the"\
                            "\r\nconfiguration.xml.\r\n"

#define MAIN_MENU           "\r\n\r\n=== LIN Master Main Menu ==="\
                            "\r\n1. Configure baud rate"\
                            "\r\n2. Transmit LIN data"\
                            "\r\n3. Transmit and receive LIN data"\
                            "\r\nSelect an option: "

#if BSP_PERIPHERAL_SCI_PRESENT
#define MODULE_NAME         "sci_lin Module  "

#elif BSP_PERIPHERAL_SCI_B_PRESENT
#define MODULE_NAME         "sci_b_lin Module"

#endif
#endif /* BSP_PERIPHERAL_SAU_PRESENT || BSP_PERIPHERAL_SCI_PRESENT || BSP_PERIPHERAL_SCI_B_PRESENT */

#define WRITE_MENU          "\r\n\r\n=== LIN Write Selection ==="\
                            "\r\n1. Frame ID: 0x20"\
                            "\r\n2. Frame ID: 0x21"\
                            "\r\n3. Frame ID: 0x22"\
                            "\r\n4. Frame ID: 0x23"\
                            "\r\n0. Exit"\
                            "\r\nSelect an option: "

#define READ_MENU          "\r\n\r\n=== LIN Read Selection ==="\
                            "\r\n1. Frame ID: 0x10"\
                            "\r\n2. Frame ID: 0x11"\
                            "\r\n3. Frame ID: 0x12"\
                            "\r\n4. Frame ID: 0x13"\
                            "\r\n0. Exit"\
                            "\r\nSelect an option: "

#define BAUDRATE_OPTION     "\r\n\r\n=== LIN Master Baud Rate Selection ==="\
                            "\r\n[1]. 2400  bps"\
                            "\r\n[2]. 4800  bps"\
                            "\r\n[3]. 9600  bps"\
                            "\r\n[4]. 10400 bps"\
                            "\r\n[5]. 14400 bps"\
                            "\r\n[6]. 19200 bps (Default)"\
                            "\r\nSelect an option: "

/* Single entry-point called in hal_entry.c */
void lin_master_example_app_run(void);

#endif /* LIN_MASTER_EXAMPLE_APP_H_ */
