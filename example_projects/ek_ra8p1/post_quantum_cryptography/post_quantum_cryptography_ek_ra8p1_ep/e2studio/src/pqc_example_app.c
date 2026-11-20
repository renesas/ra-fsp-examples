/***********************************************************************************************************************
 * File Name    : pqc_example_app.c
 * Description  : Example application layer for the Post Quantum Cryptography EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: pqc_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pqc_ep.h"
#include "pqc_mldsa.h"
#include "pqc_mlkem.h"
#include "pqc_example_app.h"
#include "usb_pcdc_comms.h"
#include "console/uart_console.h"

/* Private functions */
static void mlkem_two_party_key_exchange(size_t bits, psa_key_lifetime_t lifetime);
static void mlkem_export_import_keypair(size_t bits, psa_key_lifetime_t lifetime);
static void mldsa_two_party_signing(size_t bits, psa_key_lifetime_t lifetime);
static void mldsa_export_import_keypair(size_t bits, psa_key_lifetime_t lifetime);
static void pqc_decap_sign_remote_verify_operation(size_t ml_kem_bits, size_t ml_dsa_bits, psa_key_lifetime_t lifetime);
static void user_input(char * buf, int len);
static void handle_error(int32_t err, const char * err_str);

/***********************************************************************************************************************
 * Function Name: pqc_example_app_run.
 * Description  : Single entry-point for the Post Quantum Cryptography example application.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
void pqc_example_app_run(void)
{
    fsp_pack_version_t version = {RESET_VALUE};
    fsp_err_t err = FSP_SUCCESS;
    uint8_t user_option = RESET_VALUE;
    char input_buff[BUFFER_SIZE] = {RESET_VALUE};
    psa_status_t psa_status = PSA_SUCCESS;
    mbedtls_platform_context ctx = {RESET_VALUE};
    const psa_key_lifetime_t lifetime = PSA_KEY_LIFETIME_VOLATILE;

    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        return;     /* UART failed to open — cannot print anything */
    }

    /* Get FSP version information */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the terminal */
    printf(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    printf(EP_INFO);
    uart_console_flush();

    /* Initialize the USB PCDC */
    err = comms_init();
    handle_error((int32_t)err, "comms_init failed\r\n");

    psa_status = mbedtls_platform_setup(&ctx);
    handle_error((int32_t)psa_status, "mbedtls_platform_setup failed\r\n");

    psa_status = psa_crypto_init();
    handle_error((int32_t)psa_status, "psa_crypto_init failed\r\n");

    if (PSA_KEY_LIFETIME_IS_PERSISTENT(lifetime))
    {
        err = littlefs_init();
        handle_error((int32_t)err, "littlefs_init failed\r\n");
    }

    printf("\r\nPSA Crypto initialized successfully\r\n");

    while (true)
    {
        printf(EP_MENU);

        while (!uart_console_has_char())
        {
            /* Do nothing */
        }

        /* Get the user option */
        memset(&input_buff[0], NULL_CHAR, BUFFER_SIZE);
        user_input(input_buff, (int)sizeof(input_buff));
        user_option = (uint8_t)atoi(input_buff);

        switch (user_option)
        {
            case ML_KEM_KEY_EXCHANGE:
            {
                /* ML-KEM: Two-party key exchange */
                mlkem_two_party_key_exchange(PSA_KEY_BITS_ML_KEM_512, lifetime);
                break;
            }

            case ML_KEM_EXPORT_IMPORT:
            {
                /* ML-KEM: Export/import keypair (Backup & recovery) */
                mlkem_export_import_keypair(PSA_KEY_BITS_ML_KEM_512, lifetime);
                break;
            }

            case ML_DSA_SIGN_VERIFY:
            {
                /* ML-DSA: Two-party signing and verification */
                mldsa_two_party_signing(PSA_KEY_BITS_ML_DSA_65, lifetime);
                break;
            }

            case ML_DSA_EXPORT_IMPORT:
            {
                /* ML-DSA: Export/import keypair (Backup & recovery) */
                mldsa_export_import_keypair(PSA_KEY_BITS_ML_DSA_65, lifetime);
                break;
            }

            case PQC_REMOTE_VERIFY:
            {
                /* ML-KEM decap & ML-DSA sign (Remote verify) */
                pqc_decap_sign_remote_verify_operation(PSA_KEY_BITS_ML_KEM_512, PSA_KEY_BITS_ML_DSA_44, lifetime);
                break;
            }

            default:
            {
                printf("\r\nInvalid input!\r\n");
                break;
            }
        }
    }

    /* Clean up */
    mbedtls_psa_crypto_free();
    mbedtls_platform_teardown(&ctx);
}
/***********************************************************************************************************************
 * End of function pqc_example_app_run.
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: mlkem_two_party_key_exchange.
 * Description  : This function demonstrates two-party key exchange.
 * Arguments    : bits                      Key size in bits.
 *                lifetime                  Key lifetime (volatile/persistent).
 * Return Value : None.
 **********************************************************************************************************************/
