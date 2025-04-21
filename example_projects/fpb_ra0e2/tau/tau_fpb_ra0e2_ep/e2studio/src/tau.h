/***********************************************************************************************************************
 * File Name    : tau.h
 * Description  : Contains macros and functions used in tau.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef TAU_H_
#define TAU_H_

#include "common_utils.h"


/* Macro for the sequence1 information */
#define SEQUENCE1_INFO          "\r\n******************************************************************"\
                                "\r\n*              Interval timer and delay timer mode.              *"\
                                "\r\n* This mode uses interval timer to set OFF time of LED1 and      *"\
                                "\r\n* the delay timer is used to set the ON time for LED1.           *"\
                                "\r\n******************************************************************\r\n"

/* Macro for the sequence2 information */
#define SEQUENCE2_INFO          "\r\n******************************************************************"\
                                "\r\n*                  External event counter mode.                  *"\
                                "\r\n* The user sets the frequency and divider for square wave.       *"\
                                "\r\n* The input capture will measure pulse width of one period and   *"\
                                "\r\n* turn on LED2. The external event counter counts LED2 ON times  *"\
                                "\r\n* and turn off LED2 once it blinks 5 times.                      *"\
                                "\r\n******************************************************************\r\n"

/* Macro for the sequence3 information */
#define SEQUENCE3_INFO          "\r\n******************************************************************"\
                                "\r\n*            Low level pulse width measurement mode.             *"\
                                "\r\n* The user sets the period for square wave, and the low level    *"\
                                "\r\n* pulse width of square wave will be measured.                   *"\
                                "\r\n******************************************************************\r\n"


/* Macro for the measurable input signal information */
#define MEASURABLE_WARNING      "\r\n\r\nWARNING: The input signal cannot be measured because it is outside the\r\n"\
                                "\r\nmeasurable range, limit values will be displayed instead of the expected\r\n"\
                                "\r\nvalue.\r\n\r\n"

/* Macro for the input square wave frequency requirement */
#define FREQUENCY_INPUT         "\r\n\r\nNOTE: Please input a frequency that is a divisor of %dHz"\
                                "\r\nto generate a square wave as expected.\r\n\r\n"

/* Common macro */
#define BUFF_SIZE                       (30U)
#define ONE                             (1)

/* Macro for define LEDs on board */
#define LED_SEQUENCE1                   (0U)
#define LED_SEQUENCE2                   (1U)

/* Macro for the maximum value TAU can capture due to overflow limitation */
#define CAPTURE_VALUE_MAX               (65536 * 2)

/* Macro for the minimum value of user input */
#define TIME_PERIOD_MIN                 (1)
#define SQUARE_WAVE_FREQUENCY_MIN       (1)
#define DIVIDER_MIN                     (1)
#define SQUARE_WAVE_PERIOD_MIN          (1)

/* Macro for the demonstration times */
#define DEMO_TIMES                      (5)

/* Macro for the conversion */
#define CONVERT_SECOND_TO_MILISECOND    (1000)
#define CONVERT_HALF_TO_ONE_CYCLE       (2)

/* Macro for the period counts */
#define MAX_PERIOD                      (65536)
#define MIN_PERIOD                      (1)

/* Enumeration for TAU operation */
typedef enum e_tau_mode
{
    INTERVAL_TIMER = 0,
    DELAY_COUNTER,
    SQUARE_WAVE_SEQUENCE2,
    DIVIDER,
    INPUT_CAPTURE,
    EXTERNAL_EVENT_COUNT,
    SQUARE_WAVE_SEQUENCE3,
    LOW_LEVEL_MEASUREMENT,
    TAU_MAX_MODE
} tau_mode_t;

/* Public function declarations */
void tau_entry (void);

#endif /* TAU_H_ */
