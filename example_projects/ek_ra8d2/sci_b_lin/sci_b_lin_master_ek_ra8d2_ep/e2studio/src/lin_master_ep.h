/***********************************************************************************************************************
 * File Name    : lin_master_ep.h
 * Description  : Contains macros data structures and functions used in lin_master_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef LIN_MASTER_EP_H_
#define LIN_MASTER_EP_H_

#include "common_utils.h"

#define EP_VERSION                  ("1.0")

#if BSP_PERIPHERAL_SAU_PRESENT
#define MODULE_NAME                 "sau_lin"
#elif BSP_PERIPHERAL_SCI_B_PRESENT
#define MODULE_NAME                 "sci_b_lin"
#endif /* BSP_PERIPHERAL_SAU_PRESENT || BSP_PERIPHERAL_SCI_B_PRESENT */

#define BANNER_1                    "\r\n******************************************************************"
#if BSP_PERIPHERAL_SAU_PRESENT
#define BANNER_2                    "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module               *"
#elif BSP_PERIPHERAL_SCI_B_PRESENT
#define BANNER_2                    "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module             *"
#endif /* BSP_PERIPHERAL_SAU_PRESENT || BSP_PERIPHERAL_SCI_B_PRESENT */
#define BANNER_3                    "\r\n*   Example Project Version %s                                  *"
#define BANNER_4                    "\r\n*   Flex Software Pack Version %d.%d.%d                             *"
#define BANNER_5                    "\r\n******************************************************************"
#define BANNER_6                    "\r\nRefer to readme.txt file for more details on Example Project and"\
                                    "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define ENABLE_MASTER_TIMEOUT_MANAGEMENT    (1U)
#define WRITE_FRAME_ID_20H                  (0x20)
#define WRITE_FRAME_ID_21H                  (0x21)
#define WRITE_FRAME_ID_22H                  (0x22)
#define WRITE_FRAME_ID_23H                  (0x23)

#define READ_FRAME_ID_10H                   (0x10)
#define READ_FRAME_ID_11H                   (0x11)
#define READ_FRAME_ID_12H                   (0x12)
#define READ_FRAME_ID_13H                   (0x13)

#define FRAME_ID_20H_DATA_LENGTH            (3)
#define FRAME_ID_21H_DATA_LENGTH            (3)
#define FRAME_ID_22H_DATA_LENGTH            (3)
#define FRAME_ID_23H_DATA_LENGTH            (3)
#define FRAME_ID_24H_DATA_LENGTH            (2)

#define FRAME_ID_10H_DATA_LENGTH            (2)
#define FRAME_ID_11H_DATA_LENGTH            (2)
#define FRAME_ID_12H_DATA_LENGTH            (2)
#define FRAME_ID_13H_DATA_LENGTH            (2)

#define FRAME_ID_20H_INDEX                  (0)
#define FRAME_ID_21H_INDEX                  (1)
#define FRAME_ID_22H_INDEX                  (2)
#define FRAME_ID_23H_INDEX                  (3)

#define MAX_NUMBER_OF_TX_FRAME_ID           (4)
#define MAX_NUMBER_OF_RX_FRAME_ID           (4)
#define MIN_NUMBER_OF_RX_FRAME_ID           (1)
#define MIN_BAUDRATE_INDEX                  (1)
#define MAX_TRANSFER_LENGTH                 (8)
#define MENU_INDEX                          (0)
#define MASTER_SET_BAUDRATE                 ('1')
#define MASTER_WRITE                        ('2')
#define MASTER_READ                         ('3')
#define WAKE_UP_SLAVE                       ('4')
#define LIN_BAUDRATE_COUNT                  (6)
#define LIN_TIMEOUT_FACTOR                  (14U)
#define LIN_TIMEOUT_DIVISOR                 (10U)
#define LIN_DEFAULT_BAUDRATE                (19200)
#define TERMINAL_READ_SIZE                  (8U)
#define IS_WRITE                            (1U)
#define IS_READ                             (0U)
#define EXIT_INDEX                          (0U)
#define TIMEOUT_LIMIT                       (1000000U)
#define LIN_PID_MASK_ID                     (0x3FU)    /* Mask to extract 6-bit LIN ID from PID */

#if BSP_PERIPHERAL_SAU_PRESENT

#define EP_INFO                     "\r\nThis project demonstrates the basic functionalities of Local Interconnect"\
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
									"\r\nby the Master.\r\n\r\n"

#define MAIN_MENU                   "\r\n=== LIN Master Main Menu ==="\
                                    "\r\n1. Configure baud rate"\
									"\r\n2. Transmit LIN data"\
									"\r\n3. Transmit and receive LIN data"\
									"\r\n4. Send a wake-up signal"\
									"\r\n\nSelect an option:"

