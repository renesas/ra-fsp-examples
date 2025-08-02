/***********************************************************************************************************************
 * File Name    : flash_hp_ep.h
 * Description  : Contains macros, data structures and functions used in flash_hp_ep.h
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef FLASH_HP_EP_H_
#define FLASH_HP_EP_H_

/*******************************************************************************************************************//**
 * @ingroup FLASH_HP_EP
 * @{
 **********************************************************************************************************************/

#if defined (BOARD_RA4L1_EK) || defined (BOARD_RA4C1_EK)
#define FLASH_HP_CF_BLOCK_SIZE_2KB        (2*1024)     /* Block Size 2KB */
#define FLASH_HP_DF_BLOCK_SIZE            (256)
#define BLOCK_SIZE                        (8)
#else
#define FLASH_HP_CF_BLOCK_SIZE_32KB       (32*1024)    /* Block Size 32 KB */
#define FLASH_HP_CF_BLOCK_SIZE_8KB        (8*1024)     /* Block Size 8KB */
#define FLASH_HP_DF_BLOCK_SIZE            (64)
#define BLOCK_SIZE                        (128)
#endif

#define BLOCK_NUM                         (2)

/* Code Flash */
#if (defined (BOARD_RA8D1_EK) || defined (BOARD_RA8M1_EK) || defined (BOARD_RA8T1_MCK) || defined (BOARD_RA8E1_FPB)\
    || defined (BOARD_RA8E2_EK))
#define FLASH_HP_CF_BLOCK_0               (0x02000000U)  /*    8 KB:  0x02000000 - 0x02001FFF */
#define FLASH_HP_CF_BLOCK_1               (0x02002000U)  /*    8 KB:  0x02002000 - 0x02003FFF */
#define FLASH_HP_CF_BLOCK_2               (0x02004000U)  /*    8 KB:  0x02004000 - 0x02005FFF */
#define FLASH_HP_CF_BLOCK_3               (0x02006000U)  /*    8 KB:  0x02006000 - 0x02007FFF */
#define FLASH_HP_CF_BLOCK_4               (0x02008000U)  /*    8 KB:  0x02008000 - 0x02009FFF */
#define FLASH_HP_CF_BLOCK_5               (0x0200A000U)  /*    8 KB:  0x0200A000 - 0x0200BFFF */
#define FLASH_HP_CF_BLOCK_6               (0x0200C000U)  /*    8 KB:  0x0200C000 - 0x0200DFFF */
#define FLASH_HP_CF_BLOCK_7               (0x0200E000U)  /*    8 KB:  0x0200E000 - 0x0200FFFF */
#define FLASH_HP_CF_BLOCK_8               (0x02010000U)  /*   32 KB:  0x02010000 - 0x02017FFF */
#define FLASH_HP_CF_BLOCK_9               (0x02018000U)  /*   32 KB:  0x02018000 - 0x0201FFFF */
#define FLASH_HP_CF_BLOCK_10              (0x02020000U)  /*   32 KB:  0x02020000 - 0x0204FFFF */
#elif (defined (BOARD_RA4L1_EK) || defined (BOARD_RA4C1_EK))
#define FLASH_HP_CF_BLOCK_0               (0x00000000U)  /*    2 KB:  0x00000000 - 0x000007FF */
#define FLASH_HP_CF_BLOCK_1               (0x00000800U)  /*    2 KB:  0x00000800 - 0x00000FFF */
#define FLASH_HP_CF_BLOCK_2               (0x00001000U)  /*    2 KB:  0x00001000 - 0x000017FF */
#define FLASH_HP_CF_BLOCK_3               (0x00001800U)  /*    2 KB:  0x00001800 - 0x00001FFF */
#define FLASH_HP_CF_BLOCK_4               (0x00002000U)  /*    2 KB:  0x00002000 - 0x000027FF */
#define FLASH_HP_CF_BLOCK_5               (0x00002800U)  /*    2 KB:  0x00002800 - 0x00002FFF */
#define FLASH_HP_CF_BLOCK_6               (0x00003000U)  /*    2 KB:  0x00003000 - 0x000037FF */
#define FLASH_HP_CF_BLOCK_7               (0x00003800U)  /*    2 KB:  0x00003800 - 0x00003FFF */
#define FLASH_HP_CF_BLOCK_8               (0x00004000U)  /*    2 KB:  0x00004000 - 0x000047FF */
#define FLASH_HP_CF_BLOCK_9               (0x00004800U)  /*    2 KB:  0x00004800 - 0x00004FFF */
#define FLASH_HP_CF_BLOCK_10              (0x00005000U)  /*    2 KB:  0x00005000 - 0x000057FF */
#else
#define FLASH_HP_CF_BLOCK_0               (0x00000000U)  /*    8 KB:  0x00000000 - 0x00001FFF */
#define FLASH_HP_CF_BLOCK_1               (0x00002000U)  /*    8 KB:  0x00002000 - 0x00003FFF */
#define FLASH_HP_CF_BLOCK_2               (0x00004000U)  /*    8 KB:  0x00004000 - 0x00005FFF */
#define FLASH_HP_CF_BLOCK_3               (0x00006000U)  /*    8 KB:  0x00006000 - 0x00007FFF */
#define FLASH_HP_CF_BLOCK_4               (0x00008000U)  /*    8 KB:  0x00008000 - 0x00009FFF */
#define FLASH_HP_CF_BLOCK_5               (0x0000A000U)  /*    8 KB:  0x0000A000 - 0x0000BFFF */
#define FLASH_HP_CF_BLOCK_6               (0x0000C000U)  /*    8 KB:  0x0000C000 - 0x0000DFFF */
#define FLASH_HP_CF_BLOCK_7               (0x0000E000U)  /*    8 KB:  0x0000E000 - 0x0000FFFF */
#define FLASH_HP_CF_BLOCK_8               (0x00010000U)  /*   32 KB:  0x00010000 - 0x00017FFF */
#define FLASH_HP_CF_BLOCK_9               (0x00018000U)  /*   32 KB:  0x00018000 - 0x0001FFFF */
#define FLASH_HP_CF_BLOCK_10              (0x00020000U)  /*   32 KB:  0x00020000 - 0x0004FFFF */
#endif

