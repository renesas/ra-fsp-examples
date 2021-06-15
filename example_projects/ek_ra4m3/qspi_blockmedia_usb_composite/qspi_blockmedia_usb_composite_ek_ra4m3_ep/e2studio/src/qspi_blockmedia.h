/***********************************************************************************************************************
 * File Name    : qspi_blockmedia.h
 * Description  : Contains macros, data structures and function declaration used in EP
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
#ifndef QSPI_BLOCKMEDIA_H_
#define QSPI_BLOCKMEDIA_H_

#define LINE_CODING_LENGTH       (0x07U)                // Line coding length
#define DATA_LEN                 (4096U)                // Data Length


#define EP_INFO                 "\r\nThis project demonstrates the QSPI as block media on Renesas RA MCU's using the USB composite Class"\
                                "\r\ndriver(PMSC + PCDC). In this example, the application will configure the MCU as a CDC device as well as"\
                                "\r\na Mass storage device on Block media on QSPI. Users can use a Serial terminal(like Tera-term) to"\
                                "\r\ncommunicate with the board. Data written by the tera-term will be echoed back on the terminal by the"\
                                "\r\nRA board. The user at the same time can read/write the data from/to the board using a host machine.\r\n"


/* function declaration */
fsp_err_t process_usb_events(void);                /* Process usb events */
fsp_err_t usb_status_request(void);                /* Process usb status request */
void handle_error(fsp_err_t err, char * err_str);  /* handle error */

#endif /* QSPI_BLOCKMEDIA_H_ */
