/***********************************************************************************************************************
* File Name    : common_app.h
* Description  : Contains macros, data structures, and functions commonly used in the EP
***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef COMMON_APP_H_
#define COMMON_APP_H_

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "common_utils.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
/* Macros for the EP information. */
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

#if (BSP_CFG_RTOS == 1U)
#define LED_CONTROL_THREAD_SLEEP_TICK   (1U)
#define TERMINAL_THREAD_SLEEP_TICK      (1U)
/* Macros for printing info, handling errors, and trapping errors. */
#define PRINT_EP_BANNER                 (terminal_send_output_queue(TERMINAL_OUTPUT_EP_BANNER, NULL, RESET_VALUE))
#define PRINT_EP_INFO                   (terminal_send_output_queue(TERMINAL_OUTPUT_EP_INFO, NULL, RESET_VALUE))
#define PRINT_EP_MENU                   (terminal_send_output_queue(TERMINAL_OUTPUT_EP_MENU, NULL, RESET_VALUE))
#define PRINT_INFO_STR(str)             (terminal_send_output_queue(TERMINAL_OUTPUT_APP_INFO_STR, (str), strlen(str)))
#define PRINT_ERR_STR(str)              (terminal_send_output_queue(TERMINAL_OUTPUT_APP_ERR_STR, (str), strlen(str)))
#define PRINT_ENTRY_INFO(info)          (terminal_send_output_queue(TERMINAL_OUTPUT_APP_ENTRY_INFO,\
                                                                     &(info), sizeof(entry_info_t)))
#define PRINT_DIR_PROPERTY(property)    (terminal_send_output_queue(TERMINAL_OUTPUT_APP_DIR_PROPERTY,\
                                                                     &(property), sizeof(dir_property_t)))

#define INFO_RET(cond, str)             ({\
    if (cond)\
    {\
        terminal_send_output_queue(TERMINAL_OUTPUT_INFO_STR, (str), strlen(str));\
        return (0U);\
    }\
})
#define ERROR_RET(cond, err, str)       ({\
    if (cond)\
    {\
        terminal_send_output_queue(TERMINAL_OUTPUT_APP_ERR_STR, (str), strlen(str));\
        return (err);\
    }\
})
#define ERROR_TRAP(cond, err, str)      ({\
    if (cond)\
    {\
        terminal_send_output_queue(TERMINAL_OUTPUT_APP_ERR_STR, (str), strlen(str));\
        terminal_send_output_queue(TERMINAL_OUTPUT_APP_ERR_TRAP, &(err), sizeof(uint32_t));\
    }\
})
#endif

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/
/* Enumeration for terminal event IDs */
typedef enum terminal_event_id
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
}terminal_event_id_t;

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

#endif /* COMMON_APP_H_ */
