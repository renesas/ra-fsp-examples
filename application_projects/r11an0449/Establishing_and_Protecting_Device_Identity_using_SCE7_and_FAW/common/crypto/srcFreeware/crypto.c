
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "crypto.h"
#include "ecc.h"
#include "sha256.h"
#include "cryptoHelpers.h"
#ifdef ENABLE_HW_CRYPTO
#include <hw_sha256.h>
#include <hw_ecc.h>
#endif
bool cryptoInitialised = false;


//--------------------------------------------------------------------------------
// Initialisation
//--------------------------------------------------------------------------------
bool cryptoInitialise(void)
{
    // todo - Initialise random number generator

    cryptoInitialised = true;

#ifdef ENABLE_HW_CRYPTO
	synergy_init_hw_crypto();
#endif
#ifndef ENABLE_HW_ECC
    cryptoPrepareRandomGenerator();
#endif
    return true;
}


//--------------------------------------------------------------------------------
// Key generation
//--------------------------------------------------------------------------------

bool cryptoMakeRandomByteString(uint8_t *pBuffer, uint32_t length)
{
    if (!cryptoInitialised)
    {
        return false;
    }
    if (!memoryChecksIsBufferInRam(pBuffer, length))
    {
        return false;
    }

    do
    {
        // We just get some random bits
    	crypto_rand(pBuffer, length);

        // generate again if it is all zero bytes (it could happen...)
        for (uint32_t i = 0; i < length; i++)
        {
            if (pBuffer[i] != '\0')
            {
                return true;
            }
        }
    } while (1);

    // can never reach here.... will eventually make a key.
}




bool cryptoMakeAesKey(AesKey *pKey)
{
    return cryptoMakeRandomByteString((uint8_t*)pKey, sizeof(AesKey));
}


bool cryptoMakeHmacKey(HmacKey *pKey)
{
    return cryptoMakeRandomByteString((uint8_t*)pKey, sizeof(HmacKey));
}


bool cryptoMakeEccKeys(EccPublicKey *pPublic, EccPrivateKey *pPrivate)
{
    bool retval = false;
    if (!cryptoInitialised)
    {
    }
    else if (!memoryChecksIsBufferInRam(pPublic, sizeof(EccPublicKey)))
    {
    }
    else if (!memoryChecksIsBufferInRam(pPrivate, sizeof(EccPrivateKey)))
    {
    }
    else
    {
        int r = uECC_make_key((uint8_t*)pPublic, (uint8_t*)pPrivate);  //to avoid compile casting warning.
        retval = (bool) r;
    }

    return retval;
}


#ifndef ENABLE_HW_ECC

static bool makeNext256bits( const uint8_t *pHmacKey,  const uint8_t *pTag, uint32_t length, uint32_t counter, uint8_t *pOutput)
{
    // Use a HMAC to generate some pseudo random data.
    // Use the supplied key and HMAC over the concatenation of counter+tag+'\0'+length

#define SEED_LENGTH  (512 / 8)

    uint8_t retval = 0;
    HMACContext ctx;

    if ((!pHmacKey) || (!pTag) || (!pOutput))
    {
        retval = 1;
    }
    else if (shaSuccess != hmacReset(&ctx, pHmacKey, SEED_LENGTH))
    {
        retval = 2;
    }
    else if (shaSuccess != hmacInput(&ctx, (const uint8_t*)&counter, 4))
    {
        retval = 3;
    }
    else if (shaSuccess != hmacInput(&ctx, pTag, (int)strlen((const char *)pTag) + 1))
    {
        retval = 4;
    }
    else if (shaSuccess != hmacInput(&ctx, (const uint8_t*)&length, 4))
    {
        retval = 5;
    }
    else if (shaSuccess != hmacResult(&ctx, pOutput))
    {
        retval = 6;
    }

    if (0 != retval)
    {
        ERROR("makeNext256bits - failed at step %d \n", retval);
        return false;
    }
    return true;
}
#endif

