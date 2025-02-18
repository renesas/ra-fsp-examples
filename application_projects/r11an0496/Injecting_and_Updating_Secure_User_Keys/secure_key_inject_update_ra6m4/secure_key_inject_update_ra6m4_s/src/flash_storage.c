/***********************************************************************************************************************
 * File Name    : flash_storage.c
 * Description  : Contains flash operation routines
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
************************************************************************************************************************/

#include "hal_data.h"
#include "flash_storage.h"

extern sce_aes_wrapped_key_t wrapped_new_aes_key;
extern sce_ecc_public_wrapped_key_t wrapped_new_ecc_public_key;

static fsp_err_t error = FSP_SUCCESS;

fsp_err_t data_flash_save_aes_key(void);
fsp_err_t data_flash_save_ecc_pub_key(void);


/*******************************************************************************************************************//**
 * @brief This function is called from the RTT input to do the data flash operations.
 * @param[IN]   None
 * @retval      FSP_SUCCESS             Upon successful FLash_HP data flash operations.
 * @retval      Any Other Error code    Upon unsuccessful Flash_HP data flash operations.
 **********************************************************************************************************************/
fsp_err_t data_flash_save_ecc_pub_key(void)
{

    flash_result_t blank_check_result = FLASH_RESULT_BLANK;
    uint8_t write_buffer[BLOCK_SIZE] = {RESET_VALUE};
    uint8_t read_buffer[BLOCK_SIZE]= {RESET_VALUE};

    memcpy(write_buffer, (uint8_t*)(wrapped_new_ecc_public_key.value.key_management_info), sizeof(wrapped_new_ecc_public_key.value));

    /* Erase Block */
    error = R_FLASH_HP_Erase(&g_flash0_ctrl, DIRECT_ECC_PUB_KEY_ADDRESS, BLOCK_NUM);
    /* Error Handle */
    if (FSP_SUCCESS != error)
    {
        return error;
    }

    /* Data flash blank check */
    error = R_FLASH_HP_BlankCheck(&g_flash0_ctrl, DIRECT_ECC_PUB_KEY_ADDRESS, FLASH_HP_DF_BLOCK_SIZE, &blank_check_result);
    /* Error Handle */
    if (FSP_SUCCESS != error)
    {
        return error;
    }

    /* Validate the blank check result */
    if (FLASH_RESULT_BLANK != blank_check_result)
    {
        return (fsp_err_t)FLASH_RESULT_NOT_BLANK;
    }

    /* Write code flash data*/
    error = R_FLASH_HP_Write(&g_flash0_ctrl, (uint32_t) write_buffer, DIRECT_ECC_PUB_KEY_ADDRESS, BLOCK_SIZE);
    /* Error Handle */
    if (FSP_SUCCESS != error)
    {
        return error;
    }

    /*Read code flash data */
    memcpy(read_buffer, (uint8_t *) DIRECT_ECC_PUB_KEY_ADDRESS, BLOCK_SIZE);

    /* comparing the write_buffer and read_buffer */
    if (RESET_VALUE != memcmp(read_buffer, write_buffer, BLOCK_SIZE))
    {
        return FSP_ERR_WRITE_FAILED;
    }
    /* clear the wrapped new user key from the RAM buffer */
    memset((uint8_t*)&wrapped_new_ecc_public_key, 0, 4*(SCE_OEM_KEY_SIZE_ECCP256_PUBLICK_KEY_INST_DATA_WORD+1));
    return error;
}

fsp_err_t data_flash_save_aes_key(void)
{
    flash_result_t blank_check_result = FLASH_RESULT_BLANK;
    uint8_t write_buffer[BLOCK_SIZE] = {RESET_VALUE};
    uint8_t read_buffer[BLOCK_SIZE]= {RESET_VALUE};

    memcpy(write_buffer, (uint8_t*)(wrapped_new_aes_key.value), HW_SCE_AES256_KEY_INDEX_WORD_SIZE*4);


    /* Erase Block */
    error = R_FLASH_HP_Erase(&g_flash0_ctrl, DIRECT_AES_KEY_ADDRESS, BLOCK_NUM);
    /* Error Handle */
    if (FSP_SUCCESS != error)
    {
        return error;
    }

    /* Data flash blank check */
    error = R_FLASH_HP_BlankCheck(&g_flash0_ctrl, DIRECT_AES_KEY_ADDRESS, FLASH_HP_DF_BLOCK_SIZE, &blank_check_result);
    /* Error Handle */
    if (FSP_SUCCESS != error)
    {
        return error;
    }

    /* Validate the blank check result */
    if (FLASH_RESULT_BLANK != blank_check_result)
    {
        return (fsp_err_t)FLASH_RESULT_NOT_BLANK;
    }

    /* Write code flash data*/
    error = R_FLASH_HP_Write(&g_flash0_ctrl, (uint32_t) write_buffer, DIRECT_AES_KEY_ADDRESS, BLOCK_SIZE);
    /* Error Handle */
    if (FSP_SUCCESS != error)
    {
        return error;
    }

    /*Read code flash data */
    memcpy(read_buffer, (uint8_t *) DIRECT_AES_KEY_ADDRESS, BLOCK_SIZE);

    /* comparing the write_buffer and read_buffer */
    if (RESET_VALUE != memcmp(read_buffer, write_buffer, BLOCK_SIZE))
    {
        return FSP_ERR_WRITE_FAILED;
    }
    /* clear the wrapped new user key from the RAM buffer */
    memset((uint8_t*)&wrapped_new_aes_key, 0, 4*(HW_SCE_AES256_KEY_INDEX_WORD_SIZE+1));
    return error;
}
fsp_err_t store_new_aes_key_to_data_flash(void)
{
    error = R_FLASH_HP_Open(&g_flash0_ctrl, &g_flash0_cfg);
    error = R_FLASH_HP_StartUpAreaSelect(&g_flash0_ctrl, FLASH_STARTUP_AREA_BLOCK0, true);
    error = data_flash_save_aes_key();
    error = R_FLASH_HP_Close(&g_flash0_ctrl);

    return error;
}
fsp_err_t store_new_ecc_pub_key_to_data_flash(void)
{
    error = R_FLASH_HP_Open(&g_flash0_ctrl, &g_flash0_cfg);
    error = R_FLASH_HP_StartUpAreaSelect(&g_flash0_ctrl, FLASH_STARTUP_AREA_BLOCK0, true);
    error = data_flash_save_ecc_pub_key();
    error = R_FLASH_HP_Close(&g_flash0_ctrl);

    return error;
}
