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
File        : CALENDAR.h
Purpose     : Message box interface
--------------------END-OF-HEADER-------------------------------------
*/

#ifndef CALENDAR_H
#define CALENDAR_H

#include "WM.h"

#if GUI_WINSUPPORT

#if defined(__cplusplus)
  extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
/*********************************************************************
*
*       CALENDAR color indexes
*
*  Description
*    Color indexes used by the CALENDAR dialog.
*/
#define CALENDAR_CI_WEEKEND 0     // Color to be used for weekend days.
#define CALENDAR_CI_WEEKDAY 1     // Color to be used for weekdays.
#define CALENDAR_CI_SEL     2     // Color to be used for the selection.
#define CALENDAR_CI_HEADER  3     // Background color to be used for the header area.
#define CALENDAR_CI_MONTH   4     // Color to be used for the month (and year) text.
#define CALENDAR_CI_LABEL   5     // Color to be used for labeling the days.
#define CALENDAR_CI_FRAME   6     // Color to be used for the frame of the current date.

/*********************************************************************
*
*       CALENDAR font indexes
*
*  Description
*    Font indexes used by the CALENDAR dialog.
*/
#define CALENDAR_FI_CONTENT 0     // Font to be used for labeling and the numbers.
#define CALENDAR_FI_HEADER  1     // Font to be used for month / year.

/*********************************************************************
*
*       CALENDAR size indexes
*
*  Description
*    Size indexes used by the routine CALENDAR_SetDefaultSize().
*/
#define CALENDAR_SI_HEADER  0     // Y-size in pixels used for the header area. (default is 25)
#define CALENDAR_SI_CELL_X  1     // Cell size in X to be used for one item in the day pad. (default is 18)
#define CALENDAR_SI_CELL_Y  2     // Cell size in Y to be used for one item in the day pad. (default is 13)

/*********************************************************************
*
*       CALENDAR Notification codes
*
* The following is the list of notification codes specific to this widget,
* Send with the WM_NOTIFY_PARENT message
*/
#define CALENDAR_NOTIFICATION_MONTH_CLICKED  (WM_NOTIFICATION_WIDGET + 0)
#define CALENDAR_NOTIFICATION_MONTH_RELEASED (WM_NOTIFICATION_WIDGET + 1)

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
/*********************************************************************
*
*       CALENDAR_DATE
*
*  Description
*    Structure used by the routine CALENDAR_GetDate() to retrieve a date.
*/
typedef struct {
  int Year;       // Year of requested date.
  int Month;      // Month of requested date.
  int Day;        // Day of requested date.
} CALENDAR_DATE;

/*********************************************************************
*
*       CALENDAR_SKINFLEX_PROPS
*/
typedef struct {
  GUI_COLOR aColorFrame[3]; // Frame colors of buttons
  GUI_COLOR aColorUpper[2]; // Upper gradient colors of buttons
  GUI_COLOR aColorLower[2]; // Lower gradient colors of buttons
  GUI_COLOR ColorArrow;     // Arrow colors
} CALENDAR_SKINFLEX_PROPS;

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
WM_HWIN CALENDAR_Create           (WM_HWIN hParent, int xPos, int yPos, unsigned Year, unsigned Month, unsigned Day, unsigned FirstDayOfWeek, int Id, int Flags);
void    CALENDAR_GetDate          (WM_HWIN hWin, CALENDAR_DATE * pDate);
int     CALENDAR_GetDaysOfMonth   (CALENDAR_DATE * pDate);
void    CALENDAR_GetSel           (WM_HWIN hWin, CALENDAR_DATE * pDate);
int     CALENDAR_GetWeekday       (CALENDAR_DATE * pDate);
void    CALENDAR_SetDate          (WM_HWIN hWin, CALENDAR_DATE * pDate);
void    CALENDAR_SetSel           (WM_HWIN hWin, CALENDAR_DATE * pDate);
void    CALENDAR_ShowDate         (WM_HWIN hWin, CALENDAR_DATE * pDate);
int     CALENDAR_AddKey           (WM_HWIN hWin, int Key);

/*********************************************************************
*
*       Default related
*/
void    CALENDAR_SetDefaultBkColor(unsigned Index, GUI_COLOR Color);
void    CALENDAR_SetDefaultColor  (unsigned Index, GUI_COLOR Color);
void    CALENDAR_SetDefaultDays   (const char ** apDays);
void    CALENDAR_SetDefaultFont   (unsigned Index, const GUI_FONT * pFont);
void    CALENDAR_SetDefaultMonths (const char ** apMonths);
void    CALENDAR_SetDefaultSize   (unsigned Index, unsigned Size);

/*********************************************************************
*
*       Skinning related
*/
void    CALENDAR_GetSkinFlexProps (CALENDAR_SKINFLEX_PROPS * pProps, int Index);
void    CALENDAR_SetSkinFlexProps (const CALENDAR_SKINFLEX_PROPS * pProps, int Index);

/*********************************************************************
*
*       The callback ...
*
* Do not call it directly ! It is only to be used from within an
* overwritten callback.
*/
void CALENDAR_Callback(WM_MESSAGE * pMsg);

#if defined(__cplusplus)
  }
#endif

#endif // GUI_WINSUPPORT
#endif // CALENDAR_H

/*************************** End of file ****************************/
