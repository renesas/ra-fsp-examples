/***********************************************************************************************************************
 * File Name    : pqc_common_app.c
 * Description  : Contains data structures and common functions use to demonstrate PQC operations.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "pqc_common_app.h"

/* Private functions declarations */
static uint8_t user_input_get (void);
static psa_status_t pqc_decap_sign_remote_verify_operation (size_t ml_kem_bits, size_t ml_dsa_bits, psa_key_lifetime_t lifetime);

/***********************************************************************************************************************
 *  Function Name: pqc_operation_entry
 *  Description  : This function is used to start PQC example operation.
 *  Arguments    : None.
 *  Return Value : None.
 **********************************************************************************************************************/
void pqc_operation_entry(void)
{
    fsp_pack_version_t  version = {RESET_VALUE};
    fsp_err_t err = FSP_SUCCESS;
    uint8_t user_input = RESET_VALUE;

    psa_status_t psa_status = PSA_SUCCESS;
    mbedtls_platform_context ctx = {RESET_VALUE};
    const psa_key_lifetime_t lifetime = PSA_KEY_LIFETIME_VOLATILE;

    /* Initialize the terminal */
    err = TERM_INIT();
    if (FSP_SUCCESS != err)
    {
        ERROR_TRAP;
    }

    /* Version get API for FSP pack information */
    R_FSP_VersionGet(&version);

    /* Example Project information printed on the terminal */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    APP_PRINT(EP_INFO);

    /* Initialize the USB PCDC */
    err = RM_COMMS_USB_PCDC_Open(&g_comms_usb_pcdc0_ctrl, &g_comms_usb_pcdc0_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("**RM_COMMS_USB_PCDC_Open API failed **\r\n");
        APP_ERR_TRAP(err);
    }

    psa_status = mbedtls_platform_setup(&ctx);
    handle_error(psa_status, "mbedtls_platform_setup failed");

    psa_status = psa_crypto_init();
    handle_error(psa_status, "psa_crypto_init failed");

    if (PSA_KEY_LIFETIME_IS_PERSISTENT(lifetime))
    {
        err = littlefs_init();
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("littlefs_init failed \r\n");
            APP_ERR_TRAP(err);
        }
    }

    APP_PRINT("\r\nPSA Crypto initialization successfully\r\n");

    while (true)
    {
        APP_PRINT(MAIN_MENU);

        while (!APP_CHECK_DATA) {;}

        /* Get user input */
        user_input = user_input_get();

        switch (user_input) {
            case ML_KEM_KEY_EXCHANGE:
            {
                /* ML_KEM: Two-party key exchange */
                psa_status = mlkem_two_party_key_exchange(PSA_KEY_BITS_ML_KEM_512, lifetime);
                handle_error(psa_status, "mlkem_two_party_key_exchange failed\r\n");
            }
                break;

            case ML_KEM_EXPORT_IMPORT:
            {
                /* ML_KEM: Export/Import keypair (Backup & Recovery) */
                psa_status = mlkem_export_import_keypair(PSA_KEY_BITS_ML_KEM_512, lifetime);
                handle_error(psa_status, "mlkem_export_import_keypair failed\r\n");
            }
                break;

            case ML_DSA_SIGN_VERIFY:
            {
                /* ML_DSA Two-party signing and verification */
                psa_status = mldsa_two_party_signing(PSA_KEY_BITS_ML_DSA_65, lifetime);
                handle_error(psa_status, "mldsa_two_party_signing failed\r\n");
            }
                break;

            case ML_DSA_EXPORT_IMPORT:
            {
                /* ML_DSA: Export/Import keypair (Backup & Recovery) */
                psa_status = mldsa_export_import_keypair(PSA_KEY_BITS_ML_DSA_65, lifetime);
                handle_error(psa_status, "mldsa_export_import_keypair failed\r\n");
            }
                break;

            case PQC_REMOTE_VERIFY:
            {

                psa_status = pqc_decap_sign_remote_verify_operation(PSA_KEY_BITS_ML_KEM_512, PSA_KEY_BITS_ML_DSA_44, lifetime);
                handle_error(psa_status, "pqc_decap_sign_remote_verify_operation failed\r\n");
            }
                break;


            default:
                APP_PRINT("\r\nInvalid Input entered\r\n");

                break;
        }
    }

    /* Clean up */
    mbedtls_psa_crypto_free();
    mbedtls_platform_teardown(&ctx);
}
/***********************************************************************************************************************
* End of function pqc_operation_entry.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: user_input_get
 *  Description  : This function is used to get the user input from terminal.
 *  Arguments    : None.
 *  Return Value : user input in integer.
 **********************************************************************************************************************/
