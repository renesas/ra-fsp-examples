/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2025 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/******************************************************************************************************************//**
 * @file          update_slcd.h
 * @Version       1.0
 * @brief         .
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 01.01.2020 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "common_init.h"
#include "slcd.h"
#include "r_rtc_api.h"
#include "lp_mode.h"
#include "agt_slcd_fresh.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#ifndef UPDATE_SLCD_H_
#define UPDATE_SLCD_H_

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * Function Name: set_time_on_slcd
 * Description  : .
 * Argument     : p_time
 * Return Value : .
 *********************************************************************************************************************/
void set_time_on_slcd (rtc_time_t * p_time);

/**********************************************************************************************************************
 * Function Name: set_temp_on_slcd
 * Description  : .
 * Argument     : p_status
 * Return Value : .
 *********************************************************************************************************************/
void set_temp_on_slcd (st_board_status_t * p_status);

/**********************************************************************************************************************
 * Function Name: set_alarm_on_slcd
 * Description  : .
 * Argument     :
 * Return Value : .
 *********************************************************************************************************************/
void set_alarm_on_slcd ();

/**********************************************************************************************************************
 * Function Name: rtc_update_callback
 * Description  : .
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void rtc_update_callback (rtc_callback_args_t * p_args);

/* Function Name: module_function */
/******************************************************************************************************************//**
 * @brief <Replace this with a one paragraph short Description>
 *        detailed function commenting is at the definition. (C file)
 *********************************************************************************************************************/
void module_function (void);

#endif /* UPDATE_SLCD_H_ */
