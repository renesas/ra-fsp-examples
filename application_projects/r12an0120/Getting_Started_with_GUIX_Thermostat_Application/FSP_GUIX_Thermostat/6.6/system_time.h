/***********************************************************************************************************************
 * Copyright [2015] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
 *
 * The contents of this file (the �contents�) are proprietary and confidential to Renesas Electronics Corporation 
 * and/or its licensors (�Renesas�) and subject to statutory and contractual protections.
 *
 * Unless otherwise expressly agreed in writing between Renesas and you: 1) you may not use, copy, modify, distribute,
 * display, or perform the contents; 2) you may not use any name or mark of Renesas for advertising or publicity
 * purposes or in connection with your use of the contents; 3) RENESAS MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE
 * SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED �AS IS� WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND
 * NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR CONSEQUENTIAL DAMAGES,
 * INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF CONTRACT OR TORT, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents included in this file may
 * be subject to different terms.
 **********************************************************************************************************************/
/**********************************************************************************************************************
* File Name    : system_time.h
* Description  : TODO
***********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*           TODO       1.00    Initial Release.
***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @file
***********************************************************************************************************************/
/*******************************************************************************************************************//**
 * @defgroup Time_Stuff
 *
 * @{
 **********************************************************************************************************************/

#ifndef SYS_TIME_H
#define SYS_TIME_H

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "bsp_api.h"
#include "tx_api.h"

#include "hal_data.h"
#include "r_rtc_api.h"

/**********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#ifdef SYS_TIME_C
#define SYS_TIME_C
#else
#define SYS_TIME_C  extern
#endif

/* Default Time: Tuesday 9:30:00 11 May 2021, daylight saving time */
#define TM_SEC                   (0U)
#define TM_MIN                   (30U)
#define TM_HOUR                  (9U)
#define TM_MDAY                  (11U)
#define TM_MON                   (4U)
#define TM_YEAR                  (121U)
#define TM_WDAY                  (2U)
#define TM_YDAY                  (131U)
#define TM_ISDST                 (1U)
/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
Function prototypes
***********************************************************************************************************************/
void system_time_init(void);
void adjust_time(rtc_time_t * p_time);
bool is_time_updated(void);
void time_message_post(void);

/***********************************************************************************************************************
Global variables
***********************************************************************************************************************/
extern volatile bool g_time_updated;

#endif /* SYS_TIME_H */

/*******************************************************************************************************************//**
 * @}
***********************************************************************************************************************/
