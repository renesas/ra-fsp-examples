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
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#ifndef ELC_EP_H_
#define ELC_EP_H_

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define EP_INFO                "\r\nThis Example Project demonstrates the basic usage of  ELC driver."\
                               "\r\nIOPORT, GPT0 and GPT1 events are linked using ELC."\
                               "\r\nThe start source for GPT0 and GPT1 is IOPORT event and the stop source for GPT1 is GPT0 counter overflow."\
                               "\r\nGPT1 runs in PWM mode and GPT0 runs in one-shot mode."\
                               "\r\nOn pressing the user push button, an IOPORT event is triggered which starts blinking the LED."\
                               "\r\nLED stops blinking after 5 sec when GPT0 expires.\r\n"

/* PFS register setting to detect falling edge on user push button pins */
#define IOPORT_FALLING_EDGE_DETECT (0x00002002U)

/* MACROs that define user push-button pins for all boards */
#if defined(BOARD_RA2A1_EK)
#define USER_PUSH_BUTTON_IOPORT (BSP_IO_PORT_02_PIN_06)
#elif defined(BOARD_RA4M1_EK)
#define USER_PUSH_BUTTON_IOPORT (BSP_IO_PORT_01_PIN_05)
#elif defined(BOARD_RA6M1_EK)
#define USER_PUSH_BUTTON_IOPORT (BSP_IO_PORT_04_PIN_15)
#elif defined(BOARD_RA6M2_EK)
#define USER_PUSH_BUTTON_IOPORT (BSP_IO_PORT_01_PIN_05)
#elif defined(BOARD_RA4W1_EK)
#define USER_PUSH_BUTTON_IOPORT (BSP_IO_PORT_04_PIN_02)
#else
#define USER_PUSH_BUTTON_IOPORT (BSP_IO_PORT_01_PIN_01)
#endif

#endif /* ELC_EP_H_ */
