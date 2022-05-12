/***********************************************************************************************************************
 * File Name    : dwt.c
 * Description  : Contains data structures and functions used in dwt.c
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

#include "dwt.h"
#include "common_utils.h"

/*********************************************************************************************************************
 *  @brief       Enable the DWT
 *  @param[IN]   None
 *  @retval      None
***********************************************************************************************************************/
void InitCycleCounter(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk |CoreDebug_DEMCR_MON_EN_Msk;
}

/*********************************************************************************************************************
 *  @brief       Reset the DWT Cycle Counter
 *  @param[IN]   None
 *  @retval      None
***********************************************************************************************************************/
void ResetCycleCounter(void)
{
    DWT->CYCCNT = 0;
}

/*********************************************************************************************************************
 *  @brief       Enable the DWT Cycle Counter
 *  @param[IN]   None
 *  @retval      None
***********************************************************************************************************************/
void EnableCycleCounter(void)
{
    DWT->CTRL |= (DWT_CTRL_CYCCNTENA_Msk << DWT_CTRL_CYCCNTENA_Pos);
}
/*********************************************************************************************************************
 *  @brief       Read the DWT Cycle Counter
 *  @param[IN]   None
 *  @retval      the cycle count
***********************************************************************************************************************/
uint32_t GetCycleCounter(void)
{
    return DWT->CYCCNT;
}
/*********************************************************************************************************************
 *  @brief       Disable the DWT
 *  @param[IN]   None
 *  @retval      None
***********************************************************************************************************************/
void DisableCycleCounter(void)
{
    CoreDebug->DEMCR &= ~(CoreDebug_DEMCR_TRCENA_Msk |CoreDebug_DEMCR_MON_EN_Msk);
}
