/***********************************************************************************************************************
 * File Name    : pqc_mlkem.c
 * Description  : Contains data structures and functions used to demonstrate ML-KEM algorithm operations..
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "pqc_mlkem.h"

/* Private function declaration */
static psa_status_t compare_shared_secrets(psa_key_id_t ss1_handle,  psa_key_id_t ss2_handle, const char *error_msg);

/***********************************************************************************************************************
 *  Function Name: setup_mlkem_attrs
 *  Description  : This function is used to setup universal key attributes for ML-KEM.
 *  Arguments    : attrs          Pointer to attributes structure.
 *                 key_type       Type of key (keypair/public/shared_secret).
 *                 bits           Key size in bits (ignored for shared_secret).
 *                 lifetime       Key lifetime (volatile/persistent).
 *  Return Value : None.
 **********************************************************************************************************************/
void setup_mlkem_attrs(psa_key_attributes_t *attrs,
                              mlkem_key_type_t key_type,
                              size_t bits,
                              psa_key_lifetime_t lifetime)
{
    psa_reset_key_attributes(attrs);
    psa_set_key_algorithm(attrs, PSA_ALG_ML_KEM);
    psa_set_key_lifetime(attrs, lifetime);

    switch (key_type)
    {
        case MLKEM_KEY_KEYPAIR:
            psa_set_key_usage_flags(attrs, PSA_KEY_USAGE_DECAPSULATE | PSA_KEY_USAGE_EXPORT);
            psa_set_key_type(attrs, PSA_KEY_TYPE_ML_KEM_KEY_PAIR);
            psa_set_key_bits(attrs, bits);
            if (PSA_KEY_LIFETIME_IS_PERSISTENT(lifetime))
            {
                psa_set_key_id(attrs, MLKEM_DECAPSULATOR_KEYPAIR_ID);
            }
            break;

        case MLKEM_KEY_PUBLIC:
            psa_set_key_usage_flags(attrs, PSA_KEY_USAGE_ENCAPSULATE);
            psa_set_key_type(attrs, PSA_KEY_TYPE_ML_KEM_PUBLIC_KEY);
            psa_set_key_bits(attrs, bits);
            if (PSA_KEY_LIFETIME_IS_PERSISTENT(lifetime))
            {
                psa_set_key_id(attrs, MLKEM_ENCAPSULATOR_PUBLIC_KEY_ID);
            }
            break;

        case MLKEM_KEY_SHARED_SECRET:
            psa_set_key_usage_flags(attrs, PSA_KEY_USAGE_EXPORT);
            psa_set_key_type(attrs, PSA_KEY_TYPE_RAW_DATA);
            psa_set_key_bits(attrs, PSA_BYTES_TO_BITS(PSA_ML_KEM_SHARED_SECRET_SIZE));
            /* Force shared secrets to be VOLATILE even if keypair is PERSISTENT */
            if (PSA_KEY_LIFETIME_IS_PERSISTENT(lifetime))
            {
                psa_set_key_lifetime(attrs, PSA_KEY_LIFETIME_VOLATILE);
            }
            break;

        default:
            APP_PRINT("The selected key type is not supported\r\n");
            break;
    }
}
/***********************************************************************************************************************
* End of function setup_mlkem_attrs.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: compare_shared_secrets
 *  Description  : This function is used to export and compare two shared secrets.
 *  Arguments    : ss1_handle     First shared secret.
 *                 ss2_handle     Second shared secret.
 *                 error_msg      Error message is display when two shared secrets are different.
 *  Return Value : PSA_SUCCESS    Upon successful operation.
 *                 Any other error code apart from PSA_SUCCESS.
 **********************************************************************************************************************/
