/***********************************************************************************************************************
 * File Name    : baremetal_ep.c
 * Description  : Contains data structures and functions used in baremetal_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "baremetal_ep.h"

/* Variable declaration */
static volatile bool s_b_baremetal_doc_isr_executed = false;

/* Private function declaration */
static void led_update(led_state_t led_state);

/*******************************************************************************************************************//**
 * @brief     This function runs the baremetal DOC comparison match process.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Verified successfully.
 * @retval    Any other error codes     Failed to verify.
 **********************************************************************************************************************/
fsp_err_t baremetal_process(void)
{
    volatile uint32_t execution_counter = UINT32_MAX;

    /* Disable Module-stop function */
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
#endif /* BSP_FEATURE_DOC_VERSION */

    /* Set the value which should result in a comparison match */
#if (BSP_FEATURE_DOC_VERSION == 2)
    R_DOC_B->DODSR0 = DOC_DATA_COMPARISON_MATCH;
#else
    R_DOC->DODSR = DOC_DATA_COMPARISON_MATCH;
#endif /* BSP_FEATURE_DOC_VERSION */

    /* Enable the interrupt */
    R_BSP_IrqEnable(DOC_INT_IRQn);

    /* Turn on LED case 1 to indicate an operation in progress */
    led_update(LED_CASE_1);

    /* Pass invalid data */
#if (BSP_FEATURE_DOC_VERSION == 2)
    R_DOC_B->DODIR = DOC_DATA_COMPARISON_MISMATCH;
#else
    R_DOC->DODIR = DOC_DATA_COMPARISON_MISMATCH;
#endif /* BSP_FEATURE_DOC_VERSION */

    execution_counter = IRQ_WAIT_TIME;
    do
    {
        --execution_counter;
    } while ((false == s_b_baremetal_doc_isr_executed) && (0U < execution_counter));

#if (BSP_FEATURE_DOC_VERSION == 2)
    if ((1U == R_DOC_B->DOSR_b.DOPCF) || (true == s_b_baremetal_doc_isr_executed))
#else
    if ((1U == R_DOC->DOCR_b.DOPCF) || (true == s_b_baremetal_doc_isr_executed))
#endif /* BSP_FEATURE_DOC_VERSION */
    {
        return FSP_ERR_INVALID_STATE;
    }

    /* Pass valid data */
#if (BSP_FEATURE_DOC_VERSION == 2)
    R_DOC_B->DODIR = DOC_DATA_COMPARISON_MATCH;
#else
    R_DOC->DODIR = DOC_DATA_COMPARISON_MATCH;
#endif /* BSP_FEATURE_DOC_VERSION */

    /* Wait until the ISR executes and modifies the flag */
    execution_counter = UINT32_MAX;
    do
    {
        --execution_counter;
    } while ((false == s_b_baremetal_doc_isr_executed) && (0U < execution_counter));

    if ((0U >= execution_counter) && (false == s_b_baremetal_doc_isr_executed))
    {
        /* Turn on LED case 3 to indicate an error */
        led_update(LED_CASE_3);
        return FSP_ERR_INVALID_STATE;
    }

    /* Check if DOC operation resulted in a successful comparison */
#if (BSP_FEATURE_DOC_VERSION == 2)
    if (0U == R_DOC_B->DOSR_b.DOPCF)
#else
    if (0U == R_DOC->DOCR_b.DOPCF)
#endif /* BSP_FEATURE_DOC_VERSION */
    {
        /* Turn on LED case 3 to indicate an error */
        led_update(LED_CASE_3);
        return FSP_ERR_INVALID_STATE;
    }

    /* Clear the DOPCF bit and the flag */
#if (BSP_FEATURE_DOC_VERSION == 2)
    R_DOC_B->DOSCR_b.DOPCFCL = 1;
#else
    R_DOC->DOCR |= R_DOC_DOCR_DOPCFCL_Msk;
#endif /* BSP_FEATURE_DOC_VERSION */

    /* Reset the flag */
    s_b_baremetal_doc_isr_executed = false;

    /* Turn on LED case 2 to indicate successful completion */
    led_update(LED_CASE_2);

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief     This function updates the LED state based on the operation status.
 * @param[in] led_state                 Specifies which LED pattern to apply.
 * @retval    None.
 **********************************************************************************************************************/
static void led_update(led_state_t led_state)
{
    if (2U == g_bsp_leds.led_count)
    {
        switch (led_state)
        {
            case LED_CASE_1:
            {
                /* Turn ON LED1 to indicate an operation in progress */
                R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[0], (bsp_io_level_t)LED_ON);
                R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[1], (bsp_io_level_t)LED_OFF);

                /* Delay 500 microseconds */
                R_BSP_SoftwareDelay(WAIT_TIME, BSP_DELAY_UNITS_MICROSECONDS);
                break;
            }
            case LED_CASE_2:
            {
                /* Turn ON LED2 to indicate successful completion */
                R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[0], (bsp_io_level_t)LED_OFF);
                R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[1], (bsp_io_level_t)LED_ON);

                /* Delay 500 microseconds */
                R_BSP_SoftwareDelay(WAIT_TIME, BSP_DELAY_UNITS_MICROSECONDS);
                break;
            }
            case LED_CASE_3:
            {
                /* Turn ON LED1 and LED2 to indicate an error */
                R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[0], (bsp_io_level_t)LED_ON);
                R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[1], (bsp_io_level_t)LED_ON);

                /* Delay 500 microseconds */
                R_BSP_SoftwareDelay(WAIT_TIME, BSP_DELAY_UNITS_MICROSECONDS);
                break;
            }
            default:
            {
                break;
            }
        }
    }
    else if (g_bsp_leds.led_count >= 3U)
    {
        switch (led_state)
        {
            case LED_CASE_1:
            {
                /* Turn ON LED1 to indicate an operation in progress */
                R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[0], (bsp_io_level_t)LED_ON);
                R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[1], (bsp_io_level_t)LED_OFF);
                R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[2], (bsp_io_level_t)LED_OFF);

                /* Delay 500 microseconds */
                R_BSP_SoftwareDelay(WAIT_TIME, BSP_DELAY_UNITS_MICROSECONDS);
                break;
            }
            case LED_CASE_2:
            {
                /* Turn ON LED2 to indicate successful completion */
                R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[0], (bsp_io_level_t)LED_OFF);
                R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[1], (bsp_io_level_t)LED_ON);
                R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[2], (bsp_io_level_t)LED_OFF);

                /* Delay 500 microseconds */
                R_BSP_SoftwareDelay(WAIT_TIME, BSP_DELAY_UNITS_MICROSECONDS);
                break;
            }
            case LED_CASE_3:
            {
                /* Turn ON LED3 to indicate an error */
                R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[0], (bsp_io_level_t)LED_OFF);
                R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[1], (bsp_io_level_t)LED_OFF);
                R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[2], (bsp_io_level_t)LED_ON);

                /* Delay 500 microseconds */
                R_BSP_SoftwareDelay(WAIT_TIME, BSP_DELAY_UNITS_MICROSECONDS);
                break;
            }
            default:
            {
                break;
            }
        }
    }
    else
    {
        /* Do nothing */
    }
}

/*******************************************************************************************************************//**
 * @brief     This function handles the DOC interrupt by setting the executed flag and clearing the IRQ status.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void baremetal_doc_isr(void)
{
    /* Get the IRQ Number */
    IRQn_Type irq = R_FSP_CurrentIrqGet();

    /* Set the flag */
    s_b_baremetal_doc_isr_executed = true;

    /* Clear the IR flag in the ICU */
    R_BSP_IrqStatusClear(irq);
}
