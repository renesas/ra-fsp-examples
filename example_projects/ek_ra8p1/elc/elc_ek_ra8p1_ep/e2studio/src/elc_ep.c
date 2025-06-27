/***********************************************************************************************************************
 * File Name    : elc_ep.c
 * Description  : Contains declarations of data structures and functions used in elc_ep.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "elc_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup elc_ep
 * @{
 **********************************************************************************************************************/

/* Private functions */
static void elc_deinit(void);
#if (BSP_PERIPHERAL_TAU_PRESENT)
static void tau_deinit(void);
static void tau_pwm_deinit(void);
#else
static void gpt_deinit(gpt_instance_ctrl_t * p_ctrl);
#endif /* BSP_PERIPHERAL_TAU_PRESENT */

/**********************************************************************************************************************
* Function implementations
**********************************************************************************************************************/
void elc_ep_entry(void)
{
    fsp_err_t err                               = FSP_SUCCESS;
    fsp_pack_version_t version                  = {RESET_VALUE};
    unsigned char user_input[BUFFER_SIZE_DOWN]  = {NULL_CHAR};

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the Console */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor,\
              version.version_id_b.patch);
    APP_PRINT(EP_INFO);

    /* Open ELC driver */
    err = R_ELC_Open(&g_elc_ctrl, &g_elc_cfg);
    /* Handle error */
    if(FSP_SUCCESS != err)
    {
        /* ELC module initialize failed */
        APP_ERR_PRINT("\r\n** R_ELC_Open API failed **\r\n");
        APP_ERR_TRAP(err);
    }

    /* Enable links between modules:
     * For elc_ep run with GPT timer: 1. ELC Software event as start source for GPT6 and GPT0
     *                                2. GPT0 counter overflow event as stop source for GPT6
     * For elc_ep run with TAU timer: 1. ELC Software event as start source for TAU1
     *                                2. Match comparison of counter TAU1 as event to start TAU0
     */
    err = R_ELC_Enable(&g_elc_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        elc_deinit();
        APP_ERR_PRINT("\r\n** R_ELC_Enable API failed **\r\n");
        APP_ERR_TRAP(err);
    }

#if (BSP_PERIPHERAL_TAU_PRESENT)
    /* Open Timer TAU One-Shot pulse output */
    err = R_TAU_PWM_Open(&g_timer0_ctrl, &g_timer0_cfg);
#else
    /* Open Timer GPT in PWM mode */
    err = R_GPT_Open(&g_timer_pwm_ctrl, &g_timer_pwm_cfg);
#endif /* BSP_PERIPHERAL_TAU_PRESENT */

    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        elc_deinit();
        APP_ERR_PRINT(OPEN_TIMER_PWM_FAIL);
        APP_ERR_TRAP(err);
    }

#if (BSP_PERIPHERAL_TAU_PRESENT)
    /* Open Timer TAU delay counter */
    err = R_TAU_Open(&g_timer1_ctrl, &g_timer1_cfg);
#else
    /* Open Timer in One-Shot mode */
    err = R_GPT_Open(&g_timer_oneshot_ctrl,&g_timer_oneshot_cfg);
#endif /* BSP_PERIPHERAL_TAU_PRESENT */

    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        elc_deinit();

#if (BSP_PERIPHERAL_TAU_PRESENT)
        tau_pwm_deinit();
#else
        gpt_deinit(&g_timer_pwm_ctrl);
#endif /* BSP_PERIPHERAL_TAU_PRESENT */

        APP_ERR_PRINT(OPEN_TIMER_FAIL);
        APP_ERR_TRAP(err);
    }

#if (BSP_PERIPHERAL_TAU_PRESENT)
    /* Enable start source for Timer in One-Shot pulse output */
    err = R_TAU_PWM_Enable(&g_timer0_ctrl);
#else
    /* Enable start and stop sources for Timer in PWM mode */
    err = R_GPT_Enable(&g_timer_pwm_ctrl);
#endif /* BSP_PERIPHERAL_TAU_PRESENT */

    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        elc_deinit();

#if (BSP_PERIPHERAL_TAU_PRESENT)
        tau_pwm_deinit();
        tau_deinit();
#else
        gpt_deinit(&g_timer_pwm_ctrl);
        gpt_deinit(&g_timer_oneshot_ctrl);
#endif /* BSP_PERIPHERAL_TAU_PRESENT */
        APP_ERR_PRINT(ENABLE_TIMER_PULSE_FAIL);
        APP_ERR_TRAP(err);
    }

#if (BSP_PERIPHERAL_TAU_PRESENT)
    /* Enable start source for Timer in delay counter function */
    err= R_TAU_Enable(&g_timer1_ctrl);
#else
	/* Enable start source for Timer in One-Shot mode */
    err = R_GPT_Enable(&g_timer_oneshot_ctrl);
#endif /* BSP_PERIPHERAL_TAU_PRESENT */

    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        elc_deinit();

#if (BSP_PERIPHERAL_TAU_PRESENT)
        tau_deinit();
        tau_pwm_deinit();
#else
        gpt_deinit(&g_timer_pwm_ctrl);
        gpt_deinit(&g_timer_oneshot_ctrl);
#endif /* BSP_PERIPHERAL_TAU_PRESENT */

        APP_ERR_PRINT(ENABLE_TIMER_FAIL);
        APP_ERR_TRAP(err);
    }
    /* On successful operations on ELC and TAU/GPT modules, the user is asked enter any key as RTT Viewer input */
    APP_PRINT(STATUS_DISPLAY);
    /* Wait until data is available in RTT Viewer */
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
                APP_ERR_PRINT("\r\n** R_ELC_SoftwareEventGenerate API failed **\r\n");
                APP_ERR_TRAP(err);
            }
            APP_PRINT(STATUS_DISPLAY);
        }
    }
}

/*******************************************************************************************************************//**
 * @brief       This function closes opened ELC module before the project ends up in an error trap.
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
        /* ELC close failure message */
        APP_ERR_PRINT("** R_ELC_Close API failed **\r\n");
    }
}

#if (BSP_PERIPHERAL_TAU_PRESENT)
/*******************************************************************************************************************//**
 * @brief       This function closes opened TAU module before the project ends up in an error trap.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void tau_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close TAU Timer */
    err = R_TAU_Close(&g_timer1_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* TAU close failure message */
        APP_ERR_PRINT("** R_TAU_Close API failed **\r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief       This function closes opened TAU PWM module before the project ends up in an error trap.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void tau_pwm_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Close TAU Timer */
    err = R_TAU_PWM_Close(&g_timer0_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* TAU PWM close failure message */
        APP_ERR_PRINT("** R_TAU_PWM_Close API failed **\r\n");
    }
}
#else
/*******************************************************************************************************************//**
 * @brief       This function closes opened GPT module before the project ends up in an error trap.
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
        /* GPT close failure message */
        APP_ERR_PRINT("** R_GPT_Close API failed **\r\n");
    }
}
#endif /* BSP_PERIPHERAL_TAU_PRESENT */

/*******************************************************************************************************************//**
 * @} (end addtogroup elc_ep)
 **********************************************************************************************************************/
