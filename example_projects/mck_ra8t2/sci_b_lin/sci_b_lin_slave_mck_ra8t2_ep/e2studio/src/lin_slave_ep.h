/***********************************************************************************************************************
 * File Name    : lin_slave_ep.h
 * Description  : Contains data structures and functions used in lin_slave_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef LIN_SLAVE_EP_H_
#define LIN_SLAVE_EP_H_

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

#define MAIN_MENU                   "\r\n=== LIN Slave Main Menu ==="\
                                    "\r\n1. Configure baud rate"\
								    "\r\n2. Enter software standby mode"\
								    "\r\n\nSelect an option or wait for master communication:\r\n"

/* LIN API Mapping */
#define LIN_OPEN                    (R_SAU_LIN_Open)
#define LIN_START_FRAME_WRITE       (R_SAU_LIN_StartFrameWrite)
#define LIN_INFO_FRAME_WRITE        (R_SAU_LIN_InformationFrameWrite)
#define LIN_COMMUNICATION_ABORT     (R_SAU_LIN_CommunicationAbort)
#define LIN_INFO_FRAME_READ         (R_SAU_LIN_InformationFrameRead)
#define LIN_BAUD_CALCULATE          (R_SAU_UART_BaudCalculate)
#define LIN_CLOSE                   (R_SAU_LIN_Close)

/* Timer API Mapping */
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

#define MAIN_MENU                   "\r\n=== LIN Slave Main Menu ==="\
                                    "\r\n1. Configure baud rate"\
									"\r\n\nSelect an option or wait for master communication:\r\n"

/* LIN API Mapping */
#define LIN_OPEN                    (R_SCI_B_LIN_Open)
#define LIN_START_FRAME_WRITE       (R_SCI_B_LIN_StartFrameWrite)
#define LIN_INFO_FRAME_WRITE        (R_SCI_B_LIN_InformationFrameWrite)
#define LIN_COMMUNICATION_ABORT     (R_SCI_B_LIN_CommunicationAbort)
#define LIN_BAUD_CALCULATE          (R_SCI_B_LIN_BaudCalculate)
#define LIN_INFO_FRAME_READ         (R_SCI_B_LIN_InformationFrameRead)
#define LIN_CLOSE                   (R_SCI_B_LIN_Close)

/* Timer API Mapping */
#define TIMER_OPEN                  (R_GPT_Open)
#define TIMER_RESET                 (R_GPT_Reset)
#define TIMER_PERIOD_SET            (R_GPT_PeriodSet)
#define TIMER_INFO_GET              (R_GPT_InfoGet)
#define TIMER_START                 (R_GPT_Start)
#define TIMER_STOP                  (R_GPT_Stop)
#define TIMER_CLOSE                 (R_GPT_Close)
#endif /* BSP_PERIPHERAL_SAU_PRESENT || BSP_PERIPHERAL_SCI_B_PRESENT */

#define TIMEOUT_LIMIT                       (1000000U)
#define TRANSFER_LENGTH                     (8)
#define LIN_BAUDRATE_COUNT                  (6)

#define READ_FRAME_ID_20H                   (0x20)
#define READ_FRAME_ID_21H                   (0x21)
#define READ_FRAME_ID_22H                   (0x22)
#define READ_FRAME_ID_23H                   (0x23)

#define READ_FRAME_ID_10H                   (0x10)
#define READ_FRAME_ID_11H                   (0x11)
#define READ_FRAME_ID_12H                   (0x12)
#define READ_FRAME_ID_13H                   (0x13)

#define FRAME_ID_20H_DATA_LENGTH            (3)
#define FRAME_ID_21H_DATA_LENGTH            (3)
#define FRAME_ID_22H_DATA_LENGTH            (3)
#define FRAME_ID_23H_DATA_LENGTH            (3)


#define FRAME_ID_10H_INDEX                  (0)
#define FRAME_ID_11H_INDEX                  (1)
#define FRAME_ID_12H_INDEX                  (2)
#define FRAME_ID_13H_INDEX                  (3)

#define FRAME_ID_10H_DATA_LENGTH            (2)
#define FRAME_ID_11H_DATA_LENGTH            (2)
#define FRAME_ID_12H_DATA_LENGTH            (2)
#define FRAME_ID_13H_DATA_LENGTH            (2)

#define MIN_BAUDRATE_INDEX                  (1)
#define MAX_TRANSFER_LENGTH                 (8)
#define MAX_NUMBER_OF_FRAME_ID              (8)
#define MIN_INDEX_OF_FRAME_ID_WRITE         (4)
#define MAX_INDEX_OF_FRAME_ID_WRITE         (7)
#define MIN_INDEX_OF_FRAME_ID_READ          (0)
#define MAX_INDEX_OF_FRAME_ID_READ          (3)
#define MENU_INDEX                          (0)
#define SLAVE_SET_BAUDRATE                  ('1')
#define SLAVE_ENTER_LPM                     ('2')
#define TERMINAL_READ_SIZE                  (8U)
#define LIN_PID_MASK_ID                     (0x3FU)    /* Mask to extract 6-bit LIN ID from PID */


#define BAUDRATE_OPTION                     "\r\n\r\n=== LIN Slave Baud Rate Selection ==="\
                                            "\r\n[1]. 2400  bps"\
											"\r\n[2]. 4800  bps"\
											"\r\n[3]. 9600  bps"\
											"\r\n[4]. 10400 bps"\
											"\r\n[5]. 14400 bps"\
											"\r\n[6]. 19200 bps (Default)"\
											"\r\n\nSelect an option:"

typedef struct
{
    uint8_t id;
    const uint8_t *expected;
    size_t length;
} lin_frame_expectation_t;

void lin_slave_operation(void);

#endif /* LIN_SLAVE_EP_H_ */
