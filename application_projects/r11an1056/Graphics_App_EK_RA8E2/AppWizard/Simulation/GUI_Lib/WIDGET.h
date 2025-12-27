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
File        : WIDGET.h
Purpose     : Widget interface
---------------------------END-OF-HEADER------------------------------
*/

#ifndef WIDGET_H        /* Avoid multiple inclusion  */
#define WIDGET_H

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

#include "WM.h"  /* Window manager, including some internals, which speed things up */
#include "GUI_Debug.h"

#if GUI_WINSUPPORT

/*********************************************************************
*
*       Typedefs
*
**********************************************************************
*/
typedef struct {
  WM_HWIN    hWin;
  int        Cmd;         /* WIDGET_ITEM_GET_XSIZE, WIDGET_ITEM_GET_YSIZE, WIDGET_ITEM_DRAW, */
  int        ItemIndex;
  int        Col;
  int        x0, y0, x1, y1;
  I32        Angle;
  void     * p;
} WIDGET_ITEM_DRAW_INFO;

typedef int  WIDGET_DRAW_ITEM_FUNC(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);
typedef void WIDGET_PAINT         (WM_HWIN hObj);
typedef void WIDGET_CREATE        (WM_HWIN hObj);

typedef struct {
  WIDGET_PAINT  * pfPaint;
  WIDGET_CREATE * pfCreate;
  void          * pSkinPrivate;
} WIDGET_SKIN;

/*********************************************************************
*
*       WIDGET_COPY
* 
*  Description
*    To be used for copy-optimizations when swiping data.
*/
typedef struct {
  I16 x0, y0, x1, y1, xSize, ySize;
} WIDGET_COPY;

/*********************************************************************
*
*       WIDGET_SCROLLSTATE_API
* 
*  Description
*    API for conversion between vertical motion and WM_SCROLL_STATE.
* 
*    Note: hParent is always the scrollable widget, it is the parent of
*    a SCROLLER.
*/
typedef struct {
  //
  // Getters
  //
  int          (* pfGetScrollPosX)       (WM_HWIN hParent);
  void         (* pfGetScrollPosY)       (WM_HWIN hParent, int * pScrollStateV, int * pMotionPosY);
  unsigned     (* pfGetNumRows)          (WM_HWIN hParent);
  int          (* pfGetRowHeight)        (WM_HWIN hParent);
  int          (* pfGetYOffset)          (WM_HWIN hParent);
  int          (* pfGetItemPosY)         (WM_HWIN hParent, unsigned Index);
  int          (* pfGetIndexFromItemPosY)(WM_HWIN hParent, int ScrollStateV, int * pRemainder);
  int          (* pfGetMaxMotionPosX)    (WM_HWIN hParent);
  int          (* pfGetMaxMotionPosY)    (WM_HWIN hParent);
  unsigned     (* pfGetOverlap)          (WM_HWIN hParent, int * pPeriod, U8 * pFlags);
  //
  // Setters
  //
  void         (* pfSetScrollPosX)       (WM_HWIN hParent, int ScrollStateH);
  void         (* pfSetScrollPosY)       (WM_HWIN hParent, int ScrollStateV, int MotionPosY);
  //
  // Properties
  //
  U8              IndvRowHeight; // 1 if each row can have a different height, 0 if not
  U8              NoConv;        // 1 if no conversion is necessary
} WIDGET_SCROLLSTATE_API;

/*********************************************************************
*
*       WIDGET_ENABLE_MOTION_CONTEXT
* 
*  Description
*    Context for WIDGET__EnableMotion().
*    With the context the routine knows which exact flags should be set
*    or removed.
*/
typedef struct {
  void    * pFlags;           // Pointer to Flags of the widget's object struct.
  U8        SizeOfFlags;      // Size of pFlags (pFlags can be U8, U16, U32).
  U32       FlagMotionH;      // Copy of <WIDGET>_CF_MOTION_H (e.g. LISTVIEW_CF_MOTION_H)
  U32       FlagMotionV;      // Copy of <WIDGET>_CF_MOTION_V
  U32       FlagAutoScrollH;  // Copy of <WIDGET>_CF_AUTOSCROLL_H
  U32       FlagAutoScrollV;  // Copy of <WIDGET>_CF_AUTOSCROLL_V
  WM_HMEM * phContext;        // Pointer to widget's motion context for (de)allocation.
} WIDGET_ENABLE_MOTION_CONTEXT;

