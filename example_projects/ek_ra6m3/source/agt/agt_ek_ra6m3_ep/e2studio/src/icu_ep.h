/***********************************************************************************************************************
 * File Name    : icu_ep.h
 * Description  : Contains declarations of data structures and functions used in icu_ep.c.
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

#ifndef ICU_EP_H_
#define ICU_EP_H_

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
/* Macros for commands to be received through RTT input */
#define ENABLE_EXT_ICU                       (1U)
#define STOP_TIMER_DISABLE_EXT_ICU           (2U)

/***********************************************************************************************************************
 * User-defined APIs
 **********************************************************************************************************************/

/*
 * function declarations
 */
fsp_err_t ext_irq_init(void);
fsp_err_t ext_irq_enable(void);
fsp_err_t ext_irq_disable(void);
void ext_irq_callback(external_irq_callback_args_t *p_args);
void ext_irq_deinit(void);


#endif /* ICU_EP_H_ */
