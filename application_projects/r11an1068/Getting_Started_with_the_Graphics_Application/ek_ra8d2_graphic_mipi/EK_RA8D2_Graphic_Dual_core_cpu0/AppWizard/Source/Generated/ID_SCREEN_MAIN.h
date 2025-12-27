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
File        : ID_SCREEN_MAIN.h
Purpose     : Generated file do NOT edit!
---------------------------END-OF-HEADER------------------------------
*/

#ifndef ID_SCREEN_MAIN_H
#define ID_SCREEN_MAIN_H

#include "AppWizard.h"

/*********************************************************************
*
*       Objects
*/
#define ID_IMAGE_SUNNY_BGD           (GUI_ID_USER + 1)
#define ID_IMAGE_CLOUDY_BGD          (GUI_ID_USER + 2)
#define ID_IMAGE_RAINY_BGD           (GUI_ID_USER + 3)
#define ID_BUTTON_NEXT_SCREEN        (GUI_ID_USER + 4)
#define ID_BOX_TEXT_BGD              (GUI_ID_USER + 78)
#define ID_BOX_TEXT_BGD_2            (GUI_ID_USER + 79)
#define ID_TEXT_CURRENT_RTC          (GUI_ID_USER + 8)
#define ID_TEXT_FORECAST_DAY         (GUI_ID_USER + 9)
#define ID_TEXT_FORECAST_SUM         (GUI_ID_USER + 10)
#define ID_TEXT_HUMIDITY             (GUI_ID_USER + 11)
#define ID_TEXT_HUMIDITY_NO          (GUI_ID_USER + 12)
#define ID_TEXT_FORECAST_TEMP        (GUI_ID_USER + 13)
#define ID_TEXT_DEGREE_F_CURRENT     (GUI_ID_USER + 14)
#define ID_WINDOW_THERMOSTAT         (GUI_ID_USER + 15)
#define ID_BOX_THERMOSTAT_BGD        (GUI_ID_USER + 80)
#define ID_BOX_THERMOSTAT_TEXT_BGD   (GUI_ID_USER + 17)
#define ID_BUTTON_TEMP_UP            (GUI_ID_USER + 18)
#define ID_BUTTON_THERMOSTAT_OUTLINE (GUI_ID_USER + 16)
#define ID_BUTTON_TEMP_DOWN          (GUI_ID_USER + 19)
#define ID_TEXT_THERMOSTAT           (GUI_ID_USER + 20)
#define ID_TEXT_TARGET_TEMP          (GUI_ID_USER + 22)
#define ID_TEXT_DEGREE_F_TARGET      (GUI_ID_USER + 21)
#define ID_WINDOW_ANIMATION          (GUI_ID_USER + 23)
#define ID_WINDOW_SUN                (GUI_ID_USER + 24)
#define ID_BUTTON_SUN                (GUI_ID_USER + 31)
#define ID_IMAGE_SUNNY_SUN           (GUI_ID_USER + 28)
#define ID_TEXT_TEXT_SUN             (GUI_ID_USER + 29)
#define ID_TEXT_TEXT_SUN_RANGE       (GUI_ID_USER + 30)
#define ID_WINDOW_MON                (GUI_ID_USER + 25)
#define ID_BUTTON_MON                (GUI_ID_USER + 37)
#define ID_IMAGE_CLOUDY_MON          (GUI_ID_USER + 34)
#define ID_TEXT_TEXT_MON             (GUI_ID_USER + 35)
#define ID_TEXT_TEXT_MON_RANGE       (GUI_ID_USER + 36)
#define ID_WINDOW_TUE                (GUI_ID_USER + 38)
#define ID_BUTTON_TUE                (GUI_ID_USER + 44)
#define ID_IMAGE_CLOUDY_TUE          (GUI_ID_USER + 41)
#define ID_TEXT_TEXT_TUE             (GUI_ID_USER + 42)
#define ID_TEXT_TEXT_TUE_RANGE       (GUI_ID_USER + 43)
#define ID_WINDOW_WED                (GUI_ID_USER + 45)
#define ID_BUTTON_WED                (GUI_ID_USER + 51)
#define ID_IMAGE_RAINY_WED           (GUI_ID_USER + 48)
#define ID_TEXT_TEXT_WED             (GUI_ID_USER + 49)
#define ID_TEXT_TEXT_WED_RANGE       (GUI_ID_USER + 50)
#define ID_WINDOW_THU                (GUI_ID_USER + 52)
#define ID_BUTTON_THU                (GUI_ID_USER + 58)
#define ID_IMAGE_RAINY_THU           (GUI_ID_USER + 55)
#define ID_TEXT_TEXT_THU             (GUI_ID_USER + 56)
#define ID_TEXT_TEXT_THU_RANGE       (GUI_ID_USER + 57)
#define ID_WINDOW_FRI                (GUI_ID_USER + 59)
#define ID_BUTTON_FRI                (GUI_ID_USER + 65)
#define ID_IMAGE_CLOUDY_FRI          (GUI_ID_USER + 62)
#define ID_TEXT_TEXT_FRI             (GUI_ID_USER + 63)
#define ID_TEXT_TEXT_FRI_RANGE       (GUI_ID_USER + 64)
#define ID_WINDOW_SAT                (GUI_ID_USER + 66)
#define ID_BUTTON_SAT                (GUI_ID_USER + 72)
#define ID_IMAGE_SUNNY_SAT           (GUI_ID_USER + 69)
#define ID_TEXT_TEXT_SAT             (GUI_ID_USER + 70)
#define ID_TEXT_TEXT_SAT_RANGE       (GUI_ID_USER + 71)
#define ID_IMAGE_ANIM_CLOUDY_BGD     (GUI_ID_USER + 75)
#define ID_IMAGE_ANIM_CLOUD_MOTION   (GUI_ID_USER + 76)
#define ID_IMAGE_ANIM_SUNNY_BGD      (GUI_ID_USER + 77)
#define ID_IMAGE_ANIM_RAINY_BGD      (GUI_ID_USER + 73)

