 /***********************************************************************************************************************
  * File Name    : usbx_otg_cdc.h
  * Description  : Contains macros, data structures and functions used usbx_otg_cdc.h
  ***********************************************************************************************************************/
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
 * Copyright (C) 2022 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/


#ifndef USBX_OTG_CDC_H_
#define USBX_OTG_CDC_H_

/*******************************************************************************************************************//**
 * @addtogroup usbx_otg_cdc_ep
 * @{
 **********************************************************************************************************************/
/******************************************************************************
 * Macro definitions
 *****************************************************************************/
/* To display EP info on the RTT viewer */
#define EP_INFO        "\r\nThis project demonstrates the basic functionalities of USBX OTG CDC driver on Renesas RA "\
		        "\r\nMCUs based on Renesas FSP using Azure RTOS. Two RA MCU boards(Device-A and Device-B) will be "\
				"\r\nconnected to back to back using USB OTG Board(EB-USB-OTG) and its associated OTG cable. "\
				"\r\nInitially Device-A will act as a CDC host device and Device-B will act as a CDC peripheral device. "\
				"\r\nThe host (Device-A) will send data to the peripheral(Device-B) and peripheral(Device-B) will echo "\
				"\r\nthe data back to host(Device-A).Upon pressing (PUSH BTN)SW1 on Peripheral(Device-B), the EP will "\
				"\r\nswitch the roles of the two devices, i.e. host acting as peripheral and peripheral as host. "\
				"\r\nAfter switching roles, the new host device(Device-B) will send data to the new peripheral  "\
				"\r\ndevice(Device-A) and the peripheral device(Device-A) will echo the data back to host(Device-B).\r\n"


#define APL_USB_OTG_A_DEVICE
#define MEMPOOL_SIZE         (18432 * 2)
#define DATA_LEN             (512)
#define UX_OTG_VBUS_OFF_APL  (3)
#define UX_OTG_HNP           (4)
#define VALUE_108            (108)
#define VALUE_105            (105)
#define VALUE_98             (98)
#define VALUE_0              (0)
#define VALUE_1              (1U)
#define VALUE_2              (2)
#define VALUE_4              (4)
#define VALUE_7              (7)
#define VALUE_8              (8)
#define VALUE_9              (9)
#define VALUE_10             (10)
#define VALUE_15             (15)
#define VALUE_20             (20)
#define VALUE_30             (30)
#define VALUE_0x00           (0x00)
#define VALUE_0x1            (0x1)
#define VALUE_0x13           (0x13)
#define SIZE_192             (192u)

/* CDC-ACM configurations */
#define UX_RECEPTION_BUFFER_SIZE   (1024)
#define UX_XMIT_BUFFER_SIZE        (1024)
#define READ_LENGTH                (1024)

/* macros to print info, error and trap the error.*/
#define PRINT_INFO_STR(str)  app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_INFO_STR, sizeof(str), str);
#define PRINT_ERR_STR(str)   app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_STR, sizeof(str), str);
#define ERROR_TRAP(err)      app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_TRAP, sizeof(UINT *), &err);

#endif /* USBX_OTG_CDC_H_ */
/*******************************************************************************************************************//**
 * @} (end defgroup usbx_otg_cdc_ep)
 **********************************************************************************************************************/
