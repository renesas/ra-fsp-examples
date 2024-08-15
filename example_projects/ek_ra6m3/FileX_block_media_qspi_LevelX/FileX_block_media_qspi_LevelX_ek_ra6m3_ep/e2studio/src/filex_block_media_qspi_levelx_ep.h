/***********************************************************************************************************************
 * File Name    : filex_block_media_qspi_levelx_ep.h
 * Description  : Contains macros, data structures and functions used in the filex_thread_entry.c
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef FILEX_BLOCK_MEDIA_QSPI_LEVELX_EP_H_
#define FILEX_BLOCK_MEDIA_QSPI_LEVELX_EP_H_

#define EP_INFO                             "\r\nThis Example project demonstrates how user can use FileX blockmedia QSPI with LevelX on RA MCU."\
		                                    "\r\nThe project will perform various file operations(Open, Close, Read, Write, Format, adding time stamp"\
		                                    "\r\nto the file) on the QSPI Flash to display the FileX with LevelX functionality using QSPI driver,"\
		                                    "\r\nbased on user input in RTT Viewer."

#define FILEX_LEVELX_BLOCKMEDIA_MENU        "\r\n\nFileX blockmedia QSPI with LevelX Main Menu options:"\
		                                    "\r\n1. Enter 1 to erase QSPI Flash device before using LevelX for the first time."\
		                                    "\r\n   It may take several minutes to complete."\
                                            "\r\n2. Enter 2 to format media."\
		                                    "\r\n3. Enter 3 to create a text file on QSPI Flash and write pre-defined data."\
		                                    "\r\n4. Enter 4 to create another text file on QSPI Flash, write and read pre-defined data."\
		                                    "\r\n   Compare the read and write data"\
		                                    "\r\nUser Input:  "

/* Enum for Menu Options */
typedef enum menu_option
{
    ERASE_QSPI_FLASH            = 1,
    FORMAT_MEDIA                = 2,
    FILE_1_WRITE                = 3,
    FILE_2_WRITE_READ_COMPARE   = 4,
}menu_option_t;

/* Macro for buffer size to read date and time */
#define DATE_TIME_MAX_LEN       (32)

/* Macro for buffer size containing file name */
#define MAX_FILE_NAME_SIZE      (32)

/* Macro for null character */
#define NULL_CHAR   ('\0')

/* Macros for sector information of media */
#define MEDIA_SECTOR_HEADS_VALUE (1U)
#define MEDIA_SECTORS_PER_HEAD    (1U)

/* Macros used in date and time calculations */
#define DATE_VALUE_CHECK         (2000U)
#define DATE_VALUE_SUB           (2400U)

/* Macro for buffer size of read/write data */
#define WRITE_ITEM_SIZE          (10240u)

/* Macros of months used in date calculation */
#define JAN_JUN_JUL (0x4A)
#define FEB         (0x46)
#define MAR_MAY     (0x4D)
#define APR_AUG     (0x41)
#define SEP         (0x53)
#define OCT         (0x4F)
#define NOV         (0x4E)
#define DEC         (0x44)

/* Macros for periodic delay and periodic line feed */
#define PERIODIC_PRINT_OUT      (0x20000U)
#define PERIODIC_LINE_FEED      (0x1000000U)

/* Enum for ASCII code */
typedef enum ascii_code
{
    ASCII_LOWER_A   = 0x61,
    ASCII_LOWER_N   = 0x6E,
    ASCII_LOWER_R   = 0x72,
    ASCII_LOWER_P   = 0x70,
}ascii_code_t;

/* Structure to store time */
typedef struct st_time_format_params
{
    UINT month;
    UINT date;
    UINT year;
    UINT hour;
    UINT min;
    UINT sec;
}time_format_t;

/* Enum for month values */
typedef enum e_month_numbers
{
    JANUARY    = 1,
    FEBRUARY   = 2,
    MARCH      = 3,
    APRIL      = 4,
    MAY        = 5,
    JUNE       = 6,
    JULY       = 7,
    AUGUST     = 8,
    SEPTEMBER  = 9,
    OCTOBER    = 10,
    NOVEMBER   = 11,
    DECEMBER   = 12,
}month_numbers_t;

/* Macro for filling buffer with pre-defined data.
 * Variable used for filling buffer will have size defined maximum to WRITE_ITEM_SIZE.
 */
#define UPDATE_BUFFER(buffer)       ({for(uint16_t count = RESET_VALUE; count < WRITE_ITEM_SIZE; count++)\
                                        (buffer)[count] = (uint8_t)count;})

/* Macros to print info, error and trap the error.*/
#define PRINT_INFO_STR(str)  (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_INFO_STR, sizeof(str), (str)))
#define PRINT_ERR_STR(str)   (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_STR, sizeof(str), (str)))
#define ERROR_TRAP(err)      (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_TRAP, sizeof(UINT *),&(err)))


#endif /* FILEX_BLOCK_MEDIA_QSPI_LEVELX_EP_H_ */
