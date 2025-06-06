/***********************************************************************************************************************
 * File Name    : usb_pmsc.h
 * Description  : Contains data structures and functions used in pmsc_thread_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef USB_PMSC_H_
#define USB_PMSC_H_

/* Macros used in usb_pmsc example project */
#if defined (BOARD_RA4L1_EK)
#define EP_INFO      "\r\nThis example project demonstrates basic functionalities of USB Peripheral Mass Storage\r\n"\
                     "Class (PMSC) driver with FreeRTOS on Renesas RA MCUs based on Renesas FSP.\r\n"\
                     "FreeRTOS uses the underlying Block media driver which utilizes USB driver. The PMSC driver\r\n"\
                     "internally configures memory device (on board QSPI Flash) to pop up as a memory drive on\r\n"\
                     "host PC. Error and info messages will be printed on J-Link RTT Viewer.\r\n\n\n"
#elif defined (BOARD_RA8E2_EK)
#define EP_INFO      "\r\nThis example project demonstrates basic functionalities of USB Peripheral Mass Storage\r\n"\
                     "Class (PMSC) driver with FreeRTOS on Renesas RA MCUs based on Renesas FSP.\r\n"\
                     "FreeRTOS uses the underlying Block media driver which utilizes USB driver. The PMSC driver\r\n"\
                     "internally configures memory device (on board OSPI Flash) to pop up as a memory drive on\r\n"\
                     "host PC. Error and info messages will be printed on J-Link RTT Viewer.\r\n\n\n"
#else
#define EP_INFO      "\r\nThis example project demonstrates basic functionalities of USB Peripheral Mass Storage\r\n"\
                     "Class (PMSC) driver with FreeRTOS on Renesas RA MCUs based on Renesas FSP.\r\n"\
                     "FreeRTOS uses the underlying Block media driver which utilizes USB driver. The PMSC driver\r\n"\
                     "internally configures memory device (sdcard) to pop up as a memory drive on host PC.\r\n"\
                     "Error and info messages will be printed on J-Link RTT Viewer.\r\n\n\n"
#endif

#define NUM_STRING_DESCRIPTOR               (7U)
#define WAIT_TIME                           (0U)
#define TASK_DELAY                          (1U)

/* Function declaration */
void deinit_usb(void);

#endif /* USB_PMSC_H_ */
