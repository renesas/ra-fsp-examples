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
File        : MOVIE.h
Purpose     : MOVIE public header file (API)
--------------------END-OF-HEADER-------------------------------------
*/

#ifndef MOVIE_H
#define MOVIE_H

#include "WM.h"
#include "DIALOG_Type.h"  // Req. for Create indirect data structure
#include "WIDGET.h"

#if GUI_WINSUPPORT

#if defined(__cplusplus)
  extern "C" {  // Make sure we have C-declarations in C++ programs
#endif

/*********************************************************************
*
*       MOVIE bitmap indexes
*
*  Description
*    Bitmap indexes for MOVIE widget.
*/
#define MOVIE_BI_CENTER_PLAY  0  // Bitmap to be shown as play button in the center of the widget.
#define MOVIE_BI_PANEL_PLAY   1  // Bitmap for panel play button.
#define MOVIE_BI_PANEL_PAUSE  2  // Bitmap for panel pause button.
#define MOVIE_BI_PANEL_BEGIN  3  // Bitmap for panel begin button.

/*********************************************************************
*
*       MOVIE color indexes
*
*  Description
*    Color indexes for MOVIE widget.
*/
#define MOVIE_CI_CENTER 0  // Color for center bitmap.
#define MOVIE_CI_PANEL  1  // Color for panel bitmaps.
#define MOVIE_CI_BK     2  // Color for background if MOVIE does not fill the complete widget area.
#define MOVIE_CI_LEFT   3  // Color for progress bar (left).
#define MOVIE_CI_RIGHT  4  // Color for progress bar (right).

/*********************************************************************
*
*       MOVIE period indexes
*
*  Description
*    Period indexes for MOVIE widgets.
*/
#define MOVIE_PI_SHIFT_IN  0  // The length in ms for shifting in the panel.
#define MOVIE_PI_SHIFT_OUT 1  // The length in ms for shifting out the panel.
#define MOVIE_PI_INACTIVE  2  // Period it takes from releasing the widget until the shifting operation is started.

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
typedef WM_HMEM MOVIE_Handle;

/*********************************************************************
*
*       Create function(s)

  Note: the parameters to a create function may vary.
         Some widgets may have multiple create functions
*/
MOVIE_Handle MOVIE_CreateUser(int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id, int NumExtraBytes);

/*********************************************************************
*
*       The callback ...
*
* Do not call it directly ! It is only to be used from within an
* overwritten callback.
*/
void MOVIE_Callback(WM_MESSAGE *pMsg);

/*********************************************************************
*
*       Private functions
*
**********************************************************************
*/
void MOVIE__CalcPanel (void * pVoid);
void MOVIE__CalcCenter(void * pVoid);

/*********************************************************************
*
*       Member functions
*
**********************************************************************
*/
int              MOVIE_GetBarSize     (MOVIE_Handle hObj);
GUI_COLOR        MOVIE_GetColor       (MOVIE_Handle hObj, unsigned ColorIndex);
GUI_MOVIE_HANDLE MOVIE_GethMovie      (MOVIE_Handle hObj);
int              MOVIE_GetInfo        (MOVIE_Handle hObj, GUI_MOVIE_INFO * pInfo);
int              MOVIE_GetPanelSize   (MOVIE_Handle hObj);
int              MOVIE_GetPeriod      (MOVIE_Handle hObj, unsigned PeriodIndex);
int              MOVIE_GetSpace       (MOVIE_Handle hObj);
int              MOVIE_GetUserData    (MOVIE_Handle hObj, void * pDest, int NumBytes);
void             MOVIE_GotoFrame      (MOVIE_Handle hObj, U32 Frame);
int              MOVIE_Pause          (MOVIE_Handle hObj);
int              MOVIE_Play           (MOVIE_Handle hObj);
void             MOVIE_SetBarSize     (MOVIE_Handle hObj, int ySizeBar);
void             MOVIE_SetBitmapObj   (MOVIE_Handle hObj, GUI_DRAW_HANDLE hDrawObj, unsigned Index);
void             MOVIE_SetCenterBitmap(MOVIE_Handle hObj, const GUI_BITMAP * pBmPlay);
void             MOVIE_SetColor       (MOVIE_Handle hObj, unsigned ColorIndex, GUI_COLOR Color);
void             MOVIE_SetColors      (MOVIE_Handle hObj, GUI_COLOR ColorCenter, GUI_COLOR ColorPanel, GUI_COLOR ColorLeft, GUI_COLOR ColorRight, GUI_COLOR ColorBk);
void             MOVIE_SetData        (MOVIE_Handle hObj, const U8 * pData, U32 Size, int DoLoop, GUI_MOVIE_FUNC * pfNotify);
void             MOVIE_SetDataEx      (MOVIE_Handle hObj, GUI_MOVIE_GET_DATA_FUNC * pfGetData, void * pVoidImage, void * pVoidTable, int DoLoop, GUI_MOVIE_FUNC * pfNotify);
void             MOVIE_SetPanelBitmaps(MOVIE_Handle hObj, const GUI_BITMAP * pBmPlay, const GUI_BITMAP * pBmPause, const GUI_BITMAP * pBmStart);
void             MOVIE_SetPeriod      (MOVIE_Handle hObj, unsigned PeriodIndex, int Period);
void             MOVIE_SetPeriods     (MOVIE_Handle hObj, int PeriodShiftIn, int PeriodShiftOut, int PeriodInactive);
void             MOVIE_SetPanelSize   (MOVIE_Handle hObj, int ySizePanel);
void             MOVIE_SetSizes       (MOVIE_Handle hObj, int ySizePanel, int ySizeBar, int Space);
void             MOVIE_SetSpace       (MOVIE_Handle hObj, int Space);
int              MOVIE_SetUserData    (MOVIE_Handle hObj, const void * pSrc, int NumBytes);

/*********************************************************************
*
*       Managing default values
*
**********************************************************************
*/
GUI_COLOR MOVIE_GetDefaultColor    (unsigned ColorIndex);
int       MOVIE_GetDefaultPeriod   (unsigned PeriodIndex);
int       MOVIE_GetDefaultSizeBar  (void);
int       MOVIE_GetDefaultSizePanel(void);
int       MOVIE_GetDefaultSpace    (void);
void      MOVIE_SetDefaultColor    (unsigned ColorIndex, GUI_COLOR Color);
void      MOVIE_SetDefaultPeriod   (unsigned PeriodIndex, int Period);
void      MOVIE_SetDefaultSizeBar  (int SizeBar);
void      MOVIE_SetDefaultSizePanel(int SizePanel);
void      MOVIE_SetDefaultSpace    (int Space);

#if defined(__cplusplus)
  }
#endif

#endif   // GUI_WINSUPPORT
#endif   // MOVIE_H

/*************************** End of file ****************************/
