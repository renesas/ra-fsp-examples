

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


#if !defined OEM_CERTIFICATE_H
#define OEM_CERTIFICATE_H

// For use in C++
#ifdef __cplusplus
extern "C" {
#endif

#include "staticAssert.h"

#define ECC_PRIVATE_KEY_HNDL_SIZE 52

typedef uint8_t EccPrivateKeyHandle[ECC_PRIVATE_KEY_HNDL_SIZE];

/** Max size of PEM format certificate.
 * This keeps the Frame Protocol message size under the max of 1030.
 * NOTE: As a result of the Frame Protocol message size limit of
 * 1030 bytes, the CA signing cert used to sign the OEM cert
 * is limited to an RSA key size of 1024 bytes.  A larger key
 * will cause the certificate size to exceed the frame protocol
 * limit. This can be addressed by breaking the PEM into 2
 * messages or increasing the frame size allowed. */
#define MAX_PEM_LENGTH   (1022)
#define OEM_CERT_PEM_LENGTH 1024  // Size of the OEMCertificatePEM structure
#define OEM_CERT_MAGIC   0x9FC301A8
typedef char PEM_CERT_BUFF[MAX_PEM_LENGTH];

// OEM Certificate
typedef struct
{
    uint16_t            certPEMLen;       // Length of PEM Cert
    PEM_CERT_BUFF       certPEM;          // PEM formatted X509 certificate generated using public key
} OEMCertificatePEM;

// OEM Certificate structure as stored in firmware
typedef struct
{
    uint32_t            oem_cert_magic;   // Predefined value to determine if there is a valid certificate in flash
    EccPrivateKeyHandle privKeyHandle;    // Private key handle for wrapped private key
    OEMCertificatePEM   cert;             // PEM formatted certificate
} OEMCertificate;


// Checking that the overall size of the structure is correct

STATIC_ASSERT(sizeof(OEMCertificatePEM) == OEM_CERT_PEM_LENGTH);


#ifdef __cplusplus
}
#endif

#endif//OEM_CERTIFICATE_H

