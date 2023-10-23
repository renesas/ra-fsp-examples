/***********************************************************************************************************************
 * File Name    : common_init.c
 * Description  : Common init function.
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

#include "common_init.h"
#include "board_cfg.h"

static const struct
{
    const timer_instance_t * const p_timer;
    const gpt_io_pin_t              pin;
} pwm_pins[] =
{
    {&g_gpt_red, GPT_IO_PIN_GTIOCA},
    {&g_gpt_green, GPT_IO_PIN_GTIOCB},
    {&g_gpt_blue, GPT_IO_PIN_GTIOCA},
};

uint32_t pwm_rates[] = {BLINK_FREQ_1HZ};

static int16_t watchdog_counter = 0;
static bool watchdog_expired = false;

bool GPT_Is_Watchdog_Expired(void)
{
    return watchdog_expired;
}

void GPT_Start_Watchdog(void)
{
    watchdog_expired = false;
    watchdog_counter = (int16_t)BLE_APP_OTA_TRANSFER_FAST;
}

void GPT_Stop_Watchdog(void)
{
    watchdog_expired = false;
    watchdog_counter = 0;
}

void GPT_Set_WatchdogExpired(void)
{
    watchdog_expired = true;
    watchdog_counter = 0;
}

void gpt_blink_callback(timer_callback_args_t *p_args)
{
    static uint8_t watchdog_cycle_cnt=0;

    if(TIMER_EVENT_CYCLE_END == p_args->event)
    {
        timer_status_t status = {0};

        /* run watchdog timer when active */
        watchdog_cycle_cnt++;
        if ((watchdog_counter > 0) && (watchdog_cycle_cnt % 2))
        {
            watchdog_counter--;
            if (watchdog_counter <= 0)
            {
                watchdog_expired = true;
            }
        }
        /* end watchdog timer */

        R_GPT_StatusGet(pwm_pins[0].p_timer->p_ctrl, &status);

        for(uint32_t i = 0; i < g_bsp_leds.led_count; i++)
        {
            if (TIMER_STATE_COUNTING == status.state)
            {
                R_GPT_Stop(pwm_pins[i].p_timer->p_ctrl);
            }
            else
            {
                R_GPT_Start(pwm_pins[i].p_timer->p_ctrl);
            }
        }
    }
}

static fsp_err_t GPT_Initialize(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    for(uint32_t i = 0; i < g_bsp_leds.led_count; i++)
    {
        fsp_err = R_GPT_Open(pwm_pins[i].p_timer->p_ctrl, pwm_pins[i].p_timer->p_cfg);
        if(FSP_SUCCESS != fsp_err)
            return fsp_err;
    }

    fsp_err = R_GPT_Open(g_blinker.p_ctrl, g_blinker.p_cfg);
    {
        if(FSP_SUCCESS != fsp_err)
            return fsp_err;
    }
    fsp_err = R_GPT_PeriodSet(g_blinker.p_ctrl, pwm_rates[0]);
    {
        if(FSP_SUCCESS != fsp_err)
            return fsp_err;
    }

    fsp_err = R_GPT_Start(g_blinker.p_ctrl);
    if(FSP_SUCCESS != fsp_err)
    {
        /* Turn ON RED LED to indicate fatal error */
        TURN_RED_ON
        /* Close the GPT timer */
        R_GPT_Close(g_blinker.p_ctrl);

        return fsp_err;
    }

    return fsp_err;
}


fsp_err_t common_init(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    fsp_err = GPT_Initialize();
    if(FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }
    return fsp_err;
}
