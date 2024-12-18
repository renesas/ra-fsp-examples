/***********************************************************************************************************************
 * File Name    : s_cache.c
 * Description  : Contains data structures and functions used in s_cache.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "s_cache.h"
#include "common_utils.h"

#define S_CACHE_FLUSH_REG_ADDR 0x40007044
#define S_CACHE_ENABLE_REG_ADDR 0x40007040

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
        R_CACHE->SCALCF = 0x02;
 }


