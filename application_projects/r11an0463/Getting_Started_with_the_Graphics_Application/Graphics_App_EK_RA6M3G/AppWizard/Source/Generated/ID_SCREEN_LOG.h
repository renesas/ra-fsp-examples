/*********************************************************************
*                     SEGGER Microcontroller GmbH                    *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2025  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************
----------------------------------------------------------------------
File        : ID_SCREEN_LOG.h
Purpose     : Generated file do NOT edit!
---------------------------END-OF-HEADER------------------------------
*/

#ifndef ID_SCREEN_LOG_H
#define ID_SCREEN_LOG_H

#include "AppWizard.h"

/*********************************************************************
*
*       Objects
*/
#define ID_IMAGE_BRG         (GUI_ID_USER + 1)
#define ID_BUTTON_BACK       (GUI_ID_USER + 2)
#define ID_TEXT_GO_BACK      (GUI_ID_USER + 3)
#define ID_TEXT_APP_EVENT    (GUI_ID_USER + 4)
#define ID_WINDOW_CONTROL    (GUI_ID_USER + 5)
#define ID_BOX_CONTROL       (GUI_ID_USER + 6)
#define ID_SLIDER_BACKLIGHT  (GUI_ID_USER + 7)
#define ID_SWITCH_DARKMODE   (GUI_ID_USER + 8)
#define ID_ROTARY_TEXT_COLOR (GUI_ID_USER + 9)
#define ID_TEXT_DARKMODE     (GUI_ID_USER + 11)
#define ID_TEXT_TEXT_COLOR   (GUI_ID_USER + 10)
#define ID_TEXT_BACKLIGHT    (GUI_ID_USER + 12)

/*********************************************************************
*
*       Slots
*/
void ID_SCREEN_LOG__APPW_NOTIFICATION_INITDIALOG__ID_SLIDER_BACKLIGHT__APPW_JOB_SETVALUE(APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_LOG__ID_SWITCH_DARKMODE__WM_NOTIFICATION_CLEAR                           (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_LOG__ID_SWITCH_DARKMODE__WM_NOTIFICATION_SET                             (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_LOG__ID_ROTARY_TEXT_COLOR__WM_NOTIFICATION_VALUE_CHANGED                 (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_LOG__WM_NOTIFICATION_VALUE_CHANGED                                       (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_LOG__ID_SLIDER_BACKLIGHT__WM_NOTIFICATION_VALUE_CHANGED                  (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_LOG__WM_NOTIFICATION_VALUE_CHANGED_LCD_BACKLIGHT                         (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_LOG__ID_BUTTON_BACK__WM_NOTIFICATION_CLICKED                             (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);

/*********************************************************************
*
*       Callback
*/
void cbID_SCREEN_LOG(WM_MESSAGE * pMsg);

#endif  // ID_SCREEN_LOG_H

/*************************** End of file ****************************/
