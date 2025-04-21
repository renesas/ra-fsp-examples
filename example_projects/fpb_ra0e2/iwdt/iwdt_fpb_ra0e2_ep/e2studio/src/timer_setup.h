/***********************************************************************************************************************
 * File Name    : timer_setup.h
 * Description  : Contains data structures and functions used in timer_setup.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
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

#if (BSP_PERIPHERAL_TAU_PRESENT == 1)
#define USED_TIMER                              "TAU"
#else
#define USED_TIMER                              "GPT"
#endif

/* Macro to define IWDT overflow time for specific boards */
#if defined(BOARD_RA8M1_EK) || defined(BOARD_RA8D1_EK) || defined(BOARD_RA8T1_MCK) ||\
    defined(BOARD_RA0E1_FPB) || defined(BOARD_RA0E2_FPB) || defined(BOARD_RA8E1_FPB)
#define IWDT_OVERFLOW                           "16"
#else
#define IWDT_OVERFLOW                           "17"
#endif

/* User command input value */
#define ENABLE_IWDT                             (1u)
#define STOP_IWDT_REFRESH                       (2u)

/* Function declarations  */
#if (BSP_PERIPHERAL_TAU_PRESENT == 1)
fsp_err_t init_tau_module(void);
void deinit_tau_module(void);
#else
fsp_err_t init_gpt_module(void);
void deinit_gpt_module(void);
#endif
fsp_err_t timer_start(void);

#endif /* TIMER_SETUP_H_ */
