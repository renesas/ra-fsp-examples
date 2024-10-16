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

#ifdef BOARD_RA0E1_FPB
#define USED_TIMER                              "TAU"
#else
#define USED_TIMER                              "GPT"
#endif

/* User command input value */
#define ENABLE_IWDT                             (1u)
#define STOP_IWDT_REFRESH                       (2u)

/* function declarations  */
#ifdef BOARD_RA0E1_FPB
fsp_err_t init_tau_module(void);
void deinit_tau_module(void);
#else
fsp_err_t init_gpt_module(void);
void deinit_gpt_module(void);
#endif
fsp_err_t timer_start(void);


#endif /* TIMER_SETUP_H_ */
