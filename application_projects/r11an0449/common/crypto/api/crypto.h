/**********************************************************************************************************************
* Copyright (c) 2015 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/




#if !defined CRYPTO_API
#define CRYPTO_API

#include <stdbool.h>
#include <stdint.h>
#include "crypto_cfg.h"
#include "cryptoTypes.h"
// For use in C++
#ifdef __cplusplus
extern "C" {
#endif
    

//--------------------------------------------------------------------------------
// Initialisation
//--------------------------------------------------------------------------------
extern bool cryptoInitialise(void);

//--------------------------------------------------------------------------------
// General random byte string.
//--------------------------------------------------------------------------------

// (Used within the SKI to make the random seed).
extern bool cryptoMakeRandomByteString(uint8_t *pBuffer, uint32_t length);

//--------------------------------------------------------------------------------
// Key generation
//--------------------------------------------------------------------------------

// Generate a random AES key.
// Passed in a pointer to a sufficiently sized buffer.
// Return TRUE if pKey is populated.
extern bool cryptoMakeAesKey(
        AesKey *pKey                // generated key is written into this buffer
        );

// Generate a random SHA256 key.
// Passed in a pointer to a sufficiently sized buffer.
// Return TRUE if pKey is populated.
extern bool cryptoMakeHmacKey(
        HmacKey *pKey             // generated key is written into this buffer
        );

// Generate random pair of ECC keys.
// Passed in pointer to sufficiently sized buffers.
// Return TRUE if keys are populated.
extern bool cryptoMakeEccKeys(
        EccPublicKey *pPublic,      // generated key is written into this buffer
        EccPrivateKey *pPrivate     // generated key is written into this buffer
        );


// Derive a ECC key from the (a) device seed plus (b) a text string tag.
// The public/private key pair is returned in the supplied buffers.
extern bool cryptoDeriveEccKey(
        const uint8_t       *pSeed,       // pointer to the seed for this device.
        const char          *pTag,        // Text string to initialise the key derivation function.
        EccPublicKey        *pPublicKey,  // generated key is written into this buffer
        EccPrivateKey       *pPrivateKey  // generated key is written into this buffer
        );


//--------------------------------------------------------------------------------
// ECC signatures
//--------------------------------------------------------------------------------

// calculate sha256 hash over the msg buffer.
// calculate ECC sig over the hash
// writes result into the supplied signature buffer.
// [often the code of a module is in one buffer and the ModuleUpdateKey in another so have to do a sign over two buffers.....
extern bool cryptoEccHashAndSign(
    const uint8_t       *pBuf1,         // Buffer to be signed.
    const uint32_t       length1,       // length of the buffer.
    const uint8_t       *pBuf2,         // 2nd part of buffer.  [NULL if not present]
    const uint32_t       length2,       // length of 2nd part.
    const EccPrivateKey *pPrivateKey,   // ECC private key to be used for the signature
    EccSignature        *pSig           // output - ECC signature of the message.
    );


// calculate sha256 hash over the msg buffer.
// Verify ECC sig over the hash
// Returns true if sig is good. false otherwise.
extern bool cryptoEccHashAndVerify(
    const uint8_t       * const pBuf1,      // Buffer to be signed.
    const uint32_t       length1,       	// length of the buffer.
    const uint8_t       * const pBuf2,      // 2nd part of buffer.  [NULL if not present]
    const uint32_t       length2,      		// length of 2nd part.
    const EccPublicKey * const pPublicKey,  // ECC public key used to check the signature.
    const EccSignature * const pSig         // signature to be checked.
    );


// calculate sha256 hash over the msg buffer.
// Verify ECC sig over the hash
// Returns true if sig is good. false otherwise.
extern bool cryptoEccVerifyAHash(
    const Sha256       * const pHash,       // Hash to be verified.
    const EccPublicKey * const pPublicKey,  // ECC public key used to check the signature.
    const EccSignature * const pSig         // signature to be checked.
    );

//--------------------------------------------------------------------------------
// ECIES encrypt/decrypt
//--------------------------------------------------------------------------------

// ECIES decrypt a buffer.
// generate shared secret
// derive keys from the shared secret
// check hmac across buffer.
// AES decrypt buffer.
// Returns true if decrypt succeeds.
extern bool cryptoECIESDecrypt(
        const uint8_t       *pMsg,          // Message to be decrypted.
        const uint32_t       length,        // length of the message.
        const EccPrivateKey *pPrivateKey,   // ECC private key to be used to derive shared secret
        uint8_t             *pOutput,       // Buffer into message is decrypted.
        uint32_t            *pOutLength     // input - size of buffer. output = size of decrypted message.
        );

// ECIES encrypt a buffer.
// Generates a random key pair.
// derives a shared secret from the random keys.
// AES encrypts the message.
// calculates and saves an HMAC across the message.
extern bool cryptoECIESEncrypt(
        const uint8_t       *pMsg,          // Message to be encrypted
        const uint32_t       length,        // length of the message.
        const EccPublicKey  *pPublicKey,    // ECC public key to be used to derive shared secret.
        uint8_t             *pOutput,       // Buffer into which the message is encrypted.
        uint32_t            *pOutLength     // input - size of buffer. output = size of encypted message.
        );


//--------------------------------------------------------------------------------
// data encrypt/decrypt
//--------------------------------------------------------------------------------

// AES-CBC encrypt the buffer and then calculate an HMAC over the result.
extern bool cryptoDataEncrypt(
        const uint8_t   * const pInput,     // Data to be encrypted
        const uint32_t   length,            // length of data to be encrypted
        const AesKey    * const pAesKey,    // AES key use to encrypt.
        const HmacKey   * const pHmacKey,   // HMAC key to use to calculate hash.
        uint8_t         *pEncrypted,        // Pointer to buffer into which to encrypt data.
        uint32_t        *pEncryptedLength   // input - size of buffer. output = size of encypted message.
        );

// Application images could be larger than the onchip ram.
// 1) HMAC can be performed over the whole image in flash.
// 2) AES decrypt has to be performed in blocks.
// AES crypto retains context internally from one invocation to the next.


// Calculate hash over received data.
extern bool cryptoDataCheckHmac(
        const uint8_t   *const pInput,      // Data to be checked.
        const uint32_t   length,            // length of data to be checked.
        const HmacKey   *const pHmacKey     // HMAC key to use to calculate hash.
        );

// Decrypt the first block of the data
extern bool cryptoDataDecryptInitial(
        const uint8_t   * const pInput,     // Data to be decrypted
        const uint32_t   length,            // length of data to be decrypted (If NOT last segment length must be multiple of AES block size = 16)
        const AesKey    * const pAesKey,    // AES key use for decrypt.
        uint8_t         *pPlain,            // Pointer to buffer into which to encrypt data.
        uint32_t        *pPlainLength,      // input - size of buffer. output = size of decrypted block.
        const bool       lastSegment        // True if this is the last segment of the data buffer (and hence de-padding is required).
        );

// Decrypt non-first block of the data.
extern bool cryptoDataDecryptBlock(
        const uint8_t   * const pInput,     // Data to be decrypted
        const uint32_t   length,            // length of data to be decrypted (If NOT last segment length must be multiple of AES block size = 16)
        uint8_t         *pPlain,            // Pointer to buffer into which to encrypt data.
        uint32_t        *pPlainLength,      // input - size of buffer. output = size of decrypted block.
        const bool       lastSegment        // True if this is the last segment of the data buffer (and hence de-padding is required).
        );

extern bool cryptoHash2Buffers(
    const uint8_t       *pBuf1,         // Buffer to be signed.
    const uint32_t       length1,       // length of the buffer.
    const uint8_t       *pBuf2,         // 2nd part of buffer.  [NULL if not present]
    const uint32_t       length2,       // length of 2nd part.
    Sha256              *pHash          // result - the hash of the pair of buffers.
    );

//--------------------------------------------------------------------------------
// Simple segmented hashes
//--------------------------------------------------------------------------------

// Each Add/Modify command within an Update contains a hash over the payload.
// Separately there is a hash over the whole update.
// Both hashes are separately signed.
//
// During the parsing of an update the receiver has to update 1 or both hashes for each block of the update.
// Therefore 2 hash contexts are required.

typedef enum
{
    SEGMENTED_HASH_1 = 0,
    SEGMENTED_HASH_2,
    NUM_OF_SEGMENTED_HASHES,

    SEGMENTED_HASH_WHOLE_UPDATE = SEGMENTED_HASH_1,
    SEGMENTED_HASH_ADD_MODIFY = SEGMENTED_HASH_2
} SegmentedHashContexts;


// Calculates a simple sha256 hash over a block of data. Underlying solution controlled by #define
bool cryptoCalculateSha256Start(
    SegmentedHashContexts context,
    const uint8_t   *pInput,         // Data to be hashed.
    const uint32_t   length                 // byte length of the data.
    );

bool cryptoCalculateSha256MiddleEnd(
    SegmentedHashContexts context,
    const uint8_t   *pInput,         // Data to be hashed. (can be PNULL if no more data to be added).
    const uint32_t   length,                // byte length of the data.
    bool            final,                  // true if final segment.
    Sha256          *pHash                  // Pointer to a block of memory into which the result can be returned.
    );

extern bool sha256_calc_hash( const uint8_t *pData, uint32_t length, uint8_t *pHash);



#ifdef __cplusplus
}
#endif

#endif
