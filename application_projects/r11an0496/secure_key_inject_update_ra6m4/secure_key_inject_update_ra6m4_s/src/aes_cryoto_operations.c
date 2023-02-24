/***********************************************************************************************************************
 * File Name    : aes_crypto_operations.c
 * Description  : Contains the cryptographic operations with AES
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
#include "new_aes_key.h"

#define BLOCK    16
/* user can reference the Security Key Management Tool to find the key size */
#define ENCRYPTED_NEW_AES_256_KEY_SIZE  48
#define AES_CBC_IV_SIZE                 16


static volatile fsp_err_t error = FSP_SUCCESS;
static void init_encrypted_new_aes_key(void);
static void init_iv_encrypt_new_aes_key(void);

static uint8_t iv_encrypt_new_key[AES_CBC_IV_SIZE] = {0};
static uint8_t encrypted_new_key[ENCRYPTED_NEW_AES_256_KEY_SIZE] = {0};

sce_aes_wrapped_key_t wrapped_new_aes_key;

/**********************************************************************************************
* Two sets of NIST AES test vectors are used for testing purpose. Refer to the AN for more details.
***********************************************************************************************/
/* NIST vector plain text message used for both directly injected AES key and updated AES key*/
static uint8_t plain_text[BLOCK] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* NIST vector Initialization Vector*/
static uint8_t iv[BLOCK] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* NIST vector AES-256 key directly injected */
#if 0
static uint8_t aes_256_key_injected_plaintext[32] = {
   0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
#endif

/* NIST vector new AES-256 key update */
#if 0
static uint8_t new_aes_256_key_plaintext[32] = {
   0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
#endif

/* NIST cipher created externally using aes_256_key_injected_directly and plain_text*/
static uint8_t cipher_expected[BLOCK] = {
   0xe3, 0x5a, 0x6d, 0xcb, 0x19, 0xb2, 0x01, 0xa0, 0x1e, 0xbc, 0xfa, 0x8a, 0xa2, 0x2b, 0x57, 0x59
};

/* NIST cipher created externally using new_aes_256_key_plaintext and plain_text*/
static uint8_t cipher_expected_new[BLOCK] = {
  0xb2, 0x91, 0x69, 0xcd, 0xcf, 0x2d, 0x83, 0xe8, 0x38, 0x12, 0x5a, 0x12, 0xee, 0x6a, 0xa4, 0x00
};

static sce_aes_wrapped_key_t injected_key;
static sce_key_update_key_t   kuk_key;
static sce_aes_wrapped_key_t wrapped_new_user_key_data_flash;
static sce_aes_handle_t      handle;
static uint8_t               cipher_calculated[BLOCK] = {0};
static uint32_t              dummy;

/* access the encrypted new AES key generated from SKMT */
void init_encrypted_new_aes_key(void)
{
    memcpy(encrypted_new_key, g_NEW_AES256.encrypted_user_key, ENCRYPTED_NEW_AES_256_KEY_SIZE);
}

/* access the initialization vector used for encrypting the new AES key */
void init_iv_encrypt_new_aes_key(void)
{
    memcpy(iv_encrypt_new_key, g_NEW_AES256.initial_vector, AES_CBC_IV_SIZE);
}

/*****************************************************************************************
 *  @brief      cipher_operation_with_directly_injected_aes_key: this function reads the directly injected
 *              AES key from the data flash and perform cryptographic operations with the AES key
 *  @retval     bool
 *  **************************************************************************************/
bool cipher_operation_with_directly_injected_aes_key(void)
{
    R_SCE_Open(&sce_ctrl, &sce_cfg);

    memcpy(injected_key.value, (uint32_t *)DIRECT_AES_KEY_ADDRESS, HW_SCE_AES256_KEY_INDEX_WORD_SIZE*4);

    injected_key.type = SCE_KEY_INDEX_TYPE_AES256;

    error = R_SCE_AES256CBC_EncryptInit(&handle, &injected_key, iv);
    error = R_SCE_AES256CBC_EncryptUpdate(&handle, plain_text, cipher_calculated, BLOCK);
    error = R_SCE_AES256CBC_EncryptFinal(&handle, cipher_calculated, &dummy);

    /* SCE power off */
    error = R_SCE_Close(&sce_ctrl);

   if (memcmp(cipher_expected, cipher_calculated, BLOCK))
   {
        /* calculated cipher text does not match the expected cipher */
        return false;
   }
   else
   {
       /* calculated cipher text match the expected cipher text */
       return true;
   }
}

/*****************************************************************************************
 *  @brief      update_aes_key: this function decrypts the key update key encrypted new AES key
 *              with the key update key, generates a new wrapped AES key and stores it to the
 *              data flash
 *  @retval     bool
 *  **************************************************************************************/

bool update_aes_key(void)
{

    R_SCE_Open(&sce_ctrl, &sce_cfg);
    init_encrypted_new_aes_key();
    init_iv_encrypt_new_aes_key();

    kuk_key.type = SCE_KEY_INDEX_TYPE_UPDATE_KEY_RING;
    memcpy(kuk_key.value, (uint32_t *)(KUK_ADDRESS), HW_SCE_UPDATE_KEY_RING_INDEX_WORD_SIZE*4);

    /*generate a new wrapped AES key using a previously injected Key-Update Key */
    error = R_SCE_AES256_EncryptedKeyWrap(iv_encrypt_new_key, encrypted_new_key, &kuk_key, &wrapped_new_aes_key);
    error = store_new_aes_key_to_data_flash();
    error = R_SCE_Close(&sce_ctrl);

    if (FSP_SUCCESS==error)
        return true;
    else
        return false;

}

/*****************************************************************************************
 *  @brief      cipher_operation_with_updated_aes_key: this function exercises the newly stored AES key
 *              for cryptographic operation.
 *  @retval     bool
 *  **************************************************************************************/
bool cipher_operation_with_updated_aes_key(void)
{
    R_SCE_Open(&sce_ctrl, &sce_cfg);

    wrapped_new_user_key_data_flash.type = SCE_KEY_INDEX_TYPE_AES256;
    memcpy(wrapped_new_user_key_data_flash.value, (uint32_t *)DIRECT_AES_KEY_ADDRESS, HW_SCE_AES256_KEY_INDEX_WORD_SIZE*4);

    error = R_SCE_AES256CBC_EncryptInit(&handle, &wrapped_new_user_key_data_flash, iv);
    error = R_SCE_AES256CBC_EncryptUpdate(&handle, plain_text, cipher_calculated, BLOCK);
    error = R_SCE_AES256CBC_EncryptFinal(&handle, cipher_calculated, &dummy);

    /* SCE power off */
    error = R_SCE_Close(&sce_ctrl);
    /* Compare plain and plain_calculated */
    if (memcmp(cipher_expected_new, cipher_calculated, BLOCK))
    {
        return false;
    }
    else
    {
        return true;
    }

}


