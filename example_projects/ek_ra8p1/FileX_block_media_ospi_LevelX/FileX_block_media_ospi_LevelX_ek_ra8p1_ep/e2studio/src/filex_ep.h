/***********************************************************************************************************************
 * File Name    : filex_ep.h
 * Description  : Contains macros, data structures, and functions used in filex_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ***********************************************************************************************************************/

#ifndef FILEX_EP_H_
#define FILEX_EP_H_

#include <stdbool.h>
#include "hal_data.h"
#include "terminal_queue.h"
#include "levelx_ep.h"

/* Macro for the application thread sleep tick */
#define APP_SLEEP_TICK                   (1U)

/* Macro for one byte */
#define ONE_BYTE                         (1U)

/* Macros for system date and time */
#define MONTH_STR_LEN                    (3U)
#define MONTH_INDEX                      (0U)
#define DATE_INDEX                       (4U)
#define YEAR_INDEX                       (7U)
#define HOUR_INDEX                       (0U)
#define MIN_INDEX                        (3U)
#define SEC_INDEX                        (6U)

/* Macros for FileX media configuration */
#define G_FX_MEDIA_HEADS                 (1U)
#define G_FX_MEDIA_SECTORS_PER_TRACK     (1U)

/* Macros for directory and file names */
#define DIR_NAME_ONE                     "dir_one"
#define FILE_NAME_ONE                    "file_one.txt"

/* Macro for the file system operation request bitmask */
#define FILE_SYSTEM_REQUEST_MASK         (0x0000FFFC)

/* Macro for the null character */
#define NULL_CHAR                        ('\0')

/* Macros for file operation values */
#define TRUNCATE_VALUE                   (0U)

#define SEEK_VALUE                       (0U)

#define OPERATION_TIME_OUT               (1000U)

#define WRITE_BUFFER_SIZE                (10240U)

#define READ_BUFFER_SIZE                 (1024U)

#define WRITE_LINE_SIZE                  (128U)

#define WRITE_LINE_TEXT                  "This EP demonstrates the operation of FileX file system, "\
                                         "incorporating LevelX wear leveling features on the OSPI "\
                                         "of the RA MCU\r\n"

/* Macros to print information/error strings to the terminal via the output queue */
#define PRINT_INFO_STR(str)              (term_send_output_queue(TERMINAL_OUTPUT_APP_INFO_STR, (str), sizeof(str)))

#define PRINT_ERR_STR(str)               (term_send_output_queue(TERMINAL_OUTPUT_APP_ERR_STR, (str), sizeof(str)))

#define PRINT_ENTRY_INFO(info)           (term_send_output_queue(TERMINAL_OUTPUT_APP_ENTRY_INFO, &(info),\
                                                                  sizeof(entry_info_t)))

#define PRINT_DIR_PROPERTY(property)     (term_send_output_queue(TERMINAL_OUTPUT_APP_DIR_PROPERTY, &(property),\
                                                                  sizeof(dir_property_t)))

#define PRINT_MEDIA_PROPERTY(property)   (term_send_output_queue(TERMINAL_OUTPUT_APP_MEDIA_PROPERTY, &(property),\
                                                                  sizeof(media_property_t)))

#define PRINT_SECTOR_INFO(info)          (term_send_output_queue(TERMINAL_OUTPUT_SECTOR_INFO, &(info),\
                                                                  sizeof(sector_info_t)))

/* Macro to print an error string and return the error code when the error occurred */
#define RETURN_ERR_STR(err, str)         ({\
                                          if (err)\
                                          {\
                                          term_send_output_queue(TERMINAL_OUTPUT_APP_ERR_STR, (str), sizeof(str));\
                                          return (err);\
                                          }\
                                          })

/* Macro to print the error code and trap the calling thread upon a fatal error */
#define PRINT_ERROR_TRAP(err)            ({\
                                          term_send_output_queue(TERMINAL_OUTPUT_APP_ERR_TRAP, &(err), sizeof(UINT));\
                                          thread_suspend();\
                                          })

