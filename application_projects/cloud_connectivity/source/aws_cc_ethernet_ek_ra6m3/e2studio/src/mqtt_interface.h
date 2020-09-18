/***********************************************************************************************************************
 * File Name    : mqtt_interface.h
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
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
#ifndef MQTT_INTERFACE_H_
#define MQTT_INTERFACE_H_

#include "common_utils.h"

/* Provide default values for undefined configuration settings. */
#ifndef MQTT_TOPIC_PREFIX
#define MQTT_TOPIC_PREFIX			"aws"
#endif

#ifndef MQTT_PUBLISH_SIZE
#define MQTT_PUBLISH_SIZE			( 1 )
#endif

#ifndef MQTT_PUBLISH_COUNT
#define MQTT_PUBLISH_COUNT			( 1 )
#endif

/*************************************************************************************
 * The first characters in the client identifier. A timestamp is appended
 * to this prefix to create a unique client identifer.
 *
 * This prefix is also used to generate topic names and topic filters used in this
 * Application Project.
 *************************************************************************************/
#define CLIENT_IDENTIFIER_PREFIX "Renesas"

/**************************************************************************************
 * The longest client identifier that an MQTT server must accept (as defined
 * by the MQTT 3.1.1 spec) is 23 characters. Add 1 to include the length of the NULL
 * terminator.
 *************************************************************************************/
#define CLIENT_IDENTIFIER_MAX_LENGTH ( 24 )

/**************************************************************************************
 * The keep-alive interval used for this Application Project
 * An MQTT ping request will be sent periodically at this interval.
 *************************************************************************************/
#define KEEP_ALIVE_SECONDS			( 60 )

/**************************************************************************************
 * The timeout for MQTT operations in this Application Project.
 *************************************************************************************/
#define MQTT_TIMEOUT_MS 			( 5000 )

/*************************************************************************************
 * The Last Will and Testament topic name in this Application Project.
 * The MQTT server will publish a message to this topic name if this client is
 * unexpectedly disconnected.
 ************************************************************************************/
#define WILL_TOPIC_NAME 			MQTT_TOPIC_PREFIX "/will"

/*************************************************************************************
 * The length of #WILL_TOPIC_NAME.
 ************************************************************************************/
#define WILL_TOPIC_NAME_LENGTH	( ( uint16_t ) ( sizeof( WILL_TOPIC_NAME ) - 1 ) )

/*************************************************************************************
 * The message to publish to #WILL_TOPIC_NAME.
 *************************************************************************************/
#define WILL_MESSAGE		   "MQTT Application unexpectedly disconnected."

/*************************************************************************************
 * The length of #WILL_MESSAGE.
 *************************************************************************************/
#define WILL_MESSAGE_LENGTH 		( ( size_t ) ( sizeof( WILL_MESSAGE ) - 1 ) )

/*************************************************************************************
 * How many topic filters will be used in this Application Project.
 ************************************************************************************/
#define TOPIC_FILTER_COUNT			( 1 )

/************************************************************************************
 * Format string of the PUBLISH messages in this Application Project.
 ***********************************************************************************/
#define PUBLISH_PAYLOAD_FORMAT_TS "MCU Temperature (F) %s"

#define PUBLISH_PAYLOAD_FORMAT_PB "Push Button Switch S%d Pressed!"

/************************************************************************************
 * Size of the buffer that holds the PUBLISH messages in this Application Project.
 ***********************************************************************************/
#define PUBLISH_PAYLOAD_BUFFER_LENGTH_TS ( sizeof( PUBLISH_PAYLOAD_FORMAT_TS ) + 5 )
#define PUBLISH_PAYLOAD_BUFFER_LENGTH_PB ( sizeof( PUBLISH_PAYLOAD_FORMAT_PB ) + 2 )



#define PUBLISH_PAYLOAD_FORMAT_PB_JSON "{\r\n" \
									   "\"status\" : {\r\n" \
									   " 	  \"User Button(S%d)\" :\"Pressed\"\r\n" \
									   "\r\n 		 }\r\n" \
									   "}\r\n"
									   
#define PUBLISH_PAYLOAD_BUFFER_LENGTH_PB_JSON ( sizeof( PUBLISH_PAYLOAD_FORMAT_TS_JSON ) + 2 )

#define PUBLISH_PAYLOAD_FORMAT_TS_JSON "{\r\n" \
                                       "\"status\" : {\r\n" \
                                       "       \"temperature(F)\" :\"%s\"\r\n" \
                                       "\r\n         }\r\n" \
                                       "}\r\n"

