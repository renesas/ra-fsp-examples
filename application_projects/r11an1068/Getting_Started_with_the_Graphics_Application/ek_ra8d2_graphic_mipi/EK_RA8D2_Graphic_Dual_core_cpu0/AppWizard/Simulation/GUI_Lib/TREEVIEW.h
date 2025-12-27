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
File        : TREEVIEW.h
Purpose     : TREEVIEW include
--------------------END-OF-HEADER-------------------------------------
*/

#ifndef TREEVIEW_H
#define TREEVIEW_H

#include "WM.h"
#include "DIALOG_Type.h"      /* Req. for Create indirect data structure */
#include "WIDGET.h"

#if GUI_WINSUPPORT

#if defined(__cplusplus)
  extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
/************************************************************
*
*       TREEVIEW create flags
*
*  Description
*    Create flags used by the TREEVIEW widget. These flags are used for the \a{ExFlags} parameter
*    of TREEVIEW_CreateEx(). These values can be OR-combined.
*/
#define TREEVIEW_CF_HIDELINES       (1 << 0)   // Joining lines are not displayed.
#define TREEVIEW_CF_ROWSELECT       (1 << 1)   // Activates row selection mode.
#define TREEVIEW_CF_AUTOSCROLLBAR_H (1 << 2)   // Enables the use of an automatic horizontal scroll bar.
#define TREEVIEW_CF_AUTOSCROLLBAR_V (1 << 3)   // Enables the use of an automatic vertical scroll bar.
/* Status flags */
#define TREEVIEW_SF_HIDELINES       TREEVIEW_CF_HIDELINES
#define TREEVIEW_SF_ROWSELECT       TREEVIEW_CF_ROWSELECT
#define TREEVIEW_SF_AUTOSCROLLBAR_H TREEVIEW_CF_AUTOSCROLLBAR_H
#define TREEVIEW_SF_AUTOSCROLLBAR_V TREEVIEW_CF_AUTOSCROLLBAR_V

/************************************************************
*
*       TREEVIEW bitmap indexes
*
*  Description
*    Bitmap indexes used by the TREEVIEW widget. Refer to TREEVIEW_SetImage().
*/
#define TREEVIEW_BI_CLOSED          0          // Image of closed nodes.
#define TREEVIEW_BI_OPEN            1          // Image of open nodes.
#define TREEVIEW_BI_LEAF            2          // Image of leaf.
#define TREEVIEW_BI_PLUS            3          // Plus sign of closed nodes.
#define TREEVIEW_BI_MINUS           4          // Minus sign of open nodes.
#define TREEVIEW_BI_PM              5          // Used by TREEVIEW_SetBitmapOffset() for setting the offset of the plus/minus bitmaps.

/************************************************************
*
*       TREEVIEW color indexes
*
*  Description
*    Color indexes used by the TREEVIEW widget.
*/
#define TREEVIEW_CI_UNSEL           0          // Color of unselected element.
#define TREEVIEW_CI_SEL             1          // Color of selected element.
#define TREEVIEW_CI_DISABLED        2          // Color of disabled element.

/************************************************************
*
*       TREEVIEW position flags (insert)
*
*  Description
*    These flags are used to specify a position when creating and inserting
*    a new item into the TREEVIEW widget.
*/
#define TREEVIEW_INSERT_ABOVE       0          // Attaches the item above the given position at the same indent level as the given position.
#define TREEVIEW_INSERT_BELOW       1          // Attaches the item below the given position at the same indent level as the given position.
#define TREEVIEW_INSERT_FIRST_CHILD 2          // Attaches the item below the given position by indenting it. The given position needs to be a node level.

/************************************************************
*
*       TREEVIEW position flags (get)
*
*  Description
*    These flags are used to specify a position when retrieving an item
*    of the TREEVIEW widget using the routine TREEVIEW_GetItem().
*/
#define TREEVIEW_GET_FIRST          0          // Returns the first item of the TREEVIEW widget. Parameter hItem is not required and can be 0.
#define TREEVIEW_GET_LAST           1          // Returns the last item of the TREEVIEW widget. Parameter hItem is not required and can be 0.
#define TREEVIEW_GET_NEXT_SIBLING   2          // Returns the next child item of the parent node of hItem.
#define TREEVIEW_GET_PREV_SIBLING   3          // Returns the previous child item of the parent node of hItem.
#define TREEVIEW_GET_FIRST_CHILD    4          // Returns the first child of the given node.
#define TREEVIEW_GET_PARENT         5          // Returns the parent node of the given item.

