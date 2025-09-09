/***********************************************************************************************************************
 * File Name    : lpm_app.h
 * Description  : Contains declarations of data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/**********************************************************************************************************************
* Copyright (c) 2019 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/

#ifndef LPM_APP_H_
#define LPM_APP_H_

/* generic headers */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hal_data.h"
#include "bsp_pin_cfg.h"
#include "r_ioport.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define LED_NO_0                  (0x00)
#define CLOCK_START	  		      (0U)
#define CLOCK_STOP	              (1U)
#define USER_SW_DEBOUNCE_LIMIT    (20U)

/*
 * MACROs that define user push-button pins, start source and stop sources for all boards
 * External IRQ channel of user push button for specific boards
 */
#if defined (BOARD_RA6M3_EK) || defined (BOARD_RA6M3G_EK)
#define USER_SW_IRQ_NUMBER        (0x0D)
#elif defined (BOARD_RA2A1_EK)
#define USER_SW_IRQ_NUMBER        (0x06)
#elif defined (BOARD_RA6M2_EK) || defined (BOARD_RA6M1_EK) || defined (BOARD_RA4M1_EK)
#define USER_SW_IRQ_NUMBER        (0x00)
#elif defined (BOARD_RA2L1_EK)
#define USER_SW_IRQ_NUMBER        (0x03)
#endif

#define APP_ERR_TRAP(err)        if(err) { __asm("BKPT #0\n");} /* trap upon the error  */

/*
 * Low Power Mode Definitions for LPM ep
 * Since there are no Normal mode definition in LPM driver, use this enum to keep LPM app state including:
 * Sleep, SW Standby, SW Standby with Snooze enabled, Deep SW Standby, Normal.
 */
typedef enum e_app_lpm_state
{
    APP_LPM_SW_STANDBY_STATE = 0,        ///< SW Standby mode
    APP_LPM_DEEP_SW_STANDBY_STATE,       ///< Deep SW Standby mode
    APP_LPM_NORMAL_STATE                 ///< Normal mode
} app_lpm_states_t;

/* Function declarations */
fsp_err_t user_sw_init              (void);
fsp_err_t user_sw_enable            (void);
fsp_err_t user_sw_deinit            (void);

fsp_err_t user_led_toggle           (bsp_leds_t leds);
fsp_err_t user_led_on				(bsp_leds_t leds);

fsp_err_t agt1_stop				    (void);
fsp_err_t agt_timer_init			(void);
fsp_err_t agt_timer_start			(void);
fsp_err_t agt_timer_stop			(void);

fsp_err_t gpt_init_start			(void);

fsp_err_t user_clocks_set			(void);
fsp_err_t lpm_mode_enter			(app_lpm_states_t lpm_mode);

#endif /* LPM_APP_H_ */