int mystrncpy (char *s1, const char *s2, int len);
int mystrncpy (char *s1, const char *s2, int len)
{
    int i;

    for (i = 0; i < len; i++)
    {
        s1[i] = s2[i];
        if (s1[i] == '\0')
        	return len;
    }
    return len;
}

bool cryptoDeriveEccKey(
#ifdef ENABLE_HW_ECC
    const uint8_t       *pSeed __attribute__((unused)),
#else
    const uint8_t       *pSeed,        // pointer to the seed for this device.
#endif
    const char          *pTag,        // Text string to initialise the key derivation function.
    EccPublicKey        *pPublicKey,  // generated key is written into this buffer
    EccPrivateKey       *pPrivateKey  // generated key is written into this buffer
    )
{
    uint8_t pseudoRand[SHA256_SIZE * 2];
#ifndef ENABLE_HW_ECC
    uint32_t length = 256 + 64;   // TODO - why these magic values?
    uint32_t counter = 0;
    uint32_t spareBytes = 0;
#endif

    bool retval = false;

    if (!memoryChecksIsBufferInRam(pPublicKey, sizeof(EccPublicKey)))
    {
    }
    else if (!memoryChecksIsBufferInRam(pPrivateKey, sizeof(EccPrivateKey)))
    {
    }
    else
    {
#ifdef ENABLE_HW_ECC
    	mystrncpy ((char *)pseudoRand, pTag, SHA256_SIZE * 2);
    	uECC_make_PRF_keyHW((uint8_t*)pPublicKey, (uint8_t*)pPrivateKey, (const uint8_t *) pTag);
#else
        do
        {
            memcpy(pseudoRand, pseudoRand + 2 * SHA256_SIZE - spareBytes, spareBytes);
            makeNext256bits(pSeed, (const uint8_t *)pTag, length, counter, pseudoRand + spareBytes);
            counter++;
            if (0 == spareBytes)
            {
                makeNext256bits(pSeed, (const uint8_t *)pTag, length, counter, pseudoRand + SHA256_SIZE);
                counter++;
                spareBytes = SHA256_SIZE;
            }
            spareBytes -= 8;
        } while (uECC_make_PRF_key((uint8_t*)pPublicKey, (uint8_t*)pPrivateKey, pseudoRand) == 0);
#endif // ENABLE_HW_ECC
        retval = true;
    }
    return retval;
}

bool sha256_calc_hash( const uint8_t *pData, uint32_t length, uint8_t *pHash)
{
	SHA256Context 	shaContext;
	int 			shaStatus;

	if (shaSuccess != (shaStatus = SHA256Reset(&shaContext)))
	{
		ERROR("[e] SHA256Reset failed %d\n", shaStatus);
	}
	else if (shaSuccess != (shaStatus = SHA256Input(&shaContext, pData, length)))
	{
		ERROR("[e] SHA256Input failed %d\n", shaStatus);
	}
	else if (shaSuccess != (shaStatus = SHA256FinalBits(&shaContext, 0, 0)))
	{
		ERROR("[e] SHA256FinalBits failed %d\n", shaStatus);
	}
	else if (shaSuccess != (shaStatus = SHA256Result(&shaContext, pHash)))
	{
		ERROR("[e] SHA256Result failed %d\n", shaStatus);
	}
	else
	{
		return true;
	}
	return false;
}



/*-------------------------------------------------------------------------------------------------
 * There is a need to calculate the SHA256 hash of a file as it is transfered down. Rather than
 * wait until the entire file is transfered, the hash is updated for each segment as it is
 * received. In addition, two separate hashes are calculated so multiple context are maintained
 * to allow them to be independently calculated.
 *------------------------------------------------------------------------------------------------*/

SHA256Context hashContexts[NUM_OF_SEGMENTED_HASHES];


/********************************************************************************************
 *
 * Function: cryptoCalculateSha256Start()
 *
 * Description:
 *   SW based function to calculate the Sha256Start of a segmented buffer.
 *
 * Parameters:  See below
 *
 * Return value: true  - function completed without error
 *               false - an error occurred
 **********************************************************************************************/
