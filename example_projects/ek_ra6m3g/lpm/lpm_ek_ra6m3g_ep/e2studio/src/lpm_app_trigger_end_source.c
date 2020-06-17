/***********************************************************************************************************************
 * File Name    : lpm_app_trigger_end_source.c
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
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
#include "lpm_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup lpm_ep
 * @{
 **********************************************************************************************************************/

/* Boolean flag to determine user push-button is pressed or not.*/
extern volatile bool 	g_user_sw_press;
static volatile bool 	user_sw_debouce_start 	 = false;
static volatile uint8_t user_sw_debouce_counter  = 0;

/*******************************************************************************************************************//**
 * @brief       This function initializes ICU module.
 * @param[IN]   None
 * @retval      FSP_SUCCESS                  Upon successful open and enable ICU module
 * @retval      Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t user_sw_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open external IRQ/ICU module */
    err = R_ICU_ExternalIrqOpen(&g_external_irq_user_sw_ctrl, &g_external_irq_user_sw_cfg);
    if(FSP_SUCCESS == err)
    {
        /* Enable ICU module */
        err = R_ICU_ExternalIrqEnable(&g_external_irq_user_sw_ctrl);

    }

    return err;
}


/*******************************************************************************************************************//**
 * @brief      User defined external irq callback.
 * @param[IN]  p_args
 * @retval     None
 **********************************************************************************************************************/
void external_irq_user_sw_cb(external_irq_callback_args_t *p_args)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Make sure it's the right interrupt*/
    if(USER_SW_IRQ_NUMBER == p_args->channel)
    {
        /* This is to avoid switch debouncing */
        if(true != user_sw_debouce_start)
        {
            /* Reset AGT0 timer (g_timer0_sw_debounce_filter_ctrl) */
            err = R_AGT_Reset(&agt_timer0_sw_debounce_filter_ctrl);
            if (FSP_SUCCESS != err)
            {
                /* Handle error */
                APP_ERR_TRAP(err);
            }

            /* Start filtering switch debounce using AGT0 timer */
            user_sw_debouce_start   = true;
            /* Reset AGT0 timer (filter) 's counter */
            user_sw_debouce_counter = 0;
        }
    }
}


/*******************************************************************************************************************//**
 * @brief       This function stops AGT1 timer.
 * @param[IN]   None
 * @retval      FSP_SUCCESS                  Timer stopped successfully
 * @retval      Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t agt1_stop(void)
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
 * @brief       This function initializes AGT0 module (g_timer0_sw_debounce_filter).
 * @param[IN]   None
 * @retval      FSP_SUCCESS                  Upon successful open/start of AGT0 module
 * @retval      Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t agt0_init_start(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open AGT0 Timer in Periodic mode */
    err = R_AGT_Open(&agt_timer0_sw_debounce_filter_ctrl, &agt_timer0_sw_debounce_filter_cfg);
    if(FSP_SUCCESS == err)
    {
        /* Start AGT0 timer */
        err = R_AGT_Start(&agt_timer0_sw_debounce_filter_ctrl);

    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief This function is callback for AGT0 timer (user switch debouncing filter).
 * @param[in] (timer_callback_args_t *)
 * @retval None
 **********************************************************************************************************************/
void agt_timer0_debounce_filter_cb(timer_callback_args_t *p_args)
{
    if(TIMER_EVENT_CYCLE_END == p_args->event)
    {

        /* Start counting to filter/avoid switch debouncing */
        if(true == user_sw_debouce_start)
        {
            user_sw_debouce_counter++;

            if (USER_SW_DEBOUNCE_LIMIT <= user_sw_debouce_counter)
            {
                /* Confirm user press event after avoid switch debouncing and reset the counter */
                g_user_sw_press 		= true;
                user_sw_debouce_start   = false;
                user_sw_debouce_counter = 0;
            }
        }
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup lpm_ep)
 **********************************************************************************************************************/
