/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2025  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V6.50 - Graphical user interface for embedded applications **
emWin is protected by international copyright laws.   Knowledge of the
source code may not be used to write a similar product.  This file may
only  be used  in accordance  with  a license  and should  not be  re-
distributed in any way. We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : WM_Layout.h
Purpose     : Public header file (API)
--------------------END-OF-HEADER-------------------------------------
*/

#ifndef WM_LAYOUT_H
#define WM_LAYOUT_H

#include "WM_Intern.h"
#include "GUI_ARRAY.h"

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

#if GUI_WINSUPPORT
/*********************************************************************
*
*       Typedefs
*
**********************************************************************
*/
typedef WM_HMEM WM_HLAYOUT;
typedef WM_HMEM WM_HLAYOUT_ITEM;

//
// Policy FIXED or GROW
//
typedef enum {
  WM_LAYOUT_POLICY_FIXED = 1,
  WM_LAYOUT_POLICY_GROW
} WM_LAYOUT_POLICY;

//
// Item type
//
typedef enum {
  WM_LAYOUT_ITEM_TYPE_WINDOW = 1,
  WM_LAYOUT_ITEM_TYPE_LAYOUT,
  WM_LAYOUT_ITEM_TYPE_SPACER
} WM_LAYOUT_ITEM_TYPE;

//
// Direction
//
typedef enum {
  WM_LAYOUT_LTR = 1,
  WM_LAYOUT_RTL,
  WM_LAYOUT_TTB,
  WM_LAYOUT_BTT
} WM_LAYOUT_DIRECTION;

//
// Spread or Float
//
typedef enum {
  WM_LAYOUT_ARRANGE_SPREAD = 1,
  WM_LAYOUT_ARRANGE_FLOAT
} WM_LAYOUT_ARRANGE;

//
// Type of layout: HBox, VBox or Grid
//
typedef enum {
  WM_LAYOUT_TYPE_HBOX = 1,
  WM_LAYOUT_TYPE_VBOX,
  WM_LAYOUT_TYPE_GRID
} WM_LAYOUT_TYPE;

//
// Layout item...
//
typedef struct {
  WM_LAYOUT_ITEM_TYPE Type;
  //
  // For calculation purpose...
  //
  U16 xPos;
  U16 yPos;
  U16 xSize;
  U16 ySize;
  U8 StretchI;
  //
  // Min- & Max size
  //
  U16 xSizeMin;
  U16 ySizeMin;
  U16 xSizeMax;
  U16 ySizeMax;
  //
  // Common attribs
  //
  U16 Col;
  U16 Row;
  U16 ColSpan;
  U16 RowSpan;
  U8 Stretch;
  U8 Align;
} WM_LAYOUT_ITEM;

//
// ...Window
//
typedef struct {
  WM_LAYOUT_ITEM Item;                                             // Is WM_LAYOUT_ITEM
  WM_HWIN hWin;                                                    // Handle of window to be managed
} WM_LAYOUT_ITEM_WINDOW;

//
// ...Layout
//
typedef struct {
  WM_LAYOUT_ITEM Item;                                             // Is WM_LAYOUT_ITEM
  WM_HLAYOUT hLayout;                                                 // Handle of layout to be managed
} WM_LAYOUT_ITEM_LAYOUT;

//
// ...Spacer
//
typedef struct {
  WM_LAYOUT_ITEM Item;                                             // Is WM_LAYOUT_ITEM
  WM_LAYOUT_POLICY xPolicy;
  WM_LAYOUT_POLICY yPolicy;
} WM_LAYOUT_ITEM_SPACER;

//
// Layout...
//
typedef struct {
  WM_LAYOUT_TYPE Type;
  GUI_ARRAY hItems;
  U16 Spacing;
  U16 MarginL;
  U16 MarginR;
  U16 MarginT;
  U16 MarginB;
  //
  // Min- & Max size
  //
  U16 xSizeMin;
  U16 ySizeMin;
  U16 xSizeMax;
  U16 ySizeMax;
  //
  // Window handle associated with (main) layout
  //
  WM_HWIN    hWin;                                                 // Handle of window which has assigned this (main)layout
  WM_HLAYOUT hNext;                                                // Handle of next (main)layout
  int (* pfUpdatePositions) (WM_HLAYOUT hLayout, GUI_RECT * pRect);
} WM_LAYOUT;

//
// ...Box
//
typedef struct {
  WM_LAYOUT Layout;                                                // Is WM_LAYOUT
  WM_LAYOUT_DIRECTION Dir;
  U8 Align;
} WM_LAYOUT_BOX;

//
// ...Grid
//
typedef struct {
  WM_LAYOUT Layout;                                                // Is WM_LAYOUT
  GUI_ARRAY aCols;
  GUI_ARRAY aRows;
  U16       SpacingH;                                              // Global spacing for layout
  U16       SpacingV;                                              // Global spacing for layout
} WM_LAYOUT_GRID;

/*********************************************************************
*
*       API Private
*
**********************************************************************
*/
void            WM_LAYOUT__GetInitalPos        (WM_LAYOUT * pLayout, GUI_RECT * pRect, int * pxPos, int * pyPos);
int             WM_LAYOUT__UpdateBoxLayout     (WM_HLAYOUT hLayout, GUI_RECT * pRect);
int             WM_LAYOUT__UpdateGridLayout    (WM_HLAYOUT hLayout, GUI_RECT * pRect);
int             WM_LAYOUT__UpdateBoxLayoutFloat(WM_HLAYOUT hLayout, GUI_RECT * pRect);
WM_HLAYOUT      WM_LAYOUT__CreateLayout        (WM_LAYOUT_TYPE Type, int (* pfUpdatePositions) (WM_HLAYOUT, GUI_RECT *));
WM_HLAYOUT      WM_LAYOUT__CreateLayoutEx      (WM_LAYOUT_TYPE Type, int (* pfUpdatePositions) (WM_HLAYOUT, GUI_RECT *), WM_LAYOUT_DIRECTION Dir);
WM_HLAYOUT_ITEM WM_LAYOUT__AddItem             (WM_HLAYOUT hLayout, WM_HMEM hItem, WM_LAYOUT_ITEM_TYPE Type, U8 Stretch, U8 Align);

