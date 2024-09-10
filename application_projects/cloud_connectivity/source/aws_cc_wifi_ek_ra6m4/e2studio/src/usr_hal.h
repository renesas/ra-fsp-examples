/***********************************************************************************************************************
 * File Name    : usr_hal.h
 * Description  : Contains declarations of data structures and functions used in usr_hal.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2015 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/ 

#ifndef USR_HAL_H_
#define USR_HAL_H_

#include "hal_data.h"
#include "mqtt_interface.h"
#include "../../../ra/board/ra6m4_ek/board_leds.h"

#define PBS1_CHANNEL 10
#define PBS2_CHANNEL 11

extern bsp_leds_t g_bsp_leds;

extern fsp_err_t usr_hal_init(void);
extern void pb_callback(external_irq_callback_args_t *p_args);
extern void adc_mcu_temp_callback(adc_callback_args_t *p_args);
extern void g_hb_timer_cb(timer_callback_args_t *p_args);
extern void led_on_off(e_led_type_t ltype, e_led_state_t lled_state);
extern void led_toggle(e_led_type_t ltype);

#endif /* USR_HAL_H_ */
