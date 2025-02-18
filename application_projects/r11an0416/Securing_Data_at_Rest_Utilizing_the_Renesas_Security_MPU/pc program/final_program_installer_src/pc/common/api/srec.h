/***********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
/*
 *  SREC parser
 */


#ifndef SREC_H_
#define SREC_H_

#ifdef __cplusplus
extern "C" {
#endif

    #include <stdint.h>
    #include <stdbool.h>

    #ifndef HANDLE
        typedef void* HANDLE;
    #endif

    /*
     * Parsing S Records
     */

    // Create a srec read parser. If pBuffer==NULL then when the file is parsed the metadata will
    // be extracted (lowest/highest addresses etc) but the data will not be saved.
    // Input: pBuffer; pointer to the buffer into which the parsed data will be placed
    // Input: bufferSize; the length of the buffer
    // Input: bufferBaseAddress; the target address to be associated with the start of the buffer
    // Returns: the srec parser handle if open succeeeded, INVALID_HANDLE_VALUE otherwise.
    extern HANDLE srecCreateParser(const uint8_t *pBuffer, const uint32_t bufferSize, const uint32_t bufferBaseAddress);

    // Parse a null terminated SREC line.
    // Input: handle; srec parser handle
    // Input: pLine; pointer to the null terminated SREC string
    // Returns: true if SREC line parsed OK, false otherwise
    extern bool srecParse(const HANDLE handle, const char *pLine);

    // Getters for access to parsed records. These getters use the following style:
    // - For types that support NULL, the getter will return the value or NULL if not available.
    // - For types that do not support NULL, the getter will be passed a pointer to the variable to hold
    //   the result and return a bool indicating if the value is available.

    // Header (memory is transferred to the caller and must be freed when no longer required)
    extern char *srecGetHeader(const HANDLE handle);
    // Start address.
    extern bool srecGetStartAddress(const HANDLE handle, uint32_t *pStartAddress);
    // Lowest buffer offset parsed.
    extern bool srecGetLowestOffset(const HANDLE handle, uint32_t *pLowestOffset);
    // Highest buffer offset parsed.
    extern bool srecGetHighestOffset(const HANDLE handle, uint32_t *pHighestOffset);
    // Lowest address parsed (equivalent to bufferBaseAddress + lowestOffset).
    extern bool srecGetLowestAddress(const HANDLE handle, uint32_t *pLowestAddress);
    // Highest address parsed (equivalent to bufferBaseAddress + highestOffset).
    extern bool srecGetHighestAddress(const HANDLE handle, uint32_t *pHighestAddress);

    // Destroy a srec parser
    // Input: handle; srec parser handle
    extern void srecDestroyParser(const HANDLE handle);

    /*
     * Creating S Records
     */

    // Function signature for S record callback. Note that the memory for the S record string
    // is owned by the S record writer function so this callback should not return until the
    // consumer has finished with the string.
    // Input: pSrec; pointer to the compiled S record string
    // Input: pUserParams; pointer to user supplied parameter(s)
    // Returns: true if handled OK, false otherwise (will terminate srec generation).
    typedef bool(*SrecGeneratorCallback)(const char *pSrec, void *pUserParams);

    // Address widths
    typedef enum SrecAddressWidth_tag {

        WIDTH_16_BIT = 0,
        WIDTH_24_BIT,
        WIDTH_32_BIT,

        NUM_ADDRESS_WIDTHS

    } SrecAddressWidth;

    // Create a srec generator.
    // Input: pGeneratorCallback; user suplied callback for generated S records
    // Input: pUserParams; pointer to user supplied parameter(s). Can be null if no user context required.
    // Input: addressWidth; the width of the address fields used when generating S records.
    // Returns: the srec generator handle if open succeeeded, INVALID_HANDLE_VALUE otherwise.
    extern HANDLE srecCreateGenerator(SrecGeneratorCallback pGeneratorCallback, void *pUserParams, const SrecAddressWidth addressWidth);

    // Optionally set the maximum data S record byte count value. This can be used to limit the
    // data S record length for interoperability if required. If not set, the srec generator will
    // create data records up to the maximum 255 byte count value.
    // Input: handle; srec generator handle
    // Input: maxbyteCount; maximum byte count value to use
    extern void srecGeneratorSetMaxByteCount(const HANDLE handle, const uint8_t maxByteCount);

    // Generate a header record. This is used to start the S records.
    // Input: handle; srec generator handle
    // Input: pStr; pointer to the header string to encode
    // Returns: true if S record generated OK, false otherwise.
    extern bool srecGenerateHeader(const HANDLE handle, const char *pStr);

    // Given a block of memory, create S records of that blocks data. Each line (line+null terminated)
    // is returned via the SrecWriterCallback.
    // Input: handle; srec generator handle
    // Input: pBuffer; pointer to the buffer containing the source data
    // Input: bufferSize; the length of the buffer
    // Input: bufferBaseAddress; the target address to be associated with the start of the buffer
    // Returns: true if S records generated OK, false otherwise.
    extern bool srecGenerateData(const HANDLE handle, const uint8_t *pBuffer, const uint32_t numBytes, const uint32_t bufferBaseAddress);

    // Generate a start address record. This is used to terminate the S records.
    // Input: handle; srec generator handle
    // Input: startAddress; the start address
    // Returns: true if S record generated OK, false otherwise.
    extern bool srecGenerateStartAddress(const HANDLE handle, const uint32_t startAddress);

    // Destroy a srec generator
    // Input: handle; srec generator handle
    extern void srecDestroyGenerator(const HANDLE handle);

#ifdef __cplusplus
}
#endif

#endif /* SREC_H_ */
