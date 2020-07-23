/*********************************************************************
*                     SEGGER Microcontroller GmbH                    *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2020  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************
----------------------------------------------------------------------
File        : Resource.h
Purpose     : Generated file do NOT edit!
---------------------------END-OF-HEADER------------------------------
*/

#ifndef RESOURCE_H
#define RESOURCE_H

#include "AppWizard.h"

/*********************************************************************
*
*       Text
*/
#define ID_TEXT_WED_TXT 0
#define ID_TEXT_HUMIDITY_TXT 1
#define ID_TEXT_THERMOSTAT_TXT 2
#define ID_TEXT_SUN_TXT 3
#define ID_TEXT_MON_TXT 4
#define ID_TEXT_TUE_TXT 5
#define ID_TEXT_THU_TXT 6
#define ID_TEXT_FRI_TXT 7
#define ID_TEXT_SAT_TXT 8
#define ID_TEXT_BACKLIGHT_TXT 9
#define ID_TEXT_TEXT_COLOR_TXT 10
#define ID_TEXT_DARKMODE_TXT 11
#define ID_TEXT_DEGR_F_TXT 12
#define ID_TEXT_LOG_LABEL_TXT 13
#define ID_TEXT_GO_BACK_TXT 14

#define APPW_MANAGE_TEXT APPW_MANAGE_TEXT_EXT
extern const unsigned char acAPPW_Language_0[];

/*********************************************************************
*
*       Fonts
*/
extern const unsigned char acNettoOT_16_Normal_EXT_AA4[];
extern const unsigned char acRoboto_16_Normal_EXT_AA4[];
extern const unsigned char acNettoOT_32_Normal_EXT_AA4[];

/*********************************************************************
*
*       Images
*/
extern const unsigned char acbg_rainy[];
extern const unsigned char acbg_partlycloudy[];
extern const unsigned char acbg_sunny[];
extern const unsigned char acbg_thermostat[];
extern const unsigned char acbox_thermostat[];
extern const unsigned char acbutton_plus[];
extern const unsigned char acbutton_plus_pressed[];
extern const unsigned char acbutton_minus[];
extern const unsigned char acbutton_minus_pressed[];
extern const unsigned char acanime_raining_1[];
extern const unsigned char acbottom_button_trans_pressed[];
extern const unsigned char acbottom_button_trans[];
extern const unsigned char acicon_sunny[];
extern const unsigned char acicon_cloudy[];
extern const unsigned char acicon_rainy[];
extern const unsigned char acanime_raining_2[];
extern const unsigned char acanime_sun_oncloud[];
extern const unsigned char acanime_cloud[];
extern const unsigned char acanime_sun_shining[];
extern const unsigned char acSimple_blue_green_gradient_480x272[];
extern const unsigned char ac_20px_Renesas_Electronics_logo_svg[];
extern const unsigned char acSliderBottomGray_20x40[];
extern const unsigned char acSliderTopGray_20x40[];
extern const unsigned char acSliderThumbGray_40x20[];
extern const unsigned char acLeftS_80x30[];
extern const unsigned char acRightS_80x30[];
extern const unsigned char acThumbLeft_30x30[];
extern const unsigned char acThumbRight_30x30[];
extern const unsigned char acrotary_silver_100[];
extern const unsigned char acMarker_white_semicircular_11x11[];

/*********************************************************************
*
*       Variables
*/
#define ID_VAR_TARGET_TEMP 4099
#define ID_VAR_TIME_UPDATE 4096
#define ID_VAR_DARKMODE 4097
#define ID_VAR_BACKLIGHT 4098
#define ID_VAR_TEXT_COLOR_CHANGE 4100

/*********************************************************************
*
*       Screens
*/
#define ID_SCREEN_MAIN (GUI_ID_USER + 4096)
#define ID_SCREEN_LOG (GUI_ID_USER + 4097)

extern APPW_ROOT_INFO ID_SCREEN_MAIN_RootInfo;
extern APPW_ROOT_INFO ID_SCREEN_LOG_RootInfo;

#define APPW_INITIAL_SCREEN &ID_SCREEN_MAIN_RootInfo

/*********************************************************************
*
*       Project path
*/
#define APPW_PROJECT_PATH "C:/Users/bnguyen01/e2_studio/workspace_fsp_1.2.0/Graphics_App_EK_RA6M3G/AppWizard"

#endif  // RESOURCE_H

/*************************** End of file ****************************/
