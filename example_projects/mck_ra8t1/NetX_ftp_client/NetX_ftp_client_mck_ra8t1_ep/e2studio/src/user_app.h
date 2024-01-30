/***********************************************************************************************************************
 * File Name    : user_app.h
 * Description  : Contains user configurable macros which is used in ftp_client_therad.c
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

#ifndef USER_APP_H_
#define USER_APP_H_


/******************************************************************************
 Macro definitions
 ******************************************************************************/
/** User Configurable Macros **/
/** @brief User has to provide the server IP address from where the FileZilla server running. For FTP server
 * setup please refer ftp_client notes under Special Topics.
 * User can create their own credentials viz., "username" and "password" according to their settings,
 * same it should modify in the respective macro fields.
 */
#define USERNAME        "rvc"                       //User configurable credential i.e., username
#define PASSWORD        "Pass1234"                       //User configurable credential i.e., password

/**@brief User configurable (ipv4) address. IP address should be provided from where the FTP server is running
 * NOTE: Current IP address is provided according to our test environment.*/
#define SERVER_ADDRESS  (IP_ADDRESS(10,231,8,90))


/* Macros for FTP file settings */
#define FTP_TIMEOUT     (5)              /* Define timeout in seconds for FTP operations */
#define FILE_SIZE       (1400)                /* Define size of file in bytes; */
#define FILE_NAME       "test_file.txt"      /* File name in txt format */

#endif /* USER_APP_H_ */
