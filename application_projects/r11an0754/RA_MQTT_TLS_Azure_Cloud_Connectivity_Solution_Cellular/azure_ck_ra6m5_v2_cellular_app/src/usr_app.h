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
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#ifndef USR_APP_H_
#define USR_APP_H_

#include "nxd_dns.h"
#include "common_utils.h"


/* Default time. GMT: Friday, Jan 1, 2021 12:00:00 AM. Epoch timestamp: 1609459200.  */
#ifndef SAMPLE_SYSTEM_TIME
#define SAMPLE_SYSTEM_TIME               (1679343453) // 1609459200
#endif /* SAMPLE_SYSTEM_TIME  */

#define USR_MSQ_WAIT               (0xFF)
#define LED_MSG_SIZE               (15)

/* Macros used tunnel the IotLogging to RTT and SWO */
#define IotLogInfo      APP_INFO_PRINT
#define IotLogErr       APP_ERR_PRINT
#define IotLogWarn      APP_WARN_PRINT
#define IotLogDebug     APP_DBG_PRINT
#define IotLog          APP_PRINT
#define IotLogErrTrap   APP_ERR_TRAP

#define LIBRARY_LOG_LEVEL IOT_LOG_ERROR

/* IP instance */
extern NX_IP g_ip0;
/* DNS instance */
extern NX_DNS g_dns0;

UINT usr_netx_init(void);
void sample_entry(NX_IP *ip_ptr, NX_PACKET_POOL *pool_ptr, NX_DNS *dns_ptr, UINT (*unix_time_callback)(ULONG *unix_time));
UINT unix_time_get(ULONG *unix_time);
UINT send_data_to_queue(TX_QUEUE *queue_ptr, VOID *source_ptr);

/* If TX_TIMER_TICKS_PER_SECOND is 100, xTimeInMs must be set to more than 10.
 * And (xTimeInMs*TX_TIMER_TICKS_PER_SECOND)/1000  must always be an integer */
#define pdMS_TO_TICKS( xTimeInMs )    ( ((xTimeInMs) * TX_TIMER_TICKS_PER_SECOND) / 1000)

/* LED Number to identify the LED as per BSP support for EK-RA6M3/G Board */
typedef enum e_led_type
{
    LED_BLUE     = 0,    /* LED1 CK HW */
    LED_GREEN   = 1,    /* LED2 CK HW */
    LED_RED    = 2,    /* LED3 CK HW*/
    RGB_LED_RED,
    RGB_LED_GREEN,
    RGB_LED_BLUE,
} e_led_type_t;

/* LED Number to identify the LED as per BSP support for EK-RA6M3/G Board */
typedef enum e_led_state
{
    LED_OFF = 0, LED_ON = 1,
} e_led_state_t;

/* Message ID used to distinguish the different messages on the message queue. */
enum e_msg_id_type
{
    ID_HS3001=0,
    ID_ZMOD4410,
    ID_ICP20100,
    ID_ICM42605,
    ID_ZMOD4510,
    ID_OB1203,
    ID_LED_ACT,
	ID_DISPLAY_HS3001,
	ID_DISPLAY_ZMOD4410,
	ID_DISPLAY_ICP20100,
	ID_DISPLAY_ICM42605,
	ID_DISPLAY_ZMOD4510,
	ID_DISPLAY_OB1203
};

/* Data Structure for ADC Data. */
typedef struct adc_data
{
    uint16_t adc_value;  /* Raw ADC value */
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

/* Message Payload for Temperature sensor message, Push button Data, and LED Message used in the application. */
typedef union msg_payload
{
    led_data_t led_data;
    adc_data_t adc_data;
} msg_payload_t;

/* Data structure for the message queue data id identifies the message ID, Payload is the
 * Data specific to the ID. msg_payload_t is union of different data types.
 */
typedef struct mq_rx_payload
{
    uint8_t id;
    msg_payload_t value;
} mqtt_rx_payload_t;

/* Structure for the IAQ Sensor Data Handling in the application. */
typedef struct usr_iaq_data
{
    float gs_etoh;
    float gs_eco2;
    float gs_tvoc;
} usr_iaq_data_t;

/* Structure for the OAQ Sensor Data Handling in the application. */
typedef struct usr_oaq_data
{
    float gs_air_quality;
} usr_oaq_data_t;

/* Structure for the HS3001 Sensor Data Handling in the application. */
typedef struct usr_hs3001_data
{
    float gs_humidity; /* Global variable to hold humidty reading*/
    float gs_temperature; /* Global variable to hold temperature reading*/
} usr_hs3001_data_t;

/* Structure for the ICM accelerometer Sensor Data Handling in the application. */
typedef struct icm_acc_data
{
    double x;
    double y;
    double z;
} icm_acc_data_t;

/* Structure for the ICM manometer Sensor Data Handling in the application. */
typedef struct icm_mag_data
{
    double x;
    double y;
    double z;
} icm_mag_data_t;

/* Structure for the ICM gyrometer Sensor Data Handling in the application. */
typedef struct icm_gyr_data
{
    double x;
    double y;
    double z;
} icm_gyr_data_t;

/* Structure for the ICM Sensor Data (accelerometer and gyrometer) Handling in the application. */
typedef struct usr_icm_acc_gyr_data
{
    icm_acc_data_t acc_data;
    icm_gyr_data_t gyr_data;
} usr_icm_acc_gyr_data_t;

/* Structure for the ICP Sensor Data Handling in the application. */
typedef struct usr_icp_data
{
    double temperature_C;
    double pressure_Pa;
} usr_icp_data_t;

/* Structure for the OB1203 Sensor Data Handling in the application. */
typedef struct usr_ob1203_data
{
    uint16_t spo2_val;
    uint16_t heart_rate_Val;
    float r_p2p;
    uint16_t breathing_rate;
} usr_ob1203_data_t;

#endif /* USR_APP_H_ */