static uint8_t user_input_get (void)
{
    uint8_t input_value = RESET_VALUE;
    uint8_t user_input[TERM_BUFFER_SIZE + 1] = {RESET_VALUE};

    /* Clean user input buffer */
    memset(user_input, NULL_CHAR, sizeof(user_input));

    /* Read user input data from terminal */
    TERM_READ(user_input, sizeof(user_input));

    /* Convert to integer value */
    input_value = (uint8_t)atoi((char *) &user_input[0]);

    return input_value;
}
/***********************************************************************************************************************
* End of function user_input_get.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: cleanup_keys
 *  Description  : This function is used to clean entire key list.
 *  Arguments    : handles        Pointer to key list.
 *                 count          Key quantity.
 *  Return Value : PSA_SUCCESS    Upon successful operation.
 *                 Any other error code apart from PSA_SUCCESS.
 **********************************************************************************************************************/
psa_status_t cleanup_keys(psa_key_id_t *handles, size_t count)
{
    psa_status_t status = PSA_SUCCESS;

    for (size_t i = 0; i < count; i++)
    {
        if (handles[i] != 0)
        {
            status = psa_destroy_key(handles[i]);
            APP_ERR_RET(PSA_SUCCESS!= status, status, "**psa_destroy_key API failed**\r\n");
            handles[i] = 0;
        }
    }

    return status;
}
/***********************************************************************************************************************
* End of function cleanup_keys.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: handle_error
 *  Description  : This function handles error if error occurred, closes all opened modules, prints and traps error.
 *  Arguments    : err          error status
 *                 err_str      error string
 *  Return Value : None
 **********************************************************************************************************************/
void handle_error(psa_status_t err, const char *err_str)
{
    if (PSA_SUCCESS != err )
    {
        /* Print error information and error code */
        APP_ERR_PRINT(err_str);

        littlefs_deinit();

        /* Trap the error */
        APP_ERR_TRAP(err);
    }
}
/***********************************************************************************************************************
* End of function handle_error
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: littlefs_init
 *  Description  : This function is used to initialize LittleFS operation.
 *  Arguments    : None.
 *  Return Value : FSP_SUCCESS    Upon successful operation.
 *                 Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
fsp_err_t littlefs_init(void)
{
    fsp_err_t err     = FSP_SUCCESS;
    int       lfs_err = RESET_VALUE;

    /* Open LittleFS SPI Flash port */
    err = RM_LITTLEFS_SPI_FLASH_Open(&g_rm_littlefs0_ctrl, &g_rm_littlefs0_cfg);
    APP_ERR_RET(FSP_SUCCESS != err, err, "** RM_LITTLEFS_SPI_FLASH_Open API FAILED **\r\n");

    /* Format the file system */
    lfs_err = lfs_format(&g_rm_littlefs0_lfs, &g_rm_littlefs0_lfs_cfg);
    APP_ERR_RET(RESET_VALUE != lfs_err, (fsp_err_t)lfs_err, "** lfs_format API FAILED **\r\n");

    /* Mount the file system */
    lfs_err = lfs_mount(&g_rm_littlefs0_lfs, &g_rm_littlefs0_lfs_cfg);
    APP_ERR_RET(RESET_VALUE != lfs_err, (fsp_err_t)lfs_err, "** lfs_mount API FAILED **\r\n");

    return err;
}
/***********************************************************************************************************************
* End of function littlefs_init.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: littlefs_deinit
 *  Description  : This function is used to deinitialize the LittleFS.
 *  Arguments    : None.
 *  Return Value : None.
 **********************************************************************************************************************/
void littlefs_deinit(void)
{
    fsp_err_t   err     = FSP_SUCCESS;

    err = RM_LITTLEFS_SPI_FLASH_Close(&g_rm_littlefs0_ctrl);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** RM_LITTLEFS_SPI_FLASH_Close API FAILED **\r\n");
    }
}
/***********************************************************************************************************************
* End of function littlefs_deinit.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: pqc_decap_sign_remote_verify_operation
 *  Description  : This function is used to demonstrate PSA Crypto API implementation can correctly interoperate with
 *                 an independent external party in a real-world distributed environment.
 *  Arguments    : ml_kem_bits    Key size of ML-KEM in bits.
 *                 ml_dsa_bits    Key size of ML-DSA in bits.
 *                 lifetime       Key lifetime (volatile/persistent).
 *  Return Value : PSA_SUCCESS    Upon successful operation.
 *                 Any other error code apart from PSA_SUCCESS.
 **********************************************************************************************************************/
