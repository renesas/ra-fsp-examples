/**********************************************************************************************************************
* Copyright (c) 2017 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/

/*******************************************************************************
* File Name    : CertAuth.cpp
* Description  : A simple example Certificate Authority providing rudimentary
*				 functionality
*
******************************************************************************/

/*****************************************************************************
* History : DD.MM.YYYY Version  Description
*
*         : 25.04.2017 1.00     First Release
******************************************************************************/

/******************************************************************************
Includes
******************************************************************************/
//#include "StdAfx.h"
#include <Windows.h>
#include "CertAuth.h"
#include "util.h"

#include <openssl/x509.h>
#include <openssl/pem.h>

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Exported global variables (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/

util utilobj;



CertAuth::CertAuth(void)
{
}


CertAuth::~CertAuth(void)
{
}

int certExpireDays = 3650;   // Expiration 10 years?

// If Device key is NULL, this is a root cert
int CertAuth::CreateCert(RSA *rootKeyPair, EC_KEY *deviceKey, char *outputFile, char *returnCert, int returnCertMaxLength, uint16_t *pemLength)
{
	X509 *cert;
	bool rootCert = (deviceKey == NULL);
    *pemLength = 0;
	cert = X509_new();

	// Set serial number
	if (rootCert)
		ASN1_INTEGER_set(X509_get_serialNumber(cert), 1);
	else
		ASN1_INTEGER_set(X509_get_serialNumber(cert), nextDeviceSerialNumber());

	// Set expiration
	X509_gmtime_adj(X509_get_notBefore(cert), 0);	// Starting now
	X509_gmtime_adj(X509_get_notAfter(cert), certExpireDays * 3600 * 24);	// Days from now

	// Convert the root key
	EVP_PKEY *keyPairEVPRoot = EVP_PKEY_new();
	EVP_PKEY_assign_RSA(keyPairEVPRoot, rootKeyPair);

	// Set the public key
	if (rootCert)
	{
		X509_set_pubkey(cert, keyPairEVPRoot);
	}
	else
	{
		EVP_PKEY *keyPairEVPDevice = EVP_PKEY_new();
		EVP_PKEY_assign_EC_KEY(keyPairEVPDevice, deviceKey);
		X509_set_pubkey(cert, keyPairEVPDevice);
	}

	// Create issuer (root) subject/name
	X509_NAME *name;
	name = X509_get_issuer_name(cert);
	X509_NAME_add_entry_by_txt(name, "C", MBSTRING_ASC, (unsigned char *)"US", -1, -1, 0);		// Country code
	X509_NAME_add_entry_by_txt(name, "O", MBSTRING_ASC, (unsigned char *)"Renesas", -1, -1, 0);	// Organization
	X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, (unsigned char *)"Renesas Ref CA", -1, -1, 0);		// Common name

	// Set the subject
	if (rootCert)	// Root is the same as the issuer
	{
		X509_set_subject_name(cert, name);
	}
	else
	{
		name = X509_get_subject_name(cert);
		X509_NAME_add_entry_by_txt(name, "C", MBSTRING_ASC, (unsigned char *)"US", -1, -1, 0);					// Country code
		X509_NAME_add_entry_by_txt(name, "O", MBSTRING_ASC, (unsigned char *)"Renesas", -1, -1, 0);				// Organization
		X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, (unsigned char *)"RA Device", -1, -1, 0);		// Common name
	}

	// Sign the cert (self sign)
	X509_sign(cert, keyPairEVPRoot, EVP_sha256());

	// TEST: Validate signature
    int verifySign = X509_verify(cert, keyPairEVPRoot);
	    verifySign = X509_verify(cert, X509_get_pubkey(cert));

	///////////////////////// Write out the cert /////////////////

	// Create in temporary file if not specified
	if (outputFile == NULL)
		outputFile = "temp";

	// Create filenames
	char filenamePEM[256];
	char filenameCER[256];
	sprintf(filenamePEM, "%s.pem", outputFile);
	sprintf(filenameCER, "%s.cer", outputFile);

	// Write out the cert PEM format
	FILE *fd = fopen(filenamePEM, "wb");
	PEM_write_X509(fd, cert);
	fclose(fd);

	// Convert it to a .cert file
	convertPEMFileToCerFile(filenamePEM, filenameCER);

	// Free the cert
	X509_free(cert);

	// Return the cert file
	if (returnCert)
	{
		// Open the file
		// Open it binary, so we can just read in everything at once
        fd = fopen(filenamePEM, "rb");

		// Check the size
		fseek(fd, 0, SEEK_END);
		int size = ftell(fd) + 1;	// We need a NULL terminator
		if (size > returnCertMaxLength)
		{
			printf("Certificate creation failed, not enough space allocted for cert.  Cert size = %d\n", size);
			return -1;
		}

		// Read it in one junk
		fseek(fd, 0, SEEK_SET);
		int bytes = fread(returnCert, 1, returnCertMaxLength, fd);

		// Add in NULL terminator
		returnCert[bytes] = 0;
        *pemLength = bytes;
	}

	fclose(fd);
	remove(filenamePEM);
	remove(filenameCER);


	return 0;
}


