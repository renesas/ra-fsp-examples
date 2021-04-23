/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2021 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/*******************************************************************************************************************//**
 * @addtogroup BAREMETAL_EP
 * @{
 **********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : baremetal_ep.c
 * Version      : 1.0
 * Description  : .
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 09.03.2021 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "common_utils.h"
/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

/** Data written to DOC register for comparison match event */
#define DOC_DATA_COMPARISON_MATCH       (0xA5A5U)

/** Data written to DOC register for comparison mismatch event */
#define DOC_DATA_COMPARISON_MISMATCH    (0xB5B5U)

/** LED Pin connections on PORT5 of MCU */
#define LED_GREEN                       (1<<4U)
#define LED_BLUE                        (1<<3U)
#define LED_RED                         (1<<5U)

#define LED_ALL                         (LED_RED | LED_BLUE | LED_GREEN)

#define IRQ_WAIT_TIME                       (10UL)

/**
 * Turn ON single LED at a time.
 * Note: LEDs are initialized by R_BSP_WarmStart function in hal_entry.c
 */
#define LED_TURN_ON(x)                  ({R_PORT5->PODR &= (uint16_t)~(LED_ALL); R_PORT5->PODR |= (x);})

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
static volatile bool s_baremetal_doc_isr_executed = false;

void baremetal_ep(void);

void baremetal_ep(void)
{
    volatile uint32_t execution_counter = UINT32_MAX;

    /** Disable Module-Stop Function */
    R_MSTP->MSTPCRC_b.MSTPC13 = 0;

    /** Configure the DOC to signal on successful compare match */
    R_DOC->DOCR_b.OMS = 0;
    R_DOC->DOCR_b.DCSEL = 1;
    R_DOC->DOCR_b.DOPCFCL = 1;

    /** Set the value which should result in a comparison match */
    R_DOC->DODSR = DOC_DATA_COMPARISON_MATCH;

    /** Enable the interrupt */
    R_BSP_IrqEnable(DOC_INT_IRQn);

    /** Pass invalid data */
    R_DOC->DODIR = DOC_DATA_COMPARISON_MISMATCH;

    /** Check if interrupt to occurred */
    execution_counter = IRQ_WAIT_TIME;
    do
    {
        --execution_counter;
    } while ((false == s_baremetal_doc_isr_executed) && (execution_counter > 0));

    if ((1U == R_DOC->DOCR_b.DOPCF) || (true == s_baremetal_doc_isr_executed))
    {
        APP_ERR_TRAP(FSP_ERR_INVALID_STATE);
    }

    /** Pass valid data */
    R_DOC->DODIR = DOC_DATA_COMPARISON_MATCH;

    /** Wait until the ISR executes and modifies the flag */
    LED_TURN_ON(LED_BLUE);

    execution_counter = UINT32_MAX;
    do
    {
        --execution_counter;
    } while ((false == s_baremetal_doc_isr_executed) && (execution_counter > 0));

    if((execution_counter <= 0) && (false == s_baremetal_doc_isr_executed))
    {
        LED_TURN_ON(LED_RED);
        APP_ERR_TRAP(FSP_ERR_INVALID_STATE);
        return;
    }

    /** Check if DOC operation resulted in successful comparison */
    if (0U == R_DOC->DOCR_b.DOPCF)
    {
        LED_TURN_ON(LED_RED);
        APP_ERR_TRAP(FSP_ERR_INVALID_STATE);
        return;
    }

    /** Clear the DOPCF bit & the flag */
    R_DOC->DOCR |= R_DOC_DOCR_DOPCFCL_Msk;
    s_baremetal_doc_isr_executed = false;

    /** Turn on Green LED to indicate success. */
    LED_TURN_ON(LED_GREEN);
}
/**********************************************************************************************************************
 End of function baremetal_ep
 *********************************************************************************************************************/



/**********************************************************************************************************************
 * Function Name: baremetal_doc_isr
 * Description  : Interrupt Service Routine to execute when the DOC.
 * Return Value : None.
 *********************************************************************************************************************/
void baremetal_doc_isr(void)
{
    /** Get the IRQ Number */
    IRQn_Type irq = R_FSP_CurrentIrqGet();

    /** Clear the flag */
    s_baremetal_doc_isr_executed = true;

    /** Clear the IR flag in the ICU */
    R_BSP_IrqStatusClear(irq);
}
/**********************************************************************************************************************
 End of function baremetal_doc_isr
 *********************************************************************************************************************/
/*******************************************************************************************************************//**
 * @} (end defgroup BAREMETAL_EP)
 **********************************************************************************************************************/

