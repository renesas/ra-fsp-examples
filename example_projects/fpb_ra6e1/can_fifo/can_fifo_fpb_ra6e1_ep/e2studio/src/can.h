/***********************************************************************************************************************
 * File Name    : can.h
 * Description  : Contains data structures and functions setup can module used in can.h.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
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
#define CAN_BUSY_DELAY                  (5000U) /* microseconds */

#define EP_INFO  "\r\nThis project demonstrates the basic functionalities of CAN running with FIFO support on Renesas RA MCUs\r\n"\
                  "using 2 RA boards. On Board 1 enter any key on RTT viewer to send a remote frame to Board 2 to request data.\r\n"\
                  "When receiving a remote frame, Board 2 will send four messages back-to-back to Board 1. Board 1 receives data\r\n"\
                  "and displays it on the RTT viewer. On successful comparison of data, Board 1 will transmit a data frame to \r\n"\
                  "Board 2 as received acknowledgment. Error messages and information will be printed on the J-link RTT Viewer.\r\n\n"\

void can_entry (void);

#endif /* CAN_H_ */
