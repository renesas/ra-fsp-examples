/***********************************************************************************************************************
 * File Name    : crc_ep.c
 * Description  : Contains data structures and functions used in crc_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "crc_ep.h"

/*******************************************************************************************************************//**
 * @brief     This function initializes CRC module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t crc_init(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Open CRC module */
    err = R_CRC_Open(&g_crc_ctrl, &g_crc_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief      This function calculates the CRC value.
 * @param[in]  p_crc_input              Pointer to the CRC input data.
 * @param[out] crc_value                Pointer to the calculated CRC value.
 * @retval     FSP_SUCCESS              Calculated successfully.
 * @retval     Any other error codes    Failed to calculate.
 **********************************************************************************************************************/
fsp_err_t crc_calculate(crc_input_t * const p_crc_input, uint32_t * crc_value)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Calculate CRC value */
    err = R_CRC_Calculate(&g_crc_ctrl, p_crc_input, crc_value);

    return err;
}

#if (BSP_FEATURE_CRC_HAS_SNOOP)
/*******************************************************************************************************************//**
 * @brief     This function enables CRC snoop mode.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Enabled successfully.
 * @retval    Any other error codes     Failed to enable.
 **********************************************************************************************************************/
fsp_err_t crc_snoop_enable(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Enable CRC snoop mode */
    err = R_CRC_SnoopEnable(&g_crc_ctrl, SEED_VALUE);

    return err;
}

/*******************************************************************************************************************//**
 * @brief      This function returns the current calculated value.
 * @param[out] crc_value                Pointer to the calculated CRC value.
 * @retval     FSP_SUCCESS              Returned successfully.
 * @retval     Any other error codes    Failed to return.
 **********************************************************************************************************************/
fsp_err_t crc_calculated_value(uint32_t * crc_value)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Get CRC value in snoop mode */
    err = R_CRC_CalculatedValueGet(&g_crc_ctrl, crc_value);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function disables CRC snoop mode.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Disabled successfully.
 * @retval    Any other error codes     Failed to disable.
 **********************************************************************************************************************/
fsp_err_t crc_snoop_disable(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Disable CRC snoop mode */
    err = R_CRC_SnoopDisable(&g_crc_ctrl);

    return err;
}
#endif /* BSP_FEATURE_CRC_HAS_SNOOP */

/*******************************************************************************************************************//**
 * @brief     This function closes the opened CRC module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void crc_deinit(void)
{
    /* Close the opened CRC module */
    if (g_crc_ctrl.open)
    {
        R_CRC_Close(&g_crc_ctrl);
    }
}
