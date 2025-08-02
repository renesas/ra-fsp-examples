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
File        : SCROLLER_Private.h
Purpose     : SCROLLER internal header file
---------------------------END-OF-HEADER------------------------------
*/

#ifndef SCROLLER_PRIVATE_H
#define SCROLLER_PRIVATE_H

#include "GUI_Private.h"
#include "WM_Intern.h"
#include "SCROLLER.h"
#include "WIDGET.h"
#include "GUI_Debug.h"

#if GUI_WINSUPPORT

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
//
// Status flags
//
#define SCROLLER_SF_FADING                      SCROLLER_CF_FADING       // Enables fading in and out of the SCROLLER.
#define SCROLLER_SF_TOUCH                       SCROLLER_CF_TOUCH        // Enables touch input for the SCROLLER.
#define SCROLLER_SF_ACTIVE                      (1 <<  2)                // SCROLLER is drawn as active (pressed or swiped)
#define SCROLLER_SF_VERTICAL                    SCROLLER_CF_VERTICAL     // SCROLLER is vertical (keep same as WIDGET_STATE_VERTICAL).
#define SCROLLER_SF_CLICKED                     (1 <<  4)                // Clicked in SCROLLER area
#define SCROLLER_SF_PID                         (1 <<  5)                // PID is active (e.g. through motion)
#define SCROLLER_SF_PRESSED_INSIDE_THUMB        (1 <<  6)                // PID is pressed inside thumb (thumb is being dragged)
#define SCROLLER_SF_MOTION_RUNNING              (1 <<  7)                // Guard to ensure the active/inactive fading is only done once
#define SCROLLER_SF_MOTION_NOT_FINISHED         (1 <<  8)                // If PID was released but motion is still running
#define SCROLLER_SF_SCROLL_ANIM_RUNNING         (1 <<  9)                // While scrolling animation is running
#define SCROLLER_SF_TIMER_RESCHEDULE            (1 << 10)                // If inactive timer has run out, but fading was not done, so the timer had to be rescheduled.
#define SCROLLER_SF_START_TIMER_ON_ANIM_END     (1 << 11)                // If flag is set, the inactive timer is immediately started when the fading animation has finished.
#define SCROLLER_SF_NO_INACTIVE_TIMER           (1 << 12)                // Inactive timer will not be started.
#define SCROLLER_SF_OVERRIDE_RECT               (1 << 13)                // Override the content rectangle to determine the SCROLLER size.
//
// Private messages
//
#define SCROLLER_MSG_ATTACHED                   (WM_USER + 0x123)       // Message sent to the parent when a SCROLLER widget has been attached.

/*********************************************************************
*
*       Private config defaults
*
**********************************************************************
*/
#ifndef   SCROLLER_COLOR_ACTIVE_DEFAULT
  #define SCROLLER_COLOR_ACTIVE_DEFAULT          GUI_MAKE_COLOR(0x50606060)
#endif

#ifndef   SCROLLER_COLOR_INACTIVE_DEFAULT
  #define SCROLLER_COLOR_INACTIVE_DEFAULT        GUI_MAKE_COLOR(0x50AAAAAA)
#endif

#ifndef   SCROLLER_BKCOLOR_ACTIVE_DEFAULT
  #define SCROLLER_BKCOLOR_ACTIVE_DEFAULT        GUI_TRANSPARENT
#endif

#ifndef   SCROLLER_BKCOLOR_INACTIVE_DEFAULT
  #define SCROLLER_BKCOLOR_INACTIVE_DEFAULT      GUI_TRANSPARENT
#endif

#ifndef   SCROLLER_RADIUS_DEFAULT
  #define SCROLLER_RADIUS_DEFAULT                3
#endif

#ifndef   SCROLLER_SIZE_DEFAULT
  #define SCROLLER_SIZE_DEFAULT                  6
#endif

#ifndef   SCROLLER_SPACING_DEFAULT
  #define SCROLLER_SPACING_DEFAULT               24
#endif

#ifndef   SCROLLER_FADE_IN_PERIOD_DEFAULT
  #define SCROLLER_FADE_IN_PERIOD_DEFAULT        200
#endif

#ifndef   SCROLLER_FADE_OUT_PERIOD_DEFAULT
  #define SCROLLER_FADE_OUT_PERIOD_DEFAULT       200
#endif

#ifndef   SCROLLER_INACTIVE_PERIOD_DEFAULT
  #define SCROLLER_INACTIVE_PERIOD_DEFAULT       200
#endif

#ifndef   SCROLLER_ANIM_PERIOD_DEFAULT
  #define SCROLLER_ANIM_PERIOD_DEFAULT           150
#endif

#ifndef   SCROLLER_ANIM_EASE_FADE_DEFAULT
  #define SCROLLER_ANIM_EASE_FADE_DEFAULT        ANIM_ACCELDECEL
#endif

#ifndef   SCROLLER_ANIM_EASE_SCROLL_DEFAULT
  #define SCROLLER_ANIM_EASE_SCROLL_DEFAULT      ANIM_ACCELDECEL
#endif

#ifndef   SCROLLER_ALIGNMENT_WIDGETPOS_DEFAULT
  #define SCROLLER_ALIGNMENT_WIDGETPOS_DEFAULT   (GUI_ALIGN_RIGHT   | GUI_ALIGN_BOTTOM)
#endif

#ifndef   SCROLLER_ALIGNMENT_THUMB_DEFAULT
  #define SCROLLER_ALIGNMENT_THUMB_DEFAULT       (GUI_ALIGN_RIGHT   | GUI_ALIGN_BOTTOM)
