/***********************************************************************************************************************
 * File Name    : acmphs_ep.c
 * Description  : Contains data structures and functions used in acmphs_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "acmphs_ep.h"

/* Private variable */
static volatile bool s_b_comparator_state_flag = true;

#if (BSP_PERIPHERAL_DAC_PRESENT || BSP_PERIPHERAL_DAC_B_PRESENT)
/*******************************************************************************************************************//**
 * @brief     This function opens DAC0 module, writes its initial value, then starts conversion.
 * @param[in] initial_val               Initial value to write before starting.
 * @retval    FSP_SUCCESS               Opened, wrote, and started successfully
 * @retval    Any other error codes     Failed to open, write, or start.
 **********************************************************************************************************************/
fsp_err_t acmphs_ep_dac0_init(uint16_t initial_val)
{
    fsp_err_t err = FSP_SUCCESS;

    err = DAC_OPEN(DAC0_CTRL, DAC0_CFG);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = DAC_WRITE(DAC0_CTRL, initial_val);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = DAC_START(DAC0_CTRL);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function writes a value to DAC0 module.
 * @param[in] dac_val                   Value to write.
 * @retval    FSP_SUCCESS               Wrote successfully.
 * @retval    Any other error codes     Failed to write.
 **********************************************************************************************************************/
fsp_err_t acmphs_ep_dac0_write(uint16_t dac_val)
{
    return DAC_WRITE(DAC0_CTRL, dac_val);
}

#if (!(defined (BOARD_RA8E1_FPB) || defined (BOARD_RA8E2_EK)))
/*******************************************************************************************************************//**
 * @brief     This function opens DAC1 module, writes its initial value, then starts conversion.
 * @param[in] initial_val               Initial value to write before starting.
 * @retval    FSP_SUCCESS               Opened, wrote, and started successfully
 * @retval    Any other error codes     Failed to open, write, or start.
 **********************************************************************************************************************/
fsp_err_t acmphs_ep_dac1_init(uint16_t initial_val)
{
    fsp_err_t err = FSP_SUCCESS;

    err = DAC_OPEN(DAC1_CTRL, DAC1_CFG);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = DAC_WRITE(DAC1_CTRL, initial_val);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = DAC_START(DAC1_CTRL);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function writes a value to DAC1 module.
 * @param[in] dac_val                   Value to write.
 * @retval    FSP_SUCCESS               Wrote successfully.
 * @retval    Any other error codes     Failed to write.
 **********************************************************************************************************************/
fsp_err_t acmphs_ep_dac1_write(uint16_t dac_val)
{
    return DAC_WRITE(DAC1_CTRL, dac_val);
}
#endif /* BOARD_RA8E1_FPB, BOARD_RA8E2_EK */
#endif /* BSP_PERIPHERAL_DAC_PRESENT, BSP_PERIPHERAL_DAC_B_PRESENT */

/*******************************************************************************************************************//**
 * @brief      This function opens the comparator, fetches its minimum stabilization wait time, then enables
 *             its output.
 * @param[out] p_stabilize_time         Pointer to comparator_info_t to receive the stabilization info.
 * @retval     FSP_SUCCESS              Opened, fetched, and enabled successfully.
 * @retval     Any other error codes    Failed to open, fetch, or enable.
 **********************************************************************************************************************/
fsp_err_t acmphs_ep_comparator_init(comparator_info_t * p_stabilize_time)
{
    fsp_err_t err = FSP_SUCCESS;

    err = ACMPHS_OPEN(&g_comparator_ctrl, &g_comparator_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = ACMPHS_INFOGET(&g_comparator_ctrl, p_stabilize_time);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    return ACMPHS_OUTPUTENABLE(&g_comparator_ctrl);
}

/*******************************************************************************************************************//**
 * @brief      This function refreshes the comparator status when the comparator state flag is set.
 * @param[out] p_status                 Pointer to comparator_status_t used to store the comparator status.
 * @retval     FSP_SUCCESS              Refreshed successfully.
 * @retval     Any other error codes    Failed to refresh.
 **********************************************************************************************************************/
fsp_err_t acmphs_ep_status_refresh(comparator_status_t * p_status)
{
    fsp_err_t err = FSP_SUCCESS;

    if (s_b_comparator_state_flag)
    {
        err = ACMPHS_STATUSGET(&g_comparator_ctrl, p_status);
        if (FSP_SUCCESS == err)
        {
            s_b_comparator_state_flag = false;
        }
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function drives the on-board LED to reflect the given comparator output state.
 * @param[in] state                     Comparator output state (COMPARATOR_STATE_OUTPUT_HIGH or _LOW).
 * @retval    FSP_SUCCESS               Drove successfully.
 * @retval    Any other error codes     Failed to drive.
 **********************************************************************************************************************/
fsp_err_t acmphs_ep_led_update(comparator_state_t state)
{
    bsp_io_level_t level;

#if defined (BOARD_RA6T1_RSSK) || defined (BOARD_RA4T1_MCK) || defined (BOARD_RA6T2_MCK) ||\
    defined (BOARD_RA6T3_MCK) || defined (BOARD_RA8T1_MCK) || defined (BOARD_RA8T2_MCK)
    level = (COMPARATOR_STATE_OUTPUT_HIGH == state) ? BSP_IO_LEVEL_LOW : BSP_IO_LEVEL_HIGH;
#else
    level = (COMPARATOR_STATE_OUTPUT_HIGH == state) ? BSP_IO_LEVEL_HIGH : BSP_IO_LEVEL_LOW;
#endif /* Specific board */

    return R_IOPORT_PinWrite(&g_ioport_ctrl, LED_PIN, level);
}

/*******************************************************************************************************************//**
 * @brief     This function closes all opened modules.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void acmphs_ep_close(void)
{
#if (BSP_PERIPHERAL_DAC_PRESENT || BSP_PERIPHERAL_DAC_B_PRESENT)
    /* Close the opened DAC0 module */
    if (DAC0_CTRL->channel_opened)
    {
        DAC_CLOSE(DAC0_CTRL);
    }

    /* Close the opened DAC1 module */
#if (!(defined (BOARD_RA8E1_FPB) || defined (BOARD_RA8E2_EK)))
    if (DAC1_CTRL->channel_opened)
    {
        DAC_CLOSE(DAC1_CTRL);
    }
#endif /* BOARD_RA8E1_FPB, BOARD_RA8E2_EK */
#endif /* BSP_PERIPHERAL_DAC_PRESENT, BSP_PERIPHERAL_DAC_B_PRESENT */

    /* Close the opened ACMPHS module */
    if (g_comparator_ctrl.open)
    {
        ACMPHS_CLOSE(&g_comparator_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function is a callback for the ACMPHS module.
 * @param[in] p_args                    Pointer to the callback argument.
 * @retval    None.
 **********************************************************************************************************************/
void acmphs_user_callback(comparator_callback_args_t * p_args)
{
    /* Check for the channel 0 of comparator */
    if (0U == p_args->channel)
    {
        /* Set the flag */
        s_b_comparator_state_flag = true;
    }
}
