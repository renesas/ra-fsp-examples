/***********************************************************************************************************************
* Copyright (c) 2015 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/


#include <Windows.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>

#include "framedProtocolHost.h"
#include "crypto.h"
#include "framedProtocolCommon.h"
#include "CertAuth.h"
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>
#include "provisioning_config.h"

#define CA_CERT_MAX_SIZE 1500

extern "C" bool sha256_calc_hash(const uint8_t *pData, uint32_t length, uint8_t *pHash);
extern "C" int uECC_verify(const uint8_t public_key[], const uint8_t hash[], const uint8_t signature[]);

static DeviceContext commsContexts[MAX_COMMS_CONTEXTS];
static uint32_t numCommsContexts = 0;

// Setup the key used for signing certificates.  I.e., this is the CA function. A self-signed 
// certificate is currently used.  
RSA *signingKey;
CertAuth *CA;
char rootCA[CA_CERT_MAX_SIZE];

static void initializeContext(DeviceContext *pContext)
{
	pContext->handle = INVALID_HANDLE_VALUE;
	pContext->portNumber = 0;
}

/*
Open a serial connection to the specified comm port number
Store info in the context
*/
static bool openConnection(DeviceContext *pContext)
{
	bool retval = false;

	fprintf(stdout, "Initialising connection to COM port [%d]\n", pContext->portNumber);
	pContext->handle = OpenSerialPort(pContext->portNumber, DEFAULT_BAUDRATE);
	if (INVALID_HANDLE_VALUE != pContext->handle)
	{
		fprintf(stdout, "Initialised connection to COM port [%d] OK\n", pContext->portNumber);
		retval = true;
	}
	else
	{
		fprintf(stderr, "Failed to open device comnnection via COM port [%d]\n", pContext->portNumber);
	}

	return retval;
}

static void freeContext(DeviceContext *pContext)
{
#if 0
	if (pContext->pCert)
	{
		free(pContext->pCert);
	}
#endif
	if (pContext->pDevCert)
	{
		free(pContext->pDevCert);
	}

	if (pContext->handle)
	{
		fpCloseCommPort(pContext->handle);
	}
}

static bool genSigningKey(void)
{
	int returnVal = 0;
	uint16_t certLen;

	signingKey = RSA_new();
	BIGNUM *exponent = BN_new();
	if ((NULL == signingKey) || (NULL == exponent))
	{
		printf("Error allocating memory for signing key\n");
		exit(-1);
	}

	// NOTE: As a result of the Frame Protocol message size limit of  1030 bytes, the CA signing cert used to 
	// sign the OEM cert is limited to an RSA key size of 1024 bytes.  A larger key will cause the certificate 
	// size to exceed the frame protocol limit. This can be addressed by breaking the PEM into 2 messages or 
	// increasing the frame size allowed.
	BN_set_word(exponent, RSA_F4);
	returnVal = RSA_generate_key_ex(signingKey, 1024, exponent, NULL);
	if (!returnVal)
	{
		printf("Error generating signing key\n");
		exit(-1);
	}

	CA = new CertAuth();
	returnVal = CA->CreateCert(signingKey, NULL, "rootCA", rootCA, CA_CERT_MAX_SIZE, &certLen);
	if (-1 == returnVal)
	{
		printf("Error creating root CA certificate. Unable to create and program OEM certificates.\n");
		exit(-1);
	}

	return returnVal;
}

static void dumpKey(uint8_t *keyData, uint16_t len)
{
	printf("Dev.PubKey.Len: %d", len);
	for (uint32_t i = 0; i < len; i++)
	{
		if (0 == i % 16)
		{
			printf("\nDev.PubKey: ");
		}
		printf("0x%02x ", keyData[i]);
	}
	printf("\n");
}

static void CreateRandomChallengeString(char *result, uint8_t maxLen)
{
	const int minLen = 10;
	int len = rand() * (maxLen - minLen) / RAND_MAX + minLen;
	for (int i = 0; i < len; i++)
		result[i] = (char)(rand() * (126 - 32) / RAND_MAX + 32);
	result[len] = 0;
}

static EC_KEY *pkey_get_eckey(EVP_PKEY *key, EC_KEY **eckey)
{
	EC_KEY *dtmp;
	if (!key) return NULL;
	dtmp = EVP_PKEY_get1_EC_KEY(key);
	EVP_PKEY_free(key);
	if (!dtmp) return NULL;
	if (eckey)
	{
		EC_KEY_free(*eckey);
		*eckey = dtmp;
	}
	return dtmp;
}


