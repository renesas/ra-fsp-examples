/***********************************************************************************************************************
 * File Name    : lpm_example_app.c
 * Description  : Example application layer for the LPM EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: lpm_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include "lpm_example_app.h"
#include "source_cancel_lpm.h"

/* LPM mode names */
static const char * g_lpm_mode_name[APP_LPM_MAX_STATE] =
{
    [APP_LPM_SLEEP_STATE]                  = NAME_LPM_SLEEP_MODE,
#if (BSP_FEATURE_LPM_HAS_DEEP_SLEEP)
    [APP_LPM_DEEP_SLEEP_STATE]             = NAME_LPM_DEEP_SLEEP_MODE,
#endif /* BSP_FEATURE_LPM_HAS_DEEP_SLEEP */
    [APP_LPM_SW_STANDBY_STATE]             = NAME_LPM_SW_STANDBY_MODE,
#if (BSP_FEATURE_LPM_HAS_SNOOZE)
    [APP_LPM_SW_STANDBY_WITH_SNOOZE_STATE] = NAME_LPM_SW_STANDBY_MODE_WITH_SNOOZE,
#endif /* BSP_FEATURE_LPM_HAS_SNOOZE */
#if (BSP_FEATURE_LPM_HAS_DEEP_STANDBY)
    [APP_LPM_DEEP_SW_STANDBY_STATE]        = NAME_LPM_DEEP_SW_STANDBY_MODE,
#endif /* BSP_FEATURE_LPM_HAS_DEEP_STANDBY */
};

/* LPM instances */
static lpm_instance_t const * g_lpm_instance[APP_LPM_MAX_STATE] =
{
    [APP_LPM_SLEEP_STATE] = &g_lpm_sleep,
#if (BSP_FEATURE_LPM_HAS_DEEP_SLEEP)
    [APP_LPM_DEEP_SLEEP_STATE] = &g_lpm_deep_sleep,
#endif /* BSP_FEATURE_LPM_HAS_DEEP_SLEEP */
    [APP_LPM_SW_STANDBY_STATE] = &g_lpm_sw_standby,
#if (BSP_FEATURE_LPM_HAS_SNOOZE)
    [APP_LPM_SW_STANDBY_WITH_SNOOZE_STATE] = &g_lpm_sw_standby_with_snooze,
#endif /* BSP_FEATURE_LPM_HAS_SNOOZE */
#if (BSP_FEATURE_LPM_HAS_DEEP_STANDBY)
    [APP_LPM_DEEP_SW_STANDBY_STATE] = &g_lpm_deep_sw_standby,
#endif /* BSP_FEATURE_LPM_HAS_DEEP_STANDBY */
};

static app_lpm_states_t s_lpm_state = APP_LPM_SLEEP_STATE;

/* Private functions */
static void lpm_operation(app_lpm_states_t lpm_mode);
static void lpm_ep_startup(void);
static void lpm_ep_main_process(void);
static void handle_error(fsp_err_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the LPM example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void lpm_example_app_run(void)
{
    /* Perform EP startup procedure */
    lpm_ep_startup();

    /* Perform EP main procedure */
    while (true)
    {
        lpm_ep_main_process();
    }
}

/*******************************************************************************************************************//**
 * @brief     This function performs the LPM operation.
 * @param[in] lpm_mode                  LPM mode to operate.
 * @retval    None.
 **********************************************************************************************************************/
static void lpm_operation(app_lpm_states_t lpm_mode)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Check the valid LPM mode */
    if (APP_LPM_MAX_STATE <= lpm_mode)
    {
        handle_error(FSP_ERR_INVALID_MODE, "This MCU does not support this LPM mode\r\n");
    }

    /* Open this LPM mode */
    err = lpm_init(g_lpm_instance[lpm_mode]->p_ctrl, g_lpm_instance[lpm_mode]->p_cfg);
    handle_error(err, "lpm_init failed\r\n");

    /* Print LPM mode name */
    printf("%s", g_lpm_mode_name[lpm_mode]);

    /* Procedure before enter to LPM */
    err = lpm_enter_procedure(lpm_mode);
    handle_error(err, "lpm_enter_procedure failed\r\n");

    /* Enter to LPM */
    err = lpm_mode_enter(g_lpm_instance[lpm_mode]->p_ctrl);
    handle_error(err, "lpm_mode_enter failed\r\n");

    /* Procedure after exit LPM */
    err = lpm_exit_procedure(lpm_mode);
    handle_error(err, "lpm_exit_procedure failed\r\n");

    /* Close LPM */
    lpm_deinit(g_lpm_instance[lpm_mode]->p_ctrl);
}

