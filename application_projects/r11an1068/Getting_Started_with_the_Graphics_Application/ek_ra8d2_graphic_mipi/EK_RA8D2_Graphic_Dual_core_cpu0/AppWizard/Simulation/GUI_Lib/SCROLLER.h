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
File        : SCROLLER.h
Purpose     : SCROLLER include
--------------------END-OF-HEADER-------------------------------------
*/

#ifndef SCROLLER_H
#define SCROLLER_H

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
/*********************************************************************
*
*       SCROLLER color indexes
*
*  Description
*    Color indexes for SCROLLER widgets.
*/
#define SCROLLER_CI_ACTIVE     0    // Color for active state (when the scroller is moving or receives touch input).
#define SCROLLER_CI_INACTIVE   1    // Color for inactive state.

/*********************************************************************
*
*       SCROLLER period indexes
*
*  Description
*    Period indexes for SCROLLER widgets.
*/
#define SCROLLER_PI_FADE_IN       0    // The length in ms for fading in, from inactive to active color.
#define SCROLLER_PI_FADE_OUT      1    // The length in ms for fading out, from active color to inactive color.
#define SCROLLER_PI_INACTIVE      2    // Period it takes from releasing the widget until the fade operation is started.
#define SCROLLER_PI_ANIM_SCROLL   3    // Period for animating the changing of the scroll state when the SCROLLER was moved by touch.

/*********************************************************************
*
*       SCROLLER alignment indexes
*
*  Description
*    Alignment indexes for SCROLLER widgets.
*/
#define SCROLLER_AI_WIDGETPOS   0   // The alignment of the SCROLLER widget within its parent widget (left or right, bottom or top).
#define SCROLLER_AI_THUMB       1   // The alignment of the thumb rectangle within the SCROLLER widget.\n
                                    // Allowed values for vertical SCROLLERs:\n
                                    // GUI_ALIGN_LEFT, GUI_ALIGN_HCENTER, GUI_ALIGN_RIGHT\n
                                    // Allowed values for horizontal SCROLLERs:\n
                                    // GUI_ALIGN_TOP, GUI_ALIGN_VCENTER, GUI_ALIGN_BOTTOM

/*********************************************************************
*
*       SCROLLER animation indexes
*
*  Description
*    Animation indexes used for SCROLLER_SetAnimEase().
*/
#define SCROLLER_ANIM_FADE    0     // Animation for fading between the active and inactive state.
#define SCROLLER_ANIM_SCROLL  1     // Animation for animating the scroll state when the SCROLLER was clicked outside of the thumb
                                    // rectangle.

/*********************************************************************
*
*       SCROLLER create flags
*
*  Description
*    Create flags for SCROLLER widgets.
*/
#define SCROLLER_CF_FADING                  (1 << 0)     // Enables automatic fading between the active and inactive state of the SCROLLER.
#define SCROLLER_CF_TOUCH                   (1 << 1)     // Enables touch input for the SCROLLER.
#define SCROLLER_CF_VERTICAL                (1 << 3)     // Creates a vertical SCROLLER.
#define SCROLLER_CF_HORIZONTAL              (0 << 3)     // Creates a horizontal SCROLLER.

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
typedef WM_HMEM SCROLLER_Handle;

