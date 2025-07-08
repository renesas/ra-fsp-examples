/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : slcd.h
 * Version      : .
 * Description  : .
 *********************************************************************************************************************/


#include "hal_data.h"
#include "r_typedefs.h"

#ifndef SLCD_H_
#define SLCD_H_

typedef enum e_day_of_week {
    DOW_SUNDAY    = 0x01,
    DOW_MONDAY    = 0x02,
    DOW_TUESDAY   = 0x04,
    DOW_WEDNESDAY = 0x08,
    DOW_THURSDAY  = 0x10,
    DOW_FRIDAY    = 0x20,
    DOW_SATURDAY  = 0x40,
    DOW_OFF       = 0x80
} e_day_of_week_t;

typedef enum e_battery_level {
    BATTERY_EMPTY = 0,
    BATTERY_0 = 0,
    BATTERY_1,
    BATTERY_2,
    BATTERY_3,
    BATTERY_FULL = 4,
    BATTERY_4 = 4,
    BATTERY_OFF
} e_battery_level_t;

typedef enum e_am_or_pm {
    AM_TIME,
    PM_TIME,
    AM_PM_OFF
} e_am_or_pm_t;

typedef enum e_degc_or_degf {
    TEMP_DEG_C,
    TEMP_DEG_F,
    DEG_C_F_OFF
} e_degc_or_degf_t;

typedef enum e_medium_dp_or_colon {
    SET_DP,
    SET_COLON,
    DP_COLON_OFF
} e_medium_dp_or_colon_t;

typedef enum e_on_or_off {
    ICON_ON,
    ICON_OFF
} e_on_or_off_t;

typedef enum e_big_digit_dp {
    WITH_DP,
    NO_DP
} e_big_digit_dp_t;

typedef enum e_big_number_integer_units {
    BN_UNITS,
    BN_TENS,
    BN_HUNDREDS
} e_big_number_integer_units_t;

typedef enum e_medium_number_units {
    MN_UNITS,
    MN_TENS,
    MN_HUNDREDS,
    MN_THOUSANDS,
    MN_TEN_THOUSANDS
} e_medium_number_units_t;

#define HEAT_SEGMENT            (13)  /* 0x0d */
#define HEAT_DATA               (0x01)
#define HEAT_ICON               (0x0d01)

#define COOL_SEGMENT            (18)  /* 0x12 */
#define COOL_DATA               (0x01)
#define COOL_ICON               (0x1201)

#define FAN_SEGMENT             (43)
#define FAN_DATA                (0x01)
#define FAN_ICON                (0x2b01)

#define ZONE_SEGMENT            (42)
#define ZONE_DATA               (0x01)
#define ZONE_ICON               (0x2a01)

#define MG_ML_SEGMENT           (44)
#define MG_ML_DATA              (0x01)
#define MG_ML_ICON              (0x2c01)

#define MG_HG_SEGMENT           (45)
#define MG_HG_DATA              (0x01)
#define MG_HG_ICON              (0x2d01)

#define HEART_SEGMENT           (40)
#define HEART_DATA              (0x01)
#define HEART_ICON              (0x2801)

#define VOLTS_SEGMENT           (41)
#define VOLTS_DATA              (0x01)
#define VOLTS_ICON              (0x2901)

#define RENESAS_SEGMENT         (21)  /* 0x15 */
#define RENESAS_DATA            (0x08)
#define RENESAS_ICON            (0x1508)

#define ALARM_SEGMENT           (35)  /* 0x23 */
#define ALARM_DATA              (0x02)
#define ALARM_ICON              (0x2302)

#define DEG_C_SEGMENT           (37) /* 0x25 */
#define DEG_C_DATA              (0x04)
#define DEG_C_ICON              (0x2504)

#define DEG_F_SEGMENT           (37) /* 0x25 */
#define DEG_F_DATA              (0x08)
#define DEG_F_ICON              (0x2508)

#define BATTERY_SHELL_SEGMENT   (37) /* 0x25 */
#define BATTERY_SHELL_DATA      (0x01)
#define BATTERY_SHELL_ICON      (0x2501)

#define BATTERY_LEVEL1_SEGMENT  (38) /* 0x26 */
#define BATTERY_LEVEL1_DATA     (0x08)
#define BATTERY_LEVEL1_ICON     (0x2608)

#define BATTERY_LEVEL2_SEGMENT  (38) /* 0x26 */
#define BATTERY_LEVEL2_DATA     (0x04)
#define BATTERY_LEVEL2_ICON     (0x2604)

#define BATTERY_LEVEL3_SEGMENT  (38) /* 0x26 */
#define BATTERY_LEVEL3_DATA     (0x02)
#define BATTERY_LEVEL3_ICON     (0x2602)

#define BATTERY_LEVEL4_SEGMENT  (38) /* 0x26 */
#define BATTERY_LEVEL4_DATA     (0x01)
#define BATTERY_LEVEL4_ICON     (0x2601)

#define AM_SEGMENT              (35) /* 0x23 */
#define AM_DATA                 (0x01)
#define AM_ICON                 (0x2301)

#define PM_SEGMENT              (37) /* 0x25 */
#define PM_DATA                 (0x02)
#define PM_ICON                 (0x2502)

#define SUNDAY_SEGMENT          (11) /* 0x0b */
#define SUNDAY_DATA             (0x01)
#define SUNDAY_ICON             (0x0b01)

#define MONDAY_SEGMENT          (14) /* 0x0e */
#define MONDAY_DATA             (0x08)
#define MONDAY_ICON             (0x0e08)

