

/***********************************************************************************************************************
* Copyright (c) 2015 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/ 

/*
*  keyDb test application
*/

#include <Windows.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include "stdafx.h"

#include "keyDbApi.h"

// define should come from mdduleTable.h .... but we don't want to include it in this test code.
#define MODULE_ID_1ST_RX  (2)

// Are the update keys the same?
static bool compareUpdateKeys(KeyDbRecord &recordA, KeyDbRecord &recordB, bool expectedEqual = true)
{
    bool result = false;
    bool areEqual = false;

    if (!memcmp(recordA.updatePrivateKey, recordB.updatePrivateKey, sizeof(recordA.updatePrivateKey)) &&
        !memcmp(recordA.updatePublicKey, recordB.updatePublicKey, sizeof(recordA.updatePublicKey)))
    {
        areEqual = true;
    }

    if (areEqual == expectedEqual)
    {
        result = true;
    }
    else
    {
        fprintf(stdout, "Update key comparison (%s) fail for moduleId[%d:%d] version[%d:%d]\n", expectedEqual ? "equal" : "diff", recordA.moduleId, recordB.moduleId, recordA.version, recordB.version);
    }

    return result;
}

// Are the signature keys the same?
static bool compareSigKeys(KeyDbRecord &recordA, KeyDbRecord &recordB, bool expectedEqual = true)
{
    bool result = false;
    bool areEqual = false;

    if (!memcmp(recordA.sigPrivateKey, recordB.sigPrivateKey, sizeof(recordA.sigPrivateKey)) &&
        !memcmp(recordA.sigPublicKey, recordB.sigPublicKey, sizeof(recordA.sigPublicKey)))
    {
        areEqual = true;
    }

    if (areEqual == expectedEqual)
    {
        result = true;
    }
    else
    {
        fprintf(stdout, "Signature key comparison (%s) fail for moduleId[%d:%d] version[%d:%d]\n", expectedEqual ? "equal" : "diff", recordA.moduleId, recordB.moduleId, recordA.version, recordB.version);
    }

    return result;
}

// Are all the keys the same?
static bool compareKeys(KeyDbRecord &recordA, KeyDbRecord &recordB, bool expectedEqual = true)
{
    bool result = false;
    bool areEqual = false;

    if (compareSigKeys(recordA, recordB, expectedEqual) && compareUpdateKeys(recordA, recordB, expectedEqual))
    {
        result = true;
    }
    else
    {
        fprintf(stdout, "Key comparison (%s) fail for moduleId[%d:%d] version[%d:%d]\n", expectedEqual ? "equal" : "diff", recordA.moduleId, recordB.moduleId, recordA.version, recordB.version);
    }

    return result;
}

// Are the records the same?
static bool compareRecords(KeyDbRecord &recordA, KeyDbRecord &recordB, bool expectedEqual = true)
{
    bool result = false;
    bool areEqual = false;

    if ((recordA.moduleId == recordB.moduleId) && (recordA.version == recordB.version))
    {
        areEqual = true;
    }

    if ((areEqual == expectedEqual) && compareKeys(recordA, recordB, expectedEqual))
    {
        result = true;
    }
    else
    {
        fprintf(stdout, "Record comparison (%s) fail for moduleId[%d:%d] version[%d:%d]\n", expectedEqual ? "equal" : "diff", recordA.moduleId, recordB.moduleId, recordA.version, recordB.version);
    }

    return result;
}

// Basic tests starting with an empty database
static bool testFromEmpty(void)
{
    bool pass = false;

    std::cout << "Testing using empty database:" << std::endl;

    KeyDbRecord record1, record2, record1Fetched, record2Fetched, scratchRecord;

    // Make a couple of records
    if (keyDbMakeRecord(MODULE_ID_1ST_RX, 1, 0, 0, &record1) &&
        keyDbMakeRecord(MODULE_ID_1ST_RX, 2, 0, 0, &record2) &&
        // Should fail as record already exists
        !keyDbMakeRecord(MODULE_ID_1ST_RX, 2, 0, 0, &record2))
    {
        // Fetch them and check they are the same
        if ((keyDbGetNumRecords() == 2) &&
            keyDbGetRecord(MODULE_ID_1ST_RX, 1, &record1Fetched) &&
            keyDbGetRecord(MODULE_ID_1ST_RX, 2, &record2Fetched) &&
            // This should fail
            !keyDbGetRecord(MODULE_ID_1ST_RX, 3, &scratchRecord))
        {
            // Compare the records
            if (compareRecords(record1, record1Fetched) && compareRecords(record2, record2Fetched) && compareRecords(record1, record2, false))
            {
                pass = true;
            }
            else
            {
                std::cout << "Record check failed" << std::endl;
            }
        }
        else
        {
            std::cout << "Failed to fetch records" << std::endl;
        }
    }
    else
    {
        std::cout << "Failed to make records" << std::endl;
    }

    std::cout << (pass ? "=> Pass" : "=> Fail") << std::endl;

    return pass;
}