bool cryptoCalculateSha256Start(
    SegmentedHashContexts context,
    const uint8_t   *pInput,            // Data to be hashed.  (can be PNULL if no more data to be added).
    const uint32_t   length       // byte length of the data.
    )
{
    int             shaStatus;

    if (context >= NUM_OF_SEGMENTED_HASHES)
    {
        ERROR("[e] cryptoCalculateSha256Start failed, invalid hash context requested %d\n", context);
        return false;
    }

    memset(&hashContexts[context],0, sizeof(hashContexts[context]));
    if (shaSuccess != (shaStatus = SHA256Reset(&hashContexts[context])))
    {
        ERROR("[e] SHA256Reset failed %d\n", shaStatus);
    }
    else if (NULL != pInput)
    {
        if (shaSuccess != (shaStatus = SHA256Input(&hashContexts[context], pInput, length)))
        {
            ERROR("[e] SHA256Input failed %d\n", shaStatus);
        }
        else
        {
            return true;
        }
    }
    else
    {
        return true;
    }
    return false;
}

/********************************************************************************************
 *
 * Function: cryptoCalculateSha256MiddleEnd()
 *
 * Description:
 *   SW based function to calculate the SHA256MiddelEnd of a segmented buffer.
 *
 * Parameters:  See below
 *
 * Return value: true  - function completed without error
 *               false - an error occurred
 **********************************************************************************************/
bool cryptoCalculateSha256MiddleEnd(
    SegmentedHashContexts context,
    const uint8_t   *pInput,                // Data to be hashed. (can be PNULL if no more data to be added).
    const uint32_t   length,          // byte length of the data.
    bool            final,            // true if final segment.
    Sha256          *pHash            // Pointer to a block of memory into which the result can be returned.
    )
{
    int             shaStatus;

    if (context >= NUM_OF_SEGMENTED_HASHES)
    {
        ERROR("[e] cryptoCalculateSha256MiddleEnd failed, invalid hash context requested %d\n", context);
        return false;
    }

    if (NULL != pInput)  // there is stuff to add to the hash.
    {
        if (shaSuccess != (shaStatus = SHA256Input(&hashContexts[context], pInput, length)))
        {
            ERROR("[e] SHA256Input failed %d\n", shaStatus);
            return false;
        }
    }

    if (final)  // need to finalise the hash.
    {
        if (shaSuccess != (shaStatus = SHA256FinalBits(&hashContexts[context], 0, 0)))
        {
            ERROR("[e] SHA256FinalBits failed %d\n", shaStatus);
            return false;
        }
        else if (shaSuccess != (shaStatus = SHA256Result(&hashContexts[context], (uint8_t*) pHash)))
        {
            ERROR("[e] SHA256Result failed %d\n", shaStatus);
            return false;
        }
    }

    return true;
}

//--------------------------------------------------------------------------------
// ECC signatures
//--------------------------------------------------------------------------------
bool cryptoHash2Buffers(
    const uint8_t       *pBuf1,         // Buffer to be signed.
    const uint32_t       length1,       // length of the buffer.
    const uint8_t       *pBuf2,         // 2nd part of buffer.  [NULL if not present]
    const uint32_t       length2,       // length of 2nd part.
    Sha256              *pHash          // result - the hash of the pair of buffers.
    )
{
    SHA256Context 	shaContext;
    int 			shaStatus;
    bool            retval = false;
    bool            tmpRetval = false;

    if (!memoryChecksIsBufferInMemory(pBuf1, length1))
    {
        ERROR("cryptoHash2Buffers - Invalid input\n");
    }
    else if ((length2 > 0) && (!memoryChecksIsBufferInMemory(pBuf2, length2)))
    {
        ERROR("cryptoHash2Buffers - Invalid input\n");
    }
    else if (shaSuccess != (shaStatus = SHA256Reset(&shaContext)))
    {
        ERROR("cryptoHash2Buffers - reset failed\n");
    }
    else if (shaSuccess != (shaStatus = SHA256Input(&shaContext, pBuf1, length1)))
    {
        ERROR("cryptoHash2Buffers - hash1 failed\n");
    }
    else if (pBuf2 != NULL && length2 > 0)
    {
        if (shaSuccess == (shaStatus = SHA256Input(&shaContext, pBuf2, length2)))
        {
            tmpRetval = true;
        }
        else
        {
            ERROR("cryptoHash2Buffers - hash2 failed\n");
        }
    }
    else
    {
        tmpRetval = true;
    }
    if (!tmpRetval)
    {

    }
    else if (shaSuccess != (shaStatus = SHA256FinalBits(&shaContext, 0, 0)))
    {
        ERROR("cryptoHash2Buffers - hash finalbits failed\n");
    }
    else if (shaSuccess != (shaStatus = SHA256Result(&shaContext, (uint8_t*) pHash)))
    {
        ERROR("cryptoHash2Buffers - hash result failed\n");
    }
    else
    {
        retval = true;
    }

    return retval;
}


