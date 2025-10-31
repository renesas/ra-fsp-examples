/***********************************************************************************************************************
 * File Name    : filex.h
 * Description  : Contains data structures and functions used in filex.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef FILEX_H_
#define FILEX_H_

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>

#include "hal_data.h"
#include "bsp_api.h"
#include "tx_api.h"
#include "common_utils.h"

/***********************************************************************************************************************
 * General macros
 **********************************************************************************************************************/
#define RESET_VALUE        (0x00)
#define NULL_CHAR          ('\0')

#define EP_VERSION         ("1.0")
#define MODULE_NAME        "FileX block media sram"
#define BANNER_INFO        "\r\n********************************************************************************"\
                           "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module              *"\
                           "\r\n*   Example Project Version %s                                                *"\
                           "\r\n*   Flex Software Pack Version  %d.%d.%d                                          *"\
                           "\r\n********************************************************************************"\
                           "\r\nRefer to readme.txt file for more details on Example Project and" \
                           "\r\nFSP User's Manual for more information about "MODULE_NAME" module\r\n"

#define EP_INFO            "\r\nThe example project showcases how the FileX file system operates on the internal"\
                           "\r\nSRAM block media of the RA MCU. The project will perform various FileX file system"\
                           "\r\noperations based on the user's selection from menu options, such as media management"\
                           "\r\n(format), directory management (create, get properties, delete), and file management"\
                           "\r\n(create, write, read, delete). The terminal application is used as the user interface."\
                           "\r\nThe menu options and system messages (errors and information messages) will be printed"\
                           "\r\non the terminal application during the execution of the project.\r\n\r\n"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define ONE_SLEEP_TICK                          (1U)
#define TERMINAL_SLEEP_TICK                     (1U)

/* Macros for FileX */
#define G_FX_MEDIA_HEADS                (1)
#define G_FX_MEDIA_SECTORS_PER_TRACK    (1)

#define DIR_NAME_ONE                    "dir_one"
#define FILE_NAME_ONE                   "file_one.txt"

#define TRUNCATE_VALUE                  (0U)
#define SEEK_VALUE                      (0U)
#define OPERATION_TIME_OUT              (1000U)
#define WRITE_BUFFER_SIZE               (10240U)
#define READ_BUFFER_SIZE                (1024U)
#define WRITE_LINE_SIZE                 (128U)
#define WRITE_LINE_TEXT                 "The example project demonstrates the operation of the Azure FileX file system"\
                                        " on block media via the SRAM driver on the RA MCU\r\n"

#define FILEX_SLEEP_TICK                (1U)
#define ONE_BYTE                        (1U)

/* Macros for system date and time */
#define MONTH_STR_LEN                   (3U)
#define MONTH_INDEX                     (0U)
#define DATE_INDEX                      (4U)
#define YEAR_INDEX                      (7U)
#define HOUR_INDEX                      (0U)
#define MIN_INDEX                       (3U)
#define SEC_INDEX                       (6U)

/* Macro for main menu */
#define MENU_OPTION                 "\r\nMenu option:"\
                                    "\r\n  1. Format Media."\
                                    "\r\n  2. Create Directory."\
                                    "\r\n  3. Get Directory Properties."\
                                    "\r\n  4. Delete Directory."\
                                    "\r\n  5. Create Empty File."\
                                    "\r\n  6. Write To File."\
                                    "\r\n  7. Read From File."\
                                    "\r\n  8. Delete File."\
                                    "\r\n  User Input:  "

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/
/* Enumerate for the terminal event */
typedef enum terminal_event
{
    TERMINAL_INPUT_MESSAGE,
    TERMINAL_OUTPUT_EP_BANNER,
    TERMINAL_OUTPUT_EP_INFO,
    TERMINAL_OUTPUT_APP_INFO_STR,
    TERMINAL_OUTPUT_APP_ERR_STR,
    TERMINAL_OUTPUT_APP_ERR_TRAP,
    TERMINAL_OUTPUT_APP_MENU,
    TERMINAL_OUTPUT_APP_MEDIA_PROPERTY,
    TERMINAL_OUTPUT_APP_MEDIA_VOLUME_INFO,
    TERMINAL_OUTPUT_APP_ENTRY_INFO,
    TERMINAL_OUTPUT_APP_DIR_PROPERTY
}terminal_event_t;

