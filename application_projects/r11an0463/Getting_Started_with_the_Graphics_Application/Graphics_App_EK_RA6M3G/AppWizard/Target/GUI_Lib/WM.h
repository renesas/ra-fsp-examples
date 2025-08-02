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
File        : WM.h
Purpose     : Windows manager include
----------------------------------------------------------------------
*/

#ifndef WM_H            /* Make sure we only include it once */
#define WM_H


#include "GUI_ConfDefaults.h"
#include "GUI_Type.h"      /* Needed because of typedefs only */
#include "WM_GUI.h"       /* Some functions needed by GUI routines */
#include "GUI.h"

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/* Static memory devices */
#ifndef   WM_SUPPORT_STATIC_MEMDEV
  #define WM_SUPPORT_STATIC_MEMDEV GUI_SUPPORT_MEMDEV
#endif

/* Support for transparency. Switching it off makes Wm smaller and faster */
#ifndef   WM_SUPPORT_TRANSPARENCY
  #define WM_SUPPORT_TRANSPARENCY 1 /* Should be defined outside of GUI_WINSUPPORT because of '#if GUI_WINSUPPORT && WM_SUPPORT_TRANSPARENCY' in some files */
#endif

/* This is for tests only. It will fill the invalid area of a window.
   Can be used for debugging. */
#ifndef WM_SUPPORT_DIAG
  #ifdef WIN32   /* In simulation */
    #define WM_SUPPORT_DIAG GUI_WINSUPPORT
  #else
    #define WM_SUPPORT_DIAG 0
  #endif
#endif

/* Make sure we actually have configured windows. If we have not,
  there is no point for a windows manager and it will therefor not
  generate any code !
*/

#if GUI_WINSUPPORT

/*********************************************************************
*
*       Config defaults
*/
#ifndef   WM_ASSERT
  #define WM_ASSERT(expr) GUI_DEBUG_ASSERT(expr)
#endif

/* Allow older API calls */
#ifndef   WM_COMPATIBLE_MODE
  #define WM_COMPATIBLE_MODE 1
#endif

/* Send a message if visibility of a window has changed */
#ifndef   WM_SUPPORT_NOTIFY_VIS_CHANGED
  #define WM_SUPPORT_NOTIFY_VIS_CHANGED 0
#endif

#ifndef   WM_SUPPORT_CPP
  #if defined (_MSC_VER)
    #define WM_SUPPORT_CPP 1
  #else
    #define WM_SUPPORT_CPP 0
  #endif
#endif

/* Different return value of WM_Exec() and WM_Exec1() */
#ifndef   WM_EXEC_RET_VAL
  #define WM_EXEC_RET_VAL 0
#endif

/* Static memory devices */
#ifndef   WM_SMD_PERIOD
  #define WM_SMD_PERIOD 3000
#endif
#ifndef   WM_SMD_LIMIT
  #define WM_SMD_LIMIT 60
#endif

/*********************************************************************
*
*       Locking macros
*/
#define WM_LOCK()   GUI_LOCK()
#define WM_UNLOCK() GUI_UNLOCK()

/*********************************************************************
*
*       Validate window handles
*/
#ifndef WM_VALIDATE_HANDLE
  #define WM_VALIDATE_HANDLE  (0)
#endif

#if WM_VALIDATE_HANDLE
  #define WM_LOCK_H(hWin) (WM_Obj *)WM__LockValid(hWin)
#else
  #define WM_LOCK_H(hWin) (WM_Obj *)GUI_LOCK_H(hWin)
#endif

/*********************************************************************
*
*       Data types
*/
typedef struct WM_WINDOW_INFO WM_WINDOW_INFO;

struct WM_WINDOW_INFO {
  GUI_HMEM hWin;
  GUI_HMEM hParent;
  GUI_HMEM hFirstChild;
  GUI_HMEM hNext;
  GUI_RECT Rect;
  U32      Status;
  U32      DebugId;
  WM_WINDOW_INFO * pNext;
};

/*********************************************************************
*
*       WM_KEY_INFO
*
*   Description
*     Contains information about a pressed key.
*/
typedef struct {
  int Key;         // The key which has been pressed.
  int PressedCnt;  // >0 if the key has been pressed, 0 if the key has been released.
} WM_KEY_INFO;

/*********************************************************************
*
*       WM_SCROLL_STATE
*
*   Description
*     Saves the scrollstate of a scrollbar.
*/
typedef struct {
  int NumItems;    // Number of items.
  int v;           // Current value.
  int PageSize;    // Number of items visible on one page.
} WM_SCROLL_STATE;

typedef struct {
  int Done;
  int ReturnValue;
} WM_DIALOG_STATUS;

/*********************************************************************
*
*       WM_PID_STATE_CHANGED_INFO
*
*   Description
*     Information about the changed PID state. Sent to a window with
*     the \uref{WM_PID_STATE_CHANGED} message.
*/
typedef struct {
  int x;          // Horizontal position of the PID in window coordinates.
  int y;          // Vertical position of the PID in window coordinates.
  U8  State;      // Pressed state (> 0 if PID is pressed).
  U8  StatePrev;  // Previous pressed state.
} WM_PID_STATE_CHANGED_INFO;

/*********************************************************************
*
*       WM_MOTION_INFO
*
*   Description
*     Contains information about a move with motion support.
*/
typedef struct {
  U8              Cmd;         // Command. See \ref{Motion messages}.
  U8              FinalMove;   // Set to 1 on the final moving operation.
  U8              StopMotion;  // Can be set to 1 to stop motion immediately.
  U8              IsDragging;  // Is set to 1 if the PID is pressed, 0 if released.
  int             dx;          // Distance in X to be used to move the window.
  int             dy;          // Distance in Y to be used to move the window.
  int             da;          // Distance in 1/10 degrees to be used to move an item.
  int             xPos;        // Used to return the current position in X for custom moving operations.
  int             yPos;        // Used to return the current position in Y for custom moving operations.
  int             Period;      // Duration of the moving operation after the PID has been released.
  int             SnapX;       // Raster size in X for snapping operations, 0 if no snapping is required.
  int             SnapY;       // Raster size in Y for snapping operations, 0 if no snapping is required.
  U8              IsOutside;   // If motion is managed by window.
  unsigned        Overlap;     // Overlapping distance allowed for dragging operations.
  U32             Flags;       // To be used to enable motion support.
  GUI_PID_STATE * pState;
  GUI_HMEM        hContext;
  int             Dest;
} WM_MOTION_INFO;