/*********************************************************************
*
*       SCROLLER_INTERFACE_API
*
*  Description
*    Interface API between the SCROLLER and its parent widget.
* 
*    This structure is only needed when attaching SCROLLERs to custom
*    widgets or windows.
* 
*    See also the function SCROLLER_SetInterfaceAPI().
*/
typedef struct {
  void (* pfUpdateScrollPos)(WM_HWIN hParent, SCROLLER_Handle hScroller);                                  // Routine for setting the scroll state of the parent widget to the SCROLLER.
  void (* pfScrollerAdded)  (WM_HWIN hParent, SCROLLER_Handle hScroller);                                  // Routine that is called when the parent receives the WM_NOTIFY_PARENT message
                                                                                                           // with a WM_NOTIFICATION_SCROLLER_ADDED notification.
  void (* pfValueChanged)   (WM_HWIN hParent, SCROLLER_Handle hScroller, WM_SCROLL_STATE * pNewState);     // Routine that is called when the parent receives the WM_NOTIFY_PARENT message
                                                                                                           // with a WM_NOTIFICATION_VALUE_CHANGED notification sent by SCROLLER widgets.
                                                                                                           // This routine sets the new scroll state of the SCROLLER to the parent widget.
  void (* pfGetContentRect) (WM_HWIN hParent, GUI_RECT * pRect);                                           // Routine for retrieving the content rectangle of the parent, called when parent
                                                                                                           // receives a WM_GET_CONTENT_RECT message.
} SCROLLER_INTERFACE_API;

/*********************************************************************
*
*       Create functions
*
**********************************************************************
*/
SCROLLER_Handle SCROLLER_CreateAttached(WM_HWIN hParent, int ExFlags);
SCROLLER_Handle SCROLLER_CreateIndirect(const GUI_WIDGET_CREATE_INFO * pCreateInfo, WM_HWIN hWinParent, int x0, int y0, WM_CALLBACK * cb);
SCROLLER_Handle SCROLLER_CreateUser    (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id, int NumExtraBytes);

/*********************************************************************
*
*       Macros
*
**********************************************************************
*/
#define SCROLLER_SetScrollState   WM_SetScrollState
#define SCROLLER_GetScrollState   WM_GetScrollState

/*********************************************************************
*
*       The callback ...
*
* Do not call it directly ! It is only to be used from within an
* overwritten callback.
*/
void SCROLLER_Callback(WM_MESSAGE * pMsg);

/*********************************************************************
*
*       Member functions
*
**********************************************************************
*/
void                 SCROLLER_AttachToWindow  (SCROLLER_Handle hObj, WM_HWIN hNewParent);
void                 SCROLLER_EnableAutoResize(int EnableAutoResize, int Vertical);
void                 SCROLLER_EnableFade      (SCROLLER_Handle hObj, U8 OnOff);
void                 SCROLLER_EnableTouch     (SCROLLER_Handle hObj, U8 OnOff);
void                 SCROLLER_PreventIntersect(int Enable);
//
// Custom management
//
int                  SCROLLER_OwnerDraw       (const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);
int                  SCROLLER_ParentMsgHandler(WM_MESSAGE * pMsg);
void                 SCROLLER_SetInterfaceAPI (SCROLLER_Handle hObj, const SCROLLER_INTERFACE_API * pAPI);
void                 SCROLLER_SetState        (SCROLLER_Handle hObj, U8 Active, U8 NoTimer);
void                 SCROLLER_SetContentRect  (SCROLLER_Handle hObj, const GUI_RECT * pRect);
//
// Properties (getters)
//
U8                   SCROLLER_GetAlign        (SCROLLER_Handle hObj, unsigned int AlignIndex);
I16                  SCROLLER_GetAlignOffset  (SCROLLER_Handle hObj);
GUI_ANIM_GETPOS_FUNC SCROLLER_GetAnimEase     (SCROLLER_Handle hObj, unsigned int AnimIndex);
GUI_COLOR            SCROLLER_GetBkColor      (SCROLLER_Handle hObj, unsigned int ColorIndex);
GUI_COLOR            SCROLLER_GetColor        (SCROLLER_Handle hObj, unsigned int ColorIndex);
int                  SCROLLER_GetPeriod       (SCROLLER_Handle hObj, unsigned int PeriodIndex);
I16                  SCROLLER_GetRadius       (SCROLLER_Handle hObj);
I16                  SCROLLER_GetSize         (SCROLLER_Handle hObj);
I16                  SCROLLER_GetSpacing      (SCROLLER_Handle hObj);
void                 SCROLLER_GetThumbRect    (SCROLLER_Handle hObj, GUI_RECT * pRect);
I16                  SCROLLER_GetThumbSizeMin (SCROLLER_Handle hObj);
int                  SCROLLER_GetUserData     (SCROLLER_Handle hObj, void * pDest, int NumBytes);
//
// Properties (setters)
//
void                 SCROLLER_SetAlign        (SCROLLER_Handle hObj, unsigned int AlignIndex, U8 Align);
void                 SCROLLER_SetAlignOffset  (SCROLLER_Handle hObj, I16 Offset);
void                 SCROLLER_SetAnimEase     (SCROLLER_Handle hObj, unsigned int AnimIndex, GUI_ANIM_GETPOS_FUNC pfEase);
void                 SCROLLER_SetBkColor      (SCROLLER_Handle hObj, unsigned int ColorIndex, GUI_COLOR BkColor);
void                 SCROLLER_SetColor        (SCROLLER_Handle hObj, unsigned int ColorIndex, GUI_COLOR Color);
void                 SCROLLER_SetOwnerDraw    (SCROLLER_Handle hObj, WIDGET_DRAW_ITEM_FUNC * pfDrawItem);
void                 SCROLLER_SetPeriod       (SCROLLER_Handle hObj, unsigned int PeriodIndex, int Period);
void                 SCROLLER_SetRadius       (SCROLLER_Handle hObj, I16 Radius);
void                 SCROLLER_SetSize         (SCROLLER_Handle hObj, I16 Size);
void                 SCROLLER_SetSpacing      (SCROLLER_Handle hObj, I16 Spacing);
void                 SCROLLER_SetThumbSizeMin (SCROLLER_Handle hObj, I16 ThumbSize);
int                  SCROLLER_SetUserData     (SCROLLER_Handle hObj, const void * pSrc, int NumBytes);