/*******************************************************************************************************************//**
 * @brief     This function performs the startup operation.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void lpm_ep_startup(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};

    /* Turn OFF state LEDs at start up */
    led_lpm_state_set(LED_OFF);
    led_error_state_set(LED_OFF);

    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        /* Turn ON LED2 to indicate an error */
        led_error_state_set(LED_ON);

        /* UART failed to open — cannot print anything */
        return;
    }

    /* In case of internal reset */
    if (true == g_source_flag)
    {
        /* Print internal reset banner */
        printf(BANNER_RESET);

        /* Detect source generates internal reset */
        source_cancel_lpm_detect();

        /* Clear all source detection flags */
        source_cancel_lpm_flag_clear();

        /* Verify data stored in Standby SRAM */
#if (BSP_FEATURE_LPM_HAS_DEEP_STANDBY) && defined(SSRAM_START) && defined(SSRAM_LENGTH)
        if (false == standby_sram_verify(APP_SSRAM_OFFSET, (uint8_t *)APP_SSRAM_DATA, APP_SSRAM_LEN))
        {
            handle_error(FSP_ERR_INVALID_DATA, "standby_sram_verify failed\r\n");
        }

        printf("\r\nData stored in Standby SRAM matches fixed data\r\n");
#endif /* BSP_FEATURE_LPM_HAS_DEEP_STANDBY, SSRAM_START, SSRAM_LENGTH */

        /* Print a new cycle start message */
        printf("\r\nStart a new LPM operating cycle\r\n");
    }

    /* In case of normal reset */
    else
    {
        /* Get FLEX pack version information */
        R_FSP_VersionGet(&version);

        /* Print EP information */
        printf(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor,
               version.version_id_b.patch);
        printf(EP_INFO);

        /* Store fixed data into Standby SRAM */
#if (BSP_FEATURE_LPM_HAS_DEEP_STANDBY) && defined(SSRAM_START) && defined(SSRAM_LENGTH)
        if (false == standby_sram_write(APP_SSRAM_OFFSET, (uint8_t *)APP_SSRAM_DATA, APP_SSRAM_LEN))
        {
            handle_error(FSP_ERR_INVALID_DATA, "standby_sram_write failed\r\n");
        }

        /* Print stored successful message */
        printf("\r\nStored fixed data into Standby SRAM\r\n");
#endif /* BSP_FEATURE_LPM_HAS_DEEP_STANDBY, SSRAM_START, SSRAM_LENGTH */
    }

    /* Initialize all modules used to cancel LPM */
    err = source_cancel_lpm_init();
    handle_error(err, "source_cancel_lpm_init failed\r\n");

    /* Print the instruction to start the LPM operating sequence */
    printf("\r\nPress SW1 button or wait 10 seconds to start LPM operating sequence\r\n");

    /* Wait until start-enable event occurs */
    err = lpm_wait_start();
    handle_error(err, "lpm_wait_start failed\r\n");

    /* Turn ON LED1 to indicate a successful startup */
    led_lpm_state_set(LED_ON);
}

/*******************************************************************************************************************//**
 * @brief     This function performs the main process.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void lpm_ep_main_process(void)
{
    /* Print start message */
    printf("\r\nLPM operating is started\r\n");

    while (true)
    {
        /* Software delay to next LPM mode */
        R_BSP_SoftwareDelay(LPM_TIME_TRANSITION_VALUE, LPM_TIME_TRANSITION_UNIT);

        /* Perform a LPM operating sequence */
        lpm_operation(s_lpm_state);

        /* Go to next LPM mode */
        s_lpm_state++;

        /* Check end of LPM mode list */
        if (APP_LPM_MAX_STATE <= s_lpm_state)
        {
            s_lpm_state = APP_LPM_SLEEP_STATE;
            printf("\r\nStart a new LPM operating sequence cycle\r\n");
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function will detect which module has canceled LPM mode.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void source_cancel_lpm_detect(void)
{
    if (true == g_timer_flag)
    {
        printf(NAME_LPM_CANCEL_SOURCE_TIMER);
    }
    else if (true == g_external_irq_flag)
    {
        printf(NAME_LPM_CANCEL_SOURCE_IRQ);
    }
#if (BSP_FEATURE_LPM_HAS_SNOOZE)
    else if (true == g_snooze_flag && g_dtc_src_data == g_dtc_dest_data)
    {
        printf(NAME_LPM_CANCEL_SOURCE_DTC);
    }
#endif /* BSP_FEATURE_LPM_HAS_SNOOZE */
    else
    {
        printf(NAME_LPM_CANCEL_SOURCE_NOT_DETECTED);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function handles an error by printing error message and code, de-initializing the related modules,
 *            turning ON LED2, and entering a trap state.
 * @param[in] err                       Error code.
 * @param[in] err_str                   Error message.
 * @retval    None.
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err, const char * err_str)
{
    if (FSP_SUCCESS != err)
    {
        printf("\r\n[ERR] %s", err_str);
        printf("Returned Error Code: 0x%x\r\n", (unsigned int)err);
        uart_console_flush();
        source_cancel_lpm_deinit();
        lpm_deinit(g_lpm_instance[s_lpm_state]->p_ctrl);
        led_error_state_set(LED_ON);
        __BKPT(0);
    }
}