#define TUESDAY_SEGMENT         (14) /* 0x0e */
#define TUESDAY_DATA            (0x04)
#define TUESDAY_ICON            (0x0e04)

#define WEDNESDAY_SEGMENT       (9) /* 0x09 */
#define WEDNESDAY_DATA          (0x01)
#define WEDNESDAY_ICON          (0x0901)

#define THURSDAY_SEGMENT        (7) /* 0x07 */
#define THURSDAY_DATA           (0x01)
#define THURSDAY_ICON           (0x0701)

#define FRIDAY_SEGMENT          (35) /* 0x23 */
#define FRIDAY_DATA             (0x08)
#define FRIDAY_ICON             (0x2308)

#define SATURDAY_SEGMENT        (35) /* 0x23 */
#define SATURDAY_DATA           (0x04)
#define SATURDAY_ICON           (0x2304)

#define BIG_DIGIT_MINUS_SEGMENT         (31) /* 0x1f */
#define BIG_DIGIT_MINUS_DATA            (0x08)
#define BIG_DIGIT_MINUS_ICON            (0x1f08)

#define BIG_DIGIT_DEC_POINT_SEGMENT     (19) /* 0x13 */
#define BIG_DIGIT_DEC_POINT_DATA        (0x08)
#define BIG_DIGIT_DEC_POINT_ICON        (0x1308)


#define MEDIUM_DIGIT_DEC_POINT_SEGMENT  (14) /* 0x0e */
#define MEDIUM_DIGIT_DEC_POINT_DATA     (0x01)
#define MEDIUM_DIGIT_DEC_POINT_ICON     (0x0e01)


#define MEDIUM_DIGIT_COLON_SEGMENT      (14) /* 0x0e */
#define MEDIUM_DIGIT_COLON_DATA         (0x02)
#define MEDIUM_DIGIT_COLON_ICON         (0x0e02)

#define BIG_DIGIT_MASK_4_2_1            (0x07)
#define BIG_DIGIT_MASK_8_4_2_1          (0x0f)

#define MED_DIGIT_MASK_8_4_2_1          (0x0f)
#define MED_DIGIT_MASK_8_4_2            (0x0e)
typedef struct st_icon_segment
{
    uint8_t segment;
    uint8_t data;
}st_icon_segment_t;


/**********************************************************************************************************************
 * Function Name: slcdc_init
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
bool_t slcdc_init (void);

/**********************************************************************************************************************
 * Function Name: slcd_start
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
bool_t slcd_start (void);

/**********************************************************************************************************************
 * Function Name: slcd_stop
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
bool_t slcd_stop (void);

/**********************************************************************************************************************
 * Function Name: set_segments_medium_digits
 * Description  : .
 * Arguments    : value
 *              : point_or_colon
 * Return Value : .
 *********************************************************************************************************************/
fsp_err_t set_segments_medium_digits (uint32_t value, e_medium_dp_or_colon_t point_or_colon);

/**********************************************************************************************************************
 * Function Name: set_segments_icon_medium_dp_colon
 * Description  : .
 * Argument     : setting
 * Return Value : .
 *********************************************************************************************************************/
fsp_err_t set_segments_icon_medium_dp_colon (e_medium_dp_or_colon_t setting);

/**********************************************************************************************************************
 * Function Name: set_segments_big_digits
 * Description  : .
 * Arguments    : value
 *              : dp_setting
 * Return Value : .
 *********************************************************************************************************************/
fsp_err_t set_segments_big_digits (int16_t value, e_big_digit_dp_t dp_setting);

/**********************************************************************************************************************
 * Function Name: set_segments_big_extras
 * Description  : .
 * Arguments    : use_minus
 *              : dp_setting
 * Return Value : .
 *********************************************************************************************************************/
fsp_err_t set_segments_big_extras (bool use_minus, e_big_digit_dp_t dp_setting);


/**********************************************************************************************************************
 * Function Name: set_segments_icon_dow
 * Description  : .
 * Argument     : day_of_week
 * Return Value : .
 *********************************************************************************************************************/
fsp_err_t set_segments_icon_dow (e_day_of_week_t day_of_week);

/**********************************************************************************************************************
 * Function Name: set_segments_icon_battery
 * Description  : .
 * Argument     : level
 * Return Value : .
 *********************************************************************************************************************/
fsp_err_t set_segments_icon_battery (e_battery_level_t level);

/**********************************************************************************************************************
 * Function Name: set_segments_icon_am_pm
 * Description  : .
 * Argument     : setting
 * Return Value : .
 *********************************************************************************************************************/
fsp_err_t set_segments_icon_am_pm (e_am_or_pm_t setting);

/**********************************************************************************************************************
 * Function Name: set_segments_icon_degC_degF
 * Description  : .
 * Argument     : setting
 * Return Value : .
 *********************************************************************************************************************/
fsp_err_t set_segments_icon_degC_degF (e_degc_or_degf_t setting);

/**********************************************************************************************************************
 * Function Name: set_segment_icon_named
 * Description  : .
 * Arguments    : segment_name
 *              : setting
 * Return Value : .
 *********************************************************************************************************************/
fsp_err_t set_segment_icon_named (uint16_t segment_name, e_on_or_off_t setting);

/**********************************************************************************************************************
 * Function Name: set_segment_icon_alarm
 * Description  : .
 * Argument     : setting
 * Return Value : .
 *********************************************************************************************************************/
fsp_err_t set_segment_icon_alarm (e_on_or_off_t setting);


#endif /* SLCD_H_ */
