/***********************************************************************************************************************
 * File Name    : timer_setup.h
 * Description  : Contains declarations of data structures and functions used in timer_setup.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef TIMER_SETUP_H_
#define TIMER_SETUP_H_

#include "hal_data.h"

/* Number of timer callback before printing IWDT refresh status */
#define IWDT_REFRESH_COUNTER_VALUE              (2U)

/* Macro to define the timer peripheral used in this application */
#if (BSP_PERIPHERAL_TAU_PRESENT)
#define USED_TIMER                              "TAU"
#define R_TIMER_OPEN(ctrl, cfg)                 (R_TAU_Open((ctrl), (cfg)))
#define R_TIMER_START(ctrl)                     (R_TAU_Start(ctrl))
#define R_TIMER_STOP(ctrl)                      (R_TAU_Stop(ctrl))
#define R_TIMER_CLOSE(ctrl)                     (R_TAU_Close(ctrl))
#else
#define USED_TIMER                              "GPT"
#define R_TIMER_OPEN(ctrl, cfg)                 (R_GPT_Open((ctrl), (cfg)))
#define R_TIMER_START(ctrl)                     (R_GPT_Start(ctrl))
#define R_TIMER_STOP(ctrl)                      (R_GPT_Stop(ctrl))
#define R_TIMER_CLOSE(ctrl)                     (R_GPT_Close(ctrl))
#endif /* BSP_PERIPHERAL_TAU_PRESENT */

/* Macro for IWDT underflow time based on specific board */
#if defined(BOARD_RA8M1_EK) || defined(BOARD_RA8D1_EK) || defined(BOARD_RA8T1_MCK) || defined(BOARD_RA0E1_FPB) ||\
    defined(BOARD_RA0E2_FPB) || defined(BOARD_RA8E1_FPB) || defined(BOARD_RA8E2_EK) || defined(BOARD_RA8P1_EK) ||\
    defined(BOARD_RA0L1_FPB) || defined(BOARD_RA8T2_MCK) || defined(BOARD_RA8D2_EK) || defined(BOARD_RA8M2_EK) ||\
    defined(BOARD_RA0E3_FPB)
#define IWDT_UNDERFLOW                          "16"
#else
#define IWDT_UNDERFLOW                          "17"
#endif

/* The user command input value */
#define ENABLE_IWDT                             (1U)
#define STOP_IWDT_REFRESH                       (2U)

/* Macros for LED state based on specific board */
#if defined(BOARD_RA4W1_EK) || defined(BOARD_RA6T1_RSSK) || defined(BOARD_RA4T1_MCK) ||\
    defined(BOARD_RA6T2_MCK) || defined(BOARD_RA6T3_MCK) || defined(BOARD_RA8T1_MCK) ||\
    defined(BOARD_RA8T2_MCK)
#define LED_ON                                  (BSP_IO_LEVEL_LOW)
#define LED_OFF                                 (BSP_IO_LEVEL_HIGH)
#else
#define LED_ON                                  (BSP_IO_LEVEL_HIGH)
#define LED_OFF                                 (BSP_IO_LEVEL_LOW)
#endif

/* Extern variable — defined in timer_setup.c */
extern volatile uint32_t g_timer_underflow_counter;

/* Function declarations */
fsp_err_t init_timer_module(void);
fsp_err_t timer_start(void);
fsp_err_t timer_stop(void);
void deinit_timer_module(void);

#endif /* TIMER_SETUP_H_ */
