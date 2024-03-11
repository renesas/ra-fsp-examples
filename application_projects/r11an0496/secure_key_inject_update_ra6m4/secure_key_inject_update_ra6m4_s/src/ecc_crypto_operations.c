/***********************************************************************************************************************
 * File Name    : ecc_crypto_operations.c
 * Description  : Contains the cryptographic operations with ECC
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright [2020-2022] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
 *
 * This software and documentation are supplied by Renesas Electronics America Inc. and may only be used with products
 * of Renesas Electronics Corp. and its affiliates ("Renesas").  No other uses are authorized.  Renesas products are
 * sold pursuant to Renesas terms and conditions of sale.  Purchasers are solely responsible for the selection and use
 * of Renesas products and Renesas assumes no liability.  No license, express or implied, to any intellectual property
 * right is granted by Renesas. This software is protected under all applicable laws, including copyright laws. Renesas
 * reserves the right to change or discontinue this software and/or this documentation. THE SOFTWARE AND DOCUMENTATION
 * IS DELIVERED TO YOU "AS IS," AND RENESAS MAKES NO REPRESENTATIONS OR WARRANTIES, AND TO THE FULLEST EXTENT
 * PERMISSIBLE UNDER APPLICABLE LAW, DISCLAIMS ALL WARRANTIES, WHETHER EXPLICITLY OR IMPLICITLY, INCLUDING WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT, WITH RESPECT TO THE SOFTWARE OR
 * DOCUMENTATION.  RENESAS SHALL HAVE NO LIABILITY ARISING OUT OF ANY SECURITY VULNERABILITY OR BREACH.  TO THE MAXIMUM
 * EXTENT PERMITTED BY LAW, IN NO EVENT WILL RENESAS BE LIABLE TO YOU IN CONNECTION WITH THE SOFTWARE OR DOCUMENTATION
 * (OR ANY PERSON OR ENTITY CLAIMING RIGHTS DERIVED FROM YOU) FOR ANY LOSS, DAMAGES, OR CLAIMS WHATSOEVER, INCLUDING,
 * WITHOUT LIMITATION, ANY DIRECT, CONSEQUENTIAL, SPECIAL, INDIRECT, PUNITIVE, OR INCIDENTAL DAMAGES; ANY LOST PROFITS,
 * OTHER ECONOMIC DAMAGE, PROPERTY DAMAGE, OR PERSONAL INJURY; AND EVEN IF RENESAS HAS BEEN ADVISED OF THE POSSIBILITY
 * OF SUCH LOSS, DAMAGES, CLAIMS OR COSTS.
 **********************************************************************************************************************/
#include <crypto_operations.h>
#include "hal_data.h"
#include "r_sce.h"
#include "flash_storage.h"
#include "new_ecc_public_key.h"

/* user can get the encrypted key size from the SKMT user's manual */

#define IV_SIZE                                     16
#define DATA_TYPE_MSG_HASH                          0           /* data type is message, not hash */

static sce_key_update_key_t   kuk_key;
static sce_ecc_public_wrapped_key_t wrapped_new_ecc_public_key_data_flash;
static sce_ecc_public_wrapped_key_t ecc_public_key_injected;

static uint8_t encrypted_new_ecc_key[NEW_ECC_PUB_SIZE] = {0};
uint8_t iv_encrypt[IV_SIZE] = {0};
sce_ecc_public_wrapped_key_t wrapped_new_ecc_public_key;

static volatile fsp_err_t error = FSP_SUCCESS;

static void init_iv_encrypt_new_ecc_key(void);
static void init_encrypted_new_ecc_key(void);

/**********************************************************************************************
* Two sets of NIST ECC SECP256R1 test vectors are used for testing purpose. Refer to the AN for more details.
***********************************************************************************************/
/* NIST vector: plaintext message used for signature creation for the directly injected ECC public key */
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

/* NIST vector: externally generated NIST signature using the ECC private key which matches the injected ECC public key*/
static uint8_t ECC_SECP256R1ExpectedSignature[] =
{
 0xf3, 0xac, 0x80, 0x61, 0xb5, 0x14, 0x79, 0x5b, 0x88, 0x43, 0xe3, 0xd6, 0x62, 0x95, 0x27, 0xed,
 0x2a, 0xfd, 0x6b, 0x1f, 0x6a, 0x55, 0x5a, 0x7a, 0xca, 0xbb, 0x5e, 0x6f, 0x79, 0xc8, 0xc2, 0xac,
 0x8b, 0xf7, 0x78, 0x19, 0xca, 0x05, 0xa6, 0xb2, 0x78, 0x6c, 0x76, 0x26, 0x2b, 0xf7, 0x37, 0x1c,
 0xef, 0x97, 0xb2, 0x18, 0xe9, 0x6f, 0x17, 0x5a, 0x3c, 0xcd, 0xda, 0x2a, 0xcc, 0x05, 0x89, 0x03
};

