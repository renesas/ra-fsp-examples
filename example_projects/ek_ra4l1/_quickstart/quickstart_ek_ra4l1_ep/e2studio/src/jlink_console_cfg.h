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
 * @file          jlink_console_cfg.h
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

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#ifndef JLINK_CONSOLE_CFG_H_
#define JLINK_CONSOLE_CFG_H_

/* Control sequences, based on ANSI. */
/* Can be used to control color, and clear the screen */

/* Reset to default colors */
#define CONSOLE_CTRL_RESET_CFG_                "\x1B[0m"
/* Clear screen, reposition cursor to top left */
#define CONSOLE_CTRL_CLEAR_CFG_                "\x1B[2J"

/* Set console text color to BLACK */
#define CONSOLE_CTRL_TEXT_BLACK_CFG_           "\x1B[2;30m"
/* Set console text color to RED */
#define CONSOLE_CTRL_TEXT_RED_CFG_             "\x1B[2;31m"
/* Set console text color to GREEN */
#define CONSOLE_CTRL_TEXT_GREEN_CFG_           "\x1B[2;32m"
/* Set console text color to YELLOW */
#define CONSOLE_CTRL_TEXT_YELLOW_CFG_          "\x1B[2;33m"
/* Set console text color to BLUE */
#define CONSOLE_CTRL_TEXT_BLUE_CFG_            "\x1B[2;34m"
/* Set console text color to MAGENTA */
#define CONSOLE_CTRL_TEXT_MAGENTA_CFG_         "\x1B[2;35m"
/* Set console text color to CYAN */
#define CONSOLE_CTRL_TEXT_CYAN_CFG_            "\x1B[2;36m"
/* Set console text color to WHITE */
#define CONSOLE_CTRL_TEXT_WHITE_CFG_           "\x1B[2;37m"
/* Set console text color to BRIGHT_BLACK */
#define CONSOLE_CTRL_TEXT_BRIGHT_BLACK_CFG_    "\x1B[1;30m"
/* Set console text color to BRIGHT_RED */
#define CONSOLE_CTRL_TEXT_BRIGHT_RED_CFG_      "\x1B[1;31m"
/* Set console text color to BRIGHT_GREEN */
#define CONSOLE_CTRL_TEXT_BRIGHT_GREEN_CFG_    "\x1B[1;32m"
/* Set console text color to BRIGHT_YELLOW */
#define CONSOLE_CTRL_TEXT_BRIGHT_YELLOW_CFG_   "\x1B[1;33m"
/* Set console text color to BRIGHT_BLUE */
#define CONSOLE_CTRL_TEXT_BRIGHT_BLUE_CFG_     "\x1B[1;34m"
/* Set console text color to BRIGHT_MAGENTARED */
#define CONSOLE_CTRL_TEXT_BRIGHT_MAGENTA_CFG_  "\x1B[1;35m"
/* Set console text color to BRIGHT_CYAN */
#define CONSOLE_CTRL_TEXT_BRIGHT_CYAN_CFG_     "\x1B[1;36m"
/* Set console text color to BRIGHT_WHITE */
#define CONSOLE_CTRL_TEXT_BRIGHT_WHITE_CFG_    "\x1B[1;37m"

/*Set console background color to BLACK          */
#define CONSOLE_CTRL_BG_BLACK_CFG_          "\x1B[24;40m"
/*Set console background color to RED            */
#define CONSOLE_CTRL_BG_RED_CFG_            "\x1B[24;41m"
/*Set console background color to GREEN          */
#define CONSOLE_CTRL_BG_GREEN_CFG_          "\x1B[24;42m"
/*Set console background color to YELLOW         */
#define CONSOLE_CTRL_BG_YELLOW_CFG_         "\x1B[24;43m"
/*Set console background color to BLUE           */
#define CONSOLE_CTRL_BG_BLUE_CFG_           "\x1B[24;44m"
/*Set console background color to MAGENTA        */
#define CONSOLE_CTRL_BG_MAGENTA_CFG_        "\x1B[24;45m"
/*Set console background color to CYAN           */
#define CONSOLE_CTRL_BG_CYAN_CFG_           "\x1B[24;46m"
/*Set console background color to WHITE          */
#define CONSOLE_CTRL_BG_WHITE_CFG_          "\x1B[24;47m"
/*Set console background color to BRIGHT_BLACK   */
#define CONSOLE_CTRL_BG_BRIGHT_BLACK_CFG_   "\x1B[4;40m"
/*Set console background color to BRIGHT_RED     */
#define CONSOLE_CTRL_BG_BRIGHT_RED_CFG_     "\x1B[4;41m"
/*Set console background color to BRIGHT_GREEN   */
#define CONSOLE_CTRL_BG_BRIGHT_GREEN_CFG_   "\x1B[4;42m"
/*Set console background color to BRIGHT_YELLOW  */
#define CONSOLE_CTRL_BG_BRIGHT_YELLOW_CFG_  "\x1B[4;43m"
/*Set console background color to BRIGHT_BLUE    */
#define CONSOLE_CTRL_BG_BRIGHT_BLUE_CFG_    "\x1B[4;44m"
/*Set console background color to BRIGHT_MAGENTA */
#define CONSOLE_CTRL_BG_BRIGHT_MAGENTA_CFG_ "\x1B[4;45m"
/*Set console background color to BRIGHT_CYAN    */
#define CONSOLE_CTRL_BG_BRIGHT_CYAN_CFG_    "\x1B[4;46m"
/*Set console background color to BRIGHT_WHITE   */
#define CONSOLE_CTRL_BG_BRIGHT_WHITE_CFG_   "\x1B[4;47m"

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/

/* Function Name: module_function */
/******************************************************************************************************************//**
 * @brief <Replace this with a one paragraph short Description>
 *        detailed function commenting is at the definition. (C file)
 *********************************************************************************************************************/
void module_function (void);

#endif /* JLINK_CONSOLE_CFG_H_ */