/*********************************************************************
*
*       Important: WIDGET_DRAW_ITEM_FUNC needs to be defined
*                  in SCROLLBAR.h!
*
**********************************************************************
*/
#include "SCROLLBAR.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define WIDGET_LOCK(hWin) ((WIDGET*)WM_LOCK_H(hWin))

/*********************************************************************
*
*       Include WIDGET_TYPE_... IDs
*
**********************************************************************
*/
#include "WIDGET_ID.h"

/*********************************************************************
*
*       Configuration
*/
#ifndef   WIDGET_MIN_TIME_PER_FRAME
  #define WIDGET_MIN_TIME_PER_FRAME 40
#endif
#ifndef   WIDGET_USE_PARENT_EFFECT
  #define WIDGET_USE_PARENT_EFFECT 0
#endif
#ifndef   WIDGET_USE_FLEX_SKIN
  #if WM_SUPPORT_TRANSPARENCY
    #define WIDGET_USE_FLEX_SKIN     1
  #else
    #define WIDGET_USE_FLEX_SKIN     0
  #endif
#endif
#if !defined(WIDGET_USE_SCHEME_SMALL) && !defined(WIDGET_USE_SCHEME_MEDIUM) && !defined(WIDGET_USE_SCHEME_LARGE)
  #define WIDGET_USE_SCHEME_SMALL  1
  #define WIDGET_USE_SCHEME_MEDIUM 0
  #define WIDGET_USE_SCHEME_LARGE  0
#endif
#ifndef   WIDGET_USE_SCHEME_SMALL
  #define WIDGET_USE_SCHEME_SMALL  0
#endif
#ifndef   WIDGET_USE_SCHEME_MEDIUM
  #define WIDGET_USE_SCHEME_MEDIUM 0
#endif
#ifndef   WIDGET_USE_SCHEME_LARGE
  #define WIDGET_USE_SCHEME_LARGE  0
#endif
#if (WIDGET_USE_SCHEME_SMALL + WIDGET_USE_SCHEME_MEDIUM + WIDGET_USE_SCHEME_LARGE) > 1
  #error Only one scheme can be selected!
#endif

/*********************************************************************
*
*       States
*/
#define WIDGET_STATE_FOCUS              (1 << 0)
#define WIDGET_STATE_HORIZONTAL         (0 << 3)
#define WIDGET_STATE_VERTICAL           (1 << 3)
#define WIDGET_STATE_FOCUSABLE          (1 << 4)
#define WIDGET_STATE_INVISIBLEFOCUS     (1 << 5)

#define WIDGET_STATE_USER0              (1 << 8)    /* Freely available for derived widget */
#define WIDGET_STATE_USER1              (1 << 9)    /* Freely available for derived widget */
#define WIDGET_STATE_USER2              (1 << 10)   /* Freely available for derived widget */

#define WIDGET_STATE_FOCUSSABLE         WIDGET_STATE_FOCUSABLE

/*********************************************************************
*
*       Skinning message identifiers
*/
#define WIDGET_ITEM_CREATE              0
#define WIDGET_ITEM_DRAW                1
#define WIDGET_ITEM_DRAW_ARROW          2
#define WIDGET_ITEM_DRAW_ARROW_L        3
#define WIDGET_ITEM_DRAW_ARROW_R        4
#define WIDGET_ITEM_DRAW_BACKGROUND     5
#define WIDGET_ITEM_DRAW_BITMAP         6
#define WIDGET_ITEM_DRAW_BUTTON         7
#define WIDGET_ITEM_DRAW_BUTTON_L       8
#define WIDGET_ITEM_DRAW_BUTTON_R       9
#define WIDGET_ITEM_DRAW_FOCUS         10
#define WIDGET_ITEM_DRAW_FRAME         11
#define WIDGET_ITEM_DRAW_OVERLAP       12
#define WIDGET_ITEM_DRAW_OVERLAY       13
#define WIDGET_ITEM_DRAW_SEP           14
#define WIDGET_ITEM_DRAW_SHAFT         15
#define WIDGET_ITEM_DRAW_SHAFT_L       16
#define WIDGET_ITEM_DRAW_SHAFT_R       17
#define WIDGET_ITEM_DRAW_TEXT          18
#define WIDGET_ITEM_DRAW_THUMB         19
#define WIDGET_ITEM_DRAW_TICKS         20
#define WIDGET_ITEM_GET_BORDERSIZE_B   21
#define WIDGET_ITEM_GET_BORDERSIZE_L   22
#define WIDGET_ITEM_GET_BORDERSIZE_R   23
#define WIDGET_ITEM_GET_BORDERSIZE_T   24
#define WIDGET_ITEM_GET_BUTTONSIZE     25
#define WIDGET_ITEM_GET_XSIZE          26
#define WIDGET_ITEM_GET_YSIZE          27
#define WIDGET_ITEM_GET_RADIUS         28
#define WIDGET_ITEM_APPLY_PROPS        29  // Not to be documented. Use this message identifier to update the
                                           // properties of attached widgets from <WIDGET>_DrawSkinFlex().