/* NIST vector: plaintext message used for signature creation for the ECC public key update */
static uint8_t ECC_SECP256R1Message_new[] = {
0xc3, 0x5e, 0x2f, 0x09, 0x25, 0x53, 0xc5, 0x57, 0x72, 0x92, 0x6b, 0xdb, 0xe8, 0x7c, 0x97, 0x96,
0x82, 0x7d, 0x17, 0x02, 0x4d, 0xbb, 0x92, 0x33, 0xa5, 0x45, 0x36, 0x6e, 0x2e, 0x59, 0x87, 0xdd,
0x34, 0x4d, 0xeb, 0x72, 0xdf, 0x98, 0x71, 0x44, 0xb8, 0xc6, 0xc4, 0x3b, 0xc4, 0x1b, 0x65, 0x4b,
0x94, 0xcc, 0x85, 0x6e, 0x16, 0xb9, 0x6d, 0x7a, 0x82, 0x1c, 0x8e, 0xc0, 0x39, 0xb5, 0x03, 0xe3,
0xd8, 0x67, 0x28, 0xc4, 0x94, 0xa9, 0x67, 0xd8, 0x30, 0x11, 0xa0, 0xe0, 0x90, 0xb5, 0xd5, 0x4c,
0xd4, 0x7f, 0x4e, 0x36, 0x6c, 0x09, 0x12, 0xbc, 0x80, 0x8f, 0xbb, 0x2e, 0xa9, 0x6e, 0xfa, 0xc8,
0x8f, 0xb3, 0xeb, 0xec, 0x93, 0x42, 0x73, 0x8e, 0x22, 0x5f, 0x7c, 0x7c, 0x2b, 0x01, 0x1c, 0xe3,
0x75, 0xb5, 0x66, 0x21, 0xa2, 0x06, 0x42, 0xb4, 0xd3, 0x6e, 0x06, 0x0d, 0xb4, 0x52, 0x4a, 0xf1
};

/* NIST vector: externally generated NIST signature using the ECC private key which matches the ECC public key update*/
static uint8_t ECC_SECP256R1ExpectedSignature_new[] =
{
 0x97, 0x6d, 0x3a, 0x4e, 0x9d, 0x23, 0x32, 0x6d, 0xc0, 0xba, 0xa9, 0xfa, 0x56, 0x0b, 0x7c, 0x4e,
 0x53, 0xf4, 0x28, 0x64, 0xf5, 0x08, 0x48, 0x3a, 0x64, 0x73, 0xb6, 0xa1, 0x10, 0x79, 0xb2, 0xdb,
 0x1b, 0x76, 0x6e, 0x9c, 0xeb, 0x71, 0xba, 0x6c, 0x01, 0xdc, 0xd4, 0x6e, 0x0a, 0xf4, 0x62, 0xcd,
 0x4c, 0xfa, 0x65, 0x2a, 0xe5, 0x01, 0x7d, 0x45, 0x55, 0xb8, 0xee, 0xef, 0xe3, 0x6e, 0x19, 0x32
};


/* access the encrypted new ECC public key generated from SKMT */
void init_encrypted_new_ecc_key(void)
{
    memcpy(encrypted_new_ecc_key, g_NEW_ECC_PUB.encrypted_user_key, NEW_ECC_PUB_SIZE);
}

/* access the initialization vector used for encrypting the new ECC public key */
void init_iv_encrypt_new_ecc_key(void)
{
    memcpy(iv_encrypt, g_NEW_ECC_PUB.initial_vector, IV_SIZE);
}

/*****************************************************************************************
 *  @brief      cryptograhpic_operation_with_directly_injected_ecc_pub_key: this function reads the directly injected
 *              ECC public key from the data flash and perform cryptographic operations with the ECC public key
 *  @retval     bool
 *  **************************************************************************************/
