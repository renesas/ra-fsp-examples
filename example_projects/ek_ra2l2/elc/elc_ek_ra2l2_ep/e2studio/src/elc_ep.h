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
#if (BSP_PERIPHERAL_TAU_PRESENT)
#define EP_INFO                "\r\nThis Example Project demonstrates the basic usage of ELC driver."\
                               "\r\nELC Software Event, TAU0 and TAU1 events are linked using ELC."\
                               "\r\nThe start source for TAU1 is ELC Software Event and start source "\
                               "\r\nfor TAU0 is end of delay counter TAU1. TAU0 runs in One-shot Pulse"\
                               "\r\noutput mode and TAU1 runs in delay counter mode. On giving valid RTT Viewer input,"\
                               "\r\nan ELC Software Event is generated that triggers LED1 turns ON after 1 seconds"\
                               "\r\nLED1 turns ON for 2 seconds until request pulse width has expires.\r\n"

#define OPEN_TIMER_FAIL         "\r\n ** TAU module Open for delay Counter mode failed ** \r\n"
#define OPEN_TIMER_PWM_FAIL     "\r\n ** TAU module Open for One-shot Pulse output mode failed ** \r\n"
#define ENABLE_TIMER_PULSE_FAIL "\r\n ** TAU Enable for One-shot Pulse output failed ** \r\n"
#define STATUS_DISPLAY          "\r\nEnter any key as RTT Viewer input. After 1 seconds LED1 turns ON for 2 seconds"
#define ENABLE_TIMER_FAIL       "\r\n ** TAU Enable for delay Counter failed ** \r\n"

#else
#define EP_INFO                "\r\nThis Example Project demonstrates the basic usage of ELC driver."\
                               "\r\nELC Software Event, GPT5 and GPT0 events are linked using ELC."\
                               "\r\nThe start source for GPT5 and GPT0 is ELC Software Event and the "\
                               "\r\nstop source for GPT5 is GPT0 counter overflow."\
                               "\r\nGPT5 runs in PWM mode and GPT0 runs in One-shot mode."\
                               "\r\nOn giving valid RTT Viewer input, an ELC Software Event is generated that triggers"\
                               "\r\nLED blinking. LED stops blinking after 5 seconds when GPT0 expires.\r\n"

#define OPEN_TIMER_FAIL         "\r\n ** GPT module Open for One-shot mode failed ** \r\n"
#define OPEN_TIMER_PWM_FAIL     "\r\n ** GPT module Open for PWM mode failed ** \r\n"
#define ENABLE_TIMER_PULSE_FAIL "\r\n ** GPT Enable for PWM timer failed ** \r\n"
#define STATUS_DISPLAY          "\r\nEnter any key as RTT Viewer input. LED blinks for 5 seconds and stops"
#define ENABLE_TIMER_FAIL       "\r\n ** GPT Enable for One-shot mode failed ** \r\n"

#endif /* BSP_PERIPHERAL_TAU_PRESENT */

/* Macro for NULL character */
#define NULL_CHAR ('\0')

/* Function declaration */
void elc_ep_entry (void);

#endif /* ELC_EP_H_ */
