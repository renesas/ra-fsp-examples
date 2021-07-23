/***********************************************************************************************************************
 * File Name    : app_definitions.h
 * Description  : Contains macros specific to this application
 ***********************************************************************************************************************/
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
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
#ifndef APP_DEFINITIONS_H_
#define APP_DEFINITIONS_H_

#include <stdint.h>
/* SEGGER RTT and error related headers */
#include "SEGGER_RTT/SEGGER_RTT.h"
#include "common_utils.h"

#define _unused(x) ((void)(x))

#define NUM_OF_FLASH_SECTOR     (1)
#define FLASH_SECTOR_SIZE_8KB   (8*1024)    // 8KB sector
#define FLASH_SECTOR_SIZE_256_BYTES (256)
#define FLASH_SECTOR_SIZE_32KB  (32*1024)   // 32kB sector

/*
 * non-secure flash test block
 */

#define FLASH_WRITE_TEST_BLOCK         (0xF0000)       // test block located in non-secure flash not locked


#define FLASH_WRITE_LENGTH                      (128)
#define FLASH_WRITE_TEST_DATA                   (0x44)


#define SYSTEM_CAME_OUT_OF_RESET                "\r\n system came out of reset, up running"
#define ACCESS_SUCCESSFUL                       "\r\n flash write successful!\r\n"
#define ACCESS_UN_SUCCESSFUL                    "\r\n flash write not successful!\r\n"
#define FLASH_ERASE_FAILED                      "\r\n flash erase failed! \r\n"
#define FLASH_WRITE_SOURCE_ADDR_CHECK_FAILED    "\r\n invalid secure flash region access! \r\n"

/* Macros for menu options to be displayed */


#define MENUSTATUS1       " Running the Primary application with overwrite update mode. \r\n"
#define MENUSTATUS2       " The Red, Blue and Green LEDs should be blinking.\r\n\r\n"


#endif /* APP_DEFINITIONS_H_ */
