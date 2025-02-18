/***********************************************************************************************************************
 * File Name    : lpm_app.c
 * Description  : Contains function definition related to LPM Application, LPM SM Clock SM and its associated Action
 *              : functions.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
#include <lpm_app_util.h>
#include "common_utils.h"
#include "hal_data.h"
#include "bsp_pin_cfg.h"

/* Board LED  Pointer */
bsp_leds_t   *leds = &g_bsp_leds;

/* Keeps track of the internal state of Current Clock and Current LPM Mode for the FSM.
 * Default Clock State and LPM  state are set to MOSC and Normal Mode
 */
clk_state_t  clk_currentState =  ST_HOCO_NORM;
lpm_state_t  lpm_current_state = ST_LPM_NORM;

/* Keep track of the currently running  State Machine. By default on Power on Reset it always enters the Clock SM*/
running_SM_t running_state_machine =  CLK_SM;

/* These are LPM Mode instances for Sleep, Software Standby, Snooze and Deep Software Standby Modes. These instances
 * are created by the FSP. We need these at the Application level to take the MCU to different LPM modes with configured
 * trigger/cancel sources
 */
#if defined (BOARD_RA6M3_EK) || defined (BOARD_RA6M3G_EK) || defined (BOARD_RA6M2_EK) || defined (BOARD_RA6M1_EK) || defined (BOARD_RA4E1_FPB)

    lpm_instance_ctrl_t *g_lpm_ctrl_instance_ctrls[] = {
    		                                                (lpm_instance_ctrl_t *)&g_lpm_sleep_ctrl,
    												        (lpm_instance_ctrl_t *)&g_lpm_sw_standby_ctrl,
													        (lpm_instance_ctrl_t *)&g_lpm_sw_standby_with_snooze_ctrl,
													        (lpm_instance_ctrl_t *)&g_lpm_deep_sw_standby_ctrl
    												   };
    lpm_cfg_t			*g_lpm_ctrl_instance_cfgs[]  = {
    		                                                (lpm_cfg_t *)&g_lpm_sleep_cfg,
    												        (lpm_cfg_t *)&g_lpm_sw_standby_cfg,
													        (lpm_cfg_t *)&g_lpm_sw_standby_with_snooze_cfg,
													        (lpm_cfg_t *)&g_lpm_deep_sw_standby_cfg
                                                       };

#elif defined (BOARD_RA4M1_EK) || defined (BOARD_RA2A1_EK)
    lpm_instance_ctrl_t *g_lpm_ctrl_instance_ctrls[] = {
                                                            (lpm_instance_ctrl_t *)&g_lpm_sleep_ctrl,
		                                                    (lpm_instance_ctrl_t *)&g_lpm_sw_standby_ctrl,
		                                                    (lpm_instance_ctrl_t *)&g_lpm_sw_standby_with_snooze_ctrl,
    												   };
    lpm_cfg_t			g*_lpm_ctrl_instance_cfgs[]  = {
                                                            (lpm_cfg_t *)&g_lpm_sleep_cfg,
			                                                (lpm_cfg_t *)&g_lpm_sw_standby_cfg,
			                                                (lpm_cfg_t *)&g_lpm_sw_standby_with_snooze_cfg,
                                                       };
#endif

/*
 * Clock Transition Table: Event to drive this clock Transition Table are, user Switch Button inputs. When User Presses
 * the Switch for longer duration, it is considered as "Button Press - Long". When the User Presses the Switch for shorter
 * duration it is considered as "Button Press - Short". Switch pressed and held for about 1-2 Seconds is - Short Press.
 * Switch pressed and held for about 3-6 Seconds is - Long Press.
 */
const clock_state_trans_t  clock_transition_table[4][2] =
{
/********************************************************************************************/
/*                   *   Button Press - Short      *   Button Press - Long                  */
/********************************************************************************************/
/*Present State      |   Next State   Action       |   Next State,   Action                 */
/********************************************************************************************/
/*ST_HOCO_NORM */    { { ST_HOCO_NORM,  enter_lpm_sm}, { ST_MOCO_NORM, change_system_clock} },
/*ST_MOCO_NORM */    { { ST_MOCO_NORM,  enter_lpm_sm}, { ST_LOCO_NORM, change_system_clock} },
/*ST_LOCO_NORM */    { { ST_LOCO_NORM,  enter_lpm_sm}, { ST_HOCO_NORM, change_system_clock} },
/*ST_CLK_NORM  */    { { ST_HOCO_NORM,  enter_lpm_sm}, { ST_HOCO_NORM, change_system_clock} }
};

