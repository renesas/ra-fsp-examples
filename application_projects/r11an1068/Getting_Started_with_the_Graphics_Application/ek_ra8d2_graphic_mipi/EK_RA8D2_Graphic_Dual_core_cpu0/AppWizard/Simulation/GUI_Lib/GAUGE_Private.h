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
File        : GAUGE.h
Purpose     : GAUGE include
--------------------END-OF-HEADER-------------------------------------
*/

#ifndef GAUGE_PRIVATE_H
#define GAUGE_PRIVATE_H

#include "WM_Intern.h"
#include "GAUGE.h"

#if (GUI_WINSUPPORT)

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define PRE_VOID                   \
  {                                \
    GAUGE_OBJ * pObj;              \
    if (hObj) {                    \
      GUI_LOCK();                  \
      pObj = GAUGE_LOCK_H(hObj); {

#define POST_VOID_INVALIDATE       \
        WM_InvalidateWindow(hObj); \
      } GUI_UNLOCK_H(pObj);        \
      GUI_UNLOCK();                \
    }                              \
  }

#define POST_VOID                  \
      } GUI_UNLOCK_H(pObj);        \
      GUI_UNLOCK();                \
    }                              \
  }

/*********************************************************************
*
*       Object definition
*
**********************************************************************
*/
typedef struct {
  GUI_COLOR aColor[2];
  GUI_COLOR BkColor;
  I16       Align;
  I16       xOff, yOff;
} GAUGE_PROPS;

typedef struct {
  WIDGET      Widget;             // (obvious)
  GAUGE_PROPS Props;              // (obvious)
  int         Radius;             // 
  int         aWidth[2];          // 
  I32         Value;              // 
  I32         Ang0;               // 
  I32         Ang1;               // 
  I32         Angle;              // Current angle within the given range (Ang0 & Ang1)
  I32         MinVRange;          // Minimum of value range
  I32         MaxVRange;          // Maximum of value range
  U8          Flags;
} GAUGE_OBJ;

/*********************************************************************
*
*       Macros for internal use
*
**********************************************************************
*/
#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  #define GAUGE_INIT_ID(p) (p->Widget.DebugId = WIDGET_TYPE_GAUGE)
#else
  #define GAUGE_INIT_ID(p)
#endif

#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  GAUGE_OBJ * GAUGE_LockH(GAUGE_Handle h);
  #define GAUGE_LOCK_H(h) GAUGE_LockH(h)
#else
  #define GAUGE_LOCK_H(h) (GAUGE_OBJ *)WM_LOCK_H(h)
#endif

/*********************************************************************
*
*       Public data (internal defaults)
*
**********************************************************************
*/
extern GAUGE_PROPS GAUGE__DefaultProps;

#endif  // (GUI_SUPPORT_MEMDEV && GUI_WINSUPPORT)
#endif  // GAUGE_PRIVATE_H
