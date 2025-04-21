/***********************************************************************************************************************
 * File Name    : elc_ep.h
 * Description  : Contains declarations of data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef ELC_EP_H_
#define ELC_EP_H_

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#if BSP_PERIPHERAL_TAU_PRESENT
#define EP_INFO                 "\r\nThis Example Project demonstrates the basic usage of ELC driver."\
                                "\r\nELC Software Event, TAU0 and TAU1 events are linked using ELC."\
                            	"\r\nThe start source for TAU1 is ELC Software Event and start source "\
                            	"\r\nfor TAU0 is end of delay counter TAU1. TAU0 runs in one-shot pulse"\
                                "\r\noutput mode and TAU1 runs in delay counter mode. On giving valid RTT input,"\
                                "\r\nan ELC Software Event is generated that triggers LED1 turn on after 1 seconds"\
                                "\r\nLED1 turn on for 2 seconds until request pulse width has expires.\r\n"

#define OPEN_TIMER_FAIL         "\r\n ** TAU module Open for delay counter mode failed ** \r\n"
#define OPEN_TIMER_PWM_FAIL     "\r\n ** TAU module Open for one-shot pulse output mode failed ** \r\n"
#define ENABLE_TIMER_PULSE_FAIL "\r\n ** TAU Enable for one-shot pulse output failed ** \r\n"
#define STATUS_DISPLAY          "\r\nEnter any key as RTT input. After 1 seconds LED1 turns on for 2 seconds"
#define ENABLE_TIMER_FAIL       "\r\n ** TAU Enable for delay counter failed ** \r\n"

#else
#define EP_INFO                 "\r\nThis Example Project demonstrates the basic usage of  ELC driver."\
                                "\r\nELC Software Event, GPT8 and GPT9 events are linked using ELC."\
                                "\r\nThe start source for GPT8 and GPT9 is ELC Software Event and the "\
                                "\r\nstop source for GPT8 is GPT9 counter overflow."\
                                "\r\nGPT8 runs in PWM mode and GPT9 runs in one-shot mode."\
                                "\r\nOn giving valid RTT input, an ELC Software Event is generated that triggers"\
                                "\r\nLED blinking. LED stops blinking after 5 seconds when GPT9 expires.\r\n"

#define OPEN_TIMER_FAIL         "\r\n ** GPT module Open for One-shot mode failed ** \r\n"
#define OPEN_TIMER_PWM_FAIL     "\r\n ** GPT module Open for PWM mode failed ** \r\n"
#define ENABLE_TIMER_PULSE_FAIL "\r\n ** GPT Enable for PWM timer failed ** \r\n"
#define STATUS_DISPLAY          "\r\nEnter any key as RTT input. LED blinks for 5 seconds and stops"
#define ENABLE_TIMER_FAIL       "\r\n ** GPT Enable for One-shot mode failed ** \r\n"

#endif /* BSP_PERIPHERAL_TAU_PRESENT */

/* MACROs for Null character */
#define NULL_CHAR ('\0')

/*
 * Function declarations
 */
void elc_ep_entry (void);

#endif /* ELC_EP_H_ */
