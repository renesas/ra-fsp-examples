/***********************************************************************************************************************
 * File Name    : sdhi_ep.h
 * Description  : Contains data structures and functions used in sdhi_thread_entry.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
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
