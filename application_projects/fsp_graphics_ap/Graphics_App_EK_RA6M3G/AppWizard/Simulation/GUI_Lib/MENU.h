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
File        : MENU.h
Purpose     : MENU include
--------------------END-OF-HEADER-------------------------------------
*/

#ifndef MENU_H
#define MENU_H

#include "WM.h"
#include "WIDGET.h"
#include "DIALOG_Type.h"      // Required for Create indirect data structure

#if GUI_WINSUPPORT

#if defined(__cplusplus)
  extern "C" {     // Make sure we have C-declarations in C++ programs
#endif

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define MENU_SKIN_FLEX    MENU_DrawSkinFlex

/*********************************************************************
*
*       Create flags
*/
#define MENU_CF_HORIZONTAL              (0 << 0)
#define MENU_CF_VERTICAL                (1 << 0)
#define MENU_CF_OPEN_ON_POINTEROVER     (1 << 1)  // Normally a menu opens first when clicked on it
#define MENU_CF_CLOSE_ON_SECOND_CLICK   (1 << 2)  // Normally a menu closes only when clicked outside it
#define MENU_CF_HIDE_DISABLED_SEL       (1 << 3)  // Hides the selection when a disabled item is selected

/*********************************************************************
*
*       MENU item flags
*
*  Description
*    Flags used by the MENU_ITEM_DATA structure.
*/
#define MENU_IF_DISABLED                (1 << 0)  // Indicates that item is disabled.
#define MENU_IF_SEPARATOR               (1 << 1)  // Indicates that item is a separator.

/*********************************************************************
*
*       MENU color indexes
*
*  Description
*    Color indexes used by the MENU widget.
*/
#define MENU_CI_ENABLED                 0         // Color of enabled and not selected MENU items.
#define MENU_CI_SELECTED                1         // Color of enabled and selected MENU items.
#define MENU_CI_DISABLED                2         // Color of disabled MENU items.
#define MENU_CI_DISABLED_SEL            3         // Color of disabled and selected MENU items.
#define MENU_CI_ACTIVE_SUBMENU          4         // Color of active submenu items.

/*********************************************************************
*
*       MENU border indexes
*
*  Description
*    Border indexes used by functions to set the border properties of
*    a MENU widget.
*/
#define MENU_BI_LEFT                    0         // Border between item text and left edge of item.
#define MENU_BI_RIGHT                   1         // Border between item text and right edge of item.
#define MENU_BI_TOP                     2         // Border between item text and item top.
#define MENU_BI_BOTTOM                  3         // Border between item text and item bottom.

/*********************************************************************
*
*       MENU message types
*
*  Description
*    Types of messages sent with the \a{MsgType} parameter of the MENU_MSG_DATA structure.
*    A pointer to this structure is sent via the \c{Data.p} pointer of a WM_MENU message.
*/
#define MENU_ON_ITEMSELECT              0         // This message is sent to the owner of a MENU immediately after a MENU item is selected. The \c{ItemId} element of
                                                  // the MENU_MSG_DATA structure contains the Id of the pressed MENU item.
#define MENU_ON_INITMENU                1         // This message is sent to the owner of MENU immediately before the MENU opens. This enables the application to
                                                  // modify the MENU before it is shown.
#define MENU_ON_INITSUBMENU             2         /* Send to owner when selecting a submenu                                                        */
#define MENU_ON_OPEN                    3         /* Internal message of menu widget (only send to submenus)                                       */
#define MENU_ON_CLOSE                   4         /* Internal message of menu widget (only send to submenus)                                       */
#define MENU_IS_MENU                    5         /* Internal message of menu widget. Owner must call WM_DefaultProc() when not handle the message */
#define MENU_ON_ITEMACTIVATE            6         // The owner window of a MENU will receive this message after a MENU item has been highlighted. The message is
                                                  // not sent after highlighting a submenu.
#define MENU_ON_ITEMPRESSED             7         // After pressing a MENU item this message will be sent to the owner window of the widget. It will be sent also for
                                                  // disabled MENU items.

/*********************************************************************
*
*       Skinning property indices
*/
#define MENU_SKINFLEX_PI_ENABLED        0
#define MENU_SKINFLEX_PI_SELECTED       1
#define MENU_SKINFLEX_PI_DISABLED       2
#define MENU_SKINFLEX_PI_DISABLED_SEL   3
#define MENU_SKINFLEX_PI_ACTIVE_SUBMENU 4

/*********************************************************************
*
*       Types
*
**********************************************************************
*/

typedef WM_HMEM MENU_Handle;

typedef struct {
  //
  // Background
  //  
  GUI_COLOR aBkColorH[2];
  GUI_COLOR BkColorV;
  GUI_COLOR FrameColorH;
  GUI_COLOR FrameColorV;
  //
  // Selection
  //
  GUI_COLOR aSelColorH[2];
  GUI_COLOR aSelColorV[2];
  GUI_COLOR FrameColorSelH;
  GUI_COLOR FrameColorSelV;
  //
  // Separator
  //
  GUI_COLOR aSepColorH[2];
  GUI_COLOR aSepColorV[2];
  //
  // Arrow
  //
  GUI_COLOR ArrowColor;
  //
  // Text
  //
  GUI_COLOR TextColor;
} MENU_SKINFLEX_PROPS;

/*********************************************************************
*
*       MENU_MSG_DATA
*
*  Description
*    This structure is used in conjunction with the WM_MENU message,
*    specific to the MENU widget. The \c{Data.p} pointer of the message
*    points to a MENU_MSG_DATA structure.
*/
typedef struct {
  U16 MsgType;              // See \ref{MENU message types}.
  U16 ItemId;               // Id of MENU item.
} MENU_MSG_DATA;

/*********************************************************************
*
*       MENU_ITEM_DATA
*
*  Description
*    This structure serves as a container to set or retrieve information about MENU items.
*/
typedef struct {
  const char  * pText;      // MENU item text.
  U16           Id;         // Id of the MENU item.
  U16           Flags;      // See \ref{MENU item flags}.
  MENU_Handle   hSubmenu;   // If the item represents a submenu this element contains the handle of the submenu.
} MENU_ITEM_DATA;

/*********************************************************************
*
*       Create functions
*
**********************************************************************
*/
MENU_Handle MENU_CreateIndirect(const GUI_WIDGET_CREATE_INFO * pCreateInfo, WM_HWIN hWinParent, int x0, int y0, WM_CALLBACK * cb);
MENU_Handle MENU_CreateEx      (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id);
MENU_Handle MENU_CreateUser    (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id, int NumExtraBytes);

/*********************************************************************
*
*       The callback ...
*
* Do not call it directly ! It is only to be used from within an
* overwritten callback.
*/
void MENU_Callback(WM_MESSAGE * pMsg);

/*********************************************************************
*
*       Individual member functions
*
**********************************************************************
*/
void             MENU_AddItem       (MENU_Handle hObj, const MENU_ITEM_DATA * pItemData);
void             MENU_Attach        (MENU_Handle hObj, WM_HWIN hDestWin, int x, int y, int xSize, int ySize, int Flags);
void             MENU_DeleteItem    (MENU_Handle hObj, U16 ItemId);
void             MENU_DisableItem   (MENU_Handle hObj, U16 ItemId);
void             MENU_EnableItem    (MENU_Handle hObj, U16 ItemId);
GUI_COLOR        MENU_GetBkColor    (MENU_Handle hObj, unsigned ColorIndex);
const GUI_FONT * MENU_GetFont       (MENU_Handle hObj);
void             MENU_GetItem       (MENU_Handle hObj, U16 ItemId, MENU_ITEM_DATA * pItemData);
void             MENU_GetItemText   (MENU_Handle hObj, U16 ItemId, char * pBuffer, unsigned BufferSize);
unsigned         MENU_GetNumItems   (MENU_Handle hObj);
WM_HWIN          MENU_GetOwner      (MENU_Handle hObj);
GUI_COLOR        MENU_GetTextColor  (MENU_Handle hObj, unsigned ColorIndex);
int              MENU_GetUserData   (MENU_Handle hObj, void * pDest, int NumBytes);
void             MENU_InsertItem    (MENU_Handle hObj, U16 ItemId, const MENU_ITEM_DATA * pItemData);
void             MENU_Popup         (MENU_Handle hObj, WM_HWIN hDestWin, int x, int y, int xSize, int ySize, int Flags);
void             MENU_SetBkColor    (MENU_Handle hObj, unsigned ColorIndex, GUI_COLOR Color);
void             MENU_SetBorderSize (MENU_Handle hObj, unsigned BorderIndex, U8 BorderSize);
void             MENU_SetFont       (MENU_Handle hObj, const GUI_FONT * pFont);
void             MENU_SetItem       (MENU_Handle hObj, U16 ItemId, const MENU_ITEM_DATA * pItemData);
void             MENU_SetOwner      (MENU_Handle hObj, WM_HWIN hOwner);
int              MENU_SetSel        (MENU_Handle hObj, int Sel);
void             MENU_SetTextColor  (MENU_Handle hObj, unsigned ColorIndex, GUI_COLOR Color);
int              MENU_SetUserData   (MENU_Handle hObj, const void * pSrc, int NumBytes);

/*********************************************************************
*
*       Managing default values
*
**********************************************************************
*/
GUI_COLOR             MENU_GetDefaultTextColor  (unsigned ColorIndex);
GUI_COLOR             MENU_GetDefaultBkColor    (unsigned ColorIndex);
U8                    MENU_GetDefaultBorderSize (unsigned BorderIndex);
const WIDGET_EFFECT * MENU_GetDefaultEffect     (void);
const GUI_FONT      * MENU_GetDefaultFont       (void);
void                  MENU_SetDefaultTextColor  (unsigned ColorIndex, GUI_COLOR Color);
void                  MENU_SetDefaultBkColor    (unsigned ColorIndex, GUI_COLOR Color);
void                  MENU_SetDefaultBorderSize (unsigned BorderIndex, U8 BorderSize);
void                  MENU_SetDefaultEffect     (const WIDGET_EFFECT * pEffect);
void                  MENU_SetDefaultFont       (const GUI_FONT * pFont);

/*********************************************************************
*
*       Member functions: Skinning
*
**********************************************************************
*/
int                     MENU_DrawSkinFlex         (const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);
void                    MENU_GetSkinFlexProps     (MENU_SKINFLEX_PROPS * pProps, int Index);
WIDGET_DRAW_ITEM_FUNC * MENU_SetDefaultSkin       (WIDGET_DRAW_ITEM_FUNC * pfDrawSkin);
void                    MENU_SetDefaultSkinClassic(void);
void                    MENU_SetSkinClassic       (MENU_Handle hObj);
void                    MENU_SetSkin              (MENU_Handle hObj, WIDGET_DRAW_ITEM_FUNC * pfDrawSkin);
void                    MENU_SetSkinFlexProps     (const MENU_SKINFLEX_PROPS * pProps, int Index);
void                    MENU_SkinEnableArrow      (MENU_Handle hObj, int OnOff);

#if defined(__cplusplus)
  }
#endif

#endif  // GUI_WINSUPPORT
#endif  // MENU_H

/*************************** End of file ****************************/
