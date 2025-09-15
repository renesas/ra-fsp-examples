/***********************************************************************************************************************
 * File Name    : console.h
 * Description  : Contains macros used in the EP
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef CONSOLE_H_
#define CONSOLE_H_

#include "common_utils.h"
#include "rtt.h"

#define CONSOLE_THREAD_SLEEP_TICK       (1U)
#define PRINT_EP_BANNER_TICK            (1U)
#define PRINT_EP_INFO_TICK              (1U)

#define SELECT_MEDIA_OPERATION          (1U)
#define SELECT_DIR_OPERATION            (2U)
#define SELECT_FILE_OPERATION           (3U)

#define SELECT_MEDIA_OPEN               (1U)
#define SELECT_MEDIA_PROPERTY           (2U)
#define SELECT_MEDIA_FORMAT             (3U)
#define SELECT_MEDIA_CLOSE              (4U)
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

#define PRINT_MENU(menu)                (send_data_to_rtt(RTT_OUTPUT_APP_MENU, sizeof(menu_t *), &(menu)))

#endif /* CONSOLE_H_ */
