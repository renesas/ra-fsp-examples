/***********************************************************************************************************************
 * File Name    : common_utils.h
 * Description  : Contains macros, data structures and functions used common to the EP
 ***********************************************************************************************************************/
/**********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
**********************************************************************************************************************/

#ifndef COMMON_UTILS_H_
#define COMMON_UTILS_H_

/* generic headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "hal_data.h"
#include "bsp_api.h"
#include "tx_api.h"
#if (USE_VIRTUAL_COM == 1)
#include "SERIAL_TERM/serial_data.h"
#else
#include "SEGGER_RTT/SEGGER_RTT.h"
#endif

#define BIT_SHIFT_8             (8u)
#define SIZE_64                 (64u)

#define LVL_ERR                 (1u)            /* error conditions   */
#define LVL_DEBUG               (3u)            /* debug-level messages */

#define LOG_LEVEL               (LVL_ERR)       /* To See the Debug Messages, LOG_LEVEL should be set to LVL_DEBUG */

#define RESET_VALUE             (0x00)

#define NULL_CHAR               ('\0')

#define EP_VERSION              ("1.0")
#define MODULE_NAME             "FileX block media ospi LevelX"
#define BANNER_INFO             "\r\n********************************************************************************"\
                                "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module       *"\
                                "\r\n*   Example Project Version %s                                                *"\
                                "\r\n*   Flex Software Pack Version  %d.%d.%d                                          *"\
                                "\r\n********************************************************************************"\
                                "\r\nRefer to readme.txt file for more details on Example Project and" \
                                "\r\nFSP User's Manual for more information about "MODULE_NAME" module\r\n"

#define EP_INFO                 "\r\nThis example project demonstrates the operation of the FileX file system, incorporating"\
                                "\r\nLevelX wear leveling features on the OSPI block media of the RA MCU. The project will"\
                                "\r\nperform various FileX file system operations based on the user's selection from menu"\
                                "\r\noption, such as erase OSPI Flash, media management (open, format), directory management"\
                                "\r\n(create, get properties, delete), file management (create, write, read, delete), and"\
                                "\r\nsector level operation (defragment, write, read, erase)."\
                                "\r\nThe terminal application is used as the user interface. The menu options and system"\
                                "\r\nmessages (errors and information messages) will be printed on the terminal application"\
                                "\r\nduring the execution of the project.\r\n\r\n"

#define SEGGER_INDEX            (0)

#define APP_READ(read_data)     (SEGGER_RTT_Read (SEGGER_INDEX, (read_data), sizeof(read_data)))

#define APP_CHECK_DATA          (SEGGER_RTT_HasKey())

#define APP_ERR_TRAP(err)       ({\
    if(err)\
    {\
        __asm("BKPT #0\n"); /* trap upon the error  */\
    }\
})

#endif /* COMMON_UTILS_H_ */
