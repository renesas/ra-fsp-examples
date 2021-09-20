/***********************************************************************************************************************
 * File Name    : lpm_ep.c
 * Description  : Contains function definition.
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
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
#include "lpm_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup lpm_ep
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

    cgc_clock_t       		 sys_clock_source = {(cgc_clock_t)0};
    cgc_divider_cfg_t 		 sys_divider_cf   = {0};
    agt_extended_cfg_t const *p_agt1_extend	  = agt_timer1_snooze_trg_source_cfg.p_extend;

    /* Open CGC module */
    err = R_CGC_Open(&g_cgc0_ctrl, &g_cgc0_cfg);
    /* Handle error */
    if(FSP_SUCCESS == err)
    {
        /* Get system clock source */
        err =  R_CGC_SystemClockGet (&g_cgc0_ctrl, &sys_clock_source, &sys_divider_cf);
        /* Handle error */
        if(FSP_SUCCESS != err)
        {
            return err;
        }

        /* Disable unused clock */
        switch(sys_clock_source)
        {
            case CGC_CLOCK_PLL:
            case CGC_CLOCK_MAIN_OSC:
                /*
                 * LOCO, Subclock should be handled properly if using them as count sources for RTC, AGT timers
                 * e.g. avoid stop Subclock if it's used as RTC counter source
                 */
                /* Stop MOCO clock if it's unused */
                err = R_CGC_ClockStop(&g_cgc0_ctrl, CGC_CLOCK_MOCO);
                if(FSP_SUCCESS == err)
                {
                    /*
                     * Stop Sub-clock if it's used,
                     * Notes: Don't stop it if you use it as RTC counter source
                     */
                    err = R_CGC_ClockStop(&g_cgc0_ctrl, CGC_CLOCK_SUBCLOCK);
                    if(FSP_SUCCESS == err)
                    {
                        /* Stop LOCO clock if LOCO is unused (not use as AGT1 count source)*/
                        if(AGT_CLOCK_LOCO != p_agt1_extend->count_source)
                        {
                            err = R_CGC_ClockStop(&g_cgc0_ctrl, CGC_CLOCK_LOCO);
                        }
                    }
                }
                break;
            case CGC_CLOCK_HOCO:
                /* Stop MOCO clock if it's unused */
                err = R_CGC_ClockStop(&g_cgc0_ctrl, CGC_CLOCK_MOCO);
                if(FSP_SUCCESS == err)
                {
                    /*
                     * Stop Sub-clock if it's used,
                     * Notes: Don't stop it if you use it as RTC counter source
                     */
                    err = R_CGC_ClockStop(&g_cgc0_ctrl, CGC_CLOCK_SUBCLOCK);
                    if(FSP_SUCCESS == err)
                    {
                        /* Stop LOCO clock if LOCO is unused (not use as AGT1 count source)*/
                        if(AGT_CLOCK_LOCO != p_agt1_extend->count_source)
                        {
                            err = R_CGC_ClockStop(&g_cgc0_ctrl, CGC_CLOCK_LOCO);
                        }
                    }
                }
                break;
            case CGC_CLOCK_MOCO:
                /*
                 * Stop Sub-clock if it's used,
                 * Notes: Don't stop it if you use it as RTC counter source
                 */
                err = R_CGC_ClockStop(&g_cgc0_ctrl, CGC_CLOCK_SUBCLOCK);
                if(FSP_SUCCESS == err)
                {
                    /* Stop LOCO clock if LOCO is unused (not use as AGT1 count source)*/
                    if(AGT_CLOCK_LOCO != p_agt1_extend->count_source)
                    {
                        err = R_CGC_ClockStop(&g_cgc0_ctrl, CGC_CLOCK_LOCO);
                    }
                }
                break;
            case CGC_CLOCK_LOCO:
                /* Stop MOCO clock if it's unused */
                err = R_CGC_ClockStop(&g_cgc0_ctrl, CGC_CLOCK_MOCO);
                if(FSP_SUCCESS == err)
                {
                    /*
                     * Stop Sub-clock if it's used,
                     * Notes: Don't stop it if you use it as RTC counter source
                     */
                    err = R_CGC_ClockStop(&g_cgc0_ctrl, CGC_CLOCK_SUBCLOCK);
                }
                break;
            case CGC_CLOCK_SUBCLOCK:
                /* Stop MOCO clock if it's unused */
                err = R_CGC_ClockStop(&g_cgc0_ctrl, CGC_CLOCK_MOCO);
                if(FSP_SUCCESS == err)
                {
                    /* Stop LOCO clock if LOCO is unused (not use as AGT1 count source)*/
                    if(AGT_CLOCK_LOCO != p_agt1_extend->count_source)
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
    }//if FSP_SUCCESS == err
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function does necessary setups before entering SW Standby with Snooze enabled.
 * @param[IN]   None
 * @retval      FSP_SUCCESS                  Upon successful checking and starting LOCO clock, AGT1 timer
 * @retval      Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
static fsp_err_t pre_lpm_sw_standby_snooze(void)
{
    fsp_err_t err 							= FSP_SUCCESS;
    agt_extended_cfg_t const *p_agt1_extend = agt_timer1_snooze_trg_source_cfg.p_extend;

    /*
     * Check and start LOCO clock. LOCO is needed since it is selected as AGT1 timer counter source
     * CGC module is opened in user_clocks_set function
     */
    if(AGT_CLOCK_LOCO == p_agt1_extend->count_source)
    {
        if(CLOCK_STOP == R_SYSTEM->LOCOCR_b.LCSTP)
        {
            /* Start LOCO clock */
            err = R_CGC_ClockStart(&g_cgc0_ctrl, CGC_CLOCK_LOCO, NULL);
            /* Handle error */
            if(FSP_SUCCESS != err)
            {
                return err;
            }
            /* LOCO does not have Oscillation Stabilization Flag, wait for its stabilization by adding delay */
            R_BSP_SoftwareDelay(BSP_FEATURE_CGC_LOCO_STABILIZATION_MAX_US, BSP_DELAY_UNITS_MICROSECONDS);
        }
        /* Start AGT1 timer */
        err = R_AGT_Start(&agt_timer1_snooze_trg_source_ctrl);
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function does necessary setups after exiting from SW Standby with Snooze enabled.
 * @param[IN]   None
 * @retval      FSP_SUCCESS                  Upon successful stopping AGT1 timer
 * @retval      Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
static fsp_err_t post_lpm_sw_standby_snooze(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Stop AGT1 timer */
    err = R_AGT_Stop(&agt_timer1_snooze_trg_source_ctrl);
    if(FSP_SUCCESS == err)
    {
        /* Reset AGT1 timer */
        err = R_AGT_Reset(&agt_timer1_snooze_trg_source_ctrl);
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function enables and puts the MCU in sleep mode.
 * @param[IN]   None
 * @retval      FSP_SUCCESS                  Upon successful entering sleep mode
 * @retval      Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t lpm_mode_enter(app_lpm_states_t lpm_mode,lpm_instance_ctrl_t * const p_current_ctrl)
{
    fsp_err_t err = FSP_SUCCESS;

    switch(lpm_mode)
    {
#if defined (BOARD_RA6M3_EK) || defined (BOARD_RA6M3G_EK) || defined (BOARD_RA6M2_EK) || defined (BOARD_RA6M1_EK) || defined (BOARD_RA6M4_EK) || defined (BOARD_RA6T1_RSSK) || defined (BOARD_RA4M3_EK)|| defined (BOARD_RA4M2_EK)|| defined (BOARD_RA6M5_EK)||defined (BOARD_RA6E1_FPB)
        case APP_LPM_DEEP_SW_STANDBY_STATE:
            /* Enter Deep SW standby mode */
            err = R_LPM_LowPowerModeEnter(p_current_ctrl);
            break;
            /*
             * Note that MCU will reset after exited from Deep SW Standby (internal reset state)
             */
#endif
        case APP_LPM_SW_STANDBY_STATE:
            /* Enter SW Standby mode */
            err = R_LPM_LowPowerModeEnter(p_current_ctrl);
            break;

        case APP_LPM_SLEEP_STATE:
            /* Stop push button deboucing filter timer (AGT0) to prevent it waking up the MCU from Sleep mode */
            err = R_AGT_Stop(&agt_timer0_sw_debounce_filter_ctrl);
            if(FSP_SUCCESS == err)
            {
                /* Enter Sleep mode */
                err = R_LPM_LowPowerModeEnter(p_current_ctrl);
                if (FSP_SUCCESS == err)
                {
                    /*
                     * Exited from Sleep mode
                     */
                    /* Restart push button debouncing filter timer  */
                    err = R_AGT_Start(&agt_timer0_sw_debounce_filter_ctrl);
                }
            }
            break;

        case APP_LPM_SW_STANDBY_SNOOZE_STATE:
            /* Set up clock, AGT timer before entering SW Standby with Snooze enabled*/
            err = pre_lpm_sw_standby_snooze();
            /* Handle error */
            if (FSP_SUCCESS == err)
            {
                /* Enter SW Standby with Snooze enabled mode */
                err = R_LPM_LowPowerModeEnter(p_current_ctrl);
                /* Handle error */
                if (FSP_SUCCESS == err)
                {
                    /*
                     * Exited from SW Standby with Snooze enabled
                     */
                    /* Handle clock, trigger source*/
                    err = post_lpm_sw_standby_snooze();
                }
            }
            break;

        default:
            /* return error */
            err = FSP_ERR_INVALID_MODE;
            break;
    }
    return err;
}
/*******************************************************************************************************************//**
 * @} (end addtogroup lpm_ep)
 **********************************************************************************************************************/
