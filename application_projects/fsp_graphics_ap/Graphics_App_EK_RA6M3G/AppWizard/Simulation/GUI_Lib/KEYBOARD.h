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
File        : KEYBOARD.h
Purpose     : KEYBOARD public header file (API)
--------------------END-OF-HEADER-------------------------------------
*/

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "WM.h"
#include "DIALOG_Type.h"      /* Req. for Create indirect data structure */
#include "WIDGET.h"

#if GUI_WINSUPPORT

#if defined(__cplusplus)
  extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/*********************************************************************
*
*       KEYBOARD color indexes
*
*  Description
*    Color indices for KEYBOARD widget.
*/
#define KEYBOARD_CI_KEY     0    // Color of key.
#define KEYBOARD_CI_FKEY    1    // Color of function key, such as shift.
#define KEYBOARD_CI_PRESSED 2    // Pressed color of a key.
#define KEYBOARD_CI_BK      5    // Background color of widget.
#define KEYBOARD_CI_CODE    3    // Text color of character on a key.
#define KEYBOARD_CI_LONG    4    // Text color of long press character on a key.
#define KEYBOARD_CI_MARK    6    // Color of shift-lock symbol.

/*********************************************************************
*
*       KEYBOARD font indexes
*
*  Description
*    Font indexes for KEYBOARD widget.
*/
#define KEYBOARD_FI_CODE 0       // Font used for text displayed on a key.
#define KEYBOARD_FI_LONG 1       // Font used for smaller long press characters displayed on a key.

/*********************************************************************
*
*       KEYBOARD period indexes
*
*  Description
*    Period indices for KEYBOARD widget.
*/
#define KEYBOARD_PI_LONGPRESS 0  // Period it takes for a long press to open the long press dialog.
#define KEYBOARD_PI_REPEAT    1  // Period for repeated actions, such as holding the backspace key.

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
typedef WM_HMEM KEYBOARD_Handle;

/*********************************************************************
*
*       KEYBOARD_CODES
*
*  Description
*    Structure that is used for storing pointers to arrays of long press characters.
*/
typedef struct {
        unsigned   NumCodes;            // Number of characters.
  const U16      * pCodes;              // Pointer to an array of character codes.
} KEYBOARD_CODES;

/*********************************************************************
*
*       KEYDEF_AREA
*
*  Description
*    This structure is used to define the position and size of keys on
*    the keyboard.
*
*  Additional information
*    All positions and sizes are entered in promille. This way, the sizes
*    and positions are relative to the keyboard's window size.
*
*    For example, if the width of a key line is defined as 1000, that means the line will span
*    over the entire keyboard widget, since 1000\promille  \preserve{equals} 100%.
*    If the keyboard widget has an x-size of 400 px, the line will also have an
*    x-size of 400.
*/
typedef struct {
  unsigned x;                           // X-position.
  unsigned y;                           // Y-position.
  unsigned w;                           // Width.
  unsigned h;                           // Height.
} KEYDEF_AREA;

/*********************************************************************
*
*       KEYDEF_LINE
*
*  Description
*    Structure that defines one line of characters of a keyboard.
*
*  Additional information
*    The given key codes defined in Codes are spread equally and horizontally
*    over the area defined in the member Area.
*/
typedef struct {
        KEYDEF_AREA      Area;          // Data structure of type KEYDEF_AREA that defines position and size of the line.
        KEYBOARD_CODES   Codes;         // Data structure of type KEYBOARD_CODES that hold the character codes to be used for the line.
  const KEYBOARD_CODES * pCodesLong;    // Pointer to an array of the type KEYBOARD_CODES that holds the character codes for long press. \n
                                        // \b{Note:} The number of elements in the array \b{must} be equal to the number of codes defined
                                        // in the previous member Codes.
} KEYDEF_LINE;

/*********************************************************************
*
*       KEYDEF_BUTTON
*
*  Description
*    Structure that holds information about how a key should look like. This structure is
*    used by KEYDEF_KEY, KEYDEF_SHIFT and KEYDEF_SWITCH.
*
*  Additional information
*    The streamed bitmap should ideally contain a compressed alpha channel (type: "Alpha channel,
*    compressed). This is necessary so the given shift color can be applied to the key bitmap.
*/
typedef struct {
  const char * pText;                   // Pointer to text shown for the key.
  const void * pBm;                     // Pointer to streamed bitmap shown for the key.
  U32          Size;                    // Array size of streamed bitmap.
} KEYDEF_BUTTON;

/*********************************************************************
*
*       KEYDEF_KEY
*
*  Description
*    Structure that defines a single key to be used in a keyboard.
*/
typedef struct {
  const KEYDEF_AREA   Area;             // Pointer to data structure of type KEYDEF_AREA that defines the
                                        // position and size of the key.
        U16           Code;             // Character code that is inserted when the key is pressed.
  const KEYDEF_BUTTON Button;           // Pointer to data structure of type KEYDEF_BUTTON that defines
                                        // the appearance of the key.
} KEYDEF_KEY;

/*********************************************************************
*
*       KEYDEF_SHIFT
*
*  Description
*    Configuration structure to define a shift key for the keyboard.
*    A shift key is used to switch between lower and upper case letters.
*/
typedef struct {
  const KEYDEF_AREA   Area;             // Data structure of type KEYDEF_AREA that defines position and size of the shift key.
  const KEYDEF_BUTTON aButton[4];       // Array of type KEYDEF_BUTTON that holds the definition of appearance of four states. The four states
                                        // are: normal, shift, shift locked and extra.
} KEYDEF_SHIFT;

