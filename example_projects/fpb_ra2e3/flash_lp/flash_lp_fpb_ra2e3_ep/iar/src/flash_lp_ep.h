/***********************************************************************************************************************
 * File Name    : flash_lp_ep.h
 * Description  : Contains declarations of data structures and functions used in flash_lp_ep.h.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef FLASH_LP_EP_H_
#define FLASH_LP_EP_H_

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

#define EP_INFO             	"\r\n\nThis example project demonstrates basic functions of Flash LP driver."\
                                "\r\nAn RTT input from the user triggers Initialization, Erase, Blank Check,"\
                                "\r\nFlash Write and Flash Read operations. User has to choose between the "\
                                "\r\nData Flash or Code Flash operation, and give the input for block number."\
                                "\r\nThe data and status output can be seen on the RTTviewer\r\n"

/* Macros for commands to be received through RTT input */
#define DATA_FLASH_OPERATIONS		(1)
#define CODE_FLASH_OPERATIONS   	(2)

/* Buffer MACROs */
#define BUFF_LEN					(16)
#define ARRAY_INDEX					(0)

/* MACRO for checking if no byte is received */
#define BYTES_RECEIVED_ZERO  		(0)

/* Block size and block number */
#define BLOCK_SIZE					(64)
#define NUM_BLOCKS					(1)

/* Print messages for block ranges */
#if defined(BOARD_RA2E1_EK)
#define DF_BLOCK_RANGE_DISPLAY	"\r\nValid Block Range for Data Flash Operation is 0 - 3\r\n"
#define CF_BLOCK_RANGE_DISPLAY	"Valid Block Range for Code Flash Operation is 10 - 63\r\n"
#define AW_BLOCK_RANGE_DISPLAY	"Valid Block Range for AccessWindow Operation is 10 - 62\r\n"
#elif defined(BOARD_RA2E2_EK) || defined(BOARD_RA2E3_FPB)
#define DF_BLOCK_RANGE_DISPLAY  "\r\nValid Block Range for Data Flash Operation is 0 - 1\r\n"
#define CF_BLOCK_RANGE_DISPLAY  "Valid Block Range for Code Flash Operation is 10 - 31\r\n"
#define AW_BLOCK_RANGE_DISPLAY  "Valid Block Range for AccessWindow Operation is 10 - 30\r\n"
#elif defined(BOARD_RA4W1_EK)
#define DF_BLOCK_RANGE_DISPLAY	"\r\nValid Block Range for Data Flash Operation is 0 - 7\r\n"
#define CF_BLOCK_RANGE_DISPLAY	"Valid Block Range for Code Flash Operation is 10 - 255\r\n"
#define AW_BLOCK_RANGE_DISPLAY	"Valid Block Range for AccessWindow Operation is 10 - 254\r\n"
#else
#define DF_BLOCK_RANGE_DISPLAY	"\r\nValid Block Range for Data Flash Operation is 0 - 7\r\n"
#define CF_BLOCK_RANGE_DISPLAY	"Valid Block Range for Code Flash Operation is 10 - 127\r\n"
#define AW_BLOCK_RANGE_DISPLAY	"Valid Block Range for AccessWindow Operation is 10 - 126\r\n"
#endif

#define RANGE_DISPLAY		({\
                            DF_BLOCK_RANGE_DISPLAY\
							CF_BLOCK_RANGE_DISPLAY\
							AW_BLOCK_RANGE_DISPLAY\
                            ;})

/* Data Flash Macros */
#define FLASH_DF_BLOCK0				(0x40100000)
#if defined(BOARD_RA2E1_EK)
#define FLASH_DF_BLOCK_END			(0x40100FFF)	//FLASH_DF_BLOCK3
#elif defined(BOARD_RA2E2_EK) || defined(BOARD_RA2E3_FPB)
#define FLASH_DF_BLOCK_END          (0x401007FF)    //FLASH_DF_BLOCK1
#else
#define FLASH_DF_BLOCK_END 			(0x40101C00)	//FLASH_DF_BLOCK7
#endif
#define FLASH_DF_BLOCK_SIZE			((1) * (1024))
#define FLASH_DF_BLOCK(x)			((FLASH_DF_BLOCK0) + (1024) * (uint32_t)(x))

/* Code Flash Macros */
#define FLASH_CF_BLOCK_SIZE			((2) * (1024))
#define FLASH_CF_BLOCK0				(0x00000000)
#define FLASH_CF_BLOCK10			(0x00005000)
#if defined(BOARD_RA2E1_EK)
#define FLASH_CF_BLOCK61			(0x0001EFFF)
#define FLASH_CF_BLOCK_PRE_END		(0x0001F7FF)  	 //FLASH_CF_BLOCK62
#define FLASH_CF_BLOCK_END			(0x0001FFFF)
#elif defined(BOARD_RA2E2_EK) || defined(BOARD_RA2E3_FPB)
#define FLASH_CF_BLOCK29            (0x0000EFFF)
#define FLASH_CF_BLOCK_PRE_END      (0x0000F7FF)     //FLASH_CF_BLOCK30
#define FLASH_CF_BLOCK_END          (0x0000FFFF)
#elif defined(BOARD_RA4W1_EK)
#define FLASH_CF_BLOCK_253			(0x0007EFFF)
#define FLASH_CF_BLOCK_PRE_END		(0x0007F7FF)	//FLASH_CF_BLOCK254
#define FLASH_CF_BLOCK_END			(0x0007FFFF)
#else
#define FLASH_CF_BLOCK125			(0x0003EFFF)
#define FLASH_CF_BLOCK_PRE_END		(0x0003F7FF)	//FLASH_CF_BLOCK126
#define FLASH_CF_BLOCK_END			(0x0003FFFF)
#endif

#define FLASH_CF_BLOCK(x)			((FLASH_CF_BLOCK0) + (2) * (1024) * (uint32_t)(x))
#define FLASH_CF_START_ADDR(x)		(FLASH_CF_BLOCK(x))
#define FLASH_CF_END_ADDR(x)		((FLASH_CF_BLOCK(x)) + (2) * (FLASH_CF_BLOCK_SIZE))

/* Printing read data */
#define READ_DATA_PRINT(buff)       ({\
                                    for (uint8_t index = RESET_VALUE; BLOCK_SIZE > index; index++)	\
                                    {																\
                                        APP_PRINT("%2.2x ",*((buff) + (index)));						\
                                        if ((index + 1) % 8 == 0)									\
										    {	APP_PRINT("\r\n");	}									\
                                    }\
                                    })

/* This function initialize FLASH LP driver */
fsp_err_t flash_lp_init();

/* This function calls APIs to perform read/write/erase and other operations on Data FLASH */
fsp_err_t flash_lp_data_flash_operations(uint8_t block_num);

/* This function calls APIs to perform read/write/erase and other operations on Code FLASH */
fsp_err_t flash_lp_code_flash_operations(uint8_t block_num);

/* This function checks the flag for blank check */
flash_result_t flash_lp_wait_for_blankcheck_flag(void);

/* This function de-initialize FLASH LP driver */
fsp_err_t flash_lp_deinit(void);

#endif /* FLASH_LP_EP_H_ */