bool cryptoEccHashAndSign(
    const uint8_t       *pBuf1,         // Buffer to be signed.
    const uint32_t       length1,       // length of the buffer.
    const uint8_t       *pBuf2,         // 2nd part of buffer.  [NULL if not present]
    const uint32_t       length2,       // length of 2nd part.
    const EccPrivateKey *pPrivateKey,   // ECC private key to be used for the signature
    EccSignature        *pSig           // output - ECC signature of the message.
    )
{
    Sha256          hash;
    bool    retval = false;

    if (
        (!memoryChecksIsBufferInMemory(pPrivateKey, sizeof(EccPrivateKey))) ||
        (!memoryChecksIsBufferInRam(pSig, sizeof(EccSignature)))
        )
    {

    }
    else if (!cryptoHash2Buffers(pBuf1, length1, pBuf2, length2, &hash))
    {
        ERROR("HashAndSign - hash failed\n");
    }
#ifdef ENABLE_HW_ECC
    else if (!uECC_signHW((uint8_t*)pPrivateKey, hash, (uint8_t*)pSig))
    {
        ERROR("HashAndSign - sign failed\n");
    }
#else
    else if (!uECC_sign((uint8_t*)pPrivateKey, hash, (uint8_t*)pSig))
    {
        ERROR("HashAndSign - sign failed\n");
    }
#endif
    else
    {
        retval = true;
    }

    return retval;
}


bool cryptoEccHashAndVerify(
    const uint8_t       * const pBuf1,      // Buffer to be signed.
    const uint32_t       length1,       	// length of the buffer.
    const uint8_t       * const pBuf2,      // 2nd part of buffer.  [NULL if not present]
    const uint32_t       length2,       	// length of 2nd part.
    const EccPublicKey * const pPublicKey,  // ECC public key used to check the signature.
    const EccSignature * const pSig         // signature to be checked.
    )
{
    Sha256  hash;
    bool    retval = false;

    if (
        (!memoryChecksIsBufferInMemory(pPublicKey, sizeof(EccPublicKey))) ||
        (!memoryChecksIsBufferInMemory(pSig, sizeof(EccSignature)))
        )
    {
        ERROR("HashAndVerify - memory check failed\n");
    }
    else if (!cryptoHash2Buffers(pBuf1, length1, pBuf2, length2, &hash))
    {
        ERROR("HashAndVerify - hash failed\n");
    }
#ifdef ENABLE_HW_ECC
    else if (!uECC_verifyHW((uint8_t*)pPublicKey, hash, (uint8_t*)pSig))
    {
        ERROR("HashAndVerify - verify failed\n");
    }
#else
    else if (!uECC_verify((uint8_t*)pPublicKey, hash, (uint8_t*)pSig))
    {
        ERROR("HashAndVerify - verify failed\n");
    }
#endif
    else
    {
        retval = true;
    }

    return retval;
}


