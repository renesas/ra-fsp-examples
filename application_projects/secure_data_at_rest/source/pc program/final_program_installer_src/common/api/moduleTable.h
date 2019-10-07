

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


/*
 * moduleTable.h
 *
 *  Created on: 3 Mar 2016
 *      Author: paul
 */

#ifndef SRC_MODULETABLE_H_
#define SRC_MODULETABLE_H_

// For use in C++
#ifdef __cplusplus
extern "C" {
#endif


/* **************************************************************************************
 * A module table serves the following purposes :-
 * a) to validate the integrity of modules.
 * b) to identify the locations of modules and their entry points.
 * c) to authenticate updates to the modules.
 * d) to be a root of trust for the addition of new modules.
 *
 *
 * The system contains upto 4 module tables.
 * Id : Location      : Purpose
 * 1  : within kernel : Signature across the kernel to self validate integrity from flash errors.
 *                    : System level root of trust. Can authorise any updates.
 * 2  : within 1st rx : to validate the 1st receiver. (Enables 1st rx to survive corruption of main module table.
 * 3  : within oem rx : [as for 1st rx]. [TODO - wrinkle of delegation module].
 * 4  : main module table : contains details of all the application modules.
 * **************************************************************************************
 */

typedef struct
{
    bool            moduleEntryUsed;    // True if the entry is in use.
    ModuleId        moduleId;
    uint32_t        moduleVersionNumber;// Opaque version number of the module.
    uint32_t        moduleFlashAddr;    // Base address in flash for the module.
    uint32_t        moduleSizeBytes;    // Byte length of the module. 0=> delegation module.
    uint32_t        moduleDecryptAddr;  // Address in ram to decrypt module to. 0 => run from flash.
    EccPublicKey    moduleSigKey;       // Key to check signature of module.
    EccSignature    moduleSig;          // Signature over code + moduleUpdateKey. Uses moduleSigKey.
    EccPublicKey    moduleUpdateKey;    // Updates to the module are signed with this key.
} ModuleTableEntry;

typedef EccSignature ModuleTableSignature;  // Signature across the whole module table.

#define MODULE_TABLE_MAGIC_VALUE            (0x9865ae9a)

#define MODULE_TABLE_DEFINITION(nAME, sIZE)    \
typedef struct                                 \
{                                              \
    ModuleTableSignature    sig;               \
    uint32_t                magic;             \
    uint8_t                 numEntries;        \
    ModuleTableEntry        entries[sIZE];     \
} nAME;


#ifdef __cplusplus
MODULE_TABLE_DEFINITION(ModuleTableGeneric, 1)  // should an empty array, but c++ can't cope with that...
#else
MODULE_TABLE_DEFINITION(ModuleTableGeneric, /*empty*/)
#endif
MODULE_TABLE_DEFINITION(ModuleTableKernel, 1)
MODULE_TABLE_DEFINITION(ModuleTableMain, MODULE_TABLE_NUM_ENTRIES_IN_MAIN)
MODULE_TABLE_DEFINITION(ModuleTable1stRx, MODULE_TABLE_NUM_ENTRIES_IN_1ST_RX)
MODULE_TABLE_DEFINITION(ModuleTableOemRx, MODULE_TABLE_NUM_ENTRIES_IN_OEM_RX)

#define MODULE_TABLE_SIG_START(tABLE)   (const uint8_t*)&(tABLE->magic)
#define MODULE_TABLE_SIG_LENGTH(tABLE)  ((const uint32_t) (offsetof(ModuleTableGeneric, entries[tABLE->numEntries]) - offsetof(ModuleTableGeneric, magic)))


#if defined _WIN32
extern ModuleTableKernel    moduleTableKernel;
#endif

// At startup, all the module tables are validated (including verifying the integrity of all the modules contained
// within each table).
// These flags set according to the status of each module table.
// They are used in the invocation decision process.
typedef struct
{
    bool kernelValid;
    bool mainValid;
    bool rx1stValid;
    bool oemRecoveryValid;
}ModuleTableValidityFlags;

/* **************************************************************************************
 * Initialisation.
 * **************************************************************************************
*/

// Called by security kernel startup to initialise the module table.
extern bool moduleTableInitialiseSystem(void);

// Called to initialise the ram shadow copy a specific module table.
// Marks all entries within the table to be unused.
extern bool moduleTableInitialiseModuleTable(ModuleTableGeneric *pTable, uint8_t numEntries);

// Full initialises the main module table, including committing it to flash.
// Used to reinitialise the main module if it is found to be corrupt.
extern bool moduleTableInitialiseMainModuleTable(void);

/* **************************************************************************************
 * Validation functions
 * **************************************************************************************
 */

// Validates all the module tables and selects something to invoke.
// Returns start address of module to invoke. Application is launched by loading SP from that address and PC from address+4.
// Returns NULL for failure.
extern uint32_t *moduleTableValidateTables(void);

// Validate the integrity of a specified module table.
extern bool moduleTableValidateSingleModuleTableSignature(const ModuleTableGeneric *pTable, const char* name);
// Validate the integrity of a specified module table.
// If the table is valid it returns the invocation address of the first valid module within the table.
extern bool moduleTableValidateSingleModuleTable(const ModuleTableGeneric *pTable, uint32_t **ppInvokeAddr, const char* name);
/* **************************************************************************************
 * Entry update functions
 * **************************************************************************************
 */

// Called to start an update.
// Copies all the module tables from flash into their ram shadows.
// If any table is invalid, it initialises the ram shadow.
extern bool moduleTableStartUpdate(void);

// Deletes an entry from the specified ram shadow module table.
// Returns false if the id is out of the range supported by any module table, or is a read-only module.
// Accepts deletes of inactive modules.
extern bool moduleTableDeleteEntry(ModuleId entryToDelete);

// Adds/modifies an entry in a module table.
// Returns false if the id is out of the range supported by any module table, or is a read-only module.
//
// Does NOT check the integrity of the module. That will be done on the after the next reset.
extern bool moduleTableUpdateEntry(
        ModuleId entryToModify,         // Module id of the entry to add/modify
        uint32_t entryNewVersion,       // version number of the new version of the module
        uint32_t entryFlashAddr,        // address in flash of the module.
        uint32_t entrySizeBytes,        // length of the module in bytes. 0==> delegation module.
        uint32_t entryDecryptAddr,      // Address in ram to decrypt module to. 0 => run from flash.
        EccPublicKey *pEntrySigKey,     // key to check signature of module.
        EccSignature *pEntrySig,        // sig of code + moduleUpdateKey.
        EccPublicKey *pEntryUpdateKey   // key used to sign update.
        );


// For each shadow module table
//    if it has been modified, then sign it and commit it to flash.
// (For oemRx and 1stRx it handles the merging of the module table with the module).
extern bool moduleTableFinaliseUpdate(void);


/* **************************************************************************************
 * Cross-validation functions for validating updates.
 * **************************************************************************************
 */

// Returns true if (a) specified module exists and (b) its MUK matches the supplied.
extern bool moduleTableCheckModuleUpdateKey(const ModuleId moduleId, EccPublicKey * const pMUKtoCheck);




/* **************************************************************************************
 * Get the module version info required by the Update GUI
 * NB: This function is called in the application context and hence it uses the application
 * stack. Hence it must not use any kernel ram structures or peripherals.
 * **************************************************************************************
 */

// Caller must provide a pointer to an array of sufficient entries for all the modules.
// Returns the number of used entries in the array.
extern uint32_t moduleTableGetModuleVersions(ModuleVersionRecord *pInfo);

#ifdef __cplusplus
}
#endif

#endif /* SRC_MODULETABLE_H_ */
