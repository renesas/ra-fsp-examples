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


#define PBS1_CHANNEL 13
#define PBS2_CHANNEL 12

extern bsp_leds_t g_bsp_leds;

/* LED Number to identify the LED as per BSP support for EK-RA6M3/G Board */
typedef enum e_led_type
{
    LED_BLUE = 0, LED_GREEN = 1, LED_RED = 2,
} e_led_type_t;
/* LED Number to identify the LED as per BSP support for EK-RA6M3/G Board */
typedef enum e_led_state
{
    LED_OFF = 0, LED_ON = 1,
} e_led_state_t;

fsp_err_t usr_hal_init(void);
void pb_callback(external_irq_callback_args_t *p_args);
void adc_mcu_temp_callback(adc_callback_args_t *p_args);
void led_on_off(e_led_type_t ltype, e_led_state_t lled_state);
void led_toggle(e_led_type_t ltype);

#define FAILURE_INDICATION              led_on_off(LED_RED,LED_ON);
#define FAILURE_RECOVERED_INDICATION    led_on_off(LED_RED,LED_OFF);
#define NETWORK_CONNECT_INDICATION      led_on_off(LED_GREEN,LED_ON);
#define AWS_CONNECT_INDICATION          led_on_off(LED_BLUE,LED_ON);
#define AWS_ACTIVITY_INDICATION         led_toggle(LED_BLUE);
#define NETWORK_ACTIVITY_INDICATION     led_toggle(LED_GREEN);
#define MQTT_ACTIVITY_FAILURE           led_toggle(LED_RED);
#endif /* USR_HAL_H_ */