/* LPM Transition Table: Event to drive this LPM Transition Table are, User Switch Button inputs and System Generated events.
 * User Switch Button inputs are "Button Press - Long" and "Button Press - Short". System Events are Timer Events (AGT Periodic Timer, RTC alarm Interrupt.
 */
const lpm_state_tran_t lpm_transition_table[8][3] =
{
/*************************************************************************************************************************************************************************************************/
/*                         *          Button Press - Short                           *   Button Press - Long                                  *               Timer Event                        */
/*************************************************************************************************************************************************************************************************/
/*Present State            |   Next State              Action                        |   Next State,   Action                                 | Next State,             Action                   */
/*************************************************************************************************************************************************************************************************/
/*ST_LPM_NORM           */{ { ST_LPM_SLEEP,          lpm_norm_exit_sleep_entry     }, { ST_LPM_NORM,           exit_lpm_sm                  }, { ST_LPM_NORM,           no_action              } },
/*ST_LPM_SLEEP          */{ { ST_LPM_NORM_FRM_SLEEP, lpm_sleep_exit_norm_entry     }, { ST_LPM_NORM_FRM_SLEEP, lpm_sleep_exit_norm_entry    }, { ST_LPM_NORM_FRM_SLEEP, no_action              } },
/*ST_LPM_NORM_FRM_SLEEP */{ { ST_LPM_SSBY,           lpm_norm_exit_ssby_entry      }, { ST_LPM_NORM,           exit_lpm_sm                  }, { ST_LPM_NORM,           no_action              } },
/*ST_LPM_SSBY           */{ { ST_LPM_NORM_FRM_SSBY,  no_action                     }, { ST_LPM_NORM_FRM_SSBY,  no_action                    }, { ST_LPM_NORM_FRM_SSBY,  lpm_ssby_timer_action  } },
/*ST_LPM_NORM_FRM_SSBY  */{ { ST_LPM_SNZ,            lpm_norm_exit_snooze_entry    }, { ST_LPM_NORM,           exit_lpm_sm                  }, { ST_LPM_NORM_FRM_SSBY,  no_action              } },
/*ST_LPM_SNZ            */{ { ST_LPM_NORM_FRM_SNZ,   no_action                     }, { ST_LPM_NORM_FRM_SNZ,   no_action                    }, { ST_LPM_NORM_FRM_SNZ,   lpm_snz_timer_action   } },
/*ST_LPM_NORM_FRM_SNZ   */{ { ST_LPM_DEEP_SSBY,      lpm_norm_exit_deep_ssby_entry }, { ST_LPM_NORM,           exit_lpm_sm                  }, { ST_LPM_NORM,           no_action              } },
/*ST_LPM_DEEP_SSBY      */{ { ST_LPM_NORM,           lpm_deep_ssby_exit_norm_entry }, { ST_LPM_NORM,           lpm_deep_ssby_exit_norm_entry}, { ST_LPM_NORM,           no_action              } },
};


/*******************************************************************
 * @brief Pre-defined LPM Transition Sequence
 * There are 2 kinds of sequence, one supports RA6Mx device and the other supports other RA devices
 * *****************************************************************/
#if defined (BOARD_RA6M3_EK) || defined (BOARD_RA6M3G_EK) || defined (BOARD_RA6M2_EK) || defined (BOARD_RA6M1_EK) || defined (BOARD_RA4E1_FPB)
app_lpm_states_t g_lpm_transition_sequence[] =
{
    APP_LPM_SLEEP_STATE,                 ///< Sleep mode
    APP_LPM_SW_STANDBY_STATE,            ///< SW Standby mode
    APP_LPM_SW_STANDBY_SNOOZE_STATE,     ///< SW Standby mode with Snooze enabled
    APP_LPM_DEEP_SW_STANDBY_STATE,       ///< Deep SW Standby mode
    APP_LPM_NORMAL_STATE                 ///< Normal mode
};
#elif defined (BOARD_RA4M1_EK) || defined (BOARD_RA2A1_EK)
app_lpm_states_t g_lpm_transition_sequence[] =
{
    APP_LPM_SLEEP_STATE,                 ///< Sleep mode
    APP_LPM_SW_STANDBY_STATE,            ///< SW Standby mode
    APP_LPM_SW_STANDBY_SNOOZE_STATE,     ///< SW Standby mode with Snooze enabled
    APP_LPM_NORMAL_STATE                 ///< Normal mode
};
#endif


