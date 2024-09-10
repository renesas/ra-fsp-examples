/***********************************************************************************************************************
 * File Name    : common_util.h
 * Description  : common utility header file.
 **********************************************************************************************************************/
/**********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/
#ifndef COMMON_UTIL_H_
#define COMMON_UTIL_H_

#define SWO_PRINTF
   
#define APP_ERR_TRAP(a)             if(a) {__asm("BKPT #0\n");} /* trap the error location */


#define PRCR_KEY (0xa500UL)
#define PRC0_MASK (0x0001UL)
#define TRCKCR (*(volatile uint8_t *)0x4001E03F)
#define TRCKEN 0x80
#define TRCK_1 0x00
#define TRCK_2 0x01
#define TRCK_4 0x02

/* Need to verify the exact amount of padding needed */
#define SECURE_PADDING do {\
    /* 2 instructions (4 bytes) for the pipeline */\
    __asm volatile ("   nop\n");\
    __asm volatile ("   nop\n");\
    /* 3 words (12 bytes) for the max prefetch size */\
    __asm volatile ("   nop\n");\
    __asm volatile ("   nop\n");\
    __asm volatile ("   nop\n");\
    __asm volatile ("   nop\n");\
    __asm volatile ("   nop\n");\
    __asm volatile ("   nop\n");\
    } while(0);
#endif /* COMMON_UTIL_H_ */
