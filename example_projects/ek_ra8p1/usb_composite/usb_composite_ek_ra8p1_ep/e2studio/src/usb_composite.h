/***********************************************************************************************************************
 * File Name    : usb_composite.h
 * Description  : Contains macros, data structures and function declaration used in the EP.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef USB_COMPOSITE_H_
#define USB_COMPOSITE_H_

#define LINE_CODING_LENGTH      (0x07U)     /* Line coding length */
#define DATA_LEN                (512U)      /* Data length */
#define TOGGLE_DELAY            (5U)        /* Led toggle delay */

#define EP_INFO         "\r\nThis project demonstrates the basic functionality of USB Composite driver on"\
                        "\r\nRenesas RA MCUs based on Renesas FSP using FreeRTOS. In this example, the application"\
                        "\r\nwill configure the MCU as a CDC device as well as a Mass storage device. Tera Term is"\
                        "\r\nused to communicate with the CDC interface of the MCU. Data written by the Tera Term"\
                        "\r\nwill be echoed back on the terminal by RA board. The user at the same time can read/write"\
                        "\r\nthe data from/to the Mass storage device using host machine.\r\n"

/* Function declarations */
fsp_err_t process_usb_events(void);                /* Process USB events */
fsp_err_t usb_status_request(void);                /* Process USB status request */
void handle_error(fsp_err_t err, char * err_str);  /* Handle error */

#endif /* USB_COMPOSITE_H_ */