/***********************************************************************************************************************
 * @brief       This function converts the FSM events into string for user debug and display purpose
 * @param[IN]   event
 * @retval      returns the equivalent string for the event.
 * @retval      Any Other event returns "UNKNOWN"
 **********************************************************************************************************************/
char *fsm_to_string(running_SM_t lfsm)
{
    switch(lfsm)
    {
      case CLK_SM:
          return "Clock State Machine";

      case LPM_SM:
          return "LPM State Machine";

      default:
    	  return "UNKNOWN";
  }
    return "UNKNOWN";
}


/***********************************************************************************************************************
 * @brief       This function converts the FSM events into string for user debug and display purpose
 * @param[IN]   event
 * @retval      returns the equivalent string for the event.
 * @retval      Any Other event returns "UNKNOWN"
 **********************************************************************************************************************/
char *event_to_string(u_event_t levent)
{
    switch(levent)
    {
      case EV_PB_SHORT_PR:
          return "EV_PB_SHORT_PR";

      case EV_PB_LONG_PR:
          return "EV_PB_LONG_PR";

      case EV_PERIODIC_TIMER:
          return "EV_PERIODIC_TIMER";

      case EV_RTC_ALARM:
          return "EV_RTC_ALARM";

      case EV_POWER_ON_RESET:
          return "EV_POWER_ON_RESET";

      case EV_POWER_ON_RESET_DSSBY:
          return "EV_POWER_ON_RESET_DSSBY";

      case EV_NONE:
      default:
    	  return "UNKNOWN";

  }
    return "UNKNOWN";
}

/***********************************************************************************************************************
 * @brief       This function converts the FSM events into  understandable string for user Notification
 * @param[IN]   event
 * @retval      returns the equivalent string for the event.
 * @retval      Any Other event returns "Unknown"
 **********************************************************************************************************************/
char *event_to_user_string(u_event_t levent)
{
    switch(levent)
    {
      case EV_PB_SHORT_PR:
          return "-> User Push Button - (Short press)";

      case EV_PB_LONG_PR:
          return "-> User Push Button - (Long press)";

      case EV_PERIODIC_TIMER:
          return "-> Periodic timer";

      case EV_RTC_ALARM:
          return "-> RTC Alarm";

      case EV_POWER_ON_RESET:
          return "-> Power on RESET";

      case EV_POWER_ON_RESET_DSSBY:
          return "-> RESET from Deep Software Standby";

      case EV_NONE:
      default:
    	  return "-> Unknown";
  }
    return "-> Unknown";
}

/**********************************************************************************************************************
 * @brief       This function converts the FSM internal states into string format for user debug and display purpose
 * @param[IN]   state
 * @retval      returns the equivalent string for the state.
 * @retval      Any Other state returns "UNKNOWN"
 **********************************************************************************************************************/
char *lpm_state_to_string(lpm_state_t lstate)
{

	switch(lstate)
    {
      case ST_LPM_NORM:
    	  return "ST_LPM_NORM";

      case ST_LPM_SLEEP:
    	  return "ST_LPM_SLEEP";

      case ST_LPM_NORM_FRM_SLEEP:
    	  return "ST_LPM_NORM_FRM_SLEEP";

      case ST_LPM_SSBY:
    	  return "ST_LPM_SSBY";

      case ST_LPM_NORM_FRM_SSBY:
    	  return "ST_LPM_NORM_FRM_SSBY";

      case ST_LPM_SNZ:
    	  return "ST_LPM_SNZ";

      case ST_LPM_NORM_FRM_SNZ:
    	  return "ST_LPM_NORM_FRM_SNZ";

      case 	ST_LPM_DEEP_SSBY:
    	  return "ST_LPM_DEEP_SSBY";

      default:
    	  return "UNKNOWN";
    }
	return "UNKNOWN";
}

/**********************************************************************************************************************
 * @brief       This function converts the FSM internal states into  understandable string for user Notification.
 * @param[IN]   state
 * @retval      returns the equivalent string for the state.
 * @retval      Any Other state returns "UNKNOWN"
 **********************************************************************************************************************/