#define WIDGET_DRAW_BACKGROUND         30

#define WIDGET_ITEM_DRAW_BUTTON_U      WIDGET_ITEM_DRAW_BUTTON_R
#define WIDGET_ITEM_DRAW_BUTTON_D      WIDGET_ITEM_DRAW_BUTTON_L

#define WIDGET_DRAW_OVERLAY    WIDGET_ITEM_DRAW_OVERLAY

/*********************************************************************
*
*       Messages
*/
#define WM_WIDGET_SET_EFFECT    (WM_WIDGET + 0)

/*********************************************************************
*
*       Create flags
*/
#define WIDGET_CF_VERTICAL      WIDGET_STATE_VERTICAL
#define WIDGET_CF_HORIZONTAL    WIDGET_STATE_HORIZONTAL

/*********************************************************************
*
*       Flags for stopping motion of widgets on a given axis
*/
#define WIDGET_MOTION_STOP_X     (1 << 8)
#define WIDGET_MOTION_STOP_Y     (1 << 9)

/*********************************************************************
*
*        Widget object
*
* The widget object is the base class for most widgets
*/
typedef struct {
  int EffectSize;
  void (* pfDrawUp)      (void);
  void (* pfDrawUpRect)  (const GUI_RECT * pRect);
  void (* pfDrawDown)    (void);
  void (* pfDrawDownRect)(const GUI_RECT * pRect);
  void (* pfDrawFlat)    (void);
  void (* pfDrawFlatRect)(const GUI_RECT * pRect);
} WIDGET_EFFECT;

typedef struct {
  WM_Obj      Win;
  const WIDGET_EFFECT* pEffect;
  I16 Id;
  U16 State;
  #if GUI_DEBUG_LEVEL > 1
    U32 DebugId;
  #endif  
} WIDGET;


/*********************************************************************
*
*         GUI_DRAW
*
* The GUI_DRAW object is used as base class for selfdrawing,
* non-windows objects. They are used as content of different widgets,
* such as the bitmap or header widgets.
*/
/* Declare Object struct */
typedef struct GUI_DRAW GUI_DRAW;
typedef void   GUI_DRAW_SELF_CB (WM_HWIN hWin);
typedef WM_HMEM GUI_DRAW_HANDLE;

/* Declare Object constants (member functions etc)  */
typedef struct {
  void (* pfDraw)    (GUI_DRAW_HANDLE hDrawObj, WM_HWIN hObj, int x, int y);
  int  (* pfGetXSize)(GUI_DRAW_HANDLE hDrawObj);
  int  (* pfGetYSize)(GUI_DRAW_HANDLE hDrawObj);
} GUI_DRAW_CONSTS;

/* Declare Object */
struct GUI_DRAW {
  const GUI_DRAW_CONSTS * pConsts;
  union {
    const void * pData;
    GUI_DRAW_SELF_CB * pfDraw;
  } Data;
  I16 xOff, yOff;
};

/* GUI_DRAW_ API */
void GUI_DRAW__Draw     (GUI_DRAW_HANDLE hDrawObj, WM_HWIN hObj, int x, int y);
int  GUI_DRAW__GetXOff  (GUI_DRAW_HANDLE hDrawObj);
int  GUI_DRAW__GetYOff  (GUI_DRAW_HANDLE hDrawObj);
int  GUI_DRAW__GetXSize (GUI_DRAW_HANDLE hDrawObj);
int  GUI_DRAW__GetYSize (GUI_DRAW_HANDLE hDrawObj);
void GUI_DRAW__SetOffset(GUI_DRAW_HANDLE hDrawObj, int xOff, int yOff);