bool cryptoEccVerifyAHash(
    const Sha256       * const pHash,       // Hash to be verified.
    const EccPublicKey * const pPublicKey,  // ECC public key used to check the signature.
    const EccSignature * const pSig         // signature to be checked.
    )
{
    bool    retval = false;
#ifdef ENABLE_HW_ECC
    if (!uECC_verifyHW((uint8_t*)pPublicKey, (uint8_t*)pHash, (uint8_t*)pSig))
    {
        ERROR("VerifyAHash - verify failed\n");
    }
#else
    if (!uECC_verify((uint8_t*)pPublicKey, (uint8_t*)pHash, (uint8_t*)pSig))
    {
        ERROR("VerifyAHash - verify failed\n");
    }
#endif

    else
    {
        retval = true;
    }

    return retval;
}

//--------------------------------------------------------------------------------
// ECIES encrypt/decrypt
//--------------------------------------------------------------------------------

#if 0
bool cryptoECIESDecrypt(
    const uint8_t       *pInput,        // Message to be decrypted.
    const uint32_t       length,        // length of the message.
    const EccPrivateKey *pPrivateKey,   // ECC private key to be used to derive shared secret
    uint8_t             *pOutput,       // Buffer into message is decrypted.
    uint32_t            *pOutLength     // input - size of buffer. output = size of decrypted message.
    )
{
    uint8_t             secret[ECC_PRIVATE_KEY_SIZE];
    HmacResult          hmacResult;

    uint8_t             kdfOutput[AES128_KEY_SIZE + AES128_IV_SIZE + HMAC_KEY_SIZE]; // AES key + IV + HMAC key
    AesKey              *aesKey             = (AesKey*)  kdfOutput;
    AesIv               *aesIV              = (AesIv*)   (kdfOutput + AES128_KEY_SIZE);
    HmacKey             *hmacKey            = (HmacKey*) (kdfOutput + AES128_KEY_SIZE + AES128_IV_SIZE);

    const uint8_t       *pEmphermalPublic   = pInput;
    const uint8_t       *pExpectedHmac      = pInput + ECC_PUBLIC_KEY_SIZE;
    const uint8_t       *pEncryptedData     = pInput + ECC_PUBLIC_KEY_SIZE + HMAC_KEY_SIZE;
    uint32_t            payloadLength       = length - (ECC_PUBLIC_KEY_SIZE + HMAC_KEY_SIZE);

    bool                 retval             = false;

    if (
        (!memoryChecksIsBufferInMemory(pInput, length)) ||
        (!memoryChecksIsBufferInMemory(pPrivateKey, sizeof(EccPrivateKey))) ||
        (!memoryChecksIsBufferInRam(pOutput, *pOutLength))
       )
    {
        ERROR("cryptoECIESDecrypt - invalid parameters\n");
    }
    else if (*pOutLength < payloadLength)
    {
        ERROR("cryptoECIESDecrypt - output buffer too small\n");
    }
    // create the shared secret
    else if (uECC_shared_secret(pEmphermalPublic, (uint8_t *)pPrivateKey, secret) == 0)
    {
        ERROR("cryptoECIESDecrypt - failed to make shared secret\n");
    }
    // Now create the Encryption and MAC keys and the IV
    // We use 128 bits for each one
    else if (!KDF2(secret, sizeof(kdfOutput), kdfOutput))
    {
        ERROR("cryptoECIESDecrypt - failed to derive keys\n");
    }
    // calculate the HMAC
    else if (shaSuccess != hmac(pEncryptedData, payloadLength, (uint8_t *)hmacKey, SHA256_SIZE, hmacResult))
    {
        ERROR("cryptoECIESDecrypt - hmac calculation failed\n");
    }
    // compare the hmac values
    else if (0 != memcmp(pExpectedHmac, hmacResult, SHA256_SIZE))
    {
        ERROR("cryptoECIESDecrypt - hmac failed\n");
    }
    // now AES decrypt the payload
    else 
    {
        *pOutLength = AES128_CBC_decrypt_buffer(pOutput, pEncryptedData, payloadLength, (uint8_t *)aesKey, (uint8_t *)aesIV, true);
        retval = true;
    }

    return retval;
}



