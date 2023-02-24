/***********************************************************************************************************************
 * File Name    : DAR_utilities.h
 * Description  : Contains macros, and functions definitions used for Security MPU register access
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