bool cryptograhpic_operation_with_directly_injected_ecc_pub_key(void)
{
    bool result = false;
    sce_ecdsa_byte_data_t local_sig, local_hash;
    uint8_t wrapped_ecc_public_key_size = 0;

    error = R_SCE_Open(&sce_ctrl, &sce_cfg);
    wrapped_ecc_public_key_size = sizeof(ecc_public_key_injected.value);
    memcpy((uint8_t *)(&(ecc_public_key_injected.value)), (uint8_t *)DIRECT_ECC_PUB_KEY_ADDRESS, wrapped_ecc_public_key_size);
    ecc_public_key_injected.type = SCE_KEY_INDEX_TYPE_ECC_P256_PUBLIC;

    local_sig.pdata = ECC_SECP256R1ExpectedSignature;
    local_sig.data_length = sizeof(ECC_SECP256R1ExpectedSignature);
    local_sig.data_type = DATA_TYPE_MSG_HASH;
    local_hash.data_type = DATA_TYPE_MSG_HASH;
    local_hash.data_length = sizeof(ECC_SECP256R1Message);
    local_hash.pdata = (uint8_t *)ECC_SECP256R1Message;

    /* using the injected ECC public key to verify the NIST signature */
    error = R_SCE_ECDSA_secp256r1_SignatureVerify(&local_sig, &local_hash, &ecc_public_key_injected);

    if (FSP_SUCCESS==error)
    {
        result = true;
    }
    else
    {
        result = false;
    }

    error = R_SCE_Close(&sce_ctrl);

    return result;
}

/*****************************************************************************************
 *  @brief      update_ecc_public_key: this function decrypts the key update key encrypted new ECC public key
 *              and generates a new wrapped ECC public key and stores it to the
 *              data flash
 *  @retval     bool
 *  **************************************************************************************/
bool update_ecc_pub_key(void)
{
   bool result = false;

   R_SCE_Open(&sce_ctrl, &sce_cfg);
   init_encrypted_new_ecc_key();
   init_iv_encrypt_new_ecc_key();

   kuk_key.type = SCE_KEY_INDEX_TYPE_UPDATE_KEY_RING;
   memcpy(kuk_key.value, (uint32_t *)(KUK_ADDRESS), HW_SCE_UPDATE_KEY_RING_INDEX_WORD_SIZE*4);

   /*generate a new wrapped ECC public key using a previously injected Key-Update Key */
   error = R_SCE_ECC_secp256r1_EncryptedPublicKeyWrap(iv_encrypt, encrypted_new_ecc_key, &kuk_key, &wrapped_new_ecc_public_key);

   if (FSP_SUCCESS==error)
    {
      result = true;
    }
    else
    {
      return false;
    }

   error = store_new_ecc_pub_key_to_data_flash();
   error = R_SCE_Close(&sce_ctrl);

   return result;
}

/*****************************************************************************************
 *  @brief      cryptographic_operation_with_updated_ecc_pub_key: this function exercises the newly stored ECC public key
 *              for cryptographic operation.
 *  @retval     bool
 *  **************************************************************************************/
bool cryptographic_operation_with_updated_ecc_pub_key(void)
{
    bool result = false;
    sce_ecdsa_byte_data_t local_sig, local_hash;
    uint8_t wrapped_ecc_public_key_size = 0;

    error = R_SCE_Open(&sce_ctrl, &sce_cfg);
    wrapped_ecc_public_key_size = sizeof(ecc_public_key_injected.value);
    memcpy((uint8_t *)(&(wrapped_new_ecc_public_key_data_flash.value)), (uint8_t *)DIRECT_ECC_PUB_KEY_ADDRESS, wrapped_ecc_public_key_size);
    wrapped_new_ecc_public_key_data_flash.type = SCE_KEY_INDEX_TYPE_ECC_P256_PUBLIC;

    local_sig.pdata = ECC_SECP256R1ExpectedSignature_new;
    local_sig.data_length = sizeof(ECC_SECP256R1ExpectedSignature_new);
    local_sig.data_type = DATA_TYPE_MSG_HASH;
    local_hash.data_type = DATA_TYPE_MSG_HASH;
    local_hash.data_length = sizeof(ECC_SECP256R1Message_new);
    local_hash.pdata = (uint8_t *)ECC_SECP256R1Message_new;

    /* using the injected ECC public key to verify the matching NIST signature */
    error = R_SCE_ECDSA_secp256r1_SignatureVerify(&local_sig, &local_hash, &wrapped_new_ecc_public_key_data_flash);

    if (FSP_SUCCESS==error)
    {
       result = true;
    }
    else
    {
       result = false;
    }

    error = R_SCE_Close(&sce_ctrl);

    return result;
}
