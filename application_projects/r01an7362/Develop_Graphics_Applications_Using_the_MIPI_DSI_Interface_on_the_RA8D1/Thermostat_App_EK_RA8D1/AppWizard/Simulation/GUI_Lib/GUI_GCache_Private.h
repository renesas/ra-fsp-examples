/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2024  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V6.44 - Graphical user interface for embedded applications **
emWin is protected by international copyright laws.   Knowledge of the
source code may not be used to write a similar product.  This file may
only  be used  in accordance  with  a license  and should  not be  re-
distributed in any way. We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : GUI_GCache_Private.h
Purpose     : Private header
---------------------------END-OF-HEADER------------------------------
*/

#ifndef GUI_GCACHE_PRIVATE_H
#define GUI_GCACHE_PRIVATE_H

#include "GUI_Private.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define DRIVER_CONTEXT DRIVER_CONTEXT_GCACHE

#define GUI_GCACHE_FLAGS_USER_BUFFER    (1 << 0)
#define GUI_GCACHE_FLAGS_COLORCONV_REQ  (1 << 1)

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
/*********************************************************************
*
*       DRIVER_CONTEXT
*/
typedef struct {
  int x0, y0, x1, y1, IsDirty;
  int xSize, ySize;
  int CacheLocked;
  int MemSize;
  int BitsPerPixel;
  int BytesPerLine;
  U32 Flags;
  //
  // Line buffer for reading operation
  //
  LCD_PIXELINDEX * pLineBuffer;
  //
  // Palette for drawing 'cache bitmap'
  //
  LCD_PIXELINDEX * pPalette;
  //
  // Cache management
  //
  void (* pfReadRect)     (GUI_DEVICE * pDevice, int _x0, int _y0, int _x1, int _y1, LCD_PIXELINDEX * pBuffer);
  void (* pfSendCacheRect)(GUI_DEVICE * pDevice);
  U32   * pVMEM;
  //
  // Drawing functions
  //
  void           (* pfDrawBitmap   )(GUI_DEVICE *  pDevice,  int _x0, int _y0, int xsize, int ysize, int _BitsPerPixel, int _BytesPerLine, const U8 * pData, int Diff, const LCD_PIXELINDEX * pTrans);
  void           (* pfDrawHLine    )(GUI_DEVICE *  pDevice,  int _x0, int _y0, int _x1);
  void           (* pfDrawVLine    )(GUI_DEVICE *  pDevice,  int _x , int _y0, int _y1);
  void           (* pfFillRect     )(GUI_DEVICE *  pDevice,  int _x0, int _y0, int _x1, int _y1);
  LCD_PIXELINDEX (* pfGetPixelIndex)(GUI_DEVICE *  pDevice,  int _x, int _y);
  void           (* pfSetPixelIndex)(GUI_DEVICE *  pDevice,  int _x, int _y, LCD_PIXELINDEX ColorIndex);
  void           (* pfXorPixel     )(GUI_DEVICE *  pDevice,  int _x, int _y);
  //
  // GetData function
  //
  const void    *(* pfGetDevData   )(GUI_DEVICE *  pDevice,  int Index);
} DRIVER_CONTEXT;

/*********************************************************************
*
*       Interface
*
**********************************************************************
*/
GUI_DEVICE * GUI_GCACHE__CreateEx           (int LayerIndex, const LCD_API_COLOR_CONV * pColorConvAPI, int BitsPerPixel, void * pUserBuffer);
const U16  * GUI_GCACHE__ConvertLineOnDemand(GUI_DEVICE * pDevice, const U16 * pSrc, int NumPixels);

#endif

/*************************** End of file ****************************/
