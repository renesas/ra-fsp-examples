/***********************************************************************************************************************
 * File Name    : filex_sram_ep.h
 * Description  : Contains macros, data structures, and functions used in filex_sram_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef FILEX_SRAM_EP_H_
#define FILEX_SRAM_EP_H_

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
#define WRITE_LINE_SIZE                 (128U)

/* Macros for file operations */
#define WRITE_LINE_TEXT                 "The example project demonstrates the operation "\
		                                "of the Azure FileX file system "\
                                        "on block media via the SRAM driver on the RA MCU\r\n"

/* Macros for system date and time */
#define MONTH_STR_LEN                   (3U)
#define MONTH_INDEX                     (0U)
#define DATE_INDEX                      (4U)
#define YEAR_INDEX                      (7U)
#define HOUR_INDEX                      (0U)
#define MIN_INDEX                       (3U)
#define SEC_INDEX                       (6U)

/* Enumerate for month values */
typedef enum e_month
{
    JANUARY = 1,
    FEBRUARY = 2,
    MARCH = 3,
    APRIL = 4,
    MAY = 5,
    JUNE = 6,
    JULY = 7,
    AUGUST = 8,
    SEPTEMBER = 9,
    OCTOBER = 10,
    NOVEMBER = 11,
    DECEMBER = 12
}month_t;

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
}request_event_t;

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

/* Structure to store file or directory entry details */
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

/* Function declarations */
UINT file_system_init(void);
UINT media_format(void);
UINT dir_create(CHAR * directory_name);
UINT media_flush(void);
UINT dir_info(entry_info_t * p_entry, CHAR * file_name);
UINT created_entry_info_get(entry_info_t * p_entry, CHAR * file_name);
UINT dir_first_full_entry_find(entry_info_t * p_entry);
UINT dir_next_full_entry_find(entry_info_t * p_entry);
UINT wait_event_get(void);
UINT dir_delete(CHAR * directory_name);
UINT file_create(CHAR * file_name);
UINT file_open(FX_FILE * file_ptr, CHAR * file_name, UINT open_type);
UINT file_write(FX_FILE * file_ptr, VOID * buffer_ptr, CHAR * file_name, time_app_t * p_time);
void file_close(FX_FILE * file_ptr);
UINT file_seek(FX_FILE * file_ptr);
UINT file_read(FX_FILE * file_ptr, VOID * buffer_ptr, ULONG * actual_size);
UINT file_delete(CHAR * file_name);
void filex_deinit(void);
UINT get_month(const CHAR * p_month);

#endif /* FILEX_SRAM_EP_H_ */
