#pragma once
#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <tchar.h>

#define MAX_COMMS_CONTEXTS		(3)
#define DEFAULT_BAUDRATE		(9600)

// OEM Certificate

// Max size of PEM Cert.  This keeps the Frame Protocol message
// size under the max of 1030.
// NOTE: As a result of the Frame Protocol message size limit of
// 1030 bytes, the CA signing cert used to sign the OEM cert
// is limited to an RSA key size of 1024 bytes.  A larger key
// will cause the certificate size to exceed the frame protocol
// limit. This can be addressed by breaking the PEM into 2
// messages or increasing the frame size allowed.

/* NOTE: The maximum PEM length has been increased from 1030 bytes to 1230 bytes
* due to the use of RSA 4096-bit keys, which increase the certificate size.*/
#define MAX_PEM_LENGTH			(1230)  


typedef char PEM_CERT_BUFF[MAX_PEM_LENGTH];

typedef struct
{
	uint16_t            certPEMLen;       // Length of PEM Cert
	PEM_CERT_BUFF       certPEM;          // PEM formatted X509 certificate generated using public key
} DevCertificatePEM;

typedef struct
{
	uint32_t portNumber;
	HANDLE handle;

	DevCertificatePEM *pDevCert;        // pointer to the OEM certificate injected into the device
}DeviceContext;