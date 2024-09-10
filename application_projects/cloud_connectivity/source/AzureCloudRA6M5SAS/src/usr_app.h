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

#include "nxd_dns.h"
#include "common_utils.h"


/* Default time. GMT: Friday, Jan 1, 2021 12:00:00 AM. Epoch timestamp: 1609459200.  */
#ifndef SAMPLE_SYSTEM_TIME
#define SAMPLE_SYSTEM_TIME                1609459200
#endif /* SAMPLE_SYSTEM_TIME  */

#define USR_MSQ_WAIT               (0xFF)
#define LED_ON_SIZE                (2)
#define LED_OFF_SIZE               (3)
#define LED_MSG_SIZE               (8)

#define MAX_PROPERTY_LEN           (8)

/* Macro for Push Button Switches */
#define PBS1                     (1)
#define PBS2                     (2)
#define PB_PRESSED               (1)

#undef  LogErr
#define LogErr          APP_ERR_PRINT
/* Macros used tunnel the IotLogging to RTT and SWO */
#define IotLogInfo      APP_INFO_PRINT
#define IotLogErr       APP_ERR_PRINT
#define IotLogWarn      APP_WARN_PRINT
#define IotLogDebug     APP_DBG_PRINT
#define IotLog          APP_PRINT
#define IotLogErrTrap  APP_ERR_TRAP

#define LIBRARY_LOG_LEVEL IOT_LOG_ERROR
	
#define FAILURE_INDICATION         led_on_off(LED_RED,LED_ON);
#define NETWORK_CONNECT_INDICATION led_on_off(LED_GREEN,LED_ON);
#define CONNECT_INDICATION     led_on_off(LED_BLUE,LED_ON);
#define ACTIVITY_INDICATION    led_toggle(LED_BLUE);

#define LOG_VERBOSE(...) \
		do { \
			vLoggingPrintf(__VA_ARGS__); \
			vLoggingPrintf("\r\n"); \
		} while(0)


/* IP instance */
extern NX_IP g_ip0;
/* DNS instance */
extern NX_DNS g_dns0;

fsp_err_t usr_hal_init(void);
UINT usr_packet_pool_init(void);
UINT usr_netx_init(void);
void sample_entry(NX_IP *ip_ptr, NX_PACKET_POOL *pool_ptr, NX_DNS *dns_ptr, UINT (*unix_time_callback)(ULONG *unix_time));
UINT unix_time_get(ULONG *unix_time);


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

/* Message ID used to distinguish the different messages on the message queue. */
enum e_msg_id_type
{
    ID_PB = 1, ID_TEMPERATURE, ID_LED_ACT,
};

/* Structure for the Push button Handling in the application. */
typedef struct usr_pb_Data
{
    uint16_t pb_num;   // 1,2
    uint16_t pb_state; // PB_PRESSED indicates the Button Press.
} usr_pb_data_t;

/* Data Structure for Push button message used in the application. */
typedef struct pb_msg
{
    uint8_t id;
    usr_pb_data_t pb_data;
} usr_pb_msg_t;

/* Data Structure for ADC Data. */
typedef struct adc_data
{
    uint16_t adc_value;  // Raw ADC value
} adc_data_t;

/* Data Structure for Temperature sensor message used in the application. */
typedef struct temp_msg
{
    uint8_t id;
    adc_data_t adc_data;
} mcu_temp_msg_t;

/* Actual LED  message obtained as part of the subscribed message. */
typedef struct led_data
{
    char led_act_topic_msg[LED_MSG_SIZE];
} led_data_t;

/* Data Structure LED Actuation message */
typedef struct led_msg
{
    uint8_t id;
    led_data_t led_data;
} led_msg_t;

/* Message Payload for Temperature sensor message, Push button Data, and LED Message used in the application. */
typedef union msg_payload
{
    led_data_t led_data;
    adc_data_t adc_data;
    usr_pb_data_t pb_data;
} msg_payload_t;

/* Data structure for the message queue data id identifies the message ID, Payload is the
 * Data specific to the ID. msg_payload_t is union of different data types.
 */
typedef struct mq_rx_payload
{
    uint8_t id;
    msg_payload_t value;
} mqtt_rx_payload_t;

/* user function Declaration of mqtt related function. */
extern int sendMessage_ts(char *ltemp_str, UINT msg_id);
extern int sendMessage_pb(uint16_t switch_num, UINT msg_id);
extern void print_float(char *buffer, size_t buflen, double value);

#endif /* USR_APP_H_ */