#define PUBLISH_PAYLOAD_BUFFER_LENGTH_TS_JSON ( sizeof( PUBLISH_PAYLOAD_FORMAT_TS_JSON ) + 5 )


/***********************************************************************************
 * The max number of times each PUBLISH in this Application Project will be retried.
 ***********************************************************************************/
#define PUBLISH_RETRY_LIMIT 		( 10 )

/***********************************************************************************
 * A PUBLISH message is retried if no response is received within this time
 ***********************************************************************************/
#define PUBLISH_RETRY_MS			( 1000 )

/***********************************************************************************
 * The topic name on which acknowledgement messages for incoming publishes
 * should be published.
 **********************************************************************************/
#define ACK_TOPIC_NAME				MQTT_TOPIC_PREFIX "/acknowledgements"

/**********************************************************************************
 * The length of #ACK_TOPIC_NAME.
 **********************************************************************************/
#define ACK_TOPIC_NAME_LENGTH	( ( uint16_t ) ( sizeof( ACK_TOPIC_NAME ) - 1 ) )

/**********************************************************************************
 * Format string of PUBLISH acknowledgement messages in this Application Project.
 **********************************************************************************/
#define ACK_MESSAGE_FORMAT			"Client has received PUBLISH %.*s from server."

/***********************************************************************************
 * Size of the buffers that hold acknowledgement messages in this Application Project.
 ***********************************************************************************/
#define ACK_MESSAGE_BUFFER_LENGTH ( sizeof( ACK_MESSAGE_FORMAT ) + 2 )

#define LED_MSG_SIZE                (64)

#define MAX_PROPERTY_LEN            (8)

/* Macro for Push Button Switches */
#define PBS1                     (1)
#define PBS2                     (2)
#define PB_PRESSED               (1)

/* Macros used tunnel the IotLogging to RTT and SWO */
#define IotLogInfo      APP_INFO_PRINT
#define IotLogError     APP_ERR_PRINT
#define IotLogWarn      APP_WARN_PRINT
#define IotLogDebug     APP_DBG_PRINT
#define IotLog          APP_PRINT
#define IotLogErrorTrap APP_ERR_TRAP

#define MQTT_REQUIRED

/***********************************************************************************************************************
 * Total length of the hash in bytes.
 ***********************************************************************************************************************/
#define _MD5_HASH_LENGTH_BYTES              (16)

/***********************************************************************************************************************
 * Length of device identifier.
 * Device identifier is represented as hex string of MD5 hash of the device certificate.
 **********************************************************************************************************************/
#define AWS_IOT_DEVICE_IDENTIFIER_LENGTH    (_MD5_HASH_LENGTH_BYTES * 2)

/**********************************************************************************************************************
 * Device metrics name format
 *********************************************************************************************************************/
#define AWS_IOT_METRICS_NAME                "?SDK=" "AmazonFreeRTOS" "&Version=4.0.0&Platform=" "Renesas" \
                                                                                                "&AFRDevID=%.*s"

/**********************************************************************************************************************
 * Length of AWS_IOT_METRICS_NAME.
 **********************************************************************************************************************/
#define AWS_IOT_METRICS_NAME_LENGTH         ((uint16_t) (sizeof(AWS_IOT_METRICS_NAME) + \
                                                         AWS_IOT_DEVICE_IDENTIFIER_LENGTH))

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

/* Data Structure used for the MQTT Client connection. This holds the aws supported mqtt mode,
 * client identifier, network server info, network credential info, network connection API
 * MQTT library init status, mqtt connection status.
 */
typedef struct mqtt_cfg
{
    bool aws_mqtt_mode;
    const char *p_identifier;
    IotNetworkServerInfo_t *p_nwk_srvr_info;
    IotNetworkCredentials_t *p_nwk_credn_info;
    IotNetworkInterface_t *p_nwk_intf;
    bool mqtt_lib_init_status;
    bool mqtt_connect_status;
} mqtt_cfg_t;

typedef struct mqtt_status
{
    bool mqtt_lib_init_status;
    bool mqtt_connect_status;
    int status;
} mqtt_status_t;

/* user function Declaration of mqtt related function. */
int mqtt_init(mqtt_cfg_t *ptr);
void mqtt_cleanup(void);
int sendMessage_ts(char *temp_msg);
int sendMessage_pb(uint16_t switch_num);
int mqtt_deinit(mqtt_cfg_t *ptr);
void mqtt_cleanup_and_reinit(void);

//! [MQTT-required-functions]
const char *getDeviceIdentifier(void);
const char *getDeviceMetrics(void);
uint16_t getDeviceMetricsLength(void);

#endif /* MQTT_INTERFACE_H_ */
