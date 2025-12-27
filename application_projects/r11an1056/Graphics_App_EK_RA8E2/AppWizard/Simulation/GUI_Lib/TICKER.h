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

** emWin V6.48 - Graphical user interface for embedded applications **
emWin is protected by international copyright laws.   Knowledge of the
source code may not be used to write a similar product.  This file may
only  be used  in accordance  with  a license  and should  not be  re-
distributed in any way. We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : TICKER.h
Purpose     : TICKER include
--------------------END-OF-HEADER-------------------------------------
*/

#ifndef TICKER_H
#define TICKER_H

#include "WM.h"
#include "DIALOG_Type.h"      /* Req. for Create indirect data structure */
#include "WIDGET.h"

#if GUI_WINSUPPORT

#if defined(__cplusplus)
  extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/************************************************************
*
*       #defines
*
*************************************************************
*/

/************************************************************
*
*       Create / Status flags
*/
#define TICKER_CF_LEFT         GUI_TA_LEFT
#define TICKER_CF_RIGHT        GUI_TA_RIGHT
#define TICKER_CF_HCENTER      GUI_TA_HCENTER

#define TICKER_CF_VCENTER      GUI_TA_VCENTER
#define TICKER_CF_TOP          GUI_TA_TOP
#define TICKER_CF_BOTTOM       GUI_TA_BOTTOM

#define TICKER_MODE_CONTINOUS  (0)
#define TICKER_MODE_ANIMATED   (1)

#define TICKER_DIR_HORIZONTAL  (0)
#define TICKER_DIR_VERTICAL    (1)

/*********************************************************************
*
*       Public Types
*
**********************************************************************

*/
typedef WM_HMEM TICKER_Handle;

/*********************************************************************
*
*       Create functions
*
**********************************************************************
*/
TICKER_Handle TICKER_CreateUser    (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id, const char * pText, int NumExtraBytes);
TICKER_Handle TICKER_CreateIndirect(const GUI_WIDGET_CREATE_INFO * pCreateInfo, WM_HWIN hWinParent, int x0, int y0, WM_CALLBACK * cb);

/*********************************************************************
*
*       The callback ...
*
* Do not call it directly ! It is only to be used from within an
* overwritten callback.
*/
void TICKER_Callback(WM_MESSAGE * pMsg);

/*********************************************************************
*
*       Member functions
*
**********************************************************************
*/

/* Methods changing properties */

GUI_COLOR            TICKER_GetBkColor          (TICKER_Handle hObj); 
U32                  TICKER_GetCurrentItemIndex (TICKER_Handle hObj);
U32                  TICKER_GetClickedItemIndex (TICKER_Handle hObj);
const GUI_FONT *     TICKER_GetFont             (TICKER_Handle hObj);
int                  TICKER_GetItemText         (TICKER_Handle hObj, char * pDest, U32 BufferSize, U32 ItemIndex);
int                  TICKER_GetTextAlign        (TICKER_Handle hObj);
GUI_COLOR            TICKER_GetTextColor        (TICKER_Handle hObj);
GUI_COLOR            TICKER_GetFrameColor       (TICKER_Handle hObj);
int                  TICKER_GetUserData         (TICKER_Handle hObj, void * pDest, int NumBytes);
void                 TICKER_MoveToItem          (TICKER_Handle hObj, int ItemIndex, U8 Dir);
void                 TICKER_MoveToNextItem      (TICKER_Handle hObj, U8 Dir, U8 Auto);
void                 TICKER_MoveToPrevItem      (TICKER_Handle hObj, U8 Dir, U8 Auto);
void                 TICKER_SetBkColor          (TICKER_Handle hObj, GUI_COLOR Color);
void                 TICKER_SetConsecutive      (TICKER_Handle hObj, int OnOff);
void                 TICKER_SetContinuousMode   (TICKER_Handle hObj);
void                 TICKER_SetCurrentItemIndex (TICKER_Handle hObj, U32 ItemIndex);
void                 TICKER_SetFont             (TICKER_Handle hObj, const GUI_FONT * pFont);
void                 TICKER_SetFrameColor       (TICKER_Handle hObj, GUI_COLOR Color);
void                 TICKER_SetItemDist         (TICKER_Handle hObj, int Dist);
int                  TICKER_SetItems            (TICKER_Handle hObj, const char ** ppStrings, U32 NumItems);
int                  TICKER_SetItemText         (TICKER_Handle hObj, const char * pString, U32 ItemIndex);
void                 TICKER_SetMovement         (TICKER_Handle hObj, int Dist, int Period);
void                 TICKER_SetTextAlign        (TICKER_Handle hObj, int Align);
void                 TICKER_SetTextColor        (TICKER_Handle hObj, GUI_COLOR Color);
void                 TICKER_SetWrapMode         (TICKER_Handle hObj, GUI_WRAPMODE WrapMode);
void                 TICKER_Start               (TICKER_Handle hObj);
void                 TICKER_Stop                (TICKER_Handle hObj);
int                  TICKER_SetUserData         (TICKER_Handle hObj, const void * pSrc, int NumBytes);

/*********************************************************************
*
*       Managing default values
*
**********************************************************************
*/

U16              TICKER_GetDefaultAnimDist(void);
int              TICKER_GetDefaultAnimPeriod(void);
GUI_COLOR        TICKER_GetDefaultBkColor(void);
U16              TICKER_GetDefaultDist(void);
const GUI_FONT * TICKER_GetDefaultFont(void);
GUI_COLOR        TICKER_GetDefaultFrameColor(void);
GUI_COLOR        TICKER_GetDefaultTextColor(void);
void             TICKER_SetDefaultAnimDist(U16 AnimDist);
void             TICKER_SetDefaultAnimPeriod(int AnimPeriod);
void             TICKER_SetDefaultBkColor(GUI_COLOR Color);
void             TICKER_SetDefaultDist(U16 Dist);
void             TICKER_SetDefaultFont(const GUI_FONT * pFont);
void             TICKER_SetDefaultFrameColor(GUI_COLOR Color);
void             TICKER_SetDefaultTextColor(GUI_COLOR Color);

#if defined(__cplusplus)
  }
#endif

#endif  // GUI_WINSUPPORT
#endif  // TICKER_H

/*************************** End of file ****************************/
