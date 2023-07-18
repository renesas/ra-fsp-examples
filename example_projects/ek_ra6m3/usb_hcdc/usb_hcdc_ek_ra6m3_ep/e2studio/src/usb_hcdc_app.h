/***********************************************************************************************************************
 * File Name    : usb_hcdc_app.h
 * Description  : Contains data structures and functions used in usb_hcdc_app.h.
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


#ifndef USB_HCDC_APP_H_
#define USB_HCDC_APP_H_
/** Macros definitions **/

#define EP_INFO        "\r\nThis example project demonstrates basic functionalities of USB Host Communication Device \r\n"\
                       "Class (HCDC) driver on Renesas RA MCUs using 2 RA Boards. The Board 1(with USB HCDC Example  \r\n"\
                       "running on it)communicates with Board2(with USB PCDC Example project running). Board 1 initiates\r\n"\
                       "the communication by sending commands to Board 2 and Board 2 responds by sending the data.\r\n"\
                       "Board1 prints the received data on the RTTViewer.\r\n\n\n"

/** Menu selection keys **/
#define KIT_INFO              ('1')
#define NEXT_STEPS            ('2')
#define CARRIAGE_RETURN       ('\r')


#define SET_LINE_CODING             (USB_CDC_SET_LINE_CODING | USB_HOST_TO_DEV | USB_CLASS | USB_INTERFACE)
#define GET_LINE_CODING             (USB_CDC_GET_LINE_CODING | USB_DEV_TO_HOST | USB_CLASS | USB_INTERFACE)
#define SET_CONTROL_LINE_STATE      (USB_CDC_SET_CONTROL_LINE_STATE | USB_HOST_TO_DEV | USB_CLASS | USB_INTERFACE)
#define LINE_CODING_LENGTH          (0x07U)
#define VALUE_ZERO                  (0x0000U)
#define NO_WAIT_TIME                (0)
#define CDC_READ_DATA_LEN           (512)
#define CDC_WRITE_DATA_LEN          (1)
#define ZERO_INDEX                  (0)

/** Function declarations **/
void usb_hcdc_task(void);


#endif /* USB_HCDC_APP_H_ */
