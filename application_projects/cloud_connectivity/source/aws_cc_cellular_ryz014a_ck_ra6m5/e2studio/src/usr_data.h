/***********************************************************************************************************************
 * File Name    : usr_hal.h
 * Description  : Contains declarations of data structures and macros used commonly project.
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

#ifndef USR_DATA_H_
#define USR_DATA_H_

#define MSG_SIZE                 (32)
#define LED_MSG_SIZE             (64)
#define QUEUE_WAIT               (20)
#define PROCESS_LOOP_TIMEOUT     (100)

/* Message ID used to distinguish the different messages on the message queue. */
enum e_msg_id_type
{
    ID_IAQ_DATA_PULL = 1,
    ID_OAQ_DATA_PULL,
    ID_HS_DATA_PULL,
    ID_ICM_DATA_PULL,
    ID_ICP_DATA_PULL,
    ID_OB1203_DATA_PULL,
    ID_BULK_SENS_DATA_PULL,
    ID_IAQ_DATA_PUSH,
    ID_OAQ_DATA_PUSH,
    ID_HS_DATA_PUSH,
    ID_ICM_DATA_PUSH,
    ID_ICP_DATA_PUSH,
    ID_OB1203_DATA_PUSH,
    ID_BULK_SENS_DATA_PUSH,
    ID_TEMPERATURE_DATA_PUSH,
    ID_SPO2_DATA_PUSH,
};

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

/* Structure for the ICM Sensor Data Handling in the application. */
typedef struct usr_icm_data
{
    icm_acc_data_t acc_data;
    icm_mag_data_t mag_data;
    icm_gyr_data_t gyr_data;
} usr_icm_data_t;

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
    uint16_t breathing_rate;
    float r_p2p;
} usr_ob1203_data_t;

typedef struct bulk_sens_data
{
    usr_iaq_data_t iaq_data;
    usr_oaq_data_t oaq_data;
    usr_hs3001_data_t hs3001_data;
    usr_icm_data_t icm_data;
    usr_icp_data_t icp_data;
    usr_ob1203_data_t ob1203_data;
} bulk_sens_data_t;

/* IAQ  Sensor query message obtained as part of the subscribed message. */
typedef struct sub_iaq_data
{
    char iaq_topic_msg[MSG_SIZE];
} sub_iaq_data_t;

/* OAQ  Sensor query message obtained as part of the subscribed message. */
typedef struct sub_oaq_data
{
    char oaq_topic_msg[MSG_SIZE];
} sub_oaq_data_t;

/* HS3001  Sensor query message obtained as part of the subscribed message. */
typedef struct sub_hs3001_data
{
    char hs3001_topic_msg[MSG_SIZE];
} sub_hs3001_data_t;

/* ICM  Sensor query message obtained as part of the subscribed message. */
typedef struct sub_icm_data
{
    char icm_topic_msg[MSG_SIZE];
} sub_icm_data_t;

/* ICP  Sensor query message obtained as part of the subscribed message. */
typedef struct sub_icp_data
{
    char icp_topic_msg[MSG_SIZE];
} sub_icp_data_t;

/* ob1203  Sensor query message obtained as part of the subscribed message. */
typedef struct sub_ob1203_data
{
    char ob1203_topic_msg[MSG_SIZE];
} sub_ob1203_data_t;

typedef struct sub_bulk_data
{
    char bulk_topic_msg[MSG_SIZE];
} sub_bulk_data_t;

/* Actual LED  message obtained as part of the subscribed message. */
typedef struct led_data
{
    char led_act_topic_msg[LED_MSG_SIZE];
} led_data_t;

/* Message payload for Sensor Data  and LED Message used in the application. */
typedef union msg_payload
{
    sub_iaq_data_t iaq_data;
    sub_oaq_data_t oaq_data;
    sub_hs3001_data_t hs3001_data;
    sub_icm_data_t icm_data;
    sub_icp_data_t icp_data;
    sub_ob1203_data_t ob1203_data;
    led_data_t led_data;
    sub_bulk_data_t bulk_data;
    bulk_sens_data_t sens_data;
} msg_payload_t;

/* Data structure for the message queue data id identifies the message ID, Payload is the
 * Data specific to the ID. msg_payload_t is union of different data types.
 */
typedef struct mq_rx_payload
{
    uint8_t id;
    msg_payload_t value;
} mqtt_rx_payload_t;

#endif /* USR_DATA_H_ */