/************************************************************
*
*       TREEVIEW item flags
*
*  Description
*    Flags that define the item type of a newly created TREEVIEW item.
*/
#define TREEVIEW_ITEM_TYPE_LEAF     (0 << 0)   // Used to create a leaf.
#define TREEVIEW_ITEM_TYPE_NODE     (1 << 0)   // Used to create a node.

/************************************************************
*
*       TREEVIEW selection modes
*
*  Description
*    Flags that are used to define the selection mode of a TREEVIEW widget.
*    Refer to TREEVIEW_SetSelMode() for more information.
*/
#define TREEVIEW_SELMODE_ROW        1          // Activates row selection mode.
#define TREEVIEW_SELMODE_TEXT       0          // Activates text selection mode.

/************************************************************
*
*       Types
*
*************************************************************
*/
typedef WM_HMEM TREEVIEW_Handle;
typedef WM_HMEM TREEVIEW_ITEM_Handle;

/************************************************************
*
*       TREEVIEW_ITEM_INFO
*
*  Description
*    Structure that contains information about a node in a TREEVIEW widget.
*/
typedef struct {
  int IsNode;           // 1 if item is a node, 0 if not.
  int IsExpanded;       // 1 if item (node) is open, 0 if closed.
  int HasLines;         // 1 if joining lines are visible, 0 if not.
  int HasRowSelect;     // 1 if row selection is active, 0 if not.
  int Level;            // Indentation level of item.
} TREEVIEW_ITEM_INFO;

typedef struct {
  GUI_COLOR ColorBk;
  GUI_COLOR ColorText;
  GUI_COLOR ColorTextBk;
  GUI_COLOR ColorLines;
  GUI_RECT rText;
  TREEVIEW_ITEM_Handle hItem;
  const GUI_FONT * pFont;
  char * pText;
  U8 NumLines;
  I16 ax0[3];
  I16 ay0[3];
  I16 ax1[3];
  I16 ay1[3];
  U8 NumConnectors;
  I16 axc[16];
  const GUI_BITMAP * pBmPM;
  const GUI_BITMAP * pBmOCL;
  I16 xPosPM, xPosOCL;
  U8 IndexPM;
  U8 IndexOCL;
} TREEVIEW_ITEM_DRAW_INFO;

/*********************************************************************
*
*       Create functions
*
**********************************************************************
*/
TREEVIEW_Handle      TREEVIEW_CreateEx      (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id);
TREEVIEW_Handle      TREEVIEW_CreateUser    (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id, int NumExtraBytes);
TREEVIEW_Handle      TREEVIEW_CreateIndirect(const GUI_WIDGET_CREATE_INFO * pCreateInfo, WM_HWIN hWinParent, int x0, int y0, WM_CALLBACK * cb);

/*********************************************************************
*
*       The callback ...
*
* Do not call it directly ! It is only to be used from within an
* overwritten callback.
*/
void TREEVIEW_Callback(WM_MESSAGE * pMsg);

