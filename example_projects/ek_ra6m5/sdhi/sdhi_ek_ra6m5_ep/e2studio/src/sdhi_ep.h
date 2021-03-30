/***********************************************************************************************************************
 * File Name    : sdhi_ep.h
 * Description  : Contains data structures and functions used in sdhi_thread_entry.c
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
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#ifndef SDHI_EP_H_
#define SDHI_EP_H_

/* Macros used in sdhi Example Project */
#define EP_INFO        "\nThis example project demonstrates basic functionalities of sdhi \n"\
                       "driver with FreeRTOS+FAT file system on Renesas RA MCUs based on Renesas FSP.\n"\
                       "FreeRTOS+FAT uses the underlying Block media driver which utilizes SDHI driver to perform\n"\
                       "file operations on the SD Card devices.\n"\
                       "Error and info messages will be printed on JlinkRTTViewer.\n\n"

#define SDHI_EP_NOTE   "Please remove and re-insert the SD Card after executing Safely Remove & \n"\
                       "Initialize FreeRTOS+FAT command"

#define FILE_NAME               "ra_sdhi.txt"     // File name
#define WRITE_MODE              "w"               // File Write mode
#define READ_MODE               "r"               // File Read mode

#define SD_WRITE                (1u)              // RTT Input selection for write operation
#define SD_FORMAT               (2u)              // RTT Input selection for format operation
#define SAFELY_EJECT_SD_CARD    (3u)              // RTT Input selection for safely eject operation
#define SD_DISPLAY_CWD          (4u)              // To display CWD content
#define SD_INIT                 (5u)              // RTT Input selection for Re-initialization of SDHI
                                                  // Valid only after Safely_Eject option is executed.

#define BUFFER_SIZE             (10240u)          // The number of elements to read
#define FF_FS_ADD_FAILURE       (0)
#define ASCII_CHAR_A            (65)

/* SDHI Menu options */
#define SDHI_MENU              "\r\nSD Card Menu options\n"\
                                "1. Write 10k data from app_buffer to ra_sdhi.txt file\n"\
                                "2. Format SD Card Device\n"\
                                "3. Safely Remove the SD Card\n"\
                                "4. Display content of Current Working Directory\n"\
                                "5. Initialize FreeRTOS+FAT (Valid only after Safely_Eject option is executed)\n"

#endif /* SDHI_EP_H_ */
