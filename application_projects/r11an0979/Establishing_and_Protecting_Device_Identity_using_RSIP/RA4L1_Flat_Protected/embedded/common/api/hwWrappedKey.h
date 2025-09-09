/***********************************************************************************************************************
 * File Name    : hwWrappedKey.h
 * Description  : hardware SCE usage related definitions
 *  **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#ifndef HEADER_HW_WRAPPED_KEY_H
#define HEADER_HW_WRAPPED_KEY_H

#include <stdbool.h>
#include <stdint.h>


/***********************************************************************************************************************
 *  These functions are used to perform wrapped key operations including key pair generation, signing and validation
 **********************************************************************************************************************/
/* Generate an ECC keypair using the HW crypto accelerate. The private key is wrapped with the hidden root key.
* Returns true if the function successfully created a key pair and false if an error was encountered. */
bool hwCreatekey(
        uint8_t * pubkey_buffer        /* buffer where the public key is stored if the function succeeds */
    );

/* Signs a hashed value using the provided wrapped private key.
* Returns true if the function successfully created the signature and false if an error was encountered. */
bool hwSignData(
        uint8_t * hash_buffer,         /* pointer to the hash string to be signed */
        uint8_t * sig_buffer         /* buffer where the signature is stored if the function succeeds */
    );

/* Validates a hashed value using the provided public key.
* Returns true if the function successfully created the signature and false if an error was encountered. */
bool hwValidateSignature(
        uint8_t * hash_buffer,         /* pointer to the hash string used in validating the signature */
        uint8_t sig_buffer[],          /* pointer to the signature to be validated */
        uint8_t * pubkey_buffer        /* pointer to the public key */
    );
#endif /* HEADER_HW_WRAPPED_KEY_H */
