/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#ifndef RTT_H_
#define RTT_H_

#include "common_utils.h"

#define RTT_THREAD_SLEEP_TICK       (1U)
#define RTT_BYTE_POOL_NAME          "rtt_byte_pool_name"
#define RTT_BYTE_POOL_SIZE          (4096U)
#define RTT_INPUT_ALIGN             (1U)

/* Macros for converter size of bytes to string */
#define THOUSAND                    (1000UL)
#define MILLION                     (1000000UL)
#define BILLION                     (1000000000UL)
#define STR_UL64_MAX_LEN            (16U)

#define DIR_ROOT_NAME               "\\"


#define FILE_SYSTEM_REQUEST_MASK    (0x00001FFC)

#define MAIN_OPTION                 "\r\nMain Menu option:"\
                                    "\r\n  1. Media operation."\
                                    "\r\n  2. Directory operation."\
                                    "\r\n  3. File operation."\
                                    "\r\n  User Input:  "

#define MEDIA_OPTION                "\r\nMain Menu >"\
                                    "\r\n  Media operation:"\
                                    "\r\n    1. Open Media."\
                                    "\r\n    2. Get Media Properties."\
                                    "\r\n    3. Format Media."\
                                    "\r\n    4. Close Media."\
                                    "\r\n    5. Go back Main Menu."\
                                    "\r\n    User Input:  "

#define DIR_OPTION                  "\r\nMain Menu >"\
                                    "\r\n  Directory operation:"\
                                    "\r\n    1. Create Directory."\
                                    "\r\n    2. Get Directory Properties."\
                                    "\r\n    3. Delete Directory."\
                                    "\r\n    4. Go back Main Menu."\
                                    "\r\n    User Input:  "

#define FILE_OPTION                 "\r\nMain Menu >"\
                                    "\r\n  File operation:"\
                                    "\r\n    1. Create Empty File."\
                                    "\r\n    2. Write To File."\
                                    "\r\n    3. Read From File."\
                                    "\r\n    4. Delete File."\
                                    "\r\n    5. Go back Main Menu."\
                                    "\r\n    User Input:  "

#define VOLUME_LABEL_LEN            (12U)

/* Enumerate for user requests */
typedef enum e_request_event
{
    REQUEST_START_CONSOLE               = 1U << 0,
    REQUEST_START_FILE_SYSTEM           = 1U << 1,
    REQUEST_MEDIA_OPEN                  = 1U << 2,
    REQUEST_MEDIA_PROPERTY              = 1U << 3,
    REQUEST_MEDIA_FORMAT                = 1U << 4,
    REQUEST_MEDIA_CLOSE                 = 1U << 5,
    REQUEST_DIR_CREATE                  = 1U << 6,
    REQUEST_DIR_PROPERTY                = 1U << 7,
    REQUEST_DIR_DELETE                  = 1U << 8,
    REQUEST_FILE_CREATE                 = 1U << 9,
    REQUEST_FILE_WRITE                  = 1U << 10,
    REQUEST_FILE_READ                   = 1U << 11,
    REQUEST_FILE_DELETE                 = 1U << 12,
    REQUEST_COMPLETED                   = 1U << 13
} request_event_t;

/* Enumerate for RTT event  */
typedef enum rtt_event_id
{
    RTT_INPUT_MESSAGE,
    RTT_OUTPUT_EP_BANNER,
    RTT_OUTPUT_EP_INFO,
    RTT_OUTPUT_APP_INFO_STR,
    RTT_OUTPUT_APP_ERR_STR,
    RTT_OUTPUT_APP_ERR_TRAP,
    RTT_OUTPUT_APP_MENU,
    RTT_OUTPUT_APP_MEDIA_PROPERTY,
    RTT_OUTPUT_APP_MEDIA_VOLUME_INFO,
    RTT_OUTPUT_APP_ENTRY_INFO,
    RTT_OUTPUT_APP_DIR_PROPERTY
}rtt_event_id_t;

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

/* Used to exchange information between application thread and RTT thread */
typedef struct rtt_msg
{
    rtt_event_id_t id;
    uint32_t size;
    uint32_t time;
    CHAR msg[];
}rtt_msg_t;

/* Structure to store date and time */
typedef struct st_time
{
    UINT month;
    UINT date;
    UINT year;
    UINT hour;
    UINT min;
    UINT sec;
}time_t;

/* Structure to store entry details */
typedef struct st_entry_info
{
    CHAR name[FX_MAX_LONG_NAME_LEN];
    UINT attr;
    ULONG size;
    time_t time;
}entry_info_t;

/* Structure to store directory property details */
typedef struct st_dir_property
{
    UINT file;
    UINT subdir;
    ULONG64 file_size;
}dir_property_t;

/* Structure to store volume details */
typedef struct st_volume_info
{
    CHAR lable[VOLUME_LABEL_LEN];
    ULONG number;
}volume_info_t;

/* Structure to store media property details */
typedef struct st_media_property
{
    ULONG64 phy_size;
    ULONG64 total_zise;
    ULONG64 free_size;
}media_property_t;

UINT rtt_framework_init(void);
UINT rtt_input_handle(void);
UINT rtt_output_handle(void);
UINT send_data_to_rtt(rtt_event_id_t id, uint32_t size, void * const p_data);
UINT get_data_from_rtt(uint8_t * p_data);

/* macros to print info, error and trap the error.*/
#define PRINT_INFO_STR(str)     (send_data_to_rtt(RTT_OUTPUT_APP_INFO_STR, sizeof(str), (str)))

#define PRINT_ERR_STR(str)      (send_data_to_rtt(RTT_OUTPUT_APP_ERR_STR, sizeof(str), (str)))

#define RETURN_ERR_STR(err, str) ({\
    if (err)\
    {\
        send_data_to_rtt(RTT_OUTPUT_APP_ERR_STR, sizeof(str), (str));\
        return (err);\
    }\
})

#define ERROR_TRAP(err)         ({\
    send_data_to_rtt(RTT_OUTPUT_APP_ERR_TRAP, sizeof(UINT), &(err));\
    tx_thread_suspend(tx_thread_identify());\
})

#endif /* RTT_H_ */
