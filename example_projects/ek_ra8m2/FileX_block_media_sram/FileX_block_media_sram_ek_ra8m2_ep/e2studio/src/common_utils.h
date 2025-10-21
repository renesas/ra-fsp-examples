/***********************************************************************************************************************
 * File Name    : common_utils.h
 * Description  : Contains macros, data structures and functions commonly used in the EP.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef COMMON_UTILS_H_
#define COMMON_UTILS_H_

/* Generic headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include "hal_data.h"
#include "bsp_api.h"
#include "tx_api.h"
#if (USE_VIRTUAL_COM == 1)
#include "SERIAL_TERM/serial_data.h"
#else
#include "SEGGER_RTT/SEGGER_RTT.h"
#endif

#define BIT_SHIFT_8        (8u)
#define SIZE_64            (64u)

#define LVL_ERR            (1u)            /* Error conditions */
#define LVL_DEBUG          (3u)            /* Debug-level messages */

#define LOG_LEVEL          (LVL_ERR)       /* To see the debug messages, LOG_LEVEL should be set to LVL_DEBUG */

#define RESET_VALUE        (0x00)

#define NULL_CHAR          ('\0')

#define EP_VERSION         ("1.0")
#define MODULE_NAME        "FileX block media sram"
#define BANNER_INFO        "\r\n********************************************************************************"\
                           "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module              *"\
                           "\r\n*   Example Project Version %s                                                *"\
                           "\r\n*   Flex Software Pack Version %d.%d.%d                                           *"\
                           "\r\n********************************************************************************"\
                           "\r\nRefer to readme.txt file for more details on Example Project and"\
                           "\r\nFSP User's Manual for more information about "MODULE_NAME" module\r\n"

#define EP_INFO            "\r\nThe example project showcases how the FileX file system operates on the internal"\
                           "\r\nSRAM block media of the RA MCU. The project will perform various FileX file system"\
                           "\r\noperations based on the user's selection from menu options, such as media management"\
                           "\r\n(format), directory management (create, get properties, delete), and file management"\
                           "\r\n(create, write, read, delete). The terminal application is used as the user interface."\
                           "\r\nThe menu options and system messages (errors and information messages) will be printed"\
                           "\r\non the terminal application during the execution of the project.\r\n\r\n"

#define SEGGER_INDEX            (0)

#define APP_READ(read_data)     (SEGGER_RTT_Read(SEGGER_INDEX, (read_data), sizeof(read_data)))

#define APP_CHECK_DATA          (SEGGER_RTT_HasKey())

#define APP_ERR_TRAP(err)       ({\
		                        if(err)\
								{\
		                        __asm("BKPT #0\n"); /* Trap upon the error */\
		                        }\
                                })

#endif /* COMMON_UTILS_H_ */