void GUI_DrawStreamedEnableAuto(void);

/* GUI_DRAW_ Constructurs for different objects */
WM_HMEM GUI_DRAW_BITMAP_Create     (const GUI_BITMAP * pBitmap, int x, int y);
WM_HMEM GUI_DRAW_BMP_Create        (const void * pBMP, int x, int y);
WM_HMEM GUI_DRAW_STREAMED_Create   (const GUI_BITMAP_STREAM * pBitmap, int x, int y);
WM_HMEM GUI_DRAW_SELF_Create       (GUI_DRAW_SELF_CB * pfDraw, int x, int y);
WM_HMEM GUI_DRAW_BITMAP_HQHR_Create(const GUI_BITMAP * pBitmap, int x, int y);

#if (GUI_SUPPORT_MEMDEV == 1)
  void GUI_MEMDEV_DrawBitmapObj32HQHR  (GUI_DRAW_HANDLE hDrawObj, WM_HWIN hWin, int x0HR, int y0HR);  // This function uses parameter which are only available when Widgets and WM are available
#endif

/*********************************************************************
*
*       Global data
*
**********************************************************************
*/
extern const WIDGET_EFFECT WIDGET_Effect_3D;
extern const WIDGET_EFFECT WIDGET_Effect_3D1L;
extern const WIDGET_EFFECT WIDGET_Effect_3D2L;
extern const WIDGET_EFFECT WIDGET_Effect_None;
extern const WIDGET_EFFECT WIDGET_Effect_Simple;

/*********************************************************************
*
*       Internal API routines
*
**********************************************************************
*/
void      WIDGET__ConvertScrollState_PixelToLine(WM_HWIN hObj, const WIDGET_SCROLLSTATE_API * pAPI, int StateVPixelwise, int * pStateVLinewise, int * pMotionOffsetY);
int       WIDGET__ConvertScrollState_LineToPixel(WM_HWIN hObj, const WIDGET_SCROLLSTATE_API * pAPI, int StateVLinewise, int MotionOffsetY);
void      WIDGET__DrawFocusRect                 (WIDGET * pWidget, const GUI_RECT * pRect, int Dist);
void      WIDGET__DrawHLine                     (WIDGET * pWidget, int y, int x0, int x1);
void      WIDGET__DrawTriangle                  (WIDGET * pWidget, int x, int y, int Size, int Inc);
void      WIDGET__DrawVLine                     (WIDGET * pWidget, int x, int y0, int y1);
void      WIDGET__EFFECT_DrawDownRect           (WIDGET * pWidget, GUI_RECT * pRect);
void      WIDGET__EFFECT_DrawDown               (WIDGET * pWidget);
void      WIDGET__EFFECT_DrawUpRect             (WIDGET * pWidget, GUI_RECT * pRect);
void      WIDGET__EnableMotion                  (WM_HWIN hObj, int Flags, WIDGET_ENABLE_MOTION_CONTEXT * pContext);
void      WIDGET__FillRectEx                    (WIDGET * pWidget, const GUI_RECT * pRect);
int       WIDGET__GetWindowSizeX                (WM_HWIN hWin);
GUI_COLOR WIDGET__GetBkColor                    (WM_HWIN hObj);
int       WIDGET__GetXSize                      (const WIDGET * pWidget);
int       WIDGET__GetYSize                      (const WIDGET * pWidget);
void      WIDGET__GetClientRect                 (WIDGET * pWidget, GUI_RECT * pRect);
void      WIDGET__GetInsideRect                 (WIDGET * pWidget, GUI_RECT * pRect);
U32       WIDGET__HandleMotionMove              (WM_HWIN hObj, WM_MOTION_INFO * pInfo, const WIDGET_SCROLLSTATE_API * pAPI);
void      WIDGET__HandleMotionMoveBack          (WM_HWIN hObj, const WIDGET_SCROLLSTATE_API * pAPI);
void      WIDGET__Init                          (WIDGET * pWidget, int Id, U16 State);
void      WIDGET__RotateRect90                  (WIDGET * pWidget, GUI_RECT * pDest, const GUI_RECT * pRect);
void      WIDGET__SetScrollState                (WM_HWIN hWin, const WM_SCROLL_STATE * pVState, const WM_SCROLL_STATE * pHState);
void      WIDGET__FillStringInRect              (const char * pText, const GUI_RECT * pFillRect, const GUI_RECT * pTextRectMax, const GUI_RECT * pTextRectAct);
void      WIDGET__FillStringInRectEx            (const char * pText, const GUI_RECT * pFillRect, const GUI_RECT * pTextRectMax, const GUI_RECT * pTextRectAct, int xOffset);

