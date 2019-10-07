

/***********************************************************************************************************************
* Copyright [2015] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
*
* The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
* and/or its licensors ("Renesas") and subject to statutory and contractual protections.
*
* Unless otherwise expressly agreed in writing between Renesas and you: 1) you may not use, copy, modify, distribute,
* display, or perform the contents; 2) you may not use any name or mark of Renesas for advertising or publicity
* purposes or in connection with your use of the contents; 3) RENESAS MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE
* SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED ‘AS IS’ WITHOUT ANY EXPRESS OR IMPLIED
* WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND
* NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR CONSEQUENTIAL DAMAGES,
* INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF CONTRACT OR TORT, ARISING
* OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents included in this file may
* be subject to different terms.
**********************************************************************************************************************/

/*
 *  SREC File Interface. This builds upon the underlying srec.h to provide
 *  file I/O.
 */


#ifndef SREC_FILE_H_
#define SREC_FILE_H_

#ifdef __cplusplus
extern "C" {
#endif

    #include <stdint.h>
    #include <stdbool.h>

    // For the cases where you don't want to include <windows.h>
    #ifndef HANDLE
        typedef void* HANDLE;
    #endif
    #ifndef INVALID_HANDLE_VALUE
        #define INVALID_HANDLE_VALUE ((HANDLE)-1)
    #endif

    /*
     * Type definitions
     */

    typedef struct
    {
        uint8_t*  pBuffer;         // pointer to a malloc’ed buffer containing the raw image data.
        uint32_t  bufferSize;      // the length of the malloc'ed buffer
        uint32_t  lowestAddr;      // flash address on target for the image. Corresponds to the first byte in pBuffer.
        uint32_t  highestAddr;     // highest address on target used by the image.  [Image length  = highestAddr-lowestAddr+1]
    } SrecReaderImage;

    typedef struct
    {
        uint8_t*  pBuffer;         // pointer to a malloc’ed buffer containing the raw image data.
        uint32_t  numBytes;        // the number of bytes to write
        uint32_t  lowestAddr;      // flash address on target for the image. Corresponds to the first byte in pBuffer.
    } SrecWriterImage;

    /*
     * Reading a S Record file
     */

    // Read a S record file.
    //
    // N.B: this function will malloc pBuffer and on successful return, the ownership
    //      of pBuffer is transferred to the caller who will be responsible for free'ing it.
    //      If the file read fails, this function will free pBuffer and the caller need
    //      not take any action with pImage.
    //
    // Input: pFilename; name of file to read
    // Output: pImage; pointer to SrecReadImage structure that will be filled in on success
    // Returns: true if read OK, false otherwise (and no action required on pImage).
    extern bool srecReadFile(const char *pFilename, SrecReaderImage *pImage);

    /*
     * Writing a S Record file
     */

     // Opens a S record file for writing.
     // Input: pFilename; name of file to open
     // Returns: the srec writer handle if open succeeeded, INVALID_HANDLE_VALUE otherwise.
    extern HANDLE srecWriteOpenFile(const char *pFilename);

    // Writes one comment line to the S record file, prepending the line with '//'.
    // Input: handle; the writer handle
    // Input: pLine; the null terminated string to write
    // Input: bufferBaseAddress; the target address to be associated with the start of the buffer
    // Returns: true if written OK, false otherwise (handle will no longer be valid on failure)
    extern bool srecWriteCommentString(const HANDLE handle, const char *pLine);

    // Writes the SrecImage and then closes the file.
    // Input: handle; the writer handle
    // Input: image; SrecWriterImage structure defining the data to write
    // Returns: true if written OK, false otherwise (handle will no longer be valid in either case)
    extern bool srecWriteSrecImageAndClose(const HANDLE handle, const SrecWriterImage image);

#ifdef __cplusplus
}
#endif

#endif /* SREC_FILE_H_ */
