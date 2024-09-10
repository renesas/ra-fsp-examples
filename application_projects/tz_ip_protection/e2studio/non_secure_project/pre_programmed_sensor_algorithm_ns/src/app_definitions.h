/***********************************************************************************************************************
 * File Name    : app_definitions.h
 * Description  : Contains Secure code region definitions and Security operation related macros
 ***********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#ifndef APP_DEFINITIONS_H_
#define APP_DEFINITIONS_H_

#include <stdint.h>
/* SEGGER RTT and error related headers */
#include "SEGGER_RTT/SEGGER_RTT.h"
#include "common_utils.h"

#define _unused(x) ((void)(x))


/* Macros for menu options to be displayed */
#define MENUOPTION_REGION		  	"\r\n Code Running in Non-secure Region\r\n"
#define MENUOPTION_MENU       		"\r\n MENU to Select \r\n"
#define MENUOPTION1       			"Press 1 to Start the IP Algorithm\r\n"
#define MENUOPTION2       			"Press 2 to Stop the IP Algorithm\r\n"

#define START_IP_ALGORITHM			(1)
#define STOP_IP_ALGORITHM			(2)

#define MENUEVENT1       " Start IP Algorithm\r\n If temperature did not change more than threshold, the Green LED will blink\r\n If temperature changed more than threshold, the Red LED will blink"
#define MENUEVENT2       " Stop_IP_Algorithm\r\n Blue LED will toggle twice\r\n Press 1 to restart the IP Algorithm\r\n"

#define IN_NON_SECURE_REGION                    "\r\n System Reset, enter non-secure region\r\n"

#endif /* APP_DEFINITIONS_H_ */
