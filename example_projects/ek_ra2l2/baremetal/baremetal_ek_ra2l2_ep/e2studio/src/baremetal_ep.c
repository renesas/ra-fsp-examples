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

/* Data written to DOC register for comparison match event */
#define DOC_DATA_COMPARISON_MATCH       (0xA5A5U)

/* Data written to DOC register for comparison mismatch event */
#define DOC_DATA_COMPARISON_MISMATCH    (0xB5B5U)
#define WAIT_TIME                       (500U)      /* Wait time value */
#define IRQ_WAIT_TIME                   (10UL)

/* Enum to select LED that is to be made ON */
typedef enum
{
   LED_CASE_1 = 1,
   LED_CASE_2 = 2,
   LED_CASE_3 = 3,
} led_state_t;

/* Enum to select status of LED */

typedef enum
{
#if defined (BOARD_RA8T1_MCK) || defined (BOARD_RA4T1_MCK) || defined (BOARD_RA6T3_MCK)
   LED_ON = BSP_IO_LEVEL_LOW,
   LED_OFF = BSP_IO_LEVEL_HIGH,
#else
   LED_ON = BSP_IO_LEVEL_HIGH,
   LED_OFF = BSP_IO_LEVEL_LOW,
#endif
} led_status_t;

extern bsp_leds_t g_bsp_leds;   /* LED Structure used to blink on board LED */

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
static void led_update(led_state_t led_state);

void baremetal_ep(void)
{
    volatile uint32_t execution_counter = UINT32_MAX;

    /* Disable Module-Stop Function */
    R_MSTP->MSTPCRC_b.MSTPC13 = 0;

    /* Configure the DOC to signal on successful compare match */
#if (BSP_FEATURE_DOC_VERSION == 2)
    R_DOC_B->DOCR_b.OMS = 0;
    R_DOC_B->DOCR_b.DCSEL = 1;
    R_DOC_B->DOCR_b.DOBW = 0;
    R_DOC_B->DOSCR_b.DOPCFCL = 1;
#else
    R_DOC->DOCR_b.OMS = 0;
    R_DOC->DOCR_b.DCSEL = 1;
    R_DOC->DOCR_b.DOPCFCL = 1;
#endif

    /* Set the value which should result in a comparison match */
#if (BSP_FEATURE_DOC_VERSION == 2)
    R_DOC_B->DODSR0 = DOC_DATA_COMPARISON_MATCH;
#else
    R_DOC->DODSR = DOC_DATA_COMPARISON_MATCH;
#endif

    /* Enable the interrupt */
    R_BSP_IrqEnable(DOC_INT_IRQn);

    /* Pass invalid data */
#if (BSP_FEATURE_DOC_VERSION == 2)
    R_DOC_B->DODIR = DOC_DATA_COMPARISON_MISMATCH;
#else
    R_DOC->DODIR = DOC_DATA_COMPARISON_MISMATCH;
#endif

    /* Check if interrupt to occurred */
    execution_counter = IRQ_WAIT_TIME;
    do
    {
        --execution_counter;
    } while ((false == s_baremetal_doc_isr_executed) && (execution_counter > 0));

#if (BSP_FEATURE_DOC_VERSION == 2)
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

    /* Pass valid data */
#if (BSP_FEATURE_DOC_VERSION == 2)
    R_DOC_B->DODIR = DOC_DATA_COMPARISON_MATCH;
#else
    R_DOC->DODIR = DOC_DATA_COMPARISON_MATCH;
#endif

    /* Wait until the ISR executes and modifies the flag */
    led_update(LED_CASE_1);

    execution_counter = UINT32_MAX;
    do
    {
        --execution_counter;
    } while ((false == s_baremetal_doc_isr_executed) && (execution_counter > 0));

    if((execution_counter <= 0) && (false == s_baremetal_doc_isr_executed))
    {
        led_update(LED_CASE_3);
        APP_ERR_TRAP(FSP_ERR_INVALID_STATE);
        return;
    }

    /* Check if DOC operation resulted in successful comparison */
#if (BSP_FEATURE_DOC_VERSION == 2)
    if (0U == R_DOC_B->DOSR_b.DOPCF)
    {
        led_update (LED_CASE_3);
        APP_ERR_TRAP(FSP_ERR_INVALID_STATE);
        return;
    }
#else
    if (0U == R_DOC->DOCR_b.DOPCF)
    {
        led_update(LED_CASE_3);
        APP_ERR_TRAP(FSP_ERR_INVALID_STATE);
        return;
    }
#endif

    /* Clear the DOPCF bit & the flag */
#if (BSP_FEATURE_DOC_VERSION == 2)
    R_DOC_B->DOSCR_b.DOPCFCL = 1;
#else
    R_DOC->DOCR |= R_DOC_DOCR_DOPCFCL_Msk;
#endif

    s_baremetal_doc_isr_executed = false;

    /* Turn on LED 2 to indicate success. */
    led_update(LED_CASE_2);
}
/**********************************************************************************************************************
 End of function baremetal_ep
 *********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief This function updates led state as per operation status
 * @param[in]  led_state      Selects which led has to be made high
 * @retval     None
 **********************************************************************************************************************/