#define APP_ERR_CHECK(err, success, msg) ({\
                                          do\
                                          {\
                                          if ((err) != (success))\
                                          {\
                                              PRINT_ERR_STR(msg);\
                                              PRINT_ERROR_TRAP(err);\
                                          }\
                                          } while (0);\
                                          })

/* Enumerate for user requests */
typedef enum e_request_event
{
    REQUEST_START_CONSOLE    = 1U << 0,
    REQUEST_START_APP_THREAD = 1U << 1,
    REQUEST_ERASE_FLASH      = 1U << 2,
    REQUEST_MEDIA_FORMAT     = 1U << 3,
    REQUEST_MEDIA_OPEN       = 1U << 4,
    REQUEST_DEFRAGMENT_FLASH = 1U << 5,
    REQUEST_DIR_CREATE       = 1U << 6,
    REQUEST_DIR_PROPERTY     = 1U << 7,
    REQUEST_DIR_DELETE       = 1U << 8,
    REQUEST_FILE_CREATE      = 1U << 9,
    REQUEST_FILE_WRITE       = 1U << 10,
    REQUEST_FILE_READ        = 1U << 11,
    REQUEST_FILE_DELETE      = 1U << 12,
    REQUEST_SECTOR_WRITE     = 1U << 13,
    REQUEST_SECTOR_READ      = 1U << 14,
    REQUEST_SECTOR_ERASE     = 1U << 15,
    REQUEST_COMPLETED        = 1U << 16,
} request_event_t;

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
    DECEMBER  = 12,
} month_t;

/* Structure to store date and time */
typedef struct st_time
{
    UINT month;
    UINT date;
    UINT year;
    UINT hour;
    UINT min;
    UINT sec;
} time_t;

/* Structure to store entry details */
typedef struct st_entry_info
{
    CHAR name[FX_MAX_LONG_NAME_LEN];
    UINT attr;
    ULONG size;
    time_t time;
} entry_info_t;

/* Structure to store directory property details */
typedef struct st_dir_property
{
    UINT file;
    UINT subdir;
    ULONG size;
} dir_property_t;

/* Structure to store media property details */
typedef struct st_media_property
{
    ULONG64 phy_size;
    ULONG64 total_size;
    ULONG64 free_size;
    CHAR *  format_type;
} media_property_t;

/* Structure to store sector info details */
typedef struct st_sector_info
{
    ULONG * sector_map_entry;
    ULONG * sector_address;
    ULONG   logical_sector;
} sector_info_t;

/* External variables */
extern FX_MEDIA g_fx_media;
extern volatile bool g_media_opened;

/* Function declaration */
UINT media_close(void);
UINT media_format(UCHAR * memory_ptr);
UINT media_open(VOID * memory_ptr);
UINT media_extended_space_available(ULONG64 * available_bytes_ptr);
UINT dir_create(CHAR * directory_name);
UINT media_flush(void);
UINT dir_info(entry_info_t * p_entry);
UINT dir_first_full_entry_find(entry_info_t * p_entry);
UINT dir_next_full_entry_find(entry_info_t * p_entry);
UINT dir_delete(CHAR * directory_name);
UINT file_create(CHAR * file_name);
UINT created_entry_info_get (entry_info_t * p_entry, CHAR * file_name, sector_info_t * p_sector);
UINT file_open(FX_FILE * file_ptr, CHAR * file_name, UINT open_type);
void file_close(FX_FILE *file_ptr);
UINT file_write(FX_FILE *file_ptr, VOID *buffer_ptr, CHAR *file_name, time_t * p_time);
UINT file_seek(FX_FILE *file_ptr);
UINT file_read(FX_FILE *file_ptr, VOID *buffer_ptr, ULONG *actual_size);
UINT file_delete(CHAR *file_name);
UINT file_system_init(void);

#endif /* FILEX_EP_H_ */

