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
File        : AppWizard.h
Purpose     : Interface of AppWizard
---------------------------END-OF-HEADER------------------------------
*/

#ifndef APPWIZARD_H
#define APPWIZARD_H

#include <stddef.h>

#include "DIALOG.h"

#include "BUTTON_Private.h"
#include "WINDOW_Private.h"
#include "IMAGE_Private.h"
#include "TEXT_Private.h"
#include "SLIDER_Private.h"
#include "ROTARY_Private.h"
#include "SWITCH_Private.h"
#include "EDIT_Private.h"
#include "QRCODE_Private.h"
#include "GAUGE_Private.h"
#include "KEYBOARD_Private.h"
#include "MULTIEDIT_Private.h"
#include "DROPDOWN_Private.h"
#include "LISTVIEW_Private.h"
#include "LISTBOX_Private.h"
#include "WHEEL_Private.h"
#include "MOVIE_Private.h"
#include "RADIO_Private.h"
#include "CHECKBOX_Private.h"

#if (GUI_WINSUPPORT && GUI_SUPPORT_MEMDEV && WM_SUPPORT_TRANSPARENCY && GUI_SUPPORT_ROTATION)

#if defined(__cplusplus)
  extern "C" {             // Make sure we have C-declarations in C++ programs
#endif

/*####################################################################
#
#       Defines & enums
#
######################################################################
*/
//
// Dispose flags (coordinates)
//
#define APPW_DISPOSE_FLAG_X0 (1 << 0)
#define APPW_DISPOSE_FLAG_Y0 (1 << 1)
#define APPW_DISPOSE_FLAG_X1 (1 << 2)
#define APPW_DISPOSE_FLAG_Y1 (1 << 3)

#define APPW_DISPOSE_DONE (APPW_DISPOSE_FLAG_X0 | APPW_DISPOSE_FLAG_Y0 | APPW_DISPOSE_FLAG_X1 | APPW_DISPOSE_FLAG_Y1)

#define APPW_DISPOSE_FLAG_ISROOT    (1 << 0)

//
// Manage screens as persistent
//
#define APPW_STATE_PERSISTENT (1 << 0)

//
// Interaction flag(s)
//
#define APPW_ACTION_FLAG_DONTGETVALUEFROMEMITTER (1UL << 16)
#define APPW_ACTION_FLAG_DEACTIVATED             (1UL << 17)

//
// Dispose modes
//
#define DISPOSE_MODE_NULL        0  // No dispose information
#define DISPOSE_MODE_REL_DISPLAY 1  // Relative to display
#define DISPOSE_MODE_REL_PARENT  2  // Relative to parent
#define DISPOSE_MODE_REL_SIBLING 3  // Relative to sibling (opposite edge)
#define DISPOSE_MODE_REL_PARTNER 4  // Relative to sibling (same edge)
// The following 4 modes are currently not supported by the AppWizard:
#define DISPOSE_MODE_MID_DISPLAY 5  // Relative to center of display
#define DISPOSE_MODE_MID_PARENT  6  // Relative to center of parent
#define DISPOSE_MODE_TAB_DISPLAY 7  // Tabular layout relative to display
#define DISPOSE_MODE_TAB_PARENT  8  // Tabular layout relative to parent

//
// Dispose indices
//
#define DISPOSE_INDEX_X0 0
#define DISPOSE_INDEX_Y0 1
#define DISPOSE_INDEX_X1 2
#define DISPOSE_INDEX_Y1 3
#define DISPOSE_INDEX_MX 4  // Not used for disposing
#define DISPOSE_INDEX_MY 5  // Not used for disposing

//
// Edges
//
#define APPW_EDGE_LEFT   0
#define APPW_EDGE_RIGHT  1
#define APPW_EDGE_TOP    2
#define APPW_EDGE_BOTTOM 3

//
// Messages
//
#define APPW_MSG_GET_DISPOSE    (APPW_MSG +  0)
#define APPW_MSG_GET_FUNCSETUP  (APPW_MSG +  1)
#define APPW_MSG_SET_LANG       (APPW_MSG +  2)
#define APPW_MSG_GET_JOBS       (APPW_MSG +  3)
#define APPW_MSG_SETUP_FINISHED (APPW_MSG +  4)
#define APPW_MSG_GET_STATE      (APPW_MSG +  5)
#define APPW_MSG_SET_VALUE      (APPW_MSG +  6)
#define APPW_MSG_SET_VAR        (APPW_MSG +  7)
#define APPW_MSG_SET_TEXT       (APPW_MSG +  8)
#define APPW_MSG_DISPOSE_DONE   (APPW_MSG +  9)
#define APPW_MSG_GET_VALUE      (APPW_MSG + 10)
#define APPW_MSG_GET_TEXT       (APPW_MSG + 11)
#define APPW_MSG_GET_FOCUSRECT  (APPW_MSG + 12)
#define APPW_MSG_GET_TYPE       (APPW_MSG + 13)
#define APPW_MSG_SET_DRAW       (APPW_MSG + 14)
#define APPW_MSG_GET_CONTENT    (APPW_MSG + 15)
#define APPW_MSG_GET_PROP       (APPW_MSG + 16)
#define APPW_MSG_GET_ITEM       (APPW_MSG + 17)

//
// Draw modes
//
#define APPW_DRAW_MODE_FILL  0
#define APPW_DRAW_MODE_GRADH 1
#define APPW_DRAW_MODE_GRADV 2

//
// Swiping modes
//
#define APPW_SWIPE_REPLACE  0
#define APPW_SWIPE_DISCLOSE 1
#define APPW_SWIPE_CUSTOM   2

//
// Off, On, Toggle
//
#define APPW_SET_OFF    0
#define APPW_SET_ON     1
#define APPW_SET_TOGGLE 2

//
// Please do not change existing values because they are used to determine the font type when reading XBF fonts!
//
#define MODE_FONT_1BPP     0       // Monochrome mode
#define MODE_FONT_2BPP     1       // 4  gray scales for antialiased fonts
#define MODE_FONT_4BPP     2       // 16 gray scales for antialiased fonts
#define MODE_FONT_1EXT     3       // Monochrome mode with extended character information
#define MODE_FONT_1FRM     4       // Monochrome mode with extended character information and frame
#define MODE_FONT_2BPP_EXT 5       // 4  gray scales for antialiased fonts with extended character information
#define MODE_FONT_4BPP_EXT 6       // 16 gray scales for antialiased fonts with extended character information

//
// Modes of Text-object
//
#define APPW_TEXTMODE_TEXT 0
#define APPW_TEXTMODE_DEC  1
#define APPW_TEXTMODE_HEX  2

//
// Modes of Edit-object
//
#define APPW_EDITMODE_TEXT 0
#define APPW_EDITMODE_DEC  1

//
// Flags for Listview-object
//
#define APPW_FLAGS_LISTVIEW_SHOWGRID_HEADER (1 << 0)
#define APPW_FLAGS_LISTVIEW_SHOWGRID_H      (1 << 1)
#define APPW_FLAGS_LISTVIEW_SHOWGRID_V      (1 << 2)

//
// Text state
//
#define APPW_STATE_TEXT_INVALID  (1U << 0)
#define APPW_STATE_TEXT_HASTEXT  (1U << 1)
#define APPW_STATE_TEXT_HASVALUE (1U << 2)

//
// Listview compare modes
//
#define APPW_LISTVIEW_COMPARE_NONE 0
#define APPW_LISTVIEW_COMPARE_TEXT 1
#define APPW_LISTVIEW_COMPARE_DEC  2
#define APPW_LISTVIEW_COMPARE_USER 3

//
// Conditions
//
#define APPW_IS_VAL 0  // Constant
#define APPW_IS_VAR 1  // Variable
#define APPW_IS_OBJ 2  // Object

//
// Modes of file access
//
#define APPW_IMAGE_MODE_INTERN 0
#define APPW_IMAGE_MODE_EXTERN 1

//
// Spy jobs
//
#define APPW_JOB_REQUEST_OBJECTS   (JOB_QUIT + 1)
#define APPW_JOB_REQUEST_VARIABLES (JOB_QUIT + 2)
#define APPW_JOB_REQUEST_TIME      (JOB_QUIT + 3)
#define APPW_JOB_SEND_PID          (JOB_QUIT + 4)
#define APPW_JOB_SEND_KEY          (JOB_QUIT + 5)

//
// Macros
//
#ifndef GENERATE_JOB
  #define GENERATE_JOB(Action, pFunc, HasReceiver) { Action, (int(*)(WM_HWIN, WM_HWIN, APPW_PARA_ITEM *, int))pFunc, HasReceiver }
#endif

//
// Object types
//
enum {
  TYPE_OBJECT_GROUP = -1,
  TYPE_OBJECT_ROOT,
  TYPE_OBJECT_WINDOW,
  TYPE_OBJECT_BOX,
  TYPE_OBJECT_BUTTON,
  TYPE_OBJECT_IMAGE,
  TYPE_OBJECT_TEXT,
  TYPE_OBJECT_SLIDER,
  TYPE_OBJECT_ROTARY,
  TYPE_OBJECT_SWITCH,
  TYPE_OBJECT_EDIT,
  TYPE_OBJECT_QRCODE,
  TYPE_OBJECT_GAUGE,
  TYPE_OBJECT_KEYBOARD,
  TYPE_OBJECT_PROGBAR,
  TYPE_OBJECT_TIMER,
  TYPE_OBJECT_MULTIEDIT,
  TYPE_OBJECT_DROPDOWN,
  TYPE_OBJECT_LISTVIEW,
  TYPE_OBJECT_LISTBOX,
  TYPE_OBJECT_WHEEL,
  TYPE_OBJECT_MOVIE,
  TYPE_OBJECT_RADIO,
  TYPE_OBJECT_CHECKBOX
};

//
// Notification codes
//
enum {
  APPW_NOTIFICATION_INITDIALOG = (WM_NOTIFICATION_WIDGET + 0),
  APPW_NOTIFICATION_CREATE,
  APPW_NOTIFICATION_DELETE,
  APPW_NOTIFICATION_MOTION,
  APPW_NOTIFICATION_ANIMEND,
  APPW_NOTIFICATION_ANIMSTART,
  APPW_NOTIFICATION_TIMER,
  APPW_NOTIFICATION_PIDPRESSED,
  APPW_NOTIFICATION_PIDRELEASED,
  APPW_NOTIFICATION_FIXED,
  APPW_NOTIFICATION_UNPINNED,
  APPW_NOTIFICATION_ROTATED,
  APPW_NOTIFICATION_LANGCHANGED
};

//
// Atom index for start, end and destination
//
enum {
  ATOM_INDEX_START = 0,
  ATOM_INDEX_END,
  ATOM_INDEX_DEST
};

//
// Time index for start and end
//
enum {
  ANIM_INDEX_TS = 0,
  ANIM_INDEX_TE
};

//
// Elementary particles of 'atoms'
//
enum {
  ATOM_OBJECT_VALUE = 0,
  ATOM_VARIABLE,
  ATOM_OBJECT_GEO,
  ATOM_SCREEN_GEO,
  ATOM_OBJECT_PROP,
  ATOM_CONSTANT,
  ATOM_ACTIVE        // WM_GetActiveWindow() is used here, Makes only sense for drawings.
};

//
// Atom geo details
//
enum {
  ATOM_DETAIL_X0 = DISPOSE_INDEX_X0,
  ATOM_DETAIL_Y0 = DISPOSE_INDEX_Y0,
  ATOM_DETAIL_X1 = DISPOSE_INDEX_X1,
  ATOM_DETAIL_Y1 = DISPOSE_INDEX_Y1,
  ATOM_DETAIL_XCENTER,
  ATOM_DETAIL_YCENTER,
  ATOM_DETAIL_XSIZE,
  ATOM_DETAIL_YSIZE
};

//
// Atom property definitions
//
enum {
  ATOM_PROP_NUMCOLS = 0,  // Number of columns
  ATOM_PROP_NUMROWS,      // Number of rows
  ATOM_PROP_NUMFRAMES,    // Number of frames
  ATOM_PROP_VALUE,        // Current value
  ATOM_PROP_RELEASED      // Released item
};

//
// Drawing functions for APPW__aDrawingFunc[]
//
enum {
  /*  0 */ APPW_ID_CLEAR = 0,
  /*  1 */ APPW_ID_COPY_RECT,
  /*  2 */ APPW_ID_DRAW_ARC,
  /*  3 */ APPW_ID_DRAW_CIRCLE,
  /*  4 */ APPW_ID_DRAW_ELLIPSE,
  /*  5 */ APPW_ID_DRAW_GRADIENT_H,
  /*  6 */ APPW_ID_DRAW_GRADIENT_V,
  /*  7 */ APPW_ID_DRAW_GRADIENT_ROUNDED_H,
  /*  8 */ APPW_ID_DRAW_GRADIENT_ROUNDED_V,
  /*  9 */ APPW_ID_DRAW_HLINE,
  /* 10 */ APPW_ID_DRAW_LINE,
  /* 11 */ APPW_ID_DRAW_LINE_REL,
  /* 12 */ APPW_ID_DRAW_LINE_TO,
  /* 13 */ APPW_ID_DRAW_PIE,
  /* 14 */ APPW_ID_DRAW_PIXEL,
  /* 15 */ APPW_ID_DRAW_POINT,
  /* 16 */ APPW_ID_DRAW_RECT,
  /* 17 */ APPW_ID_DRAW_ROUNDED_FRAME,
  /* 18 */ APPW_ID_DRAW_ROUNDED_RECT,
  /* 19 */ APPW_ID_DRAW_VLINE,
  /* 20 */ APPW_ID_ENABLE_ALPHA,
  /* 21 */ APPW_ID_FILL_CIRCLE,
  /* 22 */ APPW_ID_FILL_ELLIPSE,
  /* 23 */ APPW_ID_FILL_RECT,
  /* 24 */ APPW_ID_FILL_ROUNDED_RECT,
  /* 25 */ APPW_ID_INVERT_RECT,
  /* 26 */ APPW_ID_MOVE_REL,
  /* 27 */ APPW_ID_MOVE_TO,
  /* 28 */ APPW_ID_SET_BLENDBKCOLOR,
  /* 29 */ APPW_ID_SET_BLENDBKCOLOREX,
  /* 30 */ APPW_ID_SET_BLENDCOLOR,
  /* 31 */ APPW_ID_SET_BLENDCOLOREX,
  /* 32 */ APPW_ID_SET_BKCOLOR,
  /* 33 */ APPW_ID_SET_COLOR,
  /* 34 */ APPW_ID_SET_PENSIZE,
  /* 35 */ APPW_ID_SET_LINESTYLE,
  /* 36 */ APPW_ID_AA_DISABLE_HIRES,
  /* 37 */ APPW_ID_AA_DRAW_ARC,
  /* 38 */ APPW_ID_AA_DRAW_CIRCLE,
  /* 39 */ APPW_ID_AA_DRAW_LINE,
  /* 40 */ APPW_ID_AA_DRAW_PIE,
  /* 41 */ APPW_ID_AA_DRAW_ROUNDED_FRAME,
  /* 42 */ APPW_ID_AA_ENABLE_HIRES,
  /* 43 */ APPW_ID_AA_FILL_CIRCLE,
  /* 44 */ APPW_ID_AA_FILL_ELLIPSE,
  /* 45 */ APPW_ID_AA_FILL_ROUNDED_RECT,
  /* 46 */ APPW_ID_AA_SET_FACTOR
};

//
// Indices for prev and post draw
//
enum {
  APPW_DRAWING_INDEX_PREV = 0,
  APPW_DRAWING_INDEX_POST
};

//
// Modes for source object of APPW_JOB_SETITEM, APPW_JOB_ADDITEM and APPW_JOB_INSITEM
//
enum {
  APPW_ITEM_MODE_UNDEFINED = 0,
  APPW_ITEM_MODE_RESOURCE,
  APPW_ITEM_MODE_OBJECT,
};

//
// Jobs of interactions
//
enum {
//--------------------------------------------------------------------------------
// Value                               Purpose
//--------------------------------------------------------------------------------
  APPW_JOB_NULL = -1,
  /*  1 */ APPW_JOB_SETENABLE = 1,  // (pPara + 0)->v     - 1 = On, 0 = Off
  /*  2 */ APPW_JOB_SETVIS,         // (pPara + 0)->v     - 1 = On, 0 = Off
  /*  3 */ APPW_JOB_ANIMCOORD,      // (pPara + 0)->v     - End value
                                    // (pPara + 1)->v     - Index of coordinate
                                    // (pPara + 2)->pFunc - Ease func
                                    // (pPara + 3)->v     - Period
  /*  4 */ APPW_JOB_ANIMVALUE,      // (pPara + 0)->v     - End value
                                    // (pPara + 1)->pFunc - Ease func
                                    // (pPara + 2)->v     - Period
  /*  5 */ APPW_JOB_ANIMRANGE,      // (pPara + 0)->v     - Start value
                                    // (pPara + 1)->v     - End value
                                    // (pPara + 2)->pFunc - Ease func
                                    // (pPara + 3)->v     - Period
  /*  6 */ APPW_JOB_SETCOORD,       // (pPara + 0)->v     - Value
                                    // (pPara + 1)->v     - Index of coordinate
  /*  7 */ APPW_JOB_SETCOLOR,       // (pPara + 0)->v     - Color to be used
                                    // (pPara + 1)->v     - Index of color
  /*  8 */ APPW_JOB_SETBKCOLOR,     // (pPara + 0)->v     - BkColor to be used
  /*  9 */ APPW_JOB_ADDVALUE,       // (pPara + 0)->v     - Value to be added
  /* 10 */ APPW_JOB_SETVALUE,       // (pPara + 0)->v     - Value to be set
  /* 11 */ APPW_JOB_SETPERIOD,      // (pPara + 0)->v     - Value to be set
  /* 12 */ APPW_JOB_SETSCALE,       // (pPara + 0)->v     - Value to be set
  /* 13 */ APPW_JOB_SETANGLE,       // (pPara + 0)->v     - Value to be set
  /* 14 */ APPW_JOB_SETALPHA,       // (pPara + 0)->v     - Value to be set
  /* 15 */ APPW_JOB_SETSIZE,        // (pPara + 0)->v     - Value to be used
                                    // (pPara + 1)->v     - Index of axis
  /* 16 */ APPW_JOB_SETFONT,        // (pPara + 0)->v     - Number of items
                                    // (pPara + 0)->p     - Pointer to first APPW_PARA_ITEM required for font creation
                                    // (pPara + 0)->pFunc - Source file names separated by semicolon (=== In AppWizard application only ===)
  /* 17 */ APPW_JOB_SHIFTSCREEN,    // (pPara + 0)->v     - Screen Id
                                    // (pPara + 1)->v     - Index of edge
                                    // (pPara + 2)->pFunc - Ease func
                                    // (pPara + 3)->v     - Period
                                    // (pPara + 4)->v     - Disclose
  /* 18 */ APPW_JOB_SHIFTWINDOW,    // (pPara + 0)->v     - Window Id
                                    // (pPara + 1)->v     - Index of edge
                                    // (pPara + 2)->pFunc - Ease func
                                    // (pPara + 3)->v     - Period
                                    // (pPara + 4)->v     - Disclose
  /* 19 */ APPW_JOB_SET,            // === NO PARAMETERS ===
  /* 20 */ APPW_JOB_CLEAR,          // === NO PARAMETERS ===
  /* 21 */ APPW_JOB_TOGGLE,         // === NO PARAMETERS ===
  /* 22 */ APPW_JOB_START,          // === NO PARAMETERS ===
  /* 23 */ APPW_JOB_STOP,           // === NO PARAMETERS ===
  /* 24 */ APPW_JOB_SETTEXT,        // (pPara + 0)->v     - Text resource Id (if (pPara + 0)->p == NULL)
                                    // (pPara + 0)->p     - Handle           (if (pPara + 0)->v < 0)
  /* 25 */ APPW_JOB_SHOWSCREEN,     // (pPara + 0)->v     - Screen Id
  /* 26 */ APPW_JOB_SETLANG,        // (pPara + 0)->v     - Index of language 
  /* 27 */ APPW_JOB_SETFOCUS,       // === NO PARAMETERS ===
  /* 28 */ APPW_JOB_ENABLEPID,      // (pPara + 0)->v     - 1 = On, 0 = Off
  /* 29 */ APPW_JOB_CLOSESCREEN,    // (pPara + 0)->v     - Screen Id
  /* 30 */ APPW_JOB_SETX0,          // (pPara + 0)->v     - Value
  /* 31 */ APPW_JOB_SETY0,          // (pPara + 0)->v     - Value
  /* 32 */ APPW_JOB_SETX1,          // (pPara + 0)->v     - Value
  /* 33 */ APPW_JOB_SETY1,          // (pPara + 0)->v     - Value
  /* 34 */ APPW_JOB_MODALMESSAGE,   // (pPara + 0)->v     - Screen Id
  /* 35 */ APPW_JOB_CALC,           // === NO PARAMETERS ===
  /* 36 */ APPW_JOB_ANIMCREATE,     // (pPara + 0)->p     - GUI: Pointer to APPW_ANIM_DATA structure, AppWizard: NULL
                                    // (pPara + 0)->v     - GUI: 0,                                   AppWizard: Animation Id
  /* 37 */ APPW_JOB_ANIMSTOP,       // (pPara + 0)->v     - Animation Id
                                    // (pPara + 1)->v     - 1 = Delete animation, 0 = Remain animation
  /* 38 */ APPW_JOB_ANIMSTART,      // (pPara + 0)->v     - Animation Id
                                    // (pPara + 1)->v     - Number of loops (<0 = endless)
  /* 39 */ APPW_JOB_SETBITMAP,      // (pPara + 0)->v     - Index
                                    // (pPara + 1)->p     - INT: Pointer to Image-Data / EXT: Pointer to Image-Filename (EXT)
                                    // (pPara + 1)->v     - 0 (INT) / 1 (EXT)
                                    // (pPara + 2)->v     - FileSize
                                    // (pPara + 3)->p     - Source file name (=== In AppWizard application only ===)
                                    // (pPara + 3)->v     - If set to 1 the source file name is in const memory
  /* 40 */ APPW_JOB_SETSTART,       // (pPara + 0)->v     - Value
  /* 41 */ APPW_JOB_SETEND,         // (pPara + 0)->v     - Value
  /* 42 */ APPW_JOB_INVALIDATE,     // === NO PARAMETERS ===
  /* 43 */ APPW_JOB_SETITEM,        // (pPara + 0)->v     - Type: 0 = text resource, 1 = object
                                    // (pPara + 1)->v     - 0: Text resource Id, 1: HB/LB: screen Id/object Id
                                    // (pPara + 2)->v     - Source: Row index    (Listview, Dropdown, Listbox)
                                    // (pPara + 3)->v     - Source: Column index (Listview only)
                                    // (pPara + 4)->v     - Target: Row index    (Listview, Dropdown, Listbox)
                                    // (pPara + 5)->v     - Target: Column index (Listview only)
  /* 44 */ APPW_JOB_ADDITEM,        // (pPara + 0)->v     - Type: 0 = text resource, 1 = object
                                    // (pPara + 1)->v     - 0: Text resource Id, 1: HB/LB: screen Id/object Id
                                    // (pPara + 2)->v     - Source: Row index    (Listview, Dropdown, Listbox)
                                    // (pPara + 3)->v     - Source: Column index (Listview only)
                                    // (pPara + 4)->v     - Target: Column index (Listview only)
  /* 45 */ APPW_JOB_INSITEM,        // (pPara + 0)->v     - Type: 0 = text resource, 1 = object
                                    // (pPara + 1)->v     - 0: Text resource Id, 1: HB/LB: screen Id/object Id
                                    // (pPara + 2)->v     - Source: Row index    (Listview, Dropdown, Listbox)
                                    // (pPara + 3)->v     - Source: Column index (Listview only)
                                    // (pPara + 4)->v     - Target: Row index    (Listview, Dropdown, Listbox)
                                    // (pPara + 5)->v     - Target: Column index (Listview only)
  /* 46 */ APPW_JOB_DELITEM,        // (pPara + 0)->v     - Item index to be deleted
  /* 47 */ APPW_JOB_MOVETO,         // (pPara + 0)->v     - Value
  /* 48 */ APPW_JOB_SETRANGE,       // (pPara + 0)->v     - Start value
                                    // (pPara + 1)->v     - End value
  /* 49 */ APPW_JOB_ROTATEDISPLAY   // (pPara + 0)->v     - Rotation command (0, CW, CCW, 180, LEFT, RIGHT) to be used
};

//
// Commands for setting properties
//
enum {
  /*  0 */ APPW_SET_PROP_COLOR = 0   ,
  /*  1 */ APPW_SET_PROP_SCALE       ,
  /*  2 */ APPW_SET_PROP_ALPHA       ,
  /*  3 */ APPW_SET_PROP_ANGLE       ,
  /*  4 */ APPW_SET_PROP_OPAQUE      ,
  /*  5 */ APPW_SET_PROP_TILE        ,
  /*  6 */ APPW_SET_PROP_TOGGLE      ,
  /*  7 */ APPW_SET_PROP_ALIGNTEXT   ,
  /*  8 */ APPW_SET_PROP_ALIGNBITMAP ,
  /*  9 */ APPW_SET_PROP_ALIGNBITMAP2,
  /* 10 */ APPW_SET_PROP_ALIGNBITMAP3,
  /*  1 */ APPW_SET_PROP_SBITMAP     ,
  /*  2 */ APPW_SET_PROP_JPEG        ,
  /*  3 */ APPW_SET_PROP_BMP         ,
  /*  4 */ APPW_SET_PROP_GIF         ,
  /*  5 */ APPW_SET_PROP_MOVIE       ,
  /*  6 */ APPW_SET_PROP_DECMODE     ,
  /*  7 */ APPW_SET_PROP_HEXMODE     ,
  /*  8 */ APPW_SET_PROP_MORPHMODE   ,
  /*  9 */ APPW_SET_PROP_GRADH       ,
  /* 20 */ APPW_SET_PROP_GRADV       ,
  /*  1 */ APPW_SET_PROP_REPEAT      ,
  /*  2 */ APPW_SET_PROP_TEXTID      ,
  /*  3 */ APPW_SET_PROP_SIZE        ,
  /*  4 */ APPW_SET_PROP_GRADH2      ,
  /*  5 */ APPW_SET_PROP_GRADV2      ,
  /*  6 */ APPW_SET_PROP_COLOR2      ,
  /*  7 */ APPW_SET_PROP_COLOR3      ,
  /*  8 */ APPW_SET_PROP_TEXTID2     ,
  /*  9 */ APPW_SET_PROP_COLORS      ,
  /* 30 */ APPW_SET_PROP_COLORS2     ,
  /*  1 */ APPW_SET_PROP_COLORS3     ,
  /*  2 */ APPW_SET_PROP_BKCOLOR     ,
  /*  3 */ APPW_SET_PROP_BKCOLORS    ,
  /*  4 */ APPW_SET_PROP_SBITMAPS    ,
  /*  5 */ APPW_SET_PROP_FONT        ,
  /*  6 */ APPW_SET_PROP_FONT2       ,
  /*  7 */ APPW_SET_PROP_MOTIONH     ,
  /*  8 */ APPW_SET_PROP_MOTIONV     ,
  /*  9 */ APPW_SET_PROP_RANGE       ,
  /* 40 */ APPW_SET_PROP_SPAN        ,
  /*  1 */ APPW_SET_PROP_VALUES      ,
  /*  2 */ APPW_SET_PROP_SLAYOUT     ,
  /*  3 */ APPW_SET_PROP_CONTENT     ,
  /*  4 */ APPW_SET_PROP_SORTCOLS    ,
  /*  5 */ APPW_SET_PROP_OFFSET      ,
  /*  6 */ APPW_SET_PROP_GROUPID     ,
  /*  7 */ APPW_SET_PROP_POS         ,
  /*  8 */ APPW_SET_PROP_VALUE       ,
  /*  9 */ APPW_SET_PROP_PERIOD      ,
  /* 50 */ APPW_SET_PROP_PERIOD2     ,
  /*  1 */ APPW_SET_PROP_PERIOD3     ,
  /*  2 */ APPW_SET_PROP_SNAP        ,
  /*  3 */ APPW_SET_PROP_ALIGN       ,
  /*  4 */ APPW_SET_PROP_RADIUS      ,
  /*  5 */ APPW_SET_PROP_RADIUS2     ,
  /*  6 */ APPW_SET_PROP_FRAME       ,
  /*  7 */ APPW_SET_PROP_FRAME2      ,
  /*  8 */ APPW_SET_PROP_FRAME3      ,
  /*  9 */ APPW_SET_PROP_BORDER      ,
  /* 60 */ APPW_SET_PROP_LENGTH      ,
  /*  1 */ APPW_SET_PROP_HEIGHT      ,
  /*  2 */ APPW_SET_PROP_HEIGHT2     ,
  /*  3 */ APPW_SET_PROP_FIXED       ,
  /*  4 */ APPW_SET_PROP_SPACING     ,
  /*  5 */ APPW_SET_PROP_ROTATION    ,
  /*  6 */ APPW_SET_PROP_ECCLEVEL    ,
  /*  7 */ APPW_SET_PROP_VERSION     ,
  /*  8 */ APPW_SET_PROP_FOCUSABLE   ,
  /*  9 */ APPW_SET_PROP_MOTION      ,
  /* 70 */ APPW_SET_PROP_INVERT      ,
  /*  1 */ APPW_SET_PROP_VERTICAL    ,
  /*  2 */ APPW_SET_PROP_HORIZONTAL  ,
  /*  3 */ APPW_SET_PROP_ENDLESS     ,
  /*  4 */ APPW_SET_PROP_PERSISTENT  ,
  /*  5 */ APPW_SET_PROP_ROTATE      ,
  /*  6 */ APPW_SET_PROP_FADE        ,
  /*  7 */ APPW_SET_PROP_WRAP        ,
  /*  8 */ APPW_SET_PROP_ROUNDEDVAL  ,
  /*  9 */ APPW_SET_PROP_ROUNDEDEND  ,
  /* 80 */ APPW_SET_PROP_OVERWRITE   ,
  /*  1 */ APPW_SET_PROP_AUTORESTART ,
  /*  2 */ APPW_SET_PROP_LQ          ,
  /*  3 */ APPW_SET_PROP_STAYONTOP   ,
  /*  4 */ APPW_SET_PROP_UNTOUCHABLE ,
  /*  5 */ APPW_SET_PROP_PWMODE      ,
  /*  6 */ APPW_SET_PROP_ROMODE      ,
  /*  7 */ APPW_SET_PROP_CELLSELECT  ,
  /*  8 */ APPW_SET_PROP_VISIBLE     ,
  /*  9 */ APPW_SET_PROP_VISIBLE2    ,
  /* 90 */ APPW_SET_PROP_VISIBLE3    ,
  /*  1 */ APPW_SET_PROP_SWITCHOFF   ,
  /*  2 */ APPW_SET_PROP_3STATE      ,
  /*  3 */ APPW_SET_PROP_FIXED2      ,
  /*  4 */ APPW_SET_PROP_WHEELTEXT   ,
  /*  5 */ APPW_SET_PROP_WHEELBITMAPS,
  /*  6 */ APPW_SET_PROP_SCROLLERH   ,
  /*  7 */ APPW_SET_PROP_SCROLLERV   ,
};

//
// Bitmap indices
//
#define APPW_BI_UNPRESSED 0
#define APPW_BI_PRESSED   1
#define APPW_BI_DISABLED  2

#define APPW_MAX_RANGE    0xFFFFu

//
// Use of parameter
//
#define ARG_V(v)             { (U32)v, (const void *)0, (void (*)(void))NULL  }
#define ARG_P(p)             { (U32)0, (const void *)p, (void (*)(void))NULL  }
#define ARG_VP(v, p)         { (U32)v, (const void *)p, (void (*)(void))NULL  }
#define ARG_VPF(v, p, pFunc) { (U32)v, (const void *)p, (void (*)(void))pFunc }
#define ARG_F(pFunc)         { (U32)0, (const void *)0, (void (*)(void))pFunc }

//
// Distinguishing between different possible text sources in APPW_GetLockedText()
//
#define APPW_USE_TEXT_HANDLE (-2)
#define APPW_USE_OBJECT_ID   (-3)

//
// Internal configuration macros
//
#ifndef   APPW_SETUP_ITEM_EXT
  #define APPW_SETUP_ITEM_EXT
#endif

#ifndef   APPW_ACTION_ITEM_EXT
  #define APPW_ACTION_ITEM_EXT
#endif

#ifndef   APPW_JOB_EXT
  #define APPW_JOB_EXT
#endif

//
// Internal macros
//
#define MANAGE_GET_DISPOSE_FUNCSETUP_JOBS(OBJECT_TYPE)                                    \
  case APPW_MSG_GET_DISPOSE:                                                              \
    pMsg->Data.pData = (void *)&((WM_##OBJECT_TYPE *)GUI_ALLOC_h2p(pMsg->hWin))->Dispose; \
    break;                                                                                \
  case APPW_MSG_GET_TYPE:                                                                 \
    pMsg->Data.v = TYPE_##OBJECT_TYPE;                                                    \
    break;                                                                                \
  case APPW_MSG_GET_FUNCSETUP:                                                            \
    pMsg->Data.pFunc = (void(*)(void))_Setup;                                             \
    break;                                                                                \
  case APPW_MSG_GET_JOBS:                                                                 \
    pMsg->Data.p = (const void *)_aJobs;                                                  \
    pMsg->MsgId  = GUI_COUNTOF(_aJobs);                                                   \
    break

#define MANAGE_GET_DISPOSE_FUNCSETUP(OBJECT_TYPE)                                         \
  case APPW_MSG_GET_DISPOSE:                                                              \
    pMsg->Data.pData = (void *)&((WM_##OBJECT_TYPE *)GUI_ALLOC_h2p(pMsg->hWin))->Dispose; \
    break;                                                                                \
  case APPW_MSG_GET_TYPE:                                                                 \
    pMsg->Data.v = TYPE_##OBJECT_TYPE;                                                    \
    break;                                                                                \
  case APPW_MSG_GET_FUNCSETUP:                                                            \
    pMsg->Data.pFunc = (void(*)(void))_Setup;                                             \
    break

//
// Macros for drawing objects
//
#define MANAGE_PREPAINT(OBJ_CAST)                     \
    pObj = (OBJ_CAST *)GUI_LOCK_H(pMsg->hWin); {      \
      APPW_DrawingPrepare(pObj->aIdDraw, pMsg->hWin); \
    } GUI_UNLOCK_H(pObj)

#define MANAGE_POSTPAINT() APPW_DrawingCleanup()

#define MANAGE_SET_DRAW(OBJ_CAST)                                          \
  case APPW_MSG_SET_DRAW:                                                  \
    pObj = (OBJ_CAST *)GUI_LOCK_H(pMsg->hWin); {                           \
      pObj->aIdDraw[APPW_DRAWING_INDEX_PREV] = (U32)pMsg->Data.v & 0xFFFF; \
      pObj->aIdDraw[APPW_DRAWING_INDEX_POST] = (U32)pMsg->Data.v >> 16;    \
    } GUI_UNLOCK_H(pObj);                                                  \
    break

#define MANAGE_DRAWING(OBJ_CAST) \
  MANAGE_SET_DRAW(OBJ_CAST);     \
  case WM_PRE_PAINT:             \
    MANAGE_PREPAINT(OBJ_CAST);   \
    break;                       \
  case WM_POST_PAINT:            \
    MANAGE_POSTPAINT();          \
    break

//
// Macros for defining drawing items used in exported code only
//
#define APPW_DRAWING_ITEM_DEF_X(Name, NumAtoms) \
static const struct {                           \
  U16                       IndexFunc;          \
  const APPW_DRAWING_ITEM * pNextItem;          \
  APPW_ATOM                 aAtom[NumAtoms];    \
} Name =

#define APPW_DRAWING_ITEM_DEF_0(Name)  \
static const struct {                  \
  U16                       IndexFunc; \
  const APPW_DRAWING_ITEM * pNextItem; \
} Name =

//
// Macros for interface definitions
//
#define DEFAULT_CRCB(Object)                                                             \
  void    WM_OBJECT_##Object##_cb    (WM_MESSAGE * pMsg);                                \
  WM_HWIN WM_OBJECT_##Object##_Create(APPW_DISPOSE * pDispose, WM_HWIN hParent, int Id)

#define DEFAULT_FUNC(Object)                                                             \
  DEFAULT_CRCB(Object);                                                                  \
  int     WM_OBJECT_##Object##_Setup (WM_HWIN hWin, APPW_SETUP_ITEM * pSetup)

/*####################################################################
#
#       Types
#
######################################################################
*/
/*********************************************************************
*
*       APPW_DISPOSE_COORD
*/
typedef struct {
  int Mode;
  int Off;
  int Id;
  int Valid;
} APPW_DISPOSE_COORD;

/*********************************************************************
*
*       APPW_DISPOSE
*/
typedef struct {
  APPW_DISPOSE_COORD aDisposeCoord[4];
  int                xSize;
  int                ySize;
  int                xSizeMin;
  int                ySizeMin;
  U8                 State;
  U32                Flags;
} APPW_DISPOSE;

/*********************************************************************
*
*       APPW_TEXT_INIT
*/
typedef struct {
  const char ** appData;
  int           NumItems;
} APPW_TEXT_INIT;

/*********************************************************************
*
*       APPW_ANIM_PARA
*/
typedef struct  {
  I32                Start;
  I32                End;
  WM_CRITICAL_HANDLE chItem;
  WM_CRITICAL_HANDLE chWinSrc;
  U16                IdSrc;
  U16                IdDst;
  int                RootIndex;
} APPW_ANIM_PARA;

/*********************************************************************
*
*       APPW_ANIM_COORD_PARA (obsolete)
*/
typedef struct {
  APPW_ANIM_PARA AnimPara;
  int            Index;
} APPW_ANIM_COORD_PARA;

/*********************************************************************
*
*       APPW_ANIM_SHIFT_PARA
*/
typedef struct {
  int                EdgeIndex;
  I32                x0, x1;
  I32                y0, y1;
  WM_CRITICAL_HANDLE chWinOld;
  WM_CRITICAL_HANDLE chWinNew;
  APPW_DISPOSE       DisposeOld;
  APPW_DISPOSE       DisposeNew;
  GUI_ANIM_HANDLE    hAnim;
  U8                 Disclose;
} APPW_ANIM_SHIFT_PARA;

/*********************************************************************
*
*       APPW_ATOM
*/
typedef struct {
  U32 Value;   // LW: ObjectGeo | ObjectValue | ObjectProp ? ObjectId : Variable ? VariableId : Value (LW)
               // HW: ObjectGeo | ObjectValue | ScreenGeo | ObjectProp  ? ScreenId : Value (HW)
  I16 Offset;  // ObjectGeo | ScreenGeo | ObjectProp ? Offset : unused
  U8  Item;    // Item specification
  U8  Detail;  // Detail to be used
} APPW_ATOM;

/*********************************************************************
*
*       APPW_ATOM_INFO
*/
typedef struct {
  U16     IdScreen;  // Id of screen
  U16     IdObject;  // Id of object
  U16     IdVar;     // Id of variable
  WM_HWIN hScreen;   // Screen handle
  WM_HWIN hObject;   // Object handle
} APPW_ATOM_INFO;

/*********************************************************************
*
*       APPW_ANIM_ITEM
*/
typedef struct {
  GUI_ANIM_GETPOS_FUNC Ease;      // Ease function
  GUI_TIMER_TIME       aTime[2];  // Timeline coordinates for start and end
  APPW_ATOM            aAtom[3];  // Item definitions for start, end and destination
} APPW_ANIM_ITEM;

/*********************************************************************
*
*       APPW_ANIM_ITEM_INFO
*/
typedef struct {
  I32 Start;               // Used to save the start value of the item
  I32 End;                 // Used to save the end value of the item
  APPW_ANIM_ITEM * pItem;  // Pointer to item
} APPW_ANIM_ITEM_INFO;

/*********************************************************************
*
*       APPW_ANIM_DATA
*/
typedef struct {
  U16                    Id;         // Anmation Id to be used
  GUI_TIMER_TIME         Period;     // Period of complete animation
  U8                     AutoStart;  // If != 0 the animation immediately starts after creation
  int                    NumLoops;   // Number of loops, <0 means endless loop
  int                    NumItems;   // Number of animation items
  const APPW_ANIM_ITEM * pItem;      // Pointer to first item
} APPW_ANIM_DATA;

/*********************************************************************
*
*       APPW_CREATE_ITEM
*/
typedef struct {
  WM_HWIN   (* pfCreate)(APPW_DISPOSE * pDispose, WM_HWIN hParent, int Id);
  int          Id;
  int          IdParent;
  APPW_DISPOSE Dispose;
  U16          aIdDraw[2];
} APPW_CREATE_ITEM;

/*********************************************************************
*
*       APPW_PARA_ITEM
*/
typedef struct {
  U32          v;
  const void * p;
  void      (* pFunc)(void);
} APPW_PARA_ITEM;

/*********************************************************************
*
*       APPW_SETUP_ITEM
*/
typedef struct {
  int            Id;
  unsigned       Command;
  APPW_PARA_ITEM aPara[6];
  APPW_SETUP_ITEM_EXT
} APPW_SETUP_ITEM;

/*********************************************************************
*
*       APPW_COMP_ITEM
*
* Purpose:
*   Comparison item which could be a constant, a variable id or an object id
*/
typedef struct {
  U8  What;
  I32 Value;
} APPW_COMP_ITEM;

/*********************************************************************
*
*       APPW_CALC_ITEM
*
* Purpose:
*   Calculation item which could be a constant, a variable id or an object id
*/
typedef struct {
  U8  What;
  U16 Index;
  I32 Value;
} APPW_CALC_ITEM;

/*********************************************************************
*
*       APPW_COND_COMP
*
* Purpose:
*   Comparison with 2 items to be compared by the given function pointer
*/
typedef struct {
  APPW_ATOM aAtom[2];
  int    (* pFunc)(I32 v0, I32 v1);
} APPW_COND_COMP;

/*********************************************************************
*
*       APPW_COND
*
* Purpose:
*   Condition with pointer to comparison table and a pointer to a
*   term to be evaluated.
*/
typedef struct {
  const char           * pTerm;  // Pointer to term to be calculated
  const APPW_COND_COMP * pComp;  // Pointer to first comparison
  unsigned               nComp;  // Number of comparisons
} APPW_COND;

/*********************************************************************
*
*       APPW_CALC
*
* Purpose:
*   Pointer to a term to be calculated, a pointer to operands to be
*   used and a pointer to an I32 array for the operands values.
*/
typedef struct {
  const char           * pTerm;  // Pointer to term to be calculated
  unsigned               nItem;  // Number of operands
  const APPW_ATOM      * pAtom;  // Pointer to first operand (TBD...)
  I32                  * pVal;   // Array for calculating operand values
} APPW_CALC;

/*********************************************************************
*
*       APPW_ACTION_ITEM
*
* Purpose:
*   Represents an interaction (including parameters) associated to a root object.
*   Each root object contains a list of interactions defining the behavior of the screen.
*/
typedef struct APPW_ACTION_ITEM APPW_ACTION_ITEM;

struct APPW_ACTION_ITEM {
  int               IdSrc;                               // Emitter
  int               NCode;                               // Signal
  int               IdDst;                               // Receiver
  int               IdJob;                               // Job
  void           (* pfSlot)(APPW_ACTION_ITEM * pAction,  // Pointer to action
                            WM_HWIN            hScreen,  // Handle of screen
                            WM_MESSAGE       * pMsg,     // pMsg->hWnd: receiver, pMsg->hWndSrc: emitter
                            int * pResult);              // Pointer to result
  APPW_PARA_ITEM    aPara[6];                            // Optional job specific parameters
  U32               Flags;                               // Bitmask: 0 means -> aPara[x].v = value
                                                         //          1 means -> aPara[x].v = Variable-Id
  const APPW_COND * pCond;                               // Condition to be fulfilled
  APPW_ACTION_ITEM_EXT
};

/*********************************************************************
*
*       APPW_JOB
*
* Purpose:
*   Defines the function to be called for a specific job
*/
typedef struct {
  int          IdJob;                               // Id of job
  APPW_JOB_EXT
  int       (* pFunc)(WM_HWIN          hWinDst,     // Handle of receiver
                      WM_HWIN          hWinSrc,     // Handle of emitter
                      APPW_PARA_ITEM * pPara,       // Parameters to be used
                      int              RootIndex);
  int          HasReceiver;
} APPW_JOB;

/*********************************************************************
*
*       APPW_OBJ_SETUP_T
*
* Purpose:
*   Common type definition of setup function
*/
typedef int APPW_OBJ_SETUP_T(const APPW_SETUP_ITEM * pSetup, WM_HWIN hParent);

/*********************************************************************
*
*       APPW_ROOT_INFO
*
* Purpose:
*   Structure to be passed to APPW_CreateRoot()
*/
typedef struct {
  U16                      RootId;
  APPW_CREATE_ITEM       * pCreateInfo;
  unsigned                 NumCreateInfo;
  const APPW_SETUP_ITEM  * pSetupInfo;
  unsigned                 NumSetupInfo;
  const APPW_ACTION_ITEM * pActionInfo;
  unsigned                 NumActionInfo;
  WM_CALLBACK            * cb;
  WM_HWIN                  hWin;
} APPW_ROOT_INFO;

/*********************************************************************
*
*       APPW_CONTENT
*
* Purpose:
*   Structure for holding the text content, header labels and column size
*   information for text related objects like dropdown, listview and listbox.
*/
typedef struct {
  const char *** pppText;     // Array of pointers:
                              //   <p>pp: - Pointer to an array containing pointers to arrays
                              //            with pointers to zero terminated strings
                              //            containing the text of one column.
                              //   p<p>p: - Pointer to an array containing pointers to zero
                              //            terminated strings for the content of one column.
                              //   pp<p>: - Pointer to zero terminated string.
  int            NumRows;     // Number of rows
  int            NumColumns;  // Number of columns
  const char  ** ppHead;      // Array of pointers to the zero terminated header text strings.
  const U16    * pSize;       // Pointer to an U16 array containing the column sizes.
  const U8     * pHeadAlign;  // Pointer to an U8 array containing the header alignment.
  const U8     * pTextAlign;  // Pointer to an U8 array containing the column alignment.
} APPW_CONTENT;

/*********************************************************************
*
*       APPW_VAR_OBJECT
*
* Purpose:
*   Configurable variables to be used
*/
typedef struct {
  U16               Id;       // Used to address the variables
  U8                IsDirty;  // Is set to 1 if content is 'dirty' and needs to be processed
  U32               Data;     // Could be anything except a pointer
  const APPW_CALC * pCalc;    // Pointer to calculation object containing term and operands
} APPW_VAR_OBJECT;

/*********************************************************************
*
*       APPW_FILE_INFO
*
* Purpose:
*   Structure for holding file name and file handle.
*/
typedef struct {
  const char * pFileName;
  void       * pVoid;
} APPW_FILE_INFO;

/*********************************************************************
*
*       APPW_DFILE_INFO
*
* Purpose:
*   Structure for holding file name and 2 file handles.
*   Using an additional file handle avoids jerking when playing
*   large movies. It is used for accessing the offset table.
*/
typedef struct {
  const char * pFileName;
  void       * apVoid[2];
} APPW_DFILE_INFO;

/*********************************************************************
*
*       APPW_DRAW_OBJECT
*
* Purpose:
*   Structure for drawing bitmaps (streamed or C)
*/
typedef struct {
  void      (* pfDraw)(const void * p, int x, int y);  // Drawing function
  const void * pData;                                  // Data pointer (bitmap) or file name (streamed bitmap)
  int          xSize;                                  // (obvious)
  int          ySize;                                  // (obvious)
  U32          FileSize;                               // (obvious)
} APPW_DRAW_OBJECT;

/*********************************************************************
*
*       APPW_FILEACCESS
*
* Purpose:
*   File access
*/
typedef struct {
  GUI_GET_DATA_FUNC_I      * pfGetDataBGJ;                                                   // Function for getting data of (B)MP, (G)IF and (J)PEG files
  GUI_GET_DATA_FUNC_II     * pfGetDataImage;                                                 // Function for getting data of streamed bitmaps and text resource files
  GUI_XBF_GET_DATA_FUNC    * pfGetDataFont;                                                  // Function for getting data of XBF fonts
  U32                     (* pfGetDataFile)  (void * p, U8 * pData, U32 NumBytes, U32 Off);  // Function for getting data of generic files
  void                  * (* pfOpenFontFile) (const char * pFilename);                       // Opens a file in resource (sub)folder 'Font'
  void                  * (* pfOpenImageFile)(const char * pFilename);                       // Opens a file in resource (sub)folder 'Image'
  void                  * (* pfOpenMovieFile)(const char * pFilename);                       // Opens a file in resource (sub)folder 'Movie'
  void                  * (* pfOpenTextFile) (const char * pFilename);                       // Opens a file in resource (sub)folder 'Text'
  void                    (* pfCloseFile)    (void *);                                       // (obvious)
  U32                     (* pfGetFileSize)  (void *);                                       // (obvious)
} APPW_FILEACCESS;

/*********************************************************************
*
*       APPW_X_FILEACCESS
*
* Purpose:
*   Bare file access
*/
typedef struct {
  void * (* pfOpen   )(const char * pFilename);
  int    (* pfSeek   )(const void * p, U32 Off);
  U32    (* pfRead   )(const void * p, void * pData, U32 NumBytes);
  void   (* pfClose  )(const void * p);
  U32    (* pfGetSize)(const void * p);
} APPW_X_FILEACCESS;

/*********************************************************************
*
*       APPW_MEMACCESS
*
* Purpose:
*   Accessing object data located in memory
*/
typedef struct {
  GUI_GET_DATA_FUNC        * pfGetDataImage;
  GUI_XBF_GET_DATA_FUNC    * pfGetDataFont;
  U32                     (* pfGetDataFile)(void * p, U8 * pData, U32 NumBytes, U32 Off);
} APPW_MEMACCESS;

/*********************************************************************
*
*       APPW_OBJECT_INFO
*
* Purpose:
*   Object info to be transferred by spy
*/
typedef struct {
  WM_WINDOW_INFO Info;
  U16            Id;
  I16            RootIndex;
} APPW_OBJECT_INFO;

/*********************************************************************
*
*       APPW_DRAW_FOCUS
*
* Purpose:
*   Defines how a focus should look like
*/
typedef struct {
  GUI_COLOR Color;   // Color of focus rect
  U8        Radius;  // Radius if required
  U8        Width;   // Width  if required (minimum 1)
} APPW_DRAW_FOCUS_INFO;

/*********************************************************************
*
*       APPW_DRAWING_INFO
*
* Purpose:
*   Info structure containing id, function pointer and number of items of one function
*/
typedef struct {
  void  (* pFunc)(const I32 * pData);
  unsigned NumAtoms;
} APPW_DRAWING_INFO;

/*********************************************************************
*
*       APPW_DRAWING_ITEM
*
* Purpose:
*   One single drawing item with atom pointer containing parameters for functions
*/
typedef struct APPW_DRAWING_ITEM APPW_DRAWING_ITEM;

struct APPW_DRAWING_ITEM {
  U16                       IndexFunc;
  const APPW_DRAWING_ITEM * pNextItem;
  APPW_ATOM                 aAtom[1];  // Further atoms will be attached at the end of the structure
};

/*********************************************************************
*
*       APPW_FONT
*
* Purpose:
*   Font structure and XBF data required to create fonts
*/
typedef struct {
  GUI_FONT     Font;
  GUI_XBF_DATA FontData;
} APPW_FONT;

/*********************************************************************
*
*       APPW_WHEEL_ADDTEXT
*
* Purpose:
*   Structure to be able to pass data APPW_SET_PROP_WHEELTEXT
*/
typedef struct {
  APPW_PARA_ITEM  ParaText;  // Parameters to be used to reference the text content
  APPW_PARA_ITEM  aPara[2];  // Parameters to be used to create the actual font in OWheel
  WHEEL_ITEM_ATTR aAttr[2];  // Alignment parameters and text color
} APPW_WHEEL_ADDTEXT;

/*********************************************************************
*
*       APPW_WHEEL_ADDBITMAPS
*
* Purpose:
*   Structure to be able to pass data on APPW_SET_PROP_WHEELBITMAPS
*/
typedef struct {
  unsigned         aNumItems[2];  // Cell- and center number of draw objects
  APPW_PARA_ITEM * apPara   [2];  // Parameters to be used to create the draw objects in OWheel
  WHEEL_ITEM_ATTR  aAttr    [2];  // Cell- and center alignment parameters and (alpha)image color
} APPW_WHEEL_ADDBITMAPS;

/*********************************************************************
*
*       WHEEL_DRAW_OBJECT
*
* Purpose:
*   Replacement for GUI_DRAW objects
*/
typedef struct {
  GUI_DRAW         GUI_Draw;  // Important: Must be the first item
  APPW_DRAW_OBJECT APP_Draw;  // Additional data required for AppWizard
} WHEEL_DRAW_OBJECT;

/*********************************************************************
*
*       APPW_SCROLLER_DEF
*
* Purpose:
*   Definition structure to be used to create scrollers on demand
*/
typedef struct {
  GUI_COLOR aColor [2];   // Color definitions:  SCROLLER_CI_ACTIVE, SCROLLER_CI_INACTIVE
  U16       aPeriod[3];   // Period definitions: SCROLLER_PI_FADE_IN, SCROLLER_PI_FADE_OUT, SCROLLER_PI_INACTIVE
  I16       aMisc  [5];   // Common propperties: Size, Spacing, Radius, AlignOffset, ThumbSizeMin
  U8        Interactive;  // Scroller is interactive
} APPW_SCROLLER_DEF;

/*####################################################################
#
#       Object definitions
#
######################################################################
*/
/*********************************************************************
*
*       WM_OBJECT_BOX
*/
typedef struct {
  WIDGET              Widget;      // GUI/WM-Widget
  APPW_DISPOSE        Dispose;     // Dispose structure
  U16                 aIdDraw[2];  // Pre- and Post-draw
  unsigned            Mode;
  GUI_COLOR           Color0;
  int                 NumColors;
  int                 Radius;
  GUI_GRADIENT_INFO * pGradVal;
  GUI_HMEM            hGradPix;
} WM_OBJECT_BOX;

/*********************************************************************
*
*       WM_OBJECT_BUTTON
*/
typedef struct {
  BUTTON_Obj          Widget;      // GUI/WM-Widget
  APPW_DISPOSE        Dispose;     // Dispose structure
  U16                 aIdDraw[2];  // Pre- and Post-draw
  U32                 State;
  unsigned            Mode;
  unsigned            Delay;
  unsigned            Period;
  unsigned            Toggle;
  unsigned            AlignBm;
  int                 xOffBm;
  int                 yOffBm;
  APPW_DRAW_OBJECT    apDraw[3];
  GUI_COLOR           aColor[3];
  int                 TextId;
  WM_HTIMER           hTimer;
  U16                 FrameRadius;
  U16                 FrameSize;
} WM_OBJECT_BUTTON;

/*********************************************************************
*
*       WM_OBJECT_IMAGE
*/
typedef struct {
  IMAGE_OBJ           Widget;      // GUI/WM-Widget
  APPW_DISPOSE        Dispose;     // Dispose structure
  U16                 aIdDraw[2];  // Pre- and Post-draw
  unsigned            Mode;
  GUI_COLOR           Color;
  const void        * pData;
  void              * pFile;
} WM_OBJECT_IMAGE;

/*********************************************************************
*
*       WM_OBJECT_ROTARY
*/
typedef struct {
  ROTARY_OBJ          Widget;      // GUI/WM-Widget
  APPW_DISPOSE        Dispose;     // Dispose structure
  U16                 aIdDraw[2];  // Pre- and Post-draw
  APPW_DRAW_OBJECT    apDraw[3];   // 0 = Bk, 1 = Marker, 2 = MarkerHR
  WM_HMEM             hMarkerPara;
} WM_OBJECT_ROTARY;

/*********************************************************************
*
*       WM_OBJECT_SLIDER
*/
typedef struct {
  SLIDER_Obj          Widget;      // GUI/WM-Widget
  APPW_DISPOSE        Dispose;     // Dispose structure
  U16                 aIdDraw[2];  // Pre- and Post-draw
  unsigned            Mode;
  APPW_DRAW_OBJECT    apDraw[4];
  GUI_COLOR           aColor[2];
  GUI_COLOR         * apStretchLine[2];  // Pointer for bitmap pixels to be stretched
  U8                  HasValue;
} WM_OBJECT_SLIDER;

/*********************************************************************
*
*       WM_OBJECT_SWITCH
*/
typedef struct {
  SWITCH_Obj          Widget;      // GUI/WM-Widget
  APPW_DISPOSE        Dispose;     // Dispose structure
  U16                 aIdDraw[2];  // Pre- and Post-draw
  U32                 State;
  APPW_DRAW_OBJECT    apDraw[6];
  int                 aTextId[2];
} WM_OBJECT_SWITCH;

/*********************************************************************
*
*       WM_OBJECT_TEXT
*/
typedef struct {
  TEXT_OBJ            Widget;      // GUI/WM-Widget
  APPW_DISPOSE        Dispose;     // Dispose structure
  U16                 aIdDraw[2];  // Pre- and Post-draw
  U32                 State;
  unsigned            Mode;
  unsigned            AlignText;
  int                 xOffText;
  int                 yOffText;
  int                 TextId;
  //
  // Elements required for decimal mode
  //
  I32                 Value;
  I32                 Min;
  I32                 Max;
  //
  // Parameters for decimal mode
  //
  U8                  Len;
  U8                  Shift;
  U8                  Signed;
  U8                  Space;
} WM_OBJECT_TEXT;

/*********************************************************************
*
*       WM_OBJECT_EDIT
*/
typedef struct {
  EDIT_Obj            Widget;      // GUI/WM-Widget
  APPW_DISPOSE        Dispose;     // Dispose structure
  U16                 aIdDraw[2];  // Pre- and Post-draw
  U32                 State;
  unsigned            Mode;
  U16                 FrameRadius;
  U16                 FrameSize;
  GUI_COLOR           FrameColor;
  int                 IndexText;
  //
  // Elements required for decimal mode
  //
  I32                 Value;
  I32                 Min;
  I32                 Max;
  //
  // Parameters for decimal mode
  //
  U8                  Len;
  U8                  Shift;
  U8                  Signed;
  U8                  Space;
} WM_OBJECT_EDIT;

/*********************************************************************
*
*       WM_OBJECT_QRCODE
*/
typedef struct {
  QRCODE_OBJ          Widget;      // GUI/WM-Widget
  APPW_DISPOSE        Dispose;     // Dispose structure
  U16                 aIdDraw[2];  // Pre- and Post-draw
  int                 Size;
  //
  // Elements required for setup
  //
  int                 TextId;
} WM_OBJECT_QRCODE;

/*********************************************************************
*
*       WM_OBJECT_GAUGE
*/
typedef struct {
  GAUGE_OBJ           Widget;      // GUI/WM-Widget
  APPW_DISPOSE        Dispose;     // Dispose structure
  U16                 aIdDraw[2];  // Pre- and Post-draw
  int                 Value;
} WM_OBJECT_GAUGE;

/*********************************************************************
*
*       WM_OBJECT_KEYBOARD
*/
typedef struct {
  KEYBOARD_OBJ        Widget;      // GUI/WM-Widget
  APPW_DISPOSE        Dispose;     // Dispose structure
  U16                 aIdDraw[2];  // Pre- and Post-draw
  APPW_FONT         * apFont[2];
  //
  // Layout
  //
  const void        * pLayout;
  U32                 FileSize;
} WM_OBJECT_KEYBOARD;

/*********************************************************************
*
*       WM_OBJECT_PROGBAR
*/
typedef struct {
  WIDGET              Widget;      // GUI/WM-Widget
  APPW_DISPOSE        Dispose;     // Dispose structure
  U16                 aIdDraw[2];  // Pre- and Post-draw
  GUI_COLOR           aColor[2];
  APPW_DRAW_OBJECT    apDraw[2];
  U32                 Value;
  U32                 Min;
  U32                 Max;
  U32                 Radius;
  U32                 Flags;
  U8                  FrameSize;
  GUI_COLOR           FrameColor;
  U32                 AlignBm;
  I32                 xOffBm;
  I32                 yOffBm;
  U32                 Period;
  WM_HTIMER           hTimer;
  I32                 Offset;
  U32                 TimeStart;
  U8                  Tiling;
} WM_OBJECT_PROGBAR;

/*********************************************************************
*
*       WM_OBJECT_TIMER
*/
typedef struct {
  WIDGET              Widget;      // GUI/WM-Widget
  APPW_DISPOSE        Dispose;
  WM_HTIMER           hTimer;
  U32                 Period;
  unsigned            Mode;
} WM_OBJECT_TIMER;

/*********************************************************************
*
*       WM_OBJECT_MULTIEDIT
*/
typedef struct {
  MULTIEDIT_OBJ       Widget;      // GUI/WM-Widget
  APPW_DISPOSE        Dispose;     // Dispose structure
  U16                 aIdDraw[2];  // Pre- and Post-draw
  U32                 State;
  unsigned            Mode;
  int                 IndexText;
  U16                 FrameRadius;
  U16                 FrameSize;
  GUI_COLOR           FrameColor;
} WM_OBJECT_MULTIEDIT;

/*********************************************************************
*
*       WM_OBJECT_DROPDOWN
*/
typedef struct {
  DROPDOWN_Obj        Widget;      // GUI/WM-Widget
  APPW_DISPOSE        Dispose;     // Dispose structure
  U16                 aIdDraw[2];  // Pre- and Post-draw
  APPW_DRAW_OBJECT    apDraw[2];
  U8                  FrameRadius;
  U8                  FrameSize;
  GUI_COLOR           FrameColor;
  const APPW_CONTENT* pContent;
} WM_OBJECT_DROPDOWN;

/*********************************************************************
*
*       WM_OBJECT_LISTVIEW
*/
typedef struct {
  LISTVIEW_Obj        Widget;      // GUI/WM-Widget
  APPW_DISPOSE        Dispose;     // Dispose structure
  U16                 aIdDraw[2];  // Pre- and Post-draw
  APPW_DRAW_OBJECT    apDraw[2];
  APPW_FONT         * apFont[2];
  GUI_COLOR           FrameColorHeader;
  GUI_COLOR           FrameColorList;
  GUI_COLOR           ColorFocus;
  U8                  FrameRadius;
  U8                  FrameSize;
  U8                  FocusSize;
  U8                  Flags;
  const APPW_CONTENT* pContent;
} WM_OBJECT_LISTVIEW;

/*********************************************************************
*
*       WM_OBJECT_LISTBOX
*/
typedef struct {
  LISTBOX_Obj         Widget;      // GUI/WM-Widget
  APPW_DISPOSE        Dispose;     // Dispose structure
  U16                 aIdDraw[2];  // Pre- and Post-draw
  GUI_COLOR           FrameColor;
  U8                  FrameSize;
  const APPW_CONTENT* pContent;
} WM_OBJECT_LISTBOX;

/*********************************************************************
*
*       WM_OBJECT_WHEEL
*/
typedef struct {
  WHEEL_OBJ           Widget;      // GUI/WM-Widget
  APPW_DISPOSE        Dispose;     // Dispose structure
  U16                 aIdDraw[2];  // Pre- and Post-draw
  APPW_DRAW_OBJECT    apDraw[3];   // Draw objects for overlay images
} WM_OBJECT_WHEEL;

/*********************************************************************
*
*       WM_OBJECT_MOVIE
*/
typedef struct {
  MOVIE_OBJ           Widget;      // GUI/WM-Widget
  APPW_DISPOSE        Dispose;     // Dispose structure
  U16                 aIdDraw[2];  // Pre- and Post-draw
  APPW_DRAW_OBJECT    apDraw[4];   // Draw objects for button images
  APPW_DFILE_INFO     FileInfo;    // File info structure with handles and pointer to file name
} WM_OBJECT_MOVIE;

/*********************************************************************
*
*       WM_OBJECT_RADIO
*/
typedef struct {
  RADIO_Obj           Widget;      // GUI/WM-Widget
  APPW_DISPOSE        Dispose;     // Dispose structure
  U16                 aIdDraw[2];  // Pre- and Post-draw
  GUI_COLOR           aColor[2];   // Optional colors for alpha bitmaps (0: normal, 1: disabled)
  APPW_DRAW_OBJECT    apDraw[4];
  U16                 Offset;
  const APPW_CONTENT* pContent;
} WM_OBJECT_RADIO;

/*********************************************************************
*
*       WM_OBJECT_CHECKBOX
*/
typedef struct {
  CHECKBOX_Obj        Widget;      // GUI/WM-Widget
  APPW_DISPOSE        Dispose;     // Dispose structure
  U16                 aIdDraw[2];  // Pre- and Post-draw
  U32                 State;
  GUI_COLOR           aColor[2];   // Optional colors for alpha bitmaps (0: normal, 1: disabled)
  APPW_DRAW_OBJECT    apDraw[6];
  int                 TextId;
  U16                 Offset;
} WM_OBJECT_CHECKBOX;

/*********************************************************************
*
*       WM_OBJECT_WINDOW
*/
typedef struct {
  WINDOW_OBJ          Widget;      // GUI/WM-Widget
  APPW_DISPOSE        Dispose;     // Dispose structure
  U16                 aIdDraw[2];  // Pre- and Post-draw
  U32                 State;
  APPW_ROOT_INFO    * pRoot;
  APPW_SETUP_ITEM   * pSetupMotionH;
  APPW_SETUP_ITEM   * pSetupMotionV;
  GUI_COLOR           Color;
} WM_OBJECT_WINDOW;

/*********************************************************************
*
*       Public Data (to be supplied by project)
*
**********************************************************************
*/
extern       APPW_ROOT_INFO    ** APPW_ppRootList;
extern       APPW_VAR_OBJECT    * APPW_paVarList;
extern       APPW_DRAWING_ITEM ** APPW_ppDrawingList;
extern const APPW_SCROLLER_DEF  * APPW_paScrollerDef;

extern       unsigned             APPW_NumScreens;
extern       unsigned             APPW_NumVars;
extern       unsigned             APPW_NumDrawings;
extern       unsigned             APPW_NumScrollers;

extern const APPW_DRAWING_INFO    APPW__aDrawingFunc[];
extern const int                  APPW__NumDrawingFuncs;

/*####################################################################
#
#       Interface
#
######################################################################
*/
//
// Interface for callback-, create- & setup-function
//
DEFAULT_FUNC(BOX);
DEFAULT_FUNC(BUTTON);
DEFAULT_FUNC(EDIT);
DEFAULT_FUNC(GAUGE);
DEFAULT_FUNC(IMAGE);
DEFAULT_FUNC(KEYBOARD);
DEFAULT_FUNC(PROGBAR);
DEFAULT_FUNC(QRCODE);
DEFAULT_FUNC(ROTARY);
DEFAULT_FUNC(SLIDER);
DEFAULT_FUNC(SWITCH);
DEFAULT_FUNC(TEXT);
DEFAULT_FUNC(MULTIEDIT);
DEFAULT_FUNC(DROPDOWN);
DEFAULT_FUNC(LISTVIEW);
DEFAULT_FUNC(LISTBOX);
DEFAULT_FUNC(WHEEL);
DEFAULT_FUNC(MOVIE);
DEFAULT_FUNC(RADIO);
DEFAULT_FUNC(CHECKBOX);
DEFAULT_CRCB(WINDOW);

//
// OWindow
//
int WM_OBJECT_WINDOW_SetPara(WM_HWIN hWin, APPW_ROOT_INFO * pRoot);

//
// OTimer
//
WM_HWIN WM_OBJECT_TIMER_Create(APPW_DISPOSE * pDispose, WM_HWIN hParent, int Id);

//
// AppWizard.c (private functions, not to be documented)
//
int                       APPW__DisposeObject          (WM_HWIN hWin, int Index);
void                      APPW__EnableMove             (unsigned OnOff);
void                      APPW__EnableUntouchable      (int OnOff);
APPW_DISPOSE            * APPW__GetDispose             (WM_HWIN hWin);
GUI_HMEM                  APPW__GetTextItem            (int Mode, U32 Para, U16 Row, U16 Col);
void                      APPW__RegisterPID_Hook       (void);
void                      APPW__SetStreamedBitmap      (APPW_DRAW_OBJECT * pDraw, APPW_PARA_ITEM * pPara, GUI_BITMAPSTREAM_INFO * pInfo);
void                      APPW__SetStreamedBitmap_HR   (APPW_DRAW_OBJECT * pDraw, APPW_PARA_ITEM * pPara, GUI_BITMAPSTREAM_INFO * pInfo);
void                      APPW__SetUnTouchable         (WM_HWIN hWin);
void                      APPW_CreateScroller          (WM_HWIN hWin, U8 Index, int ExFlags);
void                      APPW_CreatePersistentScreens (void);
WM_HWIN                   APPW_CreateRoot              (APPW_ROOT_INFO * pRoot, WM_HWIN hParent);
int                       APPW_Dispose                 (WM_HWIN hWin, unsigned Invalidate);
void                      APPW_DragCoord               (APPW_DISPOSE * pDisposeOld, APPW_DISPOSE * pDisposeNew, unsigned Index, int Delta, U8 AllowSetSize);
void                      APPW_DrawFocusRect           (WM_HWIN hWin);
void                      APPW_DrawObject              (APPW_DRAW_OBJECT * pDraw, GUI_RECT * pRect, int AlignBm, int xOffBm, int yOffBm);
void                      APPW_DrawObjectAt            (APPW_DRAW_OBJECT * pDraw, int xPos, int yPos);
void                      APPW_DrawStreamedBitmapINT   (const APPW_DRAW_OBJECT * pDraw, int x, int y);
void                      APPW_DrawStreamedBitmapINT_HR(const APPW_DRAW_OBJECT * pDraw, int x, int y);
void                      APPW_GetAtomInfo             (const APPW_ATOM * pAtom, APPW_ATOM_INFO * pInfo);
I32                       APPW_GetAtomValue            (const APPW_ATOM * pAtom);
APPW_ROOT_INFO          * APPW_GetCurrentRootInfo      (WM_HWIN hWin);
int                       APPW_GetFocusWidth           (void);
const GUI_FONT          * APPW_GetFontFromPara         (APPW_PARA_ITEM * pPara);
GUI_HMEM                  APPW_GetLockedText           (char ** ppBuffer, APPW_PARA_ITEM * pPara);
int                       APPW_GetOpponent             (int Index);
const char              * APPW_GetResourcePath         (void);
GUI_HMEM                  APPW_GetResourceStringLocked (const char * pText, const char ** ppData);
WM_HWIN                   APPW_GetRoot                 (WM_HWIN hWin);
APPW_ROOT_INFO          * APPW_GetRootInfo             (WM_HWIN hRoot, int * pRootIndex);
APPW_ROOT_INFO          * APPW_GetRootInfoByHandle     (WM_HWIN hWin);
APPW_ROOT_INFO          * APPW_GetRootInfoByIndex      (int RootIndex);
APPW_ROOT_INFO          * APPW_GetRootInfoByRootId     (U16 RootId);
U8                        APPW_GetSupportFocus         (void);
U8                        APPW_GetSupportScroller      (void);
void                      APPW_Init                    (const char * pResourcePath);
void                      APPW_InitResourcePath        (const char * pResourcePath);
int                       APPW_IsValid                 (WM_HWIN hWin);
int                       APPW_IsRoot                  (WM_HWIN hWin);
void                      APPW_MoveDispose             (APPW_DISPOSE * pDisposeOld, APPW_DISPOSE * pDisposeNew, int dx, int dy);
void                      APPW_MULTIBUF_Enable         (int OnOff);
void                      APPW_RequestContent          (WM_HWIN hWin, void (* pFunc)(WM_HWIN, const APPW_CONTENT *));
APPW_ROOT_INFO         ** APPW_SetData                 (APPW_ROOT_INFO         ** ppRootInfo,    int NumScreens,
                                                        APPW_VAR_OBJECT         * paVarList,     int NumVars,
                                                        const APPW_SCROLLER_DEF * paScrollerDef, int NumScrollers,
                                                        APPW_DRAWING_ITEM      ** ppDrawingList, int NumDrawings);
void                      APPW_SetDefaultPosition      (WM_HWIN hRoot, U16 Id);
void                      APPW_SetDefaultPositionRoot  (WM_HWIN hRoot);
void                      APPW_SetFocusInfo            (APPW_DRAW_FOCUS_INFO * pInfo);
void                      APPW_SetpfInitFS             (void (* pFunc)(void));
void                      APPW_SetpfInitText           (void (* pFunc)(void));
void                      APPW_SetSupportFocus         (U8 SupportFocus);
void                      APPW_SetSupportScroller      (U8 SupportScroller);
void                      APPW_SetWidgetFocusable      (WM_HWIN hWin, APPW_PARA_ITEM * pPara);
void                      APPW_TextInitMem             (const APPW_TEXT_INIT * pPara);
void                      APPW_TextInitExt             (const APPW_TEXT_INIT * pPara);

extern void (* APPW__pfInvalidateWindow)(WM_HWIN hWin);

//
// AppWizard.c (public functions, to be documented)
//
void                      APPW_CalcVar                 (U16 IdVar);
int                       APPW_DoJob                   (U16 IdScreen, U16 IdWidget, int IdJob, APPW_PARA_ITEM * pPara);
void                      APPW_Exec                    (void);
int                       APPW_GetFont                 (U16 IdScreen, U16 IdWidget, GUI_FONT * pFont, GUI_XBF_DATA * pData);
int                       APPW_GetText                 (U16 IdScreen, U16 IdWidget, char * pBuffer, U32 SizeOfBuffer);
int                       APPW_GetValue                (U16 IdScreen, U16 IdWidget, int * pError);
void                      APPW_SetCustCallback         (void (* pFunc)(void));
void                      APPW_SetPos                  (WM_HWIN hItem, int xPos, int yPos);
void                      APPW_SetStreamedBitmap       (U16 IdScreen, U16 IdWidget, int Index, const void * pData);
void                      APPW_SetStreamedBitmapEx     (U16 IdScreen, U16 IdWidget, int Index, const char * pFileName);
int                       APPW_SetText                 (U16 IdScreen, U16 IdWidget, char * pText);
int                       APPW_SetValue                (U16 IdScreen, U16 IdWidget, int Value);
//
// AppWizard_Action.c
//
int                       APPW_ACTION_SetEnable        (WM_HWIN hWinDst, WM_HWIN hWinSrc, APPW_PARA_ITEM * pPara, int RootIndex);
void                      APPW_ClearShiftScreenActive  (void);
const APPW_JOB          * APPW_GetJob                  (int Id);
const APPW_JOB          * APPW_GetJobs                 (int * pNumJobs);
const APPW_JOB          * APPW_GetJobsVar              (int * pNumJobs);
U8                        APPW_GetTimerActive          (void);
int                       APPW_ProcessActions          (WM_MESSAGE * pMsg, int RootIndex);
int                       APPW_ProcessMotion           (WM_MESSAGE * pMsg, APPW_SETUP_ITEM * pSetupMotion);
void                      APPW_SendNotification        (WM_HWIN hWin, int Notification);
void                      APPW_SetTimerActive          (U8 TimerActive);

//
// AppWizard_Condition.c
//
int                       APPW__CompareIsLess          (I32 v0, I32 v1);
int                       APPW__CompareIsLessOrEqual   (I32 v0, I32 v1);
int                       APPW__CompareIsEqual         (I32 v0, I32 v1);
int                       APPW__CompareIsGreaterOrEqual(I32 v0, I32 v1);
int                       APPW__CompareIsGreater       (I32 v0, I32 v1);
int                       APPW__CompareIsNotEqual      (I32 v0, I32 v1);
int                       APPW_CalcCond                (const APPW_COND * pCond, U32 * pResult, WM_HWIN hRoot);

//
// AppWizard_Drawing.c
//
void                      APPW_DrawingPrepare          (U16 * pIdDraw, WM_HWIN hWin);
void                      APPW_DrawingDraw             (int DrawingIndex);
void                      APPW_DrawingCleanup          (void);
WM_HWIN                   APPW_GetActiveWindow         (void);

//
// AppWizard_FileAccess.c
//
void                      APPW_DrawStreamedBitmapEXT   (const APPW_DRAW_OBJECT * pDraw, int x, int y);
void                      APPW_DrawStreamedBitmapEXT_HR(const APPW_DRAW_OBJECT * pDraw, int x, int y);
const APPW_FILEACCESS   * APPW_GetFileAccess           (void);
const APPW_X_FILEACCESS * APPW_GetFileAccessLowLevel   (void);
const GUI_XBF_TYPE      * APPW_GetFileType             (U8 Type);
const APPW_MEMACCESS    * APPW_GetMemAccess            (void);
void                      APPW_GetStreamedBitmapInfoEXT(const APPW_DRAW_OBJECT * pDraw, GUI_BITMAPSTREAM_INFO * pInfo);
int                       APPW_MakeFont                (const APPW_SETUP_ITEM * pSetup);
int                       APPW_MakeFontEx              (const APPW_PARA_ITEM * pPara);
void                      APPW_SetFileAccess           (APPW_X_FILEACCESS * pFileAccess);

//
// AppWizard_Spy.c
//
int                       APPW_SPY_Process             (U8 Cmd, void * pVoid);

//
// AppWizard_Var.c
//
WM_HWIN                   APPW_GethWinVar              (void);
APPW_VAR_OBJECT         * APPW_GetVar                  (U16 Id);
int                       APPW_SetVarData              (U16 Id, I32 Data);
I32                       APPW_GetVarData              (U16 Id, int * pError);
WM_HWIN                   APPW_GetVarWin               (void);
int                       APPW_CalcTerm                (const APPW_CALC * pCalc);

//
// APPWConf.c,   generated by export function(s)
//
void                      APPW__GetResource            (APPW_ROOT_INFO         *** pppRootInfo,    int * pNumScreens,
                                                        APPW_VAR_OBJECT         ** ppaVarList,     int * pNumVars,
                                                        const APPW_SCROLLER_DEF ** ppaScrollerDef, int * pNumScrollers,
                                                        APPW_DRAWING_ITEM      *** pppDrawingList, int * pNumDrawings,
                                                                                                   int * pCreateFlags);
//
// Resource.c,   generated by export function(s)
//
void                      APPW__GetTextInit            (GUI_CONST_STORAGE APPW_TEXT_INIT ** ppTextInit);

//
// APPW_X_xxx.c, generated by export function(s)
//
void                      APPW_X_Config                (void);
void                      APPW_X_InitText              (void);
void                      APPW_X_FS_Init               (void);
void                      APPW_X_Setup                 (void);

#if defined(__cplusplus)
  }
#endif

#endif  // GUI_WINSUPPORT

#endif  // APPWIZARD_H

/*************************** End of file ****************************/
