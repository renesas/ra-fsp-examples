/***********************************************************************************************************************
 * File Name    : icu_ep.h
 * Description  : Contains Macros and function declarations.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#ifndef ICU_EP_H_
#define ICU_EP_H_

/* External IRQ channel for specific boards*/
#if defined (BOARD_RA6M3_EK) || defined (BOARD_RA6M3G_EK) || defined (BOARD_RA8M1_EK) || defined (BOARD_RA8D1_EK)
#define USER_SW_IRQ_NUMBER        (0x0D)
#elif defined (BOARD_RA2A1_EK)
#define USER_SW_IRQ_NUMBER        (0x06)
#elif defined (BOARD_RA6M1_EK)
#define USER_SW_IRQ_NUMBER        (0x08)
#elif defined (BOARD_RA6M2_EK) || defined (BOARD_RA4M1_EK)|| defined (BOARD_RA2A2_EK) || defined (BOARD_RA0E1_FPB)
#define USER_SW_IRQ_NUMBER        (0x00)
#elif defined (BOARD_RA4W1_EK)
#define USER_SW_IRQ_NUMBER        (0x04)
#elif defined (BOARD_RA6M4_EK) || defined (BOARD_RA4M3_EK) || defined (BOARD_RA4M2_EK) || defined (BOARD_RA6M5_EK)
#define USER_SW_IRQ_NUMBER        (0x0A)
#elif defined (BOARD_RA6T1_RSSK)
#define USER_SW_IRQ_NUMBER        (0x06)
#else
#define USER_SW_IRQ_NUMBER        (0x03)      //EK_RA2L1, EK_RA2E1 and  MCK_RA8T1 board
#endif

#define EP_INFO    "\r\nThis Example Project demonstrates the functionality of ICU driver.\r\n" \
    "On pressing the user push button, an external IRQ is triggered, which toggles on-board LED.\r\n"


/* Function declaration */
fsp_err_t icu_init(void);
fsp_err_t icu_enable(void);
void icu_deinit(void);

#endif /* ICU_EP_H_ */
