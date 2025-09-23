/***********************************************************************************************************************
 * File Name    : mram_storage.c
 * Description  : Contains MRAM operation routines
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
************************************************************************************************************************/

#include <mram_storage.h>
#include "hal_data.h"

extern rsip_wrapped_key_t wrapped_new_ecc_public_key;
extern rsip_wrapped_key_t wrapped_new_ecc_private_key;

static fsp_err_t error = FSP_SUCCESS;

bool mram_is_blank(uint32_t mram_address, uint32_t num_bytes)
{
    uint8_t *p_addr = (uint8_t *) mram_address;
    for (uint32_t index = 0; index < num_bytes; index++)
    {
        if (p_addr[index] != UINT8_MAX)
        {
            return false; /* Not blank */
        }
    }
    return true;
}

fsp_err_t mram_save_ecc_key_pair(void)
{
    uint8_t pub_write_buffer[NUM_BYTES] = {RESET_VALUE};
    uint8_t pri_write_buffer[NUM_BYTES] = {RESET_VALUE};
    uint8_t pub_read_buffer[NUM_BYTES]= {RESET_VALUE};
    uint8_t pri_read_buffer[NUM_BYTES]= {RESET_VALUE};

    memcpy(pub_write_buffer, wrapped_new_ecc_public_key.p_value, RSIP_BYTE_SIZE_WRAPPED_KEY(RSIP_KEY_TYPE_ECC_SECP256R1_PUBLIC));

    /* Disable interrupts to prevent vector table access while code MRAM is in P/E mode */
    __disable_irq();

    /* Erase Block of injected public key*/
    error = R_MRAM_Erase(&g_mram_ctrl, DIRECT_ECC_PUB_KEY_ADDRESS, BLOCK_NUM);
    /* Error Handle */
    if (FSP_SUCCESS != error)
    {
        return error;
    }

    /* Check blank before writing */
    if (!mram_is_blank(DIRECT_ECC_PUB_KEY_ADDRESS, NUM_BYTES))
    {
        return FSP_ERR_ABORTED;
    }
    /* Write updated public key data*/
    error = R_MRAM_Write(&g_mram_ctrl, (uint32_t) pub_write_buffer, DIRECT_ECC_PUB_KEY_ADDRESS, NUM_BYTES);
    /* Error Handle */
    if (FSP_SUCCESS != error)
    {
        return error;
    }
    /*Read MRAM data */
    memcpy(pub_read_buffer, (uint8_t *) DIRECT_ECC_PUB_KEY_ADDRESS, NUM_BYTES);

    /*Comparing the write_buffer and read_buffer */
    if (RESET_VALUE != memcmp(pub_read_buffer, pub_write_buffer, NUM_BYTES))
    {
        return FSP_ERR_WRITE_FAILED;
    }
    /* clear the wrapped new user public key from the RAM buffer */
    memset(wrapped_new_ecc_public_key.p_value, 0, RSIP_BYTE_SIZE_WRAPPED_KEY(RSIP_KEY_TYPE_ECC_SECP256R1_PUBLIC));


    memcpy(pri_write_buffer, wrapped_new_ecc_private_key.p_value, RSIP_BYTE_SIZE_WRAPPED_KEY(RSIP_KEY_TYPE_ECC_SECP256R1_PRIVATE));
    /* Erase Block of injected private key*/
    error = R_MRAM_Erase(&g_mram_ctrl, DIRECT_ECC_PRI_KEY_ADDRESS, BLOCK_NUM);
    /* Error Handle */
    if (FSP_SUCCESS != error)
    {
        return error;
    }

    /* Check blank before writing */
    if (!mram_is_blank(DIRECT_ECC_PRI_KEY_ADDRESS, NUM_BYTES))
    {
        return FSP_ERR_ABORTED;
    }
    /* Write updated private key data*/
    error = R_MRAM_Write(&g_mram_ctrl, (uint32_t) pri_write_buffer, DIRECT_ECC_PRI_KEY_ADDRESS, NUM_BYTES);
    /* Error Handle */
    if (FSP_SUCCESS != error)
    {
        return error;
    }

    /*Read MRAM data */
    memcpy(pri_read_buffer, (uint8_t *) DIRECT_ECC_PRI_KEY_ADDRESS, NUM_BYTES);

    /*Comparing the write_buffer and read_buffer */
    if (RESET_VALUE != memcmp(pub_read_buffer, pub_write_buffer, NUM_BYTES))
    {
        return FSP_ERR_WRITE_FAILED;
    }
    /*Clear the wrapped new user key from the RAM buffer */
    memset(wrapped_new_ecc_private_key.p_value, 0, RSIP_BYTE_SIZE_WRAPPED_KEY(RSIP_KEY_TYPE_ECC_SECP256R1_PRIVATE));

    /* Enable interrupts after MRAM operations are complete. */
    __enable_irq();
    return error;
}

fsp_err_t store_new_ecc_key_pair_to_mram(void)
{
    error = R_MRAM_Open(&g_mram_ctrl, &g_mram_cfg);
    error = R_MRAM_StartUpAreaSelect(&g_mram_ctrl, FLASH_STARTUP_AREA_BLOCK0, true);
    error = mram_save_ecc_key_pair();
    error = R_MRAM_Close(&g_mram_ctrl);
    return error;
}
