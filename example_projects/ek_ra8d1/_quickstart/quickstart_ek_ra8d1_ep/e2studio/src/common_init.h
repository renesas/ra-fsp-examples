/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/**********************************************************************************************************************
 * File Name    : common_init.h
 * Description  : Common init functions.
 *********************************************************************************************************************/

#include <stdint.h>
#include <stdbool.h>

#include "hal_data.h"
#include "board_cfg.h"
#include "common_utils.h"

#ifndef COMMON_INIT_H_
 #define COMMON_INIT_H_

 #define SHORT_NAME                       ("RA8D1")
 #define FULL_NAME                        ("EK-RA8D1")
 #define PART_NUMBER                      ("RTK7EKA8D1S01001BE")
 #define DEVICE_NUMBER                    ("R7FA8D1BHECBD")

 #define KIT_NAME                         ("EK-RA8D1")
 #define EP_VERSION                       ("0.01")

 #define MAX_DETECTION_NUMS               (5)

 #define LED_INTENSITY_10                 (10)       /* 10 percent */
 #define LED_INTENSITY_50                 (50)       /* 50 percent */
 #define LED_INTENSITY_90                 (90)       /* 90 percent */

 #define BLINK_FREQ_1HZ                   (60000000) /* 1HZ */
 #define BLINK_FREQ_5HZ                   (12000000) /* 5HZ */
 #define BLINK_FREQ_10HZ                  (6000000)  /* 10HZ */

 #define NUM_STRING_DESCRIPTOR            (7U)

/* DISABLE RTT I/F */
 #define SEGGER_INDEX                     0
 #define APP_PRINT(fn_, ...)        ()
 #define APP_ERR_PRINT(fn_, ...)    ()
 #define APP_ERR_TRAP(err)          ()
 #define APP_READ(read_data)        ()
 #define APP_CHECK_DATA                   ()

/* g_update_console_event */
 #define STATUS_DISPLAY_MENU_KIS          (1 << 0)  /* Update Kit Information Screen EVENT */
 #define STATUS_UPDATE_KIS_INFO           (1 << 1)  /* Update Kit Information Screen data EVENT */
 #define STATUS_UPDATE_TEMP_INFO          (1 << 2)  /* Update Kit Temperature EVENT */
 #define STATUS_UPDATE_FREQ_INFO          (1 << 3)  /* Update Kit Blue LED Frequency EVENT */
 #define STATUS_UPDATE_INTENSE_INFO       (1 << 4)  /* Update Kit Blue LED Intensity EVENT */

 #define STATUS_HMI_NEXT_DEMO             (1 << 6)  /* NEXT SCREEN demo */
 #define STATUS_HMI_PREV_DEMO             (1 << 7)  /* PREVIOUS demo */

 #if 0
  #define STATUS_ENABLE_IOT_DEMO          (1 << 8)  /* ENABLE IOT demo */
  #define STATUS_DISABLE_IOT_DEMO         (1 << 9)  /* DISABLE IOT demo */

  #define STATUS_START_ENABLE_AI_DEMO     (1 << 10) /* Start ai demo*/
  #define STATUS_END_ENABLE_AI_DEMO       (1 << 11) /* Complete ai demo */

 #else
  #define STATUS_IOT_REQUEST_WEATHER      (1 << 8)  /*  */
  #define STATUS_IOT_REQUEST_CURRENCY     (1 << 9)  /*  */
  #define STATUS_IOT_REQUEST_TIME         (1 << 10) /*  */
  #define STATUS_IOT_RESPONSE_COMPLETE    (1 << 11) /*  */
 #endif

 #define STATUS_ENABLE_ETHERNET           (1 << 12) /* Enable Ether Thread to connect (on required once) */
 #define STATUS_ETHERNET_LINKUP           (1 << 13) /* Ethernet is UP / Down */
 #define STATUS_CONSOLE_AVAILABLE         (1 << 14) /* UART Console has initiated and is ready for I/O */
 #define STATUS_IOT_RESPONSE_CANCEL       (1 << 15) /*  */

 #define MENU_RETURN_INFO                 "\r\n\r\n> Press space bar to return to MENU.\r\n"

 #define TMR_DEF_QSEP_BLINKY_BLUE         (0)
 #define TMR_DEF_LED_UX_BLINK_RED         (1)
 #define TMR_DEF_LED_UX_BLINK_GREEN       (2)
 #define TMR_DEF_LED_UX_BLINK_BLUE        (3)

/* Unsigned equiv of -1 */
 #define INVALID_CHARACTER                (0xFFFFFFFF)

/* Unsigned equiv of -2 */
 #define INVALID_BLOCK_SIZE               (0xFFFFFFFE)

/* Unsigned equiv of -3 */
 #define INVALID_BLOCK_BOUNDARY           (0xFFFFFFFD)

 #define INVALID_MARKERS                  (0xFFFFFFF0)

 #define MAX_MENU_STR_LEN                 (256)

 #define MAX_OPTION_STR_LEN               (32)

/* Macro definitions */
 #ifndef MIN