static void mlkem_two_party_key_exchange(size_t bits, psa_key_lifetime_t lifetime)
{
    psa_status_t status = PSA_SUCCESS;

    /* Decapsulator's keys */
    psa_key_handle_t decapsulator_keypair = RESET_VALUE;
    psa_key_id_t decapsulator_ss_handle = RESET_VALUE;

    /* Encapsulator's keys */
    psa_key_handle_t encapsulator_public_key = RESET_VALUE;
    psa_key_id_t encapsulator_ss_handle = RESET_VALUE;

    /* Attributes */
    psa_key_attributes_t attrs_keypair = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_attributes_t attrs_public = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_attributes_t attrs_shared_secret = PSA_KEY_ATTRIBUTES_INIT;

    /* Network transfer buffers */
    uint8_t public_key_to_send[PSA_KEY_GEN_ML_KEM_PUBLIC_KEY_MAX_SIZE(PSA_KEY_BITS_ML_KEM_512)] BSP_ALIGN_VARIABLE(4);
    size_t public_key_len = RESET_VALUE;
    uint8_t ciphertext_to_send[PSA_ML_KEM_CIPHERTEXT_SIZE(PSA_KEY_BITS_ML_KEM_512)] BSP_ALIGN_VARIABLE(4);
    size_t ciphertext_len = RESET_VALUE;

    printf("\r\n+==========================================================+");
    printf("\r\n| Decapsulator-Encapsulator key exchange operation         |");
    printf("\r\n+==========================================================+\r\n");

    /* DECAPSULATOR SIDE - Key generation */
    printf("\r\n+-- DECAPSULATOR (Receiver) -------------------------------+");

    setup_mlkem_attrs(&attrs_keypair, MLKEM_KEY_KEYPAIR, bits, lifetime);

    status = psa_generate_key(&attrs_keypair, &decapsulator_keypair);
    handle_error((int32_t)status, "Decapsulator: psa_generate_key failed\r\n");

    printf("\r\n| [PSA] Generated ML-KEM keypair                           |");

    /* Export public key to send to encapsulator */
    status = psa_export_public_key(decapsulator_keypair, public_key_to_send,
                                   sizeof(public_key_to_send), &public_key_len);
    handle_error((int32_t)status, "Decapsulator: psa_export_public_key failed\r\n");

    printf("\r\n| [PSA] Exported public key (%lu bytes)                    |", (unsigned long)public_key_len);
    printf("\r\n+----------------------------------------------------------+\r\n");

    /* Transfer simulation */
    printf("\r\n  [SIM] Decapsulator sends public key to Encapsulator...\r\n");

    /* ENCAPSULATOR SIDE - Encapsulation */
    printf("\r\n+-- ENCAPSULATOR (Sender) ---------------------------------+");

    /* Encapsulator imports decapsulator's public key */
    setup_mlkem_attrs(&attrs_public, MLKEM_KEY_PUBLIC, bits, lifetime);

    status = psa_import_key(&attrs_public, public_key_to_send,
                            public_key_len, &encapsulator_public_key);
    handle_error((int32_t)status, "Encapsulator: psa_import_key (public key) failed\r\n");

    printf("\r\n| [SIM] Encapsulator received the public key               |");
    printf("\r\n|       (Encapsulator has NO private key!)                 |");
    printf("\r\n| [PSA] Encapsulator imported the public key               |");

    /* Encapsulator encapsulates using ONLY public key */
    setup_mlkem_attrs(&attrs_shared_secret, MLKEM_KEY_SHARED_SECRET, 0, lifetime);

    status = psa_encapsulate(encapsulator_public_key,
                             PSA_ALG_ML_KEM,
                             &attrs_shared_secret,
                             &encapsulator_ss_handle,
                             ciphertext_to_send,
                             sizeof(ciphertext_to_send),
                             &ciphertext_len);
    handle_error((int32_t)status, "Encapsulator: psa_encapsulate failed\r\n");

    printf("\r\n| [PSA] Encapsulated (ciphertext %lu bytes)                |", (unsigned long)ciphertext_len);
    printf("\r\n| [PSA] Generated shared secret (Encapsulator side)        |");
    printf("\r\n+----------------------------------------------------------+\r\n");

    /* Transfer simulation */
    printf("\r\n  [SIM] Encapsulator sends ciphertext to Decapsulator...\r\n");

    /* DECAPSULATOR SIDE - Decapsulation */
    printf("\r\n+-- DECAPSULATOR (Receiver) -------------------------------+");

    /* Decapsulator decapsulates using PRIVATE KEY */
    status = psa_decapsulate(decapsulator_keypair,
                             PSA_ALG_ML_KEM,
                             ciphertext_to_send,
                             ciphertext_len,
                             &attrs_shared_secret,
                             &decapsulator_ss_handle);
    handle_error((int32_t)status, "Decapsulator: psa_decapsulate failed\r\n");

    printf("\r\n| [PSA] Decapsulated ciphertext                            |");
    printf("\r\n| [PSA] Generated shared secret (Decapsulator side)        |");
    printf("\r\n+----------------------------------------------------------+\r\n");

    /* VERIFICATION */
    printf("\r\n+-- VERIFICATION ------------------------------------------+");

    status = compare_shared_secrets(decapsulator_ss_handle, encapsulator_ss_handle);
    handle_error((int32_t)status, "compare_shared_secrets failed: Shared secrets mismatch!\r\n");

    printf("\r\n| Decapsulator and Encapsulator have same shared secrets   |");
    printf("\r\n| Key exchange successful!                                 |");
    printf("\r\n+----------------------------------------------------------+\r\n");

    /* Cleanup */
    psa_key_id_t cleanup_list[] =
    {
        decapsulator_keypair, encapsulator_public_key,
        decapsulator_ss_handle, encapsulator_ss_handle
    };

    status = cleanup_keys(cleanup_list, sizeof(cleanup_list) / sizeof(cleanup_list[0]));
    handle_error((int32_t)status, "cleanup_keys failed\r\n");

    psa_reset_key_attributes(&attrs_keypair);
    psa_reset_key_attributes(&attrs_public);
    psa_reset_key_attributes(&attrs_shared_secret);
}
/***********************************************************************************************************************
 * End of function mlkem_two_party_key_exchange.
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: mlkem_export_import_keypair.
 * Description  : This function demonstrates two-party key exchange with backup keypair.
 * Arguments    : bits                      Key size in bits.
 *                lifetime                  Key lifetime (volatile/persistent).
 * Return Value : None.
 **********************************************************************************************************************/
