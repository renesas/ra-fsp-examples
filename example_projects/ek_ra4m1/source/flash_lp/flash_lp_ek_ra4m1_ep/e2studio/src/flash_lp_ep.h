/***********************************************************************************************************************
 * File Name    : flash_lp_ep.h
 * Description  : Contains declarations of data structures and functions used in r_spi_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright [2015-2017] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
 *
 * This file is part of Renesas RA Flex Software Package (FSP)
 *
 * The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
 * and/or its licensors ("Renesas") and subject to statutory and contractual protections.
 *
 * This file is subject to a Renesas FSP license agreement. Unless otherwise agreed in an FSP license agreement with
 * Renesas: 1) you may not use, copy, modify, distribute, display, or perform the contents; 2) you may not use any name
 * or mark of Renesas for advertising or publicity purposes or in connection with your use of the contents; 3) RENESAS
 * MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED
 * "AS IS" WITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, AND NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR
 * CONSEQUENTIAL DAMAGES, INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF
 * CONTRACT OR TORT, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents
 * included in this file may be subject to different terms.
 **********************************************************************************************************************/

#ifndef FLASH_LP_EP_H_
#define FLASH_LP_EP_H_

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/* Macros for commands to be received through RTT input */
#define DATA_FLASH_OPERATIONS		1
#define CODE_FLASH_OPERATIONS   	2

/* Buffer MACROs */
#define BUFF_LEN					(16)
#define ARRAY_INDEX					(0)

/* MACRO for checking if no byte is received */
#define BYTES_RECEIVED_ZERO  		(0)

/* Block size and block number */
#define BLOCK_SIZE					(64)
#define NUM_BLOCKS					(1)

/* Data Flash Macros */
#define FLASH_DF_BLOCK_SIZE			((1) * (1024))
#define FLASH_DF_BLOCK0				(0x40100000)
#define FLASH_DF_BLOCK7 			(0x40101C00)
#define FLASH_DF_BLOCK(x)			((FLASH_DF_BLOCK0) + (1024) * (uint32_t)(x))

/* Code Flash Macros */
#define FLASH_CF_BLOCK_SIZE			((2) * (1024))
#define FLASH_CF_BLOCK0				(0x00000000)
#define FLASH_CF_BLOCK10			(0x00005000)
#define FLASH_CF_BLOCK126			(0x0003F000)
#define FLASH_CF_BLOCK127			(0x0003F800)
#define FLASH_CF_BLOCK(x)			((FLASH_CF_BLOCK0) + (2) * (1024) * (uint32_t)(x))
#define FLASH_CF_START_ADDR(x)		(FLASH_CF_BLOCK(x))
#define FLASH_CF_END_ADDR(x)		((FLASH_CF_BLOCK(x)) + (2) * (FLASH_CF_BLOCK_SIZE))

/* Printing read data */
#define READ_DATA_PRINT(buff)		for (uint8_t index = RESET_VALUE; BLOCK_SIZE > index; index++)	\
									{																\
										APP_PRINT("%2.2x ",*((buff) + (index)));						\
										if ((index + 1) % 8 == 0)									\
										{	APP_PRINT("\r\n");	}									\
									}

/* This function initialize FLASH LP driver */
fsp_err_t flash_lp_init();

/* This function calls APIs to perform read/write/erase and other operations on Data FLASH */
fsp_err_t flash_lp_data_flash_operations(uint8_t block_num);

/* This function calls APIs to perform read/write/erase and other operations on Code FLASH */
fsp_err_t flash_lp_code_flash_operations(uint8_t block_num);

/* This function checks the flag for blank check */
flash_result_t flash_lp_wait_for_blankcheck_flag(uint8_t block_num);

/* This function de-initialize FLASH LP driver */
fsp_err_t flash_lp_deinit(void);

#endif /* FLASH_LP_EP_H_ */
