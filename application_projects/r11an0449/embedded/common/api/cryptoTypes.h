/***********************************************************************************************************************
 * File Name    : cryptoTypes.h
 * Description  : common cryptographic related definitions.
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

#if !defined CRYPTO_TYPES_H
#define CRYPTO_TYPES_H

#include <stdbool.h>
#include <stdint.h>
#include "cryptoTypes.h"
/* For use in C++ */
#ifdef __cplusplus
extern "C" {
#endif
    

#define AES128_KEY_SIZE           16
#define AES128_IV_SIZE            AES128_KEY_SIZE
#define HMAC_KEY_SIZE             32
#define SHA256_SIZE               32
#define ECC_PUBLIC_KEY_SIZE       64
#define ECC_PRIVATE_KEY_SIZE      32
#define ECC_SIGNATURE_SIZE        64

typedef uint8_t AesKey[AES128_KEY_SIZE];
typedef uint8_t AesIv[AES128_IV_SIZE];
typedef uint8_t HmacKey[HMAC_KEY_SIZE];
typedef uint8_t HmacResult[HMAC_KEY_SIZE];
typedef uint8_t Sha256[SHA256_SIZE];
typedef uint8_t EccPublicKey[ECC_PUBLIC_KEY_SIZE];
typedef uint8_t EccPrivateKey[ECC_PRIVATE_KEY_SIZE];
typedef uint8_t EccSignature[ECC_SIGNATURE_SIZE];

#define CRYPTO_DERIVATION_STRING_MT_SIG     "mtSig"
#define CRYPTO_DERIVATION_STRING_DEV_SIGN   "devSign"
#define CRYPTO_DERIVATION_STRING_DEV_ENC    "devEnc"

#ifdef __cplusplus
}
#endif

#endif //CRYPTO_TYPES_H