/*********************************************************************
*
*       WM_ZOOM_INFO
*
*  Description
*    Structure used for scaling and panning windows.
*
*  Additional information
*    The factor is shifted by 16 bits to be able to achieve a smooth result. The native size
*    needs to be passed by the application to the WM because otherwise it is not possible
*    to get the exact size of the window in unscaled state.
*/
typedef struct {
  I32       FactorMin;    // Minimum factor to be used (<< 16).
  I32       FactorMax;    // Maximum factor to be used (<< 16).
  U32       xSize;        // Native xSize of window to be zoomed in pixels.
  U32       ySize;        // Native ySize of window to be zoomed in pixels.
  U32       xSizeParent;  /* xSize of parent window.                                  */
  U32       ySizeParent;  /* ySize of parent window.                                  */
  I32       Factor0;      /* Primary factor when starting zoom gesture (<< 16).       */
  int       xPos0;        /* Primary window position in x when starting the gesture.  */
  int       yPos0;        /* Primary window position in y when starting the gesture.  */
  GUI_POINT Center0;      /* Primary center point when starting the gesture.          */
} WM_ZOOM_INFO;

/*********************************************************************
*
*       WM_GESTURE_INFO
*
*  Description
*    Stores the information about a gesture.
*/
typedef struct {
  int            Flags;      // Information regarding gesture type. See \ref{MultiTouch gesture flags}.
  GUI_POINT      Point;      // Relative movement to be processed by the application.
  GUI_POINT      Center;     // Center point for zooming.
  I32            Angle;      // Relative angle difference to be processed by the application.
  I32            Factor;     // When starting a zoom gesture the application has to set the element to the initial value for the gesture. After that during
                             // the gesture it contains the updated value to be processed by the application.
  WM_ZOOM_INFO * pZoomInfo;  // Pointer to be set to a valid location of a WM_ZOOM_INFO structure. The application should keep sure, that the
                             // location remains valid during the gesture.
} WM_GESTURE_INFO;

/*********************************************************************
*
*       WM_MOVE_INFO
*
*   Description
*     Stores the distance of a window move operation.
*/
typedef struct {
  int dx;   // Difference between old and new position on the X-axis.
  int dy;   // Difference between old and new position on the Y-axis.
} WM_MOVE_INFO;

/*********************************************************************
*
*       MultiTouch gesture flags
*
*   Description
*     Flags used for processing gesture input.
*/
#define WM_GF_BEGIN  (1 << 0)  // This flag is set when sending the first message for the gesture.
#define WM_GF_END    (1 << 1)  // Set when releasing a touch point at the end of a gesture.
#define WM_GF_PAN    (1 << 2)  // A panning gesture is detected. The element "Point" of WM_GESTURE_INFO contains the relative movement in pixels to be processed by the application.
#define WM_GF_ZOOM   (1 << 3)  // Zooming is active. When starting a zooming gesture the element "Factor" of WM_GESTURE_INFO has to be set to the initial value to be used by the gesture.
                               // During the gesture the same element contains the updated value to be processed by the application. If movement should be considered simultaneously the element "Point" contains also the relative movement.
#define WM_GF_ROTATE (1 << 4)  // Rotation is active. The element "Angle" of WM_GESTURE_INFO contains the relative movement in degrees (<< 16) to be processed by the application.
                               // To be able to achieve a smooth rotation the value is passed in 1/65536 degrees. If movement should be considered simultaneously the element "Point" contains also the relative movement.
#define WM_GF_DTAP   (1 << 5)

/*********************************************************************
*
*       Messages Ids
*
* The following is the list of windows messages.
*/
#define WM_CREATE                   0x0001  /* The first message received, right after client has actually been created */
#define WM_MOVE                     0x0003  /* window has been moved (Same as WIN32) */

#define WM_SIZE                     0x0005  /* Is sent to a window after its size has changed (Same as WIN32, do not change !) */

#define WM_DELETE                   11      /* Delete (Destroy) command: This tells the client to free its data strutures since the window
                                               it is associates with no longer exists.*/
#define WM_TOUCH                    0x0240  /* Touch screen message */
#define WM_TOUCH_CHILD              13      /* Touch screen message to ancestors */
#define WM_KEY                      14      /* Key has been pressed */

#define WM_PAINT                    0x000F  /* Repaint window (because content is (partially) invalid */

#if GUI_SUPPORT_MOUSE
#define WM_MOUSEOVER                16      /* Mouse has moved, no key pressed */
#define WM_MOUSEOVER_END            18      /* Mouse has moved, no key pressed */
#endif

#define WM_PID_STATE_CHANGED        17      /* Pointer input device state has changed */

#define WM_GET_INSIDE_RECT          20      /* get inside rectangle: client rectangle minus pixels lost to effect */
#define WM_GET_ID                   21      /* Get id of widget */
#define WM_SET_ID                   22      /* Set id of widget */
#define WM_GET_CLIENT_WINDOW        23      /* Get window handle of client window. Default is the same as window */
#define WM_CAPTURE_RELEASED         24      /* Let window know that mouse capture is over */

#define WM_INIT_DIALOG              29      /* Inform dialog that it is ready for init */

#define WM_SET_FOCUS                30      /* Inform window that it has gotten or lost the focus */
#define WM_GET_ACCEPT_FOCUS         31      /* Find out if window can accept the focus */
#define WM_NOTIFY_CHILD_HAS_FOCUS   32      /* Sent to parent when child receives / loses focus */

#define WM_NOTIFY_OWNER_KEY         33      /* Some widgets (e.g. listbox) notify owner when receiving key messages */

#define WM_GET_BKCOLOR              34      /* Return back ground color (only frame window and similar) */
#define WM_GET_SCROLL_STATE         35      /* Query state of scroll bar */

#define WM_SET_SCROLL_STATE         36      /* Set scroll info ... only effective for scrollbars */

#define WM_NOTIFY_CLIENTCHANGE      37      /* Client area may have changed */
#define WM_NOTIFY_PARENT            38      /* Notify parent. Information is detailed as notification code */
#define WM_NOTIFY_PARENT_REFLECTION 39      /* Notify parent reflection.
                                               Sometimes send back as a result of the WM_NOTIFY_PARENT message
                                               to let child react on behalf of its parent.
                                               Information is detailed as notification code */
