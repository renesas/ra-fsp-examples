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
File        : GUIDRV_SLinEPD_Private.h
Purpose     : Interface definition for GUIDRV_SLinEPD driver
---------------------------END-OF-HEADER------------------------------
*/

#include "GUIDRV_SLinEPD.h"

#ifndef GUIDRV_SLINEPD_PRIVATE_H
#define GUIDRV_SLINEPD_PRIVATE_H

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
//
// Use unique context identified
//
#define DRIVER_CONTEXT DRIVER_CONTEXT_SLINEPD

//
//      Command definitions for Solomon SSD1673
//
#define CMD_SSD1673_SET_RAM_X_AREA    0x44  // X0, X1
#define CMD_SSD1673_SET_RAM_Y_AREA    0x45  // Y0, Y1
#define CMD_SSD1673_WRITE_RAM         0x24
#define CMD_SSD1673_UPDATE_CONTROL_2  0x22  // Option
#define CMD_SSD1673_MASTER_ACTIVATION 0x20
#define CMD_SSD1673_ENTER_DEEP_SLEEP  0x10  // 0 - normal, 1 - sleep
#define CMD_SSD1673_SET_RAM_X_POS     0x4E  // X
#define CMD_SSD1673_SET_RAM_Y_POS     0x4F  // Y
#define CMD_SSD1673_WRITE_LUT         0x32

//
// Command definitions for UltraChip UC8451
//
#define CMD_UC8451_PANEL_SETTING            0x00
#define CMD_UC8451_POWER_ON_DISPLAY         0x04
#define CMD_UC8451_DEEP_SLEEP               0x07
#define CMD_UC8451_START_FRAME_1            0x10
#define CMD_UC8451_DISPLAY_REFRESH          0x12
#define CMD_UC8451_START_FRAME_2            0x13
#define CMD_UC8451_VCOM_LUT                 0x20
#define CMD_UC8451_W2W_LUT                  0x21
#define CMD_UC8451_B2W_LUT                  0x22
#define CMD_UC8451_W2B_LUT                  0x23
#define CMD_UC8451_B2B_LUT                  0x24
#define CMD_UC8451_PLL_CONTROL              0x30
#define CMD_UC8451_VCOM_AND_DATA_INTERVAL   0x50
#define CMD_UC8451_VCOM_DC_SETTING          0x82
#define CMD_UC8451_PARTIAL_WINDOW           0x90
#define CMD_UC8451_PARTIAL_IN               0x91
#define CMD_UC8451_PARTIAL_OUT              0x92
#define CMD_UC8451_FORCE_TEMPERATURE        0xE5

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
  int              xSize, ySize;
  int              vxSize, vySize;
  int              xSizePhys, ySizePhys;
  int              BytesPerLine;
  int              BitsPerPixel;
  int              PixelsPerByte;
  U32              MemSize;
  GUI_RECT         rDirty;       // !!! NOT PIXELS BUT COMPLETE BYTES !!!
  U32              AutoUpdatePeriod;
  GUI_TIMER_HANDLE hTimer;
  GUI_TIMER_TIME   tLast;
  U8               LayerIndex;
  U8               SleepActive;
  U8               UpdateRequired;
  U8             * pVMEM;
  U8             * pVMEMOld;
  U8               CacheLocked;
  U8               IsDirty;
  U8               InitialUpdateDone;
  U8               PartialMode;
  U8               FastMode;
  //
  // Hardware routines
  //
  GUI_PORT_API     HW_API;         // Hardware routines
  //
  // Function pointers
  //
  void (* pfAddDirtyByte)(DRIVER_CONTEXT * pContext, int x,  int y);
  void (* pfSetRect)     (DRIVER_CONTEXT * pContext, int x0, int y0, int x1, int y1);
  void (* pfRefresh)     (DRIVER_CONTEXT * pContext);
  void (* pfInit)        (GUI_DEVICE     * pDevice);
  int  (* pfCheck)       (DRIVER_CONTEXT * pContext);
  int  (* pfFlushCache)  (DRIVER_CONTEXT * pContext);
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
  LCD_PIXELINDEX PixelIndex;                                                     \
  DRIVER_CONTEXT * pContext;                                                     \
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
*       Private functions
*
**********************************************************************
*/
void   GUIDRV__SLinEPD_FlushCacheOnDemand(DRIVER_CONTEXT * pContext);
//
// Device API functions
//
void (*GUIDRV__SLinEPD_GetDevFunc        (GUI_DEVICE ** ppDevice, int Index))(void);
void   GUIDRV__SLinEPD_SetOrg            (GUI_DEVICE *  pDevice,  int x, int y);
I32    GUIDRV__SLinEPD_GetDevProp        (GUI_DEVICE *  pDevice,  int Index);
void   GUIDRV__SLinEPD_GetRect           (GUI_DEVICE *  pDevice,  LCD_RECT * pRect);
//
// Drawing functions
//
void   GUIDRV__SLinEPD_XorPixel    (GUI_DEVICE * pDevice, int x, int y);
void   GUIDRV__SLinEPD_DrawHLine   (GUI_DEVICE * pDevice, int x0, int y,  int x1);
void   GUIDRV__SLinEPD_DrawVLine   (GUI_DEVICE * pDevice, int x, int y0,  int y1);
void   GUIDRV__SLinEPD_FillRect    (GUI_DEVICE * pDevice, int x0, int y0, int x1, int y1);
void   GUIDRV__SLinEPD_DrawBitmap  (GUI_DEVICE * pDevice, int x0, int y0, int xSize, int ySize, int BitsPerPixel, int BytesPerLine, const U8 * pData, int Diff, const LCD_PIXELINDEX * pTrans);

/*********************************************************************
*
*       Simulation (Segger internal use only)
*
**********************************************************************
*/
#if defined(WIN32) && defined(LCD_SIMCONTROLLER)
  void SIM_SLinEPD_Write8_A0 (U8 Data);
  void SIM_SLinEPD_Write8_A1 (U8 Data);
  void SIM_SLinEPD_WriteM8_A1(U8 * pData, int NumItems);
  U8   SIM_SLinEPD_Read8_A0  (void);
  void SIM_SLinEPD_Config    (GUI_DEVICE * pDevice, int xSize, int ySize, int BitsPerPixel, U32 AutoUpdatePeriod);
  void SIM_SLinEPD_SetSSD1673(GUI_DEVICE * pDevice);
#endif

#endif

/*************************** End of file ****************************/
