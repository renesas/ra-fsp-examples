/***********************************************************************************************************************
 * Copyright [2015] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
 *
 * The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
 * and/or its licensors ("Renesas") and subject to statutory and contractual protections.
 *
 * Unless otherwise expressly agreed in writing between Renesas and you: 1) you may not use, copy, modify, distribute,
 * display, or perform the contents; 2) you may not use any name or mark of Renesas for advertising or publicity
 * purposes or in connection with your use of the contents; 3) RENESAS MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE
 * SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND
 * NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR CONSEQUENTIAL DAMAGES,
 * INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF CONTRACT OR TORT, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents included in this file may
 * be subject to different terms.
 **********************************************************************************************************************/

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
//#include "r_rtc_api.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define APP_ERR_TRAP(err)        if(err) { __asm("BKPT #0\n");} /* trap upon the error  */
#define ADCTEMP_AS_C(a)             ((((float)a) * 0.196551f) - 277.439f)
#define ADCTEMP_AS_F(a)             ((((float)a) * 0.353793f) - 467.39f)

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

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

typedef struct st_system_state
{
//    rtc_time_t          time;
    system_hour_mode_t  hour_mode;
    system_mode_t       mode;
    system_fan_mode_t   fan_mode;
    system_temp_units_t temp_units;
    float               temp_c;         ///< In Celsius
    float               target_temp;    ///< In system_state_t::temp_units
    uint8_t             brightness;
//    bool                fan_on;
} system_state_t;

typedef struct st_system_payload
{
    GX_EVENT        gx_event;
    system_state_t  state;
} system_payload_t;

#endif /* SYSTEM_API_H */
