/***********************************************************************************************************************
 * File Name    : usb_pmsc_example_app.h
 * Description  : Public interface for the USB PMSC example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef USB_PMSC_EXAMPLE_APP_H_
#define USB_PMSC_EXAMPLE_APP_H_

#include "usb_pmsc_ep.h"

#define EP_VERSION   "1.0"

#define MODULE_NAME  "USB Peripheral Mass Storage Class"

#define BANNER_INFO  "\r\n********************************************************************************"\
                     "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module   *"\
                     "\r\n*   Example Project Version "EP_VERSION"                                                *"\
                     "\r\n*   Flex Software Pack Version %d.%d.%d                                           *"\
                     "\r\n********************************************************************************"\
                     "\r\nRefer to readme.txt file for more details on Example Project and"\
                     "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO      "\r\nThe example project demonstrates basic functionalities of USB PMSC driver"\
		             "\r\nwith FreeRTOS on Renesas RA MCUs based on Renesas FSP. FreeRTOS uses the"\
					 "\r\nunderlying Block Media driver. The PMSC driver internally configures memory"\
					 "\r\ndevice ("MEMORY_DEV") to pop up as a memory drive on host PC. Error and"\
					 "\r\ninfo messages will be printed on terminal.\r\n"

#if defined (BOARD_RA8T1_MCK) || defined (BOARD_RA8T2_MCK)
#define MEMORY_DEV   "MicroSD Card"
#elif defined (BOARD_RA4L1_EK)
#define MEMORY_DEV   "On-board QSPI Flash"
#elif defined (BOARD_RA8E2_EK)
#define MEMORY_DEV   "On-board OSPI Flash"
#else
#define MEMORY_DEV   "SD Card"
#endif /* Specific board */

/* Single entry-point called in pmsc_thread_entry.c */
void usb_pmsc_example_app_run(void);
#endif /* USB_PMSC_EXAMPLE_APP_H_ */
