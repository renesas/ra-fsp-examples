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
File        : GUI_FontIntern.h
Purpose     : Internal declarations used in font files
---------------------------END-OF-HEADER------------------------------
*/

#ifndef  GUI_FONTINTERN_H    /* Guard against multiple inclusion */
#define  GUI_FONTINTERN_H

#include "GUI.h"

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

//
// Proportional fonts
//
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font8_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font10S_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font10_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font13_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font13B_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font13H_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font13HB_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font16_1_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font16_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_CHARINFO_EXT  GUI_Font16_HK_CharInfo[169];
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font16B_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font20_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font20B_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font24_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font24B_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font32_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font32B_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_FontComic18B_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_FontComic24B_ASCII_Prop1;

//
// Monospaced fonts
//
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font6x8_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font8x8_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font8x13_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font8x15B_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font8x16_ASCII_Prop1;

//
// Proportional fonts, antialiased
//
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font12_ASCII_AA4_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font16_ASCII_AA4_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font24_ASCII_AA4_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font32_ASCII_AA4_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font48_ASCII_AA4_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font64_ASCII_AA4_Prop1;

//
// Monospaced fonts, antialiased
//
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font5x12_ASCII_AA4_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font8x16_ASCII_AA4_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font12x24_ASCII_AA4_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font16x32_ASCII_AA4_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font24x48_ASCII_AA4_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP_EXT GUI_Font32x64_ASCII_AA4_Prop1;

#if defined(__cplusplus)
  }
#endif


#endif   /* Guard against multiple inclusion */

/*************************** End of file ****************************/