/*********************************************************************
*
*       KEYDEF_SWITCH
*
*  Description
*    Configuration structure to define a switch key for the keyboard.
*    A switch key is used to switch between different lines of characters,
*    mostly used for switching between letters and special characters.
*/
typedef struct {
  const KEYDEF_AREA   Area;             // Data structure of type KEYDEF_AREA that defines position and size of the switch key.
  const KEYDEF_BUTTON aButton[2];       // Array of type KEYDEF_BUTTON that holds the appearance definition for pressed and unpressed state.
} KEYDEF_SWITCH;

/*********************************************************************
*
*       KEYDEF_KEYBOARD
*
*  Description
*    Configuration structure to define the entire layout of a KEYBOARD
*    widget. A pointer to a filled structure of this type can be passed
*    to KEYBOARD_SetLayout() or KEYBOARD_CreateUser().
*
*    If any keyboard components are not needed for the layout, such as certain keys
*    or shift lines etc., NULL can be passed instead.
*/
typedef struct {
  const char          * pLongName;      // Name of the layout. The name is only used by AppWizard, therefore the pointer can be NULL.
  const KEYDEF_KEY    * pDefBackspace;  // Pointer to key definition of backspace key.
  const KEYDEF_KEY    * pDefEnter;      // Pointer to key definition of enter key.
  const KEYDEF_KEY    * pDefSpace;      // Pointer to key definition of space key.
  const KEYDEF_SHIFT  * pDefShift;      // Pointer to the definition structure of the shift key.
  const KEYDEF_SWITCH * pDefSwitch;     // Pointer to the definition structure of the switch key.
        unsigned        NumFixedLines;  // Number of fixed lines of characters in the keyboard that will not be affected by the shift or switch key.
        unsigned        NumCodesLines;  // Number of lines of characters in the keyboard that are shown when neither shift nor switch has been pressed.
        unsigned        NumShiftLines;  // Number of lines of characters to be shown when the shift key has been pressed.
        unsigned        NumExtraLines;  // Number of lines of characters to be shown when the switch key has been pressed.
  const KEYDEF_LINE  ** ppLineFixed;    // Pointer to an array of definition structures for the fixed lines of characters.
  const KEYDEF_LINE  ** ppLineCodes;    // Pointer to an array of definition structures for the lines shown by default.
  const KEYDEF_LINE  ** ppLineShift;    // Pointer to an array of definition structures for the lines shown when the shift key is pressed.
  const KEYDEF_LINE  ** ppLineExtra;    // Pointer to an array of definition structures for the lines shown when the switch key is pressed.
        unsigned        wLong;          // Width of a long press key in the long press dialog. The size is specified in promille and relative to the KEYBOARD widget's size.
        unsigned        hLong;          // Height of a long press key in the long press dialog. The size is specified in promille and relative to the KEYBOARD widget's size.
} KEYDEF_KEYBOARD;

/*********************************************************************
*
*       Create function(s)

  Note: the parameters to a create function may vary.
         Some widgets may have multiple create functions
*/
KEYBOARD_Handle KEYBOARD_CreateUser    (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, const KEYDEF_KEYBOARD * pKeyboard, int Id, int NumExtraBytes);
KEYBOARD_Handle KEYBOARD_CreateIndirect(const GUI_WIDGET_CREATE_INFO * pCreateInfo, WM_HWIN hWinParent, int x0, int y0, WM_CALLBACK * cb);

/*********************************************************************
*
*       The callback ...
*
* Do not call it directly ! It is only to be used from within an
* overwritten callback.
*/
void KEYBOARD_Callback(WM_MESSAGE *pMsg);

/*********************************************************************
*
*       Member functions
*
**********************************************************************
*/
void KEYBOARD_ExportLayout     (GUI_CALLBACK_VOID_U8_P * pfSerialize, void * pVoid, const KEYDEF_KEYBOARD * pKeyboard);
void KEYBOARD_ExportPatternFile(GUI_CALLBACK_VOID_U8_P * pfSerialize, void * pVoid, const KEYDEF_KEYBOARD * pKeyboard);
void KEYBOARD_SetColor         (KEYBOARD_Handle hObj, unsigned Index, GUI_COLOR Color);
void KEYBOARD_SetFont          (KEYBOARD_Handle hObj, unsigned Index, const GUI_FONT * pFont);
int  KEYBOARD_SetLayout        (KEYBOARD_Handle hObj, const KEYDEF_KEYBOARD * pKeyboard);
void KEYBOARD_SetPeriod        (KEYBOARD_Handle hObj, unsigned Index, unsigned Period);
void KEYBOARD_SetRadius        (KEYBOARD_Handle hObj, unsigned Radius);
void KEYBOARD_SetSpace         (KEYBOARD_Handle hObj, unsigned Axis, unsigned Space);
int  KEYBOARD_SetStreamedLayout(KEYBOARD_Handle hObj, const void * pVoid, U32 Size);

/*********************************************************************
*
*       Predefined keyboard layout(s)
*
**********************************************************************
*/
extern const KEYDEF_KEYBOARD KEYBOARD_DEU;
extern const KEYDEF_KEYBOARD KEYBOARD_DEU_LP;
extern const KEYDEF_KEYBOARD KEYBOARD_ENG;
extern const KEYDEF_KEYBOARD KEYBOARD_ENG_LP;
extern const KEYDEF_KEYBOARD KEYBOARD_FRA_LP;
extern const KEYDEF_KEYBOARD KEYBOARD_RUS;
extern const KEYDEF_KEYBOARD KEYBOARD_NUMPAD;

#if defined(__cplusplus)
  }
#endif

#endif  // GUI_WINSUPPORT
#endif  // KEYBOARD_H
