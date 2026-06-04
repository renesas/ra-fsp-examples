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
#ifndef __JPEGENC__
#define __JPEGENC__
#if defined( MICROPY_PY_SENSOR ) || defined( __MACH__ ) || defined( __LINUX__ ) || defined( __MCUXPRESSO ) || defined( ESP_PLATFORM )
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#define memcpy_P memcpy
#define PROGMEM
#else
//#include <Arduino.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
//
// JPEG Encoder
// Written by Larry Bank
// Copyright (c) 2021 BitBank Software, Inc.
// 
// Designed to encode JPEG files from source images (8 or 24 bpp)
// using less than 20K of RAM
//
#ifndef FALSE
#define FALSE 0
#define TRUE 1
#endif

/* Defines and variables */
#define JPEGE_FILE_BUF_SIZE 2048

#ifndef DCTSIZE
#define DCTSIZE 64
#endif

#if (INTPTR_MAX == INT64_MAX)
#define REGISTER_WIDTH 64
#define BIGINT int64_t
#define BIGUINT uint64_t
// structure for outputting variable length codes
typedef struct pil_code_tag
{
unsigned char *pOut; // current output pointer
int64_t iLen;  // length of data in accumulator
uint64_t ulAcc; // code accumulator (holds codes until at least 64-bits ready to write
} PIL_CODE;
#define STORECODE(pOut, iLen, ulCode, ulAcc, iNewLen) \
        if (iLen+iNewLen > 64) \
                                                { uint64_t ul1, ul2 = ulAcc | (ulCode >> (iLen+iNewLen-64)); \
        ul1 = ul2 & (ul2 >> 4); ul1 &= (ul1 >> 2); ul1 &= (ul1 >> 1);  ul1 &= 0x0101010101010101; \
    if (ul1 == 0) \
        {*(uint64_t *)pOut = __builtin_bswap64(ul2); pOut += 8; \
     iLen -= 64; ulAcc = 0;} else \
        {while (iLen >= 8) \
            {unsigned char c = (unsigned char)(ulAcc >> 56); *pOut++ = c; \
            if (c == 0xff) { *pOut++ = 0;} ulAcc <<= 8; iLen -= 8; }}} \
        iLen += iNewLen; ulAcc |= (ulCode << (64-iLen));
#else
#define REGISTER_WIDTH 32
#define BIGINT int32_t
#define BIGUINT uint32_t
// structure for outputting variable length codes
typedef struct pil_code_tag
{
unsigned char *pOut; // current output pointer
int32_t iLen;  // length of data in accumulator
uint32_t ulAcc; // code accumulator (holds codes until at least 32-bits ready to write
} PIL_CODE;
// 32-bit output stage assumes that unaligned writes are not allowed
#define STORECODE(pOut, iLen, ulCode, ulAcc, iNewLen) \
        if (iLen+iNewLen > 32) { while (iLen >= 8) \
                        {unsigned char c = (unsigned char)(ulAcc >> 24); *pOut++ = c; \
        if (c == 0xff) { *pOut++ = 0;} ulAcc <<= 8; iLen -= 8; }} \
        iLen += iNewLen; ulAcc |= (ulCode << (32-iLen));
#endif

//#define WRITEMOTO32(p, o, val) {uint32_t l = val; p[o] = (unsigned char)(l >> 24); p[o+1] = (unsigned char)(l >> 16); p[o+2] = (unsigned char)(l >> 8); p[o+3] = (unsigned char)l;}
//#define WRITEMOTO16(p, o, val) {uint32_t l = val; p[o] = (unsigned char)(l >> 8); p[o+1] = (unsigned char)l;}

#define WRITEMOTO32(p, o, val) do {            \
    uint32_t _l = (uint32_t)(val);             \
    (p)[(o)]     = (unsigned char)(_l >> 24);  \
    (p)[(o) + 1] = (unsigned char)(_l >> 16);  \
    (p)[(o) + 2] = (unsigned char)(_l >> 8);   \
    (p)[(o) + 3] = (unsigned char)(_l);        \
} while (0)

