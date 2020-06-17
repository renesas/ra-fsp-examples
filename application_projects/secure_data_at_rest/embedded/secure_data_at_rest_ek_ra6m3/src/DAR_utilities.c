/***********************************************************************************************************************
 * File Name    : DAR_utilities.c
 * Description  : Contains Security MPU register macros and register access routines
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


#include <string.h>
#include "hal_data.h"
#include "DAR_utilities.h"


volatile uint16_t *reg_secureMPUAccessControl = (uint16_t *) 0x00000438;
volatile uint32_t *reg_secureMPU_PC_0_Start = (uint32_t *) 0x00000408;
volatile uint32_t *reg_secureMPU_PC_1_Start = (uint32_t *) 0x00000410;
volatile uint32_t *reg_secureMPU_PC_0_End = (uint32_t *) 0x0000040C;
volatile uint32_t *reg_secureMPU_PC_1_End = (uint32_t *) 0x00000414;
volatile uint32_t *reg_secureMPU_Region0_Start = (uint32_t *) 0x00000418;
volatile uint32_t *reg_secureMPU_Region1_Start = (uint32_t *) 0x00000420;
volatile uint32_t *reg_secureMPU_Region0_End = (uint32_t *) 0x0000041C;
volatile uint32_t *reg_secureMPU_Region1_End = (uint32_t *) 0x00000424;
volatile uint32_t *reg_secureMPU_Region2_Start = (uint32_t *) 0x00000428;
volatile uint32_t *reg_secureMPU_Region3_Start = (uint32_t *) 0x00000430;
volatile uint32_t *reg_secureMPU_Region2_End = (uint32_t *) 0x0000042C;
volatile uint32_t *reg_secureMPU_Region3_End = (uint32_t *) 0x00000434;


/*******************************************************************************************************************//**
 * start of functions reading the security MPU and FAW settings
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief s_readSecureMpuSettings function
 * This function reads the security MPU access control setting.
 *
 * return: void
 **********************************************************************************************************************/

void s_readSecureMpuSettings(uint16_t *secMpuAc)
{
    *secMpuAc = *reg_secureMPUAccessControl;
    SECURE_PADDING;
}

/*******************************************************************************************************************//**
 * @brief s_readPc0StartAddress function
 * This function reads the security MPU PC region0 start address.
 *
 * return: void
 **********************************************************************************************************************/
void s_readPc0StartAddress(uint32_t* start)
{
    *start = *reg_secureMPU_PC_0_Start;
    SECURE_PADDING;
}

/*******************************************************************************************************************//**
 * @brief s_readPc0EndAddress function
 * This function reads the security MPU PC region0 end address.
 *
 * return: void
 **********************************************************************************************************************/
void s_readPc0EndAddress(uint32_t* end)
{
    *end = *reg_secureMPU_PC_0_End;
    SECURE_PADDING;
}

/*******************************************************************************************************************//**
 * @brief s_readPc1StartAddress function
 * This function reads the security MPU PC region1 start address.
 *
 * return: void
 **********************************************************************************************************************/

void s_readPc1StartAddress(uint32_t* start)
{
    *start = *reg_secureMPU_PC_1_Start;
    SECURE_PADDING;
}

/*******************************************************************************************************************//**
 * @brief s_readPc1EndAddress function
 * This function reads the security MPU PC region1 end address.
 *
 * return: void
 **********************************************************************************************************************/

void s_readPc1EndAddress(uint32_t* end)
{
    *end = *reg_secureMPU_PC_1_End;
    SECURE_PADDING;
}

/*******************************************************************************************************************//**
 * @brief s_readRegion0StartAddress function
 * This function reads the secure flash region start address.
 *
 * return: void
 **********************************************************************************************************************/

void s_readRegion0StartAddress(uint32_t* start)
{
    *start = *reg_secureMPU_Region0_Start;
    SECURE_PADDING;
}

/*******************************************************************************************************************//**
 * @brief s_readRegion0StartAddress function
 * This function reads the secure flash region end address.
 *
 * return: void
 **********************************************************************************************************************/

void s_readRegion0EndAddress(uint32_t* end)
{
    *end = *reg_secureMPU_Region0_End;
    SECURE_PADDING;
}

/*******************************************************************************************************************//**
 * @brief s_readRegion1StartAddress function
 * This function reads the secure SRAM region start address.
 *
 * return: void
 **********************************************************************************************************************/

void s_readRegion1StartAddress(uint32_t* start)
{
    *start = *reg_secureMPU_Region1_Start;
    SECURE_PADDING;
}

/*******************************************************************************************************************//**
 * @brief s_readRegion1EndAddress function
 * This function reads the secure SRAM region end address.
 *
 * return: void
 **********************************************************************************************************************/

void s_readRegion1EndAddress(uint32_t* end)
{
    *end = *reg_secureMPU_Region1_End;
    SECURE_PADDING;
}

/*******************************************************************************************************************//**
 * @brief s_readRegion2StartAddress function
 * This function reads the security function 1 start address.
 *
 * return: void
 **********************************************************************************************************************/

void s_readRegion2StartAddress(uint32_t* start)
{
    *start = *reg_secureMPU_Region2_Start;
    SECURE_PADDING;
}

/*******************************************************************************************************************//**
 * @brief s_readRegion2EndAddress function
 * This function reads the security function 1 end address.
 *
 * return: void
 **********************************************************************************************************************/

void s_readRegion2EndAddress(uint32_t* end)
{
    *end = *reg_secureMPU_Region2_End;
    SECURE_PADDING;
}

/*******************************************************************************************************************//**
 * @brief s_readRegion3StartAddress function
 * This function reads the security function 2 start address.
 *
 * return: void
 **********************************************************************************************************************/

void s_readRegion3StartAddress(uint32_t* start)
{
    *start = *reg_secureMPU_Region3_Start;
    SECURE_PADDING;
}

/*******************************************************************************************************************//**
 * @brief s_readRegion3EndAddress function
 * This function reads the security function 2 start address.
 *
 * return: void
 **********************************************************************************************************************/

void s_readRegion3EndAddress(uint32_t* end)
{
    *end = *reg_secureMPU_Region3_End;
    SECURE_PADDING;
}

/*******************************************************************************************************************//**
 * @brief s_read_faw_settings function
 * This function reads the FAW settings.
 *
 * return: void
 **********************************************************************************************************************/

void s_read_faw_settings(uint32_t * faws, uint32_t * fawe)
{
	*faws =  (uint32_t) ((R_FACI_HP->FAWMON_b.FAWS) << 13);
	*fawe =  (uint32_t) ((R_FACI_HP->FAWMON_b.FAWE) << 13);
	SECURE_PADDING;
}