static void mlkem_export_import_keypair(size_t bits, psa_key_lifetime_t lifetime)
{
    psa_status_t status = PSA_SUCCESS;

    /* Decapsulator's keys (before and after destroy keypair) */
    psa_key_handle_t decapsulator_keypair_original = RESET_VALUE;
    psa_key_handle_t decapsulator_keypair_restored = RESET_VALUE;

    /* Encapsulator's key (only has public key) */
    psa_key_handle_t encapsulator_public_key = RESET_VALUE;

    /* Shared secrets */
    psa_key_id_t decapsulator_ss_original = RESET_VALUE;    /* Before destroy keypair */
    psa_key_id_t encapsulator_ss = RESET_VALUE;             /* Encapsulator side */
    psa_key_id_t decapsulator_ss_restored = RESET_VALUE;    /* After destroy keypair */

    /* Attributes */
    psa_key_attributes_t attrs_keypair = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_attributes_t attrs_public = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_attributes_t attrs_shared_secret = PSA_KEY_ATTRIBUTES_INIT;

    /* Backup keypair */
    uint8_t keypair_backup[PSA_KEY_GEN_ML_KEM_KEY_PAIR_MAX_SIZE(PSA_KEY_BITS_ML_KEM_512)] BSP_ALIGN_VARIABLE(4);
    size_t keypair_backup_len = RESET_VALUE;

    /* Public key to share with encapsulators */
    uint8_t public_key_bytes[PSA_KEY_GEN_ML_KEM_PUBLIC_KEY_MAX_SIZE(PSA_KEY_BITS_ML_KEM_512)] BSP_ALIGN_VARIABLE(4);
    size_t public_key_len = RESET_VALUE;

    /* Ciphertext from encapsulator (stored for reuse after destroy keypair) */
    uint8_t ciphertext_from_encapsulator[PSA_ML_KEM_CIPHERTEXT_SIZE(PSA_KEY_BITS_ML_KEM_512)] BSP_ALIGN_VARIABLE(4);
    size_t ciphertext_len = RESET_VALUE;

    printf("\r\n+==========================================================+");
    printf("\r\n|         ML-KEM export & import keypair operation         |");
    printf("\r\n+==========================================================+\r\n");

    /* PHASE 1: Initial setup */
    printf("\r\n+==========================================================+");
    printf("\r\n| PHASE 1: Initialize device setup                         |");
    printf("\r\n+==========================================================+\r\n");

    /* STEP 1: Decapsulator generates keypair */
    printf("\r\n+-- STEP 1: Decapsulator generates keypair ----------------+");

    setup_mlkem_attrs(&attrs_keypair, MLKEM_KEY_KEYPAIR, bits, lifetime);

    status = psa_generate_key(&attrs_keypair, &decapsulator_keypair_original);
    handle_error((int32_t)status, "psa_generate_key failed\r\n");

    printf("\r\n| [PSA] Decapsulator generated ML-KEM keypair              |");
    printf("\r\n|       (Private key + Public key)                         |");
    printf("\r\n+----------------------------------------------------------+\r\n");

    /* STEP 2: Decapsulator backs up keypair */
    printf("\r\n+-- STEP 2: Backup keypair --------------------------------+");

    status = psa_export_key(decapsulator_keypair_original, keypair_backup,
                            sizeof(keypair_backup), &keypair_backup_len);
    handle_error((int32_t)status, "psa_export_key failed\r\n");

    printf("\r\n| [PSA] Exported keypair                                   |");
    printf("\r\n|       Size: %lu bytes                                   |", (unsigned long)sizeof(keypair_backup));
    printf("\r\n+----------------------------------------------------------+\r\n");

    /* STEP 3: Decapsulator shares public key with Encapsulator */
    printf("\r\n+-- STEP 3: Share public key with Encapsulator-------------+");

    status = psa_export_public_key(decapsulator_keypair_original, public_key_bytes,
                                   sizeof(public_key_bytes), &public_key_len);
    handle_error((int32_t)status, "psa_export_public_key failed\r\n");

    printf("\r\n| [PSA] Exported public key                                |");
    printf("\r\n|       Size: %lu bytes                                    |", (unsigned long)public_key_len);
    printf("\r\n+----------------------------------------------------------+\r\n");

    printf("\r\n  [SIM] Decapsulator sends public key to Encapsulator...\r\n");

    /* STEP 4: Encapsulator imports public key */
    printf("\r\n+-- STEP 4: Encapsulator imports public key ---------------+");

    setup_mlkem_attrs(&attrs_public, MLKEM_KEY_PUBLIC, bits, lifetime);

    status = psa_import_key(&attrs_public, public_key_bytes,
                            public_key_len, &encapsulator_public_key);
    handle_error((int32_t)status, "psa_import_key (public) failed\r\n");

    printf("\r\n| [SIM] Encapsulator received public key                   |");
    printf("\r\n| [PSA] Encapsulator imported public key                   |");
    printf("\r\n|       (Encapsulator has no private key!)                 |");
    printf("\r\n+----------------------------------------------------------+\r\n");

    /* STEP 5: Encapsulator sends encrypted data to decapsulator */
    printf("\r\n+-- STEP 5: Encapsulator sends encrypted data -------------+");

    setup_mlkem_attrs(&attrs_shared_secret, MLKEM_KEY_SHARED_SECRET, 0, lifetime);

    /* Encapsulator encapsulates using ONLY public key (no private key!) */
    status = psa_encapsulate(encapsulator_public_key, PSA_ALG_ML_KEM,
                             &attrs_shared_secret, &encapsulator_ss,
                             ciphertext_from_encapsulator, sizeof(ciphertext_from_encapsulator),
                             &ciphertext_len);
    handle_error((int32_t)status, "psa_encapsulate failed\r\n");

    printf("\r\n| [PSA] Encapsulator encapsulated with public key          |");
    printf("\r\n|       Ciphertext: %lu bytes                              |", (unsigned long)ciphertext_len);
    printf("\r\n| [PSA] Encapsulator derived shared secret                 |");
    printf("\r\n+----------------------------------------------------------+\r\n");

    printf("\r\n  [SIM] Encapsulator sends ciphertext to Decapsulator...\r\n");

    /* STEP 6: Decapsulator decapsulates data */
    printf("\r\n+-- STEP 6: Decapsulator decapsulates data ----------------+\r\n");

    /* Decapsulator uses private key (in keypair) to decapsulate */
    status = psa_decapsulate(decapsulator_keypair_original, PSA_ALG_ML_KEM,
                             ciphertext_from_encapsulator, ciphertext_len,
                             &attrs_shared_secret, &decapsulator_ss_original);
    handle_error((int32_t)status, "psa_decapsulate failed\r\n");

    printf("\r\n| [SIM] Decapsulator received ciphertext                   |");
    printf("\r\n| [PSA] Decapsulator decapsulated successfully             |");
    printf("\r\n| [PSA] Decapsulator derived shared secret                 |");
    printf("\r\n|       (Using private key from keypair)                   |");
    printf("\r\n+----------------------------------------------------------+\r\n");

    /* PHASE 2: Destroy key */
    printf("\r\n+==========================================================+");
    printf("\r\n| PHASE 2: Destroy key                                     |");
    printf("\r\n+==========================================================+\r\n");

    status = psa_destroy_key(decapsulator_keypair_original);
    handle_error((int32_t)status, "psa_destroy_key failed\r\n");
    decapsulator_keypair_original = RESET_VALUE;

    printf("\r\n  [PSA] Destroy the keypair of Decapsulator successfully");

    status = psa_destroy_key(encapsulator_public_key);
    handle_error((int32_t)status, "psa_destroy_key failed\r\n");
    encapsulator_public_key = RESET_VALUE;

    printf("\r\n  [PSA] Destroy the public key of Encapsulator successfully\r\n");

    /* PHASE 3: Recover after destroy key */
    printf("\r\n+==========================================================+");
    printf("\r\n| PHASE 3: Recover the key                                 |");
    printf("\r\n+==========================================================+\r\n");

    /* STEP 7: Decapsulator restores keypair */
    printf("\r\n+-- STEP 7: Restore keypair -------------------------------+");

    /* Re-setup keypair attributes for import */
    setup_mlkem_attrs(&attrs_keypair, MLKEM_KEY_KEYPAIR, bits, lifetime);
    psa_set_key_usage_flags(&attrs_keypair, PSA_KEY_USAGE_DECAPSULATE | PSA_KEY_USAGE_EXPORT);

    /* Decapsulator imports keypair from flash backup */
    status = psa_import_key(&attrs_keypair, keypair_backup,
                            sizeof(keypair_backup), &decapsulator_keypair_restored);
    handle_error((int32_t)status, "psa_import_key failed\r\n");

    printf("\r\n| [SIM] Read keypair from backup                           |");
    printf("\r\n| [PSA] Imported keypair                                   |");
    printf("\r\n|       Decapsulator restored its identity!                |");
    printf("\r\n+----------------------------------------------------------+\r\n");

    /* STEP 8: Decapsulator decapsulates old ciphertext */
    printf("\r\n+-- STEP 8: Decapsulate old ciphertext --------------------+");

    /* Decapsulate using the restored keypair */
    status = psa_decapsulate(decapsulator_keypair_restored, PSA_ALG_ML_KEM,
                             ciphertext_from_encapsulator, ciphertext_len,
                             &attrs_shared_secret, &decapsulator_ss_restored);
    handle_error((int32_t)status, "psa_decapsulate failed\r\n");

    printf("\r\n| [PSA] Decapsulated with restored keypair                 |");
    printf("\r\n| [PSA] Derived shared secret (after reboot)               |");
    printf("\r\n+----------------------------------------------------------+\r\n");

    /* STEP 9: Verify shared secrets match */
    printf("\r\n+-- STEP 9: Verify consistency ----------------------------+");

    /* Compare: original decapsulator Shared secrets vs restored decapsulator Shared secrets
     * They should be identical if backup/restore works correctly */
    status = compare_shared_secrets(decapsulator_ss_original, decapsulator_ss_restored);
    handle_error((int32_t)status, "compare_shared_secrets failed: Shared secrets mismatch after reboot!\r\n");

    /* Also verify against encapsulator's shared secret */
    status = compare_shared_secrets(encapsulator_ss, decapsulator_ss_restored);
    handle_error((int32_t)status,
                 "compare_shared_secrets failed: Decapsulator/Encapsulator shared secrets mismatch!\r\n");

    printf("\r\n| [PSA] Decapsulator SS (original) == (restored)           |");
    printf("\r\n| [PSA] Decapsulator SS (restored) == Encapsulator SS      |");
    printf("\r\n|       Key recovery successful!                           |");
    printf("\r\n|                                                          |");
    printf("\r\n| Verification complete:                                   |");
    printf("\r\n| - Restored keypair is identical                          |");
    printf("\r\n| - Can decrypt old messages from clients                  |");
    printf("\r\n+----------------------------------------------------------+\r\n");

    /* Cleanup */
    psa_key_id_t cleanup_list[] =
    {
        decapsulator_keypair_restored,
        decapsulator_ss_original,
        encapsulator_ss,
        decapsulator_ss_restored
    };

    status = cleanup_keys(cleanup_list, sizeof(cleanup_list) / sizeof(cleanup_list[0]));
    handle_error((int32_t)status, "cleanup_keys failed\r\n");

    psa_reset_key_attributes(&attrs_keypair);
    psa_reset_key_attributes(&attrs_public);
    psa_reset_key_attributes(&attrs_shared_secret);
}
/***********************************************************************************************************************
 * End of function mlkem_export_import_keypair.
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: mldsa_two_party_signing.
 * Description  : This function demonstrates two-party digital signature.
 * Arguments    : bits                      Key size in bits.
 *                lifetime                  Key lifetime (volatile/persistent).
 * Return Value : None.
 **********************************************************************************************************************/
