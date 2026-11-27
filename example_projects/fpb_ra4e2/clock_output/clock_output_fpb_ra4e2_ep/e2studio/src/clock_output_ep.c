/***********************************************************************************************************************
 * File Name    : clock_output_ep.c
 * Description  : Contains data structures and functions used in clock_output_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2024 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "clock_output_ep.h"

#if !(defined (BOARD_RA4E2_FPB))
/***********************************************************************************************************************
 * Function Name: start_main_clock.
 * Description  : This function performs Main clock operation.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
void start_main_clock(void)
{
    /* Unlock protect register */
    R_SYSTEM->PRCR = (uint16_t) PRV_PRCR_UNLOCK;

    /* Clock out disable */
    R_SYSTEM->CKOCR_b.CKOEN = 0x0;

    /* Clock output frequency division ratio */
    R_SYSTEM->CKOCR_b.CKODIV = MAIN_CLOCK_DIV;

    /* Select Main clock in Clock Out Control Register */
#if defined (BOARD_RA8M1_EK) || defined (BOARD_RA8D1_EK)
    R_SYSTEM->CKOCR = (R_SYSTEM->CKOCR & (uint8_t) ~R_SYSTEM_CKOCR_CKOSEL_MSK)
    | (BSP_CLOCKS_SOURCE_CLOCK_MAIN_OSC << R_SYSTEM_CKOCR_CKOSEL_POS);
#else
    R_SYSTEM->CKOCR_b.CKOSEL = BSP_CLOCKS_SOURCE_CLOCK_MAIN_OSC;
#endif /* BOARD_RA8M1_EK, BOARD_RA8D1_EK */

    /* Clock out enable */
    R_SYSTEM->CKOCR_b.CKOEN = 0x1;

    /* Lock protect register */
    R_SYSTEM->PRCR = (uint16_t) PRV_PRCR_LOCK;
}
/***********************************************************************************************************************
* End of function start_main_clock
***********************************************************************************************************************/
#endif /* BOARD_RA4E2_FPB */

/***********************************************************************************************************************
 * Function Name: start_sub_clock.
 * Description  : This function performs Sub-clock operation.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
void start_sub_clock(void)
{
    /* Unlock protect register */
    R_SYSTEM->PRCR = (uint16_t) PRV_PRCR_UNLOCK;

    /* Clock out disable */
    R_SYSTEM->CKOCR_b.CKOEN = 0x0;

    /* Clock output frequency division ratio */
    R_SYSTEM->CKOCR_b.CKODIV = 0;

    /* Select Sub-clock in Clock Out Control Register */
#if defined (BOARD_RA8M1_EK) || defined (BOARD_RA8D1_EK)
    R_SYSTEM->CKOCR = (R_SYSTEM->CKOCR & (uint8_t) ~R_SYSTEM_CKOCR_CKOSEL_MSK)
    | (BSP_CLOCKS_SOURCE_CLOCK_SUBCLOCK << R_SYSTEM_CKOCR_CKOSEL_POS);
#else
    R_SYSTEM->CKOCR_b.CKOSEL = BSP_CLOCKS_SOURCE_CLOCK_SUBCLOCK;
#endif /* BOARD_RA8M1_EK, BOARD_RA8D1_EK */

    /* Clock out enable */
    R_SYSTEM->CKOCR_b.CKOEN = 0x1;

    /* Lock protect register */
    R_SYSTEM->PRCR = (uint16_t) PRV_PRCR_LOCK;
}
/***********************************************************************************************************************
* End of function start_sub_clock
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: stop_all_clocks.
 * Description  : This function stops all clocks.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
void stop_all_clocks(void)
{
    /* Unlock protect register */
    R_SYSTEM->PRCR = (uint16_t) PRV_PRCR_UNLOCK;

    /* Clock out disable */
    R_SYSTEM->CKOCR_b.CKOEN = 0x0;

    /* Lock protect register */
    R_SYSTEM->PRCR = (uint16_t) PRV_PRCR_LOCK;
}
/***********************************************************************************************************************
* End of function stop_all_clocks
***********************************************************************************************************************/
