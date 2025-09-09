/***********************************************************************************************************************
 * File Name    : lpm_app_util.h
 * Description  : Contains declarations of data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/**********************************************************************************************************************
* Copyright (c) 2019 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/

#ifndef LPM_APP_UTIL_H_
#define LPM_APP_UTIL_H_

/* generic headers */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hal_data.h"
#include "bsp_pin_cfg.h"
#include "r_ioport.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define LED_NO_0                  (0x00)
#define LED_NO_1                  (0x01)
#define NONE                      (0x00)

#define USER_SECOND               (00)
#define USER_MINUTE               (00)
#define USER_HOUR                 (00)
#define USER_DAY_OF_THE_MONTH     (01)
#define USER_MONTH                (00)
#define USER_YEAR                 (120)  // Add the User Year to the 1900. For 2020 = 1900 + 120

#define SECONDS_IN_A_HOUR         (3600)
#define SECONDS_IN_A_MINUTE       (60)
#define ALARM_TIME_IN_SECONDS     (10)

/*MACROs to adjust month and year values */
#define MON_ADJUST_VALUE          (1)
#define YEAR_ADJUST_VALUE         (1900)

#define USER_SW_RANGE_SHORT_PRESS          (1U)
#define USER_SW_RANGE_LONG_PRESS_LOW_LIMT  (3U)
#define USER_SW_RANGE_LONG_PRESS_HIGH_LIMT (6U)


#define INIT_FAILURE    user_led_on(leds);
#define PRE_FSM_ENTRY   user_led_off(leds);


#define PRE_FSM_ENTRY_PRINT   if (LOG_LVL == LOG_DEBUG) \
		                          APP_PRINT("\r\nStarting LPM Application, event  %s received\r",event_to_string(event_type))\
				              else \
					              APP_PRINT("\r\nApplication Started with received event %s \r",event_to_user_string(event_type))\

#define FSM_ENTRY_PRINT    if (LOG_LVL == LOG_DEBUG) \
		                       APP_PRINT("==>Entering State Machine with %s event\r",event_to_string(event_type))\
				           else \
					           APP_PRINT("==>Event Received %s \r",event_to_user_string(event_type))\

#define FSM_ALARM_PRINT  if(ST_LPM_SSBY == lpm_current_state)\
                         {\
                             APP_PRINT("RTC Alarm will trigger in %d Seconds, to cancel the \"%s\" LPM Mode \r",ALARM_TIME_IN_SECONDS,lpm_state_to_user_string(lpm_current_state))\
                         }\
                         else if (ST_LPM_SNZ == lpm_current_state)\
						 {\
                             APP_PRINT("RTC Alarm will trigger in %d Seconds, to cancel the Software Standby Mode during the \"%s\" LPM Mode \r",ALARM_TIME_IN_SECONDS,lpm_state_to_user_string(lpm_current_state))\
						 }\
                         else\
						 {\
						 }

#define CURRENT_FSM_PRINT    APP_PRINT("\r\n**************************************************************************\r")\
		                     APP_PRINT("***************    Running FSM : %s       ***************\r",fsm_to_string(running_state_machine))\
					         APP_PRINT("**************************************************************************\r")

#define EXIT_CLOCK_FSM_ENTER_LPM_FSM  APP_PRINT("Exiting Clock FSM and Entering LPM FSM\r")

#define EXIT_LPM_FSM_ENTER_CLOCK_FSM  APP_PRINT("Exiting LPM FSM and Entering Clock FSM\r")

#define USER_NOTE_PRINT      APP_PRINT("\r\n***********************************************************************************************\r")\
		                     APP_PRINT("Note: Press, Hold (5 seconds) and release the User Push button to generate Event- \"Long press\" \r")\
		                     APP_PRINT("      Press, Hold (2 seconds) and release the User Push button to generate Event- \"Short press\" \r")\
					         APP_PRINT("***********************************************************************************************\r")


#define FSM_ACTION_PRINT if(LOG_LVL == LOG_DEBUG) \
	                         APP_PRINT("Inside the Function %s, Current clock state  = %s, Current LPM state = %s\r\n",__FUNCTION__,clk_state_to_string(clk_currentState),lpm_state_to_string(lpm_current_state))\
                         else \
 	                         APP_PRINT("Current clock source = %s, Current LPM Mode = %s\r\n",clk_state_to_user_string(clk_currentState),lpm_state_to_user_string(lpm_current_state))


/*
 * MACROs that define user push-button pins, start source and stop sources for all boards
 * External IRQ channel of user push button for specific boards
 */
#if defined (BOARD_RA6M3_EK) || defined (BOARD_RA6M3G_EK)
#define USER_SW_IRQ_NUMBER        (0x0D)
#elif defined (BOARD_RA2A1_EK)
#define USER_SW_IRQ_NUMBER        (0x06)
#elif defined (BOARD_RA6M2_EK) || defined (BOARD_RA6M1_EK) || defined (BOARD_RA4M1_EK)
#define USER_SW_IRQ_NUMBER        (0x00)
#elif defined (BOARD_RA2L1_EK)
#define USER_SW_IRQ_NUMBER        (0x03)
#elif defined (BOARD_RA6E1_FPB)
#define USER_SW_IRQ_NUMBER        (0x01)
#elif defined (BOARD_RA4E1_FPB)
#define USER_SW_IRQ_NUMBER        (0x01)
#endif

typedef  void (*action) (void);

/*
 * Low Power Mode Definitions for LPM app
 * Since there are no Normal mode definition in LPM driver, use this enum to keep LPM app state including:
 * Sleep, SW Standby, SW Standby with Snooze enabled, Deep SW Standby, Normal.
 */