/* LIN API mapping */
#define LIN_OPEN                    (R_SAU_LIN_Open)
#define LIN_START_FRAME_WRITE       (R_SAU_LIN_StartFrameWrite)
#define LIN_INFO_FRAME_WRITE        (R_SAU_LIN_InformationFrameWrite)
#define LIN_COMMUNICATION_ABORT     (R_SAU_LIN_CommunicationAbort)
#define LIN_INFO_FRAME_READ         (R_SAU_LIN_InformationFrameRead)
#define LIN_BAUD_CALCULATE          (R_SAU_UART_BaudCalculate)
#define LIN_CLOSE                   (R_SAU_LIN_Close)

/* Timer API mapping */
#define TIMER_OPEN                  (R_TAU_Open)
#define TIMER_RESET                 (R_TAU_Reset)
#define TIMER_PERIOD_SET            (R_TAU_PeriodSet)
#define TIMER_INFO_GET              (R_TAU_InfoGet)
#define TIMER_START                 (R_TAU_Start)
#define TIMER_STOP                  (R_TAU_Stop)
#define TIMER_CLOSE                 (R_TAU_Close)

#elif BSP_PERIPHERAL_SCI_B_PRESENT
#define EP_INFO                     "\r\nThis project demonstrates the basic functionalities of Local Interconnect"\
                                    "\r\nNetwork (LIN) on Renesas RA MCUs based on the Renesas FSP. The LIN"\
									"\r\nmodules communicate with transceivers that satisfy the ISO9141 protocol."\
									"\r\nThe Master defines four distinct messages, each assigned a unique ID,"\
									"\r\nallowing the user to select and transmit a specific message to the Slave."\
									"\r\nThe Master sends a start frame with the selected ID to retrieve data,"\
									"\r\nand the Slave responds accordingly. The user can select a baud rate"\
									"\r\n(2400, 4800, 9600, 10400, 14400, 19200) from the application menu."\
									"\r\nAdditionally, the LIN module's baud rate can be configured to other"\
									"\r\nsupported values, as specified in the markdown file, by modifying the"\
									"\r\nconfiguration.xml.\r\n\r\n"

#define MAIN_MENU                   "\r\n=== LIN Master Main Menu ==="\
                                    "\r\n1. Configure baud rate"\
									"\r\n2. Transmit LIN data"\
									"\r\n3. Transmit and receive LIN data"\
									"\r\n\nSelect an option:"

/* LIN API mapping */
#define LIN_OPEN                    (R_SCI_B_LIN_Open)
#define LIN_START_FRAME_WRITE       (R_SCI_B_LIN_StartFrameWrite)
#define LIN_INFO_FRAME_WRITE        (R_SCI_B_LIN_InformationFrameWrite)
#define LIN_COMMUNICATION_ABORT     (R_SCI_B_LIN_CommunicationAbort)
#define LIN_BAUD_CALCULATE          (R_SCI_B_LIN_BaudCalculate)
#define LIN_INFO_FRAME_READ         (R_SCI_B_LIN_InformationFrameRead)
#define LIN_CLOSE                   (R_SCI_B_LIN_Close)

/* Timer API mapping */
#define TIMER_OPEN                  (R_GPT_Open)
#define TIMER_RESET                 (R_GPT_Reset)
#define TIMER_PERIOD_SET            (R_GPT_PeriodSet)
#define TIMER_INFO_GET              (R_GPT_InfoGet)
#define TIMER_START                 (R_GPT_Start)
#define TIMER_STOP                  (R_GPT_Stop)
#define TIMER_CLOSE                 (R_GPT_Close)
#endif /* BSP_PERIPHERAL_SAU_PRESENT || BSP_PERIPHERAL_SCI_B_PRESENT */

#define WRITE_MENU                  "\r\n=== LIN Write Selection ==="\
                                    "\r\n1. Frame ID: 0x20"\
									"\r\n2. Frame ID: 0x21"\
									"\r\n3. Frame ID: 0x22"\
									"\r\n4. Frame ID: 0x23"\
									"\r\n0. Exit"\
									"\r\n\nSelect an option:\r\n"

#define READ_MENU                   "\r\n=== LIN Read Selection ==="\
                                    "\r\n1. Frame ID: 0x10"\
									"\r\n2. Frame ID: 0x11"\
									"\r\n3. Frame ID: 0x12"\
									"\r\n4. Frame ID: 0x13"\
									"\r\n0. Exit"\
									"\r\n\nSelect an option:\r\n"

#define BAUDRATE_OPTION             "\r\n\r\n=== LIN Master Baud Rate Selection ==="\
                                    "\r\n[1]. 2400  bps"\
									"\r\n[2]. 4800  bps"\
									"\r\n[3]. 9600  bps"\
									"\r\n[4]. 10400 bps"\
									"\r\n[5]. 14400 bps"\
									"\r\n[6]. 19200 bps (Default)"\
									"\r\n\nSelect an option:"
typedef struct
{
    uint32_t header_timeout_us;   /* Header timeout in microseconds */
    uint32_t response_timeout_us; /* Response timeout in microseconds */
} lin_timing_t;

typedef struct
{
    uint8_t id;
    const uint8_t * expected;
    size_t length;
} lin_frame_expectation_t;

void lin_master_operation(void);

#endif /* LIN_MASTER_EP_H_ */
