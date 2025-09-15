/***********************************************************************************************************************
 * File Name    : timer_setup.h
 * Description  : Contains data structures and functions used in timer_setup.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef TIMER_SETUP_H_
#define TIMER_SETUP_H_

/* IWDT/NMI detect reset value */
#define SYSTEM_RSTSR1_IWDTRF_DETECT_RESET       (1u)
#define SYSTEM_RSTSR1_SWRF_DETECT_RESET         (1u)

/* Number of counts for printing IWDT Refresh status */
#define IWDT_REFRESH_COUNTER_VALUE              (2u)

#define BUFFER_SIZE                             (16u)

/* Macro to define the timer peripheral used in this application */
#if (BSP_PERIPHERAL_TAU_PRESENT)
#define USED_TIMER                              "TAU"
#else
#define USED_TIMER                              "GPT"
#endif /* BSP_PERIPHERAL_TAU_PRESENT */

/* Macro to define IWDT underflow time for specific boards */
#if defined(BOARD_RA8M1_EK) || defined(BOARD_RA8D1_EK) || defined(BOARD_RA8T1_MCK) || defined(BOARD_RA0E1_FPB) ||\
    defined(BOARD_RA0E2_FPB) || defined(BOARD_RA8E1_FPB) || defined(BOARD_RA8E2_EK) || defined(BOARD_RA8P1_EK) ||\
    defined(BOARD_RA0L1_FPB)
#define IWDT_UNDERFLOW                          "16"
#else
#define IWDT_UNDERFLOW                          "17"
#endif

/* The user command input value */
#define ENABLE_IWDT                             (1u)
#define STOP_IWDT_REFRESH                       (2u)

/* Function declarations */
#if (BSP_PERIPHERAL_TAU_PRESENT)
fsp_err_t init_tau_module(void);
void deinit_tau_module(void);
#else
fsp_err_t init_gpt_module(void);
void deinit_gpt_module(void);
#endif /* BSP_PERIPHERAL_TAU_PRESENT */
fsp_err_t timer_start(void);

#endif /* TIMER_SETUP_H_ */