#define WRITEMOTO16(p, o, val) do {            \
    uint32_t _l = (uint32_t)(val);             \
    (p)[(o)]     = (unsigned char)(_l >> 8);   \
    (p)[(o) + 1] = (unsigned char)(_l);        \
} while (0)

// Error codes returned by getLastError()
enum {
    JPEGE_SUCCESS = 0,
    JPEGE_INVALID_PARAMETER,
    JPEGE_ENCODE_ERROR,
    JPEGE_MEM_ERROR,
    JPEGE_NO_BUFFER,
    JPEGE_UNSUPPORTED_FEATURE,
    JPEGE_INVALID_FILE
};
// Subsample types
enum {
    JPEGE_SUBSAMPLE_444 = 0,
    JPEGE_SUBSAMPLE_420
};

// Pixel types
enum {
    JPEGE_PIXEL_GRAYSCALE = 0,
    JPEGE_PIXEL_RGB565,
    JPEGE_PIXEL_RGB888,
    JPEGE_PIXEL_ARGB8888,
    JPEGE_PIXEL_YUV422,
    JPEGE_PIXEL_COUNT
};
// Compression quality
enum {
    JPEGE_Q_BEST = 0,
    JPEGE_Q_HIGH,
    JPEGE_Q_MED,
    JPEGE_Q_LOW
};

typedef struct jpege_file_tag
{
  int32_t iPos; // current file position
  int32_t iSize; // file size
  uint8_t *pData; // memory file pointer
  void * fHandle; // class pointer to File/SdFat or whatever you want
} JPEGE_FILE;

// Callback function prototypes
typedef int32_t (JPEGE_READ_CALLBACK)(JPEGE_FILE *pFile, uint8_t *pBuf, int32_t iLen);
typedef int32_t (JPEGE_WRITE_CALLBACK)(JPEGE_FILE *pFile, uint8_t *pBuf, int32_t iLen);
typedef int32_t (JPEGE_SEEK_CALLBACK)(JPEGE_FILE *pFile, int32_t iPosition);
typedef void * (JPEGE_OPEN_CALLBACK)(const char *szFilename);
typedef void (JPEGE_CLOSE_CALLBACK)(JPEGE_FILE *pFile);

//
// our private structure to hold a JPEG image encode state
//
typedef struct jpege_image_tag
{
    int iWidth, iHeight; // image size
    int iMCUWidth, iMCUHeight; // number of horizontal and vertical MCUs
    int x, y; // current MCU x/y
    uint8_t ucPixelType, ucSubSample, ucNumComponents;
    uint8_t ucMemType;
    uint8_t *pOutput, *pHighWater;
    int iBufferSize; // output buffer size provided by caller
    int iHeaderSize; // size of the JPEG header
    int iCompressedSize; // size of compressed output
    int iDataSize; // total output file size
    int iPitch; // bytes per line
    int iError;
    int iRestart; // current restart counter
    int iDCPred0, iDCPred1, iDCPred2; // DC predictor values for the 3 color components
    PIL_CODE pc;
    int *huffdc[2];
    signed short sQuantTable[DCTSIZE*4];
    signed char MCUc[6*DCTSIZE]; // captured image data
    signed short MCUs[DCTSIZE]; // final processed output
    JPEGE_READ_CALLBACK *pfnRead;
    JPEGE_WRITE_CALLBACK *pfnWrite;
    JPEGE_SEEK_CALLBACK *pfnSeek;
    JPEGE_OPEN_CALLBACK *pfnOpen;
    JPEGE_CLOSE_CALLBACK *pfnClose;
    JPEGE_FILE JPEGFile;
    uint8_t ucFileBuf[JPEGE_FILE_BUF_SIZE]; // holds temp file data
} JPEGE_IMAGE;

typedef struct jpegencode_t
{
    int x, y; // current encoding position
    int cx, cy; // current width+height of the MCU
} JPEGENCODE;