#define WM_NOTIFY_ENABLE            40      /* Enable or disable widget */
#define WM_NOTIFY_VIS_CHANGED       41      /* Visibility of a window has or may have changed */

#define WM_HANDLE_DIALOG_STATUS     42      /* Set or get dialog status */
#define WM_GET_RADIOGROUP           43      /* Send to all siblings and children of a radio control when
                                               selection changed */
#define WM_MENU                     44      /* Send to owner window of menu widget */
#define WM_SCREENSIZE_CHANGED       45      /* Send to all windows when size of screen has changed */
#define WM_PRE_PAINT                46      /* Send to a window before it receives a WM_PAINT message */
#define WM_POST_PAINT               47      /* Send to a window after (the last) WM_PAINT message */

#define WM_MOTION                   48      /* Automatic motion messages */

#define WM_GET_WINDOW_ID            49      /* Return widget type specific Id (DebugId) */

#define WM_PRE_BANDING              50      /* Send before starting banding process */
#define WM_POST_BANDING             51      /* Send after finishing banding process */

#define WM_USER_DATA                52      /* Send immediately after setting user data */
#define WM_SET_CALLBACK             53      /* Send immediately after setting user data */

#define WM_GET_OFFSET               54      /* Return alignment offset */

#define WM_GET_CONTENT_RECT         55      /* Get content rectangle, e.g. for LISTVIEW: InsideRect minus the HEADER */

#define WM_GESTURE                  0x0119  /* Gesture message */

#define WM_TIMER                    0x0113  /* Timer has expired              (Keep the same as WIN32) */
#define WM_WIDGET                   0x0300  /* 256 messages reserved for Widget messages */
#define WM_USER                     0x0400  /* Reserved for user messages ... (Keep the same as WIN32) */

#define APPW_MSG                    0x1000  /* 256 messages reserved for AppWizard messages */

/*********************************************************************
*
*       Motion messages
*
*  Description
*    Commands sent with a WM_MOTION message. The command can be found
*    in the member \c{Cmd} of the WM_MOTION_INFO structure.
*
*  Additional information
*    More information about these commands can be read under
*    \ref{WM_MOTION message and WM_MOTION_INFO}.
*/
#define WM_MOTION_INIT       0   // Sent to a window to initiate a motion operation.
#define WM_MOTION_MOVE       1   // Sent to a window to achieve custom moving operations.
#define WM_MOTION_GETPOS     2   // Sent to get the current position of custom moving operations.
#define WM_MOTION_GETCONTEXT 3
#define WM_MOTION_GETDEST    4   // Sent to receive the desired position to move to
#define WM_MOTION_GETSNAP    5   // Sent to receive the nearest snap position
#define WM_MOTION_SETCONTEXT 6

/*********************************************************************
*
*       Motion flags
*
*  Description
*    Flags for motion support. The flags are supposed to be OR-combined
*    with the member \c{Flags} of the WM_MOTION_INFO structure.
*/
#define WM_MOTION_MANAGE_BY_WINDOW   (1 << 0)   // Window movement is managed by window itself.

/*********************************************************************
*
*       Motion overlap flags
*
*  Description
*    Flags for setting overlap behavior of widgets. The overlap will
*    come into effect if a distance has been set.
*/
#define WM_MOTION_OVERLAP_TOP        (1 << 4)   // Overlap will be active at the top edge of the widget.
#define WM_MOTION_OVERLAP_BOTTOM     (1 << 5)   // Overlap will be active at the bottom edge of the widget.
#define WM_MOTION_OVERLAP_LEFT       (1 << 6)   // Overlap will be active at the left edge of the widget.
#define WM_MOTION_OVERLAP_RIGHT      (1 << 7)   // Overlap will be active at the right edge of the widget.

/*********************************************************************
*
*       Notification codes
*
*  Description
*    List of all notifications sent by the Window Manager. A notification
*    code is sent with a WM_NOTIFY_PARENT message and can be read with
*    \c{pMsg->Data.v}.
*/
#define WM_NOTIFICATION_CLICKED                   1      // This notification message will be sent when the window has been clicked.
#define WM_NOTIFICATION_RELEASED                  2      // This notification message will be sent when a clicked widget has been released.
#define WM_NOTIFICATION_MOVED_OUT                 3      // This notification message will be sent when the pointer was moved out of the window while it is clicked.
#define WM_NOTIFICATION_SEL_CHANGED               4      // This notification message will be sent when the selection of a widget has changed.
#define WM_NOTIFICATION_VALUE_CHANGED             5      // This notification message will be sent when a widget specific value has changed.
#define WM_NOTIFICATION_SCROLLBAR_ADDED           6      // This notification message will be sent when a SCROLLBAR widget has been added to the window.
#define WM_NOTIFICATION_CHILD_DELETED             7      // This notification message will be sent from a window to its parent before it is deleted.
#define WM_NOTIFICATION_GOT_FOCUS                 8      // This notification message will be sent once a window receives and accepts the focus.
#define WM_NOTIFICATION_LOST_FOCUS                9      // This notification message will be sent when the window has lost the focus.
#define WM_NOTIFICATION_SCROLL_CHANGED           10      // This notification message will be sent when the scroll position of an attached SCROLLBAR widget has changed.
#define WM_NOTIFICATION_SCROLLER_ADDED           16      // This notification message will be sent when a SCROLLER widget has been added to the window.
#define WM_NOTIFICATION_OVERLAP_TOP_ENTERED      17      // This notification message will be sent when the top overlap area of a widget was entered.
#define WM_NOTIFICATION_OVERLAP_BOTTOM_ENTERED   18      // This notification message will be sent when the bottom overlap area of a widget was entered.
#define WM_NOTIFICATION_OVERLAP_LEFT_ENTERED     19      // This notification message will be sent when the left overlap area of a widget was entered.
#define WM_NOTIFICATION_OVERLAP_RIGHT_ENTERED    20      // This notification message will be sent when the right overlap area of a widget was entered.
#define WM_NOTIFICATION_OVERLAP_RELEASED         21      // This notification message will be sent when the overlap area of a widget was entered and has been released.
#define WM_NOTIFICATION_STARTED                  22      // This notification message will be sent when a widget has been started.
#define WM_NOTIFICATION_STOPPED                  23      // This notification message will be sent when a widget has been stopped.

