// SPDX-FileCopyrightText: 2021 Larry Bank <bitbank@pobox.com>
// SPDX-License-Identifier: Apache-2.0 
//
// JPEG Encoder
//
// Arduino port started 7/22/2021
// Original JPEG code written 20+ years ago :)
// The goal of this code is to encode JPEG images on embedded systems
//
// Copyright 2021 BitBank Software, Inc. All Rights Reserved.
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//    http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//===========================================================================
//
#include "JPEGENC.h"

// Include the C code which does the actual work
#include "jpeg.h"

//
// File (SD/MMC) based initialization
//
//int JPEGOpenFile(JPEGE_IMAGE *_jpeg, const char *szFilename, JPEGE_OPEN_CALLBACK *pfnOpen, JPEGE_CLOSE_CALLBACK *pfnClose, JPEGE_READ_CALLBACK *pfnRead, JPEGE_WRITE_CALLBACK *pfnWrite, JPEGE_SEEK_CALLBACK *pfnSeek)
//{
//    if (szFilename == NULL || pfnOpen == NULL || pfnClose == NULL || pfnRead == NULL || pfnWrite == NULL || pfnSeek == NULL) {
//        return JPEGE_INVALID_PARAMETER;
//    }
//    memset(&_jpeg, 0, sizeof(JPEGE_IMAGE));
//    _jpeg->pfnRead = pfnRead;
//    _jpeg->pfnWrite = pfnWrite;
//    _jpeg->pfnSeek = pfnSeek;
//    _jpeg->pfnOpen = pfnOpen;
//    _jpeg->pfnClose = pfnClose;
//    _jpeg->JPEGFile.fHandle = (*pfnOpen)(szFilename);
//    _jpeg->pHighWater = &_jpeg->ucFileBuf[JPEGE_FILE_BUF_SIZE - 512];
//    if (_jpeg->JPEGFile.fHandle == NULL) {
//        _jpeg->iError = JPEGE_INVALID_FILE;
//       return JPEGE_INVALID_FILE;
//    }
//    return JPEGE_SUCCESS;
//
//} /* open() */

int JPEGOpenRAM(JPEGE_IMAGE *_jpeg, uint8_t *pOutput, int iBufferSize)
{
    if (!pOutput || iBufferSize < 1024) return JPEGE_INVALID_PARAMETER;
    memset(_jpeg, 0, sizeof(JPEGE_IMAGE));
    _jpeg->pOutput = pOutput;
    _jpeg->iBufferSize = iBufferSize;
    _jpeg->pHighWater = &pOutput[iBufferSize - 512];

    return JPEGE_SUCCESS;
} /* open() */

//
// return the last error (if any)
//
int getLastError(JPEGE_IMAGE *_jpeg)
{
    return _jpeg->iError;
} /* getLastError() */
//
// Close the file - not needed when decoding from memory
//
int close(JPEGE_IMAGE *_jpeg)
{
    JPEGEncodeEnd(_jpeg);
    if (_jpeg->pfnClose)
        (*_jpeg->pfnClose)(&_jpeg->JPEGFile);
    return _jpeg->iDataSize;
} /* close() */

int encodeBegin(JPEGE_IMAGE *_jpeg, JPEGENCODE *pEncode, int iWidth, int iHeight, uint8_t ucPixelType, uint8_t ucSubSample, uint8_t ucQFactor)
{
    return JPEGEncodeBegin(_jpeg, pEncode, iWidth, iHeight, ucPixelType, ucSubSample, ucQFactor);
} /* encodeBegin() */

int addMCU(JPEGE_IMAGE *_jpeg, JPEGENCODE *pEncode, uint8_t *pPixels, int iPitch)
{
    return JPEGAddMCU(_jpeg, pEncode, pPixels, iPitch);
} /* addMCU() */

int addFrame(JPEGE_IMAGE *_jpeg, JPEGENCODE *pEncode, uint8_t *pPixels, int iPitch)
{
    return JPEGAddFrame(_jpeg, pEncode, pPixels, iPitch);
} /* addFrame() */

