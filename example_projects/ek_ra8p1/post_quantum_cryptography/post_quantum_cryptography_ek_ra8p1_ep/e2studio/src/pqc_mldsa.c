/***********************************************************************************************************************
* File Name    : pqc_mldsa.c
* Description  : Contains data structures and functions used to demonstrate ML-DSA algorithm operations..
***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "pqc_mldsa.h"

/***********************************************************************************************************************
 *  Function Name: setup_mldsa_attrs
 *  Description  : This function is used to setup universal key attributes for ML-DSA.
 *  Arguments    : attrs          Pointer to attributes structure.
 *                 key_type       Type of key (keypair/public).
 *                 bits           Key size in bits.
 *                 lifetime       Key lifetime (volatile/persistent).
 *  Return Value : None.
 **********************************************************************************************************************/
void setup_mldsa_attrs(psa_key_attributes_t *attrs, mldsa_key_type_t key_type, size_t bits,
        psa_key_lifetime_t lifetime)
{
    psa_reset_key_attributes(attrs);
    psa_set_key_algorithm(attrs, PSA_ALG_ML_DSA);
    psa_set_key_lifetime(attrs, lifetime);
    psa_set_key_bits(attrs, bits);

    switch (key_type)
    {
        case MLDSA_KEY_KEYPAIR:
            psa_set_key_usage_flags(attrs, PSA_KEY_USAGE_SIGN_MESSAGE |
                                           PSA_KEY_USAGE_EXPORT);
            psa_set_key_type(attrs, PSA_KEY_TYPE_ML_DSA_KEY_PAIR);
            if (PSA_KEY_LIFETIME_IS_PERSISTENT(lifetime))
            {
                psa_set_key_id(attrs, MLDSA_SIGNER_KEYPAIR_ID);
            }
            break;

        case MLDSA_KEY_PUBLIC:
            psa_set_key_usage_flags(attrs, PSA_KEY_USAGE_VERIFY_MESSAGE);
            psa_set_key_type(attrs, PSA_KEY_TYPE_ML_DSA_PUBLIC_KEY);
            if (PSA_KEY_LIFETIME_IS_PERSISTENT(lifetime))
            {
                psa_set_key_id(attrs, MLDSA_VERIFIER_PUBLIC_KEY_ID);
            }
            break;

        default:
            APP_PRINT("The selected key type is not supported\r\n");
            break;
    }
}
/***********************************************************************************************************************
* End of function setup_mldsa_attrs.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: mldsa_two_party_signing
 *  Description  : This function is used to demonstrate two-party digital signature.
 *  Arguments    : bits           Key size in bits.
 *                 lifetime       Key lifetime (volatile/persistent).
 *  Return Value : PSA_SUCCESS    Upon successful operation.
 *                 Any other error code apart from PSA_SUCCESS.
 **********************************************************************************************************************/