char *lpm_state_to_user_string(lpm_state_t lstate)
{

	switch(lstate)
    {
      case ST_LPM_NORM:
    	  return "Normal";

      case ST_LPM_SLEEP:
    	  return "Sleep";

      case ST_LPM_NORM_FRM_SLEEP:
    	  return "Normal";

      case ST_LPM_SSBY:
    	  return "Software Standby";

      case ST_LPM_NORM_FRM_SSBY:
    	  return "Normal";

      case ST_LPM_SNZ:
    	  return "Snooze";

      case ST_LPM_NORM_FRM_SNZ:
    	  return "Normal";

      case 	ST_LPM_DEEP_SSBY:
    	  return "Deep Software Standby";

      default:
    	  return "UNKNOWN";
    }
	return "Unknown";
}

/***********************************************************************************************************************
 * @brief       This function converts the FSM internal clock state into understandable string for user Notification.
 * @param[IN]   clock state
 * @retval      returns the equivalent string for the clock state.
 * @retval      Any Other state returns "UNKNOWN"
 **********************************************************************************************************************/
char *clk_to_string(cgc_clock_t cstate)
{
    switch(cstate)
    {
      case CGC_CLOCK_HOCO:
          return "HOCO CLOCK";

      case CGC_CLOCK_MOCO:
          return "MOCO CLOCK";

      case CGC_CLOCK_LOCO:
          return "LOCO CLOCK";

      case CGC_CLOCK_MAIN_OSC:
          return "MAIN OSC CLOCK";

      case CGC_CLOCK_SUBCLOCK:
          return "SUB OSC CLOCK";

      case CGC_CLOCK_PLL:
          return "PLL OSC CLOCK";

      default:
    	  return "UNKNOWN";
    }
    return "UNKNOWN";
}

/**********************************************************************************************************************
 * @brief       This function converts the FSM internal clock states into string format for user debug and display
 *              purpose only.
 * @param[IN]   clock state
 * @retval      returns the equivalent string for the clock state.
 * @retval      Any Other state returns "UNKNOWN"
 **********************************************************************************************************************/
char *clk_state_to_string(clk_state_t cstate)
{
	switch(cstate)
    {

      case ST_HOCO_NORM:
    	  return "ST_HOCO_NORM";

      case ST_MOCO_NORM:
    	  return "ST_MOCO_NORM";

      case ST_LOCO_NORM:
    	  return "ST_LOCO_NORM";

      case ST_CLK_NORM:
    	  return "ST_CLK_NORM";

      default:
    	  return "UNKNOWN";
    }
	return "UNKNOWN";
}

/**********************************************************************************************************************
 * @brief       This function converts the FSM internal clock states into short string format for user display purpose.
 * @param[IN]   clock state
 * @retval      returns the equivalent string for the clock state.
 * @retval      Any Other state returns "UNKNOWN"
 **********************************************************************************************************************/
char *clk_state_to_user_string(clk_state_t cstate)
{
	switch(cstate)
    {

      case ST_HOCO_NORM:
    	  return "HOCO";

      case ST_MOCO_NORM:
    	  return "MOCO";

      case ST_LOCO_NORM:
    	  return "LOCO";

      case ST_SOSC_NORM:
    	  return "SOSC";

      case ST_CLK_NORM:
    	  return "MOSC";

      default:
    	  return "UNKNOWN";
    }
	return "UNKNOWN";
}
/**********************************************************************************************************************
 * @brief       This function changes the System Clock to the desired clock from the User Input.
 * @param[IN]   void
 * @retval      None.
 **********************************************************************************************************************/
