/***********************************************************************************************************************
 * File Name    : ecc_thread_entry.c
 * Description  : Contains macros and functions used in ecc_thread_entry.c
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
#include "ecc_thread.h"
#include "rm_netx_secure_crypto.h"
#include "crypto_ep_definitions.h"
#include "common_utils.h"

/*******************************************************************************************************************//**
 * @addtogroup netx_crypto_ep
 * @{
 **********************************************************************************************************************/

/******************************************************************************
 Macro definitions
 ******************************************************************************/

#define            ECDSA_SIZE                      (sizeof(NX_CRYPTO_ECDSA))
#define            ECDH_SIZE                       (sizeof(NX_CRYPTO_ECDH))

/******************************************************************************
 Exported global functions (to be accessed by other files)
 ******************************************************************************/

/******************************************************************************
 Exported global variables
 ******************************************************************************/
extern NX_CRYPTO_METHOD crypto_method_ecdh;
extern NX_CRYPTO_METHOD crypto_method_ecdsa;
extern NX_CRYPTO_METHOD crypto_method_ec_secp256;
extern NX_CRYPTO_METHOD crypto_method_sha256;

/* ECDSA key data */
/* NIST vector */
/*
 * d = 8c14b793cb19137e323a6d2e2a870bca2e7a493ec1153b3a95feb8a4873f8d08
 Qx = 7a4e287890a1a47ad3457e52f2f76a83ce46cbc947616d0cbaa82323818a793d
 Qy = eec4084f5b29ebf29c44cce3b3059610922f8b30ea6e8811742ac7238fe87308
 *
 */
const uint8_t ECC_SECP256R1Keydata[] =
{ 0x7d,  0x7d,  0xc5,  0xf7,  0x1e,  0xb2,  0x9d,  0xda,  0xf8,  0x0d,  0x62,  0x14,  0x63,  0x2e,  0xea,  0xe0,
  0x3d,  0x90,  0x58,  0xaf,  0x1f,  0xb6,  0xd2,  0x2e,  0xd8,  0x0b,  0xad,  0xb6,  0x2b,  0xc1,  0xa5,  0x34
};
/* NIST vector */
const uint8_t ECC_SECP256R1PublicKeydata[] =
{ 0x00,  0xea,  0xd2,  0x18,  0x59,  0x01,  0x19,  0xe8,  0x87,  0x6b,  0x29,  0x14,  0x6f,  0xf8,  0x9c,  0xa6,
  0x17,  0x70,  0xc4,  0xed,  0xbb,  0xf9,  0x7d,  0x38,  0xce,  0x38,  0x5e,  0xd2,  0x81,  0xd8,  0xa6,  0xb2,
  0x30,  0x28,  0xaf,  0x61,  0x28,  0x1f,  0xd3,  0x5e,  0x2f,  0xa7,  0x00,  0x25,  0x23,  0xac,  0xc8,  0x5a,
  0x42,  0x9c,  0xb0,  0x6e,  0xe6,  0x64,  0x83,  0x25,  0x38,  0x9f,  0x59,  0xed,  0xfc,  0xe1,  0x40,  0x51,
  0x41 /* ASN1 Constant */
};
const uint8_t ECC_SECP256R1Message[] = "ASYMMETRIC_INPUT_FOR_SIGN......";

/* ECDH key data */
/* NIST vector */
/*Private key 59137e38152350b195c9718d39673d519838055ad908dd4757152fd8255c09bf */
const uint8_t ECC_SECP256R1Keydata_ecdh[] =
{ 0x59, 0x13, 0x7e, 0x38, 0x15, 0x23, 0x50, 0xb1, 0x95, 0xc9, 0x71, 0x8d, 0x39, 0x67, 0x3d, 0x51, // NOLINT(readability-magic-numbers)
  0x98, 0x38, 0x05, 0x5a, 0xd9, 0x08, 0xdd, 0x47, 0x57, 0x15, 0x2f, 0xd8, 0x25, 0x5c, 0x09, 0xbf, // NOLINT(readability-magic-numbers)
};
/* NIST vector */
/*Public key 4, a8c5fdce8b62c5ada598f141adb3b26cf254c280b2857a63d2ad783a73115f6b, 806e1aafec4af80a0d786b3de45375b517a7e5b51ffb2c356537c9e6ef227d4a*/
const uint8_t ECC_SECP256R1PublicKeydata_ecdh[] =
{ 0x04, 0xa8, 0xc5, 0xfd, 0xce, 0x8b, 0x62, 0xc5, 0xad, 0xa5, 0x98, 0xf1, 0x41, 0xad, 0xb3, 0xb2, 0x6c, // NOLINT(readability-magic-numbers)
  0xf2, 0x54, 0xc2, 0x80, 0xb2, 0x85, 0x7a, 0x63, 0xd2, 0xad, 0x78, 0x3a, 0x73, 0x11, 0x5f, 0x6b, 0x80,     // NOLINT(readability-magic-numbers)
  0x6e, 0x1a, 0xaf, 0xec, 0x4a, 0xf8, 0x0a, 0x0d, 0x78, 0x6b, 0x3d, 0xe4, 0x53, 0x75, 0xb5, // NOLINT(readability-magic-numbers)
  0x17, 0xa7, 0xe5, 0xb5, 0x1f, 0xfb, 0x2c, 0x35, 0x65, 0x37, 0xc9, 0xe6, 0xef, 0x22, 0x7d,
  0x4a, // NOLINT(readability-magic-numbers)
 };