psa_status_t mldsa_two_party_signing(size_t bits, psa_key_lifetime_t lifetime)
{
    psa_status_t status = PSA_SUCCESS;

    /* Signer's keys (signer) */
    psa_key_handle_t signer_keypair = 0;

    /* Verifier's key (verifier - only has public key) */
    psa_key_handle_t verifier_public_key = 0;

    /* Attributes */
    psa_key_attributes_t attrs_keypair = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_attributes_t attrs_public = PSA_KEY_ATTRIBUTES_INIT;

    /* Public key to share */
    uint8_t public_key_to_send[PSA_KEY_GEN_ML_DSA_PUB_KEY_SIZE(PSA_KEY_BITS_ML_DSA_65)];
    size_t public_key_len = 0;

    /* Message to sign */
    const uint8_t message[] = "Firmware version 2.1.0 - Build 20260119";
    const size_t message_len = sizeof(message) - 1; /* Exclude null terminator */

    /* Signature buffer */
    uint8_t signature[PSA_ML_DSA_SIGNATURE_SIZE(PSA_KEY_BITS_ML_DSA_65)];
    size_t signature_len = 0;

    APP_PRINT("\r\n+============================================+\r\n");
    APP_PRINT(    "|   Signing & Verification Operation         |\r\n");
    APP_PRINT(    "+============================================+\r\n\r\n");

    /* ====================================================================
     * SIGNER SIDE - Key Generation
     * ==================================================================== */
    APP_PRINT("+-- SIGNER (Signer) -------------------------+\r\n");

    setup_mldsa_attrs(&attrs_keypair, MLDSA_KEY_KEYPAIR, bits, lifetime);

    /* Generate key pair */
    status = psa_generate_key(&attrs_keypair, &signer_keypair);
    APP_ERR_RET(PSA_SUCCESS != status, status, "Signer: psa_generate_key failed\r\n");
    APP_PRINT("| [PSA] Generated ML-DSA keypair             |\r\n");

    /* Export public key to send to verifiers */
    status = psa_export_public_key(signer_keypair, public_key_to_send,
                                   sizeof(public_key_to_send),
                                   &public_key_len);
    APP_ERR_RET(PSA_SUCCESS != status, status, "Signer: psa_export_public_key failed\r\n");
    APP_PRINT("| [PSA] Exported public key (%lu bytes)     |\r\n", (unsigned long)public_key_len);
    APP_PRINT("+--------------------------------------------+\r\n\r\n");

    /* Transfer simulation */
    APP_PRINT("  [SIM] Signer publishes PUBLIC KEY...\r\n\r\n");

    /* ====================================================================
     * VERIFIER SIDE - Import Public Key
     * ==================================================================== */
    APP_PRINT("+-- VERIFIER (Verifier) ---------------------+\r\n");

    /* Verifier imports signer's public key */
    setup_mldsa_attrs(&attrs_public, MLDSA_KEY_PUBLIC, bits, lifetime);

    status = psa_import_key(&attrs_public, public_key_to_send,
                           public_key_len, &verifier_public_key);
    APP_ERR_RET(PSA_SUCCESS != status, status, "Verifier: psa_import_key (public key) failed\r\n");

    APP_PRINT("| [SIM] Verifier received the public key     |\r\n");
    APP_PRINT("|      (Verifier has NO private key!)        |\r\n");
    APP_PRINT("| [PSA] Verifier imported the public key     |\r\n");
    APP_PRINT("+--------------------------------------------+\r\n\r\n");

    /* ====================================================================
     * SIGNER SIDE - Sign message
     * ==================================================================== */
    APP_PRINT("+-- SIGNER Signs Message --------------------+\r\n");
    APP_PRINT("| Message to sign:                           |\r\n");
    APP_PRINT("| \"%s\"  |\r\n", message);
    APP_PRINT("|                                            |\r\n");

    /* Signer signs the message using private key */
    status = psa_sign_message (signer_keypair, PSA_ALG_ML_DSA, message, message_len, signature, sizeof(signature), &signature_len);
    APP_ERR_RET(PSA_SUCCESS != status, status, "Signer: psa_sign_message failed\r\n");

    APP_PRINT("| [PSA] Signed message with private key      |\r\n");
    APP_PRINT("|       Signature size: %lu bytes           |\r\n", (unsigned long)signature_len);
    APP_PRINT("+--------------------------------------------+\r\n\r\n");

    /* Transfer simulation */
    APP_PRINT("  [SIM] Signer releases MESSAGE + SIGNATURE...\r\n");

    /* ====================================================================
     * VERIFIER SIDE - Verify Signature
     * ==================================================================== */
    APP_PRINT("+-- VERIFIER Verifies Message ---------------+\r\n");

    APP_PRINT("| [SIM] Received Message:                    |\r\n");
    APP_PRINT("| \"%s\"  |\r\n", message);
    APP_PRINT("| [SIM] Received signature: %lu bytes       |\r\n", (unsigned long)signature_len);
    APP_PRINT("|                                            |\r\n");

    /* Verifier verifies using ONLY public key (no private key needed!) */
    status = psa_verify_message (verifier_public_key, PSA_ALG_ML_DSA, message, message_len, signature, signature_len);
    APP_ERR_RET(PSA_SUCCESS != status, status, "Verifier: psa_verify_message failed\r\n");

    APP_PRINT("| [PSA] Signature verified successfully!     |\r\n");
    APP_PRINT("|      (Using public key only)               |\r\n");
    APP_PRINT("|                                            |\r\n");
    APP_PRINT("| VERIFICATION RESULT:                       |\r\n");
    APP_PRINT("| - Message is authentic                     |\r\n");
    APP_PRINT("| - Message came from signer                 |\r\n");
    APP_PRINT("| - Message was not tampered                 |\r\n");
    APP_PRINT("+--------------------------------------------+\r\n\r\n");

    /* ====================================================================
     * TEST: Verify fails with modified firmware
     * ==================================================================== */
    APP_PRINT("+-- TEST: Tampered Firmware Detection -------+\r\n");

    const uint8_t tampered_message[] = "Firmware version 2.1.0 - Build 20260120"; /* Changed date */
    const size_t tampered_len = sizeof(tampered_message) - 1;

    APP_PRINT("| [SIM] Attacker modifies firmware:          |\r\n");
    APP_PRINT("| \"%s\"  |\r\n", tampered_message);
    APP_PRINT("| (Changed build date)                       |\r\n");
    APP_PRINT("|                                            |\r\n");

    status = psa_verify_message (verifier_public_key, PSA_ALG_ML_DSA, tampered_message, tampered_len, signature, signature_len);

    if (PSA_SUCCESS != status)
    {
        APP_PRINT("|-- RESULT: ---------------------------------|\r\n");
        APP_PRINT("| [PSA] Verification FAILED (as expected!)   |\r\n");
        APP_PRINT("| [SIM] Tampered message detected!           |\r\n");
    }
    else
    {
        APP_PRINT("| ERROR: Should have failed!                 |\r\n");
        APP_ERR_RET(PSA_SUCCESS != PSA_ERROR_GENERIC_ERROR, PSA_ERROR_GENERIC_ERROR, "Tamper detection failed\r\n");
    }

    APP_PRINT("+--------------------------------------------+\r\n\r\n");

    /* Cleanup */
    psa_key_id_t cleanup_list[] = {
        signer_keypair,
        verifier_public_key
    };
    status = cleanup_keys(cleanup_list, sizeof(cleanup_list) / sizeof(cleanup_list[0]));
    APP_ERR_RET(PSA_SUCCESS != status, status, "cleanup_keys failed\r\n");

    psa_reset_key_attributes(&attrs_keypair);
    psa_reset_key_attributes(&attrs_public);

    return status;
}
/***********************************************************************************************************************
* End of function mldsa_two_party_signing.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: mldsa_export_import_keypair
 *  Description  : This function is used to demonstrate two-party digital signature with backup keypair.
 *  Arguments    : bits           Key size in bits.
 *                 lifetime       Key lifetime (volatile/persistent).
 *  Return Value : PSA_SUCCESS    Upon successful operation.
 *                 Any other error code apart from PSA_SUCCESS.
 **********************************************************************************************************************/
