/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2020  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V6.14 - Graphical user interface for embedded applications **
emWin is protected by international copyright laws.   Knowledge of the
source code may not be used to write a similar product.  This file may
only  be used  in accordance  with  a license  and should  not be  re-
distributed in any way. We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : QRCODE_Private.h
Purpose     : Internal header file
--------------------END-OF-HEADER-------------------------------------
*/

#ifndef QRCODE_PRIVATE_H
#define QRCODE_PRIVATE_H

#include "WM_Intern.h"
#include "QRCODE.h"

#if (GUI_WINSUPPORT)

/*********************************************************************
*
*       Object definition
*
**********************************************************************
*/
typedef struct {
  GUI_COLOR Color;
  GUI_COLOR BkColor;
} QRCODE_PROPS;

typedef struct {
  WIDGET       Widget;
  QRCODE_PROPS Props;
  GUI_HMEM     hMem;
  WM_HMEM      hText;
  int          PixelSize;
  int          EccLevel;
  int          Version;
} QRCODE_OBJ;

/*********************************************************************
*
*       Macros for internal use
*
**********************************************************************
*/
#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  #define QRCODE_INIT_ID(p) (p->Widget.DebugId = QRCODE_ID)
#else
  #define QRCODE_INIT_ID(p)
#endif

#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  QRCODE_OBJ * QRCODE_LockH(QRCODE_Handle h);
  #define QRCODE_LOCK_H(h) QRCODE_LockH(h)
#else
  #define QRCODE_LOCK_H(h) (QRCODE_OBJ *)GUI_LOCK_H(h)
#endif

/*********************************************************************
*
*       Public data (internal defaults)
*
**********************************************************************
*/
extern QRCODE_PROPS QRCODE__DefaultProps;

#endif  // (GUI_SUPPORT_MEMDEV && GUI_WINSUPPORT)
#endif  // QRCODE_PRIVATE_H
