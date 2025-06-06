/***********************************************************************************************************************
 * File Name    : agt_ep.h
 * Description  : Contains declarations of data structures and functions used in agt_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef AGT_EP_H_
#define AGT_EP_H_

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
/* Array indexing Macro */
#define ARRAY_INDEX                          (0U)

/* Macros to flag the status */
#define SET_FLAG                             (0x01)
#define RESET_FLAG                           (0x00)

/* Status of timer */
#define ALREADY_RUNNING                      (0x02)

/* Macro to check if the LED count is zero */
#define LED_COUNT_ZERO                       (0U)

/* Macros to define time-period value limits */
#define TIME_PERIOD_MAX                      (2000U)
#define TIME_PERIOD_MIN                      (0U)

/* Macro for null character */
#define NULL_CHAR                            ('\0')

/***********************************************************************************************************************
 * User-defined APIs
 **********************************************************************************************************************/

/* Function declarations */
fsp_err_t agt_init(void);
fsp_err_t agt_start_oneshot_timer(void);
void one_shot_timer_callback(timer_callback_args_t *p_args);
void periodic_timer_callback(timer_callback_args_t *p_args);
void agt_deinit(void);

/** @} */

#endif /* AGT_EP_H_ */