/* Data Flash */
#if (defined (BOARD_RA6M4_EK) || defined (BOARD_RA6M5_EK) || defined (BOARD_RA4M3_EK)||defined(BOARD_RA4M2_EK)\
       || defined(BOARD_RA4E2_EK) || defined(BOARD_RA6E2_EK) || defined(BOARD_RA4E1_FPB) || defined(BOARD_RA6E1_FPB)\
       || defined(BOARD_RA4T1_MCK) || defined(BOARD_RA6T2_MCK) || defined(BOARD_RA6T3_MCK) )
#define FLASH_HP_DF_BLOCK_0               (0x08000000U) /*   64 B:  0x08000000 - 0x0800003F */
#define FLASH_HP_DF_BLOCK_1               (0x08000040U) /*   64 B:  0x08000040 - 0x0800007F */
#define FLASH_HP_DF_BLOCK_2               (0x08000080U) /*   64 B:  0x08000080 - 0x080000BF */
#define FLASH_HP_DF_BLOCK_3               (0x080000C0U) /*   64 B:  0x080000C0 - 0x080000FF */
#elif (defined (BOARD_RA8D1_EK) || defined (BOARD_RA8M1_EK) || defined (BOARD_RA8T1_MCK) || defined (BOARD_RA8E1_FPB)\
       || defined (BOARD_RA8E2_EK))
#define FLASH_HP_DF_BLOCK_0               (0x27000000U) /*   64 B:  0x27000000 - 0x2700003F */
#define FLASH_HP_DF_BLOCK_1               (0x27000040U) /*   64 B:  0x27000040 - 0x2700007F */
#define FLASH_HP_DF_BLOCK_2               (0x27000080U) /*   64 B:  0x27000080 - 0x270000BF */
#define FLASH_HP_DF_BLOCK_3               (0x270000C0U) /*   64 B:  0x270000C0 - 0x270000FF */
#elif (defined (BOARD_RA4L1_EK) || defined (BOARD_RA4C1_EK))
#define FLASH_HP_DF_BLOCK_0               (0x08000000U) /*   256 B:  0x08000000 - 0x080000FF */
#define FLASH_HP_DF_BLOCK_1               (0x08000100U) /*   256 B:  0x08000100 - 0x080001FF */
#define FLASH_HP_DF_BLOCK_2               (0x08000200U) /*   256 B:  0x08000200 - 0x080002FF */
#define FLASH_HP_DF_BLOCK_3               (0x08000300U) /*   256 B:  0x08000300 - 0x080003FF */
#else
#define FLASH_HP_DF_BLOCK_0               (0x40100000U) /*   64 B:  0x40100000 - 0x4010003F */
#define FLASH_HP_DF_BLOCK_1               (0x40100040U) /*   64 B:  0x40100040 - 0x4010007F */
#define FLASH_HP_DF_BLOCK_2               (0x40100080U) /*   64 B:  0x40100080 - 0x401000BF */
#define FLASH_HP_DF_BLOCK_3               (0x401000C0U) /*   64 B:  0x401000C0 - 0x401000FF */
#endif

/* Switch Commands */
#define CODE_FLASH                        (1U)
#define DATA_FLASH                        (2U)
#define EXIT                              (3U)
#define BUFF_SIZE                         (0x0F)
#define BUFF_INDEX                        (0x00)

/* Printing read data */
#define READ_DATA_PRINT(buff)             ({\
                                          for(uint8_t index = RESET_VALUE; BLOCK_SIZE > index; index++)\
                                          {     APP_PRINT("%x ",*(read_buffer+index));\
                                                if((index + 1) % 8 == 0) {\
                                                    APP_PRINT("\r\n");  }\
                                          }})

/* Flash_hp operating functions */
fsp_err_t flash_hp_code_flash_operations(void);
fsp_err_t flash_hp_data_flash_operations(void);
void flash_hp_deinit(void);
void exit_flash_hp(void);

/*******************************************************************************************************************//**
 * @} (end defgroup FLASH_HP_EP)
 **********************************************************************************************************************/

#endif /* FLASH_HP_EP_H_ */