static bool ChallengeDeviceIdentity(DeviceContext *pContext)
{
	// Generate a random “challenge string” then compute the SHA256 hash of the challenge string.
	char ChallengeString[132] = { 0 };

	CreateRandomChallengeString(ChallengeString, 131);

	uint8_t challengeHashed[SHA256_SIZE];
	bool status = sha256_calc_hash((const uint8_t *)ChallengeString, (uint32_t)strlen(ChallengeString), challengeHashed);

	/* Use the ChallengeResponse command to send the hashed value to the device and get back the signature. The device
	will sign this hash value using the private key and send the signature back inside the response message. */
	uint8_t *pSignature;
	pSignature = fpSendChallengeResponseMessage(pContext->handle, challengeHashed, SHA256_SIZE);
	if (NULL == pSignature)
	{
		status = false;
		fprintf(stdout, "fpSendChallengeResponseMessage() failed!\n");
	}
	
	//Extract the public key from the Device Cert and use it to validate the returned signature
	X509 *x509;
	EVP_PKEY *pkey;
	EC_KEY   *ecKey;
	uint32_t devicekeyLength;
	uint8_t *deviceKey, *deviceKey2;
	int verified = false;

	BIO *bio_mem = BIO_new(BIO_s_mem());
	BIO_puts(bio_mem, pContext->pDevCert->certPEM);
	if (!(x509 = PEM_read_bio_X509(bio_mem, NULL, NULL, NULL)))
	{
		fprintf(stderr, "Failed to extract x509 certificate from OEM Certificate\n");
	}
	else if (!(pkey = X509_get_pubkey(x509)))
	{
		fprintf(stderr, "Failed to extract public key from OEM Certificate\n");
	}
	else if (!(ecKey = pkey_get_eckey(pkey, NULL)))
	{
		fprintf(stderr, "Failed to convert public key from OEM Certificate into EC_KEY format\n");
	}
	else
	{
		devicekeyLength = i2o_ECPublicKey(ecKey, NULL);
		deviceKey = (uint8_t *)malloc(devicekeyLength);
		deviceKey2 = deviceKey;
		if (!i2o_ECPublicKey(ecKey, &deviceKey2))
		{
			fprintf(stderr, "Failed to convert public key from OEM Certificate into Octet string format\n");
		}
		else
		{
			// Device Key has a 1 byte header that the uECC functions don't use. 
			verified = uECC_verify((const uint8_t *)&deviceKey[1], challengeHashed, pSignature);
		}
	}
	
	if (pSignature != NULL)
		free(pSignature);

	if (verified)
		return true;
	else
		return false;
}

static void dumpPEM(char *pemData, uint16_t len)
{
	printf("CERT PEM Len: %d", len);
	for (uint32_t i = 0; i < len; i++)
	{
		if (0 == i % 16)
		{
			printf("\n[%d]  OEMCert.%s: ", i, "PEM");
		}
		printf("0x%02x ", pemData[i]);
	}
	printf("\n");
}