static void mldsa_two_party_signing(size_t bits, psa_key_lifetime_t lifetime)
{
    psa_status_t status = PSA_SUCCESS;

    /* Signer's keys (signer) */
    psa_key_handle_t signer_keypair = RESET_VALUE;

    /* Verifier's key (verifier - only has public key) */
    psa_key_handle_t verifier_public_key = RESET_VALUE;

    /* Attributes */
    psa_key_attributes_t attrs_keypair = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_attributes_t attrs_public = PSA_KEY_ATTRIBUTES_INIT;

    /* Public key to share */
    uint8_t public_key_to_send[PSA_KEY_GEN_ML_DSA_PUB_KEY_SIZE(PSA_KEY_BITS_ML_DSA_65)] BSP_ALIGN_VARIABLE(4);
    size_t public_key_len = RESET_VALUE;

    /* Message to sign */
    const uint8_t message[] = "Firmware version 2.1.0 - Build 20260119";
    const size_t message_len = sizeof(message) - 1; /* Exclude null terminator */

    /* Signature buffer */
    uint8_t signature[PSA_ML_DSA_SIGNATURE_SIZE(PSA_KEY_BITS_ML_DSA_65)] BSP_ALIGN_VARIABLE(4);
    size_t signature_len = RESET_VALUE;

    printf("\r\n+==========================================================+");
    printf("\r\n|             Signing & Verification operation             |");
    printf("\r\n+==========================================================+\r\n");

    /* SIGNER SIDE - Key generation */
    printf("\r\n+-- SIGNER (Signer) ---------------------------------------+");

    setup_mldsa_attrs(&attrs_keypair, MLDSA_KEY_KEYPAIR, bits, lifetime);

    /* Generate key pair */
    status = psa_generate_key(&attrs_keypair, &signer_keypair);
    handle_error((int32_t)status, "Signer: psa_generate_key failed\r\n");
    printf("\r\n| [PSA] Generated ML-DSA keypair                           |");

    /* Export public key to send to verifiers */
    status = psa_export_public_key(signer_keypair, public_key_to_send,
                                   sizeof(public_key_to_send),
                                   &public_key_len);
    handle_error((int32_t)status, "Signer: psa_export_public_key failed\r\n");

    printf("\r\n| [PSA] Exported public key (%lu bytes)                   |", (unsigned long)public_key_len);
    printf("\r\n+----------------------------------------------------------+\r\n");

    /* Transfer simulation */
    printf("\r\n  [SIM] Signer publishes public key...\r\n");

    /* VERIFIER SIDE - Import public key */
    printf("\r\n+-- VERIFIER (Verifier) -----------------------------------+");

    /* Verifier imports signer's public key */
    setup_mldsa_attrs(&attrs_public, MLDSA_KEY_PUBLIC, bits, lifetime);

    status = psa_import_key(&attrs_public, public_key_to_send,
                            public_key_len, &verifier_public_key);
    handle_error((int32_t)status, "Verifier: psa_import_key (public key) failed\r\n");

    printf("\r\n| [SIM] Verifier received the public key                   |");
    printf("\r\n|       (Verifier has no private key!)                     |");
    printf("\r\n| [PSA] Verifier imported the public key                   |");
    printf("\r\n+----------------------------------------------------------+\r\n");

    /* SIGNER SIDE - Sign message */
    printf("\r\n+-- SIGNER (Signs message) --------------------------------+");
    printf("\r\n| Message to sign:                                         |");
    printf("\r\n| \"%s\"                |", message);
    printf("\r\n|                                                          |");

    /* Signer signs the message using private key */
    status = psa_sign_message(signer_keypair, PSA_ALG_ML_DSA, message, message_len, signature, sizeof(signature),
                              &signature_len);
    handle_error((int32_t)status, "Signer: psa_sign_message failed\r\n");

    printf("\r\n| [PSA] Signed message with private key                    |");
    printf("\r\n|       Signature size: %lu bytes                         |", (unsigned long)signature_len);
    printf("\r\n+----------------------------------------------------------+\r\n");

    /* Transfer simulation */
    printf("\r\n  [SIM] Signer releases message + signature...\r\n");

    /* VERIFIER SIDE - Verify signature */
    printf("\r\n+-- VERIFIER (Verifies message) ---------------------------+");

    printf("\r\n| [SIM] Received message:                                  |");
    printf("\r\n| \"%s\"                |", message);
    printf("\r\n| [SIM] Received signature: %lu bytes                     |", (unsigned long)signature_len);
    printf("\r\n|                                                          |");

    /* Verifier verifies using ONLY public key (no private key needed!) */
    status = psa_verify_message(verifier_public_key, PSA_ALG_ML_DSA, message, message_len, signature, signature_len);
    handle_error((int32_t)status, "Verifier: psa_verify_message failed\r\n");

    printf("\r\n| [PSA] Signature verified successfully!                   |");
    printf("\r\n|       (Using public key only)                            |");
    printf("\r\n|                                                          |");
    printf("\r\n| Verification result:                                     |");
    printf("\r\n| - Message is authentic                                   |");
    printf("\r\n| - Message came from signer                               |");
    printf("\r\n| - Message was not tampered                               |");
    printf("\r\n+----------------------------------------------------------+\r\n");

    /* TEST: Verify fails with modified firmware */
    printf("\r\n+-- TEST: Tampered firmware detection ---------------------+");

    const uint8_t tampered_message[] = "Firmware version 2.1.0 - Build 20260120"; /* Changed date */
    const size_t tampered_len = sizeof(tampered_message) - 1;

    printf("\r\n| [SIM] Attacker modifies firmware:                        |");
    printf("\r\n| \"%s\"                |", tampered_message);
    printf("\r\n| (Changed build date)                                     |");
    printf("\r\n|                                                          |");

    status = psa_verify_message(verifier_public_key, PSA_ALG_ML_DSA, tampered_message, tampered_len, signature,
                                signature_len);
    if (PSA_SUCCESS != status)
    {
        printf("\r\n|-- RESULT: -----------------------------------------------|");
        printf("\r\n| [PSA] Verification failed (as expected!)                 |");
        printf("\r\n| [SIM] Tampered message detected!                         |");
    }
    else
    {
        printf("\r\n| ERROR: Should have failed!                               |");
        handle_error(PSA_ERROR_GENERIC_ERROR, "Tamper detection failed\r\n");
    }

    printf("\r\n+----------------------------------------------------------+\r\n");

    /* Cleanup */
    psa_key_id_t cleanup_list[] =
    {
        signer_keypair,
        verifier_public_key
    };

    status = cleanup_keys(cleanup_list, sizeof(cleanup_list) / sizeof(cleanup_list[0]));
    handle_error((int32_t)status, "cleanup_keys failed\r\n");

    psa_reset_key_attributes(&attrs_keypair);
    psa_reset_key_attributes(&attrs_public);
}
/***********************************************************************************************************************
 * End of function mldsa_two_party_signing.
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: mldsa_export_import_keypair.
 * Description  : This function demonstrates two-party digital signature with backup keypair.
 * Arguments    : bits                      Key size in bits.
 *                lifetime                  Key lifetime (volatile/persistent).
 * Return Value : None.
 **********************************************************************************************************************/
