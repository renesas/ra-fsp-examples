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
File        : DROPDOWN.h
Purpose     : Multiple choice object include
--------------------END-OF-HEADER-------------------------------------
*/

#ifndef DROPDOWN_H
#define DROPDOWN_H

#include "WM.h"
#include "DIALOG_Type.h"      /* Req. for Create indirect data structure */
#include "LISTBOX.h"

#if GUI_WINSUPPORT

#if defined(__cplusplus)
  extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/************************************************************
*
*       DROPDOWN create flags
*
*  Description
*    These flags can be used when creating a DROPDOWN widget via
*    DROPDOWN_CreateEx(). 0 can be specified for the \a{ExFlags} parameter,
*    if no flags should be used.
*/
#define DROPDOWN_CF_AUTOSCROLLBAR   (1 << 0)    // Enable automatic use of a scroll bar. For details, refer to DROPDOWN_SetAutoScroll().
#define DROPDOWN_CF_UP              (1 << 1)    // Creates a DROPDOWN widget which opens the dropdown list above the widget. This flag is useful if
                                                // the space below the widget is not sufficient for the dropdown list.
#define DROPDOWN_CF_MOTION          (1 << 2)    // Enables motion support on the vertical axis.

/*********************************************************************
*
*       DROPDOWN color indexes
*
*  Description
*    Color indexes for DROPDOWN widget.
*/
#define DROPDOWN_CI_UNSEL    0    // Unselected element.
#define DROPDOWN_CI_SEL      1    // Selected element, without focus.
#define DROPDOWN_CI_SELFOCUS 2    // Selected element, with focus.
/* not documented */
#define DROPDOWN_CI_ARROW    0
#define DROPDOWN_CI_BUTTON   1

/*********************************************************************
*
*       Skinning property indices
*/
#define DROPDOWN_SKINFLEX_PI_EXPANDED 0
#define DROPDOWN_SKINFLEX_PI_FOCUSED  1
#define DROPDOWN_SKINFLEX_PI_ENABLED  2
#define DROPDOWN_SKINFLEX_PI_DISABLED 3

/*********************************************************************
*
*       DROPDOWN notification codes
*
*  Description
*    Notifications sent by DROPDOWN widget to its parent widget through
*    a WM_NOTIFY_PARENT message.
*/
#define DROPDOWN_NOTIFICATION_EXPANDED   (WM_NOTIFICATION_WIDGET + 0)     // Sent when the list of the DROPDOWN has been expanded.
#define DROPDOWN_NOTIFICATION_COLLAPSED  (WM_NOTIFICATION_WIDGET + 1)     // Sent when the list of the DROPDOWN has been collapsed.

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
typedef WM_HMEM DROPDOWN_Handle;

typedef struct {
  GUI_COLOR aColorFrame[3];
  GUI_COLOR aColorUpper[2];
  GUI_COLOR aColorLower[2];
  GUI_COLOR ColorArrow;
  GUI_COLOR ColorText;
  GUI_COLOR ColorSep;
  int Radius;
} DROPDOWN_SKINFLEX_PROPS;

/*********************************************************************
*
*       Create functions
*
**********************************************************************
*/
DROPDOWN_Handle DROPDOWN_Create        (WM_HWIN hWinParent, int x0, int y0, int xSize, int ySize, int Flags);
DROPDOWN_Handle DROPDOWN_CreateEx      (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id);
DROPDOWN_Handle DROPDOWN_CreateUser    (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id, int NumExtraBytes);
DROPDOWN_Handle DROPDOWN_CreateIndirect(const GUI_WIDGET_CREATE_INFO* pCreateInfo, WM_HWIN hWinParent, int x0, int y0, WM_CALLBACK* cb);

/*********************************************************************
*
*       The callback ...
*
* Do not call it directly ! It is only to be used from within an
* overwritten callback.
*/
void DROPDOWN_Callback(WM_MESSAGE * pMsg);

