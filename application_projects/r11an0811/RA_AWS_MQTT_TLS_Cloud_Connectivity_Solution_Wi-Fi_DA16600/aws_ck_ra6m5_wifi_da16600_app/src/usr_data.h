/***********************************************************************************************************************
 * File Name    : usr_data.h
 * Description  : Contains declarations of data structures and macros used commonly project.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2015 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef USR_DATA_H_
#define USR_DATA_H_

#define MSG_SIZE                 (32)
#define LED_MSG_SIZE             (64)
#define QUEUE_WAIT               (200)
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
    float gs_humidity; /* Global variable to hold humidity reading*/
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

/* Structure for the ICM gyroscope Sensor Data Handling in the application. */
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
    float r_p2p;
    uint16_t breathing_rate;
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


/* Actual LED  message obtained as part of the subscribed message. */
typedef struct led_data
{
    char led_act_topic_msg[LED_MSG_SIZE];
} led_data_t;


/* Data structure for the message queue data id identifies the message ID.
 * led_data_t is struct of led's action message.
 */
typedef struct mq_rx_payload
{
    uint8_t id;
    led_data_t led_data;
} mqtt_rx_payload_t;

#endif /* USR_DATA_H_ */