/******************************************************************************
 Private global variables and functions
 ******************************************************************************/
static uint32_t netx_secure_crypto_ecdsa_example(void);
static uint32_t netx_secure_crypto_ecdh_example(void);

/* ECC Thread entry function */
void ecc_thread_entry(void)
{
    uint8_t read_data = RESET_VALUE;
    UINT status = TX_SUCCESS;
    ULONG actual_flags = RESET_VALUE;
    /* Initialize the RTT Thread.*/
    rtt_thread_init_check ();
    /* wait for the AES flag to get the user input from ecc thread */
    status = tx_event_flags_get (&g_user_input_event_flags, ECC_EVENT_FLAG, TX_OR_CLEAR, &actual_flags,
                                 TX_WAIT_FOREVER);
    /* Check status. */
    if ((TX_SUCCESS == status) && (ECC_EVENT_FLAG == actual_flags))
    {
        /* Get the user input data from the previous ecc thread */
        status = tx_queue_receive (&g_user_input_queue, (VOID*) &read_data, TX_WAIT_TIME);
        if (TX_SUCCESS != status)
        {
            PRINT_ERR_STR("thread receive failed");
            ERROR_TRAP(status);
        }
    }
    /* Forward the user input data to RSA thread */
    status = tx_queue_send (&g_user_input_queue, (uint8_t*) &read_data, TX_WAIT_FOREVER);
    /* Set RSA event flag */
    status = tx_event_flags_set (&g_user_input_event_flags, RSA_EVENT_FLAG, TX_OR);
    tx_thread_sleep (THREAD_SLEEP_TIME);

    /* verify the user input to run the ep with event flag enable */
    if (EVENT_FLAG_ENABLE == read_data)
    {
        while (true)
        {
            /* Wait for ECC event flag */
            status = tx_event_flags_get (&g_netx_crypto_event_flags, ECC_EVENT_FLAG, TX_OR_CLEAR, &actual_flags, TX_WAIT_TIME);
            /* Check status. */
            if ((TX_SUCCESS == status) && (ECC_EVENT_FLAG == actual_flags))
            {
                /* To know the time taken for an execution of each crypto algorithms and user can utilise the time variables
                 * by taking the difference of consecutive time arrays will give the exact time taken for execution */
                ULONG time[THREE] = { RESET_VALUE };
                PRINT_INFO_STR("_______________________netx crypto ECC demo start_________________________");
                /* Get the current time: */
                time[ZERO] = tx_time_get ();
                /* Not currently using the time variable. User can use for printing the time to check the execution time
                 * each algorithm */
                FSP_PARAMETER_NOT_USED(time);
                /* Call ecdsa example function to execute crypto algorithm*/
                status = netx_secure_crypto_ecdsa_example ();
                if (NX_CRYPTO_SUCCESS != status)
                {
                    PRINT_ERR_STR("netx_secure_crypto_ecdsa_example is failed");
                    ERROR_TRAP(status);
                }
                /* Get the current time in 'time1' after executing the ecdsa example.
                 * By calculating the difference of time1 & time will get the actual execution time */
                time[ONE] = tx_time_get ();

                /* thread sleep */
                tx_thread_sleep (THREAD_SLEEP_TIME);

                /* call ecdh example function to execute crypto algorithm*/
                status = netx_secure_crypto_ecdh_example ();
                if (NX_CRYPTO_SUCCESS != status)
                {
                    PRINT_ERR_STR("netx_secure_crypto_ecdh_example is failed");
                    ERROR_TRAP(status);
                }
                /* Get the time after executing the ecdh example.
                 * By calculating the time difference of time2 & time1 will get the actual execution time period */
                time[TWO] = tx_time_get ();

                /* Set AES event flag to wakeup aes thread. */
                status = tx_event_flags_set (&g_netx_crypto_event_flags, RSA_EVENT_FLAG, TX_OR);
                PRINT_INFO_STR("_______________________netx crypto ECC demo end____________________________");

                tx_thread_sleep (THREAD_SLEEP_TIME);

            }
        }
    }
    /* Event based thread switching is disabled */
    else if(EVENT_FLAG_DISABLE == read_data) //Event based thread switching is disabled
    {
        while (true)
        {
            /* Call ecdsa example function to execute crypto algorithm*/
            status = netx_secure_crypto_ecdsa_example ();
            if (NX_CRYPTO_SUCCESS != status)
            {
                PRINT_ERR_STR("netx_secure_crypto_ecdsa_example is failed");
                ERROR_TRAP(status);
            }

            tx_thread_sleep (THREAD_SLEEP_TIME);
            /* call ecdh example function to execute crypto algorithm*/
            netx_secure_crypto_ecdh_example ();
            if (NX_CRYPTO_SUCCESS != status)
            {
                PRINT_ERR_STR("netx_secure_crypto_ecdh_example is failed");
                ERROR_TRAP(status);
            }
            tx_thread_sleep (THREAD_SLEEP_TIME);
        }
    }
    else
    {
        /* do nothing */
    }
}

