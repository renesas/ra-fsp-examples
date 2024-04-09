/***********************************************************************************************************************
 * File Name    : elc_ep.c
 * Description  : Contains declarations of data structures and functions used in elc_ep.c.
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
 * Copyright (C) [2020-2024] Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#include "common_utils.h"
#include "elc_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup elc_ep
 * @{
 **********************************************************************************************************************/

/* Private functions */
static void elc_deinit(void);

#ifdef BOARD_RA0E1_FPB
static void tau_deinit(void);
static void tau_pwm_deinit(void);
#else
static void gpt_deinit(gpt_instance_ctrl_t * p_ctrl);
#endif /* BOARD_RA0E1_FPB */

/**********************************************************************************************************************
* Function implementations
**********************************************************************************************************************/
void elc_ep_entry(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};
    unsigned char user_input[BUFFER_SIZE_DOWN] = {NULL_CHAR};

    /* version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example Project information printed on the Console */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    APP_PRINT(EP_INFO);

    /* Open ELC driver */
    err = R_ELC_Open(&g_elc_ctrl, &g_elc_cfg);
    /* Handle error */
    if(FSP_SUCCESS != err)
    {
        /* ELC module init failed */
        APP_ERR_PRINT("\r\n ** ELC module Open failed ** \r\n");
        APP_ERR_TRAP(err);
    }

    /* Enable Links between modules:
     * For elc_ep run with GPT timer: 1. ELC Software event as start source for GPT0 and GPT1
     *                                2. GPT1 counter overflow event as stop source for GPT0
     * For elc_ep run with TAU timer: 1. ELC Software event as start source for TAU1
     *                                2. Match comparison of counter TAU1 as event to start TAU0
     */
    err = R_ELC_Enable(&g_elc_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        elc_deinit();
        APP_ERR_PRINT("\r\n ** ELC Enable operation failed ** \r\n");
        APP_ERR_TRAP(err);
    }

#ifdef BOARD_RA0E1_FPB
    /* Open timer TAU one-shot pulse output */
    err = R_TAU_PWM_Open(&g_timer0_ctrl, &g_timer0_cfg);
#else
    /* Open Timer GPT in PWM mode */
    err = R_GPT_Open(&g_timer_pwm_ctrl,&g_timer_pwm_cfg);
#endif /* BOARD_RA0E1_FPB */

    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        elc_deinit();
        APP_ERR_PRINT(OPEN_TIMER_FAIL);
        APP_ERR_TRAP(err);
    }

#ifdef BOARD_RA0E1_FPB
    /* Open timer TAU delay counter */
    err = R_TAU_Open(&g_timer1_ctrl, &g_timer1_cfg);
#else
    /* Open Timer in One-shot mode */
    err = R_GPT_Open(&g_timer_oneshot_ctrl,&g_timer_oneshot_cfg);
#endif /* BOARD_RA0E1_FPB */

    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        elc_deinit();

#ifdef BOARD_RA0E1_FPB
        tau_pwm_deinit();
#else
        gpt_deinit(&g_timer_pwm_ctrl);
#endif /* BOARD_RA0E1_FPB */

        APP_ERR_PRINT(OPEN_TIMER_PWM_FAIL);
        APP_ERR_TRAP(err);
    }

#ifdef BOARD_RA0E1_FPB
    /* Enable Start source for timer in one-shot pulse output */
    err = R_TAU_PWM_Enable(&g_timer0_ctrl);
#else
    /* Enable Start and Stop sources for Timer in PWM mode */
    err = R_GPT_Enable(&g_timer_pwm_ctrl);
#endif /* BOARD_RA0E1_FPB */

    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        elc_deinit();

#ifdef BOARD_RA0E1_FPB
        tau_pwm_deinit();
        tau_deinit();
#else
        gpt_deinit(&g_timer_pwm_ctrl);
        gpt_deinit(&g_timer_oneshot_ctrl);
#endif /* BOARD_RA0E1_FPB */
        APP_ERR_PRINT(ENABLE_TIMER_PULSE_FAIL);
        APP_ERR_TRAP(err);
    }

#ifdef BOARD_RA0E1_FPB
    /* Enable Start source for Timer in delay counter function */
    err= R_TAU_Enable(&g_timer1_ctrl);
#else
	/* Enable Start source for Timer in one-shot mode */
    err = R_GPT_Enable(&g_timer_oneshot_ctrl);
#endif /* BOARD_RA0E1_FPB */

    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        elc_deinit();

#ifdef BOARD_RA0E1_FPB
        tau_deinit();
        tau_pwm_deinit();
#else
        gpt_deinit(&g_timer_pwm_ctrl);
        gpt_deinit(&g_timer_oneshot_ctrl);
#endif /* BOARD_RA0E1_FPB */

        APP_ERR_PRINT(ENABLE_TIMER_FAIL);
        APP_ERR_TRAP(err);
    }
    /* On successful operations on ELC and TAU/GPT modules, user is asked enter any key as RTT input */
    APP_PRINT(STATUS_DISPLAY);
    /* Wait until data is available in RTT */
    while (true)
    {
        if (APP_CHECK_DATA)
        {
            memset(&user_input[0], NULL_CHAR, BUFFER_SIZE_DOWN);
            APP_READ(user_input);
            err = R_ELC_SoftwareEventGenerate(&g_elc_ctrl, ELC_SOFTWARE_EVENT_0);
            if (FSP_SUCCESS != err)
            {
                elc_deinit();
                APP_ERR_PRINT("\r\n ** ELC Software event generation failed ** \r\n");
                APP_ERR_TRAP(err);
            }
            APP_PRINT(STATUS_DISPLAY);
        }
    }
}

/*******************************************************************************************************************//**
 * @brief This function closes opened ELC module before the project ends up in an Error Trap.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void elc_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close ELC module */
    err = R_ELC_Close(&g_elc_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* ELC Close failure message */
        APP_ERR_PRINT("** ELC module Close failed **  \r\n");
    }
}

#ifdef BOARD_RA0E1_FPB
/*******************************************************************************************************************//**
 * @brief This function closes opened TAU module before the project ends up in an Error Trap.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void tau_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close TAU timer */
    err = R_TAU_Close(&g_timer1_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* TAU close failure message */
        APP_ERR_PRINT("** TAU module close fail ** \r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief This function closes opened TAU PWM module before the project ends up in an Error Trap.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void tau_pwm_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Close TAU timer */
    err = R_TAU_PWM_Close(&g_timer0_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* TAU PWM close failure message */
        APP_ERR_PRINT("** TAU PWM module close fail ** \r\n");
    }
}
#else
/*******************************************************************************************************************//**
 * @brief This function closes opened GPT module before the project ends up in an Error Trap.
 * @param[IN]   p_ctrl    Pointer to instance control.
 * @retval      None
 **********************************************************************************************************************/
static void gpt_deinit(gpt_instance_ctrl_t * p_ctrl)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close GPT timer */
    err = R_GPT_Close(p_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* GPT Close failure message */
        APP_ERR_PRINT("** GPT module Close failed **  \r\n");
    }
}
#endif /* BOARD_RA0E1_FPB */

/*******************************************************************************************************************//**
 * @} (end addtogroup elc_ep)
 **********************************************************************************************************************/
