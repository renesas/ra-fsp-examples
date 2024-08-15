/***********************************************************************************************************************
 * File Name    : elc_ep.h
 * Description  : Contains declarations of data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef ELC_EP_H_
#define ELC_EP_H_

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define EP_INFO                "\r\nThis Example Project demonstrates the basic usage of  ELC driver."\
                               "\r\nELC Software Event, GPT0 and GPT1 events are linked using ELC."\
                               "\r\nThe start source for GPT0 and GPT1 is ELC Software Event and the stop source for GPT1 is GPT0 counter overflow."\
                               "\r\nGPT1 runs in PWM mode and GPT0 runs in one-shot mode."\
                               "\r\nOn giving valid RTT input, an ELC Software Event is generated that triggers LED blinking."\
                               "\r\nLED stops blinking after 5 seconds when GPT0 expires.\r\n"

/* MACROs for Null character */
#define NULL_CHAR ('\0')

#endif /* ELC_EP_H_ */
