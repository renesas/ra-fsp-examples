

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
 *  Simple CSV based implementation of the keyDbApi for the evaluation system
 */


#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <memory>
#include <map>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <iomanip>

#include "keyDbApi.h"
#include "crypto.h"

/*
 * Private data
 */

// Key class for KeyDbRecords
class RecordKey
{
    public: 
        RecordKey(const uint8_t moduleId, const uint32_t version) :
            m_moduleId(moduleId),
            m_version(version)
        {}
        virtual ~RecordKey() {}

        bool operator<(const RecordKey& rhs) const
        { 
            if (m_moduleId < rhs.m_moduleId)
                return true;
            else if (m_moduleId == rhs.m_moduleId && m_version < rhs.m_version)
                return true;
            else
                return false;
        }

        uint8_t  m_moduleId;
        uint32_t m_version;
};

// Simple container class for KeyDbRecords
class Record
{
    public:
        Record() : m_isValid(false) {}

        Record(const RecordKey key) :
            m_isValid(true)
        {
            m_record.moduleId = key.m_moduleId;
            m_record.version  = key.m_version;
        };

        virtual ~Record() {};

        RecordKey getRecordKey(void) { return RecordKey(m_record.moduleId, m_record.version); }
        bool isValid(void) { return m_isValid; };

        std::string toCsv(void);
        bool fromCsv(const std::string csvLine);
        
        KeyDbRecord m_record;

    private:
        // A record is considered valid if it has a key
        bool m_isValid;
};

typedef std::shared_ptr<Record> RecordPtr;

// Container for records
typedef std::map < RecordKey, RecordPtr, std::less<RecordKey> > KeyRecords;
static KeyRecords keyRecords;

// And its iterator
typedef std::map<RecordKey, RecordPtr>::iterator KeyRecordsIterator;

/*
 * Private functions
 */

// uint8_t array to ASCII hex string
static std::string byteArrayToString(const uint8_t *pArray, const int numBytes)
{
    std::ostringstream str;
    for (int i = 0; i < numBytes; i++)
    {
        // Flags don't appear to be reliably 'sticky' so set for each digit
        str << std::setw(2) << std::setfill('0') << std::right << std::hex << (int)pArray[i];
    }
    return str.str();
}

// ASCII hex string to uint8_t array (the lengths must match for this to perform the conversion)
static bool asciiHexToByteArray(std::string hexStr, uint8_t *pArray, uint32_t arrayLength)
{
    bool success = false;

    if (hexStr.length() == (2 * arrayLength))
    {
        for (uint32_t i = 0; i < hexStr.length(); i += 2)
        {
            std::string byteString = hexStr.substr(i, 2);
            pArray[i >> 1] = (uint8_t)strtol(byteString.c_str(), NULL, 16);
        }

        success = true;
    }
    else
    {
        std::cerr << "Non-matching record length: " << (int)hexStr.length() <<  " for array: " << (int)arrayLength << std::endl;
    }

    return success;
}

// Split a CSV string into a vector of component strings
static std::vector<std::string> splitCsv(std::string csv)
{
    std::vector<std::string> strings;

    std::string delimiter = ",";
    size_t last = 0;
    size_t next = 0;
    while ((next = csv.find(delimiter, last)) != std::string::npos)
    {
        strings.push_back(csv.substr(last, next - last));
        last = next + 1;
    }
    strings.push_back(csv.substr(last));

    return strings;
}

// Format a record in CSV
std::string Record::toCsv()
{
    std::ostringstream str;

    str << (int)m_record.moduleId << "," << (int)m_record.version << 
        "," << byteArrayToString(m_record.sigPrivateKey,    sizeof(m_record.sigPrivateKey))    <<
        "," << byteArrayToString(m_record.sigPublicKey,     sizeof(m_record.sigPublicKey))     <<
        "," << byteArrayToString(m_record.updatePrivateKey, sizeof(m_record.updatePrivateKey)) <<
        "," << byteArrayToString(m_record.updatePublicKey,  sizeof(m_record.updatePublicKey));

    return str.str();
}

