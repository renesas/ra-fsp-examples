/***********************************************************************************************************************
 * File Name    : aes_functions.c
 * Description  : contains aes key generation, encryption, decryption services
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
#include "hal_data.h"
#include "aes_functions.h"
#include "hw_sce_private.h"
#include "hw_sce_aes_private.h"

const psa_algorithm_t    alg         = PSA_ALG_CBC_PKCS7;
psa_key_handle_t     key_handle        = 0;
static size_t iv_len = 0;

volatile psa_status_t psa_status = PSA_SUCCESS;
volatile fsp_err_t status = FSP_SUCCESS;

psa_cipher_operation_t   operation_aes = PSA_CIPHER_OPERATION_INIT;

enum
    {
        block_size = PSA_BLOCK_CIPHER_BLOCK_SIZE(PSA_KEY_TYPE_AES), ///< NOLINT(readability-uppercase-literal-suffix)
        part_size  = 10,
    };

static uint8_t              iv[block_size]    = {0};

mbedtls_platform_context ctx         = {0};

/*****************************************************************************************
 *  @brief      init_lfs: format and mount the LittleFS system for key management
 *  @retval     bool
 *  **************************************************************************************/
bool init_lfs(void)
{
    /* - To use persistent keys:
      * -    The file system must be initialized prior to calling the generate/import key functions.
      */
     volatile fsp_err_t lfs_status = FSP_SUCCESS;
     lfs_status = RM_LITTLEFS_FLASH_Open(&g_rm_littlefs0_ctrl, &g_rm_littlefs0_cfg);

     if(FSP_SUCCESS == lfs_status)
     {
         lfs_status = lfs_format(&g_rm_littlefs0_lfs, &g_rm_littlefs0_lfs_cfg);
         if(FSP_SUCCESS == lfs_status)
         {
             lfs_status = lfs_mount(&g_rm_littlefs0_lfs, &g_rm_littlefs0_lfs_cfg);
         }

     }
     if (FSP_SUCCESS == lfs_status)
     {
         return true;
     }
     else
     {
         return false;
     }
}

/*****************************************************************************************
 *  @brief      create_aes_key: this function takes user input for key_bits, key_id,
 *              whether to generate wrapped AES key directly using SCE or install plaintext
 *              AES key as wrapped key based on key type definitions in the secure region.
 *  @retval     bool
 *  **************************************************************************************/
bool create_aes_key(aes_creation_args_t *args)
{
    aes_creation_args_t *const p_args_checked = cmse_check_pointed_object(args, CMSE_AU_NONSECURE);
    FSP_ASSERT (p_args_checked == args );
    /* Create storage to hold the generated OEM key index. Size = Largest key size possible. */
     uint8_t encrypted_aes_key[SIZE_AES_256BIT_KEYLEN_BYTES_WRAPPED] = {0};
     const unsigned char * p_internal_key = encrypted_aes_key;

    uint8_t  random[32]      = {0};
   psa_key_lifetime_t lifetime = 0;
   psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
   psa_status =  mbedtls_platform_setup(&ctx);

   if(PSA_SUCCESS == psa_status)
   {
       psa_status = psa_crypto_init();
       if(PSA_SUCCESS == psa_status)
       {
           /* Set key attributes */
             psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
             psa_set_key_algorithm(&attributes, alg);
             psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
             psa_set_key_bits(&attributes, p_args_checked->key_bit);


             // persistent plain text key can be installed
             lifetime = PSA_KEY_LIFETIME_PERSISTENT_WRAPPED;

             psa_set_key_lifetime(&attributes, lifetime);

             if (PSA_KEY_LIFETIME_IS_PERSISTENT(lifetime))
             {
                 /* Set the id to a positive integer. */
                 psa_set_key_id(&attributes, (psa_key_id_t)p_args_checked->key_id);
             }
             if(PSA_SUCCESS == psa_status)
             {
                /* use TRNG to generate plaintext AES key */
                psa_status = psa_generate_random(random, p_args_checked->key_bit/8);
                if (PSA_SUCCESS == psa_status)
                {
                   /* call HW_SCE procedure to convert to wrapped AES key */
                    status = (int) HW_SCE_GenerateOemKeyIndexPrivate(SCE_OEM_KEY_TYPE_PLAIN,
                                                             SCE_OEM_CMD_AES256,
                                                             NULL,
                                                             random,
                                                             NULL,
                                                             (uint32_t *) p_internal_key);
                    /* clear the plaintext key */
                    memset(random, 0, p_args_checked->key_bit/8);
                    if (FSP_SUCCESS == status)
                    {

                        /* install the wrapped AES key */
                       psa_status = psa_import_key(&attributes, p_internal_key, 52, &key_handle);
                    }
                }
             }
          }
       }

   if(PSA_SUCCESS != psa_status  || FSP_SUCCESS != status)
       return false;
    else
        return true;
}

/*****************************************************************************************
 *  @brief      cipher_operation: handle AES block cipher encryption decryption
 *  @retval     bool
 *  **************************************************************************************/
