/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : slcd.h
 * Version      : 1.0
 * Description  : SLCD Configuration and controls.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 20.05.2025 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "hal_data.h"
#include "r_typedefs.h"

 /**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#ifndef SLCD_H_
#define SLCD_H_

/* Pin 32 SEG32 */
#define HEAT_SEGMENT            (32)
#define HEAT_DATA               (0x01)
#define HEAT_ICON               (0x2001)

/* Pin 28 SEG14 */
#define COOL_SEGMENT            (14)
#define COOL_DATA               (0x01)
#define COOL_ICON               (0x0E01)

/* Pin 20 SEG37 */
#define FAN_SEGMENT             (37)
#define FAN_DATA                (0x01)
#define FAN_ICON                (0x2501)

/* Pin 22 SEG35 */
#define ZONE_SEGMENT            (35)
#define ZONE_DATA               (0x01)
#define ZONE_ICON               (0x2301)

/* Pin 26 SEG22 */
#define HEART_SEGMENT           (22)
#define HEART_DATA              (0x01)
#define HEART_ICON              (0x1601)

/* Pin24 SEG21 */
#define VOLTS_SEGMENT           (21)
#define VOLTS_DATA              (0x01)
#define VOLTS_ICON              (0x1501)

/* Pin 39 SEG17 */
#define RENESAS_SEGMENT         (17)
#define RENESAS_DATA            (0x08)
#define RENESAS_ICON            (0x1108)

/* Pin 50 SEG31 */
#define ALARM_SEGMENT           (31)
#define ALARM_DATA              (0x02)
#define ALARM_ICON              (0x1F02)

/* Pin 47 SEG34 */
#define DEG_C_SEGMENT           (34)
#define DEG_C_DATA              (0x04)
#define DEG_C_ICON              (0x2204)

/* Pin 47 SEG34 */
#define DEG_F_SEGMENT           (34)
#define DEG_F_DATA              (0x08)
#define DEG_F_ICON              (0x2208)

/* Pin 47 SEG34 */
#define BATTERY_SHELL_SEGMENT   (34)
#define BATTERY_SHELL_DATA      (0x01)
#define BATTERY_SHELL_ICON      (0x2201)

/* Pin 49 SEG33 */
#define BATTERY_LEVEL1_SEGMENT  (33)
#define BATTERY_LEVEL1_DATA     (0x08)
#define BATTERY_LEVEL1_ICON     (0x2108)

/* Pin 49 SEG33 */
#define BATTERY_LEVEL2_SEGMENT  (33)
#define BATTERY_LEVEL2_DATA     (0x04)
#define BATTERY_LEVEL2_ICON     (0x2104)

/* Pin 49 SEG33 */
#define BATTERY_LEVEL3_SEGMENT  (33)
#define BATTERY_LEVEL3_DATA     (0x02)
#define BATTERY_LEVEL3_ICON     (0x2102)

/* Pin 49 SEG33 */
#define BATTERY_LEVEL4_SEGMENT  (33)
#define BATTERY_LEVEL4_DATA     (0x01)
#define BATTERY_LEVEL4_ICON     (0x2101)

/* Pin 50 SEG31 */
#define AM_SEGMENT              (31)
#define AM_DATA                 (0x01)
#define AM_ICON                 (0x1F01)

/* Pin 47 SEG34 */
#define PM_SEGMENT              (34)
#define PM_DATA                 (0x02)
#define PM_ICON                 (0x2202)

/* Pin 36 SEG19 */
#define SUNDAY_SEGMENT          (19)
#define SUNDAY_DATA             (0x01)
#define SUNDAY_ICON             (0x1301)

/* Pin 40 SEG10 */
#define MONDAY_SEGMENT          (10)
#define MONDAY_DATA             (0x08)
#define MONDAY_ICON             (0x0A08)

/* Pin 40 SEG10 */
#define TUESDAY_SEGMENT         (10)
#define TUESDAY_DATA            (0x04)
#define TUESDAY_ICON            (0x0A04)

/* Pin 42 SEG24 */
#define WEDNESDAY_SEGMENT       (24)
#define WEDNESDAY_DATA          (0x01)
#define WEDNESDAY_ICON          (0x1801)

/* Pin 46 SEG12 */
#define THURSDAY_SEGMENT        (12)
#define THURSDAY_DATA           (0x01)
#define THURSDAY_ICON           (0x0C01)