void change_system_clock(void)
{
	fsp_err_t err = FSP_SUCCESS;
	cgc_divider_cfg_t sys_divider_cf = { RESET_VALUE };
	cgc_clock_t  sys_clock_source    = { RESET_VALUE };
	cgc_clocks_cfg_t sys_clk_cfg     = { RESET_VALUE };
	cgc_pll_cfg_t new_clk            = { RESET_VALUE };


	err = R_CGC_SystemClockGet(&g_cgc0_ctrl, &sys_clock_source, &sys_divider_cf);
	if (FSP_SUCCESS != err)
	{
		APP_ERR_TRAP(err);
	}

	APP_PRINT("\r\nMCU Running with Clock Source = %s, \t", clk_to_string(sys_clock_source));
	switch (clk_currentState)
	{
		case ST_HOCO_NORM:
		{
			sys_clock_source = CGC_CLOCK_HOCO;
			sys_clk_cfg.hoco_state = CGC_CLOCK_CHANGE_START;

			sys_clk_cfg.loco_state = CGC_CLOCK_CHANGE_START;
			sys_clk_cfg.mainosc_state = CGC_CLOCK_CHANGE_STOP;
			sys_clk_cfg.pll_state = CGC_CLOCK_CHANGE_STOP;
			sys_clk_cfg.pll2_state = CGC_CLOCK_CHANGE_STOP;
			sys_clk_cfg.system_clock = CGC_CLOCK_HOCO;
			sys_clk_cfg.divider_cfg.sckdivcr_b.fclk_div = CGC_SYS_CLOCK_DIV_1;
			sys_clk_cfg.divider_cfg.sckdivcr_b.iclk_div = CGC_SYS_CLOCK_DIV_1;
			sys_clk_cfg.divider_cfg.sckdivcr_b.pclka_div =  CGC_SYS_CLOCK_DIV_2;
			sys_clk_cfg.divider_cfg.sckdivcr_b.pclkb_div = CGC_SYS_CLOCK_DIV_2;
			sys_clk_cfg.divider_cfg.sckdivcr_b.pclkc_div = CGC_SYS_CLOCK_DIV_4;
			sys_clk_cfg.divider_cfg.sckdivcr_b.pclkd_div = CGC_SYS_CLOCK_DIV_2;


			break;
		}
		case ST_MOCO_NORM:
		{
			sys_clock_source = CGC_CLOCK_MOCO;
			sys_clk_cfg.moco_state = CGC_CLOCK_CHANGE_START;
			sys_clk_cfg.hoco_state = CGC_CLOCK_CHANGE_NONE;
			sys_clk_cfg.loco_state = CGC_CLOCK_CHANGE_NONE;
			sys_clk_cfg.mainosc_state = CGC_CLOCK_CHANGE_STOP;
			sys_clk_cfg.pll_state = CGC_CLOCK_CHANGE_STOP;
			sys_clk_cfg.pll2_state = CGC_CLOCK_CHANGE_STOP;
			sys_clk_cfg.system_clock = CGC_CLOCK_MOCO;
			sys_clk_cfg.divider_cfg.sckdivcr_b.fclk_div = CGC_SYS_CLOCK_DIV_1;
			sys_clk_cfg.divider_cfg.sckdivcr_b.iclk_div = CGC_SYS_CLOCK_DIV_1;
			sys_clk_cfg.divider_cfg.sckdivcr_b.pclka_div =  CGC_SYS_CLOCK_DIV_1;
			sys_clk_cfg.divider_cfg.sckdivcr_b.pclkb_div = CGC_SYS_CLOCK_DIV_1;
			sys_clk_cfg.divider_cfg.sckdivcr_b.pclkc_div = CGC_SYS_CLOCK_DIV_1;
			sys_clk_cfg.divider_cfg.sckdivcr_b.pclkd_div = CGC_SYS_CLOCK_DIV_1;

			break;
		}
	    case ST_LOCO_NORM:
	    {
			sys_clock_source = CGC_CLOCK_LOCO;
			sys_clk_cfg.loco_state = CGC_CLOCK_CHANGE_START;
			sys_clk_cfg.hoco_state = CGC_CLOCK_CHANGE_NONE;
			sys_clk_cfg.moco_state = CGC_CLOCK_CHANGE_NONE;
			sys_clk_cfg.mainosc_state = CGC_CLOCK_CHANGE_STOP;
			sys_clk_cfg.pll_state = CGC_CLOCK_CHANGE_STOP;
			sys_clk_cfg.pll2_state = CGC_CLOCK_CHANGE_STOP;
			sys_clk_cfg.system_clock = CGC_CLOCK_LOCO;
			sys_clk_cfg.divider_cfg.sckdivcr_b.fclk_div = CGC_SYS_CLOCK_DIV_1;
			sys_clk_cfg.divider_cfg.sckdivcr_b.iclk_div = CGC_SYS_CLOCK_DIV_1;
			sys_clk_cfg.divider_cfg.sckdivcr_b.pclka_div =  CGC_SYS_CLOCK_DIV_2;
			sys_clk_cfg.divider_cfg.sckdivcr_b.pclkb_div = CGC_SYS_CLOCK_DIV_2;
			sys_clk_cfg.divider_cfg.sckdivcr_b.pclkc_div = CGC_SYS_CLOCK_DIV_4;
			sys_clk_cfg.divider_cfg.sckdivcr_b.pclkd_div = CGC_SYS_CLOCK_DIV_2;
			break;
	    }

		case ST_CLK_NORM:
		default:
		{
			sys_clock_source = CGC_CLOCK_HOCO;
			break;
		}
	}

	sys_clk_cfg.system_clock = sys_clock_source;

	err = R_CGC_ClocksCfg(&g_cgc0_ctrl, &sys_clk_cfg);
	if (FSP_SUCCESS != err)
	{
		APP_ERR_TRAP(err);
	}

	APP_PRINT("Requested Clock Source for MCU = %s \r\n",clk_to_string(sys_clock_source));

	if (CGC_CLOCK_SUBCLOCK == sys_clock_source)
	{
		new_clk.source_clock = CGC_CLOCK_SUBCLOCK;
		err = R_CGC_ClockStart(&g_cgc0_ctrl, CGC_CLOCK_SUBCLOCK, &new_clk);
		if (FSP_SUCCESS != err)
		{
			APP_ERR_TRAP(err);
		}
	}
	else if (CGC_CLOCK_LOCO == sys_clock_source)
	{
		new_clk.source_clock = CGC_CLOCK_LOCO;
		err = R_CGC_ClockStart(&g_cgc0_ctrl, CGC_CLOCK_LOCO, &new_clk);
		if (FSP_SUCCESS != err) {
			APP_ERR_TRAP(err);
		}
	}
	else if (CGC_CLOCK_MOCO == sys_clock_source)
	{
		new_clk.source_clock = CGC_CLOCK_MOCO;
		err = R_CGC_ClockStart(&g_cgc0_ctrl, CGC_CLOCK_MOCO, &new_clk);
		if (FSP_SUCCESS != err)
		{
			APP_ERR_TRAP(err);
		}
	}
	else if (CGC_CLOCK_HOCO == sys_clock_source)
	{
		new_clk.source_clock = CGC_CLOCK_HOCO;
		err = R_CGC_ClockStart(&g_cgc0_ctrl, CGC_CLOCK_HOCO, &new_clk);
		if (FSP_SUCCESS != err)
		{
			APP_ERR_TRAP(err);
		}
	}
	/* Clock  Oscillation Stabilization, wait for its stabilization by adding delay */
	R_BSP_SoftwareDelay(BSP_FEATURE_CGC_LOCO_STABILIZATION_MAX_US,BSP_DELAY_UNITS_MICROSECONDS);

	FSM_ACTION_PRINT
	read_and_print_rtc_time();
}

 /**********************************************************************************************************************
  * @brief       This function initiates the LPM Mode based on the state transition by FSM.
  *              When user/system event is triggered ("Button Press - Short"/timer), as part of the action this gets
  *              called.
  * @param[IN]   state: This is requested LPM state by the FSM code
  * @retval      None.
  **********************************************************************************************************************/