//
// Function pointers for drawing streamed bitmaps
//
extern void (* GUI__pfDrawStreamedBitmap)  (const void * p, int x, int y);
extern int  (* GUI__pfDrawStreamedBitmapEx)(GUI_DTA_GET_DATA_FUNC * pfGetData, const void * p, int x, int y);

/*********************************************************************
*
*       API routines
*
**********************************************************************
*/
void  WIDGET_SetState          (WM_HWIN hObj, int State);
void  WIDGET_AndState          (WM_HWIN hObj, int State);
void  WIDGET_OrState           (WM_HWIN hObj, int State);
int   WIDGET_HandleActive      (WM_HWIN hObj, WM_MESSAGE* pMsg);
int   WIDGET_GetState          (WM_HWIN hObj);
int   WIDGET_SetWidth          (WM_HWIN hObj, int Width);
void  WIDGET_SetFocusable      (WM_HWIN hObj, int State);
U32   WIDGET_GetType           (WM_HWIN hObj);
U16   WIDGET_SetMinTimePerFrame(U16 MinTimePerFrame);
U16   WIDGET_GetMinTimePerFrame(void);

void  WIDGET_EFFECT_3D_DrawUp(void);

const WIDGET_EFFECT* WIDGET_SetDefaultEffect(const WIDGET_EFFECT* pEffect);

void  WIDGET_SetEffect              (WM_HWIN hObj, const WIDGET_EFFECT* pEffect);

const WIDGET_EFFECT* WIDGET_GetDefaultEffect(void);

void WIDGET_EFFECT_3D_SetColor    (unsigned Index, GUI_COLOR Color);
void WIDGET_EFFECT_3D1L_SetColor  (unsigned Index, GUI_COLOR Color);
void WIDGET_EFFECT_3D2L_SetColor  (unsigned Index, GUI_COLOR Color);
void WIDGET_EFFECT_Simple_SetColor(unsigned Index, GUI_COLOR Color);

GUI_COLOR WIDGET_EFFECT_3D_GetColor    (unsigned Index);
GUI_COLOR WIDGET_EFFECT_3D1L_GetColor  (unsigned Index);
GUI_COLOR WIDGET_EFFECT_3D2L_GetColor  (unsigned Index);
GUI_COLOR WIDGET_EFFECT_Simple_GetColor(unsigned Index);

int WIDGET_EFFECT_3D_GetNumColors(void);
int WIDGET_EFFECT_3D1L_GetNumColors(void);
int WIDGET_EFFECT_3D2L_GetNumColors(void);
int WIDGET_EFFECT_Simple_GetNumColors(void);

/*********************************************************************
*
*       Compatibility macros
*
**********************************************************************
*/
#define WIDGET_EnableStreamAuto() GUI_DrawStreamedEnableAuto()

#define WIDGET_SetDefaultEffect_3D()     WIDGET_SetDefaultEffect(&WIDGET_Effect_3D)
#define WIDGET_SetDefaultEffect_3D1L()   WIDGET_SetDefaultEffect(&WIDGET_Effect_3D1L)
#define WIDGET_SetDefaultEffect_3D2L()   WIDGET_SetDefaultEffect(&WIDGET_Effect_3D2L)
#define WIDGET_SetDefaultEffect_None()   WIDGET_SetDefaultEffect(&WIDGET_Effect_None)
#define WIDGET_SetDefaultEffect_Simple() WIDGET_SetDefaultEffect(&WIDGET_Effect_Simple)

#endif /* GUI_WINSUPPORT */

#if defined(__cplusplus)
  }
#endif

#endif   /* SLIDER_H */

/*************************** End of file ****************************/
