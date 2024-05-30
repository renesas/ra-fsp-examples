/***********************************************************************************************************************
 * File Name    : filex.h
 * Description  : Contains data structures and functions used in filex.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2024 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#ifndef FILEX_H_
#define FILEX_H_

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "common_utils.h"
#include "terminal.h"

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

/* macros to print info, error and trap the error.*/
#define PRINT_INFO_STR(str)             (terminal_send_output_queue(TERMINAL_OUTPUT_APP_INFO_STR, sizeof(str), (str)))

#define PRINT_ERR_STR(str)              (terminal_send_output_queue(TERMINAL_OUTPUT_APP_ERR_STR, sizeof(str), (str)))

#define PRINT_ENTRY_INFO(info)          (terminal_send_output_queue(TERMINAL_OUTPUT_APP_ENTRY_INFO, sizeof(entry_info_t), &(info)))

#define PRINT_DIR_PROPERTY(property)    (terminal_send_output_queue(TERMINAL_OUTPUT_APP_DIR_PROPERTY, sizeof(dir_property_t), &(property)))

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
UINT file_system_init(void);
UINT file_system_operation(UINT request);

#endif /* FILEX_H_ */