/* Macro contains multiple references to parameters, but only once in the comparison, so safe use. */
  #define MIN(a, b)    (((a) < (b)) ? (a) : (b))
 #endif

/* The ADC slope, for 12bit at a full scale of 3.3v
 * Cal is given for +127degC. So slope*127 (rounded to integer) is the zero cal offset
 * Deg F conversion used: (C * (9/5)) +32
 * (Don't use the R_ADC_InfoGet .slope_microvolts, as this is inaccurate according to data supplied.
 *  Use slope=4.1mV/DegC)
 */
 #define TSN_ADC_COVERSION_SLOPE_COUNTS_PER_DEG_C       (5.08896f)
 #define TSN_CAL_OFFEST_COUNTS_AT_127DEG_TO_0DEG_C      (646)

 #define TSN_ADC_COVERSION_SLOPE_COUNTS_PER_DEG_F       (2.8272f)
 #define TSN_CAL_OFFEST_COUNTS_AT_260_6DEG_TO_0DEG_F    (737)

typedef struct ai_detection_point_t
{
    signed short m_x;
    signed short m_y;
    signed short m_w;
    signed short m_h;
} st_ai_detection_point_t;

typedef struct ai_classification_point_t
{
    unsigned short category;
    float          prob;
} st_ai_classification_point_t;

typedef struct
{
    uint16_t whole_number;             // integer part of temperature
    uint16_t mantissa;                 // decimal part of temperature
} st_temp_expression_t;

typedef struct
{
    uint16_t             adc_temperature_data; // temperature (un-calibrated data)
    st_temp_expression_t temperature_f;        // temperature (fahrenheit)
    st_temp_expression_t temperature_c;        // temperature (celsius)
    float32_t            temperature_f_as_f;   // temperature (fahrenheit) as float
    float32_t            temperature_c_as_f;   // temperature (celsius) as float
    uint16_t             led_intensity;        // PWM pulse width
    uint16_t             led_frequency;        // PWM pulse frequency
} st_board_status_t;

typedef enum test_status
{
    TEST_DISABLED = 0,                 ///< TEST_DISABLED
    TEST_ENABLED,                      ///< TEST_ENABLED
    TEST_TODO,                         ///< TEST_TODO
    TEST_LATCH_DISABLED,               ///< TEST_DISABLED
    TEST_LATCH_ENABLED,                ///< TEST_ENABLED
} e_test_status_t;

typedef enum e_hp_data_flash_iot_keys
{
    IOT_KEY_NONE = 0,
    IOT_KEY_CURRENCY,
    IOT_KEY_WEATHER
} e_hp_data_flash_iot_keys_t;

typedef enum pmod_reset_pin
{
    PMOD_RESET_LOW  = 0,
    PMOD_RESET_HIGH = 1
} e_pmod_reset_pin_t;

typedef enum test_mode
{
    UNINITIALISED = 0,
    STARTING,
    RUNNING,
    STOPPING,
    HALTED
} e_test_mode_t;

typedef enum e_https_certificate
{
// API_CERTIFICATE_DISABLED   = -1,
    API_CERTIFICATE_CURRENCYAPI = 0,
    API_CERTIFICATE_WEATHERAPI
} e_https_certificate_t;

extern e_https_certificate_t g_https_certificate;

typedef struct st_emc_menu_status
{
    e_test_status_t state;
    e_test_mode_t   activation_mode;
    uint32_t        internal;
} menu_settings_t;

typedef struct irq_pins
{
    const external_irq_instance_t * const p_irq;
} st_irq_pins_t;

typedef struct pwm_pins
{
    const timer_instance_t * const p_timer;
    const gpt_io_pin_t             pin;
} st_pwm_pins_t;

extern const char_t * const gp_cursor_store;
extern const char_t * const gp_cursor_restore;
extern const char_t * const gp_cursor_temp;
extern const char_t * const gp_cursor_frequency;
extern const char_t * const gp_cursor_intensity;

extern const char_t * const gp_red_fg;
extern const char_t * const gp_orange_fg;
extern const char_t * const gp_green_fg;
extern const char_t * const gp_white_fg;

extern const char_t * const gp_clear_screen;
extern const char_t * const gp_cursor_home;

extern st_pwm_pins_t g_pwm_pins[];

extern uint8_t g_pwm_dcs_data[];
extern uint8_t g_pwm_rates_data[];

extern uint32_t g_pwm_dcs[3];
extern uint32_t g_pwm_rates[3];

extern bool_t do_detection;
extern bool_t do_classification;

extern int8_t            g_selected_menu;
extern bsp_leds_t        g_bsp_leds;
extern st_board_status_t g_board_status;

extern fsp_err_t common_init(void);
extern fsp_err_t print_to_console(char_t * p_data);
extern int8_t    input_from_console(void);
extern int8_t  * input_from_console_multi(void);
extern void      led_duty_cycle_update();
extern float64_t currency_get_rate(char_t * table, char_t request[3], char_t * _pres);
extern bool_t    json_get_data_from_key(char_t * table, char_t * timestring, char_t * _key, char_t * _pres);
extern bool_t    initialise_simulated_data_in_sdram(void);

#endif                                 /* COMMON_INIT_H_ */
