/***********************************************************************************************************************
 * File Name    : wifi_ep.h
 * Description  : Define macros and functions used in a file named wifi_ep.c
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
 * Copyright (C) 2024 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#ifndef WIFI_H_
#define WIFI_H_

#define EP_INFO                 "\r\n This project demonstrates the functionalities of the HTTP Client within the DA16200"\
		                        "\r\n module employing the FSP Integrated Onchip DA16XXX Middleware driver, and leveraging"\
		                        "\r\n FreeRTOS on Renesas RA MCUs EP uses the DA16200MOD module to connect to an access"\
                                "\r\n point and communicate with a web server using HTTP methods(GET, PUT, POST). After"\
                                "\r\n sending each request, EP waits for the HTTP response and prints it to the RTT Viewer."\
                                "\r\n Additionally, status and failure messages are displayed on the RTT Viewer.\r\n"

#define PRINT_HTTP_MENU         "\r\nPlease select the index of HTTP method to be tested from below: "\
                                "\r\n 1. GET Request"\
                                "\r\n 2. PUT Request"\
                                "\r\n 3. POST Request\r\n"

#define PRINT_WIFI_MENU         "\r\n Menu:"\
                                "\r\n Press 1 to scan for WiFi Access Points"\
                                "\r\n Press 2 to enter WiFi Access Point's credentials"\
                                "\r\n User Input: \r\n"

#define MAX_WIFI_SCAN_RESULTS               (10)
#define HTTP_ONCHIP_DA16XXX_BUFFER_SIZE     (2048)
#define HTTP_GET_SELECTION                  (1)
#define HTTP_PUT_SELECTION                  (2)
#define HTTP_POST_SELECTION                 (3)
#define HTTP_GET_ENDPOINT                   ("http://httpbin.org/get")
#define HTTP_PUT_ENDPOINT                   ("http://httpbin.org/put")
#define HTTP_POST_ENDPOINT                  ("http://httpbin.org/post")
#define HTTP_PUT_REQUEST_BODY_SAMPLE        ("HTTP-Client PUT method sample test!")
#define HTTP_POST_REQUEST_BODY_SAMPLE       ("HTTP-Client POST method sample test!")
#define INDEX_ZERO                          (0)
#define NULL_CHARACTER                      ('\0')
#define TAB_CHARACTER                       ('\t')
#define ZERO_ASCII                          ('0')
#define TWO_ASCII                           ('2')
#define NINE_ASCII                          ('9')
#define NEWLINE_CHARACTER                   ('\n')
#define CARRIAGE_RETURN_CHARACTER           ('\r')

/* Macros for array indexing */
#define BYTES_RECEIVED_ZERO                 (0U)
#define INDEX_ONE                           (1U)
#define INDEX_TWO                           (2U)
#define INDEX_THREE                         (3U)
#define BUFF_LEN                            (32U)
#define SCAN_AP                             (1)
#define ENTER_SSID                          (2)
#define WIFI_SECURITY_OPEN                  (0)
#define WIFI_SECURITY_WPA                   (1)

void user_input(char *user_buff);
WIFIReturnCode_t wifi_config(void);
void wifi_handle_error (WIFIReturnCode_t err,  const char * err_str);

#endif /* WIFI_H_ */
