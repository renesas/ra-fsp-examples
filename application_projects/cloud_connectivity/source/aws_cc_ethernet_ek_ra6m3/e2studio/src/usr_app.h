/***********************************************************************************************************************
 * File Name    : usr_app.h
 * Description  : Contains macros, data structures and functions used  in the Application
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
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
