/***********************************************************************************************************************
 * File Name    : DAR_utilities.h
 * Description  : Contains macros, and functions definitions used for Security MPU register access
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
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

/* change the FAW_START address to demonstrate more use cases of the Security MPU and FAW usage */
//#define FAW_START   (0x0a0000) // flash access window start
#define FAW_START   (0x100000) // flash access window start
#define FAW_END     (0x200000)  // flash access window ends

#endif /* DAR_UTILITIES_H_ */
