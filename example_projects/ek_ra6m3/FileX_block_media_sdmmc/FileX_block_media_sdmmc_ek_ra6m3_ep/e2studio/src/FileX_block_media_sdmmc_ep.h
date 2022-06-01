/***********************************************************************************************************************
 * File Name    : FileX_block_media_sdmmc_ep.h
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


#ifndef FILEX_BLOCK_MEDIA_SDMMC_EP_H_
#define FILEX_BLOCK_MEDIA_SDMMC_EP_H_

#define EP_INFO                        "\r\nThis Example project demonstrates how user can use FileX interface on blockmedia over SDHI driver on RA MCU."\
                                       "\r\nThe project will perform various file operations(Open, Close, Read, Write, Format, adding time stamp to the file)"\
                                       "\r\non the SD Card to display the FileX functionality using BlockMedia SDMMC driver, based on user input in RTT Viewer."

#define FILEX_BLOCKMEDIA_MENU          "\r\nFileX blockmedia Main Menu options:"\
                                       "\r\n1. Enter 1 to format SD Card."\
                                       "\r\n2. Enter 2 to create a directory on SD card after checking available space."\
                                       "\r\nCreate a text file inside the directory, write and read pre-defined data"\
                                       "\r\n3. Enter 3 to display SD card content."\
                                       "\r\nUser Input:  "
#define MAIL_BODY                      "This is a test message for testing the testing of filex_blockmedia API"
#define FORMAT_CARD                   (1U)
#define FILE_CREATION                 (2U)
#define DISPLAY_CONTENT               (3U)



#define MAX_BYTES 32
#define ARRAY_SIZE 20
/* ASCII code */
#define ASCII_ZERO_CODE 48
#define ASCII_CHAR_A    65
#define ASCII_LOWER_A   0x61
#define ASCII_LOWER_N   0x6E
#define ASCII_LOWER_R   0x72
#define ASCII_LOWER_P   0x70

/* MACRO for null character */
#define NULL_CHAR   '\0'

#define YEAR_SIZE                (4U)
#define TIME_FORMAT_SIZE         (2U)
#define TRUNCATION_VALUE         (0U)
#define DIVIDE_VALUE             (2U)
#define DEC_VALUE                (10U)
#define MEDIA_SECTOR_HEADS_VALUE (1U)
#define DATE_VALUE_CHECK         (2000U)
#define DATE_VALUE_SUB           (2400U)
#define ALLOCATION_SIZE          (20U)
#define WRITE_ITEM_SIZE          (10240u)
#define DIR_SPACE_REQUIRED       (512u)
#define TIMER_TICKS              (2000U)

#define JAN_JUN_JUL 0x4A
#define FEB         0x46
#define MAR_MAY     0x4D
#define APR_AUG     0x41
#define SEP         0x53
#define OCT         0x4F
#define NOV         0x4E
#define DEC         0x44

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

/* Structure to store file details */
typedef struct st_file_info
{
    CHAR directory_name[ARRAY_SIZE];
    CHAR attributes[ARRAY_SIZE];
    ULONG size;
    time_format_t time_stamp;
}file_info_t;

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

/* macros to print info, error and trap the error.*/
#define PRINT_INFO_STR(str)  app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_INFO_STR, sizeof(str), str);
#define PRINT_ERR_STR(str)   app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_STR, sizeof(str), str);
#define ERROR_TRAP(err)      app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_TRAP, sizeof(UINT *),&err);



#endif /* FILEX_BLOCK_MEDIA_SDMMC_EP_H_ */
