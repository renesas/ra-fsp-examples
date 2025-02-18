/***********************************************************************************************************************
 * File Name    : hwWrappedKey.c
 * Description  : provide ECC key pair generation and signing service.
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

#include "hwWrappedKey.h"
#include "common_util.h"
#include "cryptoTypes.h"
#include "hal_data.h"

#define HASH_SIZE               32
#define ECC_SIG_SIZE            64
#define ECC_PSA_KEY_SKIP        1               /* skip the PSA ECC key padding to get the key */

static mbedtls_platform_context ctx = {0};

/*****************************************************************************************************************
 *  @brief      hwSignData
 *  @param[in]  hash_buffer: hash to be signed by the private key
 *  @param[in]  sig_buffer:  signature to send to the PC program
 *  @param[in]  key_handle:  ECC key handle
 *  @retval     status:      true if signing operation is successful
 *  ****************************************************************************************************************/

bool hwSignData(uint8_t * hash_buffer, uint8_t sig_buffer[], psa_key_handle_t *key_handle)
{
    volatile psa_status_t status = PSA_SUCCESS;
    size_t signature_length = 0;

    /* need to get the size of sig_buffer */
    status = psa_sign_hash(*key_handle, PSA_ALG_ECDSA(PSA_ALG_SHA_256), hash_buffer, HASH_SIZE, sig_buffer,
                           ECC_SIG_SIZE, &signature_length);

    if(PSA_SUCCESS != status )
    {

        status = false;
    }
    else
    {
        status = true;
    }

    mbedtls_psa_crypto_free();
    mbedtls_platform_teardown(&ctx);

    return (bool)status;
}


/*****************************************************************************************************************
 *  @brief      hwCreatekey
 *  @param[in]  pubkey_buffer: the buffer which holds the public key sending to the PC
 *  @param[in]  key_handle:  ECC key handle
 *  @retval     status:      true if signing operation is successful
 *  ****************************************************************************************************************/

bool hwCreatekey(uint8_t * pubkey_buffer, psa_key_handle_t *key_handle )
{
    volatile fsp_err_t status = FSP_SUCCESS;
    volatile psa_status_t psa_status = PSA_SUCCESS;
    size_t publickey_size = 0;

    status = mbedtls_platform_setup(&ctx);
    if(FSP_SUCCESS == status)
    {
        psa_status = psa_crypto_init();
        if(PSA_SUCCESS == status)
        {
           psa_key_attributes_t persistent_attributes = PSA_KEY_ATTRIBUTES_INIT;

           /* Set key pair attributes */
           psa_set_key_usage_flags(&persistent_attributes, PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_VERIFY_HASH);

           psa_set_key_algorithm(&persistent_attributes, PSA_ALG_ECDSA(PSA_ALG_SHA_256));
           psa_set_key_type(&persistent_attributes,PSA_KEY_TYPE_ECC_KEY_PAIR_WRAPPED(PSA_ECC_FAMILY_SECP_R1));
           psa_set_key_bits(&persistent_attributes, ECC_256_PRIVATE_KEY_LENGTH_BITS);

           psa_set_key_lifetime(&persistent_attributes, PSA_KEY_LIFETIME_PERSISTENT);
          
           psa_set_key_id(&persistent_attributes, (psa_key_id_t) 5);
           /*
            * Open LittleFS Flash port and mount the flash little file system before generating the key.
            * This operation is needed based on FSP 1.0.0 cryptographic persistent key storage method
            */
           status = RM_LITTLEFS_FLASH_Open(&g_rm_littlefs0_ctrl, &g_rm_littlefs0_cfg);
           if(FSP_SUCCESS == status)
           {
               status = lfs_format(&g_rm_littlefs0_lfs, &g_rm_littlefs0_lfs_cfg);
               if(FSP_SUCCESS == status)
               {
                   status = lfs_mount(&g_rm_littlefs0_lfs, &g_rm_littlefs0_lfs_cfg);
                   if (FSP_SUCCESS == status)
                   {
                       psa_status = psa_generate_key(&persistent_attributes, key_handle);
                       if(PSA_SUCCESS == psa_status)
                       {
                           psa_status = psa_export_public_key(*key_handle, pubkey_buffer - ECC_PSA_KEY_SKIP, ECC_PUBLIC_KEY_SIZE + ECC_PSA_KEY_SKIP, &publickey_size);
                       }
                   }
               }
           }
        }
    }


   if(FSP_SUCCESS == status || PSA_SUCCESS == psa_status)
   {
       return true;
   }
   else
       return false;

}

