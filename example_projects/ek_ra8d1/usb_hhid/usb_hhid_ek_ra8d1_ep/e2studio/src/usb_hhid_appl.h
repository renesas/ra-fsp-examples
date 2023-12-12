/***********************************************************************************************************************
 * File Name    : usb_hhid_appl.h
 * Description  : Contains data structures and functions used in usb_hhid_appl.h.
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

#ifndef USB_HHID_APPL_H_
#define USB_HHID_APPL_H_

/* macro definition.*/
#define SET_PROTOCOL               (USB_HID_SET_PROTOCOL | USB_HOST_TO_DEV | USB_CLASS | USB_INTERFACE)
#define BOOT_PROTOCOL              (0U)
#define BUFSIZE                    (8U)
#define DATASIZE                   (12U)
#define TASK_DELAY                 (1U)
#define NULL_CHAR                  ('\0')
#define ROW_SIZE                   (40U)
#define COL_SIZE                   (25U)
#define SET_ZERO                   (0x0000)
#define ONE                        (1U)
#define INDEX                      (2U)
#define BUFF_SIZE                  (25U)
#define KEY_MOD_LSHIFT             (0x02)
#define KEY_MOD_RSHIFT             (0x20)



#define EP_INFO        "\r\nThis example project demonstrates basic functionalities of USB Host Human Interface Device \r\n"\
                       "Class (HHID) driver with FreeRTOS on Renesas RA MCUs based on Renesas FSP.\r\n"\
                       "USB HHID driver configures keyboard as a hhid device. on pressing key from\r\n"\
                       "the keyboard,received keycode is decoded and displayed on the Jlink RTTViewer.\r\n"\
                       "Error and info messages will be printed on JlinkRTTViewer.\r\n\n\n"

/* Function declaration */
void deinit_usb(void);

#endif /* USB_HHID_APPL_H_ */