void execute_lpm(uint8_t state)
{
    fsp_err_t err = FSP_SUCCESS;

    PRE_FSM_ENTRY
	delay_500ms();

	/* Disable IO port if it's not in Deep SW Standby mode */
	if (APP_LPM_DEEP_SW_STANDBY_STATE != g_lpm_transition_sequence[state])
	{
		/* Disable IO port before going to LPM mode*/
        err = R_IOPORT_PinsCfg(&g_ioport_ctrl, &g_bsp_pin_cfg);;
		/* Handle error */
		if (FSP_SUCCESS != err)
		{
			APP_ERR_TRAP(err);
		}
	}
	/* Open LPM instance*/
	err = R_LPM_Open(g_lpm_ctrl_instance_ctrls[state], g_lpm_ctrl_instance_cfgs[state]);
	/* Handle error */
	if (FSP_SUCCESS != err)
	{
		APP_ERR_TRAP(err);
	}

	/* Enter LPM mode */
    err = lpm_mode_enter(g_lpm_transition_sequence[state], g_lpm_ctrl_instance_ctrls[state]);
	/* Handle error */
	if (FSP_SUCCESS != err)
	{
		/* Turn on user LED to indicate error occurred*/
		R_IOPORT_PinWrite(&g_ioport_ctrl, leds->p_leds[LED_NO_0], BSP_IO_LEVEL_HIGH);
		APP_ERR_TRAP(err);
	}
	else
	{
		/* Close LPM instance.*/
		err = R_LPM_Close(g_lpm_ctrl_instance_ctrls[state]);
		/* Handle error */
		if (FSP_SUCCESS != err)
		{
			APP_ERR_TRAP(err);
		}

		/* Put IO port configuration back to user's selections */
		err = R_IOPORT_PinsCfg(&g_ioport_ctrl, &g_bsp_pin_cfg);

		/* Handle error */
		if (FSP_SUCCESS != err)
		{
			APP_ERR_TRAP(err);
		}
	}
}

