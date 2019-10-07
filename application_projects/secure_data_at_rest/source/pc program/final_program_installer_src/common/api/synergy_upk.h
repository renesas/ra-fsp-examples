

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


#if !defined SYNERGY_UPK_H
#define SYNERGY_UPK_H

// To constrain the size of UPK the number of signatures is limited to 5.
#define SYNERGY_UPDATE_MAX_SIGS  5



typedef struct
{
    EccSignature signature;   // ECC signature over a blob.
    EccPublicKey publicKey;   // The ECC public key used to verify the signature.
} EccSignaturePair;


// Defines the contents of the unencrypted update protection key (UPK) structure.
// This structure is encrypted with ECIES for transport.
// Its purpose is to deliver the 16 byte AES key required to decrypt the payload of the update.
// See Synergy Security Architecture Document.
typedef struct
{
    uint8_t             version;                        // version number of the messsage format.
    AesKey              aesKey;                         // randomly generated AES key. Used to encrypt the update.
    HmacKey             hmacKey;                        // randomly generated HAMC key. Used to validate the update.
    uint8_t             sigModNum;                      // number of the module used to sign this UPK.
    uint8_t             nsig;                           // number of signatures over this and preceeding fields of the UPK.
    EccSignaturePair    sigs[SYNERGY_UPDATE_MAX_SIGS];  // Values of the signatures.
} UpdateProtectionKey;

#endif
