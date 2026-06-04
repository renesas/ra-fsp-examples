#ifndef _JPEP_H_
#define _JPEP_H_

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include "JPEGENC.h"

void JPEGMakeHuffE(JPEGE_IMAGE *pJPEG);
int JPEGEncodeEnd(JPEGE_IMAGE *pJPEG);
int JPEGEncodeBegin(JPEGE_IMAGE *pJPEG, JPEGENCODE *pEncode, int iWidth, int iHeight, uint8_t ucPixelType, uint8_t ucSubSample, uint8_t ucQFactor);
int JPEGQuantize(JPEGE_IMAGE *pJPEG, signed short *pMCUSrc, int iTable);
int JPEGEncodeMCU(int iDCTable, JPEGE_IMAGE *pJPEG, signed short *pMCUData, int iDCPred, int bSparse);
void JPEGGetMCU(unsigned char *pSrc, int iPitch, signed char *pMCU);
void JPEGSubSample24(unsigned char *pSrc, signed char *pLUM, signed char *pCb, signed char *pCr, int lsize, int cx, int cy);
void JPEGSubSample16(unsigned char *pSrc, signed char *pLUM, signed char *pCb, signed char *pCr, int lsize, int cx, int cy);
void JPEGSubSample32(unsigned char *pSrc, signed char *pLUM, signed char *pCb, signed char *pCr, int lsize, int cx, int cy);
void JPEGSample32(unsigned char *pSrc, signed char *pMCU, int lsize, int cx, int cy);
void JPEGSubSampleYUV422(uint8_t *pImage, int8_t *pMCUData, int iPitch);
void JPEGGetMCU22(unsigned char *pImage, JPEGE_IMAGE *pPage, int iPitch);
void JPEGSample16(unsigned char *pSrc, signed char *pMCU, int lsize, int cx, int cy);
void JPEGSample24(unsigned char *pSrc, signed char *pMCU, int lsize, int cx, int cy);
void JPEGGetMCU11(unsigned char *pImage, JPEGE_IMAGE *pPage, int iPitch);
void JPEGFDCT(signed char *pMCUSrc, signed short *pMCUDest);
void FlushCode(PIL_CODE *pPC);
int JPEGAddMCU(JPEGE_IMAGE *pJPEG, JPEGENCODE *pEncode, uint8_t *pPixels, int iPitch);
int JPEGAddFrame(JPEGE_IMAGE *pJPEG, JPEGENCODE *pEncode, uint8_t *pPixels, int iPitch);

#endif