// Basic tests starting with an empty database
static bool testGetVersions(void)
{
    bool pass = false;

    std::cout << "Testing get versions:" << std::endl;

    // Should have 2 versions of module id 1st RX
    uint32_t numVersions;
    uint32_t *pVersions = keyDbGetVersions(MODULE_ID_1ST_RX, &numVersions);

    if (pVersions)
    { 
        if (numVersions == 2)
        {
            // Version values should be 1 and 2
            if ((pVersions[0] == 1) && (pVersions[1] == 2))
            {
                pass = true;
            }
            else
            {
                std::cout << "Invalid versions array contents: " << pVersions[0] << ", " << pVersions[1] << std::endl;
            }

            free(pVersions);
        }
        else
        {
            std::cout << "Wrong number of records:" << (int)numVersions << std::endl;
        }
    }
    else
    {
        std::cout << "Null versions array returned" << std::endl;
    }

    // And check that a non-existant module id returns the correct response
    if (pass)
    {
        pVersions = keyDbGetVersions(27, &numVersions);
        if ((pVersions != NULL) || (numVersions != 0))
        {
            std::cout << "Request for a non-existant module id failed" << std::endl;
            pass = false;
        }
    }

    std::cout << (pass ? "=> Pass" : "=> Fail") << std::endl;

    return pass;
}

// Test saving and restoring database
static bool testSaveAndRestore(const char *pFilename)
{
    bool pass = false;

    std::cout << "Testing save and restore:" << std::endl;

    if (keyDbSave(pFilename))
    {
        uint32_t startingRecordNum = keyDbGetNumRecords();
        KeyDbRecord record1, record2, record1Reloaded, record2Reloaded;
        // Store the existing records so that the newly loaded ones can be tested
        if ((keyDbGetNumRecords() == 2) &&
            keyDbGetRecord(MODULE_ID_1ST_RX, 1, &record1) &&
            keyDbGetRecord(MODULE_ID_1ST_RX, 2, &record2))
        {
            // Re-initialise the database and check there are no records left
            if ((startingRecordNum > 0) && keyDbInitialise() && (keyDbGetNumRecords() == 0))
            {
                // Load the database from the file
                if (keyDbLoad(pFilename) && (keyDbGetNumRecords() == startingRecordNum))
                {
                    // Check records look the same as before
                    if (keyDbGetRecord(MODULE_ID_1ST_RX, 1, &record1Reloaded) &&
                        keyDbGetRecord(MODULE_ID_1ST_RX, 2, &record2Reloaded))
                    {
                        if (compareRecords(record1, record1Reloaded) && compareRecords(record2, record2Reloaded))
                        {
                            pass = true;
                        }
                        else
                        {
                            std::cout << "Reloaded records don't match originals" << std::endl;
                        }
                    }
                    else
                    {
                        std::cout << "Failed to read records after database load" << std::endl;
                    }
                }
                else
                {
                    std::cout << "Failed to load database" << std::endl;
                }
            }
            else
            {
                std::cout << "Failed to re-initialise" << std::endl;
            }
        }
        else
        {
            std::cout << "Failed to cache existing records" << std::endl;
        }
    }
    else
    {
        std::cout << "Failed to save" << std::endl;
    }

    std::cout << (pass ? "=> Pass" : "=> Fail") << std::endl;

    return pass;
}

static bool testReUseOfKeys(void)
{
    bool pass = false;

    std::cout << "Testing re-use of keys:" << std::endl;

    KeyDbRecord record3, record4, record1Fetched, record2Fetched;

    if (keyDbMakeRecord(MODULE_ID_1ST_RX, 3, 1, 0, &record3) &&
        keyDbMakeRecord(MODULE_ID_1ST_RX, 4, 0, 2, &record4))
    {
        // Fetch all 4 records
        if (keyDbGetRecord(MODULE_ID_1ST_RX, 1, &record1Fetched) &&
            keyDbGetRecord(MODULE_ID_1ST_RX, 2, &record2Fetched))
        {
            // The keys in the new records should not be the same as each other
            if (compareKeys(record3, record4, false))
            {
                // Now test the key re-use
                if (compareSigKeys(record3, record1Fetched, true)  && compareUpdateKeys(record3, record1Fetched, false) &&
                    compareSigKeys(record3, record2Fetched, false) && compareUpdateKeys(record3, record2Fetched, false) &&
                    compareSigKeys(record4, record1Fetched, false) && compareUpdateKeys(record4, record1Fetched, false)  &&
                    compareSigKeys(record4, record2Fetched, false) && compareUpdateKeys(record4, record2Fetched, true))
                {
                    pass = true;
                }
                else
                {
                    std::cout << "Key re-use failure" << std::endl;
                }
            }
            else
            {
                std::cout << "New records shoukdn't match" << std::endl;
            }
        }
        else
        {
            std::cout << "Failed to fetch the records" << std::endl;
        }
    }
    else
    {
        std::cout << "Failed to make records with key re-use" << std::endl;
    }

    std::cout << (pass ? "=> Pass" : "=> Fail") << std::endl;

    return pass;
}

