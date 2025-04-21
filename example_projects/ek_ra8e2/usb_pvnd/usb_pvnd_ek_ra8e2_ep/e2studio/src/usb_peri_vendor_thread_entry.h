/***********************************************************************************************************************
 * File Name    : usb_peri_vendor_thread_entry.h
 * Description  : Contains macros definitions.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#ifndef USB_PERI_VENDOR_THREAD_ENTRY_H_
#define USB_PERI_VENDOR_THREAD_ENTRY_H_

#define BUF_SIZE                (20)                /* Buffer size */
#define REQ_SIZE                (20)                /* Request buffer size */
#define USB_VALUE_FF            (0xFFU)             /* FF macro */
#define START_PIPE              (USB_PIPE1)         /* Start pipe number */
#define END_PIPE                (USB_PIPE9 + 1)     /* Total pipe */

/* For Vendor Class Request */
#define USB_SET_VENDOR_NO_DATA  (0x0000U)
#define USB_SET_VENDOR          (0x0100U)
#define USB_GET_VENDOR          (0x0200U)
#define SET_VENDOR_NO_DATA      (USB_SET_VENDOR_NO_DATA | USB_HOST_TO_DEV | USB_VENDOR | USB_INTERFACE)
#define SET_VENDOR              (USB_SET_VENDOR | USB_HOST_TO_DEV | USB_VENDOR | USB_INTERFACE)
#define GET_VENDOR              (USB_GET_VENDOR | USB_DEV_TO_HOST | USB_VENDOR | USB_INTERFACE)
#define DELAY                   (10U)               /* Delay for print */
#if defined (BOARD_RA6M3_EK) || defined (BOARD_RA6M3G_EK)
#define USB_APL_MXPS            (512U)              /* Max packet size high speed */
#else
#define USB_APL_MXPS            (64U)               /* Max packet size full speed */
#endif
#define EP_INFO     "\r\nThis project demonstrates the basic functionalities of USB Peripheral Vendor class driver\r\n"\
		            "on Renesas RA MCUs based on Renesas FSP. In this example, the application will configure the\r\n"\
		            "MCU as a Vendor Peripheral device. This Peripheral device will be connected to the USB Host\r\n"\
		            "Vendor device which is another RA board. After the enumeration is completed, the Vendor\r\n"\
		            "Peripheral board will read 15 Bytes of data from the Vendor Host board and write back the\r\n"\
		            "same data to the Vendor Host board. The Vendor Host and Vendor Peripheral application is\r\n"\
		            "designed to continuously transfer data between both the boards. The user will be able to see\r\n"\
		            "all the operation sequence and status on J-Link RTT Viewer.\r\n"

#endif /* USB_PERI_VENDOR_THREAD_ENTRY_H_ */
