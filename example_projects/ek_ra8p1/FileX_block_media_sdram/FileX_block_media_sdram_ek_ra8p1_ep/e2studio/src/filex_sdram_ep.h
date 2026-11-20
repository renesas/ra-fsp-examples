/***********************************************************************************************************************
 * File Name    : filex_sdram_ep.h
 * Description  : Contains macros, data structures, and functions used in filex_sdram_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef FILEX_SDRAM_EP_H_
#define FILEX_SDRAM_EP_H_

#include "hal_data.h"

/* Macro for the reset value */
#define RESET_VALUE                     (0x00)

/* Macro for the null char */
#define NULL_CHAR                       ('\0')

/* Macros for FileX */
#define G_FX_MEDIA_HEADS                (1U)
#define G_FX_MEDIA_SECTORS_PER_TRACK    (1U)

/* Macros for directory and file names */
#define DIR_NAME_ONE                    "dir_one"
#define FILE_NAME_ONE                   "file_one.txt"

/* Macros for file operations */
#define TRUNCATE_VALUE                  (0U)
#define SEEK_VALUE                      (0U)
#define OPERATION_TIME_OUT              (1000U)
#define WRITE_BUFFER_SIZE               (10240U)
#define READ_BUFFER_SIZE                (1024U)
#define WRITE_TIMES                     (3200U)
#define WRITE_ONE_PERCENT               (WRITE_TIMES / 100U)
#define WRITE_LINE_SIZE                 (strlen(WRITE_LINE_TEXT))
#define WRITE_LINE_TEXT                 "The example project showcases how the FileX file system operates on the SDRAM"\
                                        " block media of the RA MCU\r\n"

/* Macros for thread sleep */
#define FILEX_SLEEP_TICK                (1U)
#define ONE_SLEEP_TICK                  (1U)
#define TERMINAL_SLEEP_TICK             (1U)

/* Macros for system date and time */
#define MONTH_STR_LEN                   (3U)
#define MONTH_INDEX                     (0U)
#define DATE_INDEX                      (4U)
#define YEAR_INDEX                      (7U)
#define HOUR_INDEX                      (0U)
#define MIN_INDEX                       (3U)
#define SEC_INDEX                       (6U)

/* Macros for the terminal framework */
#define TERMINAL_BYTE_POOL_NAME         "terminal_byte_pool_name"
#define TERMINAL_BYTE_POOL_SIZE         (4096U)
#define TERMINAL_INPUT_ALIGN            (1U)

/* Macros for converter size of bytes to string */
#define THOUSAND                        (1000UL)
#define MILLION                         (1000000UL)
#define BILLION                         (1000000000UL)
#define STR_UL64_MAX_LEN                (16U)

/* Enumerate for month values */
typedef enum e_month
{
    JANUARY   = 1,
    FEBRUARY  = 2,
    MARCH     = 3,
    APRIL     = 4,
    MAY       = 5,
    JUNE      = 6,
    JULY      = 7,
    AUGUST    = 8,
    SEPTEMBER = 9,
    OCTOBER   = 10,
    NOVEMBER  = 11,
    DECEMBER  = 12
} month_t;

/* Enumerate for the Menu screen */
typedef enum e_menu
{
    MENU_MAIN = 0,
    MENU_MEDIA,
    MENU_DIR,
    MENU_FILE,
    MENU_MAX
} menu_t;

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

/* Structure to store date and time */
typedef struct st_date_time
{
    UINT month;
    UINT date;
    UINT year;
    UINT hour;
    UINT min;
    UINT sec;
} date_time_t;

/* Structure to store file or directory entry details */
typedef struct st_entry_info
{
    CHAR name[FX_MAX_LONG_NAME_LEN];
    UINT attr;
    ULONG size;
    date_time_t time;
} entry_info_t;

/* Structure to store directory property details */
typedef struct st_dir_property
{
    UINT file;
    UINT subdir;
    ULONG size;
} dir_property_t;

/* Structure to store media properties */
typedef struct st_media_property
{
    ULONG64 phy_size;
    ULONG64 total_size;
    ULONG64 free_size;
    CHAR * format_type;
} media_property_t;

/* Macros for printing info, error and trapping the error */
#define PRINT_INFO_STR(str)             (term_send_output_queue(TERMINAL_OUTPUT_APP_INFO_STR, (str), strlen(str)))

#define PRINT_ENTRY_INFO(info)          (term_send_output_queue(TERMINAL_OUTPUT_APP_ENTRY_INFO,\
                                                                &(info), sizeof(entry_info_t)))

#define PRINT_DIR_PROPERTY(property)    (term_send_output_queue(TERMINAL_OUTPUT_APP_DIR_PROPERTY,\
                                                                &(property), sizeof(dir_property_t)))

#define PRINT_MEDIA_PROPERTY(property)  (term_send_output_queue(TERMINAL_OUTPUT_APP_MEDIA_PROPERTY,\
                                                                &(property), sizeof(media_property_t)))

#define TERM_ERR_TRAP(cond, err, str)   ({\
                                             if (cond)\
                                             {\
                                                 term_send_output_queue(TERMINAL_OUTPUT_APP_ERR_STR, (str),\
                                                                        strlen(str));\
                                                 term_send_output_queue(TERMINAL_OUTPUT_APP_ERR_TRAP, &(err),\
                                                                        sizeof(err));\
                                                 tx_thread_suspend(tx_thread_identify());\
                                             }\
                                         })

/* Function declarations */
UINT filex_init(void);
void filex_deinit(void);
UINT get_month(const CHAR * p_month);

#endif /* FILEX_SDRAM_EP_H_ */