/* not documented */
#define WM_NOTIFICATION_MOTION_STOPPED           11
#define WM_NOTIFICATION_SET                      12
#define WM_NOTIFICATION_CLEAR                    13
#define WM_NOTIFICATION_TEXT_CHANGED             14
#define WM_NOTIFICATION_ENTER_PRESSED            15

#define WM_NOTIFICATION_WIDGET                   30      /* Space for widget defined notifications */
#define WM_NOTIFICATION_USER                     40      /* Space for  application (user) defined notifications */

/*********************************************************************
*
*       Memory management
*/
#define WM_HWIN        GUI_HWIN
#define WM_HWIN_NULL   GUI_HMEM_NULL
#define WM_HMEM        GUI_HMEM
#define WM_HMEM_NULL   GUI_HMEM_NULL
#define WM_HTIMER      GUI_HMEM

/*********************************************************************
*
*       Window defines
*/
#define WM_HBKWIN      WM_GetDesktopWindow()                /* Handle of background window */
#define WM_UNATTACHED  ((WM_HMEM) - 1)                      /* Do not attach to a window */

/*********************************************************************
*
*       Window create flags
*
*  Description
*    Flags that define a window upon creation.
*    These flags can be passed to the create window
*    function as flag-parameter. The flags are combinable using the
*    binary OR-operator.
*/
#define WM_CF_HASTRANS         (1UL << 0)  // Has transparency flag. Must be defined for windows whose client area is not entirely filled.
                                           // To set this flag after the window has been created the function WM_SetTransState() should be used.
#define WM_CF_HIDE             (0UL << 1)  // Hide window after creation (default).
#define WM_CF_SHOW             (1UL << 1)  // Show window after creation.
#define WM_CF_MEMDEV           (1UL << 2)  // Automatically use a Memory Device for drawing. This will avoid flickering and also improve the
                                           // output speed in most cases, as clipping is simplified. The Window Manager creates a Memory
                                           // Device for the current window according to the configured color depth and window size. The
                                           // Memory Device is deleted immediately after the drawing process was finished. \n 
                                           // In order to draw images into a remaining Memory Device the IMAGE widget can be used with the
                                           // creation flag IMAGE_CF_MEMDEV. Details can be found in the section \ref{IMAGE: Image widget}.
                                           // Note that the Memory Device package is required (and needs to be enabled in the configuration)
                                           // in order to be able to use this flag. If Memory Devices are not enabled, this flag is ignored.
#define WM_CF_STAYONTOP        (1UL << 3)  // Make sure window stays on top of all siblings created without this flag.
#define WM_CF_DISABLED         (1UL << 4)  // Window is disabled after creation. This means it receives no PID (mouse and touch) input.
#define WM_CF_ACTIVATE         (1UL << 5)  /* Is (re)used as WM_SF_INVALID */
#define WM_CF_FGND             (0UL << 6)  // Put window in foreground after creation (default).
#define WM_CF_BGND             (1UL << 6)  // Put window in background after creation.
#define WM_CF_ANCHOR_RIGHT     (1UL << 7)  // Anchors the right edge of the new window relative to the right edge of the parent window. If
                                           // the position of the parent windows right edge will be adjusted due to a size change, the
                                           // position of new window will also be adjusted.
#define WM_CF_ANCHOR_BOTTOM    (1UL << 8)  // Anchors the bottom edge of the new window relative to the bottom edge of the parent window. If
                                           // the position of the parent windows bottom edge will be adjusted due to a size change, the
                                           // position of new window will also be adjusted.
#define WM_CF_ANCHOR_LEFT      (1UL << 9)  // Anchors the left edge of the new window relative to the left edge of the parent window (default).
                                           // If the position of the parent windows left edge will be adjusted due to a size change,
                                           // the position of new window will also be adjusted.
#define WM_CF_ANCHOR_TOP       (1UL << 10) // Anchors the top edge of the new window relative to the top edge of the parent window (default).
                                           // If the position of the parent windows top edge will be adjusted due to a size change, the
                                           // position of new window will also be adjusted.
#define WM_CF_CONST_OUTLINE    (1UL << 11) // This flag is an optimization for transparent windows. It gives the Window Manager a chance
                                           // to optimize redrawing and invalidation of transparent windows. A transparent window is normally
                                           // redrawn as part of the background, which is less efficient than redrawing the window separately.
                                           // However, this flag may NOT be used if the window has semi transparency (alpha blending /
                                           // anti-aliasing with background) or the outline (the shape) changes with the window's states.
                                           // To set this flag after the window has been created the function WM_SetTransState() should be used.
#define WM_CF_LATE_CLIP        (1UL << 12) // This flag can be used to tell the WM that the clipping should be done in the drawing routines
                                           // (late clipping). The default behavior of the WM is early clipping. That means that the clipping
                                           // rectangle will be calculated before a WM_PAINT message will be sent to a window. In dependence
                                           // of other existing windows it might be necessary to send more than one WM_PAINT message to a window.
                                           // If using WM_CF_LATE_CLIP the WM makes sure only one message will be sent to an invalid window and
                                           // the clipping will be done by the drawing routines. The \c{Sample} folder of emWin contains the
                                           // example \c{WM_LateClipping.c} to show the effect.
#define WM_CF_MEMDEV_ON_REDRAW (1UL << 13) // Equals WM_CF_MEMDEV with the difference that the according window is drawn the first time without
                                           // using a Memory Device. The WM will automatically use a Memory Device for redrawing. This flag can
                                           // be used as a replacement of WM_CF_MEMDEV. It typically accelerates the initial rendering of the
                                           // window, but maintains the advantage of flicker free updates.
#define WM_SF_INVALID_DRAW     (1UL << 14) /* Tells the WM to redraw the window using the static memory device */
#define WM_SF_DELETE           (1UL << 15) /* The window will be deleted on the next time WM_Exec() is called */
#define WM_CF_STATIC           (1UL << 16) // A static memory device is used for drawing. That means if a window with this flag receives a
                                           // WM_PAINT message, emWin first checks if a static memory device exists. If not, it will be created
                                           // and the content of the window/widget will be (pre)rendered in the memory device. If such a window
                                           // is moved to a new position, only the already existing memory needs to be drawn at the new position.
                                           // This kind of drawing a window could improve the performance a lot, but needs a large amount of RAM.
