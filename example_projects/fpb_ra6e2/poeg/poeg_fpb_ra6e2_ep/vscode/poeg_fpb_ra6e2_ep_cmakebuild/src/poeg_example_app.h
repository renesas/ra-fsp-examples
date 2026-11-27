/***********************************************************************************************************************
 * File Name    : poeg_example_app.h
 * Description  : Public interface for the POEG example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef POEG_EXAMPLE_APP_H_
#define POEG_EXAMPLE_APP_H_

#include "poeg_ep.h"

#define EP_VERSION      "1.0"

#define MODULE_NAME     "r_poeg"

#define BANNER_INFO     "\r\n******************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                *"\
                        "\r\n*   Example Project Version %s                                  *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
                        "\r\n******************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO         "\r\nThis project demonstrates the basic functionalities of the Port Output Enable"\
                        "\r\nfor GPT (POEG) driver on Renesas RA MCUs based on Renesas FSP. The user can disable"\
                        "\r\nthe GPT output pins using one of the following options:"\
						"\r\n"EP_MENU""\
						"\r\nInformation, guideline and error messages are displayed on the terminal. LED1 is"\
						"\r\nturned ON when the GPT output pin is disabled successfully. LED1 is turned OFF when"\
						"\r\nPOEG resets successfully. Additionally, the user can verify the EP operation using"\
					    "\r\na logic analyzer.\r\n"

#define MENU_TITLE      "\r\nMain Menu\r\n"

#if (BSP_PERIPHERAL_ACMPHS_PRESENT) || (BSP_PERIPHERAL_ACMPHS_B_PRESENT)
#define EP_MENU         "1: Disable GPT output pins based on the GTETRG input pin level\r\n"\
                        "2: Disable GPT output pins by software API\r\n"\
                        "3: Disable GPT output pins when GTIOC pins are at the same logic level\r\n"\
                        "4: Disable GPT output pins based on comparator (ACMPHS) crossing events\r\n"\
                        "5: Reset the POEG status\r\n"

#define RESET_MENU      "\r\nReset Menu\r\n"\
                        "1: Reset POEG channel 0 (Required after running option 3)\r\n"\
                        "2: Reset POEG channel 1 (Required after running options 1, 2, or 4)\r\n"\
                        "User input: "

#define OPTION_1_INFO   "\r\n** Remove connection of GTETRG pin and reset of the application is needed prior to **"\
                        "\r\n** repeatedly selecting menu option '1' or selecting other menu options **"\
                        "\r\n** (except option '5') **\r\n"

#if defined (BOARD_RA8E1_FPB) || defined (BOARD_RA8E2_EK)
#define OPTION_4_INFO   "\r\n** Reset of the application is needed prior to repeatedly selecting menu option '4' **"\
                        "\r\n** or selecting other menu options (except option '5') to restart DAC0 and ACMPHS. **"\
                        "\r\nUser input: "
#elif defined (BOARD_RA2T1_FPB)
#define OPTION_4_INFO   "\r\n** Reset of the application is needed prior to repeatedly selecting menu option '4' **"\
                        "\r\n** or selecting other menu options (except option '5') to restart ACMPHS. **\r\n"\
                        "\r\nUser input: "
#else
#define OPTION_4_INFO   "\r\n** Reset of the application is needed prior to repeatedly selecting menu option '4' **"\
                        "\r\n** or selecting other menu options (except option '5') to restart DAC0, DAC1 and "\
                        "ACMPHS. **"\
                        "\r\nUser input: "
#endif /* Specific board */
#else
#define EP_MENU         "1: Disable GPT output pins based on the GTETRG input pin level\r\n"\
                        "2: Disable GPT output pins by software API\r\n"\
                        "3: Disable GPT output pins when GTIOC pins are at the same logic level\r\n"\
                        "4: Reset the POEG status\r\n"

#define OPTION_1_INFO   "\r\n** Remove connection of GTETRG pin and reset of the application is needed prior **"\
                        "\r\n** to repeatedly selecting menu option '1' or selecting other menu options **"\
                        "\r\n** (except option '4') **\r\n"\
                        "User input: "

#define RESET_MENU      "\r\nReset Menu\r\n"\
                        "1: Reset POEG channel 0 (Required after running option 3)\r\n"\
                        "2: Reset POEG channel 1 (Required after running options 1 or 2)\r\n"\
                        "User input: "
#endif /* BSP_PERIPHERAL_ACMPHS_PRESENT, BSP_PERIPHERAL_ACMPHS_B_PRESENT */

/* Single entry-point called in hal_entry.c */
void poeg_example_app_run(void);

#endif /* POEG_EXAMPLE_APP_H_ */