/*********************************************************************
*
*       Member functions
*
**********************************************************************
*/
void             DROPDOWN_AddKey           (DROPDOWN_Handle hObj, int Key);
void             DROPDOWN_AddString        (DROPDOWN_Handle hObj, const char* s);
void             DROPDOWN_Clear            (DROPDOWN_Handle hObj);
void             DROPDOWN_Collapse         (DROPDOWN_Handle hObj);
void             DROPDOWN_DecSel           (DROPDOWN_Handle hObj);
void             DROPDOWN_DecSelExp        (DROPDOWN_Handle hObj);
void             DROPDOWN_DeleteItem       (DROPDOWN_Handle hObj, unsigned int Index);
void             DROPDOWN_EnableMotion     (DROPDOWN_Handle hObj, int Flags);
void             DROPDOWN_Expand           (DROPDOWN_Handle hObj);
GUI_COLOR        DROPDOWN_GetBkColor       (DROPDOWN_Handle hObj, unsigned int Index);
GUI_COLOR        DROPDOWN_GetColor         (DROPDOWN_Handle hObj, unsigned int Index);
const GUI_FONT * DROPDOWN_GetFont          (DROPDOWN_Handle hObj);
unsigned         DROPDOWN_GetItemDisabled  (DROPDOWN_Handle hObj, unsigned Index);
unsigned         DROPDOWN_GetItemSpacing   (DROPDOWN_Handle hObj);
int              DROPDOWN_GetItemText      (DROPDOWN_Handle hObj, unsigned Index, char * pBuffer, int MaxSize);
LISTBOX_Handle   DROPDOWN_GetListbox       (DROPDOWN_Handle hObj);
int              DROPDOWN_GetNumItems      (DROPDOWN_Handle hObj);
int              DROPDOWN_GetSel           (DROPDOWN_Handle hObj);
int              DROPDOWN_GetSelExp        (DROPDOWN_Handle hObj);
GUI_COLOR        DROPDOWN_GetTextColor     (DROPDOWN_Handle hObj, unsigned int Index);
U8               DROPDOWN_GetUpMode        (DROPDOWN_Handle hObj);
int              DROPDOWN_GetUserData      (DROPDOWN_Handle hObj, void * pDest, int NumBytes);
void             DROPDOWN_IncSel           (DROPDOWN_Handle hObj);
void             DROPDOWN_IncSelExp        (DROPDOWN_Handle hObj);
void             DROPDOWN_InsertString     (DROPDOWN_Handle hObj, const char* s, unsigned int Index);
void             DROPDOWN_SetAutoScroll    (DROPDOWN_Handle hObj, int OnOff);
void             DROPDOWN_SetBkColor       (DROPDOWN_Handle hObj, unsigned int Index, GUI_COLOR Color);
void             DROPDOWN_SetColor         (DROPDOWN_Handle hObj, unsigned int Index, GUI_COLOR Color);
void             DROPDOWN_SetFont          (DROPDOWN_Handle hObj, const GUI_FONT * pFont);
void             DROPDOWN_SetItemDisabled  (DROPDOWN_Handle hObj, unsigned Index, int OnOff);
void             DROPDOWN_SetItemSpacing   (DROPDOWN_Handle hObj, unsigned Value);
int              DROPDOWN_SetListHeight    (DROPDOWN_Handle hObj, unsigned Height);
void             DROPDOWN_SetScrollbarColor(DROPDOWN_Handle hObj, unsigned Index, GUI_COLOR Color);
void             DROPDOWN_SetScrollbarWidth(DROPDOWN_Handle hObj, unsigned Width);
void             DROPDOWN_SetSel           (DROPDOWN_Handle hObj, int Sel);
void             DROPDOWN_SetSelExp        (DROPDOWN_Handle hObj, int Sel);
void             DROPDOWN_SetString        (DROPDOWN_Handle hObj, const char* s, unsigned int Index);
void             DROPDOWN_SetText          (DROPDOWN_Handle hObj, const GUI_ConstString * ppText);
void             DROPDOWN_SetTextAlign     (DROPDOWN_Handle hObj, int Align);
void             DROPDOWN_SetTextColor     (DROPDOWN_Handle hObj, unsigned int Index, GUI_COLOR Color);
void             DROPDOWN_SetTextHeight    (DROPDOWN_Handle hObj, unsigned TextHeight);
int              DROPDOWN_SetUpMode        (DROPDOWN_Handle hObj, int OnOff);
int              DROPDOWN_SetUserData      (DROPDOWN_Handle hObj, const void * pSrc, int NumBytes);

/*********************************************************************
*
*       Member functions: Skinning
*
**********************************************************************
*/
void DROPDOWN_GetSkinFlexProps     (DROPDOWN_SKINFLEX_PROPS * pProps, int Index);
void DROPDOWN_SetSkinClassic       (DROPDOWN_Handle hObj);
void DROPDOWN_SetSkin              (DROPDOWN_Handle hObj, WIDGET_DRAW_ITEM_FUNC * pfDrawSkin);
int  DROPDOWN_DrawSkinFlex         (const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);
void DROPDOWN_SetSkinFlexProps     (const DROPDOWN_SKINFLEX_PROPS * pProps, int Index);
void DROPDOWN_SetDefaultSkinClassic(void);
WIDGET_DRAW_ITEM_FUNC * DROPDOWN_SetDefaultSkin(WIDGET_DRAW_ITEM_FUNC * pfDrawSkin);

#define DROPDOWN_SKIN_FLEX    DROPDOWN_DrawSkinFlex

/*********************************************************************
*
*       Managing default values
*
**********************************************************************
*/
GUI_COLOR        DROPDOWN_GetDefaultBkColor       (int Index);
GUI_COLOR        DROPDOWN_GetDefaultColor         (int Index);
const GUI_FONT * DROPDOWN_GetDefaultFont          (void);
GUI_COLOR        DROPDOWN_GetDefaultScrollbarColor(int Index);
void             DROPDOWN_SetDefaultFont          (const GUI_FONT * pFont);
GUI_COLOR        DROPDOWN_SetDefaultBkColor       (int Index, GUI_COLOR Color);
GUI_COLOR        DROPDOWN_SetDefaultColor         (int Index, GUI_COLOR Color);
GUI_COLOR        DROPDOWN_SetDefaultScrollbarColor(int Index, GUI_COLOR Color);

#if defined(__cplusplus)
  }
#endif

#endif  // GUI_WINSUPPORT
#endif  // DROPDOWN_H

/*************************** End of file ****************************/
