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
File        : GUIDRV_SPage_Private.h
Purpose     : Private interface of GUIDRV_SPage driver
---------------------------END-OF-HEADER------------------------------
*/

#include "GUIDRV_SPage.h"
#include "GUIDRV_NoOpt_1_8.h"

#ifndef GUIDRV_SPAGE_PRIVATE_H
#define GUIDRV_SPAGE_PRIVATE_H

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define SPAGE_MAX_NUM_DUMMY_READS 5

//
// Use unique context identified
//
#define DRIVER_CONTEXT DRIVER_CONTEXT_SPAGE

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
typedef struct DRIVER_CONTEXT DRIVER_CONTEXT;

/*********************************************************************
*
*       DRIVER_CONTEXT
*/
struct DRIVER_CONTEXT {
  //
  // Common data
  //
  int xSize, ySize;
  int vxSize, vySize;
  int NumDummyReads;
  int BitsPerPixel;
  int NumPages;
  //
  // Driver specific data
  //
  int FirstSEG;
  int FirstCOM;
  //
  // Accelerators for calculation
  //
  int BytesPerLine;
  //
  // Cache
  //
  U8 * pVRAM;
  int CacheLocked;
  int CacheStat;
  int xOff;
  U16 * pDirtyMin;
  U16 * pDirtyMax;
  //
  // Pointer to driver internal initialization routine
  //
  void (* pfInit)(GUI_DEVICE * pDevice);
  //
  // Display controller specific functions
  //
  void (* pfSetAddrW) (DRIVER_CONTEXT * pContext, int Column, int Page);
  void (* pfSetAddrR) (DRIVER_CONTEXT * pContext, int Column, int Page);
  U8   (* pfReadVMem) (DRIVER_CONTEXT * pContext, int Column, int Page);
  void (* pfWriteVMem)(DRIVER_CONTEXT * pContext, int Column, int Page, U8 Data);
  //
  // Writing and reading data, makes it possible to mirror pixels if required
  //
  U8   (* pfReadData) (U8 * pData);
  void (* pfWriteData)(U8 * pData, U8 Data);
  void (* pfWrite)    (DRIVER_CONTEXT * pContext, U8 Data);
  //
  // Hardware routines
  //
  GUI_PORT_API HW_API;
};

/*********************************************************************
*
*       LOG2PHYS_xxx
*/
#define LOG2PHYS_X      (                  x    )
#define LOG2PHYS_X_OX   (pContext->xSize - x - 1)
#define LOG2PHYS_X_OY   (                  x    )
#define LOG2PHYS_X_OXY  (pContext->xSize - x - 1)
#define LOG2PHYS_X_OS   (                  y    )
#define LOG2PHYS_X_OSX  (pContext->ySize - y - 1)
#define LOG2PHYS_X_OSY  (                  y    )
#define LOG2PHYS_X_OSXY (pContext->ySize - y - 1)

#define LOG2PHYS_Y      (                  y    )
#define LOG2PHYS_Y_OX   (                  y    )
#define LOG2PHYS_Y_OY   (pContext->ySize - y - 1)
#define LOG2PHYS_Y_OXY  (pContext->ySize - y - 1)
#define LOG2PHYS_Y_OS   (                  x    )
#define LOG2PHYS_Y_OSX  (                  x    )
#define LOG2PHYS_Y_OSY  (pContext->xSize - x - 1)
#define LOG2PHYS_Y_OSXY (pContext->xSize - x - 1)

/*********************************************************************
*
*       _SetPixelIndex_##EXT
*/
#define DEFINE_SETPIXELINDEX(EXT, X_PHYS, Y_PHYS)                                                 \
static void _SetPixelIndex_##EXT(GUI_DEVICE * pDevice, int x, int y, LCD_PIXELINDEX PixelIndex) { \
  DRIVER_CONTEXT * pContext;                                                                      \
                                                                                                  \
  pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;                                               \
  _SetPixelIndex(pContext, X_PHYS, Y_PHYS, PixelIndex);                                           \
}

