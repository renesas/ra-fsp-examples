/***********************************************************************************************************************
 * File Name    : hwWrappedKey.h
 * Description  : hardware SCE usage related definitions
 *  **********************************************************************************************************************/
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
#ifndef HEADER_HW_WRAPPED_KEY_H
#define HEADER_HW_WRAPPED_KEY_H

#include <stdbool.h>
#include <stdint.h>
#include "psa/crypto.h"


/***********************************************************************************************************************
 *  These functions are used to perform wrapped key operations including key pair generation, signing and validation
 **********************************************************************************************************************/

/* Generate an ECC keypair using the HW crypto accelerate. The private key is wrapped with the hidden root key.
* Returns true if the function successfully created a key pair and false if an error was encountered. */
bool hwCreatekey(
        uint8_t * pubkey_buffer,        /* buffer where the public key is stored if the function succeeds */
        psa_key_handle_t *key_handle  /* ECC key handle */
    );

/* Signs a hashed value using the provided wrapped private key.
* Returns true if the function successfully created the signature and false if an error was encountered. */
bool hwSignData(
        uint8_t * hash_buffer,         /* pointer to the hash string to be signed */
        uint8_t * sig_buffer,          /* buffer where the signature is stored if the function succeeds */
        psa_key_handle_t *key_handle   /* ECC key handle */
    );

/* Validates a hashed value using the provided public key.
* Returns true if the function successfully created the signature and false if an error was encountered. */
bool hwValidateSignature(
        uint8_t * hash_buffer,         /* pointer to the hash string used in validating the signature */
        uint8_t sig_buffer[],          /* pointer to the signature to be validated */
        uint8_t * pubkey_buffer        /* pointer to the public key */
    );
#endif /* HEADER_HW_WRAPPED_KEY_H */