static psa_status_t cipher_operation (psa_cipher_operation_t * operation,
                                      const uint8_t          * c_input,
                                      size_t                   c_input_size,
                                      size_t                   c_part_size,
                                      uint8_t                * output,
                                      size_t                   output_size,
                                      size_t                 * output_len)
{

    size_t       bytes_to_write = 0;
    size_t       bytes_written  = 0;
    size_t       len            = 0;

    *output_len = 0;
    while (bytes_written != c_input_size)
    {
        bytes_to_write = (c_input_size - bytes_written >c_part_size ?
                          c_part_size :
                          c_input_size - bytes_written);

        psa_status = psa_cipher_update(operation,
                                   c_input + bytes_written,
                                   bytes_to_write,
                                   output + *output_len,
                                   output_size - *output_len,
                                   &len);
        if (PSA_SUCCESS != psa_status)
        {
            return psa_status;
        }

        bytes_written += bytes_to_write;
        *output_len   += len;
    }

    psa_status = psa_cipher_finish(operation, output + *output_len, output_size - *output_len, &len);
    if (PSA_SUCCESS != psa_status)
    {
        return psa_status;
    }

    *output_len += len;

    return psa_status;
}
/*****************************************************************************************
 *  @brief      encryption_operation: encrypt user data
 *              non_secure_encryption_args_t *args:
 *              input: data and size to be encrypted
 *              output: encrypted result and length
 *  @retval     bool
 *  **************************************************************************************/
bool encryption_operation(non_secure_encryption_args_t *args)
{
    /* range check to make sure entire buffer is in Non-secure region */
        /* Verify all pointers are in non-secure memory. */
        non_secure_encryption_args_t *const p_args_checked = cmse_check_pointed_object(args, CMSE_AU_NONSECURE);
        FSP_ASSERT (p_args_checked == args );

        uint8_t * c_input_checked = cmse_check_address_range ((uint8_t *)p_args_checked->c_input, p_args_checked->input_size, CMSE_AU_NONSECURE);
        FSP_ASSERT (c_input_checked == p_args_checked->c_input);

        uint8_t *output_checked = cmse_check_address_range (p_args_checked->output, p_args_checked->output_size, CMSE_AU_NONSECURE);
        FSP_ASSERT (output_checked == p_args_checked->output);

    psa_status = psa_cipher_encrypt_setup(&operation_aes, key_handle, alg);
    if (PSA_SUCCESS == psa_status)
    {
        psa_status = psa_cipher_generate_iv(&operation_aes, iv, sizeof(iv), &iv_len);
        if (PSA_SUCCESS == psa_status)
        {
            cipher_operation(&operation_aes, c_input_checked, p_args_checked->input_size, part_size, output_checked, p_args_checked->output_size,
                             p_args_checked->output_len);
        }
    }

    if(PSA_SUCCESS != psa_status)
           return false;
       else
           return true;
}

/*****************************************************************************************
 *  @brief      decrypt encrypted data
 *              non_secure_decryption_args_t *args:
 *              input: encrypted data to be decrypted
 *              output: decrypted result
 *  @retval     bool
 *  **************************************************************************************/
bool decryption_operation(non_secure_decryption_args_t *args)
{
    non_secure_decryption_args_t *const p_args_checked = cmse_check_pointed_object(args, CMSE_AU_NONSECURE);
    FSP_ASSERT (p_args_checked == args );

    uint8_t * c_input_checked = cmse_check_address_range ((uint8_t *)p_args_checked->c_input, p_args_checked->input_size2, CMSE_AU_NONSECURE);
    FSP_ASSERT (c_input_checked == p_args_checked->c_input);

    uint8_t *output_checked = cmse_check_address_range (p_args_checked->output, p_args_checked->output_size, CMSE_AU_NONSECURE);
       FSP_ASSERT (output_checked == p_args_checked->output);


    psa_status = psa_cipher_decrypt_setup(&operation_aes, key_handle, alg);
    if (PSA_SUCCESS == psa_status)
    {
        psa_status = psa_cipher_set_iv(&operation_aes, iv, sizeof(iv));
        if(PSA_SUCCESS == psa_status)
        {
            psa_status = cipher_operation(&operation_aes, c_input_checked, p_args_checked->input_size2, part_size, output_checked,
                                          p_args_checked->output_size, p_args_checked->output_len);
            if(PSA_SUCCESS == psa_status)
            {
                psa_status = psa_cipher_abort(&operation_aes);
            }
        }
    }


    if(PSA_SUCCESS != psa_status)
        return false;
    else
        return true;

}

/*****************************************************************************************
 *  @brief      destroy_the_key: the key is removed and SCE shutdown
 *  @retval     bool
 *  **************************************************************************************/
bool destroy_the_key(void)
{
    psa_status = psa_destroy_key(key_handle);

    if(PSA_SUCCESS == psa_status)
    {
        /* Close the SCE */
        mbedtls_platform_teardown(&ctx);
    }
    if(PSA_SUCCESS != psa_status)
        return false;
    else
        return true;

}

