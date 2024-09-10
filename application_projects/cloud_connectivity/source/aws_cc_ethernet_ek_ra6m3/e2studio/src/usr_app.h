/***********************************************************************************************************************
 * File Name    : usr_app.h
 * Description  : Contains macros, data structures and functions used  in the Application
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/ 

#ifndef USR_APP_H_
#define USR_APP_H_

#define ATHOME    0
#define ATWORK    0
#define NETWORK   1
#define MQTT      1
	
#define LIBRARY_LOG_LEVEL               IOT_LOG_DEBUG  /* IOT_LOG_DEBUG, IOT_LOG_WARN, IOT_LOG_INFO, IOT_LOG_ERROR, IOT_LOG_NONE */
	
#define LOGGING_TASK_STACK_SIZE         (1 * 1024)
#define LOGGING_TASK_STACK_PRIORITY     (6)
#define LOGGING_TASK_QUEUE_SIZE         (1 * 1024)

#define FAILURE_INDICATION              led_on_off(LED_RED,LED_ON);
#define FAILURE_RECOVERED_INDICATION    led_on_off(LED_RED,LED_OFF);
#define NETWORK_CONNECT_INDICATION      led_on_off(LED_GREEN,LED_ON);
#define AWS_CONNECT_INDICATION          led_on_off(LED_BLUE,LED_ON);
#define AWS_ACTIVITY_INDICATION         led_toggle(LED_BLUE);
#define NETWORK_ACTIVITY_INDICATION     led_toggle(LED_GREEN);
#define MQTT_ACTIVITY_FAILURE           led_toggle(LED_RED);

#define LOG_VERBOSE(...) \
		do { \
			vLoggingPrintf(__VA_ARGS__); \
			vLoggingPrintf("\r\n"); \
		} while(0)

extern TaskHandle_t application_thread;


extern uint32_t  get_mqtt_fail_count(void);
extern void print_float(char *buffer, size_t buflen, double value);
extern int parse_led_actuation_message(mqtt_rx_payload_t  *lmq_data);
extern fsp_err_t config_littlFs_flash(void);

#endif /* USR_APP_H_ */