/*********************************************************************
*
*       Common functions
*
**********************************************************************
*/
int                  TREEVIEW_AttachItem     (TREEVIEW_Handle hObj, TREEVIEW_ITEM_Handle hItem, TREEVIEW_ITEM_Handle hItemAt, int Position);
void                 TREEVIEW_DecSel         (TREEVIEW_Handle hObj);
TREEVIEW_ITEM_Handle TREEVIEW_GetItem        (TREEVIEW_Handle hObj, TREEVIEW_ITEM_Handle hItem, int Flags);
TREEVIEW_ITEM_Handle TREEVIEW_GetSel         (TREEVIEW_Handle hObj);
int                  TREEVIEW_GetUserData    (TREEVIEW_Handle hObj, void * pDest, int NumBytes);
void                 TREEVIEW_IncSel         (TREEVIEW_Handle hObj);
TREEVIEW_ITEM_Handle TREEVIEW_InsertItem     (TREEVIEW_Handle hObj, int IsNode, TREEVIEW_ITEM_Handle hItemPrev, int Position, const char * s);
int                  TREEVIEW_OwnerDraw      (const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);
void                 TREEVIEW_ScrollToSel    (TREEVIEW_Handle hObj);
void                 TREEVIEW_SetAutoScrollH (TREEVIEW_Handle hObj, int State);
void                 TREEVIEW_SetAutoScrollV (TREEVIEW_Handle hObj, int State);
void                 TREEVIEW_SetBitmapOffset(TREEVIEW_Handle hObj, int Index, int xOff, int yOff);
void                 TREEVIEW_SetBkColor     (TREEVIEW_Handle hObj, int Index, GUI_COLOR Color);
void                 TREEVIEW_SetFont        (TREEVIEW_Handle hObj, const GUI_FONT * pFont);
void                 TREEVIEW_SetHasLines    (TREEVIEW_Handle hObj, int State);
void                 TREEVIEW_SetImage       (TREEVIEW_Handle hObj, int Index, const GUI_BITMAP * pBitmap);
int                  TREEVIEW_SetIndent      (TREEVIEW_Handle hObj, int Indent);
void                 TREEVIEW_SetLineColor   (TREEVIEW_Handle hObj, int Index, GUI_COLOR Color);
void                 TREEVIEW_SetOwnerDraw   (TREEVIEW_Handle hObj, WIDGET_DRAW_ITEM_FUNC * pfDrawItem);
void                 TREEVIEW_SetSel         (TREEVIEW_Handle hObj, TREEVIEW_ITEM_Handle hItem);
void                 TREEVIEW_SetSelMode     (TREEVIEW_Handle hObj, int Mode);
void                 TREEVIEW_SetTextColor   (TREEVIEW_Handle hObj, int Index, GUI_COLOR Color);
int                  TREEVIEW_SetTextIndent  (TREEVIEW_Handle hObj, int TextIndent);
int                  TREEVIEW_SetUserData    (TREEVIEW_Handle hObj, const void * pSrc, int NumBytes);


/*********************************************************************
*
*       Treeview item related functions
*
**********************************************************************
*/
void                 TREEVIEW_ITEM_Collapse   (TREEVIEW_ITEM_Handle hItem);
void                 TREEVIEW_ITEM_CollapseAll(TREEVIEW_ITEM_Handle hItem);
TREEVIEW_ITEM_Handle TREEVIEW_ITEM_Create     (int IsNode, const char * s, U32 UserData);
void                 TREEVIEW_ITEM_Delete     (TREEVIEW_ITEM_Handle hItem);
void                 TREEVIEW_ITEM_Detach     (TREEVIEW_ITEM_Handle hItem);
void                 TREEVIEW_ITEM_Expand     (TREEVIEW_ITEM_Handle hItem);
void                 TREEVIEW_ITEM_ExpandAll  (TREEVIEW_ITEM_Handle hItem);
void                 TREEVIEW_ITEM_GetInfo    (TREEVIEW_ITEM_Handle hItem, TREEVIEW_ITEM_INFO * pInfo);
void                 TREEVIEW_ITEM_GetText    (TREEVIEW_ITEM_Handle hItem, U8 * pBuffer, int MaxNumBytes);
U32                  TREEVIEW_ITEM_GetUserData(TREEVIEW_ITEM_Handle hItem);
void                 TREEVIEW_ITEM_SetImage   (TREEVIEW_ITEM_Handle hItem, int Index, const GUI_BITMAP * pBitmap);
TREEVIEW_ITEM_Handle TREEVIEW_ITEM_SetText    (TREEVIEW_ITEM_Handle hItem, const char * s);
void                 TREEVIEW_ITEM_SetUserData(TREEVIEW_ITEM_Handle hItem, U32 UserData);

/*********************************************************************
*
*       Managing default values
*
**********************************************************************
*/
GUI_COLOR        TREEVIEW_GetDefaultBkColor  (int Index);
const GUI_FONT * TREEVIEW_GetDefaultFont     (void);
GUI_COLOR        TREEVIEW_GetDefaultLineColor(int Index);
GUI_COLOR        TREEVIEW_GetDefaultTextColor(int Index);
void             TREEVIEW_SetDefaultBkColor  (int Index, GUI_COLOR Color);
void             TREEVIEW_SetDefaultFont     (const GUI_FONT * pFont);
void             TREEVIEW_SetDefaultLineColor(int Index, GUI_COLOR Color);
void             TREEVIEW_SetDefaultTextColor(int Index, GUI_COLOR Color);

#if defined(__cplusplus)
  }
#endif

#endif  // GUI_WINSUPPORT
#endif  // TREEVIEW_H

/*************************** End of file ****************************/
