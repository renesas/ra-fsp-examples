/***********************************************************************************************************************
 * File Name    : netx_duo_ftp_client_ep.h
 * Description  : Contains macros, data structures and functions used netx_duo_ftp_client_ep
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


#ifndef NETX_DUO_FTP_CLIENT_EP_H_
#define NETX_DUO_FTP_CLIENT_EP_H_
#include "ftp_client_thread.h"
#include "common_utils.h"
#include "user_app.h"

/*******************************************************************************************************************//**
 * @addtogroup ftp_client_ep
 * @{
 **********************************************************************************************************************/

/******************************************************************************
 Macro definitions
 ******************************************************************************/
/* To display EP info on the RTT viewer */
#define EP_INFO        "\r\nThis project demonstrates the basic functionality of Netx Duo FTP client\r\n"\
                       "with ethernet driver on Renesas RA MCUs based on Renesas FSP using AzureRTOS.\r\n"\
                       "The example project configures the FTP client to establish the connection with\r\n"\
                       "FTP server. The FTP client will perform read and write operation, on to the \r\n"\
                       "currently opened file on the FTP server. The status message and other relevant \r\n"\
                       "data will be displayed on JlinkRTTViewer\r\n"\

/* macros to print info, error and trap the error.*/
#define PRINT_INFO_STR(str)  (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_INFO_STR, sizeof(str), (str)))
#define PRINT_ERR_STR(str)   (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_STR, sizeof(str), (str)))
#define ERROR_TRAP(err)      (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_TRAP, sizeof(UINT *), &(err)))

#define MAX_COUNT_VALUE  (0xFF)       //Max value for counter
/******************************************************************************
 Global variables and functions
 ******************************************************************************/

/* Declare the function to run the FTP session. */
UINT run_ftp_client_session(NX_FTP_CLIENT *ftp_client_ptr, NX_PACKET_POOL *pool_ptr, CHAR *username, CHAR *password, NXD_ADDRESS *server_ip_address, CHAR *filename, UCHAR *file_buffer, UINT file_size, UINT timeout);


#endif /* NETX_DUO_FTP_CLIENT_EP_H_ */
/*******************************************************************************************************************//**
 * @} (end defgroup ftp_client_ep)
 **********************************************************************************************************************/
