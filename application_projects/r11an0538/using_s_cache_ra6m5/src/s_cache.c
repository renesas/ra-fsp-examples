/***********************************************************************************************************************
 * File Name    : s_cache.c
 * Description  : Contains data structures and functions used in s_cache.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright [2020-2021] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
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

#include "s_cache.h"
#include "common_utils.h"

#define S_CACHE_FLUSH_REG_ADDR 0x40007044
#define S_CACHE_ENABLE_REG_ADDR 0x40007040
//volatile uint32_t *invalidate_s_cache = (volatile uint32_t *)S_CACHE_FLUSH_REG_ADDR;

/*********************************************************************************************************************
 *  @brief       Enable S Cache
 *  @param[IN]   None
 *  @retval      None
***********************************************************************************************************************/
void enable_s_cache(void)
{
    /* Flush S cache */
    R_CACHE->SCAFCT = 1;
   /*Enable  S cache */
    R_CACHE->SCACTL = 1;
}

/*********************************************************************************************************************
 *  @brief       Disable S Cache
 *  @param[IN]   None
 *  @retval      None
***********************************************************************************************************************/
void disable_s_cache(void)
{
    /* Flush S cache */
    R_CACHE->SCAFCT = 1;
    /*Disable  S cache */
    R_CACHE->SCACTL &= 0x00;
}

/*********************************************************************************************************************
 *  @brief       Flush S Cache
 *  @param[IN]   None
 *  @retval      None
***********************************************************************************************************************/
void flush_s_cache(void)
{
    /* Flush S cache */
    R_CACHE->SCAFCT = 1;
}

/*********************************************************************************************************************
 *  @brief       Set S Cache Line Size
 *  @param[IN]   if line_size_32 is true, line size is set to 32; if line_size_32 is false, line size is set to 64.
 *  @retval      None
***********************************************************************************************************************/
void select_s_cache_line_size(bool line_size_32)
{
    /* update the s cache line control register */
     if(true == line_size_32)
        R_CACHE->SCALCF = 0x01;
    else
        R_CACHE->SCALCF = 0x10;
 }


