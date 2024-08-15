/**********************************************************************************************************************
 * File Name    : r_console_if.h
 * Description  : RTT console support. API file
 *********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

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