static psa_status_t compare_shared_secrets(psa_key_id_t ss1_handle,
                                           psa_key_id_t ss2_handle,
                                           const char *error_msg)
{
    uint8_t ss1[PSA_ML_KEM_SHARED_SECRET_SIZE];
    uint8_t ss2[PSA_ML_KEM_SHARED_SECRET_SIZE];
    size_t ss1_len = 0, ss2_len = 0;
    psa_status_t status = PSA_SUCCESS;

    status = psa_export_key(ss1_handle, ss1, sizeof(ss1), &ss1_len);
    APP_ERR_RET(PSA_SUCCESS != status, status, "Export ss1 failed\r\n");

    status = psa_export_key(ss2_handle, ss2, sizeof(ss2), &ss2_len);
    APP_ERR_RET(PSA_SUCCESS != status, status, "Export ss2 failed\r\n");

    if (ss1_len != ss2_len || memcmp(ss1, ss2, ss1_len) != 0)
    {
        APP_ERR_PRINT(error_msg);
        return PSA_ERROR_GENERIC_ERROR;
    }

    return PSA_SUCCESS;
}
/***********************************************************************************************************************
* End of function compare_shared_secrets.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: mlkem_two_party_key_exchange
 *  Description  : This function is used to demonstrate two-party key exchange.
 *  Arguments    : bits           Key size in bits.
 *                 lifetime       Key lifetime (volatile/persistent).
 *  Return Value : PSA_SUCCESS    Upon successful operation.
 *                 Any other error code apart from PSA_SUCCESS.
 **********************************************************************************************************************/
