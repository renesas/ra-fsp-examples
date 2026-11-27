/***********************************************************************************************************************
 * File Name    : vee_flash_ep.c
 * Description  : Contains data structures and functions used in vee_flash_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "vee_flash_ep.h"

/* Capture callback event */
static volatile bool s_write_flag = false;

/*******************************************************************************************************************//**
 * @brief     This function initializes RM_VEE_FLASH module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t vee_flash_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open the RM_VEE_FLASH module */
    err = RM_VEE_FLASH_Open(&g_vee_ctrl, &g_vee_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function writes the data to data flash.
 * @param[in] p_api_ctrl                Pointer to the Virtual EEPROM (VEEPROM) API control block.
 * @param[in] rec_id                    Record ID.
 * @param[in] p_rec_data                Actual data.
 * @param[in] num_bytes                 Number of bytes to write.
 * @retval    FSP_SUCCESS               Wrote successfully.
 * @retval    Any other error codes     Failed to write.
 **********************************************************************************************************************/
fsp_err_t vee_write(rm_vee_ctrl_t * const p_api_ctrl, uint32_t const rec_id,
                    void const * const p_rec_data, uint32_t const num_bytes)
{
    fsp_err_t err = FSP_SUCCESS;
    uint16_t write_time_out = UINT16_MAX;

    /* Reset the flag before writing the data */
    s_write_flag = false;

    /* Write the data to a VEEPROM Record */
    err = RM_VEE_FLASH_RecordWrite(p_api_ctrl, rec_id, p_rec_data, num_bytes);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Wait for the VEEPROM callback to indicate that it has finished writing data
     * and VEEPROM is in ready state */
    while (true != s_write_flag)
    {
        /* Start checking for time out to avoid infinite loop */
        --write_time_out;
        if (RESET_VALUE == write_time_out)
        {
            /* Callback event did not occur within the timeout period */
            return FSP_ERR_TIMEOUT;
        }
    }

    /* Reset the flag */
    s_write_flag = false;

    return err;
}

/*******************************************************************************************************************//**
 * @brief      This function reads the data from the RECORD_ID.
 * @param[in]  p_api_ctrl               Pointer to the VEEPROM API control block.
 * @param[in]  rec_id                   Record ID.
 * @param[out] pp_rec_data              Pointer to the record data pointer.
 * @param[out] p_num_bytes              Number of bytes in the record.
 * @retval     FSP_SUCCESS              Read successfully.
 * @retval     Any other error codes    Failed to read.
 **********************************************************************************************************************/
fsp_err_t vee_read(rm_vee_ctrl_t * const p_api_ctrl, uint32_t const rec_id,
                   uint8_t ** const pp_rec_data, uint32_t * const p_num_bytes)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Read data from a VEEPROM Record */
    err = RM_VEE_FLASH_RecordPtrGet(p_api_ctrl, rec_id, pp_rec_data, p_num_bytes);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function erases the entire data flash used by VEEPROM.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Erased successfully.
 * @retval    Any other error codes     Failed to erase.
 **********************************************************************************************************************/
fsp_err_t vee_format(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t ref_data = RESET_VALUE;

    /* Start a manual format operation */
    err = RM_VEE_FLASH_Format(&g_vee_ctrl, &ref_data);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function gets the current status of the RM_VEE_FLASH module.
 * @param[in] p_status                  Pointer to the VEEPROM status structure.
 * @retval    FSP_SUCCESS               Got successfully.
 * @retval    Any other error codes     Failed to get.
 **********************************************************************************************************************/
fsp_err_t vee_status_get(rm_vee_status_t * const p_status)
{
    fsp_err_t err = FSP_SUCCESS;

    err = RM_VEE_FLASH_StatusGet(&g_vee_ctrl, p_status);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function manually starts a refresh operation.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Refreshed successfully.
 * @retval    Any other error codes     Failed to refresh.
 **********************************************************************************************************************/
fsp_err_t vee_refresh(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = RM_VEE_FLASH_Refresh(&g_vee_ctrl);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function de-initializes the opened RM_VEE_FLASH module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void vee_flash_deinit(void)
{
    if (g_vee_ctrl.open)
    {
        RM_VEE_FLASH_Close(&g_vee_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function is the callback for the RM_VEE_FLASH module.
 * @param[in] p_args                    Pointer to the callback argument.
 * @retval    None.
 **********************************************************************************************************************/
void vee_callback(rm_vee_callback_args_t * p_args)
{
    if ((NULL != p_args) && (RM_VEE_STATE_READY == p_args->state))
    {
        s_write_flag = true;
    }
}