#endif

#ifndef   SCROLLER_ALIGNMENT_OFFSET_DEFAULT
  #define SCROLLER_ALIGNMENT_OFFSET_DEFAULT     -3
#endif

#ifndef   SCROLLER_THUMB_SIZE_MIN_DEFAULT
  #define SCROLLER_THUMB_SIZE_MIN_DEFAULT        10
#endif

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
/*********************************************************************
*
*       SCROLLER_ANIM_DATA
* 
*  Description
*    Data for an animation.
*/
typedef struct {
  GUI_ANIM_HANDLE hAnim;
  int             Start;
  int             End;
  I16             Pos;
} SCROLLER_ANIM_DATA;

/*********************************************************************
*
*       ANIM_SCROLL_CONTEXT
* 
*  Description
*    Context to be used during scrolling animation.
*/
typedef struct {
  SCROLLER_Handle hScroller;
  int             Mul;
} ANIM_SCROLL_CONTEXT;

/*********************************************************************
*
*       SCROLLER_WIDGET_API
* 
*  Description
*    API functions used by the parent widget the SCROLLER is attached to.
*/
typedef struct {
  const WIDGET_SCROLLSTATE_API * pConvAPI;       // API for scrollstate conversion. Optional, can be NULL.
  const SCROLLER_INTERFACE_API * pInterfaceAPI;  // Interface between parent widget and SCROLLER. Must be set.
} SCROLLER_WIDGET_API;

/*********************************************************************
*
*       SCROLLER_CONTROL_API
* 
*  Description
*    API that widget uses to access SCROLLER routines and control the SCROLLER.
*    This API is used so that SCROLLER routines do have to be referenced by widgets.
*/
typedef struct {
  //
  // General functions
  //
  int             (* pfParentMsgHandler)   (WM_MESSAGE * pMsg);
  void            (* pfAttachToWindow)     (SCROLLER_Handle hScroller, WM_HWIN hNewParent);
  void            (* pfSetActive)          (WM_HWIN hParent, U8 Vertical);
  void            (* pfResizeScrollers)    (WM_HWIN hParent);
  void            (* pfHideScroller)       (SCROLLER_Handle hScroller);
  //
  // Vertical scrollstate conversion
  //
  void            (* pfLinewiseToPixelwise)(WM_HWIN hParent, WM_SCROLL_STATE * pVState);
  void            (* pfPixelwiseToLinewise)(WM_HWIN hParent, WM_SCROLL_STATE * pVState);
} SCROLLER_CONTROL_API;

/*********************************************************************
*
*       SCROLLER_API_STRUCT
* 
*  Description
*    Struct with SCROLLER-side API and parent-side API for exchanging
*    function pointers upon initialization.
*/
typedef struct {
  SCROLLER_WIDGET_API        * pWidgetAPI;
  const SCROLLER_CONTROL_API * pControlAPI;
} SCROLLER_API_STRUCT;

/*********************************************************************
*
*       Object definition
*
**********************************************************************
*/
typedef struct SCROLLER_Obj SCROLLER_Obj;

typedef struct {
  GUI_COLOR            aColor     [2];
  GUI_COLOR            aBkColor   [2];
  int                  aPeriod    [4];
  GUI_ANIM_GETPOS_FUNC apfAnimEase[2];
  U8                   aAlign     [2];
  I16                  AlignOffset;
  I16                  Size;
  I16                  Spacing;
  I16                  Radius;
  I16                  ThumbSizeMin;
} SCROLLER_PROPS;

struct SCROLLER_Obj {
  WIDGET                   Widget;
  SCROLLER_PROPS           Props;
  WIDGET_DRAW_ITEM_FUNC  * pfOwnerDraw;
  int                      NumItems;          // In pixels for H and V!
  int                      v;                 // In pixels for H and V!
  int                      PageSize;          // In pixels for H and V!
  int                      Overlap;           // Overlapping distance in px (cached from parent widget)
  GUI_TIMER_HANDLE         hTimerInactive;
  GUI_HMEM                 hTimerMsg;         // Copy of GUI_TIMER_MESSAGE if inactive timer needs to be rescheduled.
  SCROLLER_ANIM_DATA       AnimFade;          // Animation handles and data for fading animation
  SCROLLER_ANIM_DATA       AnimScroll;        // Animation handles and data for scrolling animation (when scroller is moved by touching)
  GUI_POINT                TouchPos;
  SCROLLER_WIDGET_API      WidgetAPI;
  GUI_HMEM                 hCustomRect;       // Copy of rectangle set with SCROLLER_SetContentRect()
  I16                      ClientRectOffset;  // Offset in px that is subtracted from the client rectangle during thumb rectangle calculation.
  U16                      Flags;
};

/*********************************************************************
*
*       Private macros
*
**********************************************************************
*/
#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  #define SCROLLER_INIT_ID(p) (p->Widget.DebugId = WIDGET_TYPE_SCROLLER)
#else
  #define SCROLLER_INIT_ID(p)
#endif

#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  SCROLLER_Obj * SCROLLER_LockH(SCROLLER_Handle h);
  #define SCROLLER_LOCK_H(h)   SCROLLER_LockH(h)
#else
  #define SCROLLER_LOCK_H(h)   (SCROLLER_Obj *)WM_LOCK_H(h)
#endif

/*********************************************************************
*
*       Private data
*
**********************************************************************
*/
extern SCROLLER_PROPS SCROLLER__DefaultProps;

#endif        /* GUI_WINSUPPORT */
#endif        /* Avoid multiple inclusion */

/*************************** End of file ****************************/
