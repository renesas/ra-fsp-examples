/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
 * File Name    : system_api.h
 * Description  : Header file for
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *           06.03.2015 1.00    Initial Release.
 **********************************************************************************************************************/

#ifndef SYSTEM_API_H
#define SYSTEM_API_H

#include "gx_api.h"
#include "r_rtc_api.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define APP_ERR_TRAP(err)           if(err) { __asm("BKPT #0\n");} /* trap upon the error  */ // @suppress("3.9b, 3.9f, 8.4b Macro expansion")
#define ADCTEMP_AS_C(a)             ((((float)a) * 0.196551f) - 277.439f) // @suppress("8.4b Macro expansion parameter")
#define ADCTEMP_AS_F(a)             ((((float)a) * 0.353793f) - 467.39f) // @suppress("8.4b Macro Comment") // @suppress("8.4b Macro expansion parameter")

/* Change or modify this based on your application requirement */
#define BYTE_POOL_SIZE              (ULONG)256U // @suppress("3.9b, 3.9f, 8.4b Macro expansion")

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/
typedef enum e_sf_message_event_class
{
    SF_MESSAGE_EVENT_CLASS_AUDIO, /* Audio Playback */
    SF_MESSAGE_EVENT_CLASS_TOUCH, /* Touch */
    SF_MESSAGE_EVENT_CLASS_SYSTEM, /* System */
    SF_MESSAGE_EVENT_CLASS_VOLUME, /* Volume */
    SF_MESSAGE_EVENT_CLASS_TIME, /* Time */
    SF_MESSAGE_EVENT_CLASS_TEMPERATURE, /* Temperature */
    SF_MESSAGE_EVENT_CLASS_DISPLAY, /* Display */
} sf_message_event_class_t;
typedef enum e_sf_message_event
{
    SF_MESSAGE_EVENT_UNUSED, /* Unused */
    SF_MESSAGE_EVENT_NEW_DATA, /* New Data */
    SF_MESSAGE_EVENT_AUDIO_START, /* Audio Playback Start */
    SF_MESSAGE_EVENT_AUDIO_STOP, /* Audio Playback Stop */
    SF_MESSAGE_EVENT_AUDIO_PAUSE, /* Audio Playback Pause */
    SF_MESSAGE_EVENT_AUDIO_RESUME, /* Audio Playback Resume */
    SF_MESSAGE_EVENT_REFRESH, /* Refresh */
    SF_MESSAGE_EVENT_TEMPERATURE_INCREMENT, /* Temperature Increment */
    SF_MESSAGE_EVENT_TEMPERATURE_DECREMENT, /* Temperature Decrement */
    SF_MESSAGE_EVENT_HOUR_MODE_TOGGLE, /* Hour Mode Toggle */
    SF_MESSAGE_EVENT_AM_PM_TOGGLE, /* AM PM Toggle */
    SF_MESSAGE_EVENT_SET_TIME, /* Set Time */
    SF_MESSAGE_EVENT_SET_DATE, /* Set Date */
    SF_MESSAGE_EVENT_FAN_TOGGLE, /* Fan Toggle */
    SF_MESSAGE_EVENT_SYSTEM_MODE_TOGGLE, /* System Mode Toggle */
    SF_MESSAGE_EVENT_TEMPERATURE_UNIT_C, /* Temperature Unit C */
    SF_MESSAGE_EVENT_TEMPERATURE_UNIT_F, /* Temperature Unit F */
    SF_MESSAGE_EVENT_BRIGHTNESS_INCREMENT, /* Brightness Increment */
    SF_MESSAGE_EVENT_BRIGHTNESS_DECREMENT, /* Brightness Decrement */
    SF_MESSAGE_EVENT_VOLUME_INCREMENT, /* Volume Increment */
    SF_MESSAGE_EVENT_VOLUME_DECREMENT, /* Volume Decrement */
    SF_MESSAGE_EVENT_UPDATE_TEMPERATURE, /* Update Temperature */
    SF_MESSAGE_EVENT_UPDATE_TIME, /* Update Time */
    SF_MESSAGE_EVENT_UPDATE_DATE, /* Update Date */
} sf_message_event_t;

