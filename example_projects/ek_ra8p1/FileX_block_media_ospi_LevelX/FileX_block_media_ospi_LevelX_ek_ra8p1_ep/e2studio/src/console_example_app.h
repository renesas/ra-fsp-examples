/***********************************************************************************************************************
 * File Name    : console_example_app.h
 * Description  : Contains macros and functions used in console_example_app.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ***********************************************************************************************************************/

#ifndef CONSOLE_EXAMPLE_APP_H_
#define CONSOLE_EXAMPLE_APP_H_

#include "filex_ep.h"

/* Macro for the console thread sleep tick */
#define CONSOLE_THREAD_SLEEP_TICK    (1U)
#define PRINT_EP_BANNER_TICK         (1U)
#define PRINT_EP_INFO_TICK           (1U)

/* Macros for the flash media sub-menu selection */
#define SELECT_FLASH_MEDIA_OPERATION (1U)
#define SELECT_DIR_OPERATION         (2U)
#define SELECT_FILE_OPERATION        (3U)
#define SELECT_SECTOR_OPERATION      (4U)

/* Macros for the flash media operation selection */
#define SELECT_ENTIRE_FLASH_ERASE    (1U)
#define SELECT_MEDIA_FORMAT          (2U)
#define SELECT_MEDIA_OPEN            (3U)
#define SELECT_FLASH_DEFRAGMENT      (4U)
#define SELECT_MEDIA_GO_BACK         (5U)

/* Macros for the directory operation selection */
#define SELECT_DIR_CREATE            (1U)
#define SELECT_DIR_PROPERTY          (2U)
#define SELECT_DIR_DELETE            (3U)
#define SELECT_DIR_GO_BACK           (4U)

/* Macros for the file operation selection */
#define SELECT_FILE_CREATE           (1U)
#define SELECT_FILE_WRITE            (2U)
#define SELECT_FILE_READ             (3U)
#define SELECT_FILE_DELETE           (4U)
#define SELECT_FILE_GO_BACK          (5U)

/* Macros for the sector operation selection */
#define SELECT_SECTOR_WRITE          (1U)
#define SELECT_SECTOR_READ           (2U)
#define SELECT_SECTOR_ERASE          (3U)
#define SELECT_SECTOR_GO_BACK        (4U)

/* Function declaration */
void console_thread_entry_run(void);

#endif /* CONSOLE_EXAMPLE_APP_H_ */
