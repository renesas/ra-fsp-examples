/***********************************************************************************************************************
 * File Name    : tml_capture.c
 * Description  : Contains data structures and functions used in tml_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "tml_ep.h"

static uint32_t g_captured_time = RESET_VALUE;
static uint16_t g_capture_overflows = RESET_VALUE;

extern volatile bool g_timer_flag [APP_TML_MAX_MODE];
extern const uint8_t * g_tml_mode_name [APP_TML_MAX_MODE];

/*******************************************************************************************************************//**
 * @brief       This function showcases how the TML operates in capture mode.
 * @retval      FSP_SUCCESS 	16-bit capture mode operates successfully
 * @retval      Any Other 		Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t tml_capture_operation (void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t user_input = RESET_VALUE;

    /* Initialize timer 16-bit capture */
    err = tml_open(APP_TML_16_BIT_CAPTURE);
    APP_ERR_RETURN(err, "\r\n**tml_open for the %s failed**\r\n", g_tml_mode_name[APP_TML_16_BIT_CAPTURE]);

    /* Start timer 16-bit capture */
    err = tml_start(APP_TML_16_BIT_CAPTURE);
    APP_ERR_RETURN(err, "\r\n**tml_start for the %s failed**\r\n", g_tml_mode_name[APP_TML_16_BIT_CAPTURE]);

    /* Initialize ELC module */
    err = R_ELC_Open(&g_elc_ctrl, &g_elc_cfg);
    APP_ERR_RETURN(err, "\r\n**R_ELC_Open API failed**\r\n");

    /* Enable ELC module */
    err = R_ELC_Enable(&g_elc_ctrl);
    APP_ERR_RETURN(err, "\r\n**R_ELC_Enable API failed**\r\n");

    while (true)
    {
        APP_PRINT("\r\nEnter 1 to capture the raw counts value of 16-bit timer, or enter 2 "\
                  "to back to the main menu\r\n");
        APP_PRINT("\r\nUser Input: ");
        user_input = get_user_input();
        if (CAPTURE == user_input)
        {
            /* Generate an ELC software event to trigger capture */
            err = R_ELC_SoftwareEventGenerate(&g_elc_ctrl, ELC_SOFTWARE_EVENT_0);
            APP_ERR_RETURN(err, "\r\n**R_ELC_SoftwareEventGenerate API failed**\r\n");

            while (false == g_timer_flag[APP_TML_16_BIT_CAPTURE])
            {
                /* Wait for capture flag */
            }
            g_timer_flag[APP_TML_16_BIT_CAPTURE] = false;
            APP_PRINT("\r\nCapture times:%d\r\n", g_captured_time);
        }
        else if (RETURN_MAIN_MENU == user_input)
        {
            /* De-initialize ELC module */
            err = R_ELC_Close(&g_elc_ctrl);
            APP_ERR_RETURN(err, "\r\n**R_ELC_Close API failed**\r\n");

            /* De-initialize timer 16-bit capture */
            err = tml_close(APP_TML_16_BIT_CAPTURE);
            APP_ERR_RETURN(err, "\r\n**tml_close timer 16-bit capture mode failed**\r\n");
            break;
        }
        else
        {
            APP_PRINT("\r\nInvalid input\r\n");
        }
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief This function is callback for 16-bit capture TML.
 * @param[in]  p_args
 * @retval     None
 **********************************************************************************************************************/
void timer_16bit_capture_callback(timer_callback_args_t *p_args)
{
    if (TIMER_EVENT_CYCLE_END == p_args->event)
    {
        g_capture_overflows++;
    }
    if (TIMER_EVENT_CAPTURE_EDGE == p_args->event)
    {
        g_captured_time = (g_timer_16bit_capture_cfg.period_counts * g_capture_overflows) + p_args->capture;
        g_timer_flag [APP_TML_16_BIT_CAPTURE] = true;
    }

}
