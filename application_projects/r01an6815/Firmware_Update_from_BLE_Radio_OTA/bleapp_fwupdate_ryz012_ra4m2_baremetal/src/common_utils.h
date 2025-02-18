/***********************************************************************************************************************
 * File Name    : common_utils.h
 * Description  : Contains macros, data structures and functions used  common to the EP
 ***********************************************************************************************************************/
/**********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/

#ifndef COMMON_UTILS_H_
#define COMMON_UTILS_H_

/* generic headers */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hal_data.h"
/* SEGGER RTT and error related headers */
#include "SEGGER_RTT/SEGGER_RTT.h"

#define DEBUG_RTT_VIEWER   // Define to enable RTT Viewer logging
/*
 * To use RTT Viewer for RYZ012 FW Update Logging
 * 1) Segger RTT block address is needed connect to RTT-Viewer.
 *
 * Search on variable in e2studio Project map file after build /Debug: _SEGGER_RTT = 0x20000EFC
 * .bss._SEGGER_RTT
                0x20000efc       0xa8 ./src/SEGGER_RTT/SEGGER_RTT.o
                0x20000efc                _SEGGER_RTT
 *
 *  2) If App Project is modified, please find the block address (for the variable in RAM called _SEGGER_RTT)
 *  in .map file generated in the build configuration folder (Debug/Release).
*/

#define BIT_SHIFT_8  (8u)
#define SIZE_64      (64u)

#define LVL_ERR      (1u)       /* error conditions   */

#define RESET_VALUE             (0x00)

#define AP_VERSION              ("1.0")
#define MODULE_NAME             "RYZ012A FW Update by BLE App"
#define BANNER_INFO	            "\r\n******************************************************************"\
                                "\r\n*   Renesas FSP Application Project for "MODULE_NAME" *"\
                                "\r\n*   Application Project Version %s                              *"\
                                "\r\n*   Flex Software Pack Version  %d.%d.%d                            *"\
                                "\r\n******************************************************************"\
                                "\r\nRefer to readme.txt file for more details on Application Project and" \
                                "\r\nFSP User's Manual for more information about "MODULE_NAME"\r\n"

#define SEGGER_INDEX            (0)

#define APP_PRINT(fn_, ...)      SEGGER_RTT_printf (SEGGER_INDEX,(fn_), ##__VA_ARGS__);

#define APP_ERR_PRINT(fn_, ...)  if(LVL_ERR)\
        SEGGER_RTT_printf (SEGGER_INDEX, "[ERR] In Function: %s(), %s",__FUNCTION__,(fn_),##__VA_ARGS__);

#define APP_ERR_TRAP(err)        if(err) {\
        SEGGER_RTT_printf(SEGGER_INDEX, "\r\nReturned Error Code: 0x%x  \r\n", err);\
        __asm("BKPT #0\n");} /* trap upon the error  */

#define APP_READ(read_data)     SEGGER_RTT_Read (SEGGER_INDEX, read_data, sizeof(read_data));

#define APP_CHECK_DATA          SEGGER_RTT_HasKey()


#endif /* COMMON_UTILS_H_ */