// Load a Record from a CSV line.
bool Record::fromCsv(const std::string csvLine)
{
    bool success = false;
    m_isValid = false;

    std::vector<std::string> vars = splitCsv(csvLine);
    if (vars.size() == 6)
    {
        try
        {
            m_record.moduleId = std::stoi(vars[0]);
            m_record.version  = std::stoi(vars[1]);

            if (asciiHexToByteArray(vars[2], m_record.sigPrivateKey,    sizeof(m_record.sigPrivateKey))    &&
                asciiHexToByteArray(vars[3], m_record.sigPublicKey,     sizeof(m_record.sigPublicKey))     &&
                asciiHexToByteArray(vars[4], m_record.updatePrivateKey, sizeof(m_record.updatePrivateKey)) &&
                asciiHexToByteArray(vars[5], m_record.updatePublicKey,  sizeof(m_record.updatePublicKey)) )
            {
                m_isValid = true;
                success = true;
            }
        }
        catch (std::exception e)
        {
            std::cerr << "Failed reading CSV line: " << e.what() << std::endl;
        }
    }

    return success;
}

/*
 * Public API
 */

 // Initialise internal data structures.
 // Afterwards, either a DB can be loaded from a file or a new databse
 // can be constructed with calls to keyDbMakeRecord()
 // Returns: true if successful, false otherwise.
bool keyDbInitialise(void)
{
    // Allow re-initialising the database
    keyRecords.clear();
    return true;
}

// Reads a database from a file and checks the hash.
// Input: pFilename; the database file to load
// Returns: true if successful, false otherwise.
bool keyDbLoad(const char *pFilename)
{
    bool success = false;

    std::ostringstream shaStream;
    std::string shaLine;

    std::ifstream file(pFilename);
    if (file.is_open())
    {
        std::string line;
        while (getline(file, line))
        {
            // Is it the SHA record?
            if (0 == line.compare(0, 3, "SHA"))
            {
                shaLine = line;
            }
            else
            {
                RecordPtr recordPtr = std::make_shared<Record>();
                if (recordPtr->fromCsv(line))
                {
                    // Only insert it if there is not a record with this key already
                    KeyRecordsIterator itor = keyRecords.find(recordPtr->getRecordKey());
                    if (itor == keyRecords.end())
                    {
                        keyRecords[recordPtr->getRecordKey()] = recordPtr;
                        success = true;
                    }
                }

                // Save line for SHA validation (N.B: getline() removes the line terminator for us)
                shaStream << line;
            }
        }
        file.close();
    }

    // Database read OK? - now validate the SHA
    if (success)
    {
        success = false;

        // Parse the SHA line
        std::vector<std::string> vars = splitCsv(shaLine);
        if ((vars.size() == 2) && (vars[0].compare("SHA") == 0))
        {
            Sha256 sha256FromFile;
            if (asciiHexToByteArray(vars[1], sha256FromFile, SHA256_SIZE))
            {
                // And compare to the SHA of the records read from the file
                std::string recordStr = shaStream.str();
                const char *pRecords = recordStr.c_str();
                Sha256 hash;
				if (!cryptoCalculateSha256Start(SEGMENTED_HASH_WHOLE_UPDATE, (uint8_t *)pRecords, (const uint32_t)strlen(pRecords)))
				{
					std::cerr << "Hash calculation failed";
				}
				else if (!cryptoCalculateSha256MiddleEnd(SEGMENTED_HASH_WHOLE_UPDATE, NULL, 0, true, &hash))
				{
					std::cerr << "Hash calculation failed";
				}
				else
                {
                    if (!memcmp(sha256FromFile, hash, SHA256_SIZE))
                    {
                        success = true;
                    }
                    else
                    {
                        std::cerr << "Database read failed: SHA invalid (" << byteArrayToString(sha256FromFile, SHA256_SIZE) << " != " << byteArrayToString(hash, SHA256_SIZE) << ")" << std::endl;
                    }
                }
            }
            else
            {
                std::cerr << "Bad or missing SHA field: " << vars[1] << std::endl;
            }
        }
        else
        {
            std::cerr << "Bad or missing SHA record: " << shaLine << std::endl;
        }
    }

    // If reading failed, ensure the database is empty
    if (!success)
    {
        keyRecords.clear();
    }

    return success;
}

