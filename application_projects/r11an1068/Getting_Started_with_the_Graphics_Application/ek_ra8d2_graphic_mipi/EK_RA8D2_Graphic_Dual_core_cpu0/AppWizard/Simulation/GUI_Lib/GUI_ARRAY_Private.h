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
File        : GUI_ARRAY_Private.h
Purpose     : Private array handling routines, should be used only
              from within GUI_ARRAY... routines!
---------------------------END-OF-HEADER------------------------------
*/

#ifndef GUI_ARRAY_PRIVATE_H
#define GUI_ARRAY_PRIVATE_H

#include "GUI_ARRAY.h"

/*********************************************************************
*
*       Private types
*
**********************************************************************
*/
typedef struct {
  U16 NumItems;
  GUI_HMEM haHandle;   /* Handle to buffer holding handles */
} GUI_ARRAY_OBJ;

/*********************************************************************
*
*       Private functions
*
**********************************************************************
*/
GUI_HMEM GUI_ARRAY__GethItem      (const GUI_ARRAY_OBJ * pThis, unsigned int Index);
void   * GUI_ARRAY__GetpItem      (const GUI_ARRAY_OBJ * pThis, unsigned int Index);
void   * GUI_ARRAY__GetpItemLocked(const GUI_ARRAY_OBJ * pThis, unsigned int Index);
int      GUI_ARRAY__SethItem      (      GUI_ARRAY_OBJ * pThis, unsigned int Index, GUI_HMEM hItem);

#endif /* GUI_ARRAY_PRIVATE_H */

/*************************** End of file ****************************/