static bool testFixedSigningKeys(void)
{
    bool pass = false;

    std::cout << "Testing fixed signing keys:" << std::endl;

    KeyDbRecord record;

    EccPrivateKey sigPrivateKey;
    EccPublicKey  sigPublicKey;

    // Initialise keys
    for (int i = 0; i < sizeof(EccPrivateKey); i++)
    {
        sigPrivateKey[i] = i;
    }
    for (int i = 0; i < sizeof(EccPublicKey); i++)
    {
        sigPublicKey[i] = 0xFF ^ i;  // Different from the private key
    }

    if (keyDbMakeRecordFixedSigningKey(MODULE_ID_1ST_RX, 5, sigPublicKey, sigPrivateKey, &record))
    {
        // Check the new record has the supplied sigining keys, and that they are not the same as the private keys 
        if (!memcmp(record.sigPublicKey,  sigPublicKey,  sizeof(KeyDbRecord::sigPublicKey))  &&
            !memcmp(record.sigPrivateKey, sigPrivateKey, sizeof(KeyDbRecord::sigPrivateKey)) &&
            (0 != memcmp(record.updatePublicKey,  sigPublicKey,  sizeof(KeyDbRecord::updatePublicKey))) &&
            (0 != memcmp(record.updatePrivateKey, sigPrivateKey, sizeof(KeyDbRecord::updatePrivateKey))))
        {
            pass = true;
        }
        else
        {
            std::cout << "Key assignment error" << std::endl;
        }
    }
    else
    {
        std::cout << "Failed to make record with signing key" << std::endl;
    }

    std::cout << (pass ? "=> Pass" : "=> Fail") << std::endl;

    return pass;
}

// Test restoring a corrupt (modified) file
static bool testRestoreCorruptFile(const char *pFilename, const char *pCorruptFilename)
{
    bool pass = false;

    std::cout << "Testing restore of corrupt file:" << std::endl;

    // Re-initialise the database and check there are no records left
    if (keyDbInitialise() && (keyDbGetNumRecords() == 0))
    {
        // Create a corrupt copy of the existing file so that its SHA sum will no longer be valid
        std::ifstream inFile(pFilename);
        std::ofstream outFile(pCorruptFilename);

        if (inFile.is_open() && outFile.is_open())
        {
            std::string line;
            bool modified = false;
            while (getline(inFile, line))
            {
                // Change 1 character on the first line
                if (!modified)
                {
                    int pos = (int) line.length() / 3;
                    char temp = line[pos];
                    if (temp > '9')
                    {
                        temp = '3';
                    }
                    else
                    {
                        temp = 'e';
                    }
                    line[pos] = temp;
                    modified = true;
                }

                // getline removes line terminations
                outFile << line << std::endl;
            }

            inFile.close();
            outFile.close();

            // Attempt to load the database from the file
            if (!keyDbLoad(pCorruptFilename))
            {
                pass = true;
            }
            else
            {
                std::cout << "Failed to detect a corrupt database" << std::endl;
            }
        }
        else
        {
            std::cout << "Failed to open files" << std::endl;
        }
    }
    else
    {
        std::cout << "Failed to re-initialise" << std::endl;
    }

    std::cout << (pass ? "=> Pass" : "=> Fail") << std::endl;

    return pass;
}


int main(const int argc, const char *argv[])
{
    bool success = false;

    if (argc == 3)
    {
        if (cryptoInitialise() && keyDbInitialise())
        {
            if (testFromEmpty() && testGetVersions() && testSaveAndRestore(argv[1]) && testReUseOfKeys() && testFixedSigningKeys() &&
                // Do this test last as it will destroy the database contents
                testRestoreCorruptFile(argv[1], argv[2]))
            {
                success = true;
            }
        }
        else
        {
            std::cout << "Failed to initialise" << std::endl;
        }
    }
    else
    {
        std::cerr << "Usage:" << std::endl;
        std::cerr << "   " << argv[0] << " <keyDb file to generate> <corrupt file to generate>" << std::endl;
    }

    if (!success)
    {
        std::cout << std::endl << "***** Tests failed *****" << std::endl;
    }

    return success ? 0 : -1;
}

