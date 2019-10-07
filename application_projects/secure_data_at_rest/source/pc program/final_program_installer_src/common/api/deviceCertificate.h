

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


#if !defined DEVICE_CERTIFICATE_H
#define DEVICE_CERTIFICATE_H

// For use in C++
#ifdef __cplusplus
extern "C" {
#endif

#include "crypto.h"
#include "staticAssert.h"

// The Synergy security seed is 512 bits in length.
typedef uint8_t SynergySeed[64];

// Type for the unique id of the device.
typedef uint8_t UniqueId[16];

#define DEVICE_CERT_VERSION_NO    (1)
#define DEVICE_CERT_LENGTH        (337)

// Device Certificate. See Synergy Security Architecture Document.
// Public personality of the chip.
typedef struct
{
    uint8_t       version;           // Certificate version number.
    UniqueId      uniqueId;          // Opaque unique id for the chip.
    EccPublicKey  sigKey;            // Public signature key for the chip, derived from the seed.
    EccPublicKey  encKey;            // Public encryption key for the chip, derived from the seed.
    EccPublicKey  fabKey;
    EccSignature  fabKeySig;         // sig over just the fabKey.
    EccSignature  devCertSig;        // sig over all the preceding fields, signed with fabKey.
} DeviceCertificate;


#if defined _WIN32
extern       SynergySeed       synergySeed;
#else
extern const SynergySeed       synergySeed;
#endif
extern const DeviceCertificate deviceCertificate;


// The device certificate is passed (without flattening/linearisation) between the device and PC.
// Structure packing rules could be different on the different platforms.
// Checking the offsets of all the fields is tricky.
// Simply checking that the overall size of the structure is the same on both platforms is a quicker check....

STATIC_ASSERT(sizeof(DeviceCertificate) == DEVICE_CERT_LENGTH);
STATIC_ASSERT(sizeof(SynergySeed) == 64);

#ifdef __cplusplus
}
#endif

#endif

