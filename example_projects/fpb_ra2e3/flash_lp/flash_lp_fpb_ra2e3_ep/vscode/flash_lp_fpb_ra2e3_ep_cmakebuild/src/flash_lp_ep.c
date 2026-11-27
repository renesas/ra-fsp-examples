/***********************************************************************************************************************
 * File Name    : flash_lp_ep.c
 * Description  : Contains data structures and functions used in flash_lp_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "flash_lp_ep.h"

/* Global variable */
flash_result_t g_blank_check_result = FLASH_RESULT_BLANK;

/* Flags from callback function */
static volatile bool s_b_flash_event_not_blank = false;
static volatile bool s_b_flash_event_blank = false;
static volatile bool s_b_flash_event_erase_complete = false;
static volatile bool s_b_flash_event_write_complete = false;

/*******************************************************************************************************************//**
 * @brief     This function initializes Flash LP module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t flash_lp_init(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Open Flash LP module */
    err = R_FLASH_LP_Open(&g_flash_ctrl, &g_flash_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function closes the opened Flash LP module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void flash_lp_deinit(void)
{
    /* Close the opened Flash LP module */
    if (g_flash_ctrl.opened)
    {
       R_FLASH_LP_Close(&g_flash_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function performs a blank check on the flash block.
 * @param[in] block_addr                Flash block address to be checked.
 * @retval    FSP_SUCCESS               Checked successfully.
 * @retval    Any other error codes     Failed to check.
 **********************************************************************************************************************/
fsp_err_t flash_lp_blank_check(uint32_t block_addr)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    g_blank_check_result = FLASH_RESULT_BLANK;

    /* Blank check */
    err = R_FLASH_LP_BlankCheck(&g_flash_ctrl, block_addr, BLOCK_SIZE, &g_blank_check_result);
    if (FSP_SUCCESS == err)
    {
        if (FLASH_RESULT_BGO_ACTIVE == g_blank_check_result)
        {
            /* Wait for callback function to set flag */
            while (!(s_b_flash_event_not_blank || s_b_flash_event_blank))
            {
                /* Do nothing */
            }

            if (s_b_flash_event_not_blank)
            {
                g_blank_check_result = FLASH_RESULT_NOT_BLANK;
                s_b_flash_event_not_blank = false;
            }
            else
            {
                g_blank_check_result = FLASH_RESULT_BLANK;
                s_b_flash_event_blank = false;
            }
        }
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function erases a code flash or data flash block.
 * @param[in] block_addr                Flash block address to erase.
 * @retval    FSP_SUCCESS               Erased successfully.
 * @retval    Any other error codes     Failed to erase.
 **********************************************************************************************************************/
fsp_err_t flash_lp_erase(uint32_t block_addr)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Reset the erase-complete flag */
    s_b_flash_event_erase_complete = false;

    err = R_FLASH_LP_Erase(&g_flash_ctrl, block_addr, NUM_BLOCKS);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Wait for the erase complete event flag, if BGO is SET (data flash only) */
    if ((FLASH_DF_BLOCK0 <= block_addr) && (true == g_flash_cfg.data_flash_bgo))
    {
        while (!s_b_flash_event_erase_complete)
        {
            /* Wait erase complete */
        }
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function writes data to a code flash or data flash address.
 * @param[in] src_address               Source buffer containing data to write.
 * @param[in] flash_address             Destination flash address.
 * @retval    FSP_SUCCESS               Written successfully.
 * @retval    Any other error codes     Failed to write flash.
 **********************************************************************************************************************/
fsp_err_t flash_lp_write(uint8_t * src_address, uint32_t flash_address)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Reset flag */
    s_b_flash_event_write_complete = false;

    err = R_FLASH_LP_Write(&g_flash_ctrl, (uint32_t)src_address, flash_address, BLOCK_SIZE);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Wait here for the write operation to complete (only needed with BGO) */
    if ((FLASH_DF_BLOCK0 <= flash_address) && (true == g_flash_cfg.data_flash_bgo))
    {
        while (!s_b_flash_event_write_complete)
        {
            /* Do nothing */
        }
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function configures an access window.
 * @param[in] block_addr                Code flash block address.
 * @retval    FSP_SUCCESS               Configured successfully.
 * @retval    Any other error codes     Failed to configure.
 **********************************************************************************************************************/
fsp_err_t flash_lp_access_window_set(uint32_t block_addr)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    err = R_FLASH_LP_AccessWindowSet(&g_flash_ctrl, FLASH_CF_START_ADDR(block_addr), FLASH_CF_END_ADDR(block_addr));

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function clears the configured access window.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Cleared successfully.
 * @retval    Any other error codes     Failed to clear.
 **********************************************************************************************************************/
fsp_err_t flash_lp_access_window_clear(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    err = R_FLASH_LP_AccessWindowClear(&g_flash_ctrl);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function handles Flash LP callback events.
 * @param[in] p_args                    Pointer to callback arguments.
 * @retval    None.
 **********************************************************************************************************************/
void bgo_callback(flash_callback_args_t * p_args)
{
    switch (p_args->event)
    {
        case FLASH_EVENT_NOT_BLANK:
        {
            s_b_flash_event_not_blank = true;
            break;
        }

        case FLASH_EVENT_BLANK:
        {
            s_b_flash_event_blank = true;
            break;
        }

        case FLASH_EVENT_ERASE_COMPLETE:
        {
            s_b_flash_event_erase_complete = true;
            break;
        }

        case FLASH_EVENT_WRITE_COMPLETE:
        {
            s_b_flash_event_write_complete = true;
            break;
        }

        default:
        {
            /* Do nothing */
            break;
        }
    }
}
