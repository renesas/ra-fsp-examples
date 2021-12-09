/***********************************************************************************************************************
 * File Name    : icu_ep.h
 * Description  : Contains Macros and function declarations.
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
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
#ifndef ICU_EP_H_
#define ICU_EP_H_

/* External IRQ channel for specific boards*/
#if defined (BOARD_RA6M3_EK) || defined (BOARD_RA6M3G_EK)
#define USER_SW_IRQ_NUMBER        (0x0D)
#elif defined (BOARD_RA2A1_EK)
#define USER_SW_IRQ_NUMBER        (0x06)
#elif defined (BOARD_RA6M1_EK)
#define USER_SW_IRQ_NUMBER        (0x08)
#elif defined (BOARD_RA6M2_EK) || defined (BOARD_RA4M1_EK)
#define USER_SW_IRQ_NUMBER        (0x00)
#elif defined (BOARD_RA4W1_EK)
#define USER_SW_IRQ_NUMBER        (0x04)
#elif defined (BOARD_RA6M4_EK) || defined (BOARD_RA4M3_EK) || defined (BOARD_RA4M2_EK) || defined (BOARD_RA6M5_EK)
#define USER_SW_IRQ_NUMBER        (0x0A)
#elif defined (BOARD_RA6T1_RSSK)
#define USER_SW_IRQ_NUMBER        (0x06)
#elif defined (BOARD_RA4E1_FPB)
#define USER_SW_IRQ_NUMBER        (0x01)
#elif defined (BOARD_RA6T2_MCK)
#define USER_SW_IRQ_NUMBER        (0x0C)
#else
#define USER_SW_IRQ_NUMBER        (0x03)      //EK_RA2L1 and EK_RA2E1 board
#endif

#define EP_INFO    "\r\nThis Example Project demonstrates the functionality of ICU driver.\r\n" \
    "On pressing the user push button, an external IRQ is triggered, which toggles on-board LED.\r\n"


/* Function declaration */
fsp_err_t icu_init(void);
fsp_err_t icu_enable(void);
void icu_deinit(void);

#endif /* ICU_EP_H_ */
