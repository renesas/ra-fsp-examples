/***********************************************************************************************************************
 * File Name    : hwWrappedKey.c
 * Description  : provide ECC key pair generation and signing service.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "hwWrappedKey.h"
#include "flash.h"
#include "r_rsip.h"
#include "common_util.h"
#include "hal_data.h"
#define HASH_SIZE               32


/*****************************************************************************************************************
 *  @brief      hwSignData
 *  @param[in]  hash_buffer: hash to be signed by the private key
 *  @param[in]  sig_buffer:  signature to send to the PC program
 *  @retval     status:      true if signing operation is successful
 *  ****************************************************************************************************************/
bool hwSignData(uint8_t *hash_buffer, uint8_t sig_buffer[])
{
    volatile fsp_err_t status = FSP_SUCCESS;
    int32_t res;

    // Prepare buffer for wrapped private key
    uint8_t            wrapped_private_key_value[RSIP_BYTE_SIZE_WRAPPED_KEY(RSIP_KEY_TYPE_RSA_4096_PRIVATE)] BSP_ALIGN_VARIABLE(4);
    rsip_wrapped_key_t wrapped_private_key =
    {
        .type = RSIP_KEY_TYPE_RSA_4096_PRIVATE, .p_value = &wrapped_private_key_value
    };

    // Read private key from file
    res = read_file(&g_rm_littlefs0_lfs, "/privkey.bin", wrapped_private_key_value, sizeof(wrapped_private_key_value));
    if (res < 0) {
        return false;
    }

    // Sign the hash using RSA with SHA256
    status = R_RSIP_RSASSA_PKCS1_V1_5_Sign(&g_rsip_ctrl, &wrapped_private_key, RSIP_HASH_TYPE_SHA256, hash_buffer, sig_buffer);

    if (status != FSP_SUCCESS) {
        return false;
    }

    // Close RSIP driver
    status = R_RSIP_Close(&g_rsip_ctrl);

    return true;
}

/*****************************************************************************************************************
 *  @brief      hwCreatekey
 *  @param[in]  pubkey_buffer: the buffer which holds the public key sending to the PC
 *  @retval     status:      true if signing operation is successful
 *  ****************************************************************************************************************/
bool hwCreatekey(uint8_t *pubkey_buffer)
{
    // Declare local variables and define the status variable for error handling.
    volatile fsp_err_t status = FSP_SUCCESS;
    int32_t res;

    /* Prepare wrapped key data area */
    uint8_t            wrapped_public_key_value[RSIP_BYTE_SIZE_WRAPPED_KEY(RSIP_KEY_TYPE_RSA_4096_PUBLIC)] BSP_ALIGN_VARIABLE(4);
    rsip_wrapped_key_t wrapped_public_key =
    {
        .type = RSIP_KEY_TYPE_RSA_4096_PUBLIC, .p_value = &wrapped_public_key_value
    };

    uint8_t            wrapped_private_key_value[RSIP_BYTE_SIZE_WRAPPED_KEY(RSIP_KEY_TYPE_RSA_4096_PRIVATE)] BSP_ALIGN_VARIABLE(4);
    rsip_wrapped_key_t wrapped_private_key =
    {
        .type = RSIP_KEY_TYPE_RSA_4096_PRIVATE, .p_value = &wrapped_private_key_value
    };
    // Open the LittleFS (filesystem) for storage.
    status = RM_LITTLEFS_FLASH_Open(&g_rm_littlefs0_ctrl, &g_rm_littlefs0_cfg);
    assert(FSP_SUCCESS == status);

    // Format the LittleFS if required.
    status = lfs_format(&g_rm_littlefs0_lfs, &g_rm_littlefs0_lfs_cfg);
    assert(FSP_SUCCESS == status);

    // Mount the LittleFS.
    status = lfs_mount(&g_rm_littlefs0_lfs, &g_rm_littlefs0_lfs_cfg);
    assert(FSP_SUCCESS == status);

    // Initialize the RSA key pair generation driver.
    status = R_RSIP_Open(&g_rsip_ctrl, &g_rsip_cfg);
    assert(FSP_SUCCESS == status);

    // Generate an RSA-4096 key pair (public and private keys).
    status = R_RSIP_KeyPairGenerate(&g_rsip_ctrl, &wrapped_public_key, &wrapped_private_key);
    assert(FSP_SUCCESS == status);

    // Save the wrapped public key to a file on the LittleFS.
    res = write_file(&g_rm_littlefs0_lfs, "/pubkey.bin", wrapped_public_key_value, sizeof(wrapped_public_key_value));
    if (res < 0)
    {
        return false;  // Return false if there is an error writing the public key.
    }

    // Save the wrapped private key to a file on the LittleFS.
    res = write_file(&g_rm_littlefs0_lfs, "/privkey.bin", wrapped_private_key_value, sizeof(wrapped_private_key_value));
    if (res < 0)
    {
        return false;  // Return false if there is an error writing the private key.
    }

    // Export the public key into the provided buffer to be sent to the PC.
    R_RSIP_PublicKeyExport(&wrapped_public_key, pubkey_buffer);

    return true;  // Return true if everything was successful.
}

