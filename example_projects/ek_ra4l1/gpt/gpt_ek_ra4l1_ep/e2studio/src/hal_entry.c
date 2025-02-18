/***********************************************************************************************************************
 * File Name    : hal_entry.c
 * Description  : Entry function.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "gpt_timer.h"

/*******************************************************************************************************************//**
 * @addtogroup r_gpt_ep
 * @{
 **********************************************************************************************************************/

void R_BSP_WarmStart(bsp_warm_start_event_t event);

/*******************************************************************************************************************//**
 * The RA Configuration tool generates main() and uses it to generate threads if an RTOS is used.  This function is
 * called by main() when no RTOS is used.
 **********************************************************************************************************************/

/* value to validate open state of timer instances */
extern uint8_t g_timer_open_state;
extern bool volatile g_one_shot_expired;

void hal_entry(void)
{
	fsp_err_t err                           = FSP_SUCCESS;
	unsigned char rByte[BUF_SIZE]           = {INITIAL_VALUE};
	uint32_t read_data                      = RESET_VALUE;
	uint32_t gpt_desired_duty_cycle_percent = RESET_VALUE;
	uint32_t gpt_desired_period_ms          = RESET_VALUE;
	uint64_t period_counts                  = RESET_VALUE;
	uint32_t pclkd_freq_hz                  = RESET_VALUE;
	int32_t  one_shot_timeout               = INT32_MAX;
	fsp_pack_version_t version;

	/* version get API for FLEX pack information */
	R_FSP_VersionGet(&version);

	/* Example Project information printed on the Console */
	APP_PRINT(BANNER_1);
	APP_PRINT(BANNER_2);
	APP_PRINT(BANNER_3,EP_VERSION);
	APP_PRINT(BANNER_4,version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
	APP_PRINT(BANNER_5);
	APP_PRINT(BANNER_6);
	APP_PRINT(EP_INFO);

	/* Print Menu option of GPT timer*/
	print_timer_menu();

	while(true)
	{
		/* Process input only when User has provided one */
		if (APP_CHECK_DATA)
		{
			/* Read First byte of data provided by user */
			APP_READ(rByte);

			/* Conversion from input string to integer value */
			read_data = (uint32_t) (atoi((char *)rByte));

			switch (read_data)
			{
			case PERIODIC_MODE_TIMER:
			{
				/* Check the status of GPT timer in PWM mode */
				if(PWM_MODE == g_timer_open_state)
				{
					/* Close PWM Timer instance */
					deinit_gpt_timer(&g_timer_pwm_ctrl);
					APP_PRINT("PWM Timer Instance Closed, Start Periodic Timer Instance\r\n");
				}

				/* Check the status of GPT timer in ONE-SHOT mode */
				if(ONE_SHOT_MODE == g_timer_open_state)
				{
					/* Close ONE_SHOT GPT Timer instance */
					deinit_gpt_timer(&g_timer_one_shot_mode_ctrl);
					APP_PRINT("ONE_SHOT GPT Timer Instance Closed, Start Periodic Timer Instance\r\n");
				}

				APP_PRINT ("\r\n Enter the desired period in millisecond\r\n");
				gpt_desired_period_ms = process_input_data();

				/* Get the source clock frequency (in Hz) */
				pclkd_freq_hz = R_FSP_SystemClockHzGet(FSP_PRIV_CLOCK_PCLKD);
				pclkd_freq_hz >>= (uint32_t)(g_timer_periodic_cfg.source_div);

				/* Convert period to PCLK counts so it can be set in hardware. */
				period_counts = (uint64_t)((gpt_desired_period_ms * (pclkd_freq_hz * CLOCK_TYPE_SPECIFIER))\
				                / TIMER_UNITS_MILLISECONDS);

				/* Validate Period Count based on user input (time period in ms) */
				if(GPT_MAX_PERIOD_COUNT < period_counts)
				{
					APP_ERR_PRINT ("\r\n ** INVALID INPUT, DESIRED PERIOD IS OUT OF RANGE. ** \r\n");
				}
				else
				{
					/* Check the status of GPT timer in Periodic mode */
					if(PERIODIC_MODE != g_timer_open_state)
					{
						/*Initialize Periodic Timer */
						err = init_gpt_timer(&g_timer_periodic_ctrl, &g_timer_periodic_cfg, PERIODIC_MODE_TIMER);
						if(FSP_SUCCESS != err)
						{
							APP_ERR_PRINT("** GPT TIMER INIT FAILED ** \r\n");
							APP_ERR_TRAP(err);
						}
						APP_PRINT("Opened Timer in Periodic Mode\r\n");

						/* Start Periodic Timer*/
						err = start_gpt_timer(&g_timer_periodic_ctrl);
						if(FSP_SUCCESS != err)
						{
							APP_ERR_PRINT("** GPT TIMER START FAILED ** \r\n");
							/*Close Periodic Timer instance */
							deinit_gpt_timer(&g_timer_periodic_ctrl);
							APP_ERR_TRAP(err);
						}
						APP_PRINT("Started Timer in Periodic Mode\r\n");
					}
					else
					{
						APP_PRINT("Periodic Timer Already Started, Update Timer Period to : %d\r\n",
						          gpt_desired_period_ms);
					}

					/* Period Set API set the desired period counts on the on-board LED */
					err = R_GPT_PeriodSet(&g_timer_periodic_ctrl, (uint32_t)period_counts);
					if (FSP_SUCCESS != err)
					{
						/* GPT Timer PeriodSet Failure message */
						APP_ERR_PRINT ("\r\n ** R_GPT_PeriodSet API failed ** \r\n");
						/*Close Periodic Timer instance */
						deinit_gpt_timer(&g_timer_periodic_ctrl);
						APP_ERR_TRAP(err);
					}
				}
				break;
			}

			case PWM_MODE_TIMER:
			{
				/* Check the status of GPT timer in Periodic mode */
				if( PERIODIC_MODE == g_timer_open_state )
				{
					/* Close Periodic Timer instance */
					deinit_gpt_timer(&g_timer_periodic_ctrl);
					APP_PRINT("Periodic Timer Instance Closed, Start PWM Timer Instance\r\n");
				}

				/* Check the status of GPT timer in ONE-SHOT mode */
				if(ONE_SHOT_MODE == g_timer_open_state)
				{
					/* Close ONE-SHOT Timer instance */
					deinit_gpt_timer(&g_timer_one_shot_mode_ctrl);
					APP_PRINT("ONE-SHOT Timer Instance Closed, Start PWM Timer Instance\r\n");
				}

				APP_PRINT ("\r\nEnter The Desired Duty Cycle in Percentage of Range 0 - 100:\r\n");
				gpt_desired_duty_cycle_percent = process_input_data();

				/* Validate Duty cycle percentage */
				if (GPT_MAX_PERCENT < gpt_desired_duty_cycle_percent)
				{
					APP_ERR_PRINT ("\r\n ** INVALID INPUT, DESIRED DUTY CYCLE IS OUT OF RANGE. ** \r\n");
				}
				else
				{
					/* Check the status of GPT timer in PWM mode */
					if(PWM_MODE != g_timer_open_state )
					{
						/* we got valid input, Initialize PWM timer */
						err = init_gpt_timer(&g_timer_pwm_ctrl, &g_timer_pwm_cfg, PWM_MODE_TIMER);
						if(FSP_SUCCESS != err)
						{
							APP_ERR_PRINT("** GPT TIMER INIT FAILED ** \r\n");
							APP_ERR_TRAP(err);
						}
						APP_PRINT("Opened Timer in PWM Mode\r\n");

						/* Start PWM Timer*/
						err = start_gpt_timer(&g_timer_pwm_ctrl);
						if(FSP_SUCCESS != err)
						{
							APP_ERR_PRINT("** GPT TIMER START FAILED ** \r\n");
							/*Close PWM Timer instance */
							deinit_gpt_timer(&g_timer_pwm_ctrl);
							APP_ERR_TRAP(err);
						}
						APP_PRINT("Started Timer in PWM Mode\r\n");
					}
					else
					{
						/* Print Duty cycle on RTT, when PWM Timer already started*/
						APP_PRINT("PWM Timer Already started, Update Duty Cycle to: %d\r\n",
						          gpt_desired_duty_cycle_percent);
					}

					/* Set DutyCycle of PWM timer */
					err = set_timer_duty_cycle((uint8_t)gpt_desired_duty_cycle_percent);
					if(FSP_SUCCESS != err)
					{
						/* GPT Timer DutyCycleSet Failure message */
						APP_ERR_PRINT ("\r\n ** GPT TIMER DUTYCYCLE SET FAILED ** \r\n");
						/*Close PWM Timer instance */
						deinit_gpt_timer(&g_timer_pwm_ctrl);
						APP_ERR_TRAP(err);
					}
				}
				break;
			}

			case ONE_SHOT_MODE_TIMER :
			{
			    /* Check the status of GPT timer in Periodic mode */
			    if( PERIODIC_MODE == g_timer_open_state )
			    {
			        /* Close Periodic Timer instance */
			        deinit_gpt_timer(&g_timer_periodic_ctrl);
			        APP_PRINT("Periodic Timer Instance Closed, Start ONE-SHOT Timer Instance\r\n");
			    }

			    /* Check the status of GPT timer in PWM mode */
			    if(PWM_MODE == g_timer_open_state)
			    {
			        /* Close PWM Timer instance */
			        deinit_gpt_timer(&g_timer_pwm_ctrl);
			        APP_PRINT("PWM Timer Instance Closed, Start ONE-SHOT Timer Instance\r\n");
			    }

			    /* Check the status of GPT timer in ONE-SHOT mode */
			    if(ONE_SHOT_MODE == g_timer_open_state)
			    {
			        /* Close ONE-SHOT Timer instance */
			        deinit_gpt_timer(&g_timer_one_shot_mode_ctrl);
			        APP_PRINT("ONE-SHOT Timer Instance Closed, If Already Opened\r\n");
			    }

			    /*Initialize ONE-SHOT Timer */
			    err = init_gpt_timer(&g_timer_one_shot_mode_ctrl, &g_timer_one_shot_mode_cfg, ONE_SHOT_MODE_TIMER);
			    if(FSP_SUCCESS != err)
			    {
			        APP_ERR_PRINT("** GPT TIMER INIT FAILED ** \r\n");
			        APP_ERR_TRAP(err);
			    }
			    APP_PRINT("Opened Timer in ONE-SHOT Mode\r\n");

			    /* Start ONE-SHOT Timer */
			    err = start_gpt_timer(&g_timer_one_shot_mode_ctrl);
			    if(FSP_SUCCESS != err)
			    {
			        APP_ERR_PRINT("** GPT TIMER START FAILED ** \r\n");
			        /*Close ONE-SHOT Timer instance */
			        deinit_gpt_timer(&g_timer_one_shot_mode_ctrl);
			        APP_ERR_TRAP(err);
			    }
			    APP_PRINT("Started Timer in ONE-SHOT Mode\r\n");

			    /* wait for one-shot mode timer to expire. */
			    while(true != g_one_shot_expired)
			    {
			        /* start checking for time out to avoid infinite loop */
			        --one_shot_timeout;
			        if (RESET_VALUE == one_shot_timeout)
			        {
			            /* we have reached to a scenario where One-shot event did not occur */
			            APP_ERR_PRINT ("callback event not received during One-Shot operation\r\n");
			            break;
			        }
			    }

			    if(true == g_one_shot_expired)
			    {
			        APP_PRINT("\r\n Timer Expired in One-Shot Mode\r\n");
			    }
			    g_one_shot_expired = false;
			    break;
			}

			default:
			{
				APP_PRINT("\r\nInvalid Menu Option Selected\r\n");
				break;
			}
			}

			read_data = RESET_VALUE;
			/* Reset buffer*/
			memset (rByte, INITIAL_VALUE, BUF_SIZE);
			/* Print Menu option of GPT timer*/
			print_timer_menu();
		}

		else
		{
			/* do nothing */
		}
	}
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

		/* Configure pins. */
		R_IOPORT_Open (&g_ioport_ctrl, &g_bsp_pin_cfg);
	}
}

/*******************************************************************************************************************//**
 * @} (end addtogroup r_gpt_ep)
 **********************************************************************************************************************/
