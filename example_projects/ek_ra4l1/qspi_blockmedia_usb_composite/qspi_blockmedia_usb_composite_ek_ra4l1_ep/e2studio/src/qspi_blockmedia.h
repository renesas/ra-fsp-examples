/***********************************************************************************************************************
 * File Name    : qspi_blockmedia.h
 * Description  : Contains macros, data structures and function declaration used in EP
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef QSPI_BLOCKMEDIA_H_
#define QSPI_BLOCKMEDIA_H_

#define LINE_CODING_LENGTH      (0x07U)            /* Line coding length */
#define DATA_LEN                (4096U)            /* Data Length */

#define EP_INFO                 "\r\nThis project demonstrates the QSPI as block media on Renesas RA MCU's using the "\
                                "\r\nUSB composite Class driver (PMSC + PCDC). In this example, the application will "\
                                "\r\nconfigure the MCU as a CDC device as well as a Mass storage device on Block media"\
                                "\r\non QSPI. Users can use a Serial terminal (like Tera-term) to communicate with the"\
                                "\r\nboard. Data written by the tera-term will be echoed back on the terminal by the "\
                                "\r\nRA board. The user at the same time can read/write the data from/to the board "\
                                "\r\nusing a host machine.\r\n"

/* function declaration */
fsp_err_t process_usb_events(void);                /* Process usb events */
fsp_err_t usb_status_request(void);                /* Process usb status request */
void handle_error(fsp_err_t err, char * err_str);  /* handle error */

#endif /* QSPI_BLOCKMEDIA_H_ */
