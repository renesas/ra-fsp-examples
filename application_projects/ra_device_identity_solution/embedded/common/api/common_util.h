/***********************************************************************************************************************
 * File Name    : common_util.h
 * Description  : common utility header file.
 **********************************************************************************************************************/
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
