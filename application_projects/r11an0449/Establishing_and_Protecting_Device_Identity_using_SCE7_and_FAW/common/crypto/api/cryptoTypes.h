/***********************************************************************************************************************
* Copyright (c) 2015 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
**********************************************************************************************************************/




#if !defined CRYPTO_TYPES_H
#define CRYPTO_TYPES_H

#include <stdbool.h>
#include <stdint.h>
#include "cryptoTypes.h"
// For use in C++
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
