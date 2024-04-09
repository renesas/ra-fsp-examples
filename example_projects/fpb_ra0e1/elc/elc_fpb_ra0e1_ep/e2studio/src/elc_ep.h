/***********************************************************************************************************************
 * File Name    : elc_ep.h
 * Description  : Contains declarations of data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2024 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#ifndef ELC_EP_H_
#define ELC_EP_H_

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#ifdef BOARD_RA0E1_FPB
#define EP_INFO                      "\r\nThis Example Project demonstrates the basic usage of ELC driver."\
                                     "\r\nELC Software Event, TAU0 and TAU1 events are linked using ELC."\
                            		 "\r\nThe start source for TAU1 is ELC Software Event and start source for TAU0 is end of delay counter TAU1."\
                                     "\r\nTAU0 runs in one-shot pulse output mode and TAU1 runs in delay counter mode."\
                                     "\r\nOn giving valid RTT input, an ELC Software Event is generated that triggers LED1 turn on after 1 seconds"\
                                     "\r\nLED1 turn on for 2 seconds until request pulse width has expires.\r\n"

#define OPEN_TIMER_FAIL              "\r\n ** TAU module Open for delay counter mode failed ** \r\n"
#define OPEN_TIMER_PWM_FAIL          "\r\n ** TAU module Open for one-shot pulse output mode failed ** \r\n"
#define ENABLE_TIMER_PULSE_FAIL      "\r\n ** TAU Enable for one-shot pulse output failed ** \r\n"
#define STATUS_DISPLAY               "\r\nEnter any key as RTT input. After 1 seconds LED1 turns on for 2 seconds"
#define ENABLE_TIMER_FAIL            "\r\n ** TAU Enable for delay counter failed ** \r\n"

#else
#define EP_INFO                      "\r\nThis Example Project demonstrates the basic usage of  ELC driver."\
                                     "\r\nELC Software Event, GPT8 and GPT9 events are linked using ELC."\
                                     "\r\nThe start source for GPT8 and GPT9 is ELC Software Event and the stop source for GPT8 is GPT9 counter overflow."\
                                     "\r\nGPT8 runs in PWM mode and GPT9 runs in one-shot mode."\
                                     "\r\nOn giving valid RTT input, an ELC Software Event is generated that triggers LED blinking."\
                                     "\r\nLED stops blinking after 5 seconds when GPT9 expires.\r\n"

#define OPEN_TIMER_FAIL              "\r\n ** GPT module Open for One-shot mode failed ** \r\n"
#define OPEN_TIMER_PWM_FAIL          "\r\n ** GPT module Open for PWM mode failed ** \r\n"
#define ENABLE_TIMER_PULSE_FAIL      "\r\n ** GPT Enable for PWM timer failed ** \r\n"
#define STATUS_DISPLAY               "\r\nEnter any key as RTT input. LED blinks for 5 seconds and stops"
#define ENABLE_TIMER_FAIL            "\r\n ** GPT Enable for One-shot mode failed ** \r\n"

#endif /* BOARD_RA0E1_FPB */

/* MACROs for Null character */
#define NULL_CHAR ('\0')

/*
 * function declarations
 */
void elc_ep_entry (void);

#endif /* ELC_EP_H_ */