#define WM_CF_MOTION_X         (1UL << 17) // Window can be moved automatically in X axis.
#define WM_CF_MOTION_Y         (1UL << 18) // Window can be moved automatically in Y axis.
#define WM_CF_GESTURE          (1UL << 19) // Marks the window to be able to receive gesture messages. This requires gesture support.
#define WM_CF_ZOOM             (1UL << 20) // Window can be scaled automatically by multi-touch gesture input.
#define WM_CF_MOTION_R         (1UL << 21) // This enables the window to be rotated.
#define WM_CF_UNTOUCHABLE      (1UL << 22) // A window created with this flag routes its touch input to its parent. This makes a window 'untouchable'.
#define WM_CF_APPWIZARD        (1UL << 23) /* Window is an AppWizard object */
#define WM_CF_MEMDEV_CLIPPING  (1UL << 24) // Reactivates the window manager when drawing into memory devices created via WM_CF_MEMDEV. Otherwise
                                           // it can happen that invisible windows (e.g. a window covered completely by another window) still draws
                                           // its content into a memory device but the memory device doesn't get drawn to the LCD. With this flag
                                           // these unnecessary drawing operations can be avoided. Attention, if there is a non transparent window
                                           // in the foreground, which is smaller than the window in the back, tiling will be used and multiple paint
                                           // events into the memory device will be performed!

/*********************************************************************
*
*       Window manager types
*/
typedef struct WM_Obj     WM_Obj;
typedef struct WM_MESSAGE WM_MESSAGE;

typedef void WM_CALLBACK( WM_MESSAGE * pMsg);

/*********************************************************************
*
*       WM_MESSAGE
*
*   Description
*     Contains the data for a message sent by a window.
*/
struct WM_MESSAGE {
  int MsgId;               // Type of message.
  WM_HWIN hWin;            // Destination window.
  WM_HWIN hWinSrc;         // Source window.
  union {
    const void * p;        // Message-specific const data pointer.
    void * pData;          // Message-specific data pointer.
    int v;                 // Message-specific data value.
    PTR_ADDR u;
    GUI_COLOR Color;
    void (* pFunc)(void);
  } Data;
};

struct WM_Obj {
  GUI_RECT Rect;        /* Outer dimensions of window */
  GUI_RECT InvalidRect; /* Invalid rectangle */
  WM_CALLBACK* cb;      /* Ptr to notification callback */
  WM_HWIN hNextLin;     /* Next window in linear list */
  WM_HWIN hParent;
  WM_HWIN hFirstChild;
  WM_HWIN hNext;
  #if WM_SUPPORT_STATIC_MEMDEV
    GUI_MEMDEV_Handle hMem; /* Static memory device */
  #endif
  U32 Status;           /* Status flags */
  #if WM_SUPPORT_CPP
    void * ObjPtr;
  #endif
};

typedef void WM_tfPollPID(void);
typedef void WM_tfForEach(WM_HWIN hWin, void * pData);

typedef void (* WM_tfInvalidateParent)  (const GUI_RECT * pInvalidRect, WM_HWIN hParent, WM_HWIN hStop);
typedef void (* WM_tfInvalidateDrawFunc)(WM_HWIN hWin);
typedef int  (* WM_tfPaint1Func)        (WM_HWIN hWin);

typedef struct {
  WM_HMEM  hTimer;
  WM_HWIN  hWin;
  int      UserId;
} WM_TIMER_OBJ;

/*********************************************************************
*
*       General control routines
*/
void WM_Activate  (void);
void WM_Deactivate(void);
void WM_Init      (void);
int  WM_Exec      (void);    /* Execute all jobs ... Return 0 if nothing was done. */
int  WM_Exec1     (void);    // Execute only one job
U32  WM_SetCreateFlags(U32 Flags);
WM_tfPollPID * WM_SetpfPollPID(WM_tfPollPID * pf);

/*********************************************************************
*
*       Window manager interface
*/
void    WM_AttachWindow              (WM_HWIN hWin, WM_HWIN hParent);
void    WM_AttachWindowAt            (WM_HWIN hWin, WM_HWIN hParent, int x, int y);
int     WM_CheckScrollPos            (WM_SCROLL_STATE * pScrollState, int Pos, int LowerDist, int UpperDist); /* not to be documented (may change in future version) */
void    WM_ClrHasTrans               (WM_HWIN hWin);
WM_HWIN WM_CreateWindow              (int x0, int y0, int xSize, int ySize, U32 Style, WM_CALLBACK * cb, int NumExtraBytes);
WM_HWIN WM_CreateWindowAsChild       (int x0, int y0, int xSize, int ySize, WM_HWIN hWinParent, U32 Style, WM_CALLBACK* cb, int NumExtraBytes);
void    WM_DeleteWindow              (WM_HWIN hWin);
void    WM_DeleteWindowSecure        (WM_HWIN hWin);
void    WM_DetachWindow              (WM_HWIN hWin);
void    WM_EnableGestures            (WM_HWIN hWin, int OnOff);
int     WM_GetHasTrans               (WM_HWIN hWin);
WM_HWIN WM_GetFocusedWindow          (void);
int     WM_GetInvalidRect            (WM_HWIN hWin, GUI_RECT * pRect);
int     WM_GetStayOnTop              (WM_HWIN hWin);
void    WM_HideWindow                (WM_HWIN hWin);
void    WM_InvalidateArea            (const GUI_RECT * pRect);
void    WM_InvalidateRect            (WM_HWIN hWin, const GUI_RECT * pRect);
void    WM_InvalidateWindow          (WM_HWIN hWin);
void    WM_InvalidateWindowAndDescsEx(WM_HWIN hWin, const GUI_RECT * pInvalidRect, U16 Flags);
void    WM_InvalidateWindowAndDescs  (WM_HWIN hWin);    /* not to be documented (may change in future version) */
int     WM_IsEnabled                 (WM_HWIN hObj);
char    WM_IsCompletelyCovered       (WM_HWIN hWin);    /* Checks if the window is completely covered by other windows */
char    WM_IsCompletelyVisible       (WM_HWIN hWin);    /* Is the window completely visible ? */
int     WM_IsFocusable               (WM_HWIN hWin);
int     WM_IsUntouchable             (WM_HWIN hWin);
int     WM_IsVisible                 (WM_HWIN hWin);
int     WM_IsWindow                  (WM_HWIN hWin);    /* Check validity */
void    WM_Rect2Screen               (WM_HWIN hWin, GUI_RECT * pRect);
void    WM_Rect2Client               (WM_HWIN hWin, GUI_RECT * pRect);
void    WM_SetAnchor                 (WM_HWIN hWin, U16 AnchorFlags);
void    WM_SetHasTrans               (WM_HWIN hWin);
void    WM_SetId                     (WM_HWIN hObj, int Id);
void    WM_SetStayOnTop              (WM_HWIN hWin, int OnOff);
void    WM_SetTransState             (WM_HWIN hWin, unsigned State);
int     WM_SetUntouchable            (WM_HWIN hWin, int OnOff);
void    WM_ShowWindow                (WM_HWIN hWin);
void    WM_ValidateRect              (WM_HWIN hWin, const GUI_RECT * pRect);
void    WM_ValidateWindow            (WM_HWIN hWin);
void    WM_XY2Screen                 (WM_HWIN hWin, int * px, int * py);
void    WM_XY2Client                 (WM_HWIN hWin, int * px, int * py);