static void mldsa_export_import_keypair(size_t bits, psa_key_lifetime_t lifetime)
{
    psa_status_t status = PSA_SUCCESS;

    /* Signing signer's keys (before and after destroy key) */
    psa_key_handle_t signer_keypair_original = RESET_VALUE;
    psa_key_handle_t signer_keypair_restored = RESET_VALUE;

    /* Public key for distribution */
    psa_key_handle_t public_key_for_verification = RESET_VALUE;

    /* Attributes */
    psa_key_attributes_t attrs_keypair = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_attributes_t attrs_public = PSA_KEY_ATTRIBUTES_INIT;

    /* Backup keypair */
    uint8_t keypair_backup[PSA_KEY_GEN_ML_DSA_KEY_PAIR_MAX_SIZE(PSA_KEY_BITS_ML_DSA_65)] BSP_ALIGN_VARIABLE(4);
    size_t keypair_backup_len = RESET_VALUE;

    /* Public key bytes */
    uint8_t public_key_bytes[PSA_KEY_GEN_ML_DSA_PUB_KEY_SIZE(PSA_KEY_BITS_ML_DSA_65)] BSP_ALIGN_VARIABLE(4);
    size_t public_key_len = RESET_VALUE;

    /* Message to be signed */
    const uint8_t msg_v1[] = "application.bin (version 1.0)";
    const size_t msg_v1_len = sizeof(msg_v1) - 1;

    const uint8_t msg_v2[] = "application.bin (version 2.0)";
    const size_t msg_v2_len = sizeof(msg_v2) - 1;

    /* Signature buffers */
    uint8_t signature_v1[PSA_ML_DSA_SIGNATURE_SIZE(PSA_KEY_BITS_ML_DSA_65)] BSP_ALIGN_VARIABLE(4);
    size_t signature_v1_len = RESET_VALUE;

    uint8_t signature_v2[PSA_ML_DSA_SIGNATURE_SIZE(PSA_KEY_BITS_ML_DSA_65)] BSP_ALIGN_VARIABLE(4);
    size_t signature_v2_len = RESET_VALUE;

    printf("\r\n+==========================================================+");
    printf("\r\n|         ML-DSA export & import keypair operation         |");
    printf("\r\n+==========================================================+\r\n");

    /* PHASE 1: Initial setup */
    printf("\r\n+==========================================================+");
    printf("\r\n| PHASE 1: Initial signer setup                            |");
    printf("\r\n+==========================================================+\r\n");

    /* STEP 1: Signer generates signing keypair */
    printf("\r\n+-- STEP 1: Generate signing keypair ----------------------+");

    setup_mldsa_attrs(&attrs_keypair, MLDSA_KEY_KEYPAIR, bits, lifetime);

    status = psa_generate_key(&attrs_keypair, &signer_keypair_original);
    handle_error((int32_t)status, "psa_generate_key failed\r\n");

    printf("\r\n| [PSA] Signer generated ML-DSA keypair                    |");
    printf("\r\n|       (Private key + Public key)                         |");
    printf("\r\n+----------------------------------------------------------+\r\n");

    /* STEP 2: Signer backs up keypair */
    printf("\r\n+-- STEP 2: Export to backup keypair ----------------------+");

    status = psa_export_key(signer_keypair_original, keypair_backup,
                            sizeof(keypair_backup), &keypair_backup_len);
    handle_error((int32_t)status, "psa_export_key failed\r\n");

    printf("\r\n| [PSA] Exported keypair to backup                         |");
    printf("\r\n|       Size: %lu bytes                                   |", (unsigned long)sizeof(keypair_backup));
    printf("\r\n+----------------------------------------------------------+\r\n");

    /* STEP 3: Signer publishes public key */
    printf("\r\n+-- STEP 3: Publish public key ----------------------------+");

    status = psa_export_public_key(signer_keypair_original, public_key_bytes,
                                   sizeof(public_key_bytes), &public_key_len);
    handle_error((int32_t)status, "psa_export_public_key failed\r\n");

    printf("\r\n| [PSA] Exported public key                                |");
    printf("\r\n|       Size: %lu bytes                                   |", (unsigned long)public_key_len);
    printf("\r\n| [SIM] Published public key                               |");
    printf("\r\n+----------------------------------------------------------+\r\n");

    /* STEP 4: Sign first message version */
    printf("\r\n+-- STEP 4: Sign message (Version 1.0) --------------------+");

    printf("\r\n| Developer submits:                                       |");
    printf("\r\n|   \"%s\"                        |", msg_v1);
    printf("\r\n|                                                          |");

    status = psa_sign_message(signer_keypair_original, PSA_ALG_ML_DSA, msg_v1, msg_v1_len, signature_v1,
                              sizeof(signature_v1), &signature_v1_len);
    handle_error((int32_t)status, "psa_sign_message failed\r\n");

    printf("\r\n| [PSA] Signed with private key                            |");
    printf("\r\n|       Signature: %lu bytes                              |", (unsigned long)signature_v1_len);
    printf("\r\n+----------------------------------------------------------+\r\n");

    /* PHASE 2: Destroy keypair */
    printf("\r\n+==========================================================+");
    printf("\r\n| PHASE 2: Destroy keypair                                 |");
    printf("\r\n+==========================================================+\r\n");

    status = psa_destroy_key(signer_keypair_original);
    handle_error((int32_t)status, "psa_destroy_key failed\r\n");
    signer_keypair_original = RESET_VALUE;

    printf("\r\n  [PSA] Destroy the keypair of signer successfully\r\n");

    /* PHASE 3: Recover keypair */
    printf("\r\n+==========================================================+");
    printf("\r\n| PHASE 3: Recover keypair                                 |");
    printf("\r\n+==========================================================+\r\n");

    /* STEP 5: Signer restores keypair from backup */
    printf("\r\n+-- STEP 5: Restore keypair from backup -------------------+");

    /* Re-setup attributes for import */
    setup_mldsa_attrs(&attrs_keypair, MLDSA_KEY_KEYPAIR, bits, lifetime);

    /* Import keypair from backup */
    status = psa_import_key(&attrs_keypair, keypair_backup,
                            sizeof(keypair_backup), &signer_keypair_restored);
    handle_error((int32_t)status, "psa_import_key failed\r\n");

    printf("\r\n| [SIM] Read keypair from backup                           |");
    printf("\r\n| [PSA] Imported keypair successfully                      |");
    printf("\r\n+----------------------------------------------------------+\r\n");

    /* STEP 6: Sign new message with restored key */
    printf("\r\n+-- STEP 6: Sign message (Version 2.0) --------------------+");

    printf("\r\n| Developer submits new version:                           |");
    printf("\r\n|   \"%s\"                        |", msg_v2);
    printf("\r\n|                                                          |");

    status = psa_sign_message(signer_keypair_restored, PSA_ALG_ML_DSA, msg_v2, msg_v2_len, signature_v2,
                              sizeof(signature_v2), &signature_v2_len);
    handle_error((int32_t)status, "psa_sign_message failed\r\n");

    printf("\r\n| [PSA] Signed with restored private key                   |");
    printf("\r\n|       Signature: %lu bytes                              |", (unsigned long)signature_v2_len);
    printf("\r\n+----------------------------------------------------------+\r\n");

    /* STEP 7: Verify both signatures with public key */
    printf("\r\n+-- STEP 7: Verify signature consistency ------------------+");

    /* Import public key for verification */
    setup_mldsa_attrs(&attrs_public, MLDSA_KEY_PUBLIC, bits, lifetime);
    status = psa_import_key(&attrs_public, public_key_bytes, sizeof(public_key_bytes), &public_key_for_verification);
    handle_error((int32_t)status, "psa_import_key (public) failed\r\n");

    /* Verify first message */
    status = psa_verify_message(public_key_for_verification, PSA_ALG_ML_DSA, msg_v1, msg_v1_len, signature_v1,
                                signature_v1_len);
    handle_error((int32_t)status, "Verify v1.0 signature failed\r\n");

    printf("\r\n| [PSA] Version 1.0 message verified                       |");
    printf("\r\n|       (Signed before recovery)                           |");

    /* Verify second message */
    status = psa_verify_message(public_key_for_verification, PSA_ALG_ML_DSA,
                                msg_v2, msg_v2_len, signature_v2, signature_v2_len);
    handle_error((int32_t)status, "Verify v2.0 signature failed\r\n");

    printf("\r\n| [PSA] Version 2.0 message verified                       |");
    printf("\r\n|       (Signed after recovery)                            |");
    printf("\r\n|                                                          |");
    printf("\r\n| Verification complete:                                   |");
    printf("\r\n| - Restored keypair is identical                          |");
    printf("\r\n| - Both old and new messages valid                        |");
    printf("\r\n| - Signing service fully operational!                     |");
    printf("\r\n+----------------------------------------------------------+\r\n");

    /* Cleanup */
    psa_key_id_t cleanup_list[] =
    {
        signer_keypair_restored,
        public_key_for_verification
    };

    status = cleanup_keys(cleanup_list, sizeof(cleanup_list) / sizeof(cleanup_list[0]));
    handle_error((int32_t)status, "cleanup_keys failed\r\n");

    psa_reset_key_attributes(&attrs_keypair);
    psa_reset_key_attributes(&attrs_public);
}
/***********************************************************************************************************************
 * End of function mldsa_export_import_keypair.
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: pqc_decap_sign_remote_verify_operation.
 * Description  : This function demonstrates PSA Crypto API implementation can correctly interoperate with
 *                an independent external party in a real-world distributed environment.
 * Arguments    : ml_kem_bits               Key size of ML-KEM in bits.
 *                ml_dsa_bits               Key size of ML-DSA in bits.
 *                lifetime                  Key lifetime (volatile/persistent).
 * Return Value : None.
 **********************************************************************************************************************/
