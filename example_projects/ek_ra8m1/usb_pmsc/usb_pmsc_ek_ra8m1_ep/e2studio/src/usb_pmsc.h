/***********************************************************************************************************************
 * File Name    : usb_pmsc.h
 * Description  : Contains data structures and functions used in usb_pmsc.h.
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
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#ifndef USB_PMSC_H_
#define USB_PMSC_H_

/* Macros used in usb_hmsc Example Project */
#define EP_INFO        "\r\nThis example project demonstrates basic functionalities of USB Peripheral Mass Storage \r\n"\
                       "Class (PMSC) driver with FreeRTOS on Renesas RA MCUs based on Renesas FSP.\r\n"\
                       "FreeRTOS uses the underlying Block media driver which utilizes USB  driver. The PMSC driver\r\n"\
                       "internally configures memory device (sdcard) to pop up as a memory drive on host PC.\r\n"\
                       "Error and info messages will be printed on JlinkRTTViewer.\r\n\n\n"

#define NUM_STRING_DESCRIPTOR               (7U)
#define WAIT_TIME                           (0U)
#define TASK_DELAY                          (1U)

/* Function declaration */
void deinit_usb(void);

#endif /* USB_PMSC_H_ */
