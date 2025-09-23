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
#include <string.h>
#include <stdlib.h>
#include "hal_data.h"

/* SEGGER RTT and error related headers */
#include "SEGGER_RTT/SEGGER_RTT.h"

#define BIT_SHIFT_8             (8u)
#define SIZE_64                 (64u)

#define LVL_ERR                 (1u)    /* Error conditions */

#define RESET_VALUE             (0x00)

#if defined (BOARD_RA6M3_EK) || defined (BOARD_RA6M3G_EK)
#define KIT_NAME                "EK-RA6M3"
#elif defined (BOARD_RA6M4_EK)
#define KIT_NAME                "EK-RA6M4"
#elif defined (BOARD_RA6M5_EK)
#define KIT_NAME                "EK-RA6M5"
#elif defined (BOARD_RA8M1_EK)
#define KIT_NAME                "EK-RA8M1"
#elif defined (BOARD_RA8D1_EK)
#define KIT_NAME                "EK-RA8D1"
#elif defined (BOARD_RA8T1_MCK)
#define KIT_NAME                "MCK-RA8T1"
#elif defined (BOARD_RA8T2_MCK)
#define KIT_NAME                "MCK-RA8T2"
#endif

#define EP_VERSION        ("1.0")
#define MODULE_NAME       "Ethernet using FreeRTOS+TCP"
#define BANNER_INFO       "\r\n********************************************************************************"\
                          "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module         *"\
                          "\r\n*   Example Project Version %s                                                *"\
                          "\r\n*   Flex Software Pack Version %d.%d.%d                                           *"\
                          "\r\n********************************************************************************"\
                          "\r\nRefer to readme.txt file for more details on Example Project and              " \
                          "\r\nFSP User's Manual for more information about "MODULE_NAME"                    "\
                          "\r\n********************************************************************************\r\n"

#define ETH_PREINIT       "\r\n\r\n--------------------------------------------------------------------------------"\
                          "\r\nEthernet adapter Configuration for Renesas "KIT_NAME": Pre IP Init       "\
                          "\r\n--------------------------------------------------------------------------------\r\n\r\n"

#define ETH_POSTINIT      "\r\n\r\n--------------------------------------------------------------------------------"\
                          "\r\nEthernet adapter Configuration for Renesas "KIT_NAME": Post IP Init       "\
                          "\r\n--------------------------------------------------------------------------------\r\n\r\n"

#define SEGGER_INDEX             (0)

#define APP_PRINT(fn_, ...)      (SEGGER_RTT_printf (SEGGER_INDEX,(fn_), ##__VA_ARGS__))

#define APP_ERR_PRINT(fn_, ...)  ({\
                                 if(LVL_ERR)\
                                 SEGGER_RTT_printf (SEGGER_INDEX, "[ERR] In Function: %s(), %s",\
                                 __FUNCTION__,(fn_),##__VA_ARGS__);\
                                 })

#define APP_ERR_TRAP(err)        ({\
                                 if((err)) {\
                                 SEGGER_RTT_printf(SEGGER_INDEX, "\r\nReturned Error Code: 0x%x  \r\n", (err));\
                                 __asm("BKPT #0\n");} /* Trap upon the error */\
                                 })

#define APP_READ(read_data)     (SEGGER_RTT_Read (SEGGER_INDEX, (read_data), sizeof(read_data)))

#define APP_CHECK_DATA          (SEGGER_RTT_HasKey())

#endif /* COMMON_UTILS_H_ */