#define WM_GetFocussedWindow WM_GetFocusedWindow
#define WM_IsFocussable      WM_IsFocusable

/* Gesture support */
void WM_GESTURE_Enable  (int OnOff);
int  WM_GESTURE_EnableEx(int OnOff, int MaxFactor);
void WM_GESTURE_Exec    (void);
I32  WM_GESTURE_SetThresholdAngle(I32 ThresholdAngle);
I32  WM_GESTURE_SetThresholdDist (I32 ThresholdDist);

/* Motion support */
void     WM_MOTION_Enable          (int OnOff);
void     WM_MOTION_SetMovement     (WM_HWIN hWin, int Axis, I32 Speed, I32 Dist);
void     WM_MOTION_SetMotion       (WM_HWIN hWin, int Axis, I32 Speed, I32 Deceleration);
void     WM_MOTION_SetMoveable     (WM_HWIN hWin, U32 Flags, int OnOff);
void     WM_MOTION_SetDeceleration (WM_HWIN hWin, int Axis, I32 Deceleration);
unsigned WM_MOTION_SetDefaultPeriod(unsigned Period);
void     WM_MOTION_SetSpeed        (WM_HWIN hWin, int Axis, I32 Velocity);
void     WM_MOTION_SetMinMotion    (unsigned MinMotion);
void     WM_MOTION_SetThreshold    (unsigned Threshold);

/* Motion support, private interface */
WM_HMEM WM_MOTION__CreateContext(void);
void    WM_MOTION__DeleteContext(WM_HMEM hContext);
WM_HMEM WM_MOTION__GetContext   (WM_HWIN hWin);
void    WM_MOTION__SetContext   (WM_HWIN hWin, WM_HMEM hContext);
int     WM_MOTION__IsActive     (void);

/* Motion support, private function(s) */
void     WM__SetMotionCallback (void(* cbMotion) (GUI_PID_STATE * pState, void * p));

/* Static memory devices */
#if (GUI_SUPPORT_MEMDEV)
  #define GUI_MEMDEV_EDGE_LEFT   0
  #define GUI_MEMDEV_EDGE_RIGHT  1
  #define GUI_MEMDEV_EDGE_TOP    2
  #define GUI_MEMDEV_EDGE_BOTTOM 3

  int               GUI_MEMDEV_BlendWinBk       (WM_HWIN hWin, int Period, U32 BlendColor, U8 BlendIntens);
  int               GUI_MEMDEV_BlurAndBlendWinBk(WM_HWIN hWin, int Period, U8 BlurDepth, U32 BlendColor, U8 BlendIntens);
  int               GUI_MEMDEV_BlurWinBk        (WM_HWIN hWin, int Period, U8 BlurDepth);
  void              GUI_MEMDEV_CreateStatic     (WM_HWIN hWin);
  GUI_MEMDEV_Handle GUI_MEMDEV_CreateWindowDevice(WM_HWIN hWin);
  int               GUI_MEMDEV_FadeInWindow     (WM_HWIN hWin, int Period);
  int               GUI_MEMDEV_FadeOutWindow    (WM_HWIN hWin, int Period);
  GUI_MEMDEV_Handle GUI_MEMDEV_GetStaticDevice  (WM_HWIN hWin);
  GUI_MEMDEV_Handle GUI_MEMDEV_GetWindowDevice  (WM_HWIN hWin);
  int               GUI_MEMDEV_MoveInWindow     (WM_HWIN hWin, int x, int y, int a180, int Period);
  int               GUI_MEMDEV_MoveOutWindow    (WM_HWIN hWin, int x, int y, int a180, int Period);
  int               GUI_MEMDEV_Paint1Static     (WM_HWIN hWin);                                     /* not to be documented */
  int               GUI_MEMDEV_ShiftInWindow    (WM_HWIN hWin, int Period, int Direction);
  int               GUI_MEMDEV_ShiftOutWindow   (WM_HWIN hWin, int Period, int Direction);
  int               GUI_MEMDEV_SwapWindow       (WM_HWIN hWin, int Period, int Edge);

  void              GUI_MEMDEV__CreateStatic    (WM_HWIN hWin);
#endif

/* Move/resize windows */
void WM_MoveWindow                (WM_HWIN hWin, int dx, int dy);
void WM_ResizeWindow              (WM_HWIN hWin, int dx, int dy);
void WM_MoveTo                    (WM_HWIN hWin, int x, int y);
void WM_MoveChildTo               (WM_HWIN hWin, int x, int y);
void WM_SetSize                   (WM_HWIN hWin, int XSize, int YSize);
void WM_SetWindowPos              (WM_HWIN hWin, int xPos, int yPos, int xSize, int ySize);
int  WM_SetXSize                  (WM_HWIN hWin, int xSize);
int  WM_SetYSize                  (WM_HWIN hWin, int ySize);
int  WM_SetScrollbarH             (WM_HWIN hWin, int OnOff); /* not to be documented (may change in future version) */
int  WM_SetScrollbarV             (WM_HWIN hWin, int OnOff); /* not to be documented (may change in future version) */


