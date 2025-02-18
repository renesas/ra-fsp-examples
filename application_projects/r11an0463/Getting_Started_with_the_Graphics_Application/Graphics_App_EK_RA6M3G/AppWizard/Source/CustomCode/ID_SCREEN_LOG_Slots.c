/*********************************************************************
*                     SEGGER Microcontroller GmbH                    *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2024  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************
----------------------------------------------------------------------
File        : ID_SCREEN_LOG_Slots.c
Purpose     : AppWizard managed file, function content could be changed
---------------------------END-OF-HEADER------------------------------
*/

#include "Application.h"
#include "../Generated/Resource.h"
#include "../Generated/ID_SCREEN_LOG.h"

/*** Begin of user code area ***/
/*** End of user code area ***/

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       cbID_SCREEN_LOG
*/
void cbID_SCREEN_LOG(WM_MESSAGE * pMsg) {
  /* Custom code */
  cuscbID_SCREEN_LOG(pMsg);
}

/*********************************************************************
*
*       ID_SCREEN_LOG__APPW_NOTIFICATION_INITDIALOG__ID_SLIDER_BACKLIGHT__APPW_JOB_SETVALUE
*/
void ID_SCREEN_LOG__APPW_NOTIFICATION_INITDIALOG__ID_SLIDER_BACKLIGHT__APPW_JOB_SETVALUE(APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult) {
  GUI_USE_PARA(pAction);
  GUI_USE_PARA(hScreen);
  GUI_USE_PARA(pMsg);
  GUI_USE_PARA(pResult);
}

/*********************************************************************
*
*       ID_SCREEN_LOG__ID_SWITCH_DARKMODE__WM_NOTIFICATION_CLEAR
*/
void ID_SCREEN_LOG__ID_SWITCH_DARKMODE__WM_NOTIFICATION_CLEAR(APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult) {
  GUI_USE_PARA(pAction);
  GUI_USE_PARA(hScreen);
  GUI_USE_PARA(pMsg);
  GUI_USE_PARA(pResult);

  /* Set MultiEdit Background*/
  cusID_SCREEN_LOG__ID_SWITCH_DARKMODE__WM_NOTIFICATION_CLEAR(pMsg,pResult);
}

/*********************************************************************
*
*       ID_SCREEN_LOG__ID_SWITCH_DARKMODE__WM_NOTIFICATION_SET
*/
void ID_SCREEN_LOG__ID_SWITCH_DARKMODE__WM_NOTIFICATION_SET(APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult) {
  GUI_USE_PARA(pAction);
  GUI_USE_PARA(hScreen);
  GUI_USE_PARA(pMsg);
  GUI_USE_PARA(pResult);

  /* Set MultiEdit Background*/
  cusID_SCREEN_LOG__ID_SWITCH_DARKMODE__WM_NOTIFICATION_SET(pMsg, pResult);
}

/*********************************************************************
*
*       ID_SCREEN_LOG__ID_ROTARY_TEXT_COLOR__WM_NOTIFICATION_VALUE_CHANGED
*/
void ID_SCREEN_LOG__ID_ROTARY_TEXT_COLOR__WM_NOTIFICATION_VALUE_CHANGED(APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult) {
  GUI_USE_PARA(pAction);
  GUI_USE_PARA(hScreen);
  GUI_USE_PARA(pMsg);
  GUI_USE_PARA(pResult);
}

/*********************************************************************
*
*       ID_SCREEN_LOG__WM_NOTIFICATION_VALUE_CHANGED
*/
void ID_SCREEN_LOG__WM_NOTIFICATION_VALUE_CHANGED(APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult) {
  GUI_USE_PARA(pAction);
  GUI_USE_PARA(hScreen);
  GUI_USE_PARA(pResult);

  /* Change MultiEdit text color */
  cusID_SCREEN_LOG__WM_NOTIFICATION_VALUE_CHANGED(pMsg);
}

/*********************************************************************
*
*       ID_SCREEN_LOG__ID_SLIDER_BACKLIGHT__WM_NOTIFICATION_VALUE_CHANGED
*/
void ID_SCREEN_LOG__ID_SLIDER_BACKLIGHT__WM_NOTIFICATION_VALUE_CHANGED(APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult) {
  GUI_USE_PARA(pAction);
  GUI_USE_PARA(hScreen);
  GUI_USE_PARA(pMsg);
  GUI_USE_PARA(pResult);
}

/*********************************************************************
*
*       ID_SCREEN_LOG__WM_NOTIFICATION_VALUE_CHANGED_LCD_BACKLIGHT
*/
void ID_SCREEN_LOG__WM_NOTIFICATION_VALUE_CHANGED_LCD_BACKLIGHT(APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult) {
  GUI_USE_PARA(pAction);
  GUI_USE_PARA(hScreen);
  GUI_USE_PARA(pResult);

  /* Update LCD backlight intensity */
  cusID_SCREEN_LOG__WM_NOTIFICATION_VALUE_CHANGED_LCD_BACKLIGHT(pMsg);
}

/*********************************************************************
*
*       ID_SCREEN_LOG__ID_BUTTON_BACK__WM_NOTIFICATION_CLICKED
*/
void ID_SCREEN_LOG__ID_BUTTON_BACK__WM_NOTIFICATION_CLICKED(APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult) {
  GUI_USE_PARA(pAction);
  GUI_USE_PARA(hScreen);
  GUI_USE_PARA(pResult);

  /* Delete MultiEdit widget */
  cusID_SCREEN_LOG__ID_BUTTON_BACK__WM_NOTIFICATION_CLICKED(pMsg);
}

/*************************** End of file ****************************/
