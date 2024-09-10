/***********************************************************************************************************************
 * File Name    : app_definitions.h
 * Description  : Contains macros specific to this application
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
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
 * three flash blocks used for flash erase and write testing
 * flash programming and erasing is not influenced by TrustZone region setup
 * flash programming and erasing is only controlled by flash block locking scheme
 */
#define FLASH_WRITE_TEST_BLOCK1         (0x7000)       // test block located in secure region not locked
#define FLASH_WRITE_TEST_BLOCK2         (0xF8000)       // test block located in non-secure flash locked by secure code
#define FLASH_WRITE_TEST_BLOCK3         (0xF0000)       // test block located in non-secure flash not locked


#define SECURE_SRAM_READ_ADDR                 	(0x20001000)
#define SECURE_SRAM_WRITE_ADDR					(0x20001F00)
#define SECURE_FALSH_READ_ADDR					(0x1000)
#define FLASH_WRITE_LENGTH  					(128)
#define FLASH_WRITE_TEST_DATA           		(0x44)
#define SRAM_WRITE_DATA							(0x33)

typedef enum
{
	TEST_F_NS_READ_S_SRAM 	 			=  		(1),
	TEST_F_NS_WRITE_SRAM   				= 		(2),
	TEST_F_NS_READ_FLASH   				= 		(3),
	TEST_F_NS_WRITE_S_FLASH  			= 		(4),
	TEST_F_NS_WRITE2_LOCKED_FLASH  		=		(5),
	TEST_F_NS_WRITE3_NS_FLASH  			=		(6),
	TEST_F_NS_COPY_S_FLASH				=   	(7),
	TEST_SRAM_NS_READ_S_SRAM    		=		(8),
	TEST_SRAM_NS_WRITE_S_SRAM			=  		(9),
} tests_t;

/* Macros for menu options to be displayed */
#define MENUOPTION_REGION		  "\r\n Code Running in Non-secure Region\r\n"
#define MENUOPTION_MENU       "\r\n MENU to Select \r\n"
#define MENUOPTION1       "Press 1 to Read Secure SRAM from Non-secure flash\r\n"
#define MENUOPTION2       "Press 2 to Write to Secure SRAM from Non-secure flash\r\n"
#define MENUOPTION3       "Press 3 to Read Secure Flash from Non-secure flash\r\n"
#define MENUOPTION4       "Press 4 to Write to (not locked) Secure flash from Non-secure callable flash driver\r\n"
#define MENUOPTION5		  "Press 5 to Write to locked Non-secure flash from Non-secure callable flash driver\r\n"
#define MENUOPTION6		  "Press 6 to write to (not locked) Non-secure flash from Non-secure callable flash driver\r\n"
#define MENUOPTION7		  "Press 7 to Attempt to copy from Secure flash region to (not locked) Non-secure flash region\r\n        from Non-secure callable flash driver\r\n"
#define MENUOPTION8		  "Press 8 to Read Secure SRAM from Non-secure SRAM \r\n"
#define MENUOPTION9		  "Press 9 to Write Secure SRAM from Non-secure SRAM \r\n"

#define MENUEVENT1       	"Read Secure SRAM from Non-secure flash\r\n"
#define MENUEVENT2       	"Write to Secure SRAM from Non-secure flash\r\n"
#define MENUEVENT3       	"Read Secure Flash from Non-secure flash\r\n"
#define MENUEVENT4       	"Write to (not locked) Secure flash from Non-secure callable flash driver\r\n"
#define MENUEVENT5			"Write to locked Non-secure flash from Non-secure callable flash driver\r\n"
#define MENUEVENT6			"write to (not locked) Non-secure flash from Non-secure callable flash driver \r\n"
#define MENUEVENT7			"Attempt to copy Secure flash region content to (not locked) Non-secure flash region\r\n from Non-secure callable flash driver\r\n"
#define MENUEVENT8		  	"Read Secure SRAM from Non-secure SRAM \r\n"
#define MENUEVENT9		  	"Write to Secure SRAM from Non-secure SRAM \r\n"

#define BANNER_1                                "\r\n******************************************************************"
#define APPLICATION_NAME                        "\r\n securing data at rest using Arm TrustZone"
#define BANNER_2                                "\r\n******************************************************************"
#define IN_SECURE_REGION                        "\r\n running in secure region"
#define IN_NON_SECURE_REGION                    "\r\n System Reset, enter non-secure region\r\n"


#define SYSTEM_CAME_OUT_OF_RESET                "\r\n system came out of reset, up running"
#define ACCESS_SUCCESSFUL                       "\r\n flash write successful!\r\n"
#define ACCESS_UN_SUCCESSFUL                    "\r\n flash write not successful!\r\n"
#define FLASH_ERASE_FAILED						"\r\n flash erase failed! \r\n"
#define FLASH_WRITE_SOURCE_ADDR_CHECK_FAILED	"\r\n invalid secure flash region access! \r\n"
#define NOT_SUPPORTED							"\r\n not supported! \r\n"

#endif /* APP_DEFINITIONS_H_ */