// Generates SHA2 hash and writes the database to a file.
// Input: pFilename; the database file to write
// Returns: true if successful, false otherwise.
bool keyDbSave(const char *pFilename)
{
    bool success = false;

    std::ostringstream shaStream;
    KeyRecordsIterator itor;
    // Get a string of all the records (without line endings) for SHA calculation
    for (itor = keyRecords.begin(); itor != keyRecords.end(); itor++)
    {
        if (itor->second->isValid())
        {
            shaStream << itor->second->toCsv();
        }
    }

    // Calculate the SHA256 over the CSV records
    std::string recordStr = shaStream.str();
    const char *pRecords = recordStr.c_str();
    Sha256 hash;

	if (!cryptoCalculateSha256Start(SEGMENTED_HASH_WHOLE_UPDATE, (uint8_t *)pRecords, (const uint32_t)strlen(pRecords)))
	{
		std::cerr << "Hash calculation failed";
	}
	else if (!cryptoCalculateSha256MiddleEnd(SEGMENTED_HASH_WHOLE_UPDATE, NULL, 0, true, &hash))
	{
		std::cerr << "Hash calculation failed";
	}
	else
	{
        std::ofstream file(pFilename);
        if (file.is_open())
        {
            // Write records to file, with line endings
            for (itor = keyRecords.begin(); itor != keyRecords.end(); itor++)
            {
                if (itor->second->isValid())
                {
                    file << itor->second->toCsv() << std::endl;
                }
            }

            // Followed by SHA
            file << "SHA," << byteArrayToString(hash, SHA256_SIZE)  << std::endl;

            file.close();
            success = true;
        }

    }

    return success;
}

// Return the number of entries in the database
uint32_t keyDbGetNumRecords(void)
{
    return (uint32_t)keyRecords.size();
}

// Gets a record from the database.
// Inputs: moduleId, version; the composite key for the database record
// Output: pRecord; the database record will be copied into the memory pointed to by pRecord.
// Returns: true on success, false otherwise.
bool keyDbGetRecord(const uint8_t moduleId, const uint32_t version, KeyDbRecord *pRecord)
{
    bool success = false;

    if (NULL == pRecord)
    {
        return false;
    }

    RecordKey key(moduleId, version);
    KeyRecordsIterator itor = keyRecords.find(key);
    if (itor != keyRecords.end())
    {
        if (itor->second->isValid())
        {
            *pRecord = itor->second->m_record;
            success = true;
        }
    }

    return success;
}

// Creates a new record in the database for the specified module/version and generates its crypto keys.
// Will fail if a record already exists with the given moduleId+version.
// Inputs: moduleId, version; the composite key for the database record
// Inputs: sigKeyVersion. 0=>make a new key, else copy key from the specified version of the same module.
// Inputs: updateKeyVersion. 0=>make a new key, else copy key from the specified version of the same module.
// Output: pRecord; the database record will be copied into the memory pointed to by pRecord. Supplied pRecord must be not NULL.
// Returns: true on success, false otherwise.
bool keyDbMakeRecord(const uint8_t moduleId, const uint32_t version, const uint32_t sigKeyVersion, const uint32_t updateKeyVersion, KeyDbRecord *pRecord)
{
    bool success = false;

    if (NULL == pRecord)
    {
        return false;
    }

    RecordKey key(moduleId, version);
    KeyRecordsIterator itor = keyRecords.find(key);
    
    // Only create the new record if there is no existing one for this key
    if (itor == keyRecords.end())
    {
        RecordPtr recordPtr = std::make_shared<Record>(key);
        bool sigKeysOk = false;
        bool updateKeysOk = false;

        // Create/re-use the crypto keys
        if (0 == sigKeyVersion)
        {
            sigKeysOk = cryptoMakeEccKeys(&recordPtr->m_record.sigPublicKey, &recordPtr->m_record.sigPrivateKey);
        }
        else
        {
            KeyDbRecord sigRecord;
            if (keyDbGetRecord(moduleId, sigKeyVersion, &sigRecord))
            {
                memcpy(recordPtr->m_record.sigPublicKey,  sigRecord.sigPublicKey,  sizeof(KeyDbRecord::sigPublicKey));
                memcpy(recordPtr->m_record.sigPrivateKey, sigRecord.sigPrivateKey, sizeof(KeyDbRecord::sigPrivateKey));
                sigKeysOk = true;
            }
        }

        if (0 == updateKeyVersion)
        {
            updateKeysOk = cryptoMakeEccKeys(&recordPtr->m_record.updatePublicKey, &recordPtr->m_record.updatePrivateKey);
        }
        else
        {
            KeyDbRecord updateRecord;
            if (keyDbGetRecord(moduleId, updateKeyVersion, &updateRecord))
            {
                memcpy(recordPtr->m_record.updatePublicKey,  updateRecord.updatePublicKey,  sizeof(KeyDbRecord::updatePublicKey));
                memcpy(recordPtr->m_record.updatePrivateKey, updateRecord.updatePrivateKey, sizeof(KeyDbRecord::updatePrivateKey));
                updateKeysOk = true;
            }
        }

        // key generation/reuse OK?
        if (sigKeysOk && updateKeysOk)
        {
            keyRecords[key] = recordPtr;
            *pRecord = recordPtr->m_record;
            success = true;
        }
    }

    return success;
}

