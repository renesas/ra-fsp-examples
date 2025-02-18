/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "hal_data.h"
#include "common_utils.h"
#include "crypto_ep.h"
#include "app_definitions.h"
#include "hw_sce_ra_private.h"
#include "initial_aes_128.h"

FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER


// HW_SCE_Aes128EcbEncryptInit(sce_aes_handle_t *handle, sce_aes_key_index_t *key_index);
#define BLOCK_SIZE    (128 / 8)
#define CALL_ONLY_INIT            (0)
#define CALL_ONLY_UPDATE_FINAL    (1)
#define SECURE_KEY    (0)
volatile psa_status_t psa_status = PSA_SUCCESS;


bool verify_cryptographic_operation_with_psa_api(sce_aes_wrapped_key_t *wrapped_key_128);
void key_inject_check(void);
bool init_lfs(void);



/* this set is used as the initial aes key test vector */
//COUNT  2
//KEY  e0000000000000000000000000000000
//IV  00000000000000000000000000000000
//PLAINTEXT  00000000000000000000000000000000
//CIPHERTEXT  72a1da770f5d7ac4c9ef94d822affd97



/* plaintext data to be encrypted */

  uint8_t enc_plain[BLOCK_SIZE] =
  {
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // NOLINT(readability-magic-numbers)
  };


  const uint8_t enc_cipher_128[BLOCK_SIZE] =
    {
      0x72, 0xa1, 0xda, 0x77, 0x0f, 0x5d, 0x7a, 0xc4, 0xc9, 0xef, 0x94, 0xd8, 0x22, 0xaf, 0xfd, 0x97
    };


  uint8_t enc_ivec[BLOCK_SIZE] =
  {
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // NOLINT(readability-magic-numbers)
  };



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

bool verify_cryptographic_operation_with_psa_api(sce_aes_wrapped_key_t *wrapped_key_128)
{
     static psa_key_handle_t     key_handle_aes_128        = 0;
    const psa_algorithm_t    alg         = PSA_ALG_CBC_NO_PADDING;
    size_t key_id=5;
    uint8_t  plain[16]  = {0};
    uint8_t  cipher[16] = {0};
    size_t output_length;
    size_t output_size = PSA_CIPHER_UPDATE_OUTPUT_SIZE(PSA_KEY_TYPE_AES_WRAPPED, alg, 10);

    psa_cipher_operation_t   operation_aes = PSA_CIPHER_OPERATION_INIT;


    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;

     /* Set key attributes */
     psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
     psa_set_key_algorithm(&attributes, alg);

     psa_set_key_type(&attributes, PSA_KEY_TYPE_AES_WRAPPED);


     psa_set_key_bits(&attributes, 8*sizeof(g_AES128.encrypted_user_key));

     psa_set_key_lifetime(&attributes, PSA_KEY_LIFETIME_PERSISTENT);

     // persistent plain text key can be installed
     /* Set the id to a positive integer. */
     psa_set_key_id(&attributes, (psa_key_id_t)key_id);


     psa_status = psa_import_key(&attributes, (unsigned char *)&(wrapped_key_128->value),
                                 SIZE_AES_128BIT_KEYLEN_BYTES_WRAPPED, &key_handle_aes_128);

    if(FSP_SUCCESS == psa_status)
    {

        psa_status = psa_cipher_encrypt_setup(&operation_aes, key_handle_aes_128, alg);
        if (PSA_SUCCESS == psa_status)
        {

            psa_status = psa_cipher_set_iv(&operation_aes, enc_ivec, sizeof(enc_ivec));
            if (PSA_SUCCESS == psa_status)
            {
                cipher_operation(&operation_aes, enc_plain, BLOCK_SIZE, 10, cipher, output_size,
                                 &output_length);
            }

            if (memcmp(enc_cipher_128, cipher, BLOCK_SIZE))
            {
                /* calculated cipher text does not match the expected cipher */
                return false;
            }
            else
            {
                psa_status = psa_cipher_decrypt_setup(&operation_aes, key_handle_aes_128, alg);

               if (PSA_SUCCESS == psa_status)
               {
                  psa_status = psa_cipher_set_iv(&operation_aes, enc_ivec, sizeof(enc_ivec));
                  if(PSA_SUCCESS == psa_status)
                  {
                      psa_status = cipher_operation(&operation_aes, enc_cipher_128, sizeof(enc_cipher_128), 10, plain,
                                                    output_size, &output_length);
                      if (memcmp(enc_plain, plain, BLOCK_SIZE))
                       {
                          /* calculated cipher text does not match the expected cipher */
                          return false;
                       }
                  }

               }
            }
        }

    }
    else
    {
        return false;
    }

    /* erase the current key, so a new key with the same id can be injected */
    psa_status = psa_destroy_key(key_handle_aes_128);
    return true;

 }
