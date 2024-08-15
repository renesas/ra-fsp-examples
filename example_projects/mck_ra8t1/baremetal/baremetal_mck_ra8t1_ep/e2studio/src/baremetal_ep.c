/***********************************************************************************************************************
* Copyright (c) 2021 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
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

/** LED Pin connections */
#define LED_1                           (0U)
#define LED_2                           (1U)
#define LED_3                           (2U)
#define LED_4                           (3U)

#define IRQ_WAIT_TIME                   (10UL)

extern bsp_leds_t g_bsp_leds;            // LED Structure used to blink on board LED

/**
 * Turn ON single LED at a time.
 * Note: LEDs are initialized by R_BSP_WarmStart function in hal_entry.c
 */
#define LED_TURN_ON(x)                                                                 \
({                                                                                     \
    R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[LED_1], BSP_IO_LEVEL_HIGH);    \
    R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[LED_2], BSP_IO_LEVEL_HIGH);    \
    R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[LED_3], BSP_IO_LEVEL_HIGH);    \
    R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[LED_4], BSP_IO_LEVEL_HIGH);    \
    R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[(x)], BSP_IO_LEVEL_LOW);       \
})                                                                                     \

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

//void baremetal_ep (void);

void baremetal_ep(void)
{
    volatile uint32_t execution_counter = UINT32_MAX;

    /** Disable Module-Stop Function */
    R_MSTP->MSTPCRC_b.MSTPC13 = 0;

    /** Configure the DOC to signal on successful compare match */
    #if defined (BOARD_RA8M1_EK) || defined (BOARD_RA8T1_MCK)
        R_DOC_B->DOCR_b.OMS = 0;
        R_DOC_B->DOCR_b.DCSEL = 1;
        R_DOC_B->DOCR_b.DOBW = 0;
        R_DOC_B->DOSCR_b.DOPCFCL = 1;
    #else
        R_DOC->DOCR_b.OMS = 0;
        R_DOC->DOCR_b.DCSEL = 1;
        R_DOC->DOCR_b.DOPCFCL = 1;
    #endif



    /** Set the value which should result in a comparison match */
    #if defined (BOARD_RA8M1_EK) || defined (BOARD_RA8T1_MCK)
        R_DOC_B->DODSR0 = DOC_DATA_COMPARISON_MATCH;
    #else
        R_DOC->DODSR = DOC_DATA_COMPARISON_MATCH;
    #endif

    /** Enable the interrupt */
    R_BSP_IrqEnable(DOC_INT_IRQn);

    /** Pass invalid data */
    #if defined (BOARD_RA8M1_EK) || defined (BOARD_RA8T1_MCK)
        R_DOC_B->DODIR = DOC_DATA_COMPARISON_MISMATCH;
    #else
        R_DOC->DODIR = DOC_DATA_COMPARISON_MISMATCH;
    #endif

    /** Check if interrupt to occurred */
    execution_counter = IRQ_WAIT_TIME;
    do
    {
        --execution_counter;
    } while ((false == s_baremetal_doc_isr_executed) && (execution_counter > 0));

    #if defined (BOARD_RA8M1_EK) || defined (BOARD_RA8T1_MCK)
        if ((1U == R_DOC_B->DOSR_b.DOPCF) || (true == s_baremetal_doc_isr_executed))
        {
            APP_ERR_TRAP(FSP_ERR_INVALID_STATE);
        }
    #else
        if ((1U == R_DOC->DOCR_b.DOPCF) || (true == s_baremetal_doc_isr_executed))
        {
            APP_ERR_TRAP(FSP_ERR_INVALID_STATE);
        }
    #endif

    /** Pass valid data */
    #if defined (BOARD_RA8M1_EK) || defined (BOARD_RA8T1_MCK)
        R_DOC_B->DODIR = DOC_DATA_COMPARISON_MATCH;
    #else
        R_DOC->DODIR = DOC_DATA_COMPARISON_MATCH;
    #endif


    /** Wait until the ISR executes and modifies the flag */
    LED_TURN_ON(LED_2);

    execution_counter = UINT32_MAX;
    do
    {
        --execution_counter;
    } while ((false == s_baremetal_doc_isr_executed) && (execution_counter > 0));

    if((execution_counter <= 0) && (false == s_baremetal_doc_isr_executed))
    {
        LED_TURN_ON(LED_1);
        APP_ERR_TRAP(FSP_ERR_INVALID_STATE);
        return;
    }

    /** Check if DOC operation resulted in successful comparison */
    #if defined (BOARD_RA8M1_EK) || defined (BOARD_RA8T1_MCK)
         if (0U == R_DOC_B->DOSR_b.DOPCF)
         {
             LED_TURN_ON(LED_1);
             APP_ERR_TRAP(FSP_ERR_INVALID_STATE);
             return;
         }
    #else
        if (0U == R_DOC->DOCR_b.DOPCF)
        {
            LED_TURN_ON(LED_1);
            APP_ERR_TRAP(FSP_ERR_INVALID_STATE);
            return;
        }
    #endif


    /** Clear the DOPCF bit & the flag */
    #if defined (BOARD_RA8M1_EK) || defined (BOARD_RA8T1_MCK)
        R_DOC_B->DOSCR_b.DOPCFCL = 1;
    #else
        R_DOC->DOCR |= R_DOC_DOCR_DOPCFCL_Msk;
    #endif

    s_baremetal_doc_isr_executed = false;

    /** Turn on LED 3 to indicate success. */
    LED_TURN_ON(LED_3);
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

