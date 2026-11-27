/***********************************************************************************************************************
 * File Name    : clock_output_ep.h
 * Description  : Contains declarations of data structures and functions used in clock_output_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2024 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef CLOCK_OUTPUT_EP_H_
#define CLOCK_OUTPUT_EP_H_

#include "hal_data.h"

/* Key code for writing PRCR register */
#define PRV_PRCR_KEY                        (0xA500U)
#define PRV_PRCR_UNLOCK                     ((PRV_PRCR_KEY) | 0x3U)
#define PRV_PRCR_LOCK                       ((PRV_PRCR_KEY) | 0x0U)

/* Macros for the boards that do not have clock out select definition */
#if defined (BOARD_RA8M1_EK) || defined (BOARD_RA8D1_EK)
#define R_SYSTEM_CKOCR_CKOSEL_POS           (0UL)       /* CKOSEL (Bit 0) */
#define R_SYSTEM_CKOCR_CKOSEL_MSK           (0x7UL)     /* CKOSEL (Bitfield-Mask: 0x07) */
#endif /* BOARD_RA8M1_EK, BOARD_RA8D1_EK */

#if !(defined (BOARD_RA4E2_FPB))
#if (GPT_MEASURE_CLKOUT)
/* Macro for Main clock output frequency division ratio */
#define MAIN_CLOCK_DIV                      (BSP_CLOCKS_SYS_CLOCK_DIV_32)
#else
#if defined (BOARD_RA2E1_EK) || defined (BOARD_RA2L1_EK)
#define MAIN_CLOCK_DIV                      (BSP_CLOCKS_SYS_CLOCK_DIV_2)
#else
#define MAIN_CLOCK_DIV                      (BSP_CLOCKS_SYS_CLOCK_DIV_1)
#endif /* BOARD_RA2E1_EK, BOARD_RA2L1_EK */
#endif /* GPT_MEASURE_CLKOUT */
#endif /* BOARD_RA4E2_FPB */

/* Macros for menu option */
#if !(defined (BOARD_RA4E2_FPB))
#define START_MAIN_CLOCK                    (1U)
#define START_SUB_CLOCK                     (2U)
#define STOP_ALL_CLOCKS                     (3U)
#else
#define START_SUB_CLOCK                     (1U)
#define STOP_ALL_CLOCKS                     (2U)
#endif /* BOARD_RA4E2_FPB */

/* Macro for clock output pin */
#if defined (BOARD_RA2A1_EK)
#define CLKOUT_PIN                          (BSP_IO_PORT_01_PIN_10)
#elif defined (BOARD_RA2A2_EK)
#define CLKOUT_PIN                          (BSP_IO_PORT_02_PIN_10)
#elif defined (BOARD_RA2E1_EK) || defined (BOARD_RA2L1_EK) || defined (BOARD_RA4E2_EK) || defined (BOARD_RA6E2_EK) ||\
      defined (BOARD_RA4E2_FPB)
#define CLKOUT_PIN                          (BSP_IO_PORT_01_PIN_09)
#elif defined (BOARD_RA8D1_EK)
#define CLKOUT_PIN                          (BSP_IO_PORT_09_PIN_13)
#elif defined (BOARD_RA8P1_EK) || defined (BOARD_RA8D2_EK) || defined (BOARD_RA8M2_EK)
#define CLKOUT_PIN                          (BSP_IO_PORT_06_PIN_11)
#else
#define CLKOUT_PIN                          (BSP_IO_PORT_02_PIN_05)
#endif /* CLKOUT pin */

/* Public function declarations */
#if !(defined (BOARD_RA4E2_FPB))
void start_main_clock(void);
#endif /* BOARD_RA4E2_FPB */
void start_sub_clock(void);
void stop_all_clocks(void);

#endif /* CLOCK_OUTPUT_EP_H_ */