/* Enumerate for month values */
typedef enum e_month
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
}month_t;

/* Enumerate for the Menu screen */
typedef enum e_menu
{
    MENU_MAIN = 0,
    MENU_MEDIA,
    MENU_DIR,
    MENU_FILE,
    MENU_MAX
} menu_t;

/* Structure used to exchange information between application thread and terminal thread */
typedef struct terminal_msg
{
    terminal_event_t id;
    uint32_t size;
    uint32_t time;
    CHAR msg[];
}terminal_msg_t;

/* Structure to store date and time */
typedef struct st_time
{
    UINT month;
    UINT date;
    UINT year;
    UINT hour;
    UINT min;
    UINT sec;
}time_app_t;

/* Structure to store entry details */
typedef struct st_entry_info
{
    CHAR name[FX_MAX_LONG_NAME_LEN];
    UINT attr;
    ULONG size;
    time_app_t time;
}entry_info_t;

/* Structure to store directory property details */
typedef struct st_dir_property
{
    UINT file;
    UINT subdir;
    ULONG size;
}dir_property_t;

/* Macros to print info, error and trap the error */
#define PRINT_INFO_STR(str)             (term_send_output_queue(TERMINAL_OUTPUT_APP_INFO_STR, (str), sizeof(str)))
#define PRINT_ERR_STR(str)              (term_send_output_queue(TERMINAL_OUTPUT_APP_ERR_STR, (str), sizeof(str)))
#define PRINT_ENTRY_INFO(info)          (term_send_output_queue(TERMINAL_OUTPUT_APP_ENTRY_INFO,\
		                                 &(info), sizeof(entry_info_t)))
#define PRINT_DIR_PROPERTY(property)    (term_send_output_queue(TERMINAL_OUTPUT_APP_DIR_PROPERTY,\
		                                 &(property), sizeof(dir_property_t)))
#define TERM_ERR_PRINT(cond, str)       ({\
                                            if (cond)\
                                            {\
                                                term_send_output_queue(TERMINAL_OUTPUT_APP_ERR_STR, (str), strlen(str));\
                                            }\
                                        })

#define TERM_ERR_RET(cond, err, str)    ({\
                                            if (cond)\
                                            {\
                                                term_send_output_queue(TERMINAL_OUTPUT_APP_ERR_STR, (str), sizeof(str));\
                                                return (err);\
                                            }\
                                        })

#define TERM_ERR_TRAP(cond, err, str)   ({\
                                            if (cond)\
                                            {\
                                                term_send_output_queue(TERMINAL_OUTPUT_APP_ERR_STR, (str), strlen(str));\
                                                term_send_output_queue(TERMINAL_OUTPUT_APP_ERR_TRAP, &(err), sizeof(uint32_t));\
                                                tx_thread_suspend(tx_thread_identify());\
                                            }\
                                        })
/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/
/* Enumerate for user requests */
typedef enum e_request_event
{
    REQUEST_MEDIA_FORMAT = 1,
    REQUEST_DIR_CREATE,
    REQUEST_DIR_PROPERTY,
    REQUEST_DIR_DELETE,
    REQUEST_FILE_CREATE,
    REQUEST_FILE_WRITE,
    REQUEST_FILE_READ,
    REQUEST_FILE_DELETE,
} request_event_t;

/***********************************************************************************************************************
 * Public function prototypes
 **********************************************************************************************************************/
UINT terminal_framework_init(void);
UINT terminal_input_handle(void);
UINT terminal_output_handle(void);
UINT terminal_thread_init_check(void);

UINT file_system_init(void);
UINT file_system_operation(UINT request);

#endif /* FILEX_H_ */
