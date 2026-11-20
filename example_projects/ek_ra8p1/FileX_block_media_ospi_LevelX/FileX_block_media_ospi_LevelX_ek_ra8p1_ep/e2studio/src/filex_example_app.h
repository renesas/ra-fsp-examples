/***********************************************************************************************************************
 * File Name    : filex_example_app.h
 * Description  : Contains macros and functions used in filex_example_app.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ***********************************************************************************************************************/

#ifndef FILEX_EXAMPLE_APP_H_
#define FILEX_EXAMPLE_APP_H_

#include "filex_ep.h"

/* Macros for messages */
#define MSG_MEDIA_NOT_OPENED    "Media has not been opened. Please open media first!!!\r\n"

#define MSG_OPERATION_LONG      "This operation will take several minutes\r\n"

#define SECTOR_WRITE_TEXT       "This operation demonstrates the low-level access with the specific\r\n"\
                                "sector of the Flash\r\n"

/* Function declaration */
void app_thread_entry_run(void);

#endif /* FILEX_EXAMPLE_APP_H_ */