psa_status_t mlkem_two_party_key_exchange(size_t bits, psa_key_lifetime_t lifetime)
{
    psa_status_t status = PSA_SUCCESS;

    /* Decapsulator's keys */
    psa_key_handle_t decapsulator_keypair = 0;
    psa_key_id_t decapsulator_ss_handle = 0;

    /* Encapsulator's keys */
    psa_key_handle_t encapsulator_public_key = 0;
    psa_key_id_t encapsulator_ss_handle = 0;

    /* Attributes */
    psa_key_attributes_t attrs_keypair = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_attributes_t attrs_public = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_attributes_t attrs_shared_secret = PSA_KEY_ATTRIBUTES_INIT;

    /* Network transfer buffers */
    uint8_t public_key_to_send[PSA_KEY_GEN_ML_KEM_PUBLIC_KEY_MAX_SIZE(PSA_KEY_BITS_ML_KEM_512)];
    size_t public_key_len = 0;

    uint8_t ciphertext_to_send[PSA_ML_KEM_CIPHERTEXT_SIZE(PSA_KEY_BITS_ML_KEM_512)];
    size_t ciphertext_len = 0;

    APP_PRINT("\r\n+======================================================+\r\n");
    APP_PRINT(    "| Decapsulator-Encapsulator Key Exchange operation     |\r\n");
    APP_PRINT(    "+======================================================+\r\n\r\n");

    /* ====================================================================
     * DECAPSULATOR SIDE - Key Generation
     * ==================================================================== */
    APP_PRINT("+-- DECAPSULATOR (Receiver) ---------------------------+\r\n");

    setup_mlkem_attrs(&attrs_keypair, MLKEM_KEY_KEYPAIR, bits, lifetime);

    status = psa_generate_key(&attrs_keypair, &decapsulator_keypair);
    APP_ERR_RET(PSA_SUCCESS != status, status, "Decapsulator: psa_generate_key failed");

    APP_PRINT("| [PSA] Generated ML-KEM keypair                       |\r\n");

    /* Export public key to send to encapsulator */
    status = psa_export_public_key(decapsulator_keypair, public_key_to_send,
                                   sizeof(public_key_to_send),
                                   &public_key_len);
    APP_ERR_RET(PSA_SUCCESS != status, status, "Decapsulator: psa_export_public_key failed");

    APP_PRINT("| [PSA] Exported public key (%lu bytes)                |\r\n", (unsigned long)public_key_len);
    APP_PRINT("+------------------------------------------------------+\r\n\r\n");

    /* Transfer simulation */
    APP_PRINT("  [SIM] Decapsulator sends PUBLIC KEY to Encapsulator...\r\n\r\n");

    /* ====================================================================
     * ENCAPSULATOR SIDE - Encapsulation
     * ==================================================================== */
    APP_PRINT("+-- ENCAPSULATOR (Sender) -----------------------------+\r\n");

    /* Encapsulator imports decapsulator's public key */
    setup_mlkem_attrs(&attrs_public, MLKEM_KEY_PUBLIC, bits, lifetime);

    status = psa_import_key(&attrs_public, public_key_to_send,
                           public_key_len, &encapsulator_public_key);
    APP_ERR_RET(PSA_SUCCESS != status, status, "Encapsulator: psa_import_key (public key) failed");

    APP_PRINT("| [SIM] Encapsulator received the public key           |\r\n");
    APP_PRINT("|      (Encapsulator has NO private key!)              |\r\n");
    APP_PRINT("| [PSA] Encapsulator imported the public key           |\r\n");

    /* Encapsulator encapsulates using ONLY public key */
    setup_mlkem_attrs(&attrs_shared_secret, MLKEM_KEY_SHARED_SECRET, 0, lifetime);

    status = psa_encapsulate(encapsulator_public_key,
                            PSA_ALG_ML_KEM,
                            &attrs_shared_secret,
                            &encapsulator_ss_handle,
                            ciphertext_to_send,
                            sizeof(ciphertext_to_send),
                            &ciphertext_len);
    APP_ERR_RET(PSA_SUCCESS != status, status, "Encapsulator: psa_encapsulate failed");

    APP_PRINT("| [PSA] Encapsulated (ciphertext %lu bytes)            |\r\n", (unsigned long)ciphertext_len);
    APP_PRINT("| [PSA] Generated shared secret (Encapsulator side)    |\r\n");
    APP_PRINT("+------------------------------------------------------+\r\n\r\n");

    /* Transfer simulation */
    APP_PRINT("  [SIM] Encapsulator sends CIPHERTEXT to Decapsulator...\r\n\r\n");

    /* ====================================================================
     * DECAPSULATOR SIDE - Decapsulation
     * ==================================================================== */
    APP_PRINT("+-- DECAPSULATOR (Receiver) ---------------------------+\r\n");

    /* Decapsulator decapsulates using PRIVATE KEY */
    status = psa_decapsulate(decapsulator_keypair,
                            PSA_ALG_ML_KEM,
                            ciphertext_to_send,
                            ciphertext_len,
                            &attrs_shared_secret,
                            &decapsulator_ss_handle);
    APP_ERR_RET(PSA_SUCCESS != status, status, "Decapsulator: psa_decapsulate failed");

    APP_PRINT("| [PSA] Decapsulated ciphertext                        |\r\n");
    APP_PRINT("| [PSA] Generated shared secret (Decapsulator side)    |\r\n");
    APP_PRINT("+------------------------------------------------------+\r\n\r\n");

    /* ====================================================================
     * VERIFICATION
     * ==================================================================== */
    APP_PRINT("+-- VERIFICATION --------------------------------------+\r\n");

    status = compare_shared_secrets(decapsulator_ss_handle, encapsulator_ss_handle,
                                   "CRITICAL: Shared secrets mismatch!");
    APP_ERR_RET(PSA_SUCCESS != status, status, "Shared secret comparison failed");

    APP_PRINT("|Decapsulator and encapsulator have same shared secrets|\r\n");
    APP_PRINT("| Key exchange successful!                             |\r\n");
    APP_PRINT("+------------------------------------------------------+\r\n\r\n");

    /* Cleanup */
    psa_key_id_t cleanup_list[] = {
        decapsulator_keypair, encapsulator_public_key,
        decapsulator_ss_handle, encapsulator_ss_handle
    };
    cleanup_keys(cleanup_list, sizeof(cleanup_list) / sizeof(cleanup_list[0]));
    APP_ERR_RET(PSA_SUCCESS != status, status, "cleanup_keys failed");

    psa_reset_key_attributes(&attrs_keypair);
    psa_reset_key_attributes(&attrs_public);
    psa_reset_key_attributes(&attrs_shared_secret);

    return status;
}
/***********************************************************************************************************************
* End of function mlkem_two_party_key_exchange.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: mlkem_export_import_keypair
 *  Description  : This function is used to demonstrate two-party key exchange with backup keypair.
 *  Arguments    : bits           Key size in bits.
 *                 lifetime       Key lifetime (volatile/persistent).
 *  Return Value : PSA_SUCCESS    Upon successful operation.
 *                 Any other error code apart from PSA_SUCCESS.
 **********************************************************************************************************************/
