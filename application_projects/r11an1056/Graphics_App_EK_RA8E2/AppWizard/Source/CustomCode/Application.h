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
File        : Application.h
Purpose     : Content to be managed by customer
---------------------------END-OF-HEADER------------------------------
*/

#ifndef APPLICATION_H
#define APPLICATION_H
#include "../Generated/Resource.h"
#include "../Generated/ID_SCREEN_MAIN.h"
#include "../Generated/ID_SCREEN_LOG.h"

/* Macro Definition */
#define IMAGE_ANIM_CLOUD_MOTION  (0x3U)
#define IMAGE_ANIM_RAINY_BGD_2  (0x4U)
#define ANIM_TIMER_PERIOD        (450U)
#define THERMO_TARGET_TEMP_HIGH  (84U)
#define THERMO_TARGET_TEMP_LOW   (61U)
#define GPT_PWM_MAX_PERCENT      (100U)
#define GUI_CUSTOM_COLOR         (0x708CAFU)

/* Default Time: Tuesday 9:30:00 5 Aug 2025, not daylight saving time */
#define TM_SEC                   (0U)
#define TM_MIN                   (30U)
#define TM_HOUR                  (9U)
#define TM_MDAY                  (5U)     // day 5
#define TM_MON                   (7U)     // August (0-based, so 7 = August)
#define TM_YEAR                  (125U)   // year 2025 (2025 - 1900 = 125)
#define TM_WDAY                  (2U)     // Tuesday (0 = Sunday)
#define TM_YDAY                  (216U)   // 217th day of the year (non-leap year)
#define TM_ISDST                 (0U)     // not daylight saving time

/* Event Logging */
#define LOG_CHAR_MAX             (272U)
#define LOG_BUF_SIZE             (256U)
#define LOG_CHAR_PER_LINE        (24U)
#define LOG_TAG1                 "Forecast"
#define LOG_TAG2                 "Target Temp"

/* trap upon the error  */
#define APP_ERR_TRAP(err)        if(err) { __asm("BKPT #0\n");}

/* Type definitions */
typedef enum e_weather_type {
    SYS_WEATHER_SUNNY = 0,
    SYS_WEATHER_RAINY,
    SYS_WEATHER_CLOUDY,
}weather_type_t;

typedef enum e_week_day {
    WEEK_DAY_SUN = 0,
    WEEK_DAY_MON,
    WEEK_DAY_TUE,
    WEEK_DAY_WED,
    WEEK_DAY_THU,
    WEEK_DAY_FRI,
    WEEK_DAY_SAT,
}week_day_t;

typedef struct st_day_info
{
    char            * day_name;
    char            * description;
    char            * humidity;
    char            * current_temp;
    char            * temp_range;
} day_info_t;

typedef struct app_data_info
{
    uint8_t         pwm_duty_cycle;
    uint8_t         bgrd_darmode;
    uint32_t        text_color;
    day_info_t      *forecast_info;
    uint8_t         thermo_target_temp;
    weather_type_t  sys_weather_type;
    week_day_t      current_forecast;
    uint8_t         total_forecast;
} app_data_info_t;

extern volatile app_data_info_t gDataApp;
/* Custom functions */
/* ID_MAIN_SCREEN */
void cuscbID_SCREEN_MAIN(WM_MESSAGE * pMsg);
void cusID_SCREEN_MAIN__WM_NOTIFICATION_CLICKED_STOP_REMOVE_ANIM(WM_MESSAGE * pMsg);
void cusID_SCREEN_MAIN__ID_BUTTON_TEMP_DOWN__WM_NOTIFICATION_CLICKED(WM_MESSAGE * pMsg);
void cusID_SCREEN_MAIN__ID_BUTTON_TEMP_UP__WM_NOTIFICATION_CLICKED(WM_MESSAGE * pMsg);
void cusID_SCREEN_MAIN__WM_NOTIFICATION_VALUE_CHANGED_ID_VAR_TIME_UPDATE(WM_MESSAGE * pMsg);
void cusID_SCREEN_MAIN__ID_BUTTON_NEXT_SCREEN__WM_NOTIFICATION_CLICKED(WM_MESSAGE * pMsg);

/* ID_EVENT_SCREEN */
void cuscbID_SCREEN_LOG(WM_MESSAGE * pMsg);
void cusID_SCREEN_LOG__ID_SWITCH_DARKMODE__WM_NOTIFICATION_SET(WM_MESSAGE * pMsg, int * pResult);
void cusID_SCREEN_LOG__ID_SWITCH_DARKMODE__WM_NOTIFICATION_CLEAR(WM_MESSAGE * pMsg, int * pResult);
void cusID_SCREEN_LOG__WM_NOTIFICATION_VALUE_CHANGED_LCD_BACKLIGHT(WM_MESSAGE * pMsg);
void cusID_SCREEN_LOG__WM_NOTIFICATION_VALUE_CHANGED(WM_MESSAGE * pMsg);
void cusID_SCREEN_LOG__ID_BUTTON_BACK__WM_NOTIFICATION_CLICKED(WM_MESSAGE * pMsg);
#endif  // APPLICATION_H

/*************************** End of file ****************************/
