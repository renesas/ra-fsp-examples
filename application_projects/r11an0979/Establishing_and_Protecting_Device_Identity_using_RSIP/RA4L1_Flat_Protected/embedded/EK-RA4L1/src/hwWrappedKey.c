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
#include "common_util.h"
#include "cryptoTypes.h"
#include "hal_data.h"

#define HASH_SIZE               32
#define ECC_SIG_SIZE            64
#define ECC_PSA_KEY_SKIP        1               /* skip the PSA ECC key padding to get the key */

/* Prepare wrapped key data area */
uint8_t wrapped_public_key_value[RSIP_BYTE_SIZE_WRAPPED_KEY(RSIP_KEY_TYPE_ECC_SECP256R1_PUBLIC)] BSP_ALIGN_VARIABLE(4);
rsip_wrapped_key_t wrapped_public_key =
{
    .type = RSIP_KEY_TYPE_ECC_SECP256R1_PUBLIC, .p_value = &wrapped_public_key_value
};
uint8_t wrapped_private_key_value[RSIP_BYTE_SIZE_WRAPPED_KEY(RSIP_KEY_TYPE_ECC_SECP256R1_PRIVATE)] BSP_ALIGN_VARIABLE(4);
rsip_wrapped_key_t wrapped_private_key =
{
    .type = RSIP_KEY_TYPE_ECC_SECP256R1_PRIVATE, .p_value = &wrapped_private_key_value
};


/*****************************************************************************************************************
 *  @brief      hwSignData
 *  @param[in]  hash_buffer: hash to be signed by the private key
 *  @param[in]  sig_buffer:  signature to send to the PC program
 *  @param[in]  key_handle:  ECC key handle
 *  @retval     status:      true if signing operation is successful
 *  ****************************************************************************************************************/

bool hwSignData(uint8_t * hash_buffer, uint8_t sig_buffer[])
{
    volatile fsp_err_t status = FSP_SUCCESS;

    status = R_RSIP_ECDSA_Sign(&g_rsip_ctrl, &wrapped_private_key, hash_buffer, sig_buffer);
    if (FSP_SUCCESS != status)
    {
        status = false;
    }
    else
    {
        status = true;
    }

    return 1;
}

/*****************************************************************************************************************
 *  @brief      hwCreatekey
 *  @param[in]  pubkey_buffer: the buffer which holds the public key sending to the PC
 *  @param[in]  key_handle:  ECC key handle
 *  @retval     status:      true if signing operation is successful
 *  ****************************************************************************************************************/

bool hwCreatekey(uint8_t * pubkey_buffer)
{
    volatile fsp_err_t status = FSP_SUCCESS;

    status = R_RSIP_KeyPairGenerate(&g_rsip_ctrl, &wrapped_public_key, &wrapped_private_key);
    assert(FSP_SUCCESS == status);
    status = R_RSIP_PublicKeyExport(&wrapped_public_key, pubkey_buffer);

   if (FSP_SUCCESS == status)
   {
       return true;
   }
   else
   {
       return false;
   }
}
