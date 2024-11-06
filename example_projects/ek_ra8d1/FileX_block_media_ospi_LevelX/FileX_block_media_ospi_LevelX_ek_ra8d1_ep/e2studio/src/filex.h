/**********************************************************************************************************************
 * File Name    : filex.h
 * Description  : Contains data structures and functions used in filex.c
 *********************************************************************************************************************/
/**********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
**********************************************************************************************************************/

#ifndef FILEX_H_
#define FILEX_H_

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "common_utils.h"
#include "terminal.h"
#include "levelx.h"
#include "board_ospi.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
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
#define WRITE_LINE_TEXT                 "This EP demonstrates the operation of FileX file system, "\
                                        "incorporating LevelX wear leveling features on the OSPI of the RA MCU\r\n"

#define APP_SLEEP_TICK                  (1U)
#define ONE_BYTE                        (1U)


/* Macros for system date and time */
#define MONTH_STR_LEN                   (3U)
#define MONTH_INDEX                     (0U)
#define DATE_INDEX                      (4U)
#define YEAR_INDEX                      (7U)
#define HOUR_INDEX                      (0U)
#define MIN_INDEX                       (3U)
#define SEC_INDEX                       (6U)

/* macros to print info, error and trap the error.*/
#define PRINT_INFO_STR(str)             (terminal_send_output_queue(TERMINAL_OUTPUT_APP_INFO_STR, sizeof(str), (str)))

#define PRINT_ERR_STR(str)              (terminal_send_output_queue(TERMINAL_OUTPUT_APP_ERR_STR, sizeof(str), (str)))

#define PRINT_ENTRY_INFO(info)          (terminal_send_output_queue(TERMINAL_OUTPUT_APP_ENTRY_INFO, sizeof(entry_info_t), &(info)))

#define PRINT_DIR_PROPERTY(property)    (terminal_send_output_queue(TERMINAL_OUTPUT_APP_DIR_PROPERTY, sizeof(dir_property_t), &(property)))

#define PRINT_MEDIA_PROPERTY(property)  (terminal_send_output_queue(TERMINAL_OUTPUT_APP_MEDIA_PROPERTY, sizeof(media_property_t), &(property)))

#define PRINT_SECTOR_INFO(info)         (terminal_send_output_queue(TERMINAL_OUTPUT_SECTOR_INFO, sizeof(sector_info_t), &(info)))

#define RETURN_ERR_STR(err, str) ({\
    if (err)\
    {\
        terminal_send_output_queue(TERMINAL_OUTPUT_APP_ERR_STR, sizeof(str), (str));\
        return (err);\
    }\
})

#define ERROR_TRAP(err)         ({\
    terminal_send_output_queue(TERMINAL_OUTPUT_APP_ERR_TRAP, sizeof(UINT), &(err));\
    tx_thread_suspend(tx_thread_identify());\
})

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/* Enumerate for user requests */
typedef enum e_request_event
{
    REQUEST_START_CONSOLE               = 1U << 0,
    REQUEST_START_APP_THREAD          	= 1U << 1,
	REQUEST_ERASE_FLASH                 = 1U << 2,
	REQUEST_MEDIA_FORMAT                = 1U << 3,
	REQUEST_MEDIA_OPEN                  = 1U << 4,
	REQUEST_DEFRAGMENT_FLASH            = 1U << 5,
    REQUEST_DIR_CREATE                  = 1U << 6,
    REQUEST_DIR_PROPERTY                = 1U << 7,
    REQUEST_DIR_DELETE                  = 1U << 8,
    REQUEST_FILE_CREATE                 = 1U << 9,
    REQUEST_FILE_WRITE                  = 1U << 10,
    REQUEST_FILE_READ                   = 1U << 11,
    REQUEST_FILE_DELETE                 = 1U << 12,
    REQUEST_SECTOR_WRITE				= 1U << 13,
    REQUEST_SECTOR_READ				    = 1U << 14,
    REQUEST_SECTOR_ERASE				= 1U << 15,
    REQUEST_COMPLETED                   = 1U << 16,
} request_event_t;

/**********************************************************************************************************************
 * Public function prototypes
 *********************************************************************************************************************/
UINT file_system_init(void);
UINT filex_ospi_levelx_operation(UINT request);

#endif /* FILEX_H_ */