/*********************************************************************
*
*       _GetPixelIndex_##EXT
*/
#define DEFINE_GETPIXELINDEX(EXT, X_PHYS, Y_PHYS)                                \
static LCD_PIXELINDEX _GetPixelIndex_##EXT(GUI_DEVICE * pDevice, int x, int y) { \
  DRIVER_CONTEXT * pContext;                                                     \
  LCD_PIXELINDEX PixelIndex;                                                     \
                                                                                 \
  pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;                              \
  PixelIndex = _GetPixelIndex(pContext, X_PHYS, Y_PHYS);                         \
  return PixelIndex;                                                             \
}

/*********************************************************************
*
*       _GetDevProp_##EXT
*/
#define DEFINE_GETDEVPROP(EXT, MX, MY, SWAP)                    \
static I32 _GetDevProp_##EXT(GUI_DEVICE * pDevice, int Index) { \
  switch (Index) {                                              \
  case LCD_DEVCAP_MIRROR_X: return MX;                          \
  case LCD_DEVCAP_MIRROR_Y: return MY;                          \
  case LCD_DEVCAP_SWAP_XY:  return SWAP;                        \
  default:                  break;                              \
  }                                                             \
  return _GetDevProp(pDevice, Index);                           \
}

/*********************************************************************
*
*       DEFINE_FUNCTIONS
*/
#define DEFINE_FUNCTIONS(EXT, X_PHYS, Y_PHYS, MX, MY, SWAP) \
  DEFINE_SETPIXELINDEX(EXT, X_PHYS, Y_PHYS)                 \
  DEFINE_GETPIXELINDEX(EXT, X_PHYS, Y_PHYS)                 \
  DEFINE_GETDEVPROP(EXT, MX, MY, SWAP)                      \
  DEFINE_GUI_DEVICE_API(EXT)

/*********************************************************************
*
*       Private functions, common
*
**********************************************************************
*/
void (*GUIDRV__SPage_GetDevFunc(GUI_DEVICE ** ppDevice, int Index))(void);
void   GUIDRV__SPage_SetOrg    (GUI_DEVICE *  pDevice,  int x, int y);
I32    GUIDRV__SPage_GetDevProp(GUI_DEVICE *  pDevice,  int Index);
void   GUIDRV__SPage_GetRect   (GUI_DEVICE *  pDevice,  LCD_RECT * pRect);
int    GUIDRV__SPage_Init      (GUI_DEVICE * pDevice);

/*********************************************************************
*
*       Private functions, cache related
*
**********************************************************************
*/
void GUIDRV__SPage_WriteCache  (DRIVER_CONTEXT * pContext, int x, int Page, U8 Data);
U8   GUIDRV__SPage_ReadCache   (DRIVER_CONTEXT * pContext, int x, int Page);
void GUIDRV__SPage_FlushCache  (DRIVER_CONTEXT * pContext);
void GUIDRV__SPage_AddDirtyRect(DRIVER_CONTEXT * pContext, int Page0, int Page1, int x0, int x1);
int  GUIDRV__SPage_ControlCache(GUI_DEVICE * pDevice, int Cmd);

/*********************************************************************
*
*       Simulation (Segger internal use only)
*
**********************************************************************
*/
#if defined(WIN32) && defined(LCD_SIMCONTROLLER)

  extern GUI_PORT_API SIM_SPage_HW_API;

  void SIM_SPage_SetFunc1502 (GUI_DEVICE * pDevice);
  void SIM_SPage_SetFunc1510 (GUI_DEVICE * pDevice);
  void SIM_SPage_SetFunc1512 (GUI_DEVICE * pDevice);
  void SIM_SPage_SetFunc1801 (GUI_DEVICE * pDevice);
  void SIM_SPage_SetFunc1802 (GUI_DEVICE * pDevice);
  void SIM_SPage_SetFunc1628 (GUI_DEVICE * pDevice);
  void SIM_SPage_SetFunc1638 (GUI_DEVICE * pDevice);
  void SIM_SPage_SetFunc75256(GUI_DEVICE * pDevice);
  void SIM_SPage_SetFunc75320(GUI_DEVICE * pDevice);
  void SIM_SPage_SetFunc7591 (GUI_DEVICE * pDevice);
  void SIM_SPage_Config      (GUI_DEVICE * pDevice, int xSize, int ySize, int FirstSEG, int FirstCOM, int BitsPerPixel, int NumDummyReads);

#endif

#if defined(__cplusplus)
}
#endif

#endif /* GUIDRV_SPAGE_PRIVATE_H */

/*************************** End of file ****************************/
