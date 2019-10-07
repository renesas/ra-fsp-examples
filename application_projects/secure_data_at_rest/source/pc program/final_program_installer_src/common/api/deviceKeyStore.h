

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


#if !defined DEVICE_KEYSTORE_H
#define DEVICE_KEYSTORE_H

// For use in C++
#ifdef __cplusplus
extern "C" {
#endif

#include "crypto.h"
#include "staticAssert.h"


#define DEVICE_KEYSTORE_KEY_PAIR_NUM    (3)
#define DEVICE_KEYSTORE_SIZE            ((ECC_PUBLIC_KEY_SIZE + ECC_PRIVATE_KEY_SIZE) * DEVICE_KEYSTORE_KEY_PAIR_NUM)

// Device Key Structure. See Synergy Security Architecture Document.
// Key pairs used in the chip.
typedef struct
{
    EccPublicKey  pubSigKey;    // Public signature key for the chip
    EccPrivateKey privSigKey;   // Private signature key for the chip
    EccPublicKey  pubEncKey;    // Public encryption key for the chip
    EccPrivateKey privEncKey;   // Private encryption key for the chip
    EccPublicKey  pubMtKey;     // Public module table key for the chip
    EccPrivateKey privMtKey;    // Private module table key for the chip
} DeviceKeyStore;


extern const DeviceKeyStore deviceKeyStore;

// Checking that the overall size of the structure is right.

STATIC_ASSERT(sizeof(DeviceKeyStore) == DEVICE_KEYSTORE_SIZE);

#ifdef __cplusplus
}
#endif

#endif