bool cryptoECIESEncrypt(
    const uint8_t       *pInput,        // Message to be encrypted
    const uint32_t       length,        // length of the message.
    const EccPublicKey  *pPublicKey,    // ECC public key to be used to derive shared secret.
    uint8_t             *pOutput,       // Buffer into which the message is encrypted.
    uint32_t            *pOutLength     // input - size of buffer. output = size of encrypted message.
    )
{
    uint8_t             secret[ECC_PRIVATE_KEY_SIZE];

    uint8_t             kdfOutput[AES128_KEY_SIZE + AES128_IV_SIZE + HMAC_KEY_SIZE]; // AES key + IV + HMAC key
    AesKey              *aesKey             = (AesKey*)kdfOutput;
    AesIv               *aesIV              = (AesIv*) (kdfOutput + AES128_KEY_SIZE);
    HmacKey             *hmacKey            = (HmacKey*) (kdfOutput + AES128_KEY_SIZE + AES128_IV_SIZE);

    EccPrivateKey       emphermalPrivate;
    const EccPublicKey  *pEmphermalPublic   = (const EccPublicKey*) pOutput;
    const HmacResult    *pExpectedHmac      = (const HmacResult*) (pOutput + ECC_PUBLIC_KEY_SIZE);
    uint8_t             *pEncryptedData     = pOutput + ECC_PUBLIC_KEY_SIZE + HMAC_KEY_SIZE;
    uint32_t            payloadLength       = length + (ECC_PUBLIC_KEY_SIZE + HMAC_KEY_SIZE);

    bool                 retval = false;

    if (*pOutLength < payloadLength + 16)  // extra 16 for AES padding.
    {
    }
    else if (uECC_make_key((uint8_t*)pEmphermalPublic, (uint8_t*)emphermalPrivate) == 0)
    {
    }
    else if (uECC_shared_secret((uint8_t*)pPublicKey, emphermalPrivate, secret) == 0)
    {
    }
    else if (!KDF2(secret, sizeof(kdfOutput), kdfOutput))
    {
    }
    else
    {
        uint32_t aesLength = AES128_CBC_encrypt_buffer(pEncryptedData, pInput, length, (uint8_t*)aesKey, (uint8_t*)aesIV, true);
        *pOutLength = payloadLength + (aesLength - length);

        if (shaSuccess != hmac(pEncryptedData, aesLength, (uint8_t*)hmacKey, SHA256_SIZE, (uint8_t*)pExpectedHmac))
        {
        }
        else
        {
            retval = true;
        }
    }
    return retval;
}


bool cryptoDataEncrypt(
    const uint8_t   * const pInput,     // Data to be encrypted
    const uint32_t   length,            // length of data to be encrypted
    const AesKey    * const pAesKey,    // AES key use to encrypt.
    const HmacKey   * const pHmacKey,   // HMAC key to use to calculate hash.
    uint8_t         *pEncrypted,        // Pointer to buffer into which to encrypt data.
    uint32_t        *pEncryptedLength   // input - size of buffer. output = size of encypted message.
    )
{
    HmacResult  *pExpectedHmac  = (HmacResult*) pEncrypted;
    AesIv       *pIv            = (AesIv*) (pEncrypted + SHA256_SIZE);
    uint8_t     *ciphertext     = pEncrypted + SHA256_SIZE + AES128_IV_SIZE;
    uint32_t    cipherLen       = 0;

    bool        retval          = false;

    // generate random iv.
    if (!crypto_rand((uint8_t*)pIv, AES128_IV_SIZE))
    {

    }
    else if (*pEncryptedLength < length + SHA256_SIZE + AES128_KEY_SIZE + 16)  // +16 for AES padding.
    {
        ERROR("DataEncrypt - target buffer too small\n");
    }
    else if (0 == (cipherLen = AES128_CBC_encrypt_buffer(ciphertext, pInput, length, (uint8_t*)pAesKey, (uint8_t*)pIv, true)))
    {

    }
    else if (shaSuccess != hmac((uint8_t*)pIv, cipherLen + AES128_KEY_SIZE, (uint8_t*)pHmacKey, SHA256_SIZE, (uint8_t*)pExpectedHmac))
    // calculate the HMAC
    {
        ERROR("DataEncrypt - hmac failed\n");
    }
    else
    {
        retval = true;
        *pEncryptedLength = cipherLen + SHA256_SIZE + AES128_KEY_SIZE;
    }

    return retval;
}

