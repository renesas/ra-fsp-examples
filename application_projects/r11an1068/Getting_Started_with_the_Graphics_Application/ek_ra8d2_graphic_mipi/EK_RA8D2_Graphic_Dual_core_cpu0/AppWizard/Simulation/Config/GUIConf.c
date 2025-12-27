/*********************************************************************
*                     SEGGER Microcontroller GmbH                    *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2019  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************
----------------------------------------------------------------------
File        : GUIConf.c
Purpose     : ...
---------------------------END-OF-HEADER------------------------------
*/

#include "GUI.h"
#include "AppWizard.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define GUI_NUMBYTES  0x800000

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _Init
*/
#ifdef SPY
static void _Init(void) {
  GUI_SPY_StartServer();
}
#endif

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_X_Config
*
* Purpose:
*   Called during the initialization process in order to set up the
*   available memory for the GUI.
*/
void GUI_X_Config(void) {
  //
  // 32 bit aligned memory area
  //
  static U32 aMemory[GUI_NUMBYTES / 4];
  //
  // Assign memory to emWin
  //
  GUI_ALLOC_AssignMemory(aMemory, GUI_NUMBYTES);
  //
  // Create and select default font
  //
  GUI_SetDefaultFont(&GUI_Font6x8);
  //
  // Call custom function after initializatio
  //
#ifdef SPY
  GUI_SetAfterInitHook(_Init);
#endif
  //
  // Set spy function
  //
  GUI_SPY_SetProcessFunc(APPW_SPY_Process);
}

/*************************** End of file ****************************/