typedef enum e_app_lpm_state
{
    APP_LPM_SLEEP_STATE = 0,             ///< Sleep mode
    APP_LPM_SW_STANDBY_STATE,            ///< SW Standby mode
    APP_LPM_SW_STANDBY_SNOOZE_STATE,     ///< SW Standby mode with Snooze enabled
    APP_LPM_DEEP_SW_STANDBY_STATE,       ///< Deep SW Standby mode
    APP_LPM_NORMAL_STATE                 ///< Normal mode
} app_lpm_states_t;

/*
 *  enums used in the FSM and LPM Application These enums are used within the code for maintaining internal states
 */

/* enums for switch state */
typedef enum e_switch_state
{
	SW_PRESSED,
	SW_RELEASED,
	SW_INVALID
}switch_state_t;

/* Currently running State Machine */
typedef enum e_running_SM
{
	CLK_SM,
	LPM_SM
}running_SM_t;

/* Internal Clock states for the  State Machine */
typedef enum e_clk_state
{
    ST_HOCO_NORM,
    ST_MOCO_NORM,
	ST_LOCO_NORM,
	ST_SOSC_NORM,
	ST_CLK_NORM
}clk_state_t;

/* Internal LPM states for the  State Machine */
typedef enum e_lpm_state
{
	ST_LPM_NORM,
	ST_LPM_SLEEP,
	ST_LPM_NORM_FRM_SLEEP,
	ST_LPM_SSBY,
	ST_LPM_NORM_FRM_SSBY,
    ST_LPM_SNZ,
	ST_LPM_NORM_FRM_SNZ,
	ST_LPM_DEEP_SSBY
}lpm_state_t;

/* Different Event for the  State Machine */
typedef enum e_u_event
{
	EV_PB_SHORT_PR,
	EV_PB_LONG_PR,
    EV_PERIODIC_TIMER,
	EV_RTC_ALARM,
    EV_POWER_ON_RESET,
	EV_POWER_ON_RESET_DSSBY,
	EV_NONE
}u_event_t;

/* Clock state Transition Table for the predefined Events and Actions */
typedef struct st_clock_state_trans
{
    clk_state_t next_state;
    action clk_action_to_do;
}clock_state_trans_t;

typedef struct st_clock_SM
{
	clock_state_trans_t clk_transition_SM;
    clk_state_t prv_state;
}clock_SM_t;

/* LPM state Transition Table for the predefined Events and Actions */
typedef struct st_lpm_state_tran
{
    lpm_state_t next_state;
    action lpm_action_to_do;
}lpm_state_tran_t;

typedef struct st_lpm_SM
{
	lpm_state_tran_t lpm_tansition_SM;
    lpm_state_t prv_state;
}lpm_SM_t;


extern bsp_leds_t g_bsp_leds;
extern u_event_t event_type;
extern clk_state_t  clk_currentState;
extern lpm_state_t  lpm_current_state;
extern running_SM_t running_state_machine;
extern lpm_instance_ctrl_t *g_lpm_ctrl_instance_ctrls[];
extern lpm_cfg_t	   *g_lpm_ctrl_instance_cfgs[];
extern app_lpm_states_t g_lpm_transition_sequence[];
extern const clock_state_trans_t  clock_transition_table[4][2];
extern const lpm_state_tran_t lpm_transition_table[8][3];
extern bsp_leds_t *leds;

extern void R_BSP_WarmStart(bsp_warm_start_event_t event);
extern void delay_500ms();
extern fsp_err_t user_led_toggle(bsp_leds_t *leds);
extern void read_and_print_rtc_time(void);
extern void startPeriodicTimer(void);
extern void stopPeriodicTimer(void);
extern fsp_err_t set_rtc_calendar_time(void);
extern fsp_err_t reset_rtc_calendar_alarm();
extern fsp_err_t set_rtc_calendar_alarm();
extern void preffered_ra6m_initial_clock_setting(cgc_clocks_cfg_t *lsys_clk_cfg);

/*Action functions to be performed by FSM */
void change_system_clock(void);
void enter_lpm_sm(void);
void exit_lpm_sm(void);
void no_action(void);
void lpm_norm_exit_sleep_entry(void);
void lpm_sleep_exit_norm_entry(void);
void lpm_norm_exit_ssby_entry (void);
void lpm_norm_exit_snooze_entry(void);
void lpm_norm_exit_deep_ssby_entry (void);
void lpm_deep_ssby_exit_norm_entry (void);
void lpm_ssby_timer_action(void);
void lpm_snz_timer_action(void);

/* LPM execution */
void execute_lpm(uint8_t state);
fsp_err_t lpm_mode_enter(app_lpm_states_t lpm_mode, lpm_instance_ctrl_t * const g_lpm_ctrl_instance_ctrls);

/* FSM Engine */
void run_FSM(u_event_t  levent );

/* Utility functions for converting the Internal states to User readable formatted messages */
char *clk_state_to_string(clk_state_t cstate);
char *clk_state_to_user_string(clk_state_t cstate);
char *clk_to_string(cgc_clock_t cstate);
char *event_to_user_string(u_event_t levent);
char *event_to_string(u_event_t levent);
char *fsm_to_string(running_SM_t lfsm);
char *lpm_state_to_string(lpm_state_t lstate);
char *lpm_state_to_user_string(lpm_state_t lstate);

/* MCU PeriPheral Function used in the Application declarations */
fsp_err_t user_switch_init(void);

fsp_err_t user_led_on(bsp_leds_t *leds);
fsp_err_t user_led_off(bsp_leds_t *leds);

void agt_timer1_periodic_cb(timer_callback_args_t *p_args);

fsp_err_t init_cgc(void);

fsp_err_t init_rtc(void);
void rtc_callback(rtc_callback_args_t *p_args);

#endif /* LPM_APP_UTIL_H_ */