//#ifdef __cplusplus
//#define JPEG_STATIC static
////
//// The JPEGENC class wraps portable C code which does the actual work
////
////class JPEGENC
////{
////  public:
////    int open(const char *szFilename, JPEGE_OPEN_CALLBACK *pfnOpen, JPEGE_CLOSE_CALLBACK *pfnClose, JPEGE_READ_CALLBACK *pfnRead, JPEGE_WRITE_CALLBACK *pfnWrite, JPEGE_SEEK_CALLBACK *pfnSeek);
////    int open(uint8_t *pOutput, int iBufferSize);
////    int close();
////    int encodeBegin(JPEGENCODE *pEncode, int iWidth, int iHeight, uint8_t ucPixelType, uint8_t ucSubSample, uint8_t ucQFactor);
////    int addMCU(JPEGENCODE *pEncode, uint8_t *pPixels, int iPitch);
////    int addFrame(JPEGENCODE *pEncode, uint8_t *pPixels, int iPitch);
////    int getLastError();
////
////  private:
////    JPEGE_IMAGE _jpeg;
////};
//
////int JPEGOpenFile(JPEGE_IMAGE *_jpeg, const char *szFilename, JPEGE_OPEN_CALLBACK *pfnOpen, JPEGE_CLOSE_CALLBACK *pfnClose, JPEGE_READ_CALLBACK *pfnRead, JPEGE_WRITE_CALLBACK *pfnWrite, JPEGE_SEEK_CALLBACK *pfnSeek);
//int JPEGOpenRAM(JPEGE_IMAGE *_jpeg, uint8_t *pOutput, int iBufferSize);
//
//
//int close(JPEGE_IMAGE *_jpeg);
//int encodeBegin(JPEGE_IMAGE *_jpeg, JPEGENCODE *pEncode, int iWidth, int iHeight, uint8_t ucPixelType, uint8_t ucSubSample, uint8_t ucQFactor);
//int addMCU(JPEGE_IMAGE *_jpeg, JPEGENCODE *pEncode, uint8_t *pPixels, int iPitch);
//int addFrame(JPEGE_IMAGE *_jpeg, JPEGENCODE *pEncode, uint8_t *pPixels, int iPitch);
//int getLastError(JPEGE_IMAGE *_jpeg);
//
//
//#else
//#define JPEG_STATIC
//int JPEGOpenRAM(JPEGE_IMAGE *pJPEG, uint8_t *pData, int iDataSize);
//int JPEGOpenFile(JPEGE_IMAGE *pJPEG, const char *szFilename);
//int JPEGEncodeBegin(JPEGE_IMAGE *pJPEG, JPEGENCODE *pEncode, int iWidth, int iHeight, uint8_t ucPixelType, uint8_t ucSubSample, uint8_t ucQFactor);
//int JPEGEncodeEnd(JPEGE_IMAGE *pJPEG);
//int JPEGAddMCU(JPEGE_IMAGE *pJPEG, JPEGENCODE *pEncode, uint8_t *pPixels, int iPitch);
//int JPEGGetLastError(JPEGE_IMAGE *pJPEG);
//#endif // __cplusplus

int JPEGOpenRAM(JPEGE_IMAGE *_jpeg, uint8_t *pOutput, int iBufferSize);

int close(JPEGE_IMAGE *_jpeg);
int encodeBegin(JPEGE_IMAGE *_jpeg, JPEGENCODE *pEncode, int iWidth, int iHeight, uint8_t ucPixelType, uint8_t ucSubSample, uint8_t ucQFactor);
int addMCU(JPEGE_IMAGE *_jpeg, JPEGENCODE *pEncode, uint8_t *pPixels, int iPitch);
int addFrame(JPEGE_IMAGE *_jpeg, JPEGENCODE *pEncode, uint8_t *pPixels, int iPitch);
int getLastError(JPEGE_IMAGE *_jpeg);

// Due to unaligned memory causing an exception, we have to do these macros the slow way
#define INTELSHORT(p) ((*p) + (*(p+1)<<8))
#define INTELLONG(p) ((*p) + (*(p+1)<<8) + (*(p+2)<<16) + (*(p+3)<<24))
#define MOTOSHORT(p) (((*(p))<<8) + (*(p+1)))
#define MOTOLONG(p) (((*p)<<24) + ((*(p+1))<<16) + ((*(p+2))<<8) + (*(p+3)))

#endif // __JPEGENC__
