/***********************************************************************************************************************
 * File Name    : common_init.h
 * Description  : Common init functions.
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "hal_data.h"
#include "board_cfg.h"

#ifndef COMMON_INIT_H_
#define COMMON_INIT_H_


#define SHORT_NAME      ("RA2E2")
#define FULL_NAME       ("EK-RA2E2")
#define PART_NUMBER     ("RTK7EKA2E2S00001BE")
#define DEVICE_NUMBER   ("R7FA2E2A72DNK")


#define LED_INTENSITY_10      (10)            /* 10 percent */
#define LED_INTENSITY_50      (50)            /* 50 percent */
#define LED_INTENSITY_90      (90)            /* 90 percent */

#define BLINK_FREQ_1HZ        (12000000)      /*  1MHZ */
#define BLINK_FREQ_5HZ        (2400000)       /*  5MHZ */
#define BLINK_FREQ_10HZ       (1200000)       /* 10MHZ */

#define NUM_STRING_DESCRIPTOR               (7U)

/* TSN conversion values */
#define TSN_ADC_COVERSION_SLOPE_COUNTS_PER_DEG_C            (-4.094f)
#define TSN_CAL_OFFEST_COUNTS_AT_125DEG_TO_0DEG_C           (-512)

#define TSN_ADC_COVERSION_SLOPE_COUNTS_PER_DEG_F            (-2.275f)
#define TSN_CAL_OFFEST_COUNTS_AT_257DEG_TO_0DEG_F           (-585)


/* g_update_console_event */
#define STATUS_DISPLAY_MENU_KIS     (1 << 0)    /* Update Kit Information Screen EVENT */
#define STATUS_UPDATE_KIS_INFO      (1 << 1)    /* Update Kit Information Screen data EVENT */
#define STATUS_UPDATE_TEMP_INFO     (1 << 2)    /* Update Kit Temperature EVENT */
#define STATUS_UPDATE_FREQ_INFO     (1 << 3)    /* Update Kit Blue LED Frequency EVENT */
#define STATUS_UPDATE_INTENSE_INFO  (1 << 4)    /* Update Kit Blue LED Intensity EVENT */

/* Macro to get the global application status flag array */
#define STATUS_GET(a)   (g_board_status.internal_events & (a))

/* Macro to set the global application status flag array*/
#define STATUS_SET(a)   (g_board_status.internal_events |= (a))

/* Macro to clear the global application status flag array*/
#define STATUS_CLEAR(a) (g_board_status.internal_events &= (uint32_t)(~(a)))

#define MENU_RETURN_INFO  "\r\n\r\n> Press space bar to return to MENU\r\n"

/* Macro definitions */

#ifndef APP_ERR_TRAP
#define APP_ERR_TRAP(a)             if(a) {__asm("BKPT #0\n");} /* trap the error location */
#endif

typedef struct
{
    uint16_t adc_temperature_data;  //temperature
    uint16_t led_intensity;         //PWM pulse width
    uint16_t led_frequency;         //frequency
    uint32_t internal_events;       //used to pass messages between isr's/callbacks & usercode
} st_board_status_t;



/* Terminal window escape sequences */
extern const char_t * const gp_clear_screen;
extern const char_t * const gp_cursor_home;

extern const char_t * const gp_cursor_store;
extern const char_t * const gp_cursor_restore;
extern const char_t * const gp_cursor_temp;
extern const char_t * const gp_cursor_frequency;
extern const char_t * const gp_cursor_intensity;

extern const char_t * const gp_green_fg;
extern const char_t * const gp_white_fg;



extern st_board_status_t g_board_status;

extern uint8_t g_pwm_dcs_data[];
extern uint8_t g_pwm_rates_data[];

extern int8_t g_selected_menu;
extern bsp_leds_t g_bsp_leds;
extern st_board_status_t g_board_status;


extern fsp_err_t common_init (void);
extern fsp_err_t print_to_console (char_t * p_data);
extern int8_t input_from_console (void);
extern void  update_console (void);

/**********************************************************************************************************************
 * Function Name: check_for_input_from_console
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
bool_t check_for_input_from_console (void);

#endif /* COMMON_INIT_H_ */
