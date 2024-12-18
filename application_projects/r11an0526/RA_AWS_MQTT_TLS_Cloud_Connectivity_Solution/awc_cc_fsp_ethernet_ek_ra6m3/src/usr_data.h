/*
 * usr_data.h
 *
 *  Created on: Jun 17, 2021
 *      Author: a5123412
 */

#ifndef USR_DATA_H_
#define USR_DATA_H_

/* Macro for Push Button Switches */
#define PBS1                     (1)
#define PBS2                     (2)
#define PB_PRESSED               (1)

#define LED_MSG_SIZE             (64)
#define QUEUE_WAIT               (200)
#define PROCESS_LOOP_TIMEOUT     (100)

/* Message ID used to distinguish the different messages on the message queue. */
enum e_msg_id_type
{
    ID_PB = 1,
    ID_TEMPERATURE = 2,
    ID_LED_ACT = 3
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

#endif /* USR_DATA_H_ */
