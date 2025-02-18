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
File        : WHEEL.h
Purpose     : WHEEL include
--------------------END-OF-HEADER-------------------------------------
*/

#ifndef WHEEL_H
#define WHEEL_H

#include "WM.h"
#include "DIALOG_Type.h"      // Req. for Create indirect data structure
#include "WIDGET.h"

#if (GUI_SUPPORT_MEMDEV && GUI_WINSUPPORT)

#if defined(__cplusplus)
  extern "C" {                  // Make sure we have C-declarations in C++ programs
#endif

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
/************************************************************
*
*       WHEEL create flags
* 
*  Description
*    Flags that are passed when the WHEEL is created.
*/
#define WHEEL_CF_HORIZONTAL 0                   // The WHEEL moves horizontally.
#define WHEEL_CF_VERTICAL   WIDGET_CF_VERTICAL  // The WHEEL moves vertically.

/************************************************************
*
*       WHEEL mode flags
* 
*  Description
*   Flags that define the movement behavior of the widget and
*   the WHEEL's cells.
* 
*  Additional information
*    A more detailed explanation of the WHEEL modes can be found
*    at the beginnging of the chapter.
*/
#define WHEEL_MODE_ENDLESS 0                   // The WHEEL's list moves continuously and does not stop when the end
                                               // of the list has been reached.
#define WHEEL_MODE_STOP    WIDGET_STATE_USER0  // The WHEEL's list moves from the first element to the last one and stops
                                               // when the last element has been reached.
#define WHEEL_MODE_PLAIN   0                   // Puts the WHEEL into plain mode. While the list is moving the items don't
                                               // morph into one another, but the center item is highlighted.
#define WHEEL_MODE_MORPH   WIDGET_STATE_USER1  // Puts the WHEEL into morph mode. While the list is moving one item
                                               // morphs into the next item.

/************************************************************
*
*       WHEEL overlay indexes
* 
*  Description
*    Indexes for the WHEEL's overlay bitmaps.
*/
#define WHEEL_OI_FIRST  0  // First overlay bitmap that is drawn above the WHEEL.
#define WHEEL_OI_SECOND 1  // Second overlay bitmap that is drawn above the WHEEL.
#define WHEEL_OI_THIRD  2  // Third overlay bitmap that is drawn above the WHEEL.

/*********************************************************************
*
*       Public Types
*
**********************************************************************
*/
typedef WM_HMEM WHEEL_Handle;

/*********************************************************************
*
*       Create functions
*
**********************************************************************
*/
WHEEL_Handle WHEEL_CreateEx      (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id);
WHEEL_Handle WHEEL_CreateUser    (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id, int NumExtraBytes);
WHEEL_Handle WHEEL_CreateIndirect(const GUI_WIDGET_CREATE_INFO * pCreateInfo, WM_HWIN hWinParent, int x0, int y0, WM_CALLBACK * cb);

/*********************************************************************
*
*       The callback ...
*
* Do not call it directly ! It is only to be used from within an
* overwritten callback.
*/
void WHEEL_Callback(WM_MESSAGE * pMsg);

/*********************************************************************
*
*       Member functions
*
**********************************************************************
*/
int  WHEEL_AddBitmaps            (WHEEL_Handle hObj, const GUI_BITMAP     ** ppBitmap, unsigned NumItems, U8 Align, I16 xOff, I16 yOff, GUI_COLOR Color);
int  WHEEL_AddItemText           (WHEEL_Handle hObj, unsigned Index, const char * s);
int  WHEEL_AddText               (WHEEL_Handle hObj, const GUI_ConstString * ppText,   unsigned NumItems, U8 Align, I16 xOff, I16 yOff, GUI_COLOR Color, const GUI_FONT * pFont);
void WHEEL_ClrCenterBox          (WHEEL_Handle hObj);
void WHEEL_ClrCenterBitmaps      (WHEEL_Handle hObj, unsigned Index);
void WHEEL_ClrCenterText         (WHEEL_Handle hObj, unsigned Index);
int  WHEEL_DelItemText           (WHEEL_Handle hObj, unsigned Index, unsigned Row);
int  WHEEL_GetItemText           (WHEEL_Handle hObj, unsigned Index, unsigned Row, char * pBuffer, int MaxLen);
int  WHEEL_GetNumText            (WHEEL_Handle hObj);
int  WHEEL_GetNumTextItems       (WHEEL_Handle hObj, unsigned Index);
int  WHEEL_GetReleasedItem       (WHEEL_Handle hObj);
int  WHEEL_GetSel                (WHEEL_Handle hObj);
int  WHEEL_GetUserData           (WHEEL_Handle hObj, void * pDest, int NumBytes);
int  WHEEL_InsertItemText        (WHEEL_Handle hObj, unsigned Index, unsigned Row, const char * s);
int  WHEEL_IsMoving              (WHEEL_Handle hObj);
int  WHEEL_OwnerDrawMorph        (const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);
int  WHEEL_OwnerDrawPlain        (const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);
void WHEEL_MoveToPos             (WHEEL_Handle hObj, int Index);
void WHEEL_MoveToPosEx           (WHEEL_Handle hObj, int Index, U32 Period);
void WHEEL_SetAlign              (WHEEL_Handle hObj, U8 Align);
void WHEEL_SetBkColor            (WHEEL_Handle hObj, GUI_COLOR BkColor);
void WHEEL_SetBox                (WHEEL_Handle hObj, GUI_COLOR Color, GUI_COLOR FrameColor, U8 FrameRadius, U8 FrameSize);
void WHEEL_SetCellBitmapAttrMorph(WHEEL_Handle hObj, unsigned Index, U8 Align, I16 xOff, I16 yOff, GUI_COLOR Color);
void WHEEL_SetCellColorPlain     (WHEEL_Handle hObj, GUI_COLOR Color);
void WHEEL_SetCenterBitmapAttr   (WHEEL_Handle hObj, unsigned Index, U8 Align, I16 xOff, I16 yOff, GUI_COLOR Color);
void WHEEL_SetCenterBitmaps      (WHEEL_Handle hObj, unsigned Index, const GUI_BITMAP ** ppBitmap, unsigned NumItems, U8 Align, I16 xOff, I16 yOff, GUI_COLOR Color);
void WHEEL_SetCenterBox          (WHEEL_Handle hObj, GUI_COLOR Color, GUI_COLOR FrameColor, U8 FrameRadius, U8 FrameSize);
void WHEEL_SetCenterColorPlain   (WHEEL_Handle hObj, GUI_COLOR Color);
void WHEEL_SetCenterTextAttrMorph(WHEEL_Handle hObj, unsigned Index, U8 Align, I16 xOff, I16 yOff, GUI_COLOR Color);
void WHEEL_SetCenterTextAttrPlain(WHEEL_Handle hObj, unsigned Index, U8 Align, I16 xOff, I16 yOff, GUI_COLOR Color, const GUI_FONT * pFont);
void WHEEL_SetHBorder            (WHEEL_Handle hObj, U16 BorderText);
int  WHEEL_SetItemText           (WHEEL_Handle hObj, unsigned Index, unsigned Row, const char * s);
void WHEEL_SetMode               (WHEEL_Handle hObj, U16 Mode);
void WHEEL_SetOwnerDraw          (WHEEL_Handle hObj, WIDGET_DRAW_ITEM_FUNC * pfDrawItem);
void WHEEL_SetOverlay            (WHEEL_Handle hObj, unsigned Index, const GUI_BITMAP * pBitmap, U8 Align, I16 xOff, I16 yOff);
void WHEEL_SetOverlayColor       (WHEEL_Handle hObj, unsigned Index, GUI_COLOR Color);
void WHEEL_SetPeriod             (WHEEL_Handle hObj, U32 Period);
void WHEEL_SetSel                (WHEEL_Handle hObj, int Index);
void WHEEL_SetSizesMorph         (WHEEL_Handle hObj, U16 CellSize, U16 CenterSize, U16 Cutaway, U8 Align);
void WHEEL_SetSizesPlain         (WHEEL_Handle hObj, U16 CellSize, U16 CenterSize);
int  WHEEL_SetUserData           (WHEEL_Handle hObj, const void * pSrc, int NumBytes);

/*********************************************************************
*
*       Global functions
*
**********************************************************************
*/
void WHEEL_SetDefaultPeriod(int Period);
int  WHEEL_GetDefaultPeriod(void);

#if defined(__cplusplus)
  }
#endif

#endif  // (GUI_SUPPORT_MEMDEV && GUI_WINSUPPORT)
#endif  // WHEEL_H

/*************************** End of file ****************************/