/*********************************************************************
*
*       ToolTip period indexes
*
*   Description
*     Period indexes for ToolTip related routines.
*/
#define WM_TOOLTIP_PI_FIRST 0    // Period to be used the first time the PID is hovered over a tool. The ToolTip appears after the
                                 // PID has not moved for at least this period. Default is 1000 ms.
#define WM_TOOLTIP_PI_SHOW  1    // Period to be used for showing the ToolTip. The ToolTip disappears after the PID remains for
                                 // at least this period without moving. Default is 5000 ms.
#define WM_TOOLTIP_PI_NEXT  2    // Period to be used if the PID hovers over a tool of the same parent as before. The ToolTip
                                 // appears after the PID is not moved for at least this period. Default is 50 ms.

/*********************************************************************
*
*       ToolTip color indexes
*
*   Description
*     Color indexes for ToolTip related routines.
*/
#define WM_TOOLTIP_CI_BK    0   // Color to be used for the background.
#define WM_TOOLTIP_CI_FRAME 1   // Color to be used for the thin frame.
#define WM_TOOLTIP_CI_TEXT  2   // Color to be used for the text.

typedef WM_HMEM WM_TOOLTIP_HANDLE;

/*********************************************************************
*
*       TOOLTIP_INFO
*
*   Description
*     Contains the information about a ToolTip.
*/
typedef struct {
  int          Id;     // Id of the ToolTip.
  const char * pText;  // String containing the text of the ToolTip.
} TOOLTIP_INFO;

int               WM_TOOLTIP_AddTool         (WM_TOOLTIP_HANDLE hToolTip, WM_HWIN hTool, const char * pText);
WM_TOOLTIP_HANDLE WM_TOOLTIP_Create          (WM_HWIN hDlg, const TOOLTIP_INFO * pInfo, unsigned NumItems);
void              WM_TOOLTIP_Delete          (WM_TOOLTIP_HANDLE hToolTip);
GUI_COLOR         WM_TOOLTIP_SetDefaultColor (unsigned Index, GUI_COLOR Color);
const GUI_FONT *  WM_TOOLTIP_SetDefaultFont  (const GUI_FONT * pFont);
unsigned          WM_TOOLTIP_SetDefaultPeriod(unsigned Index, unsigned Period);

/* ToolTip support, private */
void WM__SetToolTipCallback(void(* cbToolTip)(GUI_PID_STATE * pState, WM_HWIN));

/* Timer */
#ifdef GUI_X_CREATE_TIMER
  int  WM_CreateTimer    (WM_HWIN hWin, int UserID, int Period, int Mode); /* not to be documented (may change in future version) */
  void WM_DeleteTimer    (WM_HWIN hWin, int UserId); /* not to be documented (may change in future version) */
#else
  WM_HMEM WM_CreateTimer (WM_HWIN hWin, int UserID, int Period, int Mode); /* not to be documented (may change in future version) */
  void    WM_DeleteTimer (WM_HMEM hTimer); /* not to be documented (may change in future version) */
  void    WM_RestartTimer(WM_HMEM hTimer, int Period);
#endif
int WM_GetTimerId(WM_HTIMER hTimer);

/* Diagnostics */
int WM_GetNumWindows(void);
int WM_GetNumInvalidWindows(void);

/* Scroll state related functions */
void WM_CheckScrollBounds(WM_SCROLL_STATE * pScrollState); /* not to be documented (may change in future version) */
int  WM_GetScrollPosH    (WM_HWIN hWin);
int  WM_GetScrollPosV    (WM_HWIN hWin);
void WM_SetScrollPosH    (WM_HWIN hWin, unsigned ScrollPos);
void WM_SetScrollPosV    (WM_HWIN hWin, unsigned ScrollPos);
int  WM_SetScrollValue   (WM_SCROLL_STATE * pScrollState, int v); /* not to be documented (may change in future version) */

/* Get / Set (new) callback function */
WM_CALLBACK * WM_SetCallback(WM_HWIN hWin, WM_CALLBACK * cb);
WM_CALLBACK * WM_GetCallback(WM_HWIN hWin);

/* Get size/origin of a window */
void      WM_GetClientRect           (GUI_RECT * pRect);
void      WM_GetClientRectEx         (WM_HWIN hWin, GUI_RECT * pRect);
void      WM_GetContentRect          (GUI_RECT * pRect);
void      WM_GetContentRectEx        (WM_HWIN hWin, GUI_RECT * pRect);
void      WM_GetInsideRect           (GUI_RECT * pRect);
void      WM_GetInsideRectEx         (WM_HWIN hWin, GUI_RECT * pRect);
void      WM_GetInsideRectExScrollbar(WM_HWIN hWin, GUI_RECT * pRect); /* not to be documented (may change in future version) */
void      WM_GetWindowRect           (GUI_RECT * pRect);
void      WM_GetWindowRectEx         (WM_HWIN hWin, GUI_RECT * pRect);
int       WM_GetOrgX                 (void);
int       WM_GetOrgY                 (void);
int       WM_GetWindowOrgX           (WM_HWIN hWin);
int       WM_GetWindowOrgY           (WM_HWIN hWin);
int       WM_GetWindowSizeX          (WM_HWIN hWin);
int       WM_GetWindowSizeY          (WM_HWIN hWin);
WM_HWIN   WM_GetFirstChild           (WM_HWIN hWin);
WM_HWIN   WM_GetNextSibling          (WM_HWIN hWin);
WM_HWIN   WM_GetParent               (WM_HWIN hWin);
WM_HWIN   WM_GetPrevSibling          (WM_HWIN hWin);
int       WM_GetId                   (WM_HWIN hWin);
WM_HWIN   WM_GetScrollbarV           (WM_HWIN hWin);
WM_HWIN   WM_GetScrollbarH           (WM_HWIN hWin);
WM_HWIN   WM_GetScrollerV            (WM_HWIN hWin);
WM_HWIN   WM_GetScrollerH            (WM_HWIN hWin);
WM_HWIN   WM_GetScrollPartner        (WM_HWIN hWin);
WM_HWIN   WM_GetClientWindow         (WM_HWIN hObj);
GUI_COLOR WM_GetBkColor              (WM_HWIN hObj);

