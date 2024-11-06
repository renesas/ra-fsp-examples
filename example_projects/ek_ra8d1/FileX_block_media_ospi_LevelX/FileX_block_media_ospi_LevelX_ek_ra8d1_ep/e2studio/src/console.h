/**********************************************************************************************************************
 * File Name    : console.h
 * Description  : Contains data structures and functions used in console_thread_entry.c
 *********************************************************************************************************************/
/**********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
**********************************************************************************************************************/

#ifndef CONSOLE_H_
#define CONSOLE_H_

#include "common_utils.h"
#include "terminal.h"
#include "filex.h"

#define CONSOLE_THREAD_SLEEP_TICK       (1U)
#define PRINT_EP_BANNER_TICK            (1U)
#define PRINT_EP_INFO_TICK              (1U)

#define SELECT_FLASH_MEDIA_OPERATION    (1U)
#define SELECT_DIR_OPERATION            (2U)
#define SELECT_FILE_OPERATION           (3U)
#define SELECT_SECTOR_OPERATION         (4U)

#define SELECT_ENTIRE_FLASH_ERASE       (1U)
#define SELECT_MEDIA_FORMAT             (2U)
#define SELECT_MEDIA_OPEN               (3U)
#define SELECT_FLASH_DEFRAGMENT         (4U)
#define SELECT_MEDIA_GO_BACK            (5U)

#define SELECT_DIR_CREATE               (1U)
#define SELECT_DIR_PROPERTY             (2U)
#define SELECT_DIR_DELETE               (3U)
#define SELECT_DIR_GO_BACK              (4U)

#define SELECT_FILE_CREATE              (1U)
#define SELECT_FILE_WRITE               (2U)
#define SELECT_FILE_READ                (3U)
#define SELECT_FILE_DELETE              (4U)
#define SELECT_FILE_GO_BACK             (5U)

#define SELECT_SECTOR_WRITE             (1U)
#define SELECT_SECTOR_READ              (2U)
#define SELECT_SECTOR_ERASE             (3U)
#define SELECT_SECTOR_GO_BACK           (4U)


#define PRINT_MENU(menu)                (terminal_send_output_queue(TERMINAL_OUTPUT_APP_MENU, sizeof(UINT *), &(menu)))


#endif /* CONSOLE_H_ */
