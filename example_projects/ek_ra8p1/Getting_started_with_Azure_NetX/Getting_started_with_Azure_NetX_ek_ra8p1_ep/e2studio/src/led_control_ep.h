/***********************************************************************************************************************
 * File Name    : led_control_ep.h
 * Description  : Contains macros, data structures and functions used in the led_control_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef LED_CONTROL_EP_H_
#define LED_CONTROL_EP_H_

#include "terminal.h"

/* Macro for the sleep time of the LED Control Thread, in ticks */
#define LED_CONTROL_THREAD_SLEEP_TICK   (1U)

/* Enumeration for LED control events */
typedef enum e_led_event
{
    LED_1_TOGGLE_EVENT = 1U << 0,
    LED_2_TOGGLE_EVENT = 1U << 1,
    LED_ANY_EVENT      = 0x03U,
} led_event_t;

/* Extern variables - Defined in led_control_ep.c */
extern TX_EVENT_FLAGS_GROUP g_led_event_flags;
extern volatile bsp_io_level_t g_led_1_status;
extern volatile bsp_io_level_t g_led_2_status;

/* Extern variable to use on-board LEDs */
extern const bsp_leds_t g_bsp_leds;

/* Function declarations */
fsp_err_t led_irq_init(void);
UINT led_event_flags_create(void);
fsp_err_t led_irq_enable(void);
UINT led_event_get(ULONG * const p_events);
void led_toggle(ULONG events);

#endif /* LED_CONTROL_EP_H_ */
