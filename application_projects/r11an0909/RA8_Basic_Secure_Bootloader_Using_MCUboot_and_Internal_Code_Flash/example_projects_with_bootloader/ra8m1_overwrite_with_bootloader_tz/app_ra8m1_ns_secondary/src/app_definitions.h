/***********************************************************************************************************************
 * File Name    : app_definitions.h
 * Description  : Contains macros specific to this application
 ***********************************************************************************************************************/
/*
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef APP_DEFINITIONS_H_
#define APP_DEFINITIONS_H_

#include <stdint.h>
/* SEGGER RTT and error related headers */
#include "SEGGER_RTT/SEGGER_RTT.h"
#include "common_utils.h"

#define _unused(x) ((void)(x))

#define NUM_OF_FLASH_SECTOR         (1)
#define FLASH_SECTOR_SIZE_8KB       (8*1024)    // 8KB sector
#define FLASH_SECTOR_SIZE_256_BYTES (256)
#define FLASH_SECTOR_SIZE_32KB      (32*1024)   // 32kB sector

/*
 * non-secure flash test block
 */

#define FLASH_WRITE_TEST_BLOCK      (0x121E8000)       // test block located in non-secure flash not locked


#define FLASH_WRITE_LENGTH          (128)
#define FLASH_WRITE_TEST_DATA       (0x44)


#define SYSTEM_CAME_OUT_OF_RESET                "\r\n system came out of reset, up running"
#define ACCESS_SUCCESSFUL                       "\r\n flash write successful!\r\n"
#define ACCESS_UN_SUCCESSFUL                    "\r\n flash write not successful!\r\n"
#define FLASH_ERASE_FAILED                      "\r\n flash erase failed! \r\n"
#define FLASH_WRITE_SOURCE_ADDR_CHECK_FAILED    "\r\n invalid secure flash region access! \r\n"

/* Macros for menu options to be displayed */


#define MENUSTATUS1       " Running the Secondary non-secure application with overwrite update mode. \r\n"
#define MENUSTATUS2       " Flash Operation is successful. The Blue and Green LEDs should be blinking.\r\n\r\n"


#endif /* APP_DEFINITIONS_H_ */
