#include "hal_data.h"
#include "common_utils.h"
#include "crypto_ep.h"
#include "app_definitions.h"
#include "hw_sce_ra_private.h"

FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER


// HW_SCE_Aes128EcbEncryptInit(sce_aes_handle_t *handle, sce_aes_key_index_t *key_index);
#define BLOCK_SIZE    (128 / 8)
#define CALL_ONLY_INIT            (0)
#define CALL_ONLY_UPDATE_FINAL    (1)
#define NON_SECURE_KEY    (1)
volatile psa_status_t psa_status = PSA_SUCCESS;
#define INITIAL_WRAPPED_KEY    0
#define NEW_WRAPPED_KEY        1

bool verify_cryptographic_operation_with_psa_api(size_t update_process, sce_aes_wrapped_key_t *wrapped_key_128);
void key_inject_check(void);
bool init_lfs(void);

/* two sets of NIsT vectors are used in this example project */
#if 0
COUNT = 0
KEY = 80000000000000000000000000000000
IV = 00000000000000000000000000000000
PLAINTEXT = 00000000000000000000000000000000
CIPHERTEXT = 0edd33d3c621e546455bd8ba1418bec8

COUNT = 1
KEY = c0000000000000000000000000000000
IV = 00000000000000000000000000000000
PLAINTEXT = 00000000000000000000000000000000
CIPHERTEXT = 4bc3f883450c113c64ca42e1112a9e87
#endif

/* for plaintext key injection, these can be random data */
uint8_t Key_Wrap_IV[16] =                                                                         // NOLINT(readability-magic-numbers)
{
    0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa  // NOLINT(readability-magic-numbers)
};

/* wrapped user factory programming key w-ufpk. ignored for plaintext key injection */

uint8_t encrypted_update_provisioning_key[] =
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // NOLINT(readability-magic-numbers)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // NOLINT(readability-magic-numbers)
};


/* Initial plaintext aes128 key to be wrapped and injected */
 uint8_t user_aes128_key[16] =                                                                       // NOLINT(readability-magic-numbers)
  {
      0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  // NOLINT(readability-magic-numbers)
  };

 /* plaintext data to be encrypted by the injected aes128 key */
  uint8_t enc_plain[BLOCK_SIZE] =
  {
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // NOLINT(readability-magic-numbers)
  };

  uint8_t enc_dec_ivec[BLOCK_SIZE] =
  {
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // NOLINT(readability-magic-numbers)
  };

  /* The known cipher based on the user_aes128_key, enc_plain text data and enc_ivec */
   const uint8_t enc_cipher_128[BLOCK_SIZE] =
   {
       0x0E, 0xDD, 0x33, 0xD3, 0xC6, 0x21, 0xE5, 0x46, 0x45, 0x5B, 0xD8, 0xBA, 0x14, 0x18, 0xBE, 0xC8 // NOLINT(readability-magic-numbers)
   };

 /* NIST test vector count 1 */

  uint8_t new_user_aes128_key[16] =                                                                       // NOLINT(readability-magic-numbers)
  {
      0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  // NOLINT(readability-magic-numbers)
  };

  uint8_t new_enc_plain[BLOCK_SIZE] =
   {
       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // NOLINT(readability-magic-numbers)
   };

   const uint8_t new_enc_cipher_128[BLOCK_SIZE] =
   {
       0x4B, 0xC3, 0xF8, 0x83, 0x45, 0x0C, 0x11, 0x3C, 0x64, 0xCA, 0x42, 0xE1, 0x11, 0x2A, 0x9E, 0x87 // NOLINT(readability-magic-numbers)
   };

   uint8_t new_enc_ivec[BLOCK_SIZE] =
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

