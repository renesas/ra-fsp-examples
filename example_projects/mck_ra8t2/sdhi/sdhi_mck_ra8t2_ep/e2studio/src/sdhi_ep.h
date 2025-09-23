/***********************************************************************************************************************
 * File Name    : sdhi_ep.h
 * Description  : Contains data structures and functions used in sdhi_thread_entry.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef SDHI_EP_H_
#define SDHI_EP_H_

/* Macro for used card */
#if defined (BOARD_RA8T1_MCK) || defined (BOARD_RA8T2_MCK)
#define CARD_TYPE     "MicroSD"
#else
#define CARD_TYPE     "SD"
#endif

/* Macros used in SDHI example project */
#define EP_INFO        "\r\nThis example project demonstrates basic functionalities of SDHI driver with"\
                       "\r\nFreeRTOS+FAT file system on Renesas RA MCUs based on Renesas FSP. FreeRTOS+FAT"\
                       "\r\nuses the underlying Block media driver which utilizes SDHI driver to perform"\
                       "\r\nfile operations on the SD Card (except for MCK-RA8T1, MCK-RA8T2 which use MicroSD Card)."\
                       "\r\nError and info messages will be printed on J-Link RTT Viewer.\r\n\r\n"

#define SDHI_EP_NOTE   "\r\nPlease remove and re-insert the "CARD_TYPE" Card after executing Safely Remove & "\
                       "Initialize FreeRTOS+FAT command\r\n"

#define FILE_NAME               "ra_sdhi.txt"     /* File name */
#define WRITE_MODE              "w"               /* File write mode */
#define READ_MODE               "r"               /* File read mode */

#define SD_WRITE                (1u)              /* RTT input selection for write operation */
#define SD_FORMAT               (2u)              /* RTT input selection for format operation */
#define SAFELY_EJECT_SD_CARD    (3u)              /* RTT input selection for safely eject operation */
#define SD_DISPLAY_CWD          (4u)              /* To display CWD content */
#define SD_INIT                 (5u)              /* RTT input selection for re-initialization of SDHI */
                                                  /* Valid only after Safely eject option is executed */

#define BUFFER_SIZE             (10240u)          /* The number of elements to read */
#define FF_FS_ADD_FAILURE       (0)
#define ASCII_CHAR_A            (65)

/* SDHI menu options */
#define SDHI_MENU               "\r\n"CARD_TYPE" Card Menu options\r\n"\
                                "1. Write 10k data from app_buffer to ra_sdhi.txt file\r\n"\
                                "2. Format "CARD_TYPE" Card Device\r\n"\
                                "3. Safely Remove the "CARD_TYPE" Card\r\n"\
                                "4. Display content of Current Working Directory\r\n"\
                                "5. Initialize FreeRTOS+FAT (Valid only after Safely_Eject option is executed)\r\n"

#endif /* SDHI_EP_H_ */