static psa_status_t pqc_decap_sign_remote_verify_operation (size_t ml_kem_bits, size_t ml_dsa_bits, psa_key_lifetime_t lifetime)
{
    psa_status_t            status                          = PSA_SUCCESS;
    uint8_t                 g_recv_buffer[DATA_LEN_SIZE]    = {RESET_VALUE};

    psa_key_handle_t        kem_key_handle                  = RESET_VALUE;
    psa_key_attributes_t    kem_attributes                  = PSA_KEY_ATTRIBUTES_INIT;

    psa_key_id_t            decap_output_key_handle         = RESET_VALUE;
    psa_key_attributes_t    shared_secret_attributes        = PSA_KEY_ATTRIBUTES_INIT;
    uint8_t                 shared_secret[PSA_ML_KEM_SHARED_SECRET_SIZE];

    uint8_t                 ciphertext[PSA_ML_KEM_CIPHERTEXT_SIZE(PSA_KEY_BITS_ML_KEM_512)];
    size_t                  ciphertext_len                  = PSA_ML_KEM_CIPHERTEXT_SIZE(PSA_KEY_BITS_ML_KEM_512);

    uint8_t                 export_kem_public_key[PSA_KEY_GEN_ML_KEM_PUBLIC_KEY_MAX_SIZE(PSA_KEY_BITS_ML_KEM_512)];
    size_t                  export_kem_public_key_len       = RESET_VALUE;

    APP_PRINT("\r\n+======================================================+\r\n");
    APP_PRINT(    "| ML-KEM Decap & ML-DSA Sign (Remote Verify) operation |\r\n");
    APP_PRINT(    "+======================================================+\r\n\r\n");

    /* Set key attributes */
    setup_mlkem_attrs(&kem_attributes, MLKEM_KEY_KEYPAIR, ml_kem_bits, lifetime);

    status = psa_generate_key(&kem_attributes, &kem_key_handle);
    APP_ERR_RET(PSA_SUCCESS != status, status, "Decapsulator: psa_generate_key (MK-KEM) failed\r\n");

    APP_PRINT("+-- DECAPSULATOR --------------------------------------+\r\n");
    APP_PRINT("|  Generated ML-KEM keypair                            |\r\n");

    /* Export the public key to send to the encapsulator */
    status = psa_export_public_key (kem_key_handle, &export_kem_public_key[0], PSA_KEY_GEN_ML_KEM_PUBLIC_KEY_MAX_SIZE(ml_kem_bits),
                                    &export_kem_public_key_len);
    APP_ERR_RET(PSA_SUCCESS != status, status, "Decapsulator: psa_export_public_key failed\r\n");

    APP_PRINT("|  Exported ML-KEM public key (%lu bytes)              |\r\n", (unsigned long) export_kem_public_key_len);
    APP_PRINT("+------------------------------------------------------+\r\n\r\n");


    APP_PRINT("  Please run pqc_test.py on the Host PC to continue\r\n\r\n");

    /* Wait to receive the ACK from the Host PC */
    while (FSP_SUCCESS != comms_recv_ack())
    {
        __NOP();
    }

    /* Send the public key to the Encapsulator */
    status = (psa_status_t) comms_send_data(export_kem_public_key, export_kem_public_key_len);
    APP_ERR_RET(FSP_SUCCESS != status, status, "Decapsulator: comms_send_data (public key) failed\r\n");

    APP_PRINT("  Decapsulator sends public key to Encapsulator...\r\n\r\n");

    /* Receive the size of the ciphertext */
    status = (psa_status_t) comms_recv_data(g_recv_buffer, DATA_LEN_SIZE);
    APP_ERR_RET(FSP_SUCCESS != status, status, "Decapsulator: comms_recv_data (ciphertext length) failed\r\n");

    uint16_t remote_ciphertext_size = g_recv_buffer[0] * 256 + g_recv_buffer[1];

    /* Receive the ciphertext */
    status = (psa_status_t) comms_recv_data(ciphertext, remote_ciphertext_size);
    APP_ERR_RET(FSP_SUCCESS != status, status, "Decapsulator: Receive the ciphertext failed\r\n");

    APP_PRINT("  Decapsulator receives ciphertext from Encapsulator (%lu bytes) \r\n\r\n", (unsigned long) remote_ciphertext_size);

    /* Set key attributes for shared secret */
    setup_mlkem_attrs(&shared_secret_attributes, MLKEM_KEY_SHARED_SECRET, 0, lifetime);

    /* Decapsulate the ciphertext */
    status = psa_decapsulate(kem_key_handle, PSA_ALG_ML_KEM, &ciphertext[0], ciphertext_len, &shared_secret_attributes,
                            &decap_output_key_handle);
    APP_ERR_RET(PSA_SUCCESS != status, status, "Decapsulator: psa_decapsulate failed\r\n");

    /* Export the shared secret of decapsulator */
    status = psa_export_key(decap_output_key_handle, &shared_secret[0], PSA_ML_KEM_SHARED_SECRET_SIZE, &export_kem_public_key_len);
    APP_ERR_RET(PSA_SUCCESS != status, status, "Decapsulator: psa_export_key (shared secret of decapsulator) failed\r\n");

    /* Send the decapsulated shared secret to the Host PC to compare with the one generated there */
    uint32_t comms_len = sizeof(shared_secret);

    status = (psa_status_t) comms_send_data(shared_secret, comms_len);
    APP_ERR_RET(FSP_SUCCESS != status, status, "Decapsulator: comms_send_data (decapsulator shared secret) failed\r\n");

    APP_PRINT("  Decapsulator sends shared secret to Encapsulator...\r\n\r\n");

    /* Receive the response from the verifier to determine the verification result */
    status = (psa_status_t) comms_read(g_recv_buffer, RESP_CMD_SIZE);
    APP_ERR_RET(FSP_SUCCESS != status, status, "Decapsulator: comms_read (response) failed\r\n");

    if (CMD_ACK != g_recv_buffer[0])
    {
        APP_ERR_PRINT("  [RESULT] The shared secret not match!\r\n");
        return PSA_ERROR_GENERIC_ERROR;
    }

    APP_PRINT("  [RESULT] The shared secret match!\r\n\r\n");

    /* Sign the shared secret */
    psa_key_handle_t        dsa_key_handle                  = RESET_VALUE;
    psa_key_attributes_t    dsa_attributes                  = PSA_KEY_ATTRIBUTES_INIT;

    uint8_t                 export_dsa_public_key[PSA_KEY_GEN_ML_DSA_PUB_KEY_SIZE(PSA_KEY_BITS_ML_DSA_44)];
    size_t                  export_dsa_public_key_len       = RESET_VALUE;

    uint8_t                 sign[PSA_ML_DSA_SIGNATURE_SIZE(PSA_KEY_BITS_ML_DSA_44)];
    size_t                  sign_len                        = RESET_VALUE;

    APP_PRINT("+-- SIGNER --------------------------------------------+\r\n");

    /* Set key attributes */
    setup_mldsa_attrs(&dsa_attributes, MLDSA_KEY_KEYPAIR, ml_dsa_bits, lifetime);

    /* Generate key pair */
    status = psa_generate_key(&dsa_attributes, &dsa_key_handle);
    APP_ERR_RET(PSA_SUCCESS != status, status, "Signer: psa_generate_key (MK-DSA) failed\r\n");

    APP_PRINT("|  Generated ML-DSA keypair                            |\r\n");

    status = psa_sign_message(dsa_key_handle, PSA_ALG_ML_DSA, shared_secret, sizeof(shared_secret), sign, sizeof(sign), &sign_len);
    APP_ERR_RET(PSA_SUCCESS != status, status, "Signer: psa_sign_message failed\r\n");

    APP_PRINT("|  Signer signs shared secret key                      |\r\n");

    /* Export the public key to send to the Host PC (Verifier) */
    status = psa_export_public_key (dsa_key_handle, &export_dsa_public_key[0],
                                    PSA_KEY_GEN_ML_DSA_PUB_KEY_SIZE(PSA_KEY_BITS_ML_DSA_44), &export_dsa_public_key_len);
    APP_ERR_RET(PSA_SUCCESS != status, status, "Signer: psa_export_public_key failed\r\n");

    APP_PRINT("|  Exported ML-DSA public key (%lu bytes)             |\r\n", (unsigned long) export_dsa_public_key_len);
    APP_PRINT("+------------------------------------------------------+\r\n\r\n");

    /* Send the public key to the Host PC (Verifier) */
    status = (psa_status_t) comms_send_data(export_dsa_public_key, export_dsa_public_key_len);
    APP_ERR_RET(FSP_SUCCESS != status, status, "Signer: comms_send_data failed\r\n");

    APP_PRINT("  Signer sends public key to verifier...\r\n\r\n");

    /* Send the signature to the Host PC (Verifier) */
    status = (psa_status_t) comms_send_data(sign, sign_len);
    APP_ERR_RET(FSP_SUCCESS != status, status, "Signer: comms_send_data (signature) failed\r\n");

    APP_PRINT("  Signer sends signature to verifier...\r\n\r\n");

    /* Receive the response from the Host PC (Verifier) to determine the verification result */
    status = (psa_status_t) comms_read(g_recv_buffer, RESP_CMD_SIZE);
    APP_ERR_RET(FSP_SUCCESS != status, status, "Signer: comms_read (response) failed\r\n");

    if (CMD_ACK != g_recv_buffer[0])
    {
        APP_ERR_PRINT("  [RESULT] Signature verified failed!\r\n");
        return PSA_ERROR_GENERIC_ERROR;
    }

    APP_PRINT("  [RESULT] Signature verified successfully!\r\n");

    return status;
}
/***********************************************************************************************************************
* End of function pqc_decap_sign_remote_verify_operation.
***********************************************************************************************************************/
