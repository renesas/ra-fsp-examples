/***********************************************************************************************************************
 * File Name    : crypto_operations.c
 * Description  : Contains the non-secure callable cryptographic operation
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright [2020-2021] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
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


#include "crypto_operations.h"
#include "hal_data.h"
#include "r_sce.h"
#include "flash_storage.h"

/* users need to update the KUK_ADDRESS and DIRECT_KEY_ADDRESS based on where they are stored */
#define KUK_ADDRESS             0x40000
#define DIRECT_KEY_ADDRESS      FLASH_HP_DF_BLOCK_1

#define BLOCK    16
#define ENCRYPTED_NEW_AES_256_KEY_SIZE  48
#define AES_CBC_IV_SIZE                 16


static volatile fsp_err_t error = FSP_SUCCESS;

extern uint8_t iv_encrypt_new_key[AES_CBC_IV_SIZE];
extern uint8_t encrypted_new_key[ENCRYPTED_NEW_AES_256_KEY_SIZE];

sce_aes_wrapped_key_t wrapped_new_user_key;

/*Initialization vector for CBC, this is overwritten by random number from TRNG prior to be used */
uint32_t enc_ivec[4] =
{
    0x00000000, 0x00000000, 0x00000000, 0x000000000
};

/*
 * this is the plaintext test data for the algorithm test
 */

static uint8_t plain[BLOCK * 2] =
{
    0x52, 0x65, 0x6e, 0x65, 0x73, 0x61, 0x73, 0x20, 0x45, 0x6c, 0x65, 0x63, 0x74, 0x72, 0x6f, 0x6e,
    0x69, 0x63, 0x73, 0x20, 0x43, 0x6f, 0x72, 0x70, 0x6f, 0x72, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x00
};

static sce_aes_wrapped_key_t installed_key;
static sce_key_update_key_t   kuk_key;

static sce_aes_wrapped_key_t wrapped_new_user_key_data_flash;

static sce_aes_handle_t      handle;
static uint8_t               cipher_calculated[32] = {0};
static uint8_t               plain_calculated[32]  = {0};
static uint32_t              dummy;

/*****************************************************************************************
 *  @brief      enc_dec_with_directly_installed_key: this function reads the directly installed
 *              user key from the code flash and perform cryptographic operations with the user key
 *  @retval     bool
 *  **************************************************************************************/
bool enc_dec_with_directly_installed_key(void)
{
    R_SCE_Open(&sce_ctrl, &sce_cfg);

    memcpy(installed_key.value, (uint32_t *)DIRECT_KEY_ADDRESS,HW_SCE_AES256_KEY_INDEX_WORD_SIZE*4);

    installed_key.type = SCE_KEY_INDEX_TYPE_AES256;

    /* Encrypt a plain text with CBC */
    error = R_SCE_RandomNumberGenerate(enc_ivec);
    error = R_SCE_AES256CBC_EncryptInit(&handle, &installed_key, (uint8_t *)enc_ivec);
    error = R_SCE_AES256CBC_EncryptUpdate(&handle, plain, cipher_calculated, BLOCK * 2);
    error = R_SCE_AES256CBC_EncryptFinal(&handle, cipher_calculated, &dummy);

    /* Decrypt a cipher text using same key as Encryption */
    error = R_SCE_AES256CBC_DecryptInit(&handle, &installed_key, (uint8_t *)enc_ivec);
    error = R_SCE_AES256CBC_DecryptUpdate(&handle, cipher_calculated, plain_calculated, BLOCK * 2);
    error = R_SCE_AES256CBC_DecryptFinal(&handle, plain_calculated, &dummy);

    /* SCE power off */
    error = R_SCE_Close(&sce_ctrl);
    /* Compare plain and plain_calculated */
    if (memcmp(plain, plain_calculated, BLOCK * 2))
    {
    return false;
    }
    else
    {
    return true;
    }

}

/*****************************************************************************************
 *  @brief      update_user_key: this function decrypts the key update key encrypted new user key
 *              with the key update key, generates a new wrapped user key and stores it to the
 *              data flash
 *  @retval     bool
 *  **************************************************************************************/

bool update_user_key(void)
{

    R_SCE_Open(&sce_ctrl, &sce_cfg);

    kuk_key.type = SCE_KEY_INDEX_TYPE_UPDATE_KEY_RING;
    memcpy(kuk_key.value, (uint32_t *)(KUK_ADDRESS),HW_SCE_UPDATE_KEY_RING_INDEX_WORD_SIZE*4);

    /* operations for secure key update */
    error = R_SCE_AES256_EncryptedKeyWrap(iv_encrypt_new_key, encrypted_new_key, &kuk_key, &wrapped_new_user_key);
    error = store_new_user_key_to_data_flash();
    error = R_SCE_Close(&sce_ctrl);

    if (FSP_SUCCESS==error)
        return true;
    else
        return false;

}

/*****************************************************************************************
 *  @brief      enc_dec_with_updated_user_key: this function exercises the newly stored user key
 *              for cryptographic operation.
 *  @retval     bool
 *  **************************************************************************************/
bool enc_dec_with_updated_user_key(void)
{

    R_SCE_Open(&sce_ctrl, &sce_cfg);

    wrapped_new_user_key_data_flash.type = SCE_KEY_INDEX_TYPE_AES256;
    memcpy(wrapped_new_user_key_data_flash.value, (uint32_t *)FLASH_HP_DF_BLOCK_1, HW_SCE_AES256_KEY_INDEX_WORD_SIZE*4);

    error = R_SCE_AES256ECB_EncryptInit(&handle, &wrapped_new_user_key_data_flash);

    /* Encrypt a plain text */
    error = R_SCE_AES256ECB_EncryptUpdate(&handle, plain, cipher_calculated, BLOCK * 2);
    error = R_SCE_AES256ECB_EncryptFinal(&handle, cipher_calculated, &dummy);


    error = R_SCE_AES256ECB_DecryptInit(&handle, &wrapped_new_user_key_data_flash);
    error = R_SCE_AES256ECB_DecryptUpdate(&handle, cipher_calculated, plain_calculated, BLOCK * 2);
    error = R_SCE_AES256ECB_DecryptFinal(&handle, plain_calculated, &dummy);

    /* SCE power off */
    error = R_SCE_Close(&sce_ctrl);
    /* Compare plain and plain_calculated */
    if (memcmp(plain, plain_calculated, BLOCK * 2))
    {
        return false;
    }
    else
    {
        return true;
    }


}


