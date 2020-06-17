/***********************************************************************************************************************
 * File Name    : DAR_utilities.h
 * Description  : Contains macros, and functions definitions used for Security MPU register access
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
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#ifndef DAR_UTILITIES_H_
#define DAR_UTILITIES_H_

#include "secure_definitions.h"

extern volatile uint16_t *reg_secureMPUAccessControl;
extern volatile uint32_t *reg_secureMPU_PC_0_Start;
extern volatile uint32_t *reg_secureMPU_PC_1_Start;
extern volatile uint32_t *reg_secureMPU_PC_0_End;
extern volatile uint32_t *reg_secureMPU_PC_1_End;
extern volatile uint32_t *reg_secureMPU_Region0_Start;
extern volatile uint32_t *reg_secureMPU_Region1_Start;
extern volatile uint32_t *reg_secureMPU_Region0_End;
extern volatile uint32_t *reg_secureMPU_Region1_End;
extern volatile uint32_t *reg_secureMPU_Region2_Start;
extern volatile uint32_t *reg_secureMPU_Region3_Start;
extern volatile uint32_t *reg_secureMPU_Region2_End;
extern volatile uint32_t *reg_secureMPU_Region3_End;

extern void SECURE_PROGRAM  s_readSecureMpuSettings(uint16_t *secMpuAc);
extern void SECURE_PROGRAM  s_readPc0StartAddress(uint32_t* start);
extern void SECURE_PROGRAM  s_readPc1StartAddress(uint32_t* start);
extern void SECURE_PROGRAM  s_readRegion0StartAddress(uint32_t* start);
extern void SECURE_PROGRAM  s_readRegion1StartAddress(uint32_t* start);
extern void SECURE_PROGRAM  s_readRegion2StartAddress(uint32_t* start);
extern void SECURE_PROGRAM  s_readRegion3StartAddress(uint32_t* start);

extern void SECURE_PROGRAM  s_readPc0EndAddress(uint32_t* end);
extern void SECURE_PROGRAM  s_readPc1EndAddress(uint32_t* end);
extern void SECURE_PROGRAM  s_readRegion0EndAddress(uint32_t* end);
extern void SECURE_PROGRAM  s_readRegion1EndAddress(uint32_t* end);
extern void SECURE_PROGRAM  s_readRegion2EndAddress(uint32_t* end);
extern void SECURE_PROGRAM  s_readRegion3EndAddress(uint32_t* end);
extern void SECURE_PROGRAM  s_read_faw_settings(uint32_t * faws, uint32_t * fawe);

#ifndef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#endif

/* two flash blocks used for flash erase and write testing */
#define FLASH_WRITE_TEST_BLOCK1 		(0xb0000)         // test block located in secure flash FAW unmodifiable region
#define FLASH_WRITE_TEST_BLOCK2 		(0x120000)            // test block located in non-secure flash FAW modifiable
#define FLASH_WRITE_LENGTH  			(128)
#define FLASH_WRITE_TEST_DATA           (0x33)


#endif /* DAR_UTILITIES_H_ */
