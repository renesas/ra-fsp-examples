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
File        : GAUGE.h
Purpose     : GAUGE public header file (API)
--------------------END-OF-HEADER-------------------------------------
*/

#ifndef GAUGE_H
#define GAUGE_H

#include "WM.h"
#include "DIALOG_Type.h"      /* Req. for Create indirect data structure */
#include "WIDGET.h"

#if GUI_WINSUPPORT

#if defined(__cplusplus)
  extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/*********************************************************************
*
*       GAUGE curved flags
*
*  Description
*    With these flags the drawing of the widget's arc lines can be set to have
*    round edges. The flags can be used upon creation of the GAUGE widget.
*/
#define GAUGE_CURVED_VALUE  (1 << 0)  // The arc that is drawn for the GAUGE's value will have a
                                      // curved edge on the beginning and end of the line.
#define GAUGE_CURVED_END    (1 << 1)  // The background arc will be drawn with a curved edge on the
                                      // beginning and end of the line.
#define GAUGE_DIRECTION_CCW (1 << 2)  // Widget works counterclockwise

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
typedef WM_HMEM GAUGE_Handle;

/*********************************************************************
*
*       Create function(s)

  Note: the parameters to a create function may vary.
         Some widgets may have multiple create functions
*/
GAUGE_Handle GAUGE_CreateUser    (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id, int NumExtraBytes);
GAUGE_Handle GAUGE_CreateIndirect(const GUI_WIDGET_CREATE_INFO * pCreateInfo, WM_HWIN hWinParent, int x0, int y0, WM_CALLBACK * cb);

/*********************************************************************
*
*       The callback ...
*
* Do not call it directly ! It is only to be used from within an
* overwritten callback.
*/
void GAUGE_Callback(WM_MESSAGE *pMsg);

/*********************************************************************
*
*       Member functions
*
**********************************************************************
*/
void GAUGE_EnableCCW      (GAUGE_Handle hObj, int OnOff);
void GAUGE_GetArcPositions(GAUGE_Handle hObj, unsigned Index, GUI_POINT * pStart, GUI_POINT * pEnd);
I32  GAUGE_GetValue       (GAUGE_Handle hObj);
void GAUGE_GetRange       (GAUGE_Handle hObj, I32 * pAng0, I32 * pAng1);
void GAUGE_SetAlign       (GAUGE_Handle hObj, int Align);
void GAUGE_SetBkColor     (GAUGE_Handle hObj, GUI_COLOR BkColor);
void GAUGE_SetColor       (GAUGE_Handle hObj, unsigned Index, GUI_COLOR Color);
void GAUGE_SetWidth       (GAUGE_Handle hObj, unsigned Index, int Width);
void GAUGE_SetOffset      (GAUGE_Handle hObj, int xOff, int yOff);
void GAUGE_SetRadius      (GAUGE_Handle hObj, int Radius);
void GAUGE_SetRange       (GAUGE_Handle hObj, I32 Ang0, I32 Ang1);
void GAUGE_SetRoundedEnd  (GAUGE_Handle hObj, int OnOff);
void GAUGE_SetRoundedValue(GAUGE_Handle hObj, int OnOff);
void GAUGE_SetValueRange  (GAUGE_Handle hObj, I32 Min, I32 Max);
void GAUGE_SetValue       (GAUGE_Handle hObj, I32 Value);

#if defined(__cplusplus)
  }
#endif

#endif  // GUI_WINSUPPORT
#endif  // GAUGE_H

/*************************** End of file ****************************/

