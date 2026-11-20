/***********************************************************************************************************************
 * File Name    : mram_ep.c
 * Description  : Contains data structures and functions used in mram_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2025 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "mram_ep.h"

/*******************************************************************************************************************//**
 * @brief     This function opens the MRAM module and selects Block 0 as the startup area.
 * @param[in] None.
 * @retval    FSP_SUCCESS                   Initialized and selected successfully.
 * @retval    Any other error codes         Failed to initialize or select.
 **********************************************************************************************************************/
fsp_err_t mram_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open MRAM */
    err = R_MRAM_Open(&g_mram_ctrl, &g_mram_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Set Default Block 0 as Startup Setup Block */
    err = R_MRAM_StartUpAreaSelect(&g_mram_ctrl, FLASH_STARTUP_AREA_BLOCK0, true);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     De-initializes the opened MRAM module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void mram_deinit(void)
{
    /* De-initialize the opened MRAM module */
    if (g_mram_ctrl.opened)
    {
        R_MRAM_Close(&g_mram_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function writes sequential data to MRAM.
 * @param[in] mram_address                  Start address of the region to write.
 * @param[in] src_address                   Pointer to the data to write.
 * @param[in] num_bytes                     Number of bytes to write.
 * @retval    FSP_SUCCESS                   Written successfully.
 * @retval    Any other error codes         Failed to write.
 **********************************************************************************************************************/
fsp_err_t mram_write(uint32_t mram_address, uint8_t const * src_address, uint32_t num_bytes)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Disable interrupts to prevent vector table access while code MRAM is in P/E mode */
    __disable_irq();

    err = R_MRAM_Write(&g_mram_ctrl, (uint32_t) src_address, mram_address, num_bytes);

    /* Enable interrupts after code MRAM operations are complete */
    __enable_irq();

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function erases MRAM blocks starting from the specified address.
 * @param[in] address                       Start address of the region to erase.
 * @param[in] num_blocks                    Number of blocks to erase.
 * @retval    FSP_SUCCESS                   Erased successfully.
 * @retval    Any other error codes         Failed to erase.
 **********************************************************************************************************************/
fsp_err_t mram_erase(uint32_t address, uint32_t num_blocks)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Disable interrupts to prevent vector table access while code MRAM is in P/E mode */
    __disable_irq();

    err = R_MRAM_Erase(&g_mram_ctrl, address, num_blocks);

    /* Enable interrupts after code MRAM operations are complete */
    __enable_irq();

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function checks whether a specified MRAM region is blank (all bytes are 0xFF).
 * @param[in] mram_address                  Start address of the MRAM region to check.
 * @param[in] num_bytes                     Number of bytes to check.
 * @retval    true                          Region is blank.
 * @retval    false                         Region contains non-blank data.
 **********************************************************************************************************************/
bool mram_is_blank(uint32_t mram_address, uint32_t num_bytes)
{
    uint8_t const * p_addr = (uint8_t const *) mram_address;

    for (uint32_t index = 0; index < num_bytes; index++)
    {
        if (UINT8_MAX != p_addr[index])
        {
            return false;
        }
    }

    return true;
}