/*********************************************************************
*
*       Slots
*/
void ID_SCREEN_MAIN__APPW_NOTIFICATION_INITDIALOG                                                         (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_MAIN__WM_NOTIFICATION_VALUE_CHANGED                                                        (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_MAIN__WM_NOTIFICATION_VALUE_CHANGED__ID_TEXT_TARGET_TEMP__APPW_JOB_SETVALUE                (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_MAIN__ID_BUTTON_TEMP_UP__WM_NOTIFICATION_CLICKED                                           (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_MAIN__ID_BUTTON_TEMP_DOWN__WM_NOTIFICATION_CLICKED                                         (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_MAIN__ID_BUTTON_SUN__WM_NOTIFICATION_CLICKED__ID_IMAGE_ANIM_CLOUD_MOTION__APPW_JOB_SETCOORD(APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_MAIN__ID_BUTTON_MON__WM_NOTIFICATION_RELEASED                                              (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_MAIN__ID_BUTTON_MON__WM_NOTIFICATION_CLICKED                                               (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_MAIN__ID_BUTTON_MON__WM_NOTIFICATION_RELEASED_0                                            (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_MAIN__ID_BUTTON_TUE__WM_NOTIFICATION_RELEASED                                              (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_MAIN__ID_BUTTON_TUE__WM_NOTIFICATION_CLICKED                                               (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_MAIN__ID_BUTTON_TUE__WM_NOTIFICATION_RELEASED_0                                            (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_MAIN__ID_BUTTON_WED__WM_NOTIFICATION_CLICKED__ID_IMAGE_ANIM_CLOUD_MOTION__APPW_JOB_SETCOORD(APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_MAIN__ID_BUTTON_THU__WM_NOTIFICATION_CLICKED__ID_IMAGE_ANIM_CLOUD_MOTION__APPW_JOB_SETCOORD(APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_MAIN__ID_BUTTON_FRI__WM_NOTIFICATION_RELEASED                                              (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_MAIN__ID_BUTTON_FRI__WM_NOTIFICATION_CLICKED                                               (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_MAIN__ID_BUTTON_FRI__WM_NOTIFICATION_RELEASED_0                                            (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_MAIN__ID_BUTTON_SAT__WM_NOTIFICATION_CLICKED__ID_IMAGE_ANIM_CLOUD_MOTION__APPW_JOB_SETCOORD(APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_MAIN__ID_BUTTON_NEXT_SCREEN__WM_NOTIFICATION_CLICKED                                       (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);

/*********************************************************************
*
*       Callback
*/
void cbID_SCREEN_MAIN(WM_MESSAGE * pMsg);

#endif  // ID_SCREEN_MAIN_H

/*************************** End of file ****************************/
