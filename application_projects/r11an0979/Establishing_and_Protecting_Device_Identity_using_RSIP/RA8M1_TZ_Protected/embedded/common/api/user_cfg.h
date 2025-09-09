/***********************************************************************************************************************
 * File Name    : user_cfg.h
 * Description  : application specific definitions
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef DEVCERT_H_
#define DEVCERT_H_

#include "cryptoTypes.h"

/* Memory Map of device certificate and device key */
#define ADDR_DEV_CERT      (0x020F0000)
#define SIZE_DEV_CERT       0x00001000
/** Max size of PEM format certificate.
 * This keeps the Frame Protocol message size under the max of 1030.
 * NOTE: As a result of the Frame Protocol message size limit of
 * 1030 bytes, the CA signing cert used to sign the OEM cert
 * is limited to an RSA key size of 1024 bytes.  A larger key
 * will cause the certificate size to exceed the frame protocol
 * limit. This can be addressed by breaking the PEM into 2
 * messages or increasing the frame size allowed. */
 
/* NOTE: The maximum PEM length has been increased from 1030 bytes to 1230 bytes
* due to the use of RSA 4096-bit keys, which increase the certificate size.*/
#define MAX_PEM_LENGTH   (1230)
#define DEV_CERT_PEM_LENGTH 1024  // Size of the OEMCertificatePEM structure
#define DEV_CERT_PTR       (*((volatile uint32_t *)ADDR_DEV_CERT))

typedef uint8_t PEM_CERT_BUFF[MAX_PEM_LENGTH];

/* Device Certificate */
typedef struct
{
    uint16_t            certPEMLen;       /* Length of PEM Cert */
    PEM_CERT_BUFF       certPEM;          /* PEM formatted X509 certificate generated using public key */
} DevCertificatePEM_t;

/* Device key structure as stored in firmware */
typedef struct
{
    RSAPublicKey  pubKey;           /* public key buffer */
    uint16_t key_handle;    		/* RSA key handle */
}DevKey_t;


#endif /* DEVCERT_H_ */
