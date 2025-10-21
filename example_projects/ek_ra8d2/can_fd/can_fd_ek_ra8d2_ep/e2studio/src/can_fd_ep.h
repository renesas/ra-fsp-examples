/***********************************************************************************************************************
 * File Name    : can_fd_ep.h
 * Description  : Contains declarations of data structures and functions used in the EP.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef CAN_FD_EP_H_
#define CAN_FD_EP_H_

#define CAN_MAILBOX_NUMBER_0          (0U)              /* Mailbox number 0 for CAN transmission */
#define CAN_CLASSIC_FRAME_DATA_BYTES  (8U)              /* Data length code (DLC) for Classic CAN frame */
#define CAN_FD_DATA_LENGTH_CODE       (64U)             /* Data length code (DLC) for CAN FD frame (max 64 bytes) */
#define CAN_ID                        (0x1100)          /* Extended CAN ID used in transmit frame */

/* Acceptance filter array parameters */
#define CANFD_FILTER_ID               (0x00001000)
#define MASK_ID                       (0x1FFFF000)
#define MASK_ID_MODE                  (1)

#define ZERO                          (0U)               /* Index value 0 */
#define NULL_CHAR                     ('\0')
#define WAIT_TIME                     (500U)             /* Wait time value */

#define EP_INFO      "\r\nThis example project demonstrates CAN FD operations on Renesas RA MCUs"\
                     "\r\nusing two RA boards. On pressing any key on the J-Link RTT Viewer, data is"\
                     "\r\ntransmitted from one board to the other. On data reception, Board 2 displays"\
                     "\r\nthe received data on the J-Link RTT Viewer and transmits the updated data to"\
                     "\r\nBoard 1. Board 1 on reception of data, changes the CAN frame to a CAN FD frame"\
                     "\r\nand sends data to Board 2. On reception, Board 2 transmits the updated data."\
                     "\r\nOn successful transmission, Board 1 prints the data on the J-Link RTT Viewer.\r\n\r\n"\

/* Enumeration to select LED that is to be made ON */
typedef enum
{
   LED_CASE_1 = 1,
   LED_CASE_2 = 2,
   LED_CASE_3 = 3,
} led_state_t;

/* Enumeration to select status of LED */
typedef enum
{
#if defined (BOARD_RA8T1_MCK) || defined (BOARD_RA4T1_MCK) || defined (BOARD_RA6T3_MCK) || defined (BOARD_RA8T2_MCK)
   LED_ON = BSP_IO_LEVEL_LOW,
   LED_OFF = BSP_IO_LEVEL_HIGH,
#else
   LED_ON = BSP_IO_LEVEL_HIGH,
   LED_OFF = BSP_IO_LEVEL_LOW,
#endif
} led_status_t;

void canfd_operation(void);
void can_read_operation(void);
void canfd_deinit(void);
void led_update(led_state_t led_state);

#endif /* CAN_FD_EP_H_ */
