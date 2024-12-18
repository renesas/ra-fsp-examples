/***********************************************************************************************************************
* File Name    : led_control_thread_entry.c
* Description  : Contains data structures and functions used in the LED control thread
**********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "led_control_thread.h"
#include "common_app.h"

/***********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/
extern const bsp_leds_t g_bsp_leds;

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Global Variables
 **********************************************************************************************************************/
/* LEDs status */
volatile bsp_io_level_t g_led_1_status;
volatile bsp_io_level_t g_led_2_status;

/* LED control event group flags */
TX_EVENT_FLAGS_GROUP g_led_event_flags;

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief Callback function for external interrupt from switch 1 (SW1).
 *
 * This function sets the event flag to toggle LED 1 when the external interrupt
 * for switch 1 is triggered.
 *
 * @param[in] p_args Pointer to the external interrupt callback arguments.
 **********************************************************************************************************************/
void external_irq_sw1_callback(external_irq_callback_args_t *p_args)
{
    if (g_external_irq_sw1_cfg.channel == p_args->channel)
    {
        /* Set the control event flag for LED 1. */
        tx_event_flags_set(&g_led_event_flags, LED_1_TOGGLE_EVENT, TX_OR);
    }
}

/*******************************************************************************************************************//**
 * @brief Callback function for external interrupt from switch 2 (SW2).
 *
 * This function sets the event flag to toggle LED 2 when the external interrupt
 * for switch 2 is triggered.
 *
 * @param[in] p_args Pointer to the external interrupt callback arguments.
 **********************************************************************************************************************/
void external_irq_sw2_callback(external_irq_callback_args_t *p_args)
{
    if (g_external_irq_sw2_cfg.channel == p_args->channel)
    {
        /* Set the control event flag for LED 2. */
        tx_event_flags_set(&g_led_event_flags, LED_2_TOGGLE_EVENT, TX_OR);
    }
}

/*******************************************************************************************************************//**
 * @brief Entry function for the LED control thread.
 *
 * This function initializes the external interrupt pins, creates event flags
 * for LED control, and toggles the LED states based on the events triggered
 * by the switches.
 **********************************************************************************************************************/
void led_control_thread_entry(void)
{
    UINT status = TX_SUCCESS;
    ULONG events;

    /* Initialize the external interrupt pin for LED 1 control. */
    status = (UINT)R_ICU_ExternalIrqOpen(&g_external_irq_sw1_ctrl, &g_external_irq_sw1_cfg);
    ERROR_TRAP(TX_SUCCESS != status, status, "R_ICU_ExternalIrqOpen for the SW1 failed\r\n");

    /* Initialize the external interrupt pin for LED 2 control. */
    status = (UINT)R_ICU_ExternalIrqOpen(&g_external_irq_sw2_ctrl, &g_external_irq_sw2_cfg);
    ERROR_TRAP(TX_SUCCESS != status, status, "R_ICU_ExternalIrqOpen for the SW2 failed\r\n");

    /* Create an event flag group for LED control. */
    status = tx_event_flags_create(&g_led_event_flags, "LED Control Events");
    ERROR_TRAP(TX_SUCCESS != status, status, "tx_event_flags_create for the LED control failed\r\n");

    /* Enable external pins to generate interrupts for SW1. */
    status = (UINT)R_ICU_ExternalIrqEnable(&g_external_irq_sw1_ctrl);
    ERROR_TRAP(TX_SUCCESS != status, status, "R_ICU_ExternalIrqEnable for the SW1 failed\r\n");

    /* Enable external pins to generate interrupts for SW2. */
    status = (UINT)R_ICU_ExternalIrqEnable(&g_external_irq_sw2_ctrl);
    ERROR_TRAP(TX_SUCCESS != status, status, "R_ICU_ExternalIrqEnable for the SW2 failed\r\n");

    while (true)
    {
        /* Wait for any LED control events. */
        status = tx_event_flags_get(&g_led_event_flags, LED_ANY_EVENT, TX_OR_CLEAR, &events, TX_WAIT_FOREVER);
        ERROR_TRAP(TX_SUCCESS != status, status, "tx_event_flags_get for the LED control event failed\r\n");

        /* Toggle the state of LED 1. */
        if (LED_1_TOGGLE_EVENT == (events & LED_1_TOGGLE_EVENT))
        {
            g_led_1_status ^= BSP_IO_LEVEL_HIGH;
            R_BSP_PinAccessEnable();
            R_BSP_PinWrite(g_bsp_leds.p_leds[BSP_LED_LED1], g_led_1_status);
            R_BSP_PinAccessDisable();
        }

        /* Toggle the state of LED 2. */
        if (LED_2_TOGGLE_EVENT == (events & LED_2_TOGGLE_EVENT))
        {
            g_led_2_status ^= BSP_IO_LEVEL_HIGH;
            R_BSP_PinAccessEnable();
            R_BSP_PinWrite(g_bsp_leds.p_leds[BSP_LED_LED2], g_led_2_status);
            R_BSP_PinAccessDisable();
        }

        tx_thread_sleep(LED_CONTROL_THREAD_SLEEP_TICK);
    }
}