static bool genAndProgramDevCert(DeviceContext *pContext)
{
	bool retval = true;
	BIO *bio_err = NULL;
	bio_err = BIO_new_fp(stderr, BIO_NOCLOSE);

	// allocte memory for the OEM cert. This is freed when the context is freed.
	pContext->pDevCert = new DevCertificatePEM();

	printf("Issuing Generate Key command to device\n");
	uint32_t devicekeyLength; // Should get back 64 bytes
	/* Send Generate Key command to device and check the device response. */
	uint8_t *dk = (uint8_t*)fpGetDeviceKey(pContext->handle, &devicekeyLength);
	if (NULL == dk)
	{
		printf("Failed to retrieve public key from the device\r\n");
		return false;
	}

	// The raw device Key has a 1 byte header with the format.  Prepend that as it is not supplied by the RA device. 
	uint8_t *deviceKey = (uint8_t *)malloc(65);
	/* The first byte defines the form of the conversion. For details, refer to the OpenSSL source code of EC_POINT_oct2point*/
	deviceKey[0] = POINT_CONVERSION_UNCOMPRESSED;
	memcpy(&deviceKey[1], dk, 64);
	//dumpKey(deviceKey, 65);

	EC_GROUP *group = NULL;
	EC_POINT *pt = NULL;
	EC_KEY   *devKey = NULL;

	group = EC_GROUP_new_by_curve_name(NID_X9_62_prime256v1);

	pt = EC_POINT_new(group);

	int success = EC_POINT_oct2point(group, pt, deviceKey, 65, NULL);
	if (!success)
	{
		fprintf(stdout, "EC_POINT_oct2point failed to convert ocket to EC_POINT\n");
		ERR_print_errors(bio_err);
		retval = false;
	}

	devKey = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);

	success = EC_KEY_set_public_key(devKey, pt);
	if (!success)
	{
		fprintf(stdout, "EC_KEY_set_public_key failed to convert EC_POINT to EC_KEY\n");
		ERR_print_errors(bio_err);
		retval = false;
	}

	if (retval)
	{	
		// Create the cert signed by signing key (initialized in main)
		fprintf(stdout, "Create the cert signed by signing key (local CA instance)\n");
		if (-1 == CA->CreateCert(signingKey, devKey, "DeviceCert.pem", (char *)pContext->pDevCert->certPEM, sizeof(pContext->pDevCert->certPEM), (uint16_t *)&pContext->pDevCert->certPEMLen))
		{
			fprintf(stderr, "ERROR: Failed to create OEM Device Certificate\n");
			retval = false;
		}
		else if (pContext->pDevCert->certPEMLen > sizeof(DevCertificatePEM))
		{
			fprintf(stderr, "ERROR: Certificate size exceeds max length support by the device. Cert size =%d\n", pContext->pDevCert->certPEMLen);
			retval = false;
		}
		else
		{		
#if 1
			//Issue a challenge and validate the response before issuing Device Certificate
			bool resp = ChallengeDeviceIdentity(pContext);
			if (resp)
				printf("Successful challenge/response \n");
			else
			{
				printf("Challenge/response failed\n");
				return resp;
			}
#endif
			// Send the certificate to the device
			fprintf(stdout, "Sending device Certificate to the device, len = %d\n", pContext->pDevCert->certPEMLen);
			retval = fpSendDeviceCert(pContext->handle, (const uint8_t *)pContext->pDevCert, (const uint16_t)pContext->pDevCert->certPEMLen);
		}
	}

	// The devKey was allocated in the OpenSSL o2i_ECPublicKey function.Free it here.Also delete the CA instance.
	if (devKey)
		OPENSSL_free(devKey);
	if (CA)
		delete CA;

	return retval;
}

		
int main(const int argc, const char *argv[])
{
	bool retval = true;
	uint32_t port = 0;
	uint32_t index = 0;
	uint32_t devKeyLen = 0;
	uint8_t *devPubKey = NULL;
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

	//Initialize serial communication protocol
	fpInitialize();

	//argv[1] = "connect";
	//argv[2] = "28";
	printf("\r\n argc = %d, argv[1] = %s, argv[2] = %s\r\n", argc, argv[1], argv[2]);

	if ((3 == argc) && (0 == strcmp(argv[1], "connect")))
	{
		//scan for serial port connection.
		printf("Scanning for devices on port %s\n", argv[2]);

		for (uint32_t i = 0; (i < strlen(argv[2])) && (numCommsContexts < MAX_COMMS_CONTEXTS); i++)
		{
			//If character is not, add it to the port value.
			if (',' != argv[2][i])
			{
				port = port * 10 + argv[2][i] - '0';
			}

			// if , or end of string, then apply the port number that has been decoded.
			if ((',' == argv[2][i]) || (i == strlen(argv[2]) - 1))
			{
				initializeContext(&commsContexts[numCommsContexts]);
				commsContexts[numCommsContexts].portNumber = port;
				port = 0;
				numCommsContexts++;
			}
		}

		if (numCommsContexts >= MAX_COMMS_CONTEXTS)
		{
			printf("[Deploy] FAILED - too many (%d>%d) comms contexts requested\n", numCommsContexts, MAX_COMMS_CONTEXTS);
			retval = false;
		}
		else
		{
			for (index = 0; index < numCommsContexts; index++)
			{
				if (!openConnection(&commsContexts[index]))
				{
					printf("[Deploy] FAILED - unable to open com port %d\n", commsContexts[index].portNumber);
					retval = false;
					return -1;
				}
			}

			index = 0;		
			
			/* Generate key pair to be used for signing certificates */
			if (genSigningKey())
			{
				printf("Local Key generation FAILED!!!\n");
				retval = false;
			}

			if (!genAndProgramDevCert(&commsContexts[index]))
			{
				printf("Error created and programmed Device Cert into device\n");
				retval = false;
			}
			else
			{
				printf("Device Cert succesfully created and programmed into device\n");
			}
		}
	}

	for (uint32_t i = 0; i < numCommsContexts; i++)
	{
		freeContext(&commsContexts[i]);
	}

	return retval ? 0 : -1;
}
