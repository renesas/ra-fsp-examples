/***********************************************************************************************************************
 * File Name    : hal_entry.c
 * Description  : Contains function for application entry.
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

#include "bsp_pin_cfg.h"
#include "common_utils.h"
#include "hal_data.h"
#include "lpm_app_util.h"
#include "r_ioport.h"
#include "r_lpm_api.h"


/***********************************************************************************************************************
 * The RA Configuration tool generates main(). When application uses bare metal coding, this function is called from
 * main(). In non RTOS based application, this is the entry point to the Application.
 **********************************************************************************************************************/
void hal_entry(void)
{
	fsp_err_t err = FSP_SUCCESS;
	fsp_pack_version_t version = { RESET_VALUE };

    /* Gets FSP version */
    R_FSP_VersionGet(&version);

    /* Application Project information printed on to the Console */
    APP_PRINT(BANNER_INFO, AP_VERSION,version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    APP_PRINT(AP_INFO);
    USER_NOTE_PRINT
	/*
	 * Initialize the CGC(Clock Generation Circuit) module. CGC module APIs are used to change the required clock
	 * source dynamically as part of this application .
	 */

	err = init_cgc();
    /* Handle error */
    if(FSP_SUCCESS != err)
    {
		APP_PRINT("\r\n CGC Initialization Failed \r\n");
    	INIT_FAILURE
        APP_ERR_TRAP(err);
    }

    /*
     * Initialize the RTC Module. RTC is used for Time keeping purpose. In this application RTC time will be
     * displayed between the different LPM  transitions. This also show cases the RTC running in Lower Power Modes.
     * Also RTC Alarm interrupt is used as Timer event in Software Standby Mode to  transition to the Normal Mode.
     */
  	err = init_rtc();
  	if(FSP_SUCCESS != err)
  	{
		APP_PRINT("\r\n RTC Initialization Failed \r\n");
        INIT_FAILURE
  	    APP_ERR_TRAP(err);
  	}


    APP_PRINT("\r\nMCU Wake Up from Reset : Power on Reset \r\n");
  	set_rtc_calendar_time();
   	event_type = EV_POWER_ON_RESET;


    /* Open AGT1 timer in Periodic mode. In this Application AGT1 Periodic timer is used to toggle the LPM mode between
     * Software standby mode and Snooze mode. This timer is only available when the MCU is configured to operate in
     * "Snooze with Software Standby" mode. */
    err = R_AGT_Open(&agt_timer1_snooze_trg_source_ctrl, &agt_timer1_snooze_trg_source_cfg);
    /* Handle error */
    if(FSP_SUCCESS != err)
    {
		APP_PRINT("\r\n AGT Timer Initialization Failed \r\n");
        INIT_FAILURE
        APP_ERR_TRAP(err);
    }

    /* Initialize External IRQ driver/user push-button. User Push buttons are used for user input event */
    err = user_switch_init();
    /* Handle error */
    if(FSP_SUCCESS != err)
    {
		APP_PRINT("\r\n User Push Button IRQ  Initialization Failed \r\n");
        INIT_FAILURE
        APP_ERR_TRAP(err);
    }

    /* If this board has no LED, then trap here. */
    if (NONE == leds->led_count)
    {
		APP_PRINT("\r\n Board has No LEDs \r\n");
    	/* Handle error */
        APP_ERR_TRAP(err);
    }

	/* If the Code has come to this point indicates the initialization is successful, so clear the LEDs */
	err = user_led_off(leds);
	/* Handle error */
	if (FSP_SUCCESS != err)
	{
	  APP_ERR_TRAP(err);
	}

	/* Main loop */
	while (true)
	{
		/* Process the event  */
        switch(event_type)
        {
            case EV_PB_LONG_PR:
	        {
	        	CURRENT_FSM_PRINT
	        	FSM_ENTRY_PRINT
				run_FSM(EV_PB_LONG_PR);
	        }
	        break;
            case EV_PB_SHORT_PR:
	        {
                if(CLK_SM == running_state_machine)
                {
                	CURRENT_FSM_PRINT
                	FSM_ENTRY_PRINT
        	        run_FSM(EV_PB_SHORT_PR);
					running_state_machine = LPM_SM;
                }
                else
                {
                	CURRENT_FSM_PRINT
        	        FSM_ENTRY_PRINT
        	        run_FSM(EV_PB_SHORT_PR);
                }
	        }
	        break;
            case EV_PERIODIC_TIMER:
            {
	    	    if(LPM_SM == running_state_machine)
	    	    {
	    	    	CURRENT_FSM_PRINT
	    	    	FSM_ENTRY_PRINT
					run_FSM(EV_PERIODIC_TIMER);
	    	    }
	        }
            break;
            case EV_RTC_ALARM:
	        {
	    	    if(LPM_SM == running_state_machine)
	    	    {
	    	    	CURRENT_FSM_PRINT
	    	    	FSM_ENTRY_PRINT
					/* For Info: RTC event is also considered as Periodic Timer event in this application */
					run_FSM(EV_PERIODIC_TIMER);
	    	    }
	        }
            break;
            /* Power on Reset and Reset from Deep Software Standby mode are prior to the FSM entry
             * These are not used in the Transition table. Added here for User Notification of the Reset status
             */
            case EV_POWER_ON_RESET:
	        {
	        	PRE_FSM_ENTRY_PRINT
				event_type = EV_NONE;
				read_and_print_rtc_time();
	        }
	        break;
            default:
            {
    	        /* Toggle the User LED to indicate the application is running in the Normal Mode */
            	err = user_led_toggle(leds);
    		    /* Handle error */
    		    if (FSP_SUCCESS != err)
    		    {
    		        APP_ERR_TRAP(err);
    		    }
            }
        }
	} //End of main loop
}

/*******************************************************************************************************************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event :  Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart(bsp_warm_start_event_t event)
{
	if (BSP_WARM_START_POST_C == event)
	{
		/* C runtime environment and system clocks are setup. */
		/* Release IO from IOKEEP state if reset from Deep SW Standby mode, and capture the reset status */
		if(1 == R_SYSTEM->RSTSR0_b.DPSRSTF)
		{
			/* Input parameter is unused */
			R_LPM_IoKeepClear(NULL);

			/* Clear DPSRSTF flag */
			R_SYSTEM->RSTSR0_b.DPSRSTF = RESET_VALUE;
		}
		/* Configure pins. */
		R_IOPORT_Open(&g_ioport_ctrl, &g_bsp_pin_cfg);
	}
}

