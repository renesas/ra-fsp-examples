/***********************************************************************************************************************
 * File Name    : smtp_client_ep.h
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
#ifndef SMTP_CLIENT_EP_H_
#define SMTP_CLIENT_EP_H_

#define EP_INFO "\r\nThis project demonstrates the basic functionalities of Netx SMTP Client with Ethernet driver\r\n"\
                "on Renesas RA MCUs based on Renesas FSP using AzureRTOS. The project creates SMTP client and transfer e-mail message\r\n"\
                "to local configured SMTP server. Based on user input from RTTViewer, EP Performs.\r\n"\
                "1.Enter 1 to send email message\r\n"\
                "2.Enter 2 to view pre-configured email message\r\n"\
                "On successful completion of each operation, success message will be printed\r\n"\
                "on RTT viewer. Error and info messages will be printed on JlinkRTTViewer.\r\n\n\n"


#define SMTP_CLIENT_MENU          "\r\nSMTP Client Main Menu options:"\
                                "\r\n1. Enter 1 to send email message."\
                                "\r\n2. Enter 2 to view Pre-configured email message."\
                                "\r\nUser Input:  "

#define SUBJECT_LINE      "SMTP example"
#define MAIL_BODY         "This is a test message for testing the testing of SMTP client azure API"

#define LINK_ENABLE_WAIT_TIME  (1000U)
#define WAIT_TIME              (300U)
#define SEND                   (1U)          //RTT Input selection for write operation.
#define READ                   (2U)          //RTT Input selection for read operation.

/* macros to print info, error and trap the error.*/
#define PRINT_INFO_STR(str)  app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_INFO_STR, sizeof(str), str);
#define PRINT_ERR_STR(str)   app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_STR, sizeof(str), str);
#define ERROR_TRAP(err)      app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_TRAP, sizeof(UINT *), &err);

#endif /* SMTP_CLIENT_EP_H_ */
