/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/ 
/**********************************************************************************************************************
 * File Name    : common_init.h
 * Description  : Common init functions.
 *********************************************************************************************************************/
#ifndef COMMON_INIT_H_
#define COMMON_INIT_H_

#include <stdint.h>
#include <stdbool.h>
#include "hal_data.h"
#include "r_typedefs.h"
#include "common_utils.h"
#include "common_utils.h"



#define SHORT_NAME      "RA6M5"
#define FULL_NAME       ("CK-RA6M5v2")
#define PART_NUMBER     ("RTK7CKA6M5S08002BE")
#define DEVICE_NUMBER   ("R7FA6M5BH3CFC")

#define LED_INTENSITY_10      (10)            /* 10 percent */
#define LED_INTENSITY_50      (50)            /* 50 percent */
#define LED_INTENSITY_90      (90)            /* 90 percent */


#define BLINK_FREQ_1HZ        (60000000)      /* 1HZ */
#define BLINK_FREQ_5HZ        (12000000)      /* 5HZ */
#define BLINK_FREQ_10HZ       (6000000)       /* 10HZ */

#define NUM_STRING_DESCRIPTOR               (7U)

/* g_update_console_event */
#define STATUS_DISPLAY_MENU_KIS     (1 << 0)    /* Update Kit Information Screen EVENT */
#define STATUS_UPDATE_KIS_INFO      (1 << 1)    /* Update Kit Information Screen data EVENT */
#define STATUS_UPDATE_TEMP_INFO     (1 << 2)    /* Update Kit Temperature EVENT */
#define STATUS_UPDATE_FREQ_INFO     (1 << 3)    /* Update Kit Blue LED Frequency EVENT */
#define STATUS_ENABLE_ETHERNET      (1 << 12)   /* Enable Ether Thread to connect (on required once) */
#define STATUS_ETHERNET_LINKUP      (1 << 13)   /* Ethernet is UP / Down */

/* Unsigned equiv of -1 */
#define INVALID_CHARACTER        (0xFFFFFFFF)

/* Unsigned equiv of -2 */
#define INVALID_BLOCK_SIZE       (0xFFFFFFFE)

/* Unsigned equiv of -3 */
#define INVALID_BLOCK_BOUNDARY   (0xFFFFFFFD)

#define INVALID_MARKERS          (0xFFFFFFF0)

/* Macro definitions */
#ifndef MIN

/* Macro contains multiple references to parameters, but only once in the comparison, so safe use. */
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif


/* The ADC slope, for 12bit at a full scale of 3.3v
 * Cal is given for +127degC. So slope*127 (rounded to integer) is the zero cal offset
 * Deg F conversion used: (C * (9/5)) +32
 * (Don't use the R_ADC_InfoGet .slope_microvolts, as this is inaccurate according to data supplied.
 *  Use slope=4.1mV/DegC)
 */
#define TSN_ADC_COVERSION_SLOPE_COUNTS_PER_DEG_C            (5.08896f)
#define TSN_CAL_OFFEST_COUNTS_AT_127DEG_TO_0DEG_C           (646)

#define TSN_ADC_COVERSION_SLOPE_COUNTS_PER_DEG_F            (2.8272f)
#define TSN_CAL_OFFEST_COUNTS_AT_260_6DEG_TO_0DEG_F         (737)

typedef struct
{
    uint16_t whole_number;                      // integer part of temperature
    uint16_t mantissa;                          // decimal part of temperature
} st_temp_expression_t;

typedef struct
{
    uint16_t adc_temperature_data;  // temperature (un-calibrated data)
    st_temp_expression_t temperature_f;         // temperature (fahrenheit)
    st_temp_expression_t temperature_c;         // temperature (celsius)
    uint16_t led_intensity;         // PWM pulse width
    uint16_t led_frequency;         // PWM pulse frequency
} st_board_status_t;

typedef struct sensorsOB_t {

    uint16_t    ob_spo2;  ///< spo2
    uint16_t    ob_hr;    ///< Heart rate
    uint16_t    ob_rr;
    float       ob_pi;
    bool_t      calibrated;
    bool_t      sensing;
} st_sensorsOB_t;

typedef struct
{
    double x;
    double y;
    double z;
} axises;

typedef struct st_icm20948_sensor
{
    bool_t               icm20948_inuse;
    bool_t               available;
    uint32_t             wait_in_ms;
    axises               my_gyro;
    axises               my_accel;
    axises               my_mag;

} st_icm20948_sensor_t;

typedef struct zmod4410_data_t {
    float     iaq;       ///< iaq
    float     ec02;       ///< eco2
    float     tvoc;       ///< tvoc
    float     etoh;       ///< etoh
} st_zmod4410_data_t;


typedef struct sensorsHS_t {

    float                  temperature; ///< Temperature
    float                  humidity;    ///< Humidity
} st_sensorsHS_t;

typedef struct st_hs_300x_sensor
{
    bool_t               hs300x_callback;
    rm_hs300x_data_t     hs300x_data;
} st_hs_300x_sensor_t;

typedef struct sensorsICP_t {

    float                  temperatureicp; ///< Temperature
    float                  pressureicp;    ///< Pressure
} st_sensorsICP_t;


typedef struct sensorsZmod4410_t
{
    st_zmod4410_data_t  zmod4410Data;
    bool_t              calibrated;
}st_sensorsZmod4410_t;

typedef struct sensorsZmod4510_t
{
    float               zmod410_oaq;
    bool_t              calibrated;
}st_sensorsZmod4510_t;

typedef struct st_sensor_data
{
    bool_t               refresh_console;
    st_icm20948_sensor_t   icm20948;
    st_hs_300x_sensor_t    hs300x;
    st_sensorsOB_t         ob1203;
    st_sensorsICP_t        icp10101;
    st_sensorsZmod4410_t   zmod4410;
    st_sensorsZmod4510_t   zmod4510;
} st_sensor_data_t;


extern uint8_t g_pwm_dcs_data[];
extern uint8_t g_pwm_rates_data[];

extern uint32_t g_pwm_dcs[3];
extern uint32_t g_pwm_rates[3];

extern int8_t g_selected_menu;
extern st_board_status_t g_board_status;

extern fsp_err_t common_init (void);
extern fsp_err_t print_to_console (uint8_t * p_data);
extern int8_t input_from_console (void);
extern void led_duty_cycle_update ();

#endif /* COMMON_INIT_H_ */
