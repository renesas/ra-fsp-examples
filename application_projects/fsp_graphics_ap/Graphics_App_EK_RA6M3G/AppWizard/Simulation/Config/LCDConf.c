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
File        : LCDConf.c
Purpose     : ...
---------------------------END-OF-HEADER------------------------------
*/

#include "GUI.h"
#include "AppWizard.h"

/*********************************************************************
*
*       LCD_X_Config
*/
void LCD_X_Config(void) {
  APPW_X_Config();
}

/*********************************************************************
*
*       LCD_X_DisplayDriver
*/
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData) {
  return 0;
}

/*************************** End of file ****************************/