psa_status_t mldsa_export_import_keypair(size_t bits, psa_key_lifetime_t lifetime)
{
    psa_status_t status = PSA_SUCCESS;

    /* Signing signer's keys (before and after destroy key) */
    psa_key_handle_t signer_keypair_original = 0;
    psa_key_handle_t signer_keypair_restored = 0;

    /* Public key for distribution */
    psa_key_handle_t public_key_for_verification = 0;

    /* Attributes */
    psa_key_attributes_t attrs_keypair = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_attributes_t attrs_public = PSA_KEY_ATTRIBUTES_INIT;

    /* Backup keypair */
    uint8_t keypair_backup[PSA_KEY_GEN_ML_DSA_KEY_PAIR_MAX_SIZE(PSA_KEY_BITS_ML_DSA_65)];
    size_t keypair_backup_len = 0;

    /* Public key bytes */
    uint8_t public_key_bytes[PSA_KEY_GEN_ML_DSA_PUB_KEY_SIZE(PSA_KEY_BITS_ML_DSA_65)];
    size_t public_key_len = 0;

    /* Message to be signed */
    const uint8_t msg_v1[] = "application.bin (version 1.0)";
    const size_t msg_v1_len = sizeof(msg_v1) - 1;

    const uint8_t msg_v2[] = "application.bin (version 2.0)";
    const size_t msg_v2_len = sizeof(msg_v2) - 1;

    /* Signature buffers */
    uint8_t signature_v1[PSA_ML_DSA_SIGNATURE_SIZE(PSA_KEY_BITS_ML_DSA_65)];
    size_t signature_v1_len = 0;

    uint8_t signature_v2[PSA_ML_DSA_SIGNATURE_SIZE(PSA_KEY_BITS_ML_DSA_65)];
    size_t signature_v2_len = 0;

    APP_PRINT("\r\n+============================================+\r\n");
    APP_PRINT(    "|  ML-DSA import & export keypair operation  |\r\n");
    APP_PRINT(    "+============================================+\r\n\r\n");

    /* ====================================================================
     * PHASE 1: INITIAL SETUP
     * ==================================================================== */
    APP_PRINT("+============================================+\r\n");
    APP_PRINT("| PHASE 1: Initial Signer Setup              |\r\n");
    APP_PRINT("+============================================+\r\n\r\n");

    /* ----------------------------------------------------------------
     * STEP 1: Signer generates signing keypair
     * ---------------------------------------------------------------- */
    APP_PRINT("+-- STEP 1: Generate Signing Keypair --------+\r\n");

    setup_mldsa_attrs(&attrs_keypair, MLDSA_KEY_KEYPAIR, bits, lifetime);

    status = psa_generate_key(&attrs_keypair, &signer_keypair_original);
    APP_ERR_RET(PSA_SUCCESS != status, status, "psa_generate_key failed\r\n");

    APP_PRINT("| [PSA] Signer generated ML-DSA keypair      |\r\n");
    APP_PRINT("|      (Private key + Public key)            |\r\n");
    APP_PRINT("+--------------------------------------------+\r\n\r\n");

    /* ----------------------------------------------------------------
     * STEP 2: Signer backs up keypair
     * ---------------------------------------------------------------- */
    APP_PRINT("+-- STEP 2: Export to backup keypair --------+\r\n");

    status = psa_export_key(signer_keypair_original, keypair_backup,
                           sizeof(keypair_backup), &keypair_backup_len);
    APP_ERR_RET(PSA_SUCCESS != status, status, "psa_export_key failed\r\n");

    APP_PRINT("| [PSA] Exported keypair to backup           |\r\n");
    APP_PRINT("|       Size: %lu bytes                     |\r\n", (unsigned long)sizeof(keypair_backup));
    APP_PRINT("+--------------------------------------------+\r\n\r\n");

    /* ----------------------------------------------------------------
     * STEP 3: signer publishes public key
     * ---------------------------------------------------------------- */
    APP_PRINT("+-- STEP 3: Publish Public Key --------------+\r\n");

    status = psa_export_public_key(signer_keypair_original, public_key_bytes,
                                   sizeof(public_key_bytes), &public_key_len);
    APP_ERR_RET(PSA_SUCCESS != status, status, "psa_export_public_key failed\r\n");

    APP_PRINT("| [PSA] Exported public key                  |\r\n");
    APP_PRINT("|       Size: %lu bytes                     |\r\n", (unsigned long)public_key_len);
    APP_PRINT("| [SIM] Published public key                 |\r\n");
    APP_PRINT("+--------------------------------------------+\r\n\r\n");

    /* ----------------------------------------------------------------
     * STEP 4: Sign first message version
     * ---------------------------------------------------------------- */
    APP_PRINT("+-- STEP 4: Sign Message (Version 1.0) ------+\r\n");

    APP_PRINT("| Developer submits:                         |\r\n");
    APP_PRINT("|   \"%s\"          |\r\n", msg_v1);
    APP_PRINT("|                                            |\r\n");

    status = psa_sign_message (signer_keypair_original, PSA_ALG_ML_DSA, msg_v1, msg_v1_len, signature_v1, sizeof(signature_v1), &signature_v1_len);
    APP_ERR_RET(PSA_SUCCESS != status, status, "psa_sign_message failed\r\n");

    APP_PRINT("| [PSA] Signed with private key              |\r\n");
    APP_PRINT("|       Signature: %lu bytes                |\r\n", (unsigned long)signature_v1_len);
    APP_PRINT("+--------------------------------------------+\r\n\r\n");

    /* ====================================================================
     * PHASE 2: DESTROY KEYPAIR
     * ==================================================================== */
    APP_PRINT("+============================================+\r\n");
    APP_PRINT("| PHASE 2: Destroy keypair                   |\r\n");
    APP_PRINT("+============================================+\r\n\r\n");

    status = psa_destroy_key(signer_keypair_original);
    APP_ERR_RET(PSA_SUCCESS != status, status, "psa_destroy_key failed\r\n");
    signer_keypair_original = 0;

    APP_PRINT("  [PSA] Destroy the keypair of signer successfully\r\n\r\n");

    /* ====================================================================
     * PHASE 3: RECOVERY KEYPAIR
     * ==================================================================== */
    APP_PRINT("+============================================+\r\n");
    APP_PRINT("| PHASE 3: Recovery Keypair                  |\r\n");
    APP_PRINT("+============================================+\r\n\r\n");

    /* ----------------------------------------------------------------
     * STEP 5: Signer restores keypair from backup
     * ---------------------------------------------------------------- */
    APP_PRINT("+-- STEP 5: Restore Keypair from Backup -----+\r\n");

    /* Re-setup attributes for import */
    setup_mldsa_attrs(&attrs_keypair, MLDSA_KEY_KEYPAIR, bits, lifetime);

    /* Import keypair from backup */
    status = psa_import_key(&attrs_keypair, keypair_backup,
                           sizeof(keypair_backup), &signer_keypair_restored);
    APP_ERR_RET(PSA_SUCCESS != status, status, "psa_import_key failed\r\n");

    APP_PRINT("| [SIM] Read keypair from backup             |\r\n");
    APP_PRINT("| [PSA] Imported keypair successfully        |\r\n");
    APP_PRINT("+--------------------------------------------+\r\n\r\n");

    /* ----------------------------------------------------------------
     * STEP 6: Sign new message with restored key
     * ---------------------------------------------------------------- */
    APP_PRINT("+-- STEP 6: Sign Message (Version 2.0) ------+\r\n");

    APP_PRINT("| Developer submits new version:             |\r\n");
    APP_PRINT("|   \"%s\"          |\r\n", msg_v2);
    APP_PRINT("|                                            |\r\n");

    status = psa_sign_message (signer_keypair_restored, PSA_ALG_ML_DSA, msg_v2, msg_v2_len, signature_v2, sizeof(signature_v2), &signature_v2_len);
    APP_ERR_RET(PSA_SUCCESS != status, status, "psa_sign_message failed\r\n");

    APP_PRINT("| [PSA] Signed with restored private key     |\r\n");
    APP_PRINT("|       Signature: %lu bytes                |\r\n", (unsigned long)signature_v2_len);
    APP_PRINT("+--------------------------------------------+\r\n\r\n");

    /* ----------------------------------------------------------------
     * STEP 7: Verify both signatures with public key
     * ---------------------------------------------------------------- */
    APP_PRINT("+-- STEP 7: Verify Signature Consistency ----+\r\n");

    /* Import public key for verification */
    setup_mldsa_attrs(&attrs_public, MLDSA_KEY_PUBLIC, bits, lifetime);
    status = psa_import_key (&attrs_public, public_key_bytes, sizeof(public_key_bytes), &public_key_for_verification);
    APP_ERR_RET(PSA_SUCCESS != status, status, "psa_import_key (public) failed\r\n");

    /* Verify first message */
    status = psa_verify_message (public_key_for_verification, PSA_ALG_ML_DSA, msg_v1, msg_v1_len, signature_v1, signature_v1_len);
    APP_ERR_RET(PSA_SUCCESS != status, status, "Verify v1.0 signature failed\r\n");

    APP_PRINT("| [PSA] Version 1.0 message verified         |\r\n");
    APP_PRINT("|      (Signed before recovery)              |\r\n");

    /* Verify second message */
    status = psa_verify_message(public_key_for_verification, PSA_ALG_ML_DSA,
                               msg_v2, msg_v2_len,
                               signature_v2, signature_v2_len);
    APP_ERR_RET(PSA_SUCCESS != status, status, "Verify v2.0 signature failed\r\n");

    APP_PRINT("| [PSA] Version 2.0 message verified         |\r\n");
    APP_PRINT("|     (Signed after recovery)                |\r\n");
    APP_PRINT("|                                            |\r\n");
    APP_PRINT("| VERIFICATION COMPLETE:                     |\r\n");
    APP_PRINT("| - Restored keypair is identical            |\r\n");
    APP_PRINT("| - Both old and new messages valid          |\r\n");
    APP_PRINT("| - Signing service fully operational!       |\r\n");
    APP_PRINT("+--------------------------------------------+\r\n\r\n");

    /* Cleanup */
    psa_key_id_t cleanup_list[] = {
        signer_keypair_restored,
        public_key_for_verification
    };
    cleanup_keys(cleanup_list, sizeof(cleanup_list) / sizeof(cleanup_list[0]));
    APP_ERR_RET(PSA_SUCCESS != status, status, "cleanup_keys failed\r\n");

    psa_reset_key_attributes(&attrs_keypair);
    psa_reset_key_attributes(&attrs_public);

    return status;
}
/***********************************************************************************************************************
* End of function mldsa_export_import_keypair.
***********************************************************************************************************************/
