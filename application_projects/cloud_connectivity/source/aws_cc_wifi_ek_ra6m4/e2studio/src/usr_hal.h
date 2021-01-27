/***********************************************************************************************************************
 * File Name    : usr_hal.h
 * Description  : Contains declarations of data structures and functions used in usr_hal.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright [2015-2020] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
 *
 * The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
 * and/or its licensors ("Renesas") and subject to statutory and contractual protections.
 *
 * This file is subject to a Renesas FSP license agreement. Unless otherwise agreed in an FSP license agreement with
 * Renesas: 1) you may not use, copy, modify, distribute, display, or perform the contents; 2) you may not use any name
 * or mark of Renesas for advertising or publicity purposes or in connection with your use of the contents; 3) RENESAS
 * MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED
 * "AS IS" WITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, AND NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR
 * CONSEQUENTIAL DAMAGES, INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF
 * CONTRACT OR TORT, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents
 * included in this file may be subject to different terms.
 **********************************************************************************************************************/

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