psa_status_t mlkem_export_import_keypair(size_t bits, psa_key_lifetime_t lifetime)
{
    psa_status_t status;

    /* Decapsulator's keys (before and after destroy keypair) */
    psa_key_handle_t decapsulator_keypair_original = 0;
    psa_key_handle_t decapsulator_keypair_restored = 0;

    /* Encapsulator's key (only has public key) */
    psa_key_handle_t encapsulator_public_key = 0;

    /* Shared secrets */
    psa_key_id_t decapsulator_ss_original = 0;    /* Before destroy keypair */
    psa_key_id_t encapsulator_ss = 0;             /* Encapsulator side */
    psa_key_id_t decapsulator_ss_restored = 0;    /* After destroy keypair */

    /* Attributes */
    psa_key_attributes_t attrs_keypair = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_attributes_t attrs_public = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_attributes_t attrs_shared_secret = PSA_KEY_ATTRIBUTES_INIT;

    /* Backup keypair */
    uint8_t keypair_backup[PSA_KEY_GEN_ML_KEM_KEY_PAIR_MAX_SIZE(PSA_KEY_BITS_ML_KEM_512)];
    size_t keypair_backup_len = 0;

    /* Public key to share with encapsulators */
    uint8_t public_key_bytes[PSA_KEY_GEN_ML_KEM_PUBLIC_KEY_MAX_SIZE(PSA_KEY_BITS_ML_KEM_512)];
    size_t public_key_len = 0;

    /* Ciphertext from encapsulator (stored for reuse after destroy keypair) */
    uint8_t ciphertext_from_encapsulator[PSA_ML_KEM_CIPHERTEXT_SIZE(PSA_KEY_BITS_ML_KEM_512)];
    size_t ciphertext_len = 0;

    APP_PRINT("\r\n+====================================================+\r\n");
    APP_PRINT(    "|      ML-KEM import & export keypair operation      |\r\n");
    APP_PRINT(    "+====================================================+\r\n\r\n");

    /* ====================================================================
     * PHASE 1: INITIAL SETUP
     * ==================================================================== */
    APP_PRINT("+====================================================+\r\n");
    APP_PRINT("| PHASE 1: Initialize Device Setup                   |\r\n");
    APP_PRINT("+====================================================+\r\n\r\n");

    /* ----------------------------------------------------------------
     * STEP 1: Decapsulator generates keypair
     * ---------------------------------------------------------------- */
    APP_PRINT("+-- STEP 1: Decapsulator Generates Keypair ----------+\r\n");

    setup_mlkem_attrs(&attrs_keypair, MLKEM_KEY_KEYPAIR, bits, lifetime);

    status = psa_generate_key(&attrs_keypair, &decapsulator_keypair_original);
    APP_ERR_RET(PSA_SUCCESS != status, status, "psa_generate_key failed");

    APP_PRINT("| [PSA] Decapsulator generated ML-KEM keypair        |\r\n");
    APP_PRINT("|       (Private key + Public key)                   |\r\n");
    APP_PRINT("+----------------------------------------------------+\r\n\r\n");

    /* ----------------------------------------------------------------
     * STEP 2: Decapsulator backs up keypair
     * ---------------------------------------------------------------- */
    APP_PRINT("+------- STEP 2: Backup Keypair ---------------------+\r\n");

    status = psa_export_key(decapsulator_keypair_original, keypair_backup,
                           sizeof(keypair_backup), &keypair_backup_len);
    APP_ERR_RET(PSA_SUCCESS != status, status, "psa_export_key failed");

    APP_PRINT("| [PSA] Exported keypair                             |\r\n");
    APP_PRINT("|       Size: %lu bytes                             |\r\n", (unsigned long)sizeof(keypair_backup));
    APP_PRINT("+----------------------------------------------------+\r\n\r\n");

    /* ----------------------------------------------------------------
     * STEP 3: Decapsulator shares public key with encapsulator
     * ---------------------------------------------------------------- */
    APP_PRINT("+-- STEP 3: Share Public Key with encapsulator-------+\r\n");

    status = psa_export_public_key(decapsulator_keypair_original, public_key_bytes,
                                   sizeof(public_key_bytes), &public_key_len);
    APP_ERR_RET(PSA_SUCCESS != status, status, "psa_export_public_key failed");

    APP_PRINT("| [PSA] Exported public key                          |\r\n");
    APP_PRINT("|       Size: %lu bytes                              |\r\n", (unsigned long)public_key_len);
    APP_PRINT("+----------------------------------------------------+\r\n\r\n");

    APP_PRINT("  [SIM] Decapsulator sends PUBLIC KEY to Encapsulator...\r\n\r\n");

    /* ----------------------------------------------------------------
     * STEP 4: Encapsulator imports public key
     * ---------------------------------------------------------------- */
    APP_PRINT("+-- STEP 4: Encapsulator Imports Public Key ---------+\r\n");

    setup_mlkem_attrs(&attrs_public, MLKEM_KEY_PUBLIC, bits, lifetime);

    status = psa_import_key(&attrs_public, public_key_bytes,
                           public_key_len, &encapsulator_public_key);
    APP_ERR_RET(PSA_SUCCESS != status, status, "psa_import_key (public) failed");

    APP_PRINT("| [SIM] Encapsulator received public key             |\r\n");
    APP_PRINT("| [PSA] Encapsulator imported public key             |\r\n");
    APP_PRINT("|      (Encapsulator has NO private key!)            |\r\n");
    APP_PRINT("+----------------------------------------------------+\r\n\r\n");

    /* ----------------------------------------------------------------
     * STEP 5: Encapsulator sends encrypted data to decapsulator
     * ---------------------------------------------------------------- */
    APP_PRINT("+-- STEP 5: Encapsulator Sends Encrypted Data -------+\r\n");

    setup_mlkem_attrs(&attrs_shared_secret, MLKEM_KEY_SHARED_SECRET, 0, lifetime);

    /* Encapsulator encapsulates using ONLY public key (no private key!) */
    status = psa_encapsulate(encapsulator_public_key, PSA_ALG_ML_KEM,
                            &attrs_shared_secret, &encapsulator_ss,
                            ciphertext_from_encapsulator, sizeof(ciphertext_from_encapsulator),
                            &ciphertext_len);
    APP_ERR_RET(PSA_SUCCESS != status, status, "psa_encapsulate failed");

    APP_PRINT("| [PSA] Encapsulator encapsulated with public key    |\r\n");
    APP_PRINT("|       Ciphertext: %lu bytes                        |\r\n", (unsigned long)ciphertext_len);
    APP_PRINT("| [PSA] Encapsulator derived shared secret           |\r\n");
    APP_PRINT("+----------------------------------------------------+\r\n\r\n");

    APP_PRINT("  [SIM] Encapsulator sends CIPHERTEXT to Decapsulator...\r\n\r\n");

    /* ----------------------------------------------------------------
     * STEP 6: Decapsulator decapsulates data
     * ---------------------------------------------------------------- */
    APP_PRINT("+-- STEP 6: Decapsulator Decapsulates Data ----------+\r\n");

    /* Decapsulator uses private key (in keypair) to decapsulate */
    status = psa_decapsulate(decapsulator_keypair_original, PSA_ALG_ML_KEM,
                            ciphertext_from_encapsulator, ciphertext_len,
                            &attrs_shared_secret, &decapsulator_ss_original);
    APP_ERR_RET(PSA_SUCCESS != status, status, "psa_decapsulate failed");

    APP_PRINT("| [SIM] Decapsulator received ciphertext             |\r\n");
    APP_PRINT("| [PSA] Decapsulator decapsulated successfully       |\r\n");
    APP_PRINT("| [PSA] Decapsulator derived shared secret           |\r\n");
    APP_PRINT("|      (Using private key from keypair)              |\r\n");
    APP_PRINT("+----------------------------------------------------+\r\n\r\n");

    /* ====================================================================
     * PHASE 2: DESTROY KEY
     * ==================================================================== */
    APP_PRINT("+====================================================+\r\n");
    APP_PRINT("| PHASE 2: Destroy key                               |\r\n");
    APP_PRINT("+====================================================+\r\n\r\n");

    status = psa_destroy_key(decapsulator_keypair_original);
    APP_ERR_RET(PSA_SUCCESS != status, status, "psa_destroy_key failed");
    decapsulator_keypair_original = 0;

    APP_PRINT(" [PSA] Destroy the keypair of decapsulator successfully\r\n");

    status = psa_destroy_key(encapsulator_public_key);
    APP_ERR_RET(PSA_SUCCESS != status, status, "psa_destroy_key failed");
    encapsulator_public_key = 0;

    APP_PRINT(" [PSA] Destroy the public key of encapsulator successfully\r\n\r\n");

    /* ====================================================================
     * PHASE 3: RECOVERY AFTER DESTROY KEY
     * ==================================================================== */
    APP_PRINT("+====================================================+\r\n");
    APP_PRINT("| PHASE 3: Recovery the key                          |\r\n");
    APP_PRINT("+====================================================+\r\n\r\n");

    /* ----------------------------------------------------------------
     * STEP 7: Decapsulator restores keypair
     * ---------------------------------------------------------------- */
    APP_PRINT("+-- STEP 7: Restore Keypair -------------------------+\r\n");

    /* Re-setup keypair attributes for import */
    setup_mlkem_attrs(&attrs_keypair, MLKEM_KEY_KEYPAIR, bits, lifetime);
    psa_set_key_usage_flags(&attrs_keypair, PSA_KEY_USAGE_DECAPSULATE |
                                            PSA_KEY_USAGE_EXPORT);

    /* Decapsulator imports keypair from flash backup */
    status = psa_import_key(&attrs_keypair, keypair_backup,
                           sizeof(keypair_backup), &decapsulator_keypair_restored);
    APP_ERR_RET(PSA_SUCCESS != status, status, "psa_import_key failed");

    APP_PRINT("| [SIM] Read keypair from backup                     |\r\n");
    APP_PRINT("| [PSA] Imported keypair                             |\r\n");
    APP_PRINT("|       Decapsulator restored its identity!          |\r\n");
    APP_PRINT("+----------------------------------------------------+\r\n\r\n");

    /* ----------------------------------------------------------------
     * STEP 8: Decapsulator decapsulates old ciphertext
     * ---------------------------------------------------------------- */
    APP_PRINT("+-- STEP 8: Decapsulate Old Ciphertext --------------+\r\n");

    /* Decapsulate using the restored keypair */
    status = psa_decapsulate(decapsulator_keypair_restored, PSA_ALG_ML_KEM,
                            ciphertext_from_encapsulator, ciphertext_len,
                            &attrs_shared_secret, &decapsulator_ss_restored);
    APP_ERR_RET(PSA_SUCCESS != status, status, "psa_decapsulate failed");

    APP_PRINT("| [PSA] Decapsulated with restored keypair           |\r\n");
    APP_PRINT("| [PSA] Derived shared secret (after reboot)         |\r\n");
    APP_PRINT("+----------------------------------------------------+\r\n\r\n");

    /* ----------------------------------------------------------------
     * STEP 9: Verify shared secrets match
     * ---------------------------------------------------------------- */
    APP_PRINT("+-- STEP 9: Verify Consistency ----------------------+\r\n");

    /* Compare: original decapsulator Shared secrets vs restored decapsulator Shared secrets
     * They should be identical if backup/restore works correctly */
    status = compare_shared_secrets(decapsulator_ss_original, decapsulator_ss_restored, "Shared secrets mismatch after reboot!");
    APP_ERR_RET(PSA_SUCCESS != status, status, "Shared secret verification failed");

    /* Also verify against encapsulator's shared secret */
    status = compare_shared_secrets(encapsulator_ss, decapsulator_ss_restored, "Decapsulator/Encapsulator shared secrets mismatch!");
    APP_ERR_RET(PSA_SUCCESS != status, status, "Decapsulator-Encapsulator verification failed");

    APP_PRINT("| [PSA] Decapsulator SS (original) == (restored)     |\r\n");
    APP_PRINT("| [PSA] Decapsulator SS (restored) == Encapsulator SS|\r\n");
    APP_PRINT("|       Key recovery successful!                     |\r\n");
    APP_PRINT("|                                                    |\r\n");
    APP_PRINT("| VERIFICATION COMPLETE:                             |\r\n");
    APP_PRINT("| - Restored keypair is identical                    |\r\n");
    APP_PRINT("| - Can decrypt old messages from clients            |\r\n");
    APP_PRINT("+----------------------------------------------------+\r\n");

    /* Cleanup */
    psa_key_id_t cleanup_list[] = {
        decapsulator_keypair_restored,
        decapsulator_ss_original,
        encapsulator_ss,
        decapsulator_ss_restored
    };
    cleanup_keys(cleanup_list, sizeof(cleanup_list) / sizeof(cleanup_list[0]));
    APP_ERR_RET(PSA_SUCCESS != status, status, "cleanup_keys failed");

    psa_reset_key_attributes(&attrs_keypair);
    psa_reset_key_attributes(&attrs_public);
    psa_reset_key_attributes(&attrs_shared_secret);

    return status;
}
/***********************************************************************************************************************
* End of function mlkem_export_import_keypair.
***********************************************************************************************************************/
