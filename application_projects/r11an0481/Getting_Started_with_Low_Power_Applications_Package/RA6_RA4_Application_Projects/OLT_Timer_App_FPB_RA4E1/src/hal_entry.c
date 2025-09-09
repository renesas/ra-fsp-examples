/***********************************************************************************************************************
 * File Name    : hal_entry.c
 * Description  : Contains function for application entry.
 **********************************************************************************************************************/
/**********************************************************************************************************************
* Copyright (c) 2019 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/
#include "hal_data.h"
#include "bsp_pin_cfg.h"
#include "r_ioport.h"
#include "lpm_app.h"
#include "lpm_app_transition_sequence_config.h"
/* Board's user LED */
extern bsp_leds_t g_bsp_leds;
/* Boolean flag to determine user push-button/switch is pressed or not. */
volatile bool g_user_sw_press                 = false;
/* This keeps transition position of LPM sequence*/
static volatile uint8_t g_lpm_transition_pos  = 0;

void R_BSP_WarmStart(bsp_warm_start_event_t event);

/*******************************************************************************************************************//**
 * The RA Configuration tool generates main() and uses it to generate threads if an RTOS is used.  This function is
 * called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
    fsp_err_t err             = FSP_SUCCESS;

    /* ctrl and cfg instances of LPM modes; these should be initialized following the order of LPM transition sequence in lpm_ep_transition_sequence_config.h */
#if defined (BOARD_RA6M3_EK) || defined (BOARD_RA6M3G_EK) || defined (BOARD_RA6M2_EK) || defined (BOARD_RA6M1_EK) || defined (BOARD_RA6E1_FPB) || defined (BOARD_RA4E1_FPB)
    lpm_instance_ctrl_t g_lpm_ctrl_instance_ctrls[] = {g_lpm_sw_standby_ctrl,
													   g_lpm_deep_sw_standby_ctrl
    												   };
    lpm_cfg_t			g_lpm_ctrl_instance_cfgs[]  = {g_lpm_sw_standby_cfg,
													   g_lpm_deep_sw_standby_cfg};
#elif defined (BOARD_RA4M1_EK) || defined (BOARD_RA2A1_EK)
    lpm_instance_ctrl_t g_lpm_ctrl_instance_ctrls[] = {g_lpm_sw_standby_ctrl};
    lpm_cfg_t			g_lpm_ctrl_instance_cfgs[]  = {g_lpm_sw_standby_cfg};
#endif
    /* LED type structure */
    bsp_leds_t leds = g_bsp_leds;

    /* Start Sub Clock and disable unused clocks that are automatically enabled when reset*/
    err = user_clocks_set();
    /* Handle error */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_TRAP(err);
    }

    /*
     * GPT timer is used to filter debouncing issue of user switch (push-button)
     */
    err = gpt_init_start();
    /* Handle error */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_TRAP(err);
    }
    /* Initialize External IRQ driver/user push-button */
    err = user_sw_init();
    /* Handle error */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_TRAP(err);
    }

    /*
     *  Open AGT timers 0 & 1
     */
	err = agt_timer_init();
	if (FSP_SUCCESS != err)
	{
		APP_ERR_TRAP(err);
	}

    /* If this board has no LED, then trap here */
    if (0 == leds.led_count)
    {
    	/* Handle error */
        APP_ERR_TRAP(err);
    }

    /* Main loop */
    while (true)
      {
          /* Toggle user LED */
    	  err = user_led_toggle(leds);
    	  /* Handle error */
    	  if (FSP_SUCCESS != err)
    	  {
    		  APP_ERR_TRAP(err);
    	  }
    	  /* Stop AGT timers if they are counting */
    	  err = agt_timer_stop();
    	  if (FSP_SUCCESS != err)
    	  {
    		  APP_ERR_TRAP(err);
    	  }
          /* If user pushbutton is pressed, entering LPM mode */
          if(true == g_user_sw_press)
          {
              /* Clear user pushbutton flag */
              g_user_sw_press = false;

              /* Start AGT Timers in Cascade mode */
              err =  agt_timer_start();
        	  if (FSP_SUCCESS != err)
        	  {
        		  APP_ERR_TRAP(err);
        	  }
              /* Reset transition sequence if reached normal state */
              if(APP_LPM_NORMAL_STATE == g_lpm_transition_sequence[g_lpm_transition_pos])
              {
            	  /* Restart LPM sequence */
        		  g_lpm_transition_pos = 0;
              }

			  /* Disable IO port if it's not in Deep SW Standby mode */
			  if(APP_LPM_DEEP_SW_STANDBY_STATE != g_lpm_transition_sequence[g_lpm_transition_pos])
			  {
				  /* Disable IO port before going to LPM mode*/
				  err = R_IOPORT_PinsCfg(&g_ioport_ctrl, &g_bsp_pin_lpm_cfg);
				  /* Handle error */
				  if(FSP_SUCCESS != err)
				  {
					  APP_ERR_TRAP(err);
				  }
			  }
			  /* Open LPM instance*/
			  err = R_LPM_Open(&g_lpm_ctrl_instance_ctrls[g_lpm_transition_pos], &g_lpm_ctrl_instance_cfgs[g_lpm_transition_pos]);
			  /* Handle error */
			  if (FSP_SUCCESS != err)
			  {
				  APP_ERR_TRAP(err);
			  }
			  /* Enter LPM mode */
			  err = lpm_mode_enter(g_lpm_transition_sequence[g_lpm_transition_pos]);
			  /* Handle error */
			  if (FSP_SUCCESS != err)
			  {
				  /* Turn on user LED to indicate error occurred*/
				  R_IOPORT_PinWrite(&g_ioport_ctrl, leds.p_leds[LED_NO_0], BSP_IO_LEVEL_HIGH);
				  APP_ERR_TRAP(err);
			  }
			  else
			  {
				  /* The below code won't be executed in Deep Software Standby mode */
				  /* Close LPM instance.*/
				  err = R_LPM_Close(&g_lpm_ctrl_instance_ctrls[g_lpm_transition_pos]);
				  /* Handle error */
				  if(FSP_SUCCESS != err)
				  {
					  APP_ERR_TRAP(err);
				  }
				  /* Put IO port configuration back to user's selections */
		    	  err = R_IOPORT_PinsCfg(&g_ioport_ctrl, &g_bsp_pin_cfg);
				  /* Handle error */
				  if(FSP_SUCCESS != err)
				  {
					  APP_ERR_TRAP(err);
				  }
				  /* Move to Next LPM state */
				  g_lpm_transition_pos++;
			  }
          }//if g_user_sw_press
      }//End of main loop
}

/*******************************************************************************************************************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart(bsp_warm_start_event_t event)
{
	if (BSP_WARM_START_POST_C == event)
	{
		/* C runtime environment and system clocks are setup. */

		/* Release IO from IOKEEP state if reset from Deep SW Standby mode */
		if(1 == R_SYSTEM->RSTSR0_b.DPSRSTF)
		{
			/* Input parameter is unused */
			R_LPM_IoKeepClear(NULL);

            /* Restart LPM sequence */
            g_lpm_transition_pos = 0;

			/* Clear DPSRSTF flag */
			R_SYSTEM->RSTSR0_b.DPSRSTF 			= 0;
		}

		/* Configure pins. */
		R_IOPORT_Open(&g_ioport_ctrl, &g_bsp_pin_cfg);
	}
}