/***********************************************************************************************************************
 * @brief       This function is a action to perform when user event is triggered in the Clock Mode (Button Press - Short).
 * @param[IN]   void
 * @retval      None.
  **********************************************************************************************************************/
 void enter_lpm_sm(void)
{
	 EXIT_CLOCK_FSM_ENTER_LPM_FSM
	 FSM_ACTION_PRINT
}

/**********************************************************************************************************************
 * @brief       This function initiates exit action from currently executing LPM state (Normal Mode)  to Clock State,
 *              when user event is triggered (Button Press - Long). T
 * @param[IN]   void
 * @retval      None.
 **********************************************************************************************************************/
void exit_lpm_sm(void)
{
	EXIT_LPM_FSM_ENTER_CLOCK_FSM
	read_and_print_rtc_time();
	FSM_ACTION_PRINT
	running_state_machine = CLK_SM;
}

/**********************************************************************************************************************
 * @brief       This function performs no LPM action, but covers the no action coverage of the FSM based on events.
 * @param[IN]   void
 * @retval      None.
 **********************************************************************************************************************/
void no_action(void)
{
	read_and_print_rtc_time();
	FSM_ACTION_PRINT
}

/**********************************************************************************************************************
 * @brief       This function performs FSM action for LPM Mode Transition: User Press button event triggers this action.
 *              Exits the Normal Mode and Prepares to enter the Sleep Mode.
 * @param[IN]   void
 * @retval      None.
 **********************************************************************************************************************/
void lpm_norm_exit_sleep_entry(void)
{
	read_and_print_rtc_time();
	FSM_ACTION_PRINT
	execute_lpm(APP_LPM_SLEEP_STATE);
}

/**********************************************************************************************************************
 * @brief       This function performs FSM action for LPM Mode Transition: User Press button event triggers this action.
 *              This gets called when Sleep mode is Exited and enters Normal Mode.
 * @param[IN]   void
 * @retval      None.
 **********************************************************************************************************************/
void lpm_sleep_exit_norm_entry(void)
{
	read_and_print_rtc_time();
	FSM_ACTION_PRINT
}

/**********************************************************************************************************************
 * @brief       This function performs FSM action for LPM Mode Transition: User Press button event triggers this action.
 *              Action performed: Exits the Normal Mode and Prepares to enter the Software Standby Mode.
 * @param[IN]   void
 * @retval      None.
 **********************************************************************************************************************/
void lpm_norm_exit_ssby_entry(void)
{
	read_and_print_rtc_time();
	set_rtc_calendar_alarm();
	FSM_ACTION_PRINT
	execute_lpm(APP_LPM_SW_STANDBY_STATE);
}

/**********************************************************************************************************************
 * @brief       This function performs FSM action for LPM Mode Transition: User Press button event triggers this.
 *              Action performed: Exits the Normal Mode and Prepares to enter the Snooze Mode.
 * @param[IN]   void
 * @retval      None.
 **********************************************************************************************************************/
void lpm_norm_exit_snooze_entry(void)
{
	read_and_print_rtc_time();
	startPeriodicTimer();
	set_rtc_calendar_alarm();
	FSM_ACTION_PRINT
	execute_lpm(APP_LPM_SW_STANDBY_SNOOZE_STATE);
}

/**********************************************************************************************************************
 * @brief       This function performs FSM action for LPM Mode Transition: User Press button event triggers this.
 *              Action performed: Exits the Normal Mode and Prepares to enter the Deep Software Standby Mode.
 * @param[IN]   void
 * @retval      None.
 **********************************************************************************************************************/
void lpm_norm_exit_deep_ssby_entry (void)
{
	read_and_print_rtc_time();
	FSM_ACTION_PRINT
	execute_lpm(APP_LPM_DEEP_SW_STANDBY_STATE);
}

/**********************************************************************************************************************
 * @brief       This function performs FSM action for LPM SM Transition based on event
 *              This is Just a place holder for the SM completeness. By the virtue of the Deep software standby mode,on
 *              interrupt the LPM mode of Deep software standby mode is exited and enters the Internal Reset Mode.
 *              So this is again not going to get executed. Just keeping it as a stub function.
 * @param[IN]   void
 * @retval      None.
 **********************************************************************************************************************/
void lpm_deep_ssby_exit_norm_entry (void)
{
	read_and_print_rtc_time();
	FSM_ACTION_PRINT
}


