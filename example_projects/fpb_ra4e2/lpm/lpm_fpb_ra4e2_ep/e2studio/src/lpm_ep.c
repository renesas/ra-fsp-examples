/***********************************************************************************************************************
 * File Name    : lpm_ep.c
 * Description  : Contains data structures and functions used in lpm_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "lpm_example_app.h"
#include "source_cancel_lpm.h"

/*******************************************************************************************************************//**
 * @brief     This function initializes the LPM mode.
 * @param[in] p_api_ctrl                Pointer to the LPM control block.
 * @param[in] p_cfg                     Pointer to the LPM configuration structure.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t lpm_init(lpm_ctrl_t * const p_api_ctrl, lpm_cfg_t const * const p_cfg)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_LPM_Open(p_api_ctrl, p_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function enters LPM using WFI macro.
 * @param[in] p_api_ctrl                Pointer to the LPM control block.
 * @retval    FSP_SUCCESS               Entered successfully.
 * @retval    Any other error codes     Failed to enter.
 **********************************************************************************************************************/
fsp_err_t lpm_mode_enter(lpm_ctrl_t * const p_api_ctrl)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Wait for pending UART transmissions to complete */
    uart_console_flush();

    err = R_LPM_LowPowerModeEnter(p_api_ctrl);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function performs a procedure before entering LPM mode.
 * @param[in] lpm_mode                  LPM mode prepares to enter.
 * @retval    FSP_SUCCESS               Operated successfully.
 * @retval    Any other error codes     Failed to operate.
 **********************************************************************************************************************/
fsp_err_t lpm_enter_procedure(app_lpm_states_t lpm_mode)
{
    fsp_err_t err = FSP_SUCCESS;

#if (!(BSP_FEATURE_LPM_HAS_SNOOZE || defined(BOARD_RA8D1_EK) || defined(BOARD_RA8M1_EK) ||\
       defined(BOARD_RA8T1_MCK) || defined(BOARD_RA8E1_FPB) || defined(BOARD_RA8E2_EK) ||\
	   defined(BOARD_RA4E2_FPB)))
    {
        FSP_PARAMETER_NOT_USED(lpm_mode);
    }
#endif /* BSP_FEATURE_LPM_HAS_SNOOZE, Specific board */

    /* Turn OFF LED1 in case of LPM mode */
    led_lpm_state_set(LED_OFF);

    /* Clear all source detection flags */
    source_cancel_lpm_flag_clear();

    /* Start/enable hardware modules to cancel LPM mode */
    err = source_cancel_lpm_enable();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

#if (BSP_FEATURE_LPM_HAS_SNOOZE)
    /* Start/enable hardware modules to cancel Snooze mode */
    if (APP_LPM_SW_STANDBY_WITH_SNOOZE_STATE == lpm_mode)
    {
        err = source_cancel_snooze_enable();
        if (FSP_SUCCESS != err)
        {
            return err;
        }
    }
#endif /* BSP_FEATURE_LPM_HAS_SNOOZE */

#if defined(BOARD_RA8D1_EK) || defined(BOARD_RA8M1_EK) || defined(BOARD_RA8T1_MCK) || defined(BOARD_RA8E1_FPB) ||\
    defined(BOARD_RA8E2_EK)
    /* The UART module stops with undefined in SW Standby mode */
    if (APP_LPM_SW_STANDBY_STATE == lpm_mode)
    {
        uart_console_deinit();
    }
#endif /* Specific board */

#if defined(BOARD_RA4E2_FPB)
    /* The UART module stops with undefined in Deep SW Standby mode */
    if (APP_LPM_DEEP_SW_STANDBY_STATE == lpm_mode)
    {
        uart_console_deinit();
    }
#endif /* Specific board */

    /* Disable unused IO ports to reduce power */
    err = R_IOPORT_PinsCfg(&g_ioport_ctrl, &g_bsp_pin_lpm_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function performs a procedure after exiting LPM mode.
 * @param[in] lpm_mode                  LPM mode just exited.
 * @retval    FSP_SUCCESS               Operated successfully.
 * @retval    Any other error codes     Failed to operate.
 **********************************************************************************************************************/
fsp_err_t lpm_exit_procedure(app_lpm_states_t lpm_mode)
{
    fsp_err_t err = FSP_SUCCESS;

#if (!(BSP_FEATURE_LPM_HAS_SNOOZE || defined(BOARD_RA8D1_EK) || defined(BOARD_RA8M1_EK) ||\
       defined(BOARD_RA8T1_MCK) || defined(BOARD_RA8E1_FPB) || defined(BOARD_RA8E2_EK) ||\
       defined(BOARD_RA4E2_FPB)))
    {
        FSP_PARAMETER_NOT_USED(lpm_mode);
    }
