/***********************************************************************************************************************
 * File Name    : can_fd_example_app.h
 * Description  : Public interface for the CAN FD example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef CAN_FD_EXAMPLE_APP_H_
#define CAN_FD_EXAMPLE_APP_H_

#include "can_fd_ep.h"

#define EP_VERSION              "1.1"

#define MODULE_NAME             "CAN FD"

#define BANNER_INFO             "\r\n************************************************************************"\
                                "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                      *"\
                                "\r\n*   Example Project Version %s                                        *"\
                                "\r\n*   Flex Software Pack Version %d.%d.%d                                   *"\
                                "\r\n************************************************************************"\
                                "\r\nRefer to the README.md for the Example Project documentation and"\
                                "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO                 "\r\nThis example project showcases CAN FD communication using two Renesas RA"\
                                "\r\nboards. The user can set the nominal rate, FD data rate, and sample point"\
                                "\r\nat runtime, and initiate data transmission with the selected settings."\
                                "\r\nTo stop the transmission and return to the baud rate configuration menu,"\
                                "\r\nthe user press '2'. During transmission, data is sent from Board 1 to"\
                                "\r\nBoard 2. Upon receiving the data, Board 2 displays it on the terminal and"\
                                "\r\nsends an updated message back to Board 1. When Board 1 receives this data,"\
                                "\r\nit converts the CAN frame to a CAN FD frame and transmits it back to Board 2."\
                                "\r\nBoard 2, upon receiving this frame, further updates the data and sends it to"\
                                "\r\nBoard 1 using the CAN FD format. Finally, once the transmission cycle completes,"\
                                "\r\nBoard 1 prints the received data on the terminal.\r\n"

#define EP_MENU                 "\r\nMain menu:"\
                                "\r\n1. Select the CANFD nominal rate."\
                                "\r\n2. Select the CANFD FD data rate."\
                                "\r\n3. Select the CANFD sample point."\
                                "\r\n4. Change CANFD baud rate and start CANFD transmission."\
                                "\r\nUser input: "

#define SUB_MENU                "\r\nPress '1' to start CAN transmission."\
                                "\r\nPress '2' to return to the baud rate configuration menu.\r\n"

#define CANFD_BAUDRATE_INFO     "\r\nCANFD nominal rate: %d bits/s"\
                                "\r\nCANFD FD data rate: %d bits/s"\
                                "\r\nCANFD sample point: %d%%\r\n"

#define NOMINAL_RATE_OPTION     "\r\nNominal rate option:"\
                                "\r\n1. 250 kbits/s"\
                                "\r\n2. 500 kbits/s"\
                                "\r\n3. 1 Mbits/s"\
                                "\r\n4. Custom (bits/s)"\
                                "\r\nUser input: "

#if defined (SUPPORT_MAX_DATA_RATE_5MBPS)
#define DATA_RATE_OPTION        "\r\nFD data rate option:"\
                                "\r\n1. 1 Mbits/s"\
                                "\r\n2. 2 Mbits/s"\
                                "\r\n3. 5 Mbits/s"\
                                "\r\n4. Custom (bits/s)"\
                                "\r\nUser input: "

#elif defined (SUPPORT_MAX_DATA_RATE_8MBPS)
#define DATA_RATE_OPTION        "\r\nFD data rate option:"\
                                "\r\n1. 1 Mbits/s"\
                                "\r\n2. 2 Mbits/s"\
                                "\r\n3. 8 Mbits/s"\
                                "\r\n4. Custom (bits/s)"\
                                "\r\nUser input: "
#endif /* SUPPORT_MAX_DATA_RATE */

/* Single entry-point called in hal_entry.c */
void can_fd_example_app_run(void);

#endif /* CAN_FD_EXAMPLE_APP_H_ */