/*********************************************************************
*
*       API Public
*
**********************************************************************
*/
WM_HLAYOUT      WM_LAYOUT_CreateHBox     (void);
WM_HLAYOUT      WM_LAYOUT_CreateVBox     (void);
WM_HLAYOUT      WM_LAYOUT_CreateGrid     (void);
WM_HLAYOUT      WM_LAYOUT_CreateHBoxFloat(void);
WM_HLAYOUT      WM_LAYOUT_CreateVBoxFloat(void);

int             WM_LAYOUT_SetDirection        (WM_HLAYOUT hLayout, WM_LAYOUT_DIRECTION Dir);
int             WM_LAYOUT_SetMargin           (WM_HLAYOUT hLayout, U16 MarginL, U16 MarginR, U16 MarginT, U16 MarginB);
int             WM_LAYOUT_SetMaxColWidth      (WM_HLAYOUT hLayout, U16 Col, U16 MaxColWidth);
int             WM_LAYOUT_SetMaxRowHeight     (WM_HLAYOUT hLayout, U16 Row, U16 MaxRowHeight);
int             WM_LAYOUT_SetMinColWidth      (WM_HLAYOUT hLayout, U16 Col, U16 MinColWidth);
int             WM_LAYOUT_SetMinRowHeight     (WM_HLAYOUT hLayout, U16 Row, U16 MinRowHeight);
int             WM_LAYOUT_SetSpacing          (WM_HLAYOUT hLayout, U16 Spacing);
int             WM_LAYOUT_SetHSpacing         (WM_HLAYOUT hLayout, U16 SpacingH);
int             WM_LAYOUT_SetVSpacing         (WM_HLAYOUT hLayout, U16 SpacingV);

int             WM_LAYOUT_SetItemAlign        (WM_HLAYOUT_ITEM hItem, U8 Align);
int             WM_LAYOUT_SetItemStretch      (WM_HLAYOUT_ITEM hItem, U8 Stretch);
int             WM_LAYOUT_SetItemSizeMin      (WM_HLAYOUT_ITEM hItem, U16 xSizeMin, U16 ySizeMin);
int             WM_LAYOUT_SetItemSizeMax      (WM_HLAYOUT_ITEM hItem, U16 xSizeMax, U16 ySizeMax);
int             WM_LAYOUT_SetItemHeightMin    (WM_HLAYOUT_ITEM hItem, U16 ySizeMin);
int             WM_LAYOUT_SetItemHeightMax    (WM_HLAYOUT_ITEM hItem, U16 ySizeMax);
int             WM_LAYOUT_SetItemWidthMin     (WM_HLAYOUT_ITEM hItem, U16 xSizeMin);
int             WM_LAYOUT_SetItemWidthMax     (WM_HLAYOUT_ITEM hItem, U16 xSizeMax);

WM_HLAYOUT_ITEM WM_LAYOUT_AddWindow    (WM_HLAYOUT hLayout, WM_HWIN hWin);
WM_HLAYOUT_ITEM WM_LAYOUT_AddWindowEx  (WM_HLAYOUT hLayout, WM_HWIN hWin, U8 Stretch, U8 Align);
WM_HLAYOUT_ITEM WM_LAYOUT_AddWindowCR  (WM_HLAYOUT hLayout, WM_HWIN hWin, U16 Col, U16 Row);
WM_HLAYOUT_ITEM WM_LAYOUT_AddWindowCREx(WM_HLAYOUT hLayout, WM_HWIN hWin, U16 Col, U16 Row, U8 Stretch, U8 Align, U16 ColSpan, U16 RowSpan);

WM_HLAYOUT_ITEM WM_LAYOUT_AddLayout    (WM_HLAYOUT hLayout, WM_HLAYOUT hMem, U8 Stretch);
WM_HLAYOUT_ITEM WM_LAYOUT_AddLayoutCR  (WM_HLAYOUT hLayout, WM_HLAYOUT hMem, U16 Col, U16 Row);
WM_HLAYOUT_ITEM WM_LAYOUT_AddLayoutCREx(WM_HLAYOUT hLayout, WM_HLAYOUT hMem, U16 Col, U16 Row, U8 Stretch, U16 ColSpan, U16 RowSpan);

WM_HLAYOUT_ITEM WM_LAYOUT_AddSpacer  (WM_HLAYOUT hLayout, U16 xSize, U16 ySize);
WM_HLAYOUT_ITEM WM_LAYOUT_AddSpacerEx(WM_HLAYOUT hLayout, U16 xSize, U16 ySize, WM_LAYOUT_POLICY xPolicy, WM_LAYOUT_POLICY yPolicy);

int             WM_LAYOUT_GetRect(WM_HLAYOUT hLayout, GUI_RECT * pRect);

int WM_SetLayout(WM_HWIN hWin, WM_HLAYOUT hLayout);

#endif   // GUI_WINSUPPORT

#if defined(__cplusplus)
}
#endif

#endif  // WM_LAYOUT_H

/*************************** End of file ****************************/