/*******************************************************************************************************************//**
 * @brief     This is an example function os ECDSA on using Netx crypto API to sign and verify input message data.
 * @param[IN] None
 * @retval    X_CRYPTO_SUCCESS or Any Other Error code apart from NX_CRYPTO_SUCCESS upon unsuccessful execution.
 **********************************************************************************************************************/
static uint32_t netx_secure_crypto_ecdsa_example(void)
{
    uint8_t metadata[ECDSA_SIZE] = {RESET_VALUE};
    uint32_t metadata_size = ECDSA_SIZE;
    uint32_t status = NX_CRYPTO_SUCCESS;
    ULONG sig_length = RESET_VALUE;
    NX_CRYPTO_EXTENDED_OUTPUT extended_output;
    void *handler = NX_CRYPTO_NULL;
    uint8_t output[RM_NETX_SECURE_CRYPTO_EXAMPLE_OUTPUT_BUFFER_SIZE] = { RESET_VALUE };

    /* Setup the platform; initialize the SCE and the TRNG */
    status = _nx_crypto_initialize ();
    if (NX_CRYPTO_SUCCESS != status)
    {
        PRINT_ERR_STR("_nx_crypto_initialize in ECDSA is failed");
        return status;
    }
    PRINT_INFO_STR("ECDSA Algorithm initialized and started successfully...");
    /* Call the crypto initialization function.  */
    status = _nx_crypto_method_ecdsa_init (&crypto_method_ecdsa, NX_CRYPTO_NULL, NX_CRYPTO_NULL, &handler, metadata, metadata_size);
    if (NX_CRYPTO_SUCCESS != status)
    {
        PRINT_ERR_STR("_nx_crypto_method_ecdsa_init of initialization is failed");
        return status;
    }

    /* Set hash method.  */
    status = _nx_crypto_method_ecdsa_operation (NX_CRYPTO_HASH_METHOD_SET, handler, &crypto_method_ecdsa,
                                                NX_CRYPTO_NULL,
                                                NX_CRYPTO_NULL, (uint8_t*) &crypto_method_sha256, sizeof(NX_CRYPTO_METHOD*),
                                                NX_CRYPTO_NULL,
                                                NX_CRYPTO_NULL, NX_CRYPTO_NULL, metadata, metadata_size,
                                                NX_CRYPTO_NULL,
                                                NX_CRYPTO_NULL);

    if (NX_CRYPTO_SUCCESS != status)
    {
        PRINT_ERR_STR("_nx_crypto_method_ecdsa_operation of set hash method is failed");
        return status;
    }

    /* Set EC curve.  */
    status = _nx_crypto_method_ecdsa_operation (NX_CRYPTO_EC_CURVE_SET, handler, &crypto_method_ecdsa,
                                                NX_CRYPTO_NULL,
                                                NX_CRYPTO_NULL, (uint8_t*) &crypto_method_ec_secp256, sizeof(NX_CRYPTO_METHOD*),
                                                NX_CRYPTO_NULL,
                                                NX_CRYPTO_NULL, NX_CRYPTO_NULL, metadata, metadata_size,
                                                NX_CRYPTO_NULL,
                                                NX_CRYPTO_NULL);

    if (NX_CRYPTO_SUCCESS != status)
    {
        PRINT_ERR_STR("_nx_crypto_method_ecdsa_operation of set EC curve is failed");
        return status;
    }
    /* assigning the initialzed output data of max length */
    extended_output.nx_crypto_extended_output_data = output;
    extended_output.nx_crypto_extended_output_length_in_byte = sizeof(output);

    /* Sign the hash data using ECDSA. */
    status = _nx_crypto_method_ecdsa_operation (NX_CRYPTO_SIGNATURE_GENERATE, handler, &crypto_method_ec_secp256,
                                                (uint8_t*) ECC_SECP256R1Keydata, sizeof(ECC_SECP256R1Keydata) << 3,
                                                (uint8_t*) ECC_SECP256R1Message, sizeof(ECC_SECP256R1Message),
                                                NX_CRYPTO_NULL,
                                                (uint8_t*) &extended_output, sizeof(extended_output), metadata,
                                                metadata_size,
                                                NX_CRYPTO_NULL,
                                                NX_CRYPTO_NULL);

    if (NX_CRYPTO_SUCCESS != status)
    {
        PRINT_ERR_STR("_nx_crypto_method_ecdsa_operation of sign hash data  is failed");
        return status;
    }
    /* generated output data of actual length */
    sig_length = extended_output.nx_crypto_extended_output_actual_size;

    /* Verify the generated signature. */
    status = _nx_crypto_method_ecdsa_operation (NX_CRYPTO_SIGNATURE_VERIFY, handler, &crypto_method_ec_secp256,
                                                (uint8_t*) ECC_SECP256R1PublicKeydata,
                                                sizeof(ECC_SECP256R1PublicKeydata) << 3,
                                                (uint8_t*) ECC_SECP256R1Message, sizeof(ECC_SECP256R1Message),
                                                NX_CRYPTO_NULL,
                                                output, sig_length, metadata, metadata_size,
                                                NX_CRYPTO_NULL,
                                                NX_CRYPTO_NULL);

    if (NX_CRYPTO_SUCCESS != status)
    {
        PRINT_ERR_STR("_nx_crypto_method_ecdsa_operation to verify generated signature is failed");
        return status;
    }

    PRINT_INFO_STR("ECDSA Algorithm executed successfully and generated signature is verified!");
    /* cleanup the ecdsa metadata */
    status = _nx_crypto_method_ecdsa_cleanup(metadata);
    if (NX_CRYPTO_SUCCESS != status)
    {
        PRINT_ERR_STR("_nx_crypto_method_ecdsa_cleanup in ecdsa is failed");
        return status;
    }
    return status;
}


