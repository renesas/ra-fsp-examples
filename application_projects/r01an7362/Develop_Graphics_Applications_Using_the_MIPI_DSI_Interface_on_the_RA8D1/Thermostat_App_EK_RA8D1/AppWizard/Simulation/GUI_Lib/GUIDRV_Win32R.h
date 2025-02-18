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
File        : GUIDRV_Win32R.h
Purpose     : Interface definition for GUIDRV_Win32R driver
---------------------------END-OF-HEADER------------------------------
*/

#ifndef GUIDRV_WIN32R_H
#define GUIDRV_WIN32R_H

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

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
  _SetPixelIndex(pDevice, X_PHYS, Y_PHYS, PixelIndex);                                            \
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
  PixelIndex = _GetPixelIndex(pDevice, X_PHYS, Y_PHYS);                          \
  return PixelIndex;                                                             \
}

/*********************************************************************
*
*       _SetPixelIndex_##EXT
*/
#define DEFINE_XORPIXEL(EXT, X_PHYS, Y_PHYS)                      \
static void _XorPixel_##EXT(GUI_DEVICE * pDevice, int x, int y) { \
  DRIVER_CONTEXT * pContext;                                      \
                                                                  \
  pContext = (DRIVER_CONTEXT *)pDevice->u.pContext;               \
  _XorPixel(pDevice, X_PHYS, Y_PHYS);                             \
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
  DEFINE_XORPIXEL(EXT, X_PHYS, Y_PHYS)                      \
  DEFINE_GETDEVPROP(EXT, MX, MY, SWAP)                      \
  DEFINE_GUI_DEVICE_API(EXT)

extern const GUI_DEVICE_API GUIDRV_Win32R_API;
extern const GUI_DEVICE_API GUIDRV_Win32R_OY_API;
extern const GUI_DEVICE_API GUIDRV_Win32R_OX_API;
extern const GUI_DEVICE_API GUIDRV_Win32R_OXY_API;
extern const GUI_DEVICE_API GUIDRV_Win32R_OS_API;
extern const GUI_DEVICE_API GUIDRV_Win32R_OSY_API;
extern const GUI_DEVICE_API GUIDRV_Win32R_OSX_API;
extern const GUI_DEVICE_API GUIDRV_Win32R_OSXY_API;

#define GUIDRV_WIN32R      &GUIDRV_Win32R_API
#define GUIDRV_WIN32R_OX   &GUIDRV_Win32R_OX_API
#define GUIDRV_WIN32R_OY   &GUIDRV_Win32R_OY_API
#define GUIDRV_WIN32R_OXY  &GUIDRV_Win32R_OXY_API
#define GUIDRV_WIN32R_OS   &GUIDRV_Win32R_OS_API
#define GUIDRV_WIN32R_OSX  &GUIDRV_Win32R_OSX_API
#define GUIDRV_WIN32R_OSY  &GUIDRV_Win32R_OSY_API
#define GUIDRV_WIN32R_OSXY &GUIDRV_Win32R_OSXY_API

void GUIDRV_Win32R_Config(void);  // Dummy

#if defined(__cplusplus)
}
#endif

#endif

/*************************** End of file ****************************/