/**********************************************************************************************************************
 * @brief       This function performs FSM action for LPM SM Transition based on the Timer event(RTC Alarm Interrupt).
 *              This is the action for the RTC Alarm generated event in the Software Standby Mode. This function performs
 *              the Post action of the Software Standby Mode exit.
 * @param[IN]   void
 * @retval      None.
 **********************************************************************************************************************/
void lpm_ssby_timer_action(void)
{
	stopPeriodicTimer();
	reset_rtc_calendar_alarm();
	delay_500ms();
	read_and_print_rtc_time();
	FSM_ACTION_PRINT
}

/**********************************************************************************************************************
 * @brief       This function performs FSM action for LPM SM Transition based on the Timer event(RTC Alarm Interrupt).
 *              This is the action for the RTC Alarm generated event in the Snooze Mode. This function performs the Post
 *              action of the Snooze Mode exit.
 * @param[IN]   void
 * @retval      None.
 **********************************************************************************************************************/
void lpm_snz_timer_action(void)
{
	stopPeriodicTimer();
	reset_rtc_calendar_alarm();
	delay_500ms();
	read_and_print_rtc_time();
	FSM_ACTION_PRINT
}

/**********************************************************************************************************************
 * @brief       This function performs 500ms Delay using the BSP Software Delay API.
 * @param[IN]   void
 * @retval      None.
 **********************************************************************************************************************/
void delay_500ms()
{
	R_BSP_SoftwareDelay(500, BSP_DELAY_UNITS_MILLISECONDS);
}


/*******************************************************************************************************************//**
 * @brief       This function enables and puts the MCU in sleep mode.
 * @param[IN]   Requested LPM Mode and the pointer to it's instance
 * @retval      FSP_SUCCESS:    Upon successful entering sleep mode
 * @retval      Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t lpm_mode_enter(app_lpm_states_t lpm_mode, lpm_instance_ctrl_t * const p_current_ctrl)
{
    fsp_err_t err = FSP_SUCCESS;

    switch(lpm_mode)
    {
#if defined (BOARD_RA6M3_EK) || defined (BOARD_RA6M3G_EK) || defined (BOARD_RA6M2_EK) || defined (BOARD_RA6M1_EK) || defined (BOARD_RA4E1_FPB)
        case APP_LPM_DEEP_SW_STANDBY_STATE:
            /* Enter Deep SW standby mode */
            err = R_LPM_LowPowerModeEnter(p_current_ctrl);
            break;
#endif
        case APP_LPM_SW_STANDBY_STATE:
            /* Enter SW Standby mode */
            err = R_LPM_LowPowerModeEnter(p_current_ctrl);
            break;

        case APP_LPM_SLEEP_STATE:
            /* Enter Sleep mode */
            err = R_LPM_LowPowerModeEnter(p_current_ctrl);
            break;

        case APP_LPM_SW_STANDBY_SNOOZE_STATE:
            /* Enter SW Standby with Snooze enabled mode */
            err = R_LPM_LowPowerModeEnter(p_current_ctrl);
            break;

    	default:
    		/* return error */
    		err = FSP_ERR_INVALID_MODE;
    		break;
    }
	return err;
}

/*******************************************************************************************************************//**
 * @brief       This function is the Core of the FSM. Runs and transition the Clock and LPM State to new State.
 *              Initiates the Action required for the state change based on the event.
 * @param[IN]   event to the FSM
 * @retval      void
 **********************************************************************************************************************/

void  run_FSM(u_event_t  levent )
{
	clock_SM_t  l_clock_SM = {RESET_VALUE};
	lpm_SM_t  l_lpm_SM = {RESET_VALUE};

	event_type = EV_NONE;

	if(CLK_SM == running_state_machine)
    {
		l_clock_SM.clk_transition_SM = clock_transition_table[clk_currentState][levent];
		l_clock_SM.prv_state = clk_currentState;

        clk_currentState  = l_clock_SM.clk_transition_SM.next_state;
        (*l_clock_SM.clk_transition_SM.clk_action_to_do)();
    }
    else if (LPM_SM == running_state_machine)
    {
    	l_lpm_SM.lpm_tansition_SM = lpm_transition_table[lpm_current_state][levent];
    	l_lpm_SM.prv_state = lpm_current_state;

       	lpm_current_state = l_lpm_SM.lpm_tansition_SM.next_state;
        (*l_lpm_SM.lpm_tansition_SM.lpm_action_to_do)();
   }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup lpm_app)
 **********************************************************************************************************************/