static void pqc_decap_sign_remote_verify_operation(size_t ml_kem_bits, size_t ml_dsa_bits, psa_key_lifetime_t lifetime)
{
    psa_status_t status = PSA_SUCCESS;
    uint8_t s_recv_buffer[DATA_LEN_SIZE] = {RESET_VALUE};
    psa_key_handle_t kem_key_handle = RESET_VALUE;
    psa_key_attributes_t kem_attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t decap_output_key_handle = RESET_VALUE;
    psa_key_attributes_t shared_secret_attributes = PSA_KEY_ATTRIBUTES_INIT;
    uint8_t shared_secret[PSA_ML_KEM_SHARED_SECRET_SIZE] BSP_ALIGN_VARIABLE(4);
    uint8_t ciphertext[PSA_ML_KEM_CIPHERTEXT_SIZE(PSA_KEY_BITS_ML_KEM_512)] BSP_ALIGN_VARIABLE(4);
    size_t ciphertext_len = PSA_ML_KEM_CIPHERTEXT_SIZE(PSA_KEY_BITS_ML_KEM_512);
    uint8_t export_kem_public_key[PSA_KEY_GEN_ML_KEM_PUBLIC_KEY_MAX_SIZE(PSA_KEY_BITS_ML_KEM_512)]
            BSP_ALIGN_VARIABLE(4);
    size_t export_kem_public_key_len = RESET_VALUE;

    printf("\r\n+==========================================================+");
    printf("\r\n|   ML-KEM decap & ML-DSA sign (Remote verify) operation   |");
    printf("\r\n+==========================================================+\r\n");

    /* Set key attributes */
    setup_mlkem_attrs(&kem_attributes, MLKEM_KEY_KEYPAIR, ml_kem_bits, lifetime);

    status = psa_generate_key(&kem_attributes, &kem_key_handle);
    handle_error((int32_t)status, "Decapsulator: psa_generate_key (ML-KEM) failed\r\n");

    printf("\r\n+-- DECAPSULATOR ------------------------------------------+");
    printf("\r\n| Generated ML-KEM keypair                                 |");

    /* Export the public key to send to the encapsulator */
    status = psa_export_public_key(kem_key_handle, &export_kem_public_key[0],
                                   PSA_KEY_GEN_ML_KEM_PUBLIC_KEY_MAX_SIZE(ml_kem_bits),
                                   &export_kem_public_key_len);
    handle_error((int32_t)status, "Decapsulator: psa_export_public_key failed\r\n");

    printf("\r\n| Exported ML-KEM public key (%lu bytes)                   |", (unsigned long)export_kem_public_key_len);
    printf("\r\n+----------------------------------------------------------+\r\n");
    printf("\r\n  Please run pqc_test.py on the host PC to continue\r\n");

    /* Wait to receive the ACK from the host PC */
    while (FSP_SUCCESS != comms_recv_ack())
    {
       /* Do nothing */
    }

    /* Send the public key to the Encapsulator */
    status = (psa_status_t)comms_send_data(export_kem_public_key, export_kem_public_key_len);
    handle_error((int32_t)status, "Decapsulator: comms_send_data (public key) failed\r\n");

    printf("\r\n  Decapsulator sends public key to Encapsulator...\r\n");

    /* Receive the size of the ciphertext */
    status = (psa_status_t)comms_recv_data(s_recv_buffer, DATA_LEN_SIZE);
    handle_error((int32_t)status, "Decapsulator: comms_recv_data (ciphertext length) failed\r\n");

    uint16_t remote_ciphertext_size = s_recv_buffer[0] * 256 + s_recv_buffer[1];

    /* Receive the ciphertext */
    status = (psa_status_t)comms_recv_data(ciphertext, remote_ciphertext_size);
    handle_error((int32_t)status, "Decapsulator: comms_recv_data (ciphertext) failed\r\n");

    printf("\r\n  Decapsulator receives ciphertext from Encapsulator (%lu bytes)\r\n",
           (unsigned long)remote_ciphertext_size);

    /* Set key attributes for shared secret */
    setup_mlkem_attrs(&shared_secret_attributes, MLKEM_KEY_SHARED_SECRET, 0, lifetime);

    /* Decapsulate the ciphertext */
    status = psa_decapsulate(kem_key_handle, PSA_ALG_ML_KEM, &ciphertext[0], ciphertext_len, &shared_secret_attributes,
                             &decap_output_key_handle);
    handle_error((int32_t)status, "Decapsulator: psa_decapsulate failed\r\n");

    /* Export the shared secret of decapsulator */
    status = psa_export_key(decap_output_key_handle, &shared_secret[0], PSA_ML_KEM_SHARED_SECRET_SIZE,
                            &export_kem_public_key_len);
    handle_error((int32_t)status, "Decapsulator: psa_export_key (shared secret of decapsulator) failed\r\n");

    /* Send the decapsulated shared secret to the host PC to compare with the one generated there */
    uint32_t comms_len = sizeof(shared_secret);

    status = (psa_status_t)comms_send_data(shared_secret, comms_len);
    handle_error((int32_t)status, "Decapsulator: comms_send_data (decapsulator shared secret) failed\r\n");

    printf("\r\n  Decapsulator sends shared secret to Encapsulator...\r\n");

    /* Receive the response from the verifier to determine the verification result */
    status = (psa_status_t)comms_read(s_recv_buffer, RESP_CMD_SIZE);
    handle_error((int32_t)status, "Decapsulator: comms_read (response) failed\r\n");

    if (CMD_ACK != s_recv_buffer[0])
    {
        handle_error((int32_t)PSA_ERROR_GENERIC_ERROR, "[Result] Shared secrets do not match!\r\n");
    }

    printf("\r\n  [Result] Shared secrets match!\r\n");

    /* Sign the shared secret */
    psa_key_handle_t dsa_key_handle = RESET_VALUE;
    psa_key_attributes_t dsa_attributes = PSA_KEY_ATTRIBUTES_INIT;

    uint8_t export_dsa_public_key[PSA_KEY_GEN_ML_DSA_PUB_KEY_SIZE(PSA_KEY_BITS_ML_DSA_44)] BSP_ALIGN_VARIABLE(4);
    size_t export_dsa_public_key_len = RESET_VALUE;
    uint8_t sign[PSA_ML_DSA_SIGNATURE_SIZE(PSA_KEY_BITS_ML_DSA_44)] BSP_ALIGN_VARIABLE(4);
    size_t sign_len = RESET_VALUE;

    printf("\r\n+-- SIGNER ------------------------------------------------+");

    /* Set key attributes */
    setup_mldsa_attrs(&dsa_attributes, MLDSA_KEY_KEYPAIR, ml_dsa_bits, lifetime);

    /* Generate key pair */
    status = psa_generate_key(&dsa_attributes, &dsa_key_handle);
    handle_error((int32_t)status, "Signer: psa_generate_key (ML-DSA) failed\r\n");

    printf("\r\n|  Generated ML-DSA keypair                                |");

    status = psa_sign_message(dsa_key_handle, PSA_ALG_ML_DSA, shared_secret, sizeof(shared_secret), sign, sizeof(sign),
                              &sign_len);
    handle_error((int32_t)status, "Signer: psa_sign_message failed\r\n");

    printf("\r\n|  Signer signs shared secret key                          |");

    /* Export the public key to send to the host PC (Verifier) */
    status = psa_export_public_key(dsa_key_handle, &export_dsa_public_key[0],
                                   PSA_KEY_GEN_ML_DSA_PUB_KEY_SIZE(PSA_KEY_BITS_ML_DSA_44), &export_dsa_public_key_len);
    handle_error((int32_t)status, "Signer: psa_export_public_key failed\r\n");

    printf("\r\n|  Exported ML-DSA public key (%lu bytes)                 |", (unsigned long)export_dsa_public_key_len);
    printf("\r\n+----------------------------------------------------------+\r\n");

    /* Send the public key to the host PC (Verifier) */
    status = (psa_status_t)comms_send_data(export_dsa_public_key, export_dsa_public_key_len);
    handle_error((int32_t)status, "Signer: comms_send_data (public key) failed\r\n\r\n");

    printf("\r\n  Signer sends public key to verifier...\r\n");

    /* Send the signature to the host PC (Verifier) */
    status = (psa_status_t)comms_send_data(sign, sign_len);
    handle_error((int32_t)status, "Signer: comms_send_data (signature) failed\r\n");

    printf("\r\n  Signer sends signature to verifier...\r\n");

    /* Receive the response from the host PC (Verifier) to determine the verification result */
    status = (psa_status_t)comms_read(s_recv_buffer, RESP_CMD_SIZE);
    handle_error((int32_t)status, "Signer: comms_read (response) failed\r\n");

    if (CMD_ACK != s_recv_buffer[0])
    {
        handle_error((int32_t)PSA_ERROR_GENERIC_ERROR, "[Result] Signature verification failed!\r\n");
    }

    printf("\r\n  [Result] Signature verified successfully!\r\n");

    /* Cleanup */
    psa_key_id_t cleanup_list[] =
    {
        kem_key_handle,
        decap_output_key_handle,
        dsa_key_handle
    };

    status = cleanup_keys(cleanup_list, sizeof(cleanup_list) / sizeof(cleanup_list[0]));
    handle_error((int32_t)status, "cleanup_keys failed\r\n");

    psa_reset_key_attributes(&kem_attributes);
    psa_reset_key_attributes(&shared_secret_attributes);
    psa_reset_key_attributes(&dsa_attributes);
}
/***********************************************************************************************************************
 * End of function pqc_decap_sign_remote_verify_operation.
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: user_input.
 * Description  : This function reads a line of user input from stdin and removes trailing newline characters.
 * Arguments    : buf                       Pointer to buffer where the input string is stored.
 *                len                       Maximum number of characters to read (buffer size).
 * Return Value : None.
 **********************************************************************************************************************/
static void user_input(char * buf, int len)
{
    fgets(buf, len, stdin);
    buf[strcspn(buf, "\r\n")] = '\0';
}
/***********************************************************************************************************************
 * End of function user_input.
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: handle_error.
 * Description  : This function handles an error by printing error message and code, de-initializing the related
 *                modules, and entering a trap state.
 * Arguments    : err                       Error code.
 *                err_str                   Error message.
 * Return Value : None.
 **********************************************************************************************************************/
static void handle_error(int32_t err, const char * err_str)
{
    if (0 != err)
    {
        printf("\r\n[ERR]: %s", err_str);
        printf("Returned Error Code: 0x%x\r\n", (unsigned int)err);
        uart_console_flush();
        littlefs_deinit();
        comms_deinit();
        __BKPT(0);
    }
}
/***********************************************************************************************************************
 * End of function handle_error.
 **********************************************************************************************************************/
