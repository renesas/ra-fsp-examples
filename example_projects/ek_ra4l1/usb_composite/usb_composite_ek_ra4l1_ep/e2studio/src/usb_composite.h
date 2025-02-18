/***********************************************************************************************************************
 * File Name    : usb_composite.h
 * Description  : Contains macros, data structures and function declaration used in EP
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef USB_COMPOSITE_H_
#define USB_COMPOSITE_H_

#define LINE_CODING_LENGTH  (0x07U)                /* Line coding length */
#define DATA_LEN            (512U)                 /* Data Length */
#define TOGGLE_DELAY        (5U)                   /* Led toggle delay */

#define EP_INFO             "\r\nThis project demonstrates the basic functionality of USB Composite driver on\r\n" \
                            "Renesas RA MCUs based on Renesas FSP using FreeRTOS. In this example, the application\r\n"\
                            "will configure the MCU as a CDC device with Host machine as well as a Mass storage \r\n" \
                            "a Mass storage device. Tera term is used to communicate with the board. Data written\r\n"\
                            "by the tera term will be echoed back on the terminal by RA board. User at the same\r\n"\
                            "time can read/write the data from/to the board using host machine. This EP requires \r\n"\
                            "external hardware connections and jumper setting modification prior to operation.\r\n"\
                            "Refer to the accompanying readme.txt file for more details.\r\n"

/* Function declaration */
fsp_err_t process_usb_events(void);                /* Process usb events */
fsp_err_t usb_status_request(void);                /* Process usb status request */
void handle_error(fsp_err_t err, char * err_str);  /* Handle error */

#endif /* USB_COMPOSITE_H_ */
