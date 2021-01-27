/***********************************************************************************************************************
 * File Name    : hal_entry.c
 * Description  : Contains declarations of data structures and functions used in hal_entry.c.
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
#include "hal_data.h"
#include "bsp_pin_cfg.h"
#include "r_ioport.h"
#include "lpm_ep.h"
#include "lpm_ep_transition_sequence_config.h"

/* Board's user LED */
extern bsp_leds_t g_bsp_leds;
/* Boolean flag to determine user push-button/switch is pressed or not. */
volatile bool g_user_sw_press                 = false;
/* This keeps transition position of LPM sequence*/
static uint8_t g_lpm_transition_pos  = 0;

void R_BSP_WarmStart(bsp_warm_start_event_t event);

/*******************************************************************************************************************//**
 * The RA Configuration tool generates main() and uses it to generate threads if an RTOS is used.  This function is
 * called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
    fsp_err_t err                                   = FSP_SUCCESS;
    /* ctrl and cfg instances of LPM modes; these should be initialized following the order of LPM transition sequence
     * in lpm_ep_transition_sequence_config.h */
#if defined (BOARD_RA6M4_EK) || defined (BOARD_RA6M3_EK) || defined (BOARD_RA6M3G_EK) || defined (BOARD_RA6M2_EK) || defined (BOARD_RA6M1_EK) || defined (BOARD_RA6T1_RSSK) || defined (BOARD_RA4M3_EK)|| defined (BOARD_RA4M2_EK)
    lpm_instance_ctrl_t g_lpm_ctrl_instance_ctrls[] = {g_lpm_sleep_ctrl,
                                                       g_lpm_sw_standby_ctrl,
                                                       g_lpm_sw_standby_with_snooze_ctrl,
                                                       g_lpm_deep_sw_standby_ctrl
    };
    lpm_cfg_t			g_lpm_ctrl_instance_cfgs[]  = {g_lpm_sleep_cfg,
             			                               g_lpm_sw_standby_cfg,
             			                               g_lpm_sw_standby_with_snooze_cfg,
             			                               g_lpm_deep_sw_standby_cfg};
#elif defined (BOARD_RA4M1_EK) || defined (BOARD_RA2A1_EK) || defined (BOARD_RA2L1_EK) || defined(BOARD_RA2E1_EK)
    lpm_instance_ctrl_t g_lpm_ctrl_instance_ctrls[] = {g_lpm_sleep_ctrl,
                                                       g_lpm_sw_standby_ctrl,
                                                       g_lpm_sw_standby_with_snooze_ctrl
    };
    lpm_cfg_t			g_lpm_ctrl_instance_cfgs[]  = {g_lpm_sleep_cfg,
             			                               g_lpm_sw_standby_cfg,
             			                               g_lpm_sw_standby_with_snooze_cfg};
#endif
    /* LED type structure */
    bsp_leds_t leds = g_bsp_leds;

    /* Disable unused clocks that are automatically enabled when reset*/
    err = user_clocks_set();
    /* Handle error */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_TRAP(err);
    }

    /* Open AGT1 timer in Periodic mode */
    err = R_AGT_Open(&agt_timer1_snooze_trg_source_ctrl, &agt_timer1_snooze_trg_source_cfg);
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
     * AGT0 timer is used to filter debouncing issue of user switch (push-button)
     */
    err = agt0_init_start();
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

    /* Toggle user LED */
    err = user_led_toggle(leds);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_TRAP(err);
    }

    /* Main loop */
    while (true)
    {
        /* If user pushbutton is pressed, entering LPM mode */
        if(true == g_user_sw_press)
        {
            /* Clear user pushbutton flag */
            g_user_sw_press = false;

            /* Reset transition sequence if reached normal state */
            if(APP_LPM_NORMAL_STATE == g_lpm_transition_sequence[g_lpm_transition_pos])
            {
                /* Restart LPM sequence */
                g_lpm_transition_pos = 0;
            }

            /* Toggle user LED */
            err = user_led_toggle(leds);
            /* Handle error */
            if (FSP_SUCCESS != err)
            {
                APP_ERR_TRAP(err);
            }

            /* Disable ioport if it's not in Deep SW Standby mode */
            if(APP_LPM_DEEP_SW_STANDBY_STATE != g_lpm_transition_sequence[g_lpm_transition_pos])
            {
                /* Disable ioport before going to LPM mode*/
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
            err = lpm_mode_enter(g_lpm_transition_sequence[g_lpm_transition_pos], &g_lpm_ctrl_instance_ctrls[g_lpm_transition_pos]);
            /* Handle error */
            if (FSP_SUCCESS != err)
            {
                /* Turn on user LED to indicate error occurred*/
#if defined (BOARD_RA6T1_RSSK)
                err = R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)leds.p_leds[LED_NO_0], BSP_IO_LEVEL_LOW);
#else
                err = R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)leds.p_leds[LED_NO_0], BSP_IO_LEVEL_HIGH);
#endif
                APP_ERR_TRAP(err);
            }
            else
            {
                /* Close LPM instance.*/
                err = R_LPM_Close(&g_lpm_ctrl_instance_ctrls[g_lpm_transition_pos]);
                /* Handle error */
                if(FSP_SUCCESS != err)
                {
                    APP_ERR_TRAP(err);
                }
                /* Put ioport configuration back to user's selections */
                err = R_IOPORT_PinsCfg(&g_ioport_ctrl, &g_bsp_pin_cfg);;
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
