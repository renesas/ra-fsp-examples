/***********************************************************************************************************************
 * File Name    : cryptoTypes.h
 * Description  : common cryptographic related definitions.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
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
    

#define SHA256_SIZE               32
#define RSA_PUBLIC_KEY_SIZE       516
#define RSA_PRIVATE_KEY_SIZE      2048
#define RSA_SIGNATURE_SIZE        512

typedef uint8_t Sha256[SHA256_SIZE];
typedef uint8_t RSAPublicKey[RSA_PUBLIC_KEY_SIZE];
typedef uint8_t RSAPrivateKey[RSA_PRIVATE_KEY_SIZE];
typedef uint8_t RSASignature[RSA_SIGNATURE_SIZE];

#define CRYPTO_DERIVATION_STRING_MT_SIG     "mtSig"
#define CRYPTO_DERIVATION_STRING_DEV_SIGN   "devSign"
#define CRYPTO_DERIVATION_STRING_DEV_ENC    "devEnc"

#ifdef __cplusplus
}
#endif

#endif //CRYPTO_TYPES_H
