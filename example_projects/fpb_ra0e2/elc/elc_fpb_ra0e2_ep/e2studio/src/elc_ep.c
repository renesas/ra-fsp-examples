/***********************************************************************************************************************
 * File Name    : elc_ep.c
 * Description  : Contains declarations of data structures and functions used in elc_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
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

#if BSP_PERIPHERAL_TAU_PRESENT
static void tau_deinit(void);
static void tau_pwm_deinit(void);
#else
static void gpt_deinit(gpt_instance_ctrl_t * p_ctrl);
#endif /* BSP_PERIPHERAL_TAU_PRESENT */

/***********************************************************************************************************************
* Function implementations
***********************************************************************************************************************/
void elc_ep_entry(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};
    unsigned char user_input[BUFFER_SIZE_DOWN] = {NULL_CHAR};

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example Project information printed on the Console */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor,\
              version.version_id_b.patch);
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

#if BSP_PERIPHERAL_TAU_PRESENT
    /* Open timer TAU one-shot pulse output */
    err = R_TAU_PWM_Open(&g_timer0_ctrl, &g_timer0_cfg);
#else
    /* Open Timer GPT in PWM mode */
    err = R_GPT_Open(&g_timer_pwm_ctrl,&g_timer_pwm_cfg);
#endif /* BSP_PERIPHERAL_TAU_PRESENT */

    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        elc_deinit();
        APP_ERR_PRINT(OPEN_TIMER_FAIL);
        APP_ERR_TRAP(err);
    }

#if BSP_PERIPHERAL_TAU_PRESENT
    /* Open timer TAU delay counter */
    err = R_TAU_Open(&g_timer1_ctrl, &g_timer1_cfg);
#else
    /* Open Timer in One-shot mode */
    err = R_GPT_Open(&g_timer_oneshot_ctrl,&g_timer_oneshot_cfg);
#endif /* BSP_PERIPHERAL_TAU_PRESENT */

    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        elc_deinit();

#if BSP_PERIPHERAL_TAU_PRESENT
        tau_pwm_deinit();
#else
        gpt_deinit(&g_timer_pwm_ctrl);
#endif /* BSP_PERIPHERAL_TAU_PRESENT */

        APP_ERR_PRINT(OPEN_TIMER_PWM_FAIL);
        APP_ERR_TRAP(err);
    }

#if BSP_PERIPHERAL_TAU_PRESENT
    /* Enable Start source for timer in one-shot pulse output */
    err = R_TAU_PWM_Enable(&g_timer0_ctrl);
#else
    /* Enable Start and Stop sources for Timer in PWM mode */
    err = R_GPT_Enable(&g_timer_pwm_ctrl);
#endif /* BSP_PERIPHERAL_TAU_PRESENT */

    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        elc_deinit();

#if BSP_PERIPHERAL_TAU_PRESENT
        tau_pwm_deinit();
        tau_deinit();
#else
        gpt_deinit(&g_timer_pwm_ctrl);
        gpt_deinit(&g_timer_oneshot_ctrl);
#endif /* BSP_PERIPHERAL_TAU_PRESENT */
        APP_ERR_PRINT(ENABLE_TIMER_PULSE_FAIL);
        APP_ERR_TRAP(err);
    }

#if BSP_PERIPHERAL_TAU_PRESENT
    /* Enable Start source for Timer in delay counter function */
    err= R_TAU_Enable(&g_timer1_ctrl);
#else
	/* Enable Start source for Timer in one-shot mode */
    err = R_GPT_Enable(&g_timer_oneshot_ctrl);
#endif /* BSP_PERIPHERAL_TAU_PRESENT */

    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        elc_deinit();

#if BSP_PERIPHERAL_TAU_PRESENT
        tau_deinit();
        tau_pwm_deinit();
#else
        gpt_deinit(&g_timer_pwm_ctrl);
        gpt_deinit(&g_timer_oneshot_ctrl);
#endif /* BSP_PERIPHERAL_TAU_PRESENT */

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

#if BSP_PERIPHERAL_TAU_PRESENT
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
#endif /* BSP_PERIPHERAL_TAU_PRESENT */

/*******************************************************************************************************************//**
 * @} (end addtogroup elc_ep)
 **********************************************************************************************************************/
