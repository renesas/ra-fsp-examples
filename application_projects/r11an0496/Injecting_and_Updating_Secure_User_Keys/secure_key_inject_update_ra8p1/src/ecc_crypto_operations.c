/***********************************************************************************************************************
 * File Name    : ecc_crypto_operations.c
 * Description  : Contains the cryptographic operations with ECC
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
************************************************************************************************************************/
#include <crypto_operations.h>
#include "hal_data.h"
#include "r_rsip.h"
#include "new_ecc_public_key.h"
#include "new_ecc_private_key.h"
#include "mram_storage.h"

static rsip_wrapped_key_t ecc_public_key_injected =
{
    .type = RSIP_KEY_TYPE_ECC_SECP256R1_PUBLIC, .p_value = (void *)DIRECT_ECC_PUB_KEY_ADDRESS
};

static rsip_wrapped_key_t ecc_private_key_injected =
{
    .type = RSIP_KEY_TYPE_ECC_SECP256R1_PRIVATE, .p_value = (void *)DIRECT_ECC_PRI_KEY_ADDRESS
};

#define SECP256R1_KEY_SIZE      (32)
#define IV_SIZE                 (16)

static uint8_t encrypted_new_ecc_pub_key[NEW_ECC_PUB_SIZE] = {0};
static uint8_t encrypted_new_ecc_pri_key[NEW_ECC_PRI_SIZE] = {0};
uint8_t iv_encrypt_pub[IV_SIZE] = {0};
uint8_t iv_encrypt_pri[IV_SIZE] = {0};

/* Prepare new wrapped key data area */
uint8_t wrapped_public_key_value[RSIP_BYTE_SIZE_WRAPPED_KEY(RSIP_KEY_TYPE_ECC_SECP256R1_PUBLIC)] BSP_ALIGN_VARIABLE(4);
rsip_wrapped_key_t wrapped_new_ecc_public_key =
{
    .type = RSIP_KEY_TYPE_ECC_SECP256R1_PUBLIC, .p_value = &wrapped_public_key_value
};

uint8_t wrapped_private_key_value[RSIP_BYTE_SIZE_WRAPPED_KEY(RSIP_KEY_TYPE_ECC_SECP256R1_PRIVATE)] BSP_ALIGN_VARIABLE(4);
rsip_wrapped_key_t wrapped_new_ecc_private_key =
{
    .type = RSIP_KEY_TYPE_ECC_SECP256R1_PRIVATE, .p_value = &wrapped_private_key_value
};

static rsip_wrapped_key_t kuk_key =
{
    .type = RSIP_KEY_TYPE_KUK, .p_value = (void *)KUK_ADDRESS
};

static void init_iv_encrypt_new_ecc_key(void);
static void init_encrypted_new_ecc_key(void);

/**********************************************************************************************
* Set of NIST ECC SECP256R1 test vectors is used for testing purpose. Refer to the AN for more details.
***********************************************************************************************/
/* NIST vector: plain text message used for signature creation for the directly injected ECC public key */
static uint8_t ECC_SECP256R1Message[] = {
0x59, 0x05, 0x23, 0x88, 0x77, 0xc7, 0x74, 0x21, 0xf7, 0x3e, 0x43, 0xee, 0x3d, 0xa6, 0xf2, 0xd9,
0xe2, 0xcc, 0xad, 0x5f, 0xc9, 0x42, 0xdc, 0xec, 0x0c, 0xbd, 0x25, 0x48, 0x29, 0x35, 0xfa, 0xaf,
0x41, 0x69, 0x83, 0xfe, 0x16, 0x5b, 0x1a, 0x04, 0x5e, 0xe2, 0xbc, 0xd2, 0xe6, 0xdc, 0xa3, 0xbd,
0xf4, 0x6c, 0x43, 0x10, 0xa7, 0x46, 0x1f, 0x9a, 0x37, 0x96, 0x0c, 0xa6, 0x72, 0xd3, 0xfe, 0xb5,
0x47, 0x3e, 0x25, 0x36, 0x05, 0xfb, 0x1d, 0xdf, 0xd2, 0x80, 0x65, 0xb5, 0x3c, 0xb5, 0x85, 0x8a,
0x8a, 0xd2, 0x81, 0x75, 0xbf, 0x9b, 0xd3, 0x86, 0xa5, 0xe4, 0x71, 0xea, 0x7a, 0x65, 0xc1, 0x7c,
0xc9, 0x34, 0xa9, 0xd7, 0x91, 0xe9, 0x14, 0x91, 0xeb, 0x37, 0x54, 0xd0, 0x37, 0x99, 0x79, 0x0f,
0xe2, 0xd3, 0x08, 0xd1, 0x61, 0x46, 0xd5, 0xc9, 0xb0, 0xd0, 0xde, 0xbd, 0x97, 0xd7, 0x9c, 0xe8
};