int CertAuth::CreateSelfSignedECDSACert(char *outputFile, char *returnCert, int returnCertMaxLength, uint16_t *pemLength)
{
    *pemLength = 0;
    EVP_PKEY * pkey = EVP_PKEY_new();
    EC_KEY *ecc = EC_KEY_new_by_curve_name(OBJ_txt2nid("secp521r1"));
    /* simpler than going through OBJ_txt2nid */
    EC_KEY_set_asn1_flag(ecc, OPENSSL_EC_NAMED_CURVE);
    EC_KEY_generate_key(ecc);
    EVP_PKEY_assign_EC_KEY(pkey, ecc);

    X509 * x509 = X509_new();
    ASN1_INTEGER_set(X509_get_serialNumber(x509), nextDeviceSerialNumber());

    // Set expiration
    X509_gmtime_adj(X509_get_notBefore(x509), 0);	// Starting now
    X509_gmtime_adj(X509_get_notAfter(x509), certExpireDays * 3600 * 24);	// Days from now

    X509_set_pubkey(x509, pkey);
    X509_NAME *name = X509_get_issuer_name(x509);
    X509_NAME_add_entry_by_txt(name, "C", MBSTRING_ASC, (unsigned char *)"US", -1, -1, 0);					// Country code
    X509_NAME_add_entry_by_txt(name, "O", MBSTRING_ASC, (unsigned char *)"Renesas", -1, -1, 0);				// Organization
    X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, (unsigned char *)"RA Device", -1, -1, 0);		// Common name
    X509_set_subject_name(x509, name);

    X509_sign(x509, pkey, EVP_sha256());

    // Create filenames
    if (outputFile == NULL)
        outputFile = "temp";

    char filenamePEM[256];
    char filenameCER[256];
    char filenamePKEY[256];
    sprintf(filenamePEM, "%s.pem", outputFile);
    sprintf(filenameCER, "%s.cer", outputFile);
    sprintf(filenamePKEY, "%s_PrivateKey.pem", outputFile);

    // Write out the cert PEM format
    FILE *fd = fopen(filenamePEM, "wb");
    PEM_write_X509(fd, x509);
    fclose(fd);

    fd = fopen(filenamePKEY, "wb");
    PEM_write_PrivateKey(fd, pkey, EVP_des_ede3_cbc(), NULL, 0, NULL, "passphrase");
    fclose(fd);

    // Convert it to a .cert file
    convertPEMFileToCerFile(filenamePEM, filenameCER);

    // Free the cert
    X509_free(x509);

    // Return the cert file
    if (returnCert)
    {
        // Open the file
        // Open it binary, so we can just read in everything at once
        fd = fopen(filenamePEM, "rb");

        // Check the size
        fseek(fd, 0, SEEK_END);
        int size = ftell(fd) + 1;	// We need a NULL terminator
        if (size > returnCertMaxLength)
        {
            //AfxMessageBox("Not enough space allocated for PEM file");
            return -1;
        }

        // Read it in one junk
        fseek(fd, 0, SEEK_SET);
        int bytes = fread(returnCert, 1, returnCertMaxLength, fd);

        // Add in NULL terminator
        returnCert[bytes] = 0;
        *pemLength = bytes;
    }

	fclose(fd);
	remove(filenamePEM);
	remove(filenameCER);

    return 0;
}


int CertAuth::convertPEMFileToCerFile(char *pemFile, char *cerFile)
{

	char pemDatBuffer[20000];
	char *ptr = pemDatBuffer;

	// Open files
	FILE *fdSource = fopen(pemFile, "rt");
	if (fdSource == NULL)
		return -1;
	FILE *fdDest = fopen(cerFile, "wb");


	// Go though pem file and put it all into 1 string
	char buffer[2014];
	while (fgets(buffer, sizeof(buffer), fdSource))
	{
		// Remove any CR/LF's
		char *crlf = strchr(buffer, '\r');
		if (crlf)
			*crlf = 0;
		crlf = strchr(buffer, '\n');
		if (crlf)
			*crlf = 0;

		// Skip first and last line
		if (strncmp(buffer, "----", 4) == 0)
			continue;

		// Copy the data
		strcpy(ptr, buffer);
		ptr = strchr(ptr, 0);
	}
	fclose(fdSource);

	// Convert Base64 to Binary
	unsigned char binaryCert[20000];
	int len = utilobj.base64_decode(pemDatBuffer, binaryCert);

	// Write out the file
	if (len)
	{
		fwrite(binaryCert, 1, len, fdDest);
	}
	fclose(fdDest);

	return 0;
}