/*********************************************************************
*
*       Default values
*
**********************************************************************
*/
//
// Default properties (getters)
//
U8                   SCROLLER_GetDefaultAlign       (unsigned int AlignIndex);
I16                  SCROLLER_GetDefaultAlignOffset (void);
GUI_ANIM_GETPOS_FUNC SCROLLER_GetDefaultAnimEase    (unsigned int AnimIndex);
GUI_COLOR            SCROLLER_GetDefaultBkColor     (unsigned int ColorIndex);
GUI_COLOR            SCROLLER_GetDefaultColor       (unsigned int ColorIndex);
int                  SCROLLER_GetDefaultPeriod      (unsigned int PeriodIndex);
I16                  SCROLLER_GetDefaultRadius      (void);
I16                  SCROLLER_GetDefaultThumbSizeMin(void);
I16                  SCROLLER_GetDefaultSize        (void);
I16                  SCROLLER_GetDefaultSpacing     (void);
//
// Default properties (setters)
//
void                 SCROLLER_SetDefaultAlign       (unsigned int AlignIndex, U8 Align);
void                 SCROLLER_SetDefaultAlignOffset (I16 Offset);
void                 SCROLLER_SetDefaultAnimEase    (unsigned int AnimIndex, GUI_ANIM_GETPOS_FUNC pfEase);
void                 SCROLLER_SetDefaultBkColor     (unsigned int ColorIndex, GUI_COLOR BkColor);
void                 SCROLLER_SetDefaultColor       (unsigned int ColorIndex, GUI_COLOR Color);
void                 SCROLLER_SetDefaultPeriod      (unsigned int PeriodIndex, int Period);
void                 SCROLLER_SetDefaultRadius      (I16 Radius);
void                 SCROLLER_SetDefaultThumbSizeMin(I16 ThumbSize);
void                 SCROLLER_SetDefaultSize        (I16 Size);
void                 SCROLLER_SetDefaultSpacing     (I16 Spacing);

#if defined(__cplusplus)
  }
#endif

#endif  // GUI_WINSUPPORT
#endif  // SCROLLER_H

/*************************** End of file ****************************/
