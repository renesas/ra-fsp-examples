/***********************************************************************************************************************
 * File Name    : smtp_client_ep.h
 * Description  : Contains macros from threads
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
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
#define WAIT_TIME              (1000U)
#define SEND                   (1U)          //RTT Input selection for write operation.
#define READ                   (2U)          //RTT Input selection for read operation.

/* macros to print info, error and trap the error.*/
#define PRINT_INFO_STR(str)  app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_INFO_STR, sizeof(str), str);
#define PRINT_ERR_STR(str)   app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_STR, sizeof(str), str);
#define ERROR_TRAP(err)      app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_TRAP, sizeof(UINT *), &err);

#endif /* SMTP_CLIENT_EP_H_ */
