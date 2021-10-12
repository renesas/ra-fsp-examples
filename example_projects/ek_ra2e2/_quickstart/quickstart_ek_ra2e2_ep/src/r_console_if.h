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
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : r_console_if.h
 * Description  : RTT console support. API file
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "r_console_cfg.h"
#include "r_typedefs.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

#ifndef CONSOLE_R_CONSOLE_IF_H_
#define CONSOLE_R_CONSOLE_IF_H_

/*
 * MACRO Definied by SEGGER
 */
#define SEGGER_PRINT(fn_, ...)      SEGGER_RTT_printf (SEGGER_INDEX,(fn_), ##__VA_ARGS__)

/*
 * Override for c priont command allowing application to write console using standard printf
 */
#define printf(fn_, ...)            SEGGER_PRINT((fn_), ##__VA_ARGS__)

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/

/* Function Name: R_CONSOLE_IsInputReady */
/******************************************************************************************************************//**
 * @brief         Check for input from user console, non-blocking
 * @return        Input ready true, or not ready false
 * @retval        Returns TRUE if input is ready
 * @retval        Returns FALSE if no input
 *********************************************************************************************************************/
bool_t R_CONSOLE_IsInputReady (void);


/* Function Name: R_CONSOLE_WaitForInput */
/******************************************************************************************************************//**
 * @brief         Blocking in function till key is pressed on console
 * @return        None
 *********************************************************************************************************************/
void R_CONSOLE_WaitForInput (void);

/* Function Name: R_CONSOLE_WaitForAnyKey */
/******************************************************************************************************************//**
 * @brief         Blocking in function till key is pressed on console, input is discarded
 * @return        None
 *********************************************************************************************************************/
void R_CONSOLE_WaitForAnyKey (void);

/* Function Name: R_CONSOLE_EmptyInputBuffer */
/******************************************************************************************************************//**
 * @brief         Flushes input buffer of surplus contents, resetting the buffer to empty
 * @return        None
 *********************************************************************************************************************/
void R_CONSOLE_EmptyInputBuffer (void);

/* Function Name: R_CONSOLE_ReadByte */
/******************************************************************************************************************//**
 * @brief         Reads single byte of data from console
 * @param[out]    read_byte_buffer single byte buffer containing data read from console
 * @return        error code (only 0 implemented)
 *********************************************************************************************************************/
int_t R_CONSOLE_ReadByte (char_t * read_byte_buffer);

#endif /* CONSOLE_R_CONSOLE_IF_H_ */
