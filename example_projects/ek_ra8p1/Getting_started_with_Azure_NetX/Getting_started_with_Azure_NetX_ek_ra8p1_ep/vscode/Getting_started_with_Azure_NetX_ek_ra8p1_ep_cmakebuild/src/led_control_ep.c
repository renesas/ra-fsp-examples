/***********************************************************************************************************************
 * File Name    : led_control_ep.c
 * Description  : Contains data structures and functions used in led_control_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "led_control_thread.h"
#include "led_control_ep.h"

/* Global variables */
/* LEDs status */
volatile bsp_io_level_t g_led_1_status;
volatile bsp_io_level_t g_led_2_status;

/* LED control event group flags */
TX_EVENT_FLAGS_GROUP g_led_event_flags;

/*******************************************************************************************************************//**
 * @brief     This function sets the event flag to toggle LED1 when the external interrupt
 *            for switch 1 is triggered.
 * @param[in] p_args                    Pointer to the external interrupt callback arguments.
 * @retval    None.
 **********************************************************************************************************************/
void external_irq_sw1_callback(external_irq_callback_args_t * p_args)
{
    if (g_external_irq_sw1_cfg.channel == p_args->channel)
    {
        /* Set the control event flag for LED1 */
        tx_event_flags_set(&g_led_event_flags, LED_1_TOGGLE_EVENT, TX_OR);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function sets the event flag to toggle LED2 when the external interrupt
 *            for switch 2 is triggered.
 * @param[in] p_args                    Pointer to the external interrupt callback arguments.
 * @retval    None.
 **********************************************************************************************************************/
void external_irq_sw2_callback(external_irq_callback_args_t * p_args)
{
    if (g_external_irq_sw2_cfg.channel == p_args->channel)
    {
        /* Set the control event flag for LED2 */
        tx_event_flags_set(&g_led_event_flags, LED_2_TOGGLE_EVENT, TX_OR);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function opens the external interrupt pins of the switch 1 and the switch 2.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Opened successfully.
 * @retval    Any other error codes     Failed to open.
 **********************************************************************************************************************/
fsp_err_t led_irq_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize the external interrupt pin for LED1 control */
    err = R_ICU_ExternalIrqOpen(&g_external_irq_sw1_ctrl, &g_external_irq_sw1_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Initialize the external interrupt pin for LED2 control */
    err = R_ICU_ExternalIrqOpen(&g_external_irq_sw2_ctrl, &g_external_irq_sw2_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function creates the event flag group for the LED control.
 * @param[in] None.
 * @retval    TX_SUCCESS                Created successfully.
 * @retval    Any other error codes     Failed to create.
 **********************************************************************************************************************/
UINT led_event_flags_create(void)
{
    return tx_event_flags_create(&g_led_event_flags, "LED Control Events");
}

/*******************************************************************************************************************//**
 * @brief     This function enables the external interrupt pins of the switch 1 and the switch 2 to generate
 *            interrupts.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Enabled successfully.
 * @retval    Any other error codes     Failed to enable.
 **********************************************************************************************************************/
fsp_err_t led_irq_enable(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Enable external pins to generate interrupts for SW1 */
    err = R_ICU_ExternalIrqEnable(&g_external_irq_sw1_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Enable external pins to generate interrupts for SW2 */
    err = R_ICU_ExternalIrqEnable(&g_external_irq_sw2_ctrl);

    return err;
}

/*******************************************************************************************************************//**
 * @brief      This function waits for any LED control event.
 * @param[out] p_events                 Pointer to store the received event flags.
 * @retval     TX_SUCCESS               Event flags received successfully.
 * @retval     Any other error codes    Failed to receive the event flags.
 **********************************************************************************************************************/
UINT led_event_get(ULONG * const p_events)
{
    return tx_event_flags_get(&g_led_event_flags, LED_ANY_EVENT, TX_OR_CLEAR, p_events, TX_WAIT_FOREVER);
}

/*******************************************************************************************************************//**
 * @brief     This function toggles the state of the LEDs according to the received LED control events.
 * @param[in] events                    LED control event flags.
 * @retval    None.
 **********************************************************************************************************************/
void led_toggle(ULONG events)
{
    /* Toggle the state of LED1 */
    if (LED_1_TOGGLE_EVENT == (events & LED_1_TOGGLE_EVENT))
    {
        g_led_1_status ^= BSP_IO_LEVEL_HIGH;
        R_BSP_PinAccessEnable();
        R_BSP_PinWrite(g_bsp_leds.p_leds[BSP_LED_LED1], g_led_1_status);
        R_BSP_PinAccessDisable();
    }

    /* Toggle the state of LED2 */
    if (LED_2_TOGGLE_EVENT == (events & LED_2_TOGGLE_EVENT))
    {
        g_led_2_status ^= BSP_IO_LEVEL_HIGH;
        R_BSP_PinAccessEnable();
        R_BSP_PinWrite(g_bsp_leds.p_leds[BSP_LED_LED2], g_led_2_status);
        R_BSP_PinAccessDisable();
    }
}