bool verify_cryptographic_operation_with_psa_api(size_t update_process, sce_aes_wrapped_key_t *wrapped_key_128)
{
     static psa_key_handle_t     key_handle_aes_128        = 0;
    const psa_algorithm_t    alg         = PSA_ALG_CBC_NO_PADDING;
    size_t key_id=5;

    uint8_t  plain[16]  = {0};
    uint8_t  cipher[16] = {0};
    uint8_t  new_cipher[16] = {0};
    size_t output_length;
    psa_cipher_operation_t   operation_aes = PSA_CIPHER_OPERATION_INIT;

    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;

     /* Set key attributes */
     psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
     psa_set_key_algorithm(&attributes, alg);

     psa_set_key_type(&attributes, PSA_KEY_TYPE_AES_WRAPPED);


     psa_set_key_bits(&attributes, 8*sizeof(enc_cipher_128));

     psa_set_key_lifetime(&attributes, PSA_KEY_LIFETIME_PERSISTENT);

     // persistent plain text key can be injected
     /* Set the id to a positive integer. */
     psa_set_key_id(&attributes, (psa_key_id_t)key_id);


     psa_status = psa_import_key(&attributes, (unsigned char *)&(wrapped_key_128->value), 48, &key_handle_aes_128);

    if(FSP_SUCCESS == psa_status)
    {
        psa_status = psa_cipher_encrypt_setup(&operation_aes, key_handle_aes_128, alg);
        if(PSA_SUCCESS==psa_status)
        {
            if(0==update_process)
            {
               if (PSA_SUCCESS == psa_status)
                {

                    psa_status = psa_cipher_set_iv(&operation_aes, enc_dec_ivec, sizeof(enc_dec_ivec));
                    if (PSA_SUCCESS == psa_status)
                    {
                        cipher_operation(&operation_aes, enc_plain, BLOCK_SIZE, 10, cipher, 48,
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
                              psa_status = psa_cipher_set_iv(&operation_aes, enc_dec_ivec, sizeof(enc_dec_ivec));
                              if(PSA_SUCCESS == psa_status)
                              {
                                  psa_status = cipher_operation(&operation_aes, enc_cipher_128, sizeof(enc_cipher_128), 10, plain,
                                                                           48, &output_length);
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
            else    /*this is to verify the updated new AES key */
            {

                 if (PSA_SUCCESS == psa_status)
                 {

                     psa_status = psa_cipher_set_iv(&operation_aes, new_enc_ivec, sizeof(new_enc_ivec));
                     if (PSA_SUCCESS == psa_status)
                     {
                         cipher_operation(&operation_aes, new_enc_plain, BLOCK_SIZE, 10, new_cipher, 48,
                                          &output_length);
                     }


                     if (memcmp(new_enc_cipher_128, new_cipher, BLOCK_SIZE))
                     {
                         /* calculated cipher text does not match the expected cipher */
                         return false;
                     }
                     else  /* encryption is successful, continue to test description */
                     {
                         psa_status = psa_cipher_decrypt_setup(&operation_aes, key_handle_aes_128, alg);

                         if (PSA_SUCCESS == psa_status)
                         {
                            psa_status = psa_cipher_set_iv(&operation_aes, new_enc_ivec, sizeof(new_enc_ivec));
                            if(PSA_SUCCESS == psa_status)
                            {
                                psa_status = cipher_operation(&operation_aes, new_enc_cipher_128, sizeof(new_enc_cipher_128), 10, plain,
                                                                         48, &output_length);
                                if (memcmp(new_enc_plain, plain, BLOCK_SIZE))
                                 {
                                    /* calculated cipher text does not match the expected cipher */
                                    return false;
                                 }
                            }

                         }
                     }

                 }

            }
        }


    /* erase the current key, so a new key with the same id can be injected */
    psa_status = psa_destroy_key(key_handle_aes_128);
    return true;

    }
    else
    {
        return false;
    }
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
    uint8_t    key_type = NON_SECURE_KEY;
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

    /* AES-128 key injection */

    error = R_SCE_AES128_InitialKeyWrap(&key_type, encrypted_update_provisioning_key, Key_Wrap_IV,
                                                      (uint8_t *) user_aes128_key, &wrapped_key_128);

    if(FSP_SUCCESS == error)
    {
        APP_PRINT(INITIAL_KEY_WRAP_SUCCESS);
        crypto_status = verify_cryptographic_operation_with_psa_api(INITIAL_WRAPPED_KEY, &wrapped_key_128);
        if (true == crypto_status)
        {
           APP_PRINT(CRYPTO_OPERATION_WITH_INITIAL_WRAPPED_KEY_SUCCESS);
            /* what to do if it is plaintext key ? */
           error = R_SCE_AES128_InitialKeyWrap(&key_type, encrypted_update_provisioning_key, Key_Wrap_IV,
                                                          (uint8_t *) new_user_aes128_key, &wrapped_key_128);
            /* this operation should fail for plaintext key update as a different key is used.*/
            crypto_status = verify_cryptographic_operation_with_psa_api(NEW_WRAPPED_KEY, &wrapped_key_128);
            if(true == crypto_status)
            {
                APP_PRINT(CRYPTO_OPERATION_WITH_UPDATED_WRAPPED_KEY_SUCCESS);
            }
            else
            {
                APP_PRINT(CRYPTO_OPERATION_WITH_UPDATED_WRAPPED_KEY_FAILED);
            }
       }
        else
        {
            APP_PRINT(CRYPTO_OPERATION_WITH_INITIAL_WRAPPED_KEY_FAILED);
        }
    }
    else
    {
        APP_PRINT(INITIAL_KEY_WRAP_FAILED);
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
