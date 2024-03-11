/***********************************************************************************************************************
 * File Name    : usb_phid_ep.h
 * Description  : Contains macros from threads
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
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
#ifndef USB_PHID_EP_H_
#define USB_PHID_EP_H_

/* Macro definitions */
#define USB_RECEIVE_REPORT_DESCRIPTOR    (76U)              // report descriptor
#define USB_RECEIVE_HID_DESCRIPTOR       (9U)               // hid descriptor
#define DATA_LEN                         (8U)               // data length
#define SIZE_NUM                         (2U)               // numlock data size
#define USAGE_ID_A                       (0x04)             // keyboard usage id of A
#define USAGE_ID_0                       (0x27)             // keyboard usage id of 0
#define DELAY                            (100U)             // delay for write
#define INDEX_THIRD                      (2U)               // third index for usb buffer
#define TASK_DELAY                       (1U)               // delay for task
#define CD_LENGTH                        (18U)              // config descriptor length
#define IDLE_VAL_INDEX                   (1U)               // index value
#define BUFF_SIZE                        (16U)              // size of send data buffer
#define ALIGN                            (4U)               // alignment
#define DELAY_3_SEC                      (3000U)            // 3 seconds delay

#define EP_INFO "\r\nThis project demonstrates the basic functionalities of USB PHID driver on \r\n"\
                "Renesas RA MCUs based on Renesas FSP. The project configures RA board as a Keyboard.\r\n"\
                "On pressing any key from RTTViewer, MCU sends the host PC characters from a - z & numbers 0 - 9.\r\n"



#endif /* USB_PHID_EP_H_ */
