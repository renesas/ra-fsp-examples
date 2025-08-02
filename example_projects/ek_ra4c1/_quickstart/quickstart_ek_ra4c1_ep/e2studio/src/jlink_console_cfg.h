/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : jlink_console_cfg.h
 * Description  : Entry function.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 20.05.2025 1.00     First Release
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

#endif /* JLINK_CONSOLE_CFG_H_ */
