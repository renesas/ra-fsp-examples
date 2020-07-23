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
File        : KEYBOARD_Private.h
Purpose     : Internal header file
--------------------END-OF-HEADER-------------------------------------
*/

#ifndef KEYBOARD_PRIVATE_H
#define KEYBOARD_PRIVATE_H

#include "WM_Intern.h"
#include "KEYBOARD.h"

#if (GUI_WINSUPPORT)

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define KEYBOARD_STATE_CODES 0
#define KEYBOARD_STATE_SHIFT 1
#define KEYBOARD_STATE_SLOCK 2
#define KEYBOARD_STATE_EXTRA 3

#define FLAG_FIXED        (1 << 0)
#define FLAG_CODES        (1 << 1)
#define FLAG_SHIFT        (1 << 2)
#define FLAG_EXTRA        (1 << 3)
#define FLAG_KEYBACKSPACE (1 << 4)
#define FLAG_KEYENTER     (1 << 5)
#define FLAG_KEYSHIFT     (1 << 6)
#define FLAG_KEYSWITCH    (1 << 7)
#define FLAG_KEYSPACE     (1 << 8)

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
typedef struct {
  GUI_RECT       Rect;
  U16            Code;
  U16            Long;
  U16            Flags;
  KEYBOARD_CODES cLong;
} KEYBOARD_KEY;

typedef struct {
  GUI_HMEM     hText;  // Handle to text
  const void * pBm;    // Pointer to GUI_BITMAP or pointer to streamed bitmap
  U32          Size;   // In case of a streamed bitmap 'Size' contains its size
} KEYBOARD_BITMAP_KEY;

/*********************************************************************
*
*       Object definition
*
**********************************************************************
*/
typedef struct {
  GUI_COLOR aColor[7];       // Colors to be used
  unsigned  aPeriod[2];      // Array for periods [0:long press, 1:repeat mode]
  unsigned  FrameRadius;     // Radius to be used for all keys
  unsigned  FrameSize;       // Frame size in pixels to be used for all keys
  unsigned  SpaceX, SpaceY;  // Space in /1000 between the keys
} KEYBOARD_PROPS;

typedef struct {
  WIDGET           Widget;
  KEYBOARD_PROPS   Props;
  const GUI_FONT * apFont[2];
  //const GUI_FONT * pFontCode;
  //const GUI_FONT * pFontLong;
  //
  // Common
  //
  U8           State;
  U8           IsPressed;
  WM_HTIMER    hTimer;
  KEYBOARD_KEY kPressed;
  //
  // Code keys
  //
  unsigned NumKeys;
  GUI_HMEM hKeys;
  //
  // Special keys
  //
  KEYBOARD_BITMAP_KEY aKeyShift[4];
  KEYBOARD_BITMAP_KEY aKeySwitch[2];
  KEYBOARD_BITMAP_KEY KeyBackspace;
  KEYBOARD_BITMAP_KEY KeyEnter;
  KEYBOARD_BITMAP_KEY KeySpace;
  //
  // Timer callback
  //
  void (* pfOnTimer)(KEYBOARD_Handle hObj);
  //
  // Size of longpress key
  //
  int xSizeKey, ySizeKey;
} KEYBOARD_OBJ;

/*********************************************************************
*
*       Macros for internal use
*
**********************************************************************
*/
#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  #define KEYBOARD_INIT_ID(p) (p->Widget.DebugId = KEYBOARD_ID)
#else
  #define KEYBOARD_INIT_ID(p)
#endif

#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  KEYBOARD_OBJ * KEYBOARD_LockH(KEYBOARD_Handle h);
  #define KEYBOARD_LOCK_H(h) KEYBOARD_LockH(h)
#else
  #define KEYBOARD_LOCK_H(h) (KEYBOARD_OBJ *)GUI_LOCK_H(h)
#endif

/*********************************************************************
*
*       Public data (internal defaults)
*
**********************************************************************
*/
extern KEYBOARD_PROPS KEYBOARD__DefaultProps;

#endif  // GUI_WINSUPPORT
#endif  // KEYBOARD_PRIVATE_H

/*************************** End of file ****************************/
