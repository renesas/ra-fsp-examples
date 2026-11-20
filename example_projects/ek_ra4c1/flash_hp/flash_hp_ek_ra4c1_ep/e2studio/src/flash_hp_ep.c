/***********************************************************************************************************************
 * File Name    : flash_hp_ep.c
 * Description  : Contains macros, data structures and functions used in flash_hp_ep.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdbool.h>
#include "flash_hp_ep.h"

/* Flags set from callback function */
static volatile _Bool g_b_flash_event_not_blank = false;
static volatile _Bool g_b_flash_event_blank = false;
static volatile _Bool g_b_flash_event_erase_complete = false;
static volatile _Bool g_b_flash_event_write_complete = false;

/* Private function */
static fsp_err_t blankcheck_event_flag(void);

/*******************************************************************************************************************//**
 * @brief     This function initializes Flash HP module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t flash_hp_init(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Open Flash HP */
    err = R_FLASH_HP_Open(&g_flash_ctrl, &g_flash_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function selects block as the startup area.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Startup area selected successfully.
 * @retval    Any other error codes     Failed to select the startup area.
 **********************************************************************************************************************/
fsp_err_t flash_hp_startup_block(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Setup default block 0 as startup setup block */
    err = R_FLASH_HP_StartUpAreaSelect(&g_flash_ctrl, FLASH_STARTUP_AREA_BLOCK0, true);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function erases a code flash or data flash block.
 * @param[in] block_addr                Flash block address to erase.
 * @retval    FSP_SUCCESS               Erased successfully.
 * @retval    Any other error codes     Failed to erase flash.
 **********************************************************************************************************************/
fsp_err_t flash_hp_erase(uint32_t block_addr)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    err = R_FLASH_HP_Erase(&g_flash_ctrl, block_addr, BLOCK_NUM);

    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Wait for the erase complete event flag, if BGO is SET (data flash only) */
    if ((FLASH_HP_DF_BLOCK_0 <= block_addr) && (true == g_flash_cfg.data_flash_bgo))
    {
        while (!g_b_flash_event_erase_complete);
        g_b_flash_event_erase_complete = false;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function performs a blank check on a code flash or data flash block.
 * @param[in] block_addr                Flash block address to blank check.
 * @param[in] num_bytes                 Number of bytes in the block to check.
 * @retval    FSP_SUCCESS               Flash block is blank.
 * @retval    Any other error codes     Failed to perform blank check.
 **********************************************************************************************************************/
fsp_err_t flash_hp_blank_check(uint32_t block_addr, uint32_t num_bytes)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;
    flash_result_t blank_check_result = FLASH_RESULT_BLANK;

    /* Blank check */
    err = R_FLASH_HP_BlankCheck(&g_flash_ctrl, block_addr, num_bytes, &blank_check_result);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    else
    {
        /* Validate the blank check result */
        if (FLASH_RESULT_BLANK == blank_check_result)
        {
            return FSP_SUCCESS;
        }
        else if (FLASH_RESULT_NOT_BLANK == blank_check_result)
        {
            return (fsp_err_t)FLASH_RESULT_NOT_BLANK;
        }
        else if (FLASH_RESULT_BGO_ACTIVE == blank_check_result)
        {
            /* BlankCheck will update in callback */
            /* Event flag will be updated in the blank check function when BGO is enabled */
            err = blankcheck_event_flag();
            if (FSP_SUCCESS != err)
            {
                return err;
            }
        }
        else
        {
            /* Do nothing */
        }
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function writes data to a code flash or data flash address.
 * @param[in] src_address               Source buffer containing data to write.
 * @param[in] flash_address             Destination flash address.
 * @param[in] num_bytes                 Number of bytes to write.
 * @retval    FSP_SUCCESS               Written successfully.
 * @retval    Any other error codes     Failed to write flash.
 **********************************************************************************************************************/
fsp_err_t flash_hp_write(uint8_t * src_address, uint32_t flash_address, uint32_t num_bytes)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    err = R_FLASH_HP_Write(&g_flash_ctrl, (uint32_t) src_address, flash_address, num_bytes);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Wait for the write complete event flag, if BGO is SET (data flash only) */
    if ((FLASH_HP_DF_BLOCK_0 <= flash_address) && (true == g_flash_cfg.data_flash_bgo))
    {
        while (!g_b_flash_event_write_complete);
        g_b_flash_event_write_complete = false;
    }

    return err;
}

#if (BSP_FEATURE_FLASH_SUPPORTS_ACCESS_WINDOW)
/*******************************************************************************************************************//**
 * @brief     This function sets the access window on code flash blocks 3 to 7, erases and writes to block 5,
 *            then clears the access window.
 * @param[in] write_buffer              Buffer containing data to write to code flash block 5.
 * @retval    FSP_SUCCESS               Access window operations completed successfully.
 * @retval    Any other error codes     Failed to perform access window operations.
 **********************************************************************************************************************/
fsp_err_t flash_hp_access_window_operations(uint8_t * write_buffer)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Disable interrupts to prevent vector table access while code flash is in P/E mode */
    __disable_irq();

    /* Set Access window.
     * CAUTION: Highly recommended not to use this function if not aware of consequences OR
     * use it with the accessWindowClear API at the end of application.
     * This API locks the Code flash and the lock retains even after power cycle.
     * Which means, even after power cycle, the user will not be able to program the code to code flash if the
     * access window is wrongly set.
     *
     * WORKAROUND: If the user uses accessWindowSet and locks the window. Flash can be unlocked by running
     * different application performing just "open" call and "accessWindowClear()" running from the RAM.
     *
     * For further information on the Access Window Features refer the Application Note
     * https://www.renesas.com/us/en/document/apn/securing-data-rest-utilizing-renesas-security-mpu?language=en
     */
    err = R_FLASH_HP_AccessWindowSet(&g_flash_ctrl, FLASH_HP_CF_BLOCK_3, FLASH_HP_CF_BLOCK_7);
    if (FSP_SUCCESS != err)
    {
        __enable_irq();
        return err;
    }

    err = flash_hp_erase(FLASH_HP_CF_BLOCK_5);
    if (FSP_SUCCESS != err)
    {
        __enable_irq();
        return err;
    }

    /* Write code flash data */
    err = flash_hp_write(write_buffer, FLASH_HP_CF_BLOCK_5, BLOCK_SIZE);
    if (FSP_SUCCESS != err)
    {
        __enable_irq();
        return err;
    }

    /* Clear Flash Access Window */
    err = R_FLASH_HP_AccessWindowClear(&g_flash_ctrl);

    __enable_irq();

    return err;
}
#endif /* BSP_FEATURE_FLASH_SUPPORTS_ACCESS_WINDOW */

/*******************************************************************************************************************//**
 * @brief     This function closes the opened Flash HP module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void flash_hp_deinit(void)
{
    /* Close the opened Flash HP module */
    if (g_flash_ctrl.opened)
    {
        R_FLASH_HP_Close(&g_flash_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function is callback for Flash HP HAL.
 * @param[in] p_args                    Pointer to the callback function parameter data.
 * @retval    None.
 **********************************************************************************************************************/
void bgo_callback(flash_callback_args_t *p_args)
{
    if (FLASH_EVENT_NOT_BLANK == p_args->event)
    {
        g_b_flash_event_not_blank = true;
    }
    else if (FLASH_EVENT_BLANK == p_args->event)
    {
        g_b_flash_event_blank = true;
    }
    else if (FLASH_EVENT_ERASE_COMPLETE == p_args->event)
    {
        g_b_flash_event_erase_complete = true;
    }
    else if (FLASH_EVENT_WRITE_COMPLETE == p_args->event)
    {
        g_b_flash_event_write_complete = true;
    }
    else
    {
        /* Do nothing */
    }
}

/*******************************************************************************************************************//**
 * @brief     This function is called to set the blank check event flags.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Upon successful Flash HP is blank.
 * @retval    Any other error codes     Upon unsuccessful Flash HP is not blank.
 **********************************************************************************************************************/
static fsp_err_t blankcheck_event_flag(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;
    
    /* Wait for callback function to set flag */
    while (!(g_b_flash_event_not_blank || g_b_flash_event_blank))
    {
        /* Do nothing */
    }

    if (g_b_flash_event_not_blank)
    {
        /* Reset flag */
        g_b_flash_event_not_blank = false;
        return (fsp_err_t)FLASH_EVENT_NOT_BLANK;
    }
    else
    {
        /* Reset flag */
        g_b_flash_event_blank = false;
    }

    return err;
}
