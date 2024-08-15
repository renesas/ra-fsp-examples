/***********************************************************************************************************************
 * File Name    : flash_hp_ep.h
 * Description  : Contains macros, data structures and functions used in flash_hp_ep.h
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef FLASH_HP_EP_H_
#define FLASH_HP_EP_H_
/*******************************************************************************************************************//**
 * @ingroup FLASH_HP_EP
 * @{
 **********************************************************************************************************************/
/* Code Flash */
#define FLASH_HP_CF_BLOCK_SIZE_32KB       (32*1024)    /* Block Size 32 KB */
#define FLASH_HP_CF_BLOCK_SIZE_8KB        (8*1024)     /* Block Size 8KB */

#define FLASH_HP_CF_BLCOK_0               0x00000000U  /*    8 KB:  0x00000000 - 0x00001FFF */
#define FLASH_HP_CF_BLOCK_1               0x00002000U  /*    8 KB:  0x00002000 - 0x00003FFF */
#define FLASH_HP_CF_BLOCK_2               0x00004000U  /*    8 KB:  0x00004000 - 0x00005FFF */
#define FLASH_HP_CF_BLOCK_3               0x00006000U  /*    8 KB:  0x00006000 - 0x00007FFF */
#define FLASH_HP_CF_BLOCK_4				  0x00008000U  /*	 8 KB:  0x00008000 - 0x00009FFF */
#define FLASH_HP_CF_BLOCK_5				  0x0000A000U  /*    8 KB:  0x0000A000 - 0x0000BFFF */
#define FLASH_HP_CF_BLOCK_6				  0x0000C000U  /*    8 KB:  0x0000C000 - 0x0000DFFF */
#define FLASH_HP_CF_BLOCK_7				  0x0000E000U  /*    8 KB:  0x0000E000 - 0x0000FFFF */
#define FLASH_HP_CF_BLOCK_8               0x00010000U  /*   32 KB: 0x00010000 - 0x00017FFF */
#define FLASH_HP_CF_BLOCK_9               0x00018000U  /*   32 KB: 0x00018000 - 0x0001FFFF */
#define FLASH_HP_CF_BLCOK_10              0x00020000U  /*   32 KB: 0x00020000 - 0x0004FFFF */

#define FLASH_HP_DF_BLOCK_SIZE            (64)
/* Data Flash */
#if (defined (BOARD_RA6M4_EK) || defined (BOARD_RA6M5_EK) || defined (BOARD_RA4M3_EK)||defined(BOARD_RA4M2_EK)||defined(BOARD_RA6E1_FPB)||defined(BOARD_RA6T2_MCK))

#define FLASH_HP_DF_BLOCK_0               0x08000000U /*   64 B:  0x40100000 - 0x4010003F */
#define FLASH_HP_DF_BLOCK_1               0x08000040U /*   64 B:  0x40100040 - 0x4010007F */
#define FLASH_HP_DF_BLOCK_2               0x08000080U /*   64 B:  0x40100080 - 0x401000BF */
#define FLASH_HP_DF_BLOCK_3               0x080000C0U /*   64 B:  0x401000C0 - 0x401000FF */


#else

#define FLASH_HP_DF_BLOCK_0               0x40100000U /*   64 B:  0x40100000 - 0x4010003F */
#define FLASH_HP_DF_BLOCK_1               0x40100040U /*   64 B:  0x40100040 - 0x4010007F */
#define FLASH_HP_DF_BLOCK_2               0x40100080U /*   64 B:  0x40100080 - 0x401000BF */
#define FLASH_HP_DF_BLOCK_3               0x401000C0U /*   64 B:  0x401000C0 - 0x401000FF */

#endif

#define BLOCK_SIZE                        (128)
#define BLOCK_NUM						  (2)


/* Switch Commands */
#define CODE_FLASH                     	(1U)
#define DATA_FLASH                     	(2U)
#define EXIT							(3U)
#define BUFF_SIZE						0x0F
#define BUFF_INDEX						0x00

/* Printing read data */
#define READ_DATA_PRINT(buff)		for(uint8_t index = RESET_VALUE; BLOCK_SIZE > index; index++)	\
        {													\
    APP_PRINT("%x ",*(read_buffer+index));						\
    if((index + 1) % 8 == 0)							\
    {	APP_PRINT("\r\n");	}						\
        }
/*flash_hp operating functions */
fsp_err_t flash_hp_code_flash_operations(void);
fsp_err_t flash_hp_data_flash_operations(void);
void flash_hp_deinit(void);
void exit_flash_hp(void);
/*******************************************************************************************************************//**
 * @} (end defgroup FLASH_HP_EP)
 **********************************************************************************************************************/

#endif /* FLASH_HP_EP_H_ */
