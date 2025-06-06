/***********************************************************************************************************************
 * File Name    : can.h
 * Description  : Contains data structures and functions setup can module used in can.h.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef CAN_H_
#define CAN_H_

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define CAN_FRAME_TRANSMIT_DATA_BYTES   (8U)
#define ZERO                            (0U)
#define NULL_CHAR                       ('\0')
#define ONE_MSG                         (1)
#define FOUR_MSG                        (4)
#define REQUIRED_MSG_COUNT              (4)
#define STANDARD_ID_EXAMPLE             (0)
#define EXTENDED_ID_EXAMPLE             (0x12345678)
#define CAN_BUSY_DELAY                  (5000U)         /* Microseconds */

#define EP_INFO  "\r\nThis project demonstrates the basic functionalities of CAN running with FIFO"\
		         " support on Renesas RA MCUs\r\n"\
                 "using 2 RA boards. On Board 1 enter any key on the J-Link RTT Viewer to send a remote frame"\
                 " to Board 2 to request data.\r\n"\
                 "When receiving a remote frame, Board 2 will send four messages back-to-back"\
                 " to Board 1. Board 1 receives data\r\n"\
                 "and displays it on the J-Link RTT Viewer. On successful comparison of data, Board 1"\
                 " will transmit a data frame to \r\n"\
                 "Board 2 as received acknowledgment. Error messages and information will be printed"\
                 " on the J-Link RTT Viewer.\r\n\n"\

void can_entry (void);

#endif /* CAN_H_ */
