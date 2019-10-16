/***********************************************************************************************************************
 * File Name    : agt_ep.h
 * Description  : Contains declarations of data structures and functions used in agt_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright [2015-2017] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
 *
 * This file is part of Renesas RA Flex Software Package (FSP)
 *
 * The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
 * and/or its licensors ("Renesas") and subject to statutory and contractual protections.
 *
 * This file is subject to a Renesas FSP license agreement. Unless otherwise agreed in an FSP license agreement with
 * Renesas: 1) you may not use, copy, modify, distribute, display, or perform the contents; 2) you may not use any name
 * or mark of Renesas for advertising or publicity purposes or in connection with your use of the contents; 3) RENESAS
 * MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED
 * "AS IS" WITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, AND NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR
 * CONSEQUENTIAL DAMAGES, INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF
 * CONTRACT OR TORT, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents
 * included in this file may be subject to different terms.
 **********************************************************************************************************************/

#ifndef AGT_EP_H_
#define AGT_EP_H_

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
/* Array indexing MACRO */
#define ARRAY_INDEX     (0U)

/* MACRO to flag the status */
#define SET_FLAG                             (0x01)
#define RESET_FLAG                           (0x00)

/* Status of timer */
#define ALREADY_RUNNING                       (0x02)

/* MACRO to check if the LED count is zero */
#define LED_COUNT_ZERO                       (0U)


/***********************************************************************************************************************
 * User-defined APIs
 **********************************************************************************************************************/

/*
 * function declarations
 */
fsp_err_t agt_init(void);
fsp_err_t agt_start_oneshot_timer(void);
fsp_err_t agt_stop(void);
fsp_err_t stop_timer_disble_icu(void);
void one_shot_timer_callback(timer_callback_args_t *p_args);
void periodic_timer_callback(timer_callback_args_t *p_args);
void agt_deinit(void);

/** @} */

#endif /* AGT_EP_H_ */
