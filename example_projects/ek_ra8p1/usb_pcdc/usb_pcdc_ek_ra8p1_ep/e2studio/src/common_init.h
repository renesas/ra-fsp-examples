/***********************************************************************************************************************
 * File Name    : common_init.h
 * Description  : Contains macros, variables, and functions used in common_init.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef COMMON_INIT_H_
#define COMMON_INIT_H_

#include <stdbool.h>
#include "hal_data.h"

/* Macros for the LED intensity (PWM duty cycle values) */
#define LED_INTENSITY_10                    (0x3E7U)        /* 10 percent */
#define LED_INTENSITY_50                    (0x1388U)       /* 50 percent */
#define LED_INTENSITY_90                    (0x2328U)       /* 90 percent */

/* Macros for the LED blinking frequency (GPT period values) */
#define BLINK_FREQ_1HZ                      (150000000U)
#define BLINK_FREQ_5HZ                      (30000000U)
#define BLINK_FREQ_10HZ                     (15000000U)

/* Macros for the ADC based on the specific version */
#if (BSP_PERIPHERAL_ADC_B_PRESENT)
    #define ADC_CHANNEL_CONFIG              (g_adc_scan_cfg)
    #define ADC_OPEN(ctrl, cfg)             (R_ADC_B_Open((ctrl), (cfg)))
    #define ADC_SCANCFG(ctrl, ch_cfg)       (R_ADC_B_ScanCfg((ctrl), (ch_cfg)))
    #define ADC_SCANSTART(ctrl)             (R_ADC_B_ScanStart((ctrl)))
    #define ADC_READ(ctrl, ch, p_data)      (R_ADC_B_Read((ctrl), (ch), (p_data)))
    #define ADC_CALIBRATE(ctrl)             (R_ADC_B_Calibrate((ctrl), NULL))
    #define ADC_STATUSGET(ctrl, p_status)   (R_ADC_B_StatusGet((ctrl), (p_status)))
#else
    #define ADC_CHANNEL_CONFIG              (g_adc_channel_cfg)
    #define ADC_OPEN(ctrl, cfg)             (R_ADC_Open((ctrl), (cfg)))
    #define ADC_SCANCFG(ctrl, ch_cfg)       (R_ADC_ScanCfg((ctrl), (ch_cfg)))
    #define ADC_SCANSTART(ctrl)             (R_ADC_ScanStart((ctrl)))
    #define ADC_READ(ctrl, ch, p_data)      (R_ADC_Read((ctrl), (ch), (p_data)))
    #define ADC_CALIBRATE(ctrl)             (R_ADC_Calibrate((ctrl), NULL))
    #define ADC_STATUSGET(ctrl, p_status)   (R_ADC_StatusGet((ctrl), (p_status)))
#endif /* BSP_PERIPHERAL_ADC_B_PRESENT */

/* Macro for the number of LED blinking frequencies */
#define NUM_RATES                           (sizeof(s_pwm_rates) / sizeof(s_pwm_rates[0]))

/* Macro for the number of LED intensities */
#define NUM_DCS                             (sizeof(s_pwm_dcs) / sizeof(s_pwm_dcs[0]))

/* Macro for the number of user buttons */
#define NUM_SWITCH                          (sizeof(s_irq_pins) / sizeof(s_irq_pins[0]))

/* External variable - Defined in common_init.c */
extern volatile uint32_t g_curr_led_freq;

/* Private variables */
static const struct
{
    const external_irq_instance_t * const p_irq;
} s_irq_pins[] =
{
    {&g_external_irq12},
    {&g_external_irq13},
};

static const struct
{
    const timer_instance_t * const p_timer;
    const gpt_io_pin_t pin;
} s_pwm_pins[] =
{
    {&g_gpt_blue, GPT_IO_PIN_GTIOCB},
    {&g_gpt_red_and_green, GPT_IO_PIN_GTIOCA_AND_GTIOCB}
};

/* Function declarations */
fsp_err_t common_init(void);
fsp_err_t adc_temperature_get(uint16_t * p_adc_data);
#if (BSP_PERIPHERAL_ADC_B_PRESENT)
fsp_err_t adc_start_calibration(void);
#endif /* BSP_PERIPHERAL_ADC_B_PRESENT */

#endif /* COMMON_INIT_H_ */