/*******************************************************************************************************************//**
 * @brief     This is an example function on using Netx crypto API to generate a shared secret using a ECDH.
 * @param[IN] None
 * @retval    X_CRYPTO_SUCCESS or Any Other Error code apart from NX_CRYPTO_SUCCESS upon unsuccessful execution.
 **********************************************************************************************************************/
static uint32_t netx_secure_crypto_ecdh_example(void)
{
    uint8_t metadata[ECDH_SIZE] = {RESET_VALUE};
    uint32_t metadata_size = ECDH_SIZE;
    uint32_t status = NX_CRYPTO_SUCCESS;
    uint8_t local_public_key[RM_NETX_SECURE_CRYPTO_EXAMPLE_OUTPUT_BUFFER_SIZE] = { RESET_VALUE };
    uint32_t local_public_key_len = RESET_VALUE;
    uint8_t shared_secret[RM_NETX_SECURE_CRYPTO_EXAMPLE_OUTPUT_BUFFER_SIZE] = { RESET_VALUE };
    uint32_t shared_secret_len = RESET_VALUE;
    uint8_t output[RM_NETX_SECURE_CRYPTO_EXAMPLE_OUTPUT_BUFFER_SIZE] = { RESET_VALUE };
    NX_CRYPTO_EXTENDED_OUTPUT extended_output;

    /* Setup the platform; initialize the SCE and the TRNG */
    status = _nx_crypto_initialize ();
    if (NX_CRYPTO_SUCCESS != status)
    {
        PRINT_ERR_STR("_nx_crypto_initialize in ECDH is failed");
        return status;
    }
    PRINT_INFO_STR("ECDH Algorithm initialized and started successfully...");

    /* Call the crypto initialization function.  */
    status = _nx_crypto_method_ecdh_init (&crypto_method_ecdh, NX_CRYPTO_NULL, NX_CRYPTO_NULL, NX_CRYPTO_NULL, metadata,
                                          metadata_size);

    if (NX_CRYPTO_SUCCESS != status)
    {
        PRINT_ERR_STR("_nx_crypto_method_ecdh_init in ECDH is failed");
        return status;
    }

    /* Set EC curve.  */
    status = _nx_crypto_method_ecdh_operation (NX_CRYPTO_EC_CURVE_SET,
                                               NX_CRYPTO_NULL,
                                               &crypto_method_ecdh,
                                               NX_CRYPTO_NULL,
                                               NX_CRYPTO_NULL, (uint8_t*) &crypto_method_ec_secp256, sizeof(NX_CRYPTO_METHOD*),
                                               NX_CRYPTO_NULL,
                                               NX_CRYPTO_NULL, NX_CRYPTO_NULL, metadata, metadata_size,
                                               NX_CRYPTO_NULL,
                                               NX_CRYPTO_NULL);
    if (NX_CRYPTO_SUCCESS != status)
    {
        PRINT_ERR_STR("_nx_crypto_method_ecdh_operation in ECDH is failed");
        return status;
    }

    /* Generate local public key. This will generate a key pair.
     * The private wrapped key will be held by the ecdh context and the public key (local_public_key)
     * will be returned for sharing with the peer.
     */
    extended_output.nx_crypto_extended_output_data = local_public_key;
    extended_output.nx_crypto_extended_output_length_in_byte = sizeof(local_public_key);
    status = _nx_crypto_method_ecdh_operation (NX_CRYPTO_DH_SETUP,
                                               NX_CRYPTO_NULL,
                                               &crypto_method_ecdh,
                                               NX_CRYPTO_NULL,
                                               NX_CRYPTO_NULL,
                                               NX_CRYPTO_NULL,
                                               NX_CRYPTO_NULL,
                                               NX_CRYPTO_NULL,
                                               (uint8_t*) &extended_output, sizeof(extended_output), metadata,
                                               metadata_size,
                                               NX_CRYPTO_NULL,
                                               NX_CRYPTO_NULL);
    if (NX_CRYPTO_SUCCESS != status)
    {
        PRINT_ERR_STR("_nx_crypto_method_ecdh_operation in ECDH is failed");
        return status;
    }
    /* actual key length of local public key */
    local_public_key_len = extended_output.nx_crypto_extended_output_actual_size;

    /* Calculate shared secret using the test (peer's) public key.  */
    extended_output.nx_crypto_extended_output_data = shared_secret;
    extended_output.nx_crypto_extended_output_length_in_byte = sizeof(shared_secret);
    status = _nx_crypto_method_ecdh_operation (NX_CRYPTO_DH_CALCULATE,
                                               NX_CRYPTO_NULL,
                                               &crypto_method_ecdh,
                                               NX_CRYPTO_NULL,
                                               NX_CRYPTO_NULL, (uint8_t*) ECC_SECP256R1PublicKeydata_ecdh,
                                               sizeof(ECC_SECP256R1PublicKeydata_ecdh),
                                               NX_CRYPTO_NULL,
                                               (uint8_t*) &extended_output, sizeof(extended_output), metadata,
                                               metadata_size,
                                               NX_CRYPTO_NULL,
                                               NX_CRYPTO_NULL);
    if (NX_CRYPTO_SUCCESS != status)
    {
        PRINT_ERR_STR("_nx_crypto_method_ecdh_operation in ECDH is failed");
        return status;
    }
    /* actual length of shared secret key */
    shared_secret_len = extended_output.nx_crypto_extended_output_actual_size;
    /* cleanup the ecdh metadata */
    status = _nx_crypto_method_ecdh_cleanup (metadata);
    if (NX_CRYPTO_SUCCESS != status)
    {
        PRINT_ERR_STR("_nx_crypto_method_ecdh_cleanup in ECDH is failed");
        return status;
    }

    /* Verify. The below operations will be carried out by the peer.  */

    /* Call the crypto initialization function.  */
    status = _nx_crypto_method_ecdh_init (&crypto_method_ecdh, NX_CRYPTO_NULL, NX_CRYPTO_NULL, NX_CRYPTO_NULL, metadata,
                                          metadata_size);
    if (NX_CRYPTO_SUCCESS != status)
    {
        PRINT_ERR_STR("_nx_crypto_method_ecdh_init in ECDH is failed");
        return status;
    }

    /* Set EC curve.  */
    status = _nx_crypto_method_ecdh_operation (NX_CRYPTO_EC_CURVE_SET,
    NX_CRYPTO_NULL,
                                               &crypto_method_ecdh,
                                               NX_CRYPTO_NULL,
                                               NX_CRYPTO_NULL, (uint8_t*) &crypto_method_ec_secp256, sizeof(NX_CRYPTO_METHOD*),
                                               NX_CRYPTO_NULL,
                                               NX_CRYPTO_NULL,NX_CRYPTO_NULL, metadata, metadata_size,
                                               NX_CRYPTO_NULL,
                                               NX_CRYPTO_NULL);
    if (NX_CRYPTO_SUCCESS != status)
    {
        PRINT_ERR_STR("_nx_crypto_method_ecdh_operation in ECDH is failed");
        return status;
    }

    /* Import the test private key. The peer could generate its own key pair,
     * in this example a test private key is used for simplicity. */
    status = _nx_crypto_method_ecdh_operation (NX_CRYPTO_DH_KEY_PAIR_IMPORT, NX_CRYPTO_NULL, &crypto_method_ecdh,
                                               (uint8_t*) ECC_SECP256R1Keydata_ecdh,
                                               (NX_CRYPTO_KEY_SIZE) (sizeof(ECC_SECP256R1Keydata_ecdh) << 3),
                                               (uint8_t*) ECC_SECP256R1PublicKeydata_ecdh,
                                               sizeof(ECC_SECP256R1PublicKeydata_ecdh),
                                               NX_CRYPTO_NULL,
                                               NX_CRYPTO_NULL, NX_CRYPTO_NULL, metadata, metadata_size,
                                               NX_CRYPTO_NULL,
                                               NX_CRYPTO_NULL);
    if (NX_CRYPTO_SUCCESS != status)
    {
        PRINT_ERR_STR("_nx_crypto_method_ecdh_operation in ECDH is failed");
        return status;
    }

    /* Calculate the shared secret using the local public key generated above and shared with the peer.  */
    extended_output.nx_crypto_extended_output_data = output;
    extended_output.nx_crypto_extended_output_length_in_byte = sizeof(output);
    status = _nx_crypto_method_ecdh_operation (NX_CRYPTO_DH_CALCULATE,
    NX_CRYPTO_NULL,
                                               &crypto_method_ecdh,
                                               NX_CRYPTO_NULL,
                                               NX_CRYPTO_NULL, local_public_key, local_public_key_len,
                                               NX_CRYPTO_NULL,
                                               (uint8_t*) &extended_output, sizeof(extended_output), metadata,
                                               metadata_size,
                                               NX_CRYPTO_NULL,
                                               NX_CRYPTO_NULL);
    if (NX_CRYPTO_SUCCESS != status)
    {
        PRINT_ERR_STR("_nx_crypto_method_ecdh_operation in ECDH is failed");
        return status;
    }
    /* Validate the output. Both the parties must generate the same shared secret  */
    status = (extended_output.nx_crypto_extended_output_actual_size != shared_secret_len);
    if (NX_CRYPTO_SUCCESS != status)
    {
        PRINT_ERR_STR("generated the same shared secret failed");
        return status;
    }

    status = (uint32_t) memcmp (output, shared_secret, extended_output.nx_crypto_extended_output_actual_size);
    if (NX_CRYPTO_SUCCESS != status)
    {
        PRINT_ERR_STR("shared secret and extended output in ECDH is mis-matched");
        return status;
    }

    PRINT_INFO_STR("ECDH Algorithm executed successfully and Both the keys matched successfully!");
    /* clean up the ecdh metadata */
    status = _nx_crypto_method_ecdh_cleanup (metadata);
    if (NX_CRYPTO_SUCCESS != status)
    {
        PRINT_ERR_STR("_nx_crypto_method_ecdh_cleanup in ECDH is failed");
        return status;
    }
    return status;
}

/*******************************************************************************************************************//**
 * @} (end defgroup netx_crypto_ep)
 **********************************************************************************************************************/

