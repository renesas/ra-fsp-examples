/***********************************************************************************************************************
 * File Name    : elc_ep.h
 * Description  : Contains declarations of data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef ELC_EP_H_
#define ELC_EP_H_

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#if (BSP_PERIPHERAL_TAU_PRESENT)
#define EP_INFO                 "\r\nThis example project demonstrates the basic usage of ELC driver."\
                                "\r\nELC Software Event, TAU0 and TAU1 events are linked using ELC."\
                                "\r\nThe start source for TAU1 is ELC Software Event and start source"\
                                "\r\nfor TAU0 is end of counter TAU1. TAU0 runs in One-Shot pulse"\
                                "\r\noutput mode, and TAU1 runs in delay counter mode. On giving valid RTT Viewer"\
                                "\r\ninput, an ELC Software Event is generated that turns on LED1 after"\
                                "\r\n1 second. LED1 turns ON for 2 seconds until request pulse width expires.\r\n"

#define OPEN_TIMER_FAIL         "\r\n** TAU module Open for delay counter mode failed **\r\n"
#define OPEN_TIMER_PWM_FAIL     "\r\n** TAU module Open for One-Shot pulse output mode failed **\r\n"
#define ENABLE_TIMER_PULSE_FAIL "\r\n** TAU Enable for One-Shot pulse output failed **\r\n"
#define STATUS_DISPLAY          "\r\nEnter any key as RTT Viewer input. After 1 second LED1 turns ON for 2 seconds"
#define ENABLE_TIMER_FAIL       "\r\n** TAU Enable for delay counter failed **\r\n"

#else
#define EP_INFO                 "\r\nThis example project demonstrates the basic usage of ELC driver."\
		                        "\r\nELC Software Event, GPT7 and GPT0 events are linked using ELC."\
                                "\r\nThe start source for GPT7 and GPT0 is ELC Software Event, and the"\
                                "\r\nstop source for GPT7 is GPT0 counter overflow."\
                                "\r\nGPT7 runs in PWM mode, and GPT0 runs in One-Shot mode."\
                                "\r\nOn giving valid RTT Viewer input, an ELC Software Event is generated that"\
                                "\r\ntriggers LED blinking. LED stops blinking after 5 seconds when GPT0 expires.\r\n"

#define OPEN_TIMER_FAIL         "\r\n** GPT module Open for One-Shot mode failed **\r\n"
#define OPEN_TIMER_PWM_FAIL     "\r\n** GPT module Open for PWM mode failed **\r\n"
#define ENABLE_TIMER_PULSE_FAIL "\r\n** GPT Enable for PWM timer failed **\r\n"
#define STATUS_DISPLAY          "\r\nEnter any key as RTT Viewer input. LED blinks for 5 seconds and stops"
#define ENABLE_TIMER_FAIL       "\r\n** GPT Enable for One-Shot mode failed **\r\n"

#endif /* BSP_PERIPHERAL_TAU_PRESENT */

/* Macro for NULL character */
#define NULL_CHAR ('\0')

/* Function declaration */
void elc_ep_entry (void);

#endif /* ELC_EP_H_ */