/* Pin 50 SEG31 */
#define FRIDAY_SEGMENT          (31)
#define FRIDAY_DATA             (0x08)
#define FRIDAY_ICON             (0x1F08)

/* Pin 50 SEG31 */
#define SATURDAY_SEGMENT        (31)
#define SATURDAY_DATA           (0x04)
#define SATURDAY_ICON           (0x1F04)

/* Pin 35 SEG27 */
#define BIG_DIGIT_MINUS_SEGMENT         (27)
#define BIG_DIGIT_MINUS_DATA            (0x08)
#define BIG_DIGIT_MINUS_ICON            (0x1808)

/* Pin 40 SEG10 */
#define MEDIUM_DIGIT_DEC_POINT_SEGMENT  (10)
#define MEDIUM_DIGIT_DEC_POINT_DATA     (0x01)
#define MEDIUM_DIGIT_DEC_POINT_ICON     (0x0A01)

/* Pin 40 SEG10*/
#define MEDIUM_DIGIT_COLON_SEGMENT      (10)
#define MEDIUM_DIGIT_COLON_DATA         (0x02)
#define MEDIUM_DIGIT_COLON_ICON         (0x0A02)

#define MED_DIGIT_MASK_8_4_2_1          (0x0f)
#define MED_DIGIT_MASK_8_4_2            (0x0e)

/* Pin 43 SEG15*/
#define BIG_DIGIT_DEC_POINT_SEGMENT     (15)
#define BIG_DIGIT_DEC_POINT_DATA        (0x08)
#define BIG_DIGIT_DEC_POINT_ICON        (0x0F08)

#define BIG_DIGIT_MASK_4_2_1            (0x07)
#define BIG_DIGIT_MASK_8_4_2_1          (0x0f)

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/
typedef enum e_day_of_week
{
    DOW_SUNDAY    = 0x01,
    DOW_MONDAY    = 0x02,
    DOW_TUESDAY   = 0x04,
    DOW_WEDNESDAY = 0x08,
    DOW_THURSDAY  = 0x10,
    DOW_FRIDAY    = 0x20,
    DOW_SATURDAY  = 0x40,
    DOW_OFF       = 0x80
} e_day_of_week_t;

typedef enum e_battery_level
{
    BATTERY_EMPTY   = 0,
    BATTERY_0       = 0,
    BATTERY_1,
    BATTERY_2,
    BATTERY_3,
    BATTERY_FULL    = 4,
    BATTERY_4       = 4,
    BATTERY_OFF
} e_battery_level_t;

typedef enum e_am_or_pm
{
    AM_TIME,
    PM_TIME,
    AM_PM_OFF
} e_am_or_pm_t;

typedef enum e_degc_or_degf
{
    TEMP_DEG_C,
    TEMP_DEG_F,
    DEG_C_F_OFF
} e_degc_or_degf_t;

typedef enum e_medium_dp_or_colon
{
    SET_DP,
    SET_COLON,
    DP_COLON_OFF
} e_medium_dp_or_colon_t;

typedef enum e_on_or_off
{
    ICON_ON,
    ICON_OFF
} e_on_or_off_t;

typedef enum e_big_digit_dp
{
    WITH_DP,
    NO_DP
} e_big_digit_dp_t;

typedef enum e_big_number_integer_units
{
    BN_UNITS,
    BN_TENS,
    BN_HUNDREDS
} e_big_number_integer_units_t;

typedef enum e_medium_number_units
{
    MN_UNITS,
    MN_TENS,
    MN_HUNDREDS,
    MN_THOUSANDS,
    MN_TEN_THOUSANDS
} e_medium_number_units_t;

typedef struct st_icon_segment
{
    uint8_t segment;
    uint8_t data;
} st_icon_segment_t;

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/
 
bool_t slcdc_init (void);
bool_t slcd_start (void);
bool_t slcd_stop (void);
fsp_err_t set_segments_medium_digits        (uint32_t value, e_medium_dp_or_colon_t point_or_colon);
fsp_err_t set_segments_icon_medium_dp_colon (e_medium_dp_or_colon_t setting);
fsp_err_t set_segments_big_digits           (int16_t value, e_big_digit_dp_t dp_setting);
fsp_err_t set_segments_big_extras           (bool use_minus, e_big_digit_dp_t dp_setting);
fsp_err_t set_segment_icon_named            (uint16_t segment_name, e_on_or_off_t setting);


#endif /* SLCD_H_ */
