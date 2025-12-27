/***********************************************************************************************************************
 * File Name    : usr_hal.h
 * Description  : Contains declarations of data structures and functions used in usr_hal.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef USR_HAL_H_
#define USR_HAL_H_

#include "hal_data.h"
#include "usr_app.h"


#define PBS1_CHANNEL (10)
#define PBS2_CHANNEL (9)

extern bsp_leds_t g_bsp_leds;

/* Function prototype */
fsp_err_t usr_hal_init(void);
void led_on_off(e_led_type_t ltype, e_led_state_t lled_state);
void led_toggle(e_led_type_t ltype);

#define FAILURE_INDICATION              (led_on_off(LED_RED,LED_ON))
#define FAILURE_RECOVERED_INDICATION    (led_on_off(LED_RED,LED_OFF))
#define CONNECT_INDICATION              (led_on_off(LED_GREEN,LED_ON))
#define NETWORK_DISCONNECT_INDICATION   (led_on_off(LED_GREEN,LED_OFF))
#define ACTIVITY_INDICATION             (led_toggle(LED_GREEN))
#define MQTT_ACTIVITY_FAILURE           (led_toggle(LED_RED))
#endif /* USR_HAL_H_ */
