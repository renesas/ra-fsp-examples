/***********************************************************************************************************************
 * File Name    : littlefs_ospi_ep.h
 * Description  : Contains macros, data structures, and functions used in littlefs_ospi_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef LITTLEFS_OSPI_B_EP_H_
#define LITTLEFS_OSPI_B_EP_H_

#include "board_ospi.h"
#include "lfs.h"

/* Macros for buffer sizes and user input handling */
#define WRITE_BUFFER_SIZE               (10240U)
#define READ_BUFFER_SIZE                (10240U)
#define PRINT_BUFFER_SIZE               (1024U)
#define ONE_BYTE                        (1U)
#define SEEK_VALUE                      (0U)
#define WRITE_LINE_SIZE                 (strlen(WRITE_LINE_TEXT))
#define USER_INPUT_BUFFER_SIZE          (64U)
#define ERR_LED                         (0U)

/* Macros for main menu selections */
#define SELECT_FLASH_OPERATION          (1U)
#define SELECT_DIR_OPERATION            (2U)
#define SELECT_FILE_OPERATION           (3U)

/* Macros for flash operation menu selections */
#define SELECT_ERASE_ENTIRE_FLASH       (1U)
#define SELECT_ERASE_32KB_FLASH         (2U)
#define SELECT_FORMAT_FILE_SYSTEM       (3U)
#define SELECT_FLASH_GO_BACK            (4U)

/* Macros for directory operation menu selections */
#define SELECT_CREATE_DIR               (1U)
#define SELECT_GET_DIR_PROPERTY         (2U)
#define SELECT_DELETE_DIR               (3U)
#define SELECT_DIR_GO_BACK              (4U)

/* Macros for file operation menu selections */
#define SELECT_OPEN_FILE                (1U)
#define SELECT_WRITE_FILE               (2U)
#define SELECT_READ_FILE                (3U)
#define SELECT_DELETE_FILE              (4U)
#define SELECT_FILE_GO_BACK             (5U)

/* Enumeration for user requests */
typedef enum e_request_event
{
    REQUEST_ERASE_ENTIRE_OSPI = 1,
    REQUEST_ERASE_32KB_OSPI,
    REQUEST_FILE_SYSTEM_FORMAT,
    REQUEST_DIR_CREATE,
    REQUEST_DIR_PROPERTY,
    REQUEST_DIR_DELETE,
    REQUEST_FILE_OPEN,
    REQUEST_FILE_WRITE,
    REQUEST_FILE_READ,
    REQUEST_FILE_DELETE,
} request_event_t;

/* Enumeration for menu states */
typedef enum e_menu
{
    MENU_MAIN = 0,
    MENU_FLASH,
    MENU_DIR,
    MENU_FILE,
    MENU_MAX
} menu_t;

/* External variable - Used for LED */
extern bsp_leds_t g_bsp_leds;

/* Function declarations */
int32_t littlefs_ospi_b_init(void);
void littlefs_ospi_b_deinit(void);

#endif /* LITTLEFS_OSPI_B_EP_H_ */