#endif /* BSP_FEATURE_LPM_HAS_SNOOZE, Specific board */

    /* Put IO port configuration back to user's selections */
    err = R_IOPORT_PinsCfg(&g_ioport_ctrl, &g_bsp_pin_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

#if defined(BOARD_RA8D1_EK) || defined(BOARD_RA8M1_EK) || defined(BOARD_RA8T1_MCK) || defined(BOARD_RA8E1_FPB) ||\
    defined(BOARD_RA8E2_EK)
    /* Re-initialize the UART module after exited SW Standby mode */
    if (APP_LPM_SW_STANDBY_STATE == lpm_mode)
    {
        uart_console_init();
    }
#endif /* Specific board */

#if defined(BOARD_RA4E2_FPB)
    /* Re-initialize the UART module after exited Deep SW Standby mode */
    if (APP_LPM_DEEP_SW_STANDBY_STATE == lpm_mode)
    {
        uart_console_init();
    }
#endif /* Specific board */

#if (BSP_FEATURE_LPM_HAS_SNOOZE)
    /* Stop/disable hardware modules used to cancel Snooze mode */
    if (APP_LPM_SW_STANDBY_WITH_SNOOZE_STATE == lpm_mode)
    {
        err = source_cancel_snooze_disable();
        if (FSP_SUCCESS != err)
        {
            return err;
        }
    }
#endif /* BSP_FEATURE_LPM_HAS_SNOOZE */

    /* Stop/disable hardware modules used to cancel LPM mode */
    err = source_cancel_lpm_disable();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Detect and print LPM cancellation source */
    source_cancel_lpm_detect();

    /* Clear all source detection flags */
    source_cancel_lpm_flag_clear();

    /* Turn ON LED1 in case of normal mode */
    led_lpm_state_set(LED_ON);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function waits for trigger to start LPM operation sequence.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Operated successfully.
 * @retval    Any other error codes     Failed to operate.
 **********************************************************************************************************************/
fsp_err_t lpm_wait_start(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Disable sources used to cancel LPM */
    err = source_cancel_lpm_disable();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Clear all source detection flags */
    source_cancel_lpm_flag_clear();

    /* Start/enable hardware modules used to cancel LPM mode */
    err = source_cancel_lpm_enable();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Wait for an event */
    while (false == g_source_flag);

    /* Stop/disable hardware modules used to cancel LPM mode */
    err = source_cancel_lpm_disable();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Clear all source detection flags */
    source_cancel_lpm_flag_clear();

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function de-initializes the opened LPM mode.
 * @param[in] p_api_ctrl                Pointer to the LPM control block.
 * @retval    None.
 **********************************************************************************************************************/
void lpm_deinit(lpm_ctrl_t * const p_api_ctrl)
{
    lpm_instance_ctrl_t * p_ctrl = (lpm_instance_ctrl_t *) p_api_ctrl;
    if (p_ctrl->lpm_open)
    {
        R_LPM_Close(p_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function sets the state of LED1.
 * @param[in] state                     LED state to set.
 * @retval    None.
 **********************************************************************************************************************/
void led_lpm_state_set(led_power_t state)
{
    /* Check if the board has this LED */
    if (LED_LPM_STATE < g_bsp_leds.led_count)
    {
        R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[LED_LPM_STATE], (bsp_io_level_t)state);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function sets the state of LED2.
 * @param[in] state                     LED state to set.
 * @retval    None.
 **********************************************************************************************************************/
void led_error_state_set(led_power_t state)
{
    /* Check if the board has this LED */
    if (LED_ERROR_STATE < g_bsp_leds.led_count)
    {
        R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[LED_ERROR_STATE], (bsp_io_level_t)state);
    }
}

#if defined(SSRAM_START) && defined(SSRAM_LENGTH)
/*******************************************************************************************************************//**
 * @brief     This function writes the data to Standby SRAM.
 * @param[in] offset_addr               Offset address at Standby SRAM sections.
 * @param[in] p_source                  Pointer to source data to be written.
 * @param[in] len                       Write data length.
 * @retval    true                      Wrote successfully.
 * @retval    false                     Failed to write.
 **********************************************************************************************************************/
bool standby_sram_write(uint32_t offset_addr, uint8_t * p_source, uint32_t len)
{
    uint8_t * p_ssram = (uint8_t *)SSRAM_START + offset_addr;

    /* Over Standby SRAM address range */
    if (SSRAM_LENGTH <= offset_addr + len)
    {
        return false;
    }

    /* Write to Standby SRAM */
    memcpy(p_ssram, p_source, len);

    return true;
}

/*******************************************************************************************************************//**
 * @brief      This function verifies the data between input buffer and Standby SRAM.
 * @param[in]  offset_addr              Offset address of the start address of data at Standby SRAM sections.
 * @param[out] p_source                 Pointer to source data for verification.
 * @param[in]  len                      Verify data length.
 * @retval     true                     Verified successfully.
 * @retval     false                    Failed to verify.
 **********************************************************************************************************************/
bool standby_sram_verify(uint32_t offset_addr, uint8_t * p_source, uint32_t len)
{
    uint8_t * p_ssram = (uint8_t *)SSRAM_START + offset_addr;

    /* Over Standby SRAM address range */
    if (SSRAM_LENGTH <= offset_addr + len)
    {
        return false;
    }

    if (memcmp(p_ssram, p_source, len))
    {
        return false;
    }

    return true;
}
#endif /* SSRAM_START, SSRAM_LENGTH */