static void led_update(led_state_t led_state)
{
    if (g_bsp_leds.led_count == 2)
    {
        switch (led_state)
        {
            case LED_CASE_1:
            {
                /* LED 1 state is made ON to show operation is in progress */
                R_IOPORT_PinWrite (&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[0], (bsp_io_level_t) LED_ON);
                R_IOPORT_PinWrite (&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[1], (bsp_io_level_t) LED_OFF);
                /* Delay */
                R_BSP_SoftwareDelay (WAIT_TIME, BSP_DELAY_UNITS_MICROSECONDS);
                break;
            }
            case LED_CASE_2:
            {
                /* LED 2 state is made ON to show successful state */
                R_IOPORT_PinWrite (&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[0], (bsp_io_level_t) LED_OFF);
                R_IOPORT_PinWrite (&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[1], (bsp_io_level_t) LED_ON);
                /* Delay */
                R_BSP_SoftwareDelay (WAIT_TIME, BSP_DELAY_UNITS_MICROSECONDS);
                break;
            }
            case LED_CASE_3:
            {
                /* LED 1 & LED 2 states are made ON to show error state */
                R_IOPORT_PinWrite (&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[0], (bsp_io_level_t) LED_ON);
                R_IOPORT_PinWrite (&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[1], (bsp_io_level_t) LED_ON);
                /* Delay */
                R_BSP_SoftwareDelay (WAIT_TIME, BSP_DELAY_UNITS_MICROSECONDS);
                break;
            }
            default:
            {
                break;
            }
        }

    }
    else if (g_bsp_leds.led_count == 3)
    {
        switch (led_state)
        {
            case LED_CASE_1:
            {
                /* LED 1 state is made ON to show operation is in progress */
                R_IOPORT_PinWrite (&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[0], (bsp_io_level_t) LED_ON);
                R_IOPORT_PinWrite (&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[1], (bsp_io_level_t) LED_OFF);
                R_IOPORT_PinWrite (&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[2], (bsp_io_level_t) LED_OFF);
                /* Delay */
                R_BSP_SoftwareDelay (WAIT_TIME, BSP_DELAY_UNITS_MICROSECONDS);
                break;
            }
            case LED_CASE_2:
            {
                /* LED 2 state is made ON to show successful state */
                R_IOPORT_PinWrite (&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[0], (bsp_io_level_t) LED_OFF);
                R_IOPORT_PinWrite (&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[1], (bsp_io_level_t) LED_ON);
                R_IOPORT_PinWrite (&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[2], (bsp_io_level_t) LED_OFF);
                /* Delay */
                R_BSP_SoftwareDelay (WAIT_TIME, BSP_DELAY_UNITS_MICROSECONDS);
                break;
            }
            case LED_CASE_3:
            {
                /* LED 3 state is made ON to show error state */
                R_IOPORT_PinWrite (&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[0], (bsp_io_level_t) LED_OFF);
                R_IOPORT_PinWrite (&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[1], (bsp_io_level_t) LED_OFF);
                R_IOPORT_PinWrite (&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[2], (bsp_io_level_t) LED_ON);
                /* Delay */
                R_BSP_SoftwareDelay (WAIT_TIME, BSP_DELAY_UNITS_MICROSECONDS);
                break;
            }
            default:
            {
                break;
            }
        }
    }
}

/**********************************************************************************************************************
 * Function Name: baremetal_doc_isr
 * Description  : Interrupt Service Routine to execute when the DOC.
 * Return Value : None.
 *********************************************************************************************************************/
void baremetal_doc_isr(void)
{
    /* Get the IRQ Number */
    IRQn_Type irq = R_FSP_CurrentIrqGet();

    /* Clear the flag */
    s_baremetal_doc_isr_executed = true;

    /* Clear the IR flag in the ICU */
    R_BSP_IrqStatusClear(irq);
}
/**********************************************************************************************************************
 End of function baremetal_doc_isr
 *********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @} (end defgroup BAREMETAL_EP)
 **********************************************************************************************************************/
