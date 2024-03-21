/***********************************************************************************************************************
* Copyright [2015] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
*
* The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
* and/or its licensors ("Renesas") and subject to statutory and contractual protections.
*
* Unless otherwise expressly agreed in writing between Renesas and you: 1) you may not use, copy, modify, distribute,
* display, or perform the contents; 2) you may not use any name or mark of Renesas for advertising or publicity
* purposes or in connection with your use of the contents; 3) RENESAS MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE
* SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED �eAS IS�f WITHOUT ANY EXPRESS OR IMPLIED
* WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND
* NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR CONSEQUENTIAL DAMAGES,
* INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF CONTRACT OR TORT, ARISING
* OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents included in this file may
* be subject to different terms.
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
