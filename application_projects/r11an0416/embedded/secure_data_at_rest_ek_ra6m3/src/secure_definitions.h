/***********************************************************************************************************************
 * File Name    : secure_definitions.h
 * Description  : Contains Secure code region definitions and Security operation related macros
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Copyright [2020] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
 *
 * This software and documentation are supplied by Renesas Electronics America Inc. and may only be used with products
 * of Renesas Electronics Corp. and its affiliates ("Renesas").  No other uses are authorized.  Renesas products are
 * sold pursuant to Renesas terms and conditions of sale.  Purchasers are solely responsible for the selection and use
 * of Renesas products and Renesas assumes no liability.  No license, express or implied, to any intellectual property
 * right is granted by Renesas. This software is protected under all applicable laws, including copyright laws. Renesas
 * reserves the right to change or discontinue this software and/or this documentation. THE SOFTWARE AND DOCUMENTATION
 * IS DELIVERED TO YOU "AS IS," AND RENESAS MAKES NO REPRESENTATIONS OR WARRANTIES, AND TO THE FULLEST EXTENT
 * PERMISSIBLE UNDER APPLICABLE LAW, DISCLAIMS ALL WARRANTIES, WHETHER EXPLICITLY OR IMPLICITLY, INCLUDING WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT, WITH RESPECT TO THE SOFTWARE OR
 * DOCUMENTATION.  RENESAS SHALL HAVE NO LIABILITY ARISING OUT OF ANY SECURITY VULNERABILITY OR BREACH.  TO THE MAXIMUM
 * EXTENT PERMITTED BY LAW, IN NO EVENT WILL RENESAS BE LIABLE TO YOU IN CONNECTION WITH THE SOFTWARE OR DOCUMENTATION
 * (OR ANY PERSON OR ENTITY CLAIMING RIGHTS DERIVED FROM YOU) FOR ANY LOSS, DAMAGES, OR CLAIMS WHATSOEVER, INCLUDING,
 * WITHOUT LIMITATION, ANY DIRECT, CONSEQUENTIAL, SPECIAL, INDIRECT, PUNITIVE, OR INCIDENTAL DAMAGES; ANY LOST PROFITS,
 * OTHER ECONOMIC DAMAGE, PROPERTY DAMAGE, OR PERSONAL INJURY; AND EVEN IF RENESAS HAS BEEN ADVISED OF THE POSSIBILITY
 * OF SUCH LOSS, DAMAGES, CLAIMS OR COSTS.
 **********************************************************************************************************************/
#ifndef SECURE_DEFINITIONS_H_
#define SECURE_DEFINITIONS_H_

#include <stdint.h>

#define _unused(x) ((void)(x))

#define SECURE_PROGRAM __attribute__((section (".secure_text")))
#define SECURE_CONST   __attribute__((section (".secure_rodata")))
#define SECURE_DATA    __attribute__((section (".secure_data")))
#define SECURE_BSS     __attribute__((section (".secure_bss")))
#define SECURE_SRAM_PROGRAM	__attribute__((section (".secure_sram_program")))

#define SECURE_DATA_CONST_VALUE         (0x53454331)  // "SEC1"
#define SECURE_DATA_VALUE               (0x53454332)  // "SEC2"
#define SECURE_DATA_WRITE_TEST_VALUE    (0x53455752)  // "SEWR"

#define DATA_CONST_VALUE        (0xCAFECAFE)
#define DATA_VALUE              (0xDEADBEEF)
#define DATA_WRITE_TEST_VALUE   (0x554E5752)  // "UNWR"

#define DUMMY_VALUE 			(0xF0F0F0F0)

#define SECURE_FLASH_WRITE_LENGTH		(128)
#define FLASH_WRITE_LENGTH              (128)
/*
 * secure data
 * they need to be global variables so the security of the secure regions can be verified
 */
extern SECURE_BSS volatile uint32_t s_dataBss;
extern SECURE_CONST const   uint32_t s_dataConst;
extern SECURE_DATA volatile uint32_t s_dataInit;
extern SECURE_DATA volatile uint32_t s_dataWritten;

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
    } while(0)

#endif /* SECURE_DEFINITIONS_H_ */
