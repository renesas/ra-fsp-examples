/***********************************************************************************************************************
 * File Name    : lpm_app.c
 * Description  : Contains function definition.
 **********************************************************************************************************************/
/**********************************************************************************************************************
* Copyright (c) 2019 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/
#include "lpm_app.h"

/*******************************************************************************************************************//**
 * @addtogroup lpm_app
 * @{
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief       This function disables unused clocks that are automatically enabled by default.
 * @param[IN]   None
 * @retval      FSP_SUCCESS                  Upon successful set up the clocks
 * @retval      Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t user_clocks_set(void)
{
    fsp_err_t err = FSP_SUCCESS;

    cgc_clock_t       		 sys_clock_source = {0};
	cgc_divider_cfg_t 		 sys_divider_cf   = {0};
	agt_extended_cfg_t const *p_agt0_extend	  = agt0_timer_cascade_trigger_cfg.p_extend;

    /* Open CGC module */
    err = R_CGC_Open(&g_cgc0_ctrl, &g_cgc0_cfg);
    /* Handle error */
    if(FSP_SUCCESS == err)
    {
		/*
		 * Start Sub clock, which is used as clock source for AGT0
		 */
		err = R_CGC_ClockStart(&g_cgc0_ctrl, CGC_CLOCK_SUBCLOCK, NULL);
		if(FSP_SUCCESS == err)
		{
			/* Get system clock source */
			err =  R_CGC_SystemClockGet (&g_cgc0_ctrl, &sys_clock_source, &sys_divider_cf);
			/* Handle error */
			if(FSP_SUCCESS == err)
			{
				/* Disable unused clock */
				switch(sys_clock_source)
				{
					case CGC_CLOCK_PLL:
					case CGC_CLOCK_MAIN_OSC:
						/*
						 * LOCO should be handled properly if using them as count sources for RTC, AGT timers
						 * e.g. avoid stop if it's used as RTC counter source
						 */
						/* Stop MOCO clock if it's unused */
						err = R_CGC_ClockStop(&g_cgc0_ctrl, CGC_CLOCK_MOCO);
						if(FSP_SUCCESS == err)
						{
							/* Stop LOCO clock if LOCO is unused (not use as AGT1 count source)*/
							if(AGT_CLOCK_LOCO != p_agt0_extend->count_source)
							{
								err = R_CGC_ClockStop(&g_cgc0_ctrl, CGC_CLOCK_LOCO);
							}
						}
						break;
					case CGC_CLOCK_HOCO:
						/* Stop MOCO clock if it's unused */
						err = R_CGC_ClockStop(&g_cgc0_ctrl, CGC_CLOCK_MOCO);
						if(FSP_SUCCESS == err)
						{
							/* Stop LOCO clock if LOCO is unused (not use as AGT1 count source)*/
							if(AGT_CLOCK_LOCO != p_agt0_extend->count_source)
							{
								err = R_CGC_ClockStop(&g_cgc0_ctrl, CGC_CLOCK_LOCO);
							}
						}
						break;
					case CGC_CLOCK_MOCO:
						/* Stop LOCO clock if it's unused (not use as AGT1 count source)*/
						if(AGT_CLOCK_LOCO != p_agt0_extend->count_source)
						{
							err = R_CGC_ClockStop(&g_cgc0_ctrl, CGC_CLOCK_LOCO);
						}
						break;
					case CGC_CLOCK_LOCO:
						/* Stop MOCO clock if it's unused */
						err = R_CGC_ClockStop(&g_cgc0_ctrl, CGC_CLOCK_MOCO);
						break;
					case CGC_CLOCK_SUBCLOCK:
						/* Stop MOCO clock if it's unused */
						err = R_CGC_ClockStop(&g_cgc0_ctrl, CGC_CLOCK_MOCO);
						if(FSP_SUCCESS == err)
						{
							/* Stop LOCO clock if LOCO is unused (not use as AGT1 count source)*/
							if(AGT_CLOCK_LOCO != p_agt0_extend->count_source)
							{
								err = R_CGC_ClockStop(&g_cgc0_ctrl, CGC_CLOCK_LOCO);
							}
						}
						break;
					default:
						/* return error */
						err = FSP_ERR_CLOCK_INACTIVE;
						break;
				}//switch sys_clock_source
			}//if FSP_SUCCESS == err (R_CGC_SystemClockGet)
		}//if FSP_SUCCESS == err (Sub clock)
    }//if FSP_SUCCESS == err
	return err;
}

/*******************************************************************************************************************//**
 * @brief       This function enables and puts the MCU in sleep mode.
 * @param[IN]   None
 * @retval      FSP_SUCCESS                  Upon successful entering sleep mode
 * @retval      Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t lpm_mode_enter(app_lpm_states_t lpm_mode)
{
    fsp_err_t err = FSP_SUCCESS;

    switch(lpm_mode)
    {
#if defined (BOARD_RA6M3_EK) || defined (BOARD_RA6M3G_EK) || defined (BOARD_RA6M2_EK) || defined (BOARD_RA6M1_EK)
    	case APP_LPM_DEEP_SW_STANDBY_STATE:
			/* Enter Deep SW standby mode */
			err = R_LPM_LowPowerModeEnter(&g_lpm_deep_sw_standby_ctrl);
			break;
			/*
			 * Note that MCU will reset after exited from Deep SW Standby (internal reset state)
			 */
#endif
    	case APP_LPM_SW_STANDBY_STATE:
			/* Enter SW Standby mode */
			err = R_LPM_LowPowerModeEnter(&g_lpm_sw_standby_ctrl);
			break;

    	default:
    		/* return error */
    		err = FSP_ERR_INVALID_MODE;
    		break;
    }
	return err;
}
/*******************************************************************************************************************//**
 * @} (end addtogroup lpm_app)
 **********************************************************************************************************************/