bool cryptoDataCheckHmac(
    const uint8_t   * const pInput,     // Data to be checked.
    const uint32_t   length,            // length of data to be checked.
    const HmacKey   * const pHmacKey    // HMAC key to use to calculate hash.
    )
{
    const uint8_t       *pExpectedHmac  = pInput;
    const uint8_t       *pIv            = pInput + SHA256_SIZE;
    uint32_t            ciphertextLen   = length - SHA256_SIZE - AES128_IV_SIZE;

    HmacResult          hmacResult;

    bool                retval          = false;

    // calculate the HMAC
    if (shaSuccess != hmac(pIv, AES128_IV_SIZE + ciphertextLen, (const unsigned char *)pHmacKey, SHA256_SIZE, hmacResult))
    {
    }
    // compare the HMAC
    else if (memcmp(hmacResult, pExpectedHmac, SHA256_SIZE) != 0)
    {
    }
    else
    {
        retval = true;
    }
    return retval;
}

bool cryptoDataDecryptInitial(
    const uint8_t   * const pInput,     // Data to be decrypted
    const uint32_t   length,            // length of data to be decrypted (If NOT last segment length must be multiple of AES block size = 16)
    const AesKey    * const pAesKey,    // AES key use for decrypt.
    uint8_t         *pPlain,            // Pointer to buffer into which to encrypt data.
    uint32_t        *pPlainLength,      // input - size of buffer. output = size of decrypted block.
    const bool       lastSegment        // True if this is the last segment of the data buffer (and hence de-padding is required).
    )
{
    const uint8_t       *pIv            = pInput + SHA256_SIZE;
    const uint8_t       *ciphertext     = pInput + SHA256_SIZE + AES128_KEY_SIZE;
    uint32_t            ciphertextLen   = length - (SHA256_SIZE + AES128_KEY_SIZE);

    bool                retval = false;

    // todo - check validity of all pointers.....

    if (*pPlainLength < ciphertextLen)
    {
        // no space for result.
        ERROR("cryptoDataDecryptInitial - no space for result %d<%d\n", (int)*pPlainLength, (int)length);
    }
    else if ((!lastSegment) && (0 != (length % AES128_KEY_SIZE)))
    {
        // intermediatory decodes must be of whole AES blocks.
        ERROR("cryptoDataDecryptInitial - non-last segments must be mod 16 size. size=%d\n", (int)length);
    }
    else if (length < SHA256_SIZE + AES128_KEY_SIZE)
    {
        // no payload.
        ERROR("cryptoDataDecryptInitial - size(%d) too short.\n", (int)length);
    }
    else
    {
        *pPlainLength = AES128_CBC_decrypt_buffer(pPlain, ciphertext, ciphertextLen, (uint8_t*)pAesKey, pIv, lastSegment);
        retval = true;
    }

    return retval;
}


bool cryptoDataDecryptBlock(
    const uint8_t   * const pInput,     // Data to be decrypted
    const uint32_t   length,            // length of data to be decrypted (If NOT last segment length must be multiple of AES block size = 16)
    uint8_t         *pPlain,            // Pointer to buffer into which to encrypt data.
    uint32_t        *pPlainLength,      // input - size of buffer. output = size of decrypted block.
    const bool       lastSegment        // True if this is the last segment of the data buffer (and hence de-padding is required).
    )
{
    bool                retval = false;

    // todo check all pointers.


    if (*pPlainLength < length)
    {
        // no space for result.
        ERROR("cryptoDataDecryptBlock - no space for result %d<%d\n", (int)*pPlainLength, (int)length);
    }
    else
    {
        *pPlainLength = AES128_CBC_decrypt_buffer(pPlain, pInput, length, NULL, NULL, lastSegment);
        retval = true;
    }

    return retval;
}
#endif