/** Conversion of system events to GUIX events */
typedef enum
{
    /** Some classes do not use the event enumeration. */
    GXEVENT_MSG_UNUSED = GX_FIRST_APP_EVENT,

    /** System data refresh. */
    GXEVENT_MSG_REFRESH_SYSTEM_DATA,

    /** Temperature set point adjustment. */
    GXEVENT_MSG_TEMPERATURE_INCREMENT,
    GXEVENT_MSG_TEMPERATURE_DECREMENT,

    /** Time setting. */
    GXEVENT_MSG_HOUR_MODE_TOGGLE,
    GXEVENT_MSG_AM_PM_TOGGLE,
    GXEVENT_MSG_TIME_UPDATE,
    GXEVENT_MSG_DATE_UPDATE,

    /** System settings. */
    GXEVENT_MSG_FAN_TOGGLE,
    GXEVENT_MSG_SYSTEM_MODE_TOGGLE,

    /** Temperature unit settings. */
    GXEVENT_MSG_TEMPERATURE_UNIT_C,
    GXEVENT_MSG_TEMPERATURE_UNIT_F,

    /** Display Settings. */
    GXEVENT_MSG_BRIGHTNESS_INCREMENT,
    GXEVENT_MSG_BRIGHTNESS_DECREMENT,

    /** Volume Settings. */
    GXEVENT_MSG_VOLUME_INCREMENT,
    GXEVENT_MSG_VOLUME_DECREMENT,

    /** Temperature data. */
    GXEVENT_MSG_UPDATE_TEMPERATURE,

    /** Time data. */
    GXEVENT_MSG_UPDATE_TIME,
} gx_event_message_t;

typedef enum
{
    SYSTEM_TEMP_UNITS_C,
    SYSTEM_TEMP_UNITS_F,
} system_temp_units_t;

typedef enum
{
    SYSTEM_HOUR_MODE_24  = 0,
    SYSTEM_HOUR_MODE_12  = 1,
} system_hour_mode_t;

typedef enum
{
    SYSTEM_FAN_MODE_AUTO   = 0,
    SYSTEM_FAN_MODE_ON     = 1,
} system_fan_mode_t;

typedef enum
{
    SYSTEM_MODE_OFF = 0,
    SYSTEM_MODE_COOL,
    SYSTEM_MODE_HEAT,
    SYSTEM_MODE_MAX
} system_mode_t;


typedef struct st_time_payload
{
    rtc_time_t  time;
} time_payload_t;

/** Message header definition  */
typedef struct st_message_header
{
    union
    {
        uint32_t  event;
        struct
        {
            uint32_t  class_code     : 8;      /* Event class code */
            uint32_t  class_instance : 8;      /* Event class instance number */
            uint32_t  code           : 16;     /* Event code */
        }  event_b;
    };
} system_msg_header_t;

typedef struct st_system_state
{
    rtc_time_t          time;
    system_hour_mode_t  hour_mode;
    system_mode_t       mode;
    system_fan_mode_t   fan_mode;
    system_temp_units_t temp_units;
    float               temp_c;         /* In Celsius */
    float               target_temp;    /* In system_state_t::temp_units */
    uint8_t             brightness;
    bool                fan_on;
} system_state_t;

typedef struct st_system_payload
{
    GX_EVENT        gx_event;
    system_state_t  state;
} system_payload_t;

typedef union u_system_msg_payload
{
    time_payload_t time_payload;
} system_msg_payload_t;

typedef struct u_system_msg
{
    system_msg_header_t msg_id;
    system_msg_payload_t msg_payload;
}system_msg_t; /*system_payload_t*/


#endif /* SYSTEM_API_H */