/* platform context structure.*/
mbedtls_platform_context ctx = {RESET_VALUE};
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
void key_inject_check(void)
{
    sce_aes_wrapped_key_t wrapped_key_128;
    uint8_t    key_type = SECURE_KEY;
    fsp_err_t error = FSP_SUCCESS;
    bool crypto_status = false;


    /*******************************************************************************************************************//**
     * This API generates 128-bit AES key within the user routine.
     *
     * @param[in]     key_type                              Selection key type when generating wrapped key
     *                                                      (0: for encrypted key, 1: for plain key)
     * @param[in]     wrapped_user_factory_programming_key  Wrapped user factory programming key by the Renesas Key Wrap Service.
     *                                                      When key_type is 1 as plain key, this is not required and
     *                                                      any value can be specified.
     * @param[in]     initial_vector                        Initialization vector when generating encrypted_key.
     *                                                      When key_type is 1 as plain key, this is not required and
     *                                                      any value can be specified.
     * @param[in]     encrypted_key                         Encrypted user key and MAC appended
     * @param[in,out] wrapped_key                           128-bit AES wrapped key
     *
     * @retval FSP_SUCCESS                          Normal termination.
     * @return If an error occurs, the return value will be as follows.
     *         * FSP_ERR_CRYPTO_SCE_FAIL Internal I/O buffer is not empty.
     *         * FSP_ERR_CRYPTO_SCE_RESOURCE_CONFLICT A resource conflict occurred because a hardware resource needed.
     *
     * @note The pre-run state is SCE Enabled State.
     *       After the function runs the state transitions to SCE Enabled State.
     **********************************************************************************************************************/

    /* AES-128 key wrap */

    error = R_SCE_AES128_InitialKeyWrap(&key_type, (uint8_t *)g_AES128.wufpk, (uint8_t *)g_AES128.initial_vector,
                                                      (uint8_t *) g_AES128.encrypted_user_key, &wrapped_key_128);

    if(FSP_SUCCESS == error)
    {
        APP_PRINT(KEY_WRAP_SUCCESS);
        crypto_status = verify_cryptographic_operation_with_psa_api(&wrapped_key_128);
        if (true == crypto_status)
        {
            APP_PRINT(CRYPTO_OPERATION_WITH_WRAPPED_KEY_SUCCESS);
        }
        else
        {
            APP_PRINT(CRYPTO_OPERATION_WITH_WRAPPED_KEY_FAILED);
        }
    }
    else
    {
        APP_PRINT(KEY_WRAP_FAILED);
    }
}

/*******************************************************************************************************************//**
 *  @brief       De-Initialize the Littlefs.
 *  @param[IN]   None
 *  @retval      None
 **********************************************************************************************************************/
void deinit_littlefs(void)
{
    fsp_err_t          err                       = FSP_SUCCESS;
    /*Closes the lower level driver */
    err = RM_LITTLEFS_FLASH_Close(&g_rm_littlefs0_ctrl);
    /* Handle error */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** RM_LITTLEFS_FLASH_Close API FAILED ** \r\n");
    }
}

/*******************************************************************************************************************//**
 * main() is generated by the RA Configuration editor and is used to generate threads if an RTOS is used.  This function
 * is called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
    fsp_pack_version_t version                   = {RESET_VALUE};
    psa_status_t       status                    = (psa_status_t)RESET_VALUE;
    int                mbed_ret_val              = RESET_VALUE;

    /* version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    init_lfs();
    /* Setup the platform; initialize the SCE */
    mbed_ret_val = mbedtls_platform_setup(&ctx);
    if (RESET_VALUE != mbed_ret_val)
    {
        APP_ERR_PRINT("\r\n** mbedtls_platform_setup API FAILED ** \r\n");
        APP_ERR_TRAP(mbed_ret_val);
    }

    /* Initialize crypto library.*/
    status = psa_crypto_init();
    if (PSA_SUCCESS != status)
    {
        APP_ERR_PRINT("\r\n** psa_crypto_init API FAILED ** \r\n");
        /* De-initialize the platform.*/
        mbedtls_platform_teardown(&ctx);
        APP_ERR_TRAP(status);
    }

    key_inject_check();

    deinit_littlefs();
    mbedtls_psa_crypto_free();
    /* De-initialize the platform.*/
    mbedtls_platform_teardown(&ctx);
    while(1);

#if BSP_TZ_SECURE_BUILD
    /* Enter non-secure code */
    R_BSP_NonSecureEnter();
#endif
}

/*******************************************************************************************************************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart(bsp_warm_start_event_t event)
{
    if (BSP_WARM_START_RESET == event)
    {
#if BSP_FEATURE_FLASH_LP_VERSION != 0

        /* Enable reading from data flash. */
        R_FACI_LP->DFLCTL = 1U;

        /* Would normally have to wait tDSTOP(6us) for data flash recovery. Placing the enable here, before clock and
         * C runtime initialization, should negate the need for a delay since the initialization will typically take more than 6us. */
#endif
    }

    if (BSP_WARM_START_POST_C == event)
    {
        /* C runtime environment and system clocks are setup. */

        /* Configure pins. */
        R_IOPORT_Open (&g_ioport_ctrl, g_ioport.p_cfg);
    }
}

#if BSP_TZ_SECURE_BUILD

BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ();

/* Trustzone Secure Projects require at least one nonsecure callable function in order to build (Remove this if it is not required to build). */
BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ()
{

}
#endif