/* Change Z-Order of windows */
void WM_BringToBottom(WM_HWIN hWin);
void WM_BringToTop   (WM_HWIN hWin);
void WM_BringBehind  (WM_HWIN hWinFore, WM_HWIN hWinBack);

GUI_COLOR WM_SetDesktopColor  (GUI_COLOR Color);
GUI_COLOR WM_SetDesktopColorEx(GUI_COLOR Color, unsigned int LayerIndex);
void      WM_SetDesktopColors (GUI_COLOR Color);

/* Select window used for drawing operations */
WM_HWIN WM_SelectWindow           (WM_HWIN  hWin);
WM_HWIN WM_GetActiveWindow        (void);
void    WM_Paint                  (WM_HWIN hObj);
void    WM_Update                 (WM_HWIN hWin);
void    WM_PaintWindowAndDescs    (WM_HWIN hWin);
void    WM_UpdateWindowAndDescs   (WM_HWIN hWin);

/* Get foreground/background windows */
WM_HWIN WM_GetDesktopWindow  (void);
WM_HWIN WM_GetDesktopWindowEx(unsigned int LayerIndex);

/* Reduce clipping area of a window */
const GUI_RECT * WM_SetUserClipRect(const GUI_RECT * pRect);
void             WM_SetDefault     (void);

/* Use of memory devices */
void WM_EnableMemdev              (WM_HWIN hWin);
void WM_DisableMemdev             (WM_HWIN hWin);

/* Automatic use of multiple buffers */
int WM_MULTIBUF_Enable  (int OnOff);
int WM_MULTIBUF_EnableEx(int OnOff, U32 LayerMask);

extern const GUI_MULTIBUF_API * WM_MULTIBUF__pAPI;

typedef void (* T_WM_EXEC_GESTURE)(void);

extern T_WM_EXEC_GESTURE WM__pExecGestures;

/* ... */
int WM_OnKey(int Key, int Pressed);

/* Modal related functions */
void    WM_MakeModal(WM_HWIN hWin);
WM_HWIN WM_GetModalWindow(void);
int     WM_SetModalLayer(int LayerIndex);
int     WM_GetModalLayer(void);

/*********************************************************************
*
*       Message related functions
*
*  Please note that some of these functions do not yet show up in the
*  documentation, as they should not be required by application program.
*/
void      WM_NotifyParent         (WM_HWIN hWin, int Notification);
void      WM_SendMessage          (WM_HWIN hWin, WM_MESSAGE * p);
void      WM_SendMessageNoPara    (WM_HWIN hWin, int MsgId);             /* not to be documented (may change in future */
void      WM_DefaultProc          (WM_MESSAGE * pMsg);
int       WM_BroadcastMessage     (WM_MESSAGE * pMsg);
void      WM_SetScrollState       (WM_HWIN hWin, const WM_SCROLL_STATE * pState);
void      WM_SetEnableState       (WM_HWIN hItem, int State);
void      WM_SendToParent         (WM_HWIN hWin, WM_MESSAGE * pMsg);
int       WM_HasFocus             (WM_HWIN hWin);
int       WM_SetFocus             (WM_HWIN hWin);
WM_HWIN   WM_SetFocusOnNextChild  (WM_HWIN hParent);     /* Set the focus to the next child */
WM_HWIN   WM_SetFocusOnPrevChild  (WM_HWIN hParent);     /* Set the focus to the previous child */
WM_HWIN   WM_GetDialogItem        (WM_HWIN hWin, int Id);
void      WM_EnableWindow         (WM_HWIN hWin);
void      WM_DisableWindow        (WM_HWIN hWin);
void      WM_GetScrollState       (WM_HWIN hObj, WM_SCROLL_STATE * pScrollState);
WM_HWIN   WM_GetChild             (WM_HWIN hWin, int Id);

/*********************************************************************
*
*       Managing user data
*/
int       WM_GetUserData   (WM_HWIN hWin, void * pDest, int SizeOfBuffer);
int       WM_SetUserData   (WM_HWIN hWin, const void * pSrc, int SizeOfBuffer);
int       WM__GetUserDataEx(WM_HWIN hWin, void * pDest, int NumBytes, int SizeOfObject);
int       WM__SetUserDataEx(WM_HWIN hWin, const void * pSrc, int NumBytes, int SizeOfObject);

/*********************************************************************
*
*       Capturing input focus
*/
int     WM_HasCaptured   (WM_HWIN hWin);
WM_HWIN WM_GetCapture    (int * pAutoRelease);
void    WM_SetCapture    (WM_HWIN hObj, int AutoRelease);
void    WM_SetCaptureMove(WM_HWIN hWin, const GUI_PID_STATE * pState, int MinVisibility, int LimitTop); /* Not yet documented */
void    WM_ReleaseCapture(void);

/*********************************************************************
*
*       Misc routines
*/
int       WM_HandlePID      (void);
WM_HWIN   WM_Screen2hWin    (int x, int y);
WM_HWIN   WM_Screen2hWinEx  (WM_HWIN hStop, int x, int y);
void      WM_ForEachDesc    (WM_HWIN hWin, WM_tfForEach * pcb, void * pData);
void      WM_SetScreenSize  (int xSize, int ySize);
int       WM_PollSimMsg     (void);
int       WM_GetWindowInfo  (WM_WINDOW_INFO * pInfo, int FirstWindow);
void      WM_EnablePID      (int OnOff);
int       WM_IsEnabledPID   (void);

/*********************************************************************
*
*       Diagnostics routines
*/
#if (WM_SUPPORT_DIAG)
void WM_DIAG_EnableInvalidationColoring(int OnOff);
#endif

/*********************************************************************
*
*       Macros for compatibility with older versions
*/
#if WM_COMPATIBLE_MODE
  #define WM_HideWin        WM_HideWindow
  #define WM_ShowWin        WM_ShowWindow
  #define WM_ExecIdle       WM_Exec
  #define WM_ExecIdle1      WM_Exec1
  #define WM_Invalidate     WM_InvalidateWindow
  #define WM_GetXSize       WM_GetWindowSizeX
  #define WM_GetYSize       WM_GetWindowSizeY
  #define WM_SetUserClipArea WM_SetUserClipRect
#endif

#endif   /* GUI_WINSUPPORT */

#if defined(__cplusplus)
}
#endif

#endif   /* WM_H */

/*************************** End of file ****************************/