int CertAuth::nextDeviceSerialNumber()
{
	int serialNumber;

	// Get the next serial number
	FILE *fd = fopen("nextserialnumber.txt", "rt");
	if (fd == NULL)
		serialNumber = 1;
	else
	{
		char buffer[128];
		fgets(buffer, sizeof(buffer), fd);
		serialNumber = atoi(buffer);
		fclose(fd);
	}

	// Write out the next serial number
	fd = fopen("nextserialnumber.txt", "wt");
	fprintf(fd, "%d", serialNumber + 1);
	fclose(fd);

	return serialNumber;

}


int CertAuth::validateCert(char *batchFile)
{
	char folderNoSlash[MAX_PATH+1];
	char folder[MAX_PATH+1];
	GetCurrentDirectory(sizeof(folderNoSlash), folderNoSlash);
	sprintf(folder, "%s\\", folderNoSlash);

	char commandLine[256];
	sprintf(commandLine, "cmd.exe /C \"%s%s\"", folder, batchFile);
	STARTUPINFO si = {sizeof(STARTUPINFO)};
	PROCESS_INFORMATION pi;

	// Delete the results file
	char resultsFile[256];
	sprintf(resultsFile, "%svalidateResults.txt", folder);
	DeleteFile(resultsFile);

	// Get the enviornment
	char *env = NULL;

	if (!CreateProcess(NULL, commandLine, NULL, NULL, false, CREATE_NO_WINDOW, env, folder, &si, &pi))
	{
		//AfxMessageBox("Error creating process");

	}

	// Wait until it's done
	WaitForSingleObject(pi.hProcess, INFINITE);

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	// Parse the results
	FILE *fd = fopen(resultsFile, "rt");
	if (fd == NULL)
	{
//		AfxMessageBox("Error getting results");
		return 0;
	}
	char buffer[256];
	fgets(buffer, sizeof(buffer), fd);
	fclose(fd);
	if (strstr(buffer, "OK") != NULL)
		return 1;
	else
		return 0;


}


int CertAuth::extractPublicKey(char *certFile, unsigned char *publicKey)
{
	X509 *certRead;

	certRead = X509_new();

	// Read the cert
	FILE *fdReadCert = fopen(certFile, "rb");
	if (fdReadCert == NULL)
	{
		//AfxMessageBox("Error reading cert");
		return 0;
	}
	certRead = PEM_read_X509(fdReadCert, &certRead, NULL, NULL);

	// Extract the public key
	int publicKeyExtractedLength = 0;
	EVP_PKEY* publicKeyEVP = X509_get_pubkey(certRead);
	RSA *rsaVersionPublicKey = EVP_PKEY_get1_RSA(publicKeyEVP);
	EVP_PKEY_free(publicKeyEVP);
	int len = BN_bn2bin(rsaVersionPublicKey->n, publicKey);
	
	return len;


#if 0
	// Show the subject entry
	X509_NAME *nameRead;
	nameRead = X509_get_subject_name(certRead);
	X509_NAME_ENTRY *name;
	for (int i = 0; i < X509_NAME_entry_count(nameRead); i++)
	{
		name = X509_NAME_get_entry(nameRead, i);

		// Show the object name
		ASN1_OBJECT *object = X509_NAME_ENTRY_get_object(name);


		// Show the value
		ASN1_STRING *data = X509_NAME_ENTRY_get_data(name);
		unsigned char *string = NULL;
		int nameLength = ASN1_STRING_to_UTF8(&string, data);
		nameLength++;
	}

	// Another approach
	int nid_country = OBJ_txt2nid("C");
	char val[256];
	X509_NAME_get_text_by_NID(nameRead, nid_country, val, sizeof(val));
	int nid_organization = OBJ_txt2nid("O");
	X509_NAME_get_text_by_NID(nameRead, nid_organization, val, sizeof(val));
	int nid_commonName = OBJ_txt2nid("CN");
	X509_NAME_get_text_by_NID(nameRead, nid_commonName, val, sizeof(val));

	// Show the serial number
	ASN1_INTEGER *asnSerialNumber;
	asnSerialNumber = X509_get_serialNumber(certRead);
	if (asnSerialNumber == NULL)
	{
		AfxMessageBox("No Serial Number");
		return 0;
	}
	long serialNumber = ASN1_INTEGER_get(asnSerialNumber);
#endif

}