// Creates a new record in the database for the specified module/version and generates its crypto keys.
// Will fail if a record already exists with the given moduleId+version.
// Inputs: moduleId, version; the composite key for the database record
// Inputs: Explicit values for the signing keys. Update keys are generated.
// Output: pRecord; the database record will be copied into the memory pointed to by pRecord. Supplied pRecord must be not NULL.
// Returns: true on success, false otherwise.
bool keyDbMakeRecordFixedSigningKey(const uint8_t moduleId, const uint32_t version, const uint8_t *pPublicSigningKey, const uint8_t *pPrivateSigningKey, KeyDbRecord *pRecord)
{
    bool success = false;

    if (NULL == pRecord)
    {
        return false;
    }

    RecordKey key(moduleId, version);
    KeyRecordsIterator itor = keyRecords.find(key);

    // Only create the new record if there is no existing one for this key
    if (itor == keyRecords.end())
    {
        RecordPtr recordPtr = std::make_shared<Record>(key);

        // Make new update keys
        if (cryptoMakeEccKeys(&recordPtr->m_record.updatePublicKey, &recordPtr->m_record.updatePrivateKey))
        {
            // Store the supplied signing keys in the record
            memcpy(recordPtr->m_record.sigPublicKey,  pPublicSigningKey,  sizeof(KeyDbRecord::sigPublicKey));
            memcpy(recordPtr->m_record.sigPrivateKey, pPrivateSigningKey, sizeof(KeyDbRecord::sigPrivateKey));

            keyRecords[key] = recordPtr;
            *pRecord = recordPtr->m_record;
            success = true;
        }
    }

    return success;
}

// Gets an array of version numbers for the given module ID
// Inputs: moduleId; the module ID being queried
// Output: pNumVersions; the number of versions in the returned array.
// Returns: pointer to version array on success, NULL otherwise.
// N.B: the versions array returned becomes the responsibility of the caller who
//      must free() that memeory when no longer required.
uint32_t * keyDbGetVersions(const uint8_t moduleId, uint32_t *pNumVersions)
{
    uint32_t *pVersions = NULL;

    // List of version numbers for the supplied module id
    std::vector<uint32_t> versions;

    // Find all the versions for the module id
    KeyRecordsIterator itor;
    for (itor = keyRecords.begin(); itor != keyRecords.end(); itor++)
    {
        if (itor->second->m_record.moduleId == moduleId)
        {
            versions.push_back(itor->second->m_record.version);
        }
    }

    // Convert to the C interface
    *pNumVersions = versions.size();
    if (*pNumVersions > 0)
    {
        pVersions = (uint32_t *)malloc(*pNumVersions * 4);
        for (uint32_t i=0; i < *pNumVersions; i++)
        {
            pVersions[i] = versions[i];
        }
    }

    return pVersions;
}
