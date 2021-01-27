/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name    : common_init.h
 * Description  : Common init functions.
 **********************************************************************************************************************/

#include "hal_data.h"
#include "board_cfg.h"

#ifndef COMMON_INIT_H_
#define COMMON_INIT_H_

/* PWM ramp values */
#define LED_INTENSITY_10      (0x3E7)         /* 10 percent */
#define LED_INTENSITY_50      (0x1388)        /* 50 percent */
#define LED_INTENSITY_90      (0x2328)        /* 90 percent */

/* Blink timer values */
#define BLINK_FREQ_1HZ        (60000000)      /* 1Hz */
#define BLINK_FREQ_5HZ        (12000000)      /* 5Hz */
#define BLINK_FREQ_10HZ       (6000000)       /* 10Hz */

#define NUM_STRING_DESCRIPTOR               (7U)

/* g_update_console_event */
#define STATUS_DISPLAY_MENU_KIS     (1 << 0)    /* Update Kit Information Screen EVENT */
#define STATUS_UPDATE_KIS_INFO      (1 << 1)    /* Update Kit Information Screen data EVENT */
#define STATUS_UPDATE_TEMP_INFO     (1 << 2)    /* Update Kit Temperature EVENT */
#define STATUS_UPDATE_FREQ_INFO     (1 << 3)    /* Update Kit Blue LED Frequency EVENT */
#define STATUS_UPDATE_INTENSE_INFO  (1 << 4)    /* Update Kit Blue LED Intensity EVENT */
#define STATUS_WRITE_COMPLETE       (1 << 5)    /* Update USB Write EVENT */
#define STATUS_USB_EVENT            (1 << 6)    /* Update USB EVENT */
#define STATUS_ENABLE_ETHERNET      (1 << 7)    /* Enable Ether Thread to conect (on required once) */
#define STATUS_ETHERNET_LINKUP      (1 << 8)    /* Ethernet is UP / Down */

#define MENU_RETURN_INFO  "\r\n\r\n> Press space bar to return to MENU\r\n"

/* Macro definitions */
#ifndef MIN
/* Select the min of two values */
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

#define APP_ERR_TRAP(a)             if(a) {__asm("BKPT #0\n");} /* trap the error location */


/* The ADC slope, for 12bit at a full scale of 3.3v
* Cal is given for +127degC. So slope*127 (rounded to integer) is the zero cal offset
* Deg F conversion used: (C * (9/5)) +32
* (Don't use the R_ADC_InfoGet .slope_microvolts, as this is inaccurate according to data supplied.
* Use slope=4.1mV/DegC)
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
    uint16_t             adc_temperature_data;  // temperature (un-calibrated data)
    st_temp_expression_t temperature_f;         // temperature (fahrenheit)
    st_temp_expression_t temperature_c;         // temperature (celsius)

    uint16_t             led_intensity;         // PWM pulse width
    uint16_t             led_frequency;         // PWM pulse frequency
} st_board_status_t;

extern char_t g_pwm_dcs_data[];
extern char_t g_pwm_rates_data[];

extern uint32_t g_pwm_dcs[3];
extern uint32_t g_pwm_rates[3];


extern int8_t g_selected_menu;
extern bsp_leds_t g_bsp_leds;
extern st_board_status_t g_board_status;


extern fsp_err_t common_init (void);
extern fsp_err_t print_to_console (char_t * p_data);
extern int8_t input_from_console (void);
extern void led_duty_cycle_update ();

#endif /* COMMON_INIT_H_ */
