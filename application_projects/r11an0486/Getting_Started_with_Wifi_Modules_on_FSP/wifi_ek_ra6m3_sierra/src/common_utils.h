/***********************************************************************************************************************
 * File Name    : common_utils.h
 * Description  : Contains macros, data structures and functions used  common to the AP
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef COMMON_UTILS_H_
#define COMMON_UTILS_H_

/* generic headers */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hal_data.h"

/* SEGGER RTT and error related headers */
#include "SEGGER_RTT/SEGGER_RTT.h"

#define BIT_SHIFT_8     (8u)
#define SIZE_64         (64u)

#define LVL_ERR         (1u)           /* error conditions   */

#define RESET_VALUE     (0x00)

#define EP_VERSION      ("1.1")
#define MODULE_NAME     "rm_wifi_onchip_sierra"
#define BANNER_INFO     "\r\n***********************************************************************"                                                              \
                        "\r\n*   Renesas FSP Application Project for "MODULE_NAME " Module  *"                                                                     \
                                                                                  "\r\n*   Application Project Version %s                                   *"     \
                                                                                  "\r\n*   Flex Software Pack Version  %d.%d.%d                                 *" \
                                                                                  "\r\n***********************************************************************"    \
                                                                                  "\r\nRefer to Application note for more details on Application Project and"      \
                                                                                  "\r\nfor more information about creating the "MODULE_NAME                        \
                                                                                  " driver\r\n"

#define SEGGER_INDEX    (0)

#define APP_PRINT(fn_, ...)        SEGGER_RTT_printf(SEGGER_INDEX, (fn_), ## __VA_ARGS__);

#define APP_ERR_PRINT(fn_, ...)    if (LVL_ERR) { \
        SEGGER_RTT_printf(SEGGER_INDEX, "[ERR] In Function: %s(), %s", __FUNCTION__, (fn_), ## __VA_ARGS__);}

#define APP_ERR_TRAP(err)          if (err) {                                        \
        SEGGER_RTT_printf(SEGGER_INDEX, "\r\nReturned Error Code: 0x%x  \r\n", err); \
        __asm("BKPT #0\n");}           /* trap upon the error  */

#define APP_READ(read_data)        SEGGER_RTT_Read(SEGGER_INDEX, read_data, sizeof(read_data));

#define APP_CHECK_DATA    SEGGER_RTT_HasKey()

#endif                                 /* COMMON_UTILS_H_ */
