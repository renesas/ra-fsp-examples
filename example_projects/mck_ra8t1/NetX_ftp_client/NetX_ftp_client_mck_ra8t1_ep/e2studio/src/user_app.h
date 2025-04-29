/***********************************************************************************************************************
 * File Name    : user_app.h
 * Description  : Contains user configurable macros which is used in ftp_client_therad.c
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef USER_APP_H_
#define USER_APP_H_


/******************************************************************************
 Macro definitions
 ******************************************************************************/
/** User Configurable Macros **/
/** @brief User has to provide the server IP address from where the FileZilla server running. For FTP server
 * setup please refer ftp_client notes under Special Torvccs.
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