/*****************************************************************************************
 *  @brief      cryptograhpic_operation_with_directly_injected_ecc_key_pair: this function reads the directly injected
 *              ECC key pair from MRAM and perform cryptographic operations with the ECC key pair
 *  @retval     bool
 *  **************************************************************************************/
bool cryptograhpic_operation_with_ecc_key_pair(void)
{
    fsp_err_t error = FSP_SUCCESS;
    uint8_t hash[SECP256R1_KEY_SIZE] BSP_ALIGN_VARIABLE(4) = {0};
    uint8_t signature[64] BSP_ALIGN_VARIABLE(4) = {0};
    uint8_t * local_hash;

    R_RSIP_Open(&g_rsip_ctrl, &g_rsip_cfg);

    local_hash = ECC_SECP256R1Message;

    /* Generate the message hash */
    error = R_RSIP_SHA_Compute(&g_rsip_ctrl, RSIP_HASH_TYPE_SHA256, local_hash, sizeof(ECC_SECP256R1Message), hash);
    if (FSP_SUCCESS != error)
    {
        return false;
    }

    /* Generate the signature using the injected ECC private key*/
    error = R_RSIP_ECDSA_Sign(&g_rsip_ctrl, &ecc_private_key_injected, hash, signature);
    if (FSP_SUCCESS != error)
    {
        return false;
    }

    /* Verify the signature using the injected ECC public key*/
    error = R_RSIP_ECDSA_Verify(&g_rsip_ctrl, &ecc_public_key_injected, hash, signature);
    if (FSP_SUCCESS != error)
    {
        return false;
    }

    R_RSIP_Close(&g_rsip_ctrl);
    return true;
}

/* Access the encrypted new ECC key pair generated from SKMT */
void init_encrypted_new_ecc_key(void)
{
    memcpy(encrypted_new_ecc_pub_key, g_NEW_ECC_PUB.encrypted_user_key, NEW_ECC_PUB_SIZE);
    memcpy(encrypted_new_ecc_pri_key, g_NEW_ECC_PRI.encrypted_user_key, NEW_ECC_PRI_SIZE);
}

/* Access the initialization vector used for encrypting the new ECC key pair*/
void init_iv_encrypt_new_ecc_key(void)
{
    memcpy(iv_encrypt_pub, g_NEW_ECC_PUB.initial_vector, IV_SIZE);
    memcpy(iv_encrypt_pri, g_NEW_ECC_PRI.initial_vector, IV_SIZE);
}

/*****************************************************************************************
 *  @brief      update_ecc_key_pair: this function decrypts the key update key encrypted new ECC key pair
 *              and generates a new wrapped ECC key pair
 *  @retval     bool
 *  **************************************************************************************/
bool update_ecc_key_pair(void)
{
    fsp_err_t error = FSP_SUCCESS;

   R_RSIP_Open(&g_rsip_ctrl, &g_rsip_cfg);
   init_encrypted_new_ecc_key();
   init_iv_encrypt_new_ecc_key();

   /*Generate a new wrapped ECC public key using a previously injected Key-Update Key */
   error = R_RSIP_EncryptedKeyWrap(&g_rsip_ctrl, &kuk_key, iv_encrypt_pub, encrypted_new_ecc_pub_key ,&wrapped_new_ecc_public_key);
   if (FSP_SUCCESS != error)
   {
       return false;
   }

   /*Generate a new wrapped ECC private key using a previously injected Key-Update Key */
   error = R_RSIP_EncryptedKeyWrap(&g_rsip_ctrl, &kuk_key, iv_encrypt_pri, encrypted_new_ecc_pri_key ,&wrapped_new_ecc_private_key);
   if (FSP_SUCCESS != error)
   {
       return false;
   }

   R_RSIP_Close(&g_rsip_ctrl);
   error = store_new_ecc_key_pair_to_mram();
   if (FSP_SUCCESS != error)
   {
       return false;
   }
   return true;
}
