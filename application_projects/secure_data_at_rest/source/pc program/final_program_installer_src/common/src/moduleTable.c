/***********************************************************************************************************************
* Copyright [2015] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
*
* The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
* and/or its licensors ("Renesas") and subject to statutory and contractual protections.
*
* Unless otherwise expressly agreed in writing between Renesas and you: 1) you may not use, copy, modify, distribute,
* display, or perform the contents; 2) you may not use any name or mark of Renesas for advertising or publicity
* purposes or in connection with your use of the contents; 3) RENESAS MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE
* SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED eAS ISf WITHOUT ANY EXPRESS OR IMPLIED
* WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND
* NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR CONSEQUENTIAL DAMAGES,
* INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF CONTRACT OR TORT, ARISING
* OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents included in this file may
* be subject to different terms.
**********************************************************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include "crypto.h"
#include "deviceCertificate.h"
#include "moduleInfo.h"
#include "moduleTable.h"
#include "memoryChecks.h"
#include "staticAssert.h"
#include "memoryMap.h"
#if !defined _WIN32
#include "flash.h"
#include "S5D9.h"
#include "buttons.h"
#include "securityUser.h"
#endif
#include "printfMapping.h"


#define MODULE_FLASH_PSUEDO_ADDR_FOR_DELEGATION_MODULE  1

// Create the module tables.
#if defined _WIN32
// Module table is compiled on the PC for 2 purposes.
// 1. for some host based tests of the module table functionality.
// 2. within the kernelPackager to securely package the kernel + 1stRx.
// To link on the PC stubs are required for functionality required provided by other parts of the kernel.

SynergySeed         synergySeed = { 1,2,3 };  // Any non-zero dummy seed sufficient for the PC test task.
ModuleTableKernel   moduleTableKernel;
static ModuleTableMain     moduleTableMain;
uint8_t             dummy1stRx[32 * 1024];
uint8_t             dummyOemRx[64 * 1024];
#else
// Create the module tables and use the linker to force them to the correct locations in memory.
static const ModuleTableMain    __attribute__ ((section(".moduleTableMain")))  moduleTableMain;
static const ModuleTableKernel  __attribute__ ((section(".moduleTableKernel"))) moduleTableKernel;
#endif

// Verify that the table fit into their specified spaces.
STATIC_ASSERT(sizeof(ModuleTableKernel) < SIZE_OF_KERNEL_MT);
STATIC_ASSERT(sizeof(ModuleTableMain)   < SIZE_OF_MAIN_MT);
STATIC_ASSERT(sizeof(ModuleTable1stRx)  < SIZE_OF_1ST_RX_MT);
STATIC_ASSERT(sizeof(ModuleTableOemRx)  < SIZE_OF_OEM_RX_MT);


// Shadow copies of the module tables for performing updates.
// Updates are assembled in here and if the whole update is good, then they are applied to flash.
typedef struct
{
    bool                 updateInProgress;      // overall Update control flag.

    bool                 mainModified;          // set to true if any entry in the main module table is modified.
    ModuleTableMain      moduleTableMainUpdate; // Shadow of the main module table.

    bool                 rx1stModified;
    ModuleTable1stRx     moduleTable1stRxUpdate;

    bool                 oemRxModified;
    ModuleTableOemRx     moduleTableOemRxUpdate;
}ModuleTableUpdate;


// Stored in memory shared with the application. They are only used in kernel mode, when no application is executing.
// They are invalidated before any application is launched.
static ModuleTableUpdate update;
ModuleTableValidityFlags validityFlags;

bool moduleTableInitialiseSystem(void)
{
    update.updateInProgress = false;
    memset(&update, 0, sizeof(ModuleTableUpdate));
    memset(&validityFlags, 0, sizeof(ModuleTableValidityFlags));
    return true;
}

// initialize a module table with the specified number of entries.
// all entries marked as empty.
// Table is signed.
// Does NOT write the table to flash.
bool moduleTableInitialiseModuleTable(ModuleTableGeneric *pTable, uint8_t numEntries)
{
    EccPrivateKey privateKey;
    EccPublicKey  publicKey;

    pTable->magic = MODULE_TABLE_MAGIC_VALUE;
    pTable->numEntries = numEntries;
    for (uint32_t i = 0; i < numEntries; i++)
    {
        pTable->entries[i].moduleEntryUsed = false;
    }

    cryptoDeriveEccKey((const uint8_t*)synergySeed, CRYPTO_DERIVATION_STRING_MT_SIG, &publicKey, &privateKey);
    cryptoEccHashAndSign(MODULE_TABLE_SIG_START(pTable), MODULE_TABLE_SIG_LENGTH(pTable), NULL, 0, (const EccPrivateKey*) &privateKey, &pTable->sig);

    return true;
}

// If the main module table is invalid, then this function is called to create a new one and commit it to flash.
bool moduleTableInitialiseMainModuleTable(void)
{
    bool retval = true;

    retval &= moduleTableStartUpdate();
    retval &= moduleTableInitialiseModuleTable((ModuleTableGeneric*)&update.moduleTableMainUpdate, MODULE_TABLE_NUM_ENTRIES_IN_MAIN);
    update.mainModified = true;
    retval &= moduleTableFinaliseUpdate();

    return retval;
}

// Validates a single entry within a module table.
// Return true if the entry is consistent.
// Additionally returns the invocation address if there is one. Otherwise NULL.
static bool moduleTableValidateEntry(uint8_t moduleId, const ModuleTableEntry *pEntry, uint32_t *pInvokeAddr)
{
    // pre-assume it is not invokable.
    *pInvokeAddr = 0;

#if defined DISABLE_PRINTF
    UNUSED(moduleId);
#endif

    if (!pEntry->moduleEntryUsed)
    {
        // empty entry, so all is ok.
        TRACE("[MT] - %d - empty module\n", moduleId);
        return true;
    }

    // Magic value of 1 ==> delegation certificate.
    if (MODULE_FLASH_PSUEDO_ADDR_FOR_DELEGATION_MODULE == pEntry->moduleFlashAddr)
    {
        // TODO - complete support for delegation modules.
        if (0 != pEntry->moduleSizeBytes)
        {
            TRACE("[MT] - %d - delegation module FAILED,\n", moduleId);
            return false;
        }
        else if (cryptoEccHashAndVerify((const uint8_t*)pEntry->moduleUpdateKey, sizeof(EccPublicKey), NULL, 0, &pEntry->moduleSigKey, &pEntry->moduleSig))
        {
            TRACE("[MT] - %d - v%d - validated delegation module.\n", moduleId, (int)pEntry->moduleVersionNumber);
            return true;
        }
        else
        {
            TRACE("[MT] - %d - delegation module FAILED.\n", moduleId);
            return false;
        }
    }

    // It is a real module.
    // Check that its address is valid.
    if (!memoryChecksIsBufferInRom((const uint8_t*)pEntry->moduleFlashAddr, pEntry->moduleSizeBytes))
    {
        TRACE("[MT] - %d - module doesn't appear to be in flash\n", moduleId);
        return false;
    }
    // Check signature of the module.
#if !defined _WIN32
    // In windows simulation the module doesn't actually exist so we can't verify its signature.....
    else if (!cryptoEccHashAndVerify(
        (const uint8_t*)pEntry->moduleFlashAddr, pEntry->moduleSizeBytes,
        (const uint8_t*)pEntry->moduleUpdateKey, sizeof(EccPublicKey),
        &pEntry->moduleSigKey, &pEntry->moduleSig))
    {

        TRACE("[MT] - %d - module FAILED sig check\n", moduleId);
        return false;
    }
#endif
    else if (0 == pEntry->moduleDecryptAddr)
    {
        TRACE("[MT] - %d - v%d - module validated - startAddr=0x%x, length=%d\n",
               moduleId, (int)pEntry->moduleVersionNumber, (int)pEntry->moduleFlashAddr, (int)pEntry->moduleSizeBytes);
        *pInvokeAddr = pEntry->moduleFlashAddr;
        return true;
    }
    // module should be decrypted to ram. check that the target address range fits into ram....
    else if (!memoryChecksIsBufferInRam((uint8_t *)pEntry->moduleDecryptAddr, pEntry->moduleSizeBytes))
    {
        TRACE("[MT] - %d - module decrypt target invalid\n", moduleId);
        return false;
    }
    else
    {
        TRACE("[MT] - %d - module is decryptable to ram.....\n", moduleId);
        *pInvokeAddr = pEntry->moduleDecryptAddr;
        // Todo - add support for decrypt of modules.
        return true;
    }
}


// Validate the integrity of a specified module table.
// If the table is valid it returns the invocation address of the first valid module within the table.
bool moduleTableValidateSingleModuleTable(const ModuleTableGeneric *pTable, uint32_t **ppInvokeAddr, const char* name)
{
    bool            retval = false;
    uint32_t        tmpInvokeAddr;
    
#if defined DISABLE_PRINTF
    UNUSED(name);
#endif

    // pre-assume it is not invokable.
    *ppInvokeAddr = 0;

    TRACE("[MT] - Validating %s module table\n", name);

    if (true != moduleTableValidateSingleModuleTableSignature(pTable, name))
    {
        TRACE("[MT] -Module Table Signature Verification failed\n");
    }
    else
    {
        // validate every module within the table.
        retval = true;
        for (uint8_t i = 0; (true == retval) && (i < pTable->numEntries); i++)
        {
            if (!moduleTableValidateEntry(i, &pTable->entries[i], &tmpInvokeAddr))
            {
                retval = false;
            }

            // If this is the first invokable module found in this module table, record its invocation address.
            if ((0 != tmpInvokeAddr) && (0 == *ppInvokeAddr))
            {
                *ppInvokeAddr = (uint32_t*)tmpInvokeAddr;
            }
        }
    }

    TRACE("[MT] - %s module table %sVALID\n\n", name, retval ? "" : "IN");
    return retval;
}

// Validate the integrity of a specified module table.
bool moduleTableValidateSingleModuleTableSignature(const ModuleTableGeneric *pTable, const char* name)
{
    bool            retval = true;
    EccPrivateKey   privateKey;
    EccPublicKey    publicKey;

#if defined DISABLE_PRINTF
    UNUSED(name);
#endif


    TRACE("[MT] - Validating %s module table\n", name);

    if (MODULE_TABLE_MAGIC_VALUE != pTable->magic)
    {
        TRACE("[MT] - magic invalid\n");
        retval = false;
    }
    else if  (!cryptoDeriveEccKey((const uint8_t*)synergySeed, CRYPTO_DERIVATION_STRING_MT_SIG, &publicKey, &privateKey))
    {
        TRACE("[MT] - key derivation failed\n");
    }
    else if (!cryptoEccHashAndVerify(MODULE_TABLE_SIG_START(pTable), MODULE_TABLE_SIG_LENGTH(pTable), NULL, 0, (const EccPublicKey*)&publicKey, &pTable->sig))
    {
        TRACE("[MT] - table sig failed.\n");
    }

    TRACE("[MT] - %s module table signature %sVALID\n\n", name, retval ? "" : "IN");
    return retval;
}


// Validate all the module tables (except Security Kernel) and then take the complex decision of what to invoke.
uint32_t  *moduleTableValidateTables(void)
{
    uint32_t *mainInvokeAddr;
    uint32_t *rx1stInvokeAddr;
    uint32_t *oemRecoveryInvokeAddr;

    // validate all the tables.
    validityFlags.mainValid = moduleTableValidateSingleModuleTable((const ModuleTableGeneric*)&moduleTableMain, &mainInvokeAddr, "main");
#if !defined _WIN32
    validityFlags.rx1stValid = moduleTableValidateSingleModuleTable((const ModuleTableGeneric*)ADDR_1ST_RX_MT, &rx1stInvokeAddr, "1stRx");
    validityFlags.oemRecoveryValid = moduleTableValidateSingleModuleTable((const ModuleTableGeneric*)ADDR_OEM_RX_MT, &oemRecoveryInvokeAddr, "oemRecovery");
#else
    validityFlags.rx1stValid = false;
    validityFlags.oemRecoveryValid = false;
    rx1stInvokeAddr = 0;
    oemRecoveryInvokeAddr = 0;
#endif

    // The kernel should have already been validated outside this function before getting here
    if (!validityFlags.kernelValid)
    {
        TRACE("[MT] - kernel is INVALID. I am a brick\n");
        return NULL;
    }


#if !defined _WIN32
    // In development builds we need to provide an escape mechanism if the user loads a broken application.
    // If button1 or 2 is held down at boot time, then the 1stRx or oemRx respectively are invoked.
    if (true == SecurityUserForceExecution(MODULE_1ST_RECEIVER))
    {
        TRACE("[MT] = button 1 pressed at boot ");
        if (validityFlags.rx1stValid)
        {
            TRACE("and 1st rx is valid, so selecting it\n");
            return rx1stInvokeAddr;
        }
        else
        {
            TRACE("but 1st rx is NOT valid\n");
        }
    }
    if (true == SecurityUserForceExecution(MODULE_OEM_RECEIVER))
    {
        TRACE("[MT] = button 2 pressed at boot ");
        if (validityFlags.oemRecoveryValid)
        {
            TRACE("and OEM Rx is valid, so selecting it\n");
            return oemRecoveryInvokeAddr;
        }
        else
        {
            TRACE("but oem rx is NOT valid\n");
        }
    }
#endif

    if(validityFlags.mainValid)
    {
        if (mainInvokeAddr)   // if there is something invokable in the main, select it.
        {
            TRACE("[MT] - Selecting program from main module table.\n");
            return mainInvokeAddr;
        }
    }
    else
    {
        TRACE("[MT] - validation of main module table FAILED. Reinitialising\n");
        moduleTableInitialiseMainModuleTable();
        TRACE("[MT] - reinitialised main module table.\n\n");
    }

    if ((validityFlags.oemRecoveryValid) && (0 != oemRecoveryInvokeAddr))
    {
        TRACE("[MT] - Selecting program from OEM recovery\n");
        return oemRecoveryInvokeAddr;
    }

    if ((validityFlags.rx1stValid) && (0 != rx1stInvokeAddr))
    {
        TRACE("[MT] - Selecting program from 1st Rx\n");
        return rx1stInvokeAddr;
    }

    TRACE("[MT] - tried all module tables. Found nothing invokable! I am a brick!\n");
    return NULL;
}


// Make updatable shadow copies of the module tables.
bool moduleTableStartUpdate(void)
{
    if (update.updateInProgress)
    {
        TRACE("Failed - attempt to start an update when one already in progress.\n");
        return false;
    }

    update.updateInProgress = true;

    update.mainModified = false;
    update.rx1stModified = false;
    update.oemRxModified = false;
    memcpy(&update.moduleTableMainUpdate, &moduleTableMain, sizeof(ModuleTableMain));
    memcpy(&update.moduleTable1stRxUpdate, (uint32_t*)ADDR_1ST_RX_MT, sizeof(ModuleTable1stRx));
    memcpy(&update.moduleTableOemRxUpdate, (uint32_t*)ADDR_OEM_RX_MT, sizeof(ModuleTableOemRx));


    // If the updatable tables are invalid, reinitialise the update versions.
    //
    // This does NOT mean the tables will actually be updated. If a module within the table is updated, then the
    // modified flag will be set...and hence at the commit point the relevant table will be signed and committed.
    // Otherwise the update version is simply abandoned.
    if (!validityFlags.mainValid)
    {
        moduleTableInitialiseModuleTable((ModuleTableGeneric *)&update.moduleTableMainUpdate, MODULE_TABLE_NUM_ENTRIES_IN_MAIN);
    }

    if (!validityFlags.rx1stValid)
    {
        moduleTableInitialiseModuleTable((ModuleTableGeneric *)&update.moduleTable1stRxUpdate, MODULE_TABLE_NUM_ENTRIES_IN_1ST_RX);
    }

    if (!validityFlags.oemRecoveryValid)
    {
        moduleTableInitialiseModuleTable((ModuleTableGeneric *)&update.moduleTableOemRxUpdate, MODULE_TABLE_NUM_ENTRIES_IN_OEM_RX);
    }

    return true;
}

// From the moduleId find the appropriate entry in the appropriate updatable shadow module table.
// Used to stage add/modify/delete of modules.
static ModuleTableEntry *moduleTableGetUpdateEntryFromIdModifiable(uint32_t moduleId)
{
    if (!update.updateInProgress)
    {
        TRACE("[MT] - update not it progress\n");
        return NULL;
    }

    switch (moduleId & MODULE_TABLE_ID_MASK)
    {
        case MODULE_TABLE_ID_KERNEL:
            switch((moduleId))
            {
                case MODULE_ID_SECURITY_KERNEL:
                default:
                    // kernel is not updatable. Nor are unallocated ids.
                    return NULL;
                    break;
                case MODULE_ID_1ST_RX:
                    // TODO - 1stRx was designed to be modifiable.
                    // However, for the evaluation it was requested that 1stRx is immutable.
                    return NULL;
                    //update.rx1stModified = true;
                    //return &update.moduleTable1stRxUpdate.entries[0];
                    break;
            }
            break;
        case MODULE_TABLE_ID_OEM_RX:
            moduleId &= MODULE_ID_MASK;
            if (moduleId < MODULE_TABLE_NUM_ENTRIES_IN_OEM_RX)
            {
                update.oemRxModified = true;
                return &update.moduleTableOemRxUpdate.entries[moduleId];
            }
            break;
        case MODULE_TABLE_ID_MAIN:
        case MODULE_TABLE_ID_MAIN2:
            moduleId &= MODULE_ID_MASK;
            if (moduleId < MODULE_TABLE_NUM_ENTRIES_IN_MAIN)
            {
                update.mainModified = true;
                return &update.moduleTableMainUpdate.entries[moduleId];
            }
            break;
        default:
            break;
    }
    TRACE("[MT] - unable to find a table entry for moduleId=%d\n", (int)moduleId);
    return NULL;
}

// From the moduleId find the appropriate entry in the appropriate constant module table.
// Used to find the MUK of an existing module to determine if an update is valid.
static const ModuleTableEntry *moduleTableGetUpdateEntryFromIdConst(uint32_t moduleId)
{
    switch (moduleId & MODULE_TABLE_ID_MASK)
    {
        case MODULE_TABLE_ID_KERNEL:
            switch(moduleId)
            {
                case MODULE_ID_1ST_RX:
                    ; //empty statement to get round compiler feature that variable declaration
                      // cannot be the first line after a case.
                    ModuleTable1stRx *p1stRx = (ModuleTable1stRx*) ADDR_1ST_RX_MT;
                    return &p1stRx->entries[0];
                    break;
                case MODULE_ID_SECURITY_KERNEL:
                    return &moduleTableKernel.entries[0];
                    break;
                default:
                    // do nothing. fall out to the main error handler.
                    break;
            }
            break;
        case MODULE_TABLE_ID_OEM_RX:
            moduleId &= MODULE_ID_MASK;
            ModuleTableOemRx *pOemRx = (ModuleTableOemRx*) ADDR_OEM_RX_MT;
            if (moduleId < MODULE_TABLE_NUM_ENTRIES_IN_OEM_RX)
            {
                return &pOemRx->entries[moduleId];
            }
            break;
        case MODULE_TABLE_ID_MAIN:
        case MODULE_TABLE_ID_MAIN2:
            moduleId &= MODULE_ID_MASK;
            if (moduleId < MODULE_TABLE_NUM_ENTRIES_IN_MAIN)
            {
                return &moduleTableMain.entries[moduleId];
            }
            break;
        default:
            break;
    }
    TRACE("[MT] - unable to find a table entry for moduleId=%d\n", (int)moduleId);
    return NULL;

}

// Delete an entry from a module table.
bool moduleTableDeleteEntry(ModuleId entryToDelete)
{
    ModuleTableEntry *pEntry = moduleTableGetUpdateEntryFromIdModifiable(entryToDelete);

    if (NULL == pEntry)
    {
        TRACE("[MT] - failed request to delete an invalid moduleId %d\n", (int) entryToDelete);
        return false;
    }

    if (!pEntry->moduleEntryUsed)
    {
        TRACE("[MT] - allowing delete of inactive entry %d\n", entryToDelete);
    }
    pEntry->moduleEntryUsed = false;

    return true;
}

// Add or modify an entry.
bool moduleTableUpdateEntry(
    ModuleId entryToModify,         // Module id of the entry to add/modify
    uint32_t entryNewVersion,       // version number of the new version of the module
    uint32_t entryFlashAddr,        // address in flash of the module.
    uint32_t entrySizeBytes,        // length of the module in bytes. 0==> delegation module.
    uint32_t entryDecryptAddr,      // Address in ram to decrypt module to. 0 => run from flash.
    EccPublicKey *pEntrySigKey,     // key to check signature of module.
    EccSignature *pEntrySig,        // sig of code + moduleUpdateKey.
    EccPublicKey *pEntryUpdateKey   // key used to sign update.
    )
{
    ModuleTableEntry *pEntry = moduleTableGetUpdateEntryFromIdModifiable(entryToModify);

    if (NULL == pEntry)
    {
        TRACE("[MT] - failed request to modify an invalid moduleId %d\n", (int) entryToModify);
        return false;
    }

    // Assumes that the caller has done ALL the validation and that this is simply an order to apply an update.
    pEntry->moduleEntryUsed = true;
    pEntry->moduleId = entryToModify;
    pEntry->moduleVersionNumber = entryNewVersion;
    pEntry->moduleFlashAddr = entryFlashAddr;
    pEntry->moduleSizeBytes = entrySizeBytes;
    pEntry->moduleDecryptAddr = entryDecryptAddr;
    memcpy(&pEntry->moduleSigKey, pEntrySigKey, ECC_PUBLIC_KEY_SIZE);
    memcpy(&pEntry->moduleSig, pEntrySig, ECC_SIGNATURE_SIZE);
    memcpy(&pEntry->moduleUpdateKey, pEntryUpdateKey, ECC_PUBLIC_KEY_SIZE);

    return true;
}


// Commit a table to flash.
static bool moduleTableCommitTable(uint32_t dst, uint16_t *pSrc, uint32_t length, const char *name)
{
#if defined DISABLE_PRINTF
    UNUSED(name);
#endif
#if defined _WIN32
    memcpy((uint8_t*)dst, pSrc, length);
    return true;
#else
    if (!flash_code_erase(dst, dst + length))
    {
        ERROR("[MT] - failed to erase %s module table 0x%x to 0x%x\n", name, (int)dst, (int)(dst+length));
    }
    else if (!flash_code_program(dst, pSrc, (1 + length)/2 ))
    {
        ERROR("[MT] - failed to program %s module table\n", name);
    }
    else if (!flash_code_verify(dst, pSrc, (1 + length)/2 ))
    {
        ERROR("[MT] - failed to verify update to %s module table\n", name);
    }
    else
    {
        return true;
    }
    return false;
#endif
}

// oemRx and 1stRx have module code and module table sharing a single flash page.
// During the processing of the update message, the code image will have been committed to flash.
// To update the module table, the whole flash page needs to be copied into a ram cache,
// then the module table merged into the ram image and then the whole page written back to flash.
// This array is the ram cache.
static uint16_t mtUpdateCache[OEM_RX_MT_PAGE_SIZE / 2];


// Finalise an update.
// For each shadow table that has been updated
//     sign it.
//     commit it to flash.
bool moduleTableFinaliseUpdate(void)
{
    bool retval = true;

    // prepare keys to sign the tables
    EccPublicKey  publicTableSigKey;
    EccPrivateKey privateTableSigKey;
    cryptoDeriveEccKey((const uint8_t*)synergySeed, CRYPTO_DERIVATION_STRING_MT_SIG, &publicTableSigKey, &privateTableSigKey);

    // sign the tables and commit them to flash
    if (update.mainModified)
    {
        ModuleTableMain *pMt = &update.moduleTableMainUpdate;
        cryptoEccHashAndSign(MODULE_TABLE_SIG_START(pMt), MODULE_TABLE_SIG_LENGTH(pMt), NULL, 0, (const EccPrivateKey*)&privateTableSigKey, &pMt->sig);
        retval &= moduleTableCommitTable((uint32_t)&moduleTableMain, (uint16_t*)pMt, sizeof(ModuleTableMain), "main");
    }

    if (update.rx1stModified)
    {
        ModuleTable1stRx *pMt = &update.moduleTable1stRxUpdate;
        cryptoEccHashAndSign(MODULE_TABLE_SIG_START(pMt), MODULE_TABLE_SIG_LENGTH(pMt), NULL, 0, (const EccPrivateKey*)&privateTableSigKey, &pMt->sig);

        // combine the module + the moduleTable into a ram buffer and then commit them to flash.
        memcpy(mtUpdateCache, (void*)ADDR_1ST_RX_MT_PAGE, OFFSET_1ST_RX_MT);
        memcpy(&mtUpdateCache[OFFSET_1ST_RX_MT/2], pMt, SIZE_OF_1ST_RX_MT);
        retval &= moduleTableCommitTable(ADDR_1ST_RX_MT_PAGE, mtUpdateCache, FIRST_RX_MT_PAGE_SIZE, "1stRx");
    }

    if (update.oemRxModified)
    {
        ModuleTableOemRx *pMt = &update.moduleTableOemRxUpdate;
        cryptoEccHashAndSign(MODULE_TABLE_SIG_START(pMt), MODULE_TABLE_SIG_LENGTH(pMt), NULL, 0, (const EccPrivateKey*)&privateTableSigKey, &pMt->sig);

        // combine the module + the moduleTable into a ram buffer and then commit them to flash.
        memcpy(mtUpdateCache, (void*)ADDR_OEM_RX_MT_PAGE, OFFSET_OEM_RX_MT);
        memcpy(&mtUpdateCache[OFFSET_OEM_RX_MT/2], pMt, SIZE_OF_OEM_RX_MT);
        retval &= moduleTableCommitTable(ADDR_OEM_RX_MT_PAGE, mtUpdateCache, OEM_RX_MT_PAGE_SIZE, "oemRx");
    }

    update.updateInProgress = false;
    return retval;
}


// An update is only allowed if it contains the MUK of an existing module.
// This function verifies that the MUK provided in an update matches the MUK of the specified module.
bool moduleTableCheckModuleUpdateKey(const ModuleId moduleId, EccPublicKey * const pMUKtoCheck)
{
	const ModuleTableEntry *pEntry = moduleTableGetUpdateEntryFromIdConst(moduleId);

	if (NULL == pEntry)
	{
		TRACE("moduleTableCheckModuleUpdateKey - unable to find module=%d\n", (int) moduleId);
		return false;
	}
	else if (!pEntry->moduleEntryUsed)
	{
		TRACE("moduleTableCheckModuleUpdateKey - module is inactive=%d\n", (int) moduleId);
		return false;
	}
	else if (0 != memcmp(pMUKtoCheck, pEntry->moduleUpdateKey, sizeof(EccPublicKey)))
	{
		TRACE("moduleTableCheckModuleUpdateKey - MUK does not match\n");
		return false;
	}
	return true;
}












// This function is called from the application to get the version numbers of every module installed on the device.
// It must NOT access kernel local variables.
// It must NOT produce any tracing (since PMODs are owned by the app, not the kernel).
static uint32_t mtGetVersionInfo(const ModuleTableGeneric *pTable, ModuleVersionRecord *pInfo)
{
    uint32_t numModules = 0;

    // Assumes the module tables are valid, which they must have been at boot time.
    if (MODULE_TABLE_MAGIC_VALUE == pTable->magic)
    {
        for (uint32_t i=0; i<pTable->numEntries; i++)
        {
            if (pTable->entries[i].moduleEntryUsed)
            {
                numModules++;
                pInfo->module = pTable->entries[i].moduleId;
                pInfo->version = pTable->entries[i].moduleVersionNumber;
                pInfo++;
            }
        }
    }
    return numModules;
}

// This function is called from the application.
// It must NOT access kernel local variables.
// It must NOT produce any tracing (since PMODs are owned by the app, not the kernel).

// Application level function that returns a list of the current version number of all installed modules.
// in: pInfo points to an array with sufficient size for all the module version info.
// return : count of the number of modules.
uint32_t moduleTableGetModuleVersions(ModuleVersionRecord *pInfo)
{
    uint32_t numModules = 0;

    numModules += mtGetVersionInfo((const ModuleTableGeneric*) ADDR_KERNEL_MT, &pInfo[numModules]);
    numModules += mtGetVersionInfo((const ModuleTableGeneric*) ADDR_MAIN_MT, &pInfo[numModules]);
    numModules += mtGetVersionInfo((const ModuleTableGeneric*) ADDR_1ST_RX_MT, &pInfo[numModules]);
    numModules += mtGetVersionInfo((const ModuleTableGeneric*) ADDR_OEM_RX_MT, &pInfo[numModules]);
    return numModules;
}
