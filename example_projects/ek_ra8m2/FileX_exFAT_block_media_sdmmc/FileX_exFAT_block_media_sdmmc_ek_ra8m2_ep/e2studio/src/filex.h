/***********************************************************************************************************************
 * File Name    : filex.h
 * Description  : Contains macros, data structures and functions commonly used in the EP.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef FILEX_H_
#define FILEX_H_

#include "common_utils.h"
#include "rtt.h"
#include "console.h"

#define FILEX_THREAD_SLEEP_TICK     (1U)
#define ONE_BYTE                    (1U)

/* Macros for system date and time */
#define MONTH_STR_LEN               (3U)
#define MONTH_INDEX                 (0U)
#define DATE_INDEX                  (4U)
#define YEAR_INDEX                  (7U)
#define HOUR_INDEX                  (0U)
#define MIN_INDEX                   (3U)
#define SEC_INDEX                   (6U)

/* Enumerate for media status */
typedef enum e_media_status
{
    MEDIA_INSERTED  = 1U << 0,
    MEDIA_OPEN      = 1U << 1,
    MEDIA_OPENED    = 1U << 2,
}media_status_t;

/* External variables */
extern FX_MEDIA g_fx_media;
extern uint8_t g_fx_media_media_memory[];
extern volatile media_status_t g_fx_media_status;

#endif /* FILEX_H_ */
