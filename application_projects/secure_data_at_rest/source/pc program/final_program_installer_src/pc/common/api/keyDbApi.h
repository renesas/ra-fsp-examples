

/***********************************************************************************************************************
* Copyright [2015] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
*
* The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
* and/or its licensors ("Renesas") and subject to statutory and contractual protections.
*
* Unless otherwise expressly agreed in writing between Renesas and you: 1) you may not use, copy, modify, distribute,
* display, or perform the contents; 2) you may not use any name or mark of Renesas for advertising or publicity
* purposes or in connection with your use of the contents; 3) RENESAS MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE
* SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED ÅeAS ISÅf WITHOUT ANY EXPRESS OR IMPLIED
* WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND
* NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR CONSEQUENTIAL DAMAGES,
* INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF CONTRACT OR TORT, ARISING
* OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents included in this file may
* be subject to different terms.
**********************************************************************************************************************/

/*
*  API definition for the Key Database
*/

#ifndef KEYDBAPI_H_
#define KEYDBAPI_H_

#include <stdint.h>
#include <stdbool.h>
#include "crypto.h"

#define DEFAULT_KEYDB_FILE  "C:\\Renesas\\Synergy\\SecurityKernel\\keydb.csv"

#ifdef __cplusplus
extern "C" {
#endif

// Structure of records within the database.
typedef struct
{
  uint8_t  moduleId;
  uint32_t version;                // Internals of format of version number is unknown to kernel.
  EccPrivateKey sigPrivateKey;
  EccPublicKey  sigPublicKey;
  EccPrivateKey updatePrivateKey;
  EccPublicKey  updatePublicKey;
} KeyDbRecord;

// Initialise internal data structures.
// Afterwards, either a DB can be loaded from a file or a new databse
// can be constructed with calls to keyDbMakeRecord()
// Returns: true if successful, false otherwise.
extern bool keyDbInitialise(void);

// Reads a database from a file and checks the hash.
// Input: pFilename; the database file to load
// Returns: true if successful, false otherwise.
extern bool keyDbLoad(const char *pFilename);

// Generates SHA2 hash and writes the database to a file.
// Input: pFilename; the database file to write
// Returns: true if successful, false otherwise.
extern bool keyDbSave(const char *pFilename);

// Return the number of entries in the database
extern uint32_t keyDbGetNumRecords(void);

// Gets a record from the database.
// Inputs: moduleId, version; the composite key for the database record
// Output: pRecord; the database record will be copied into the memory pointed to by pRecord.
// Returns: true on success, false otherwise.
extern bool keyDbGetRecord(const uint8_t moduleId, const uint32_t version, KeyDbRecord *pRecord);

// Creates a new record in the database for the specified module/version and generates its crypto keys.
// Will fail if a record already exists with the given moduleId+version.
// Inputs: moduleId, version; the composite key for the database record
// Inputs: sigKeyVersion. 0=>make a new key, else copy key from the specified version of the same module.
// Inputs: updateKeyVersion. 0=>make a new key, else copy key from the specified version of the same module.
// Output: pRecord; the database record will be copied into the memory pointed to by pRecord. Supplied pRecord must be not NULL.
// Returns: true on success, false otherwise.
extern bool keyDbMakeRecord(const uint8_t moduleId, const uint32_t version, const uint32_t sigKeyVersion, const uint32_t updateKeyVersion, KeyDbRecord *pRecord);

// Creates a new record in the database for the specified module/version and generates its crypto keys.
// Will fail if a record already exists with the given moduleId+version.
// Inputs: moduleId, version; the composite key for the database record
// Inputs: Explicit values for the signing keys. Update keys are generated.
// Output: pRecord; the database record will be copied into the memory pointed to by pRecord. Supplied pRecord must be not NULL.
// Returns: true on success, false otherwise.
extern bool keyDbMakeRecordFixedSigningKey(const uint8_t moduleId, const uint32_t version, const uint8_t *pPublicSigningKey, const uint8_t *pPrivateSigningKey, KeyDbRecord *pRecord);

// Gets an array of version numbers for the given module ID
// Inputs: moduleId; the module ID being queried
// Output: pNumVersions; the number of versions in the returned array.
// Returns: pointer to version array on success, NULL otherwise.
// N.B: the versions array returned becomes the responsibility of the caller who
//      must free() that memeory when no longer required.
extern uint32_t * keyDbGetVersions(const uint8_t moduleId, uint32_t *pNumVersions);

#ifdef __cplusplus
}
#endif

#endif  // KEYDBAPI_H_
