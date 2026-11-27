/***********************************************************************************************************************
 * File Name    : timer_setup.h
 * Description  : Contains data structures and functions used in timer_setup.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef TIMER_SETUP_H_
#define TIMER_SETUP_H_

#include "hal_data.h"

/* Macro for LED state */
#if defined (BOARD_RA4W1_EK) || defined (BOARD_RA6T1_RSSK) || defined (BOARD_RA6T2_MCK) || defined (BOARD_RA6T3_MCK)\
    || defined (BOARD_RA4T1_MCK) || defined (BOARD_RA8T1_MCK) || defined (BOARD_RA8T2_MCK)
#define LED_ON   (BSP_IO_LEVEL_LOW)
#define LED_OFF  (BSP_IO_LEVEL_HIGH)
#else
#define LED_ON   (BSP_IO_LEVEL_HIGH)
#define LED_OFF  (BSP_IO_LEVEL_LOW)
#endif

/* Function declarations */
fsp_err_t init_gpt_module(void);
fsp_err_t timer_start(void);
fsp_err_t timer_stop(void);
void deinit_gpt_module(void);

#endif /* TIMER_SETUP_H_ */
