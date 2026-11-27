/***********************************************************************************************************************
 * File Name    : elc_example_app.h
 * Description  : Public interface for the ELC example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef ELC_EXAMPLE_APP_H_
#define ELC_EXAMPLE_APP_H_

#include "elc_ep.h"

#define EP_VERSION              "1.1"

#define MODULE_NAME             "r_elc"

#define BANNER_INFO             "\r\n******************************************************************"\
                                "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                 *"\
                                "\r\n*   Example Project Version %s                                  *"\
                                "\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
                                "\r\n******************************************************************"\
                                "\r\nRefer to readme.txt file for more details on Example Project and"\
                                "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#if (BSP_PERIPHERAL_TAU_PRESENT)
#define EP_INFO                 "\r\nThis example project demonstrates the basic usage of ELC driver."\
                                "\r\nELC Software Event, "TIMER1" and "TIMER2" events are linked using ELC."\
                                "\r\nThe start source for "TIMER1" is ELC Software Event and start source"\
                                "\r\nfor "TIMER2" is end of counter "TIMER1". "TIMER1" runs in "MODE1" mode,"\
                                "\r\nand "TIMER2" runs in "MODE2" mode. On giving a valid terminal"\
                                "\r\ninput, an ELC Software Event is generated that turns ON LED1 after"\
                                "\r\n1 second. LED1 turns ON for 2 seconds until request pulse width expires."

#define STATUS_DISPLAY          "\r\n\r\nEnter any key as terminal input. After 1 second LED1 turns ON for 2 seconds."\
		                        "\r\nUser input: "
#elif (BSP_PERIPHERAL_GPT_PRESENT)
#define EP_INFO                 "\r\nThis example project demonstrates the basic usage of ELC driver."\
                                "\r\nELC Software Event, "TIMER1" and "TIMER2" events are linked using ELC."\
                                "\r\nThe start source for "TIMER1" and "TIMER2" is ELC Software Event, and the"\
                                "\r\nstop source for "TIMER2" is "TIMER1" counter overflow. "TIMER1" runs in "MODE1""\
                                "\r\nmode, and "TIMER2" runs in "MODE2" mode. On giving a valid terminal"\
                                "\r\ninput, an ELC Software Event is generated that triggers LED blinking."\
                                "\r\nLED stops blinking after 5 seconds when "TIMER1" expires."

#define STATUS_DISPLAY          "\r\n\r\nEnter any key as terminal input. LED blinks for 5 seconds and stops."\
		                        "\r\nUser input: "
#endif /* Timer version */

/* Single entry-point called in hal_entry.c */
void elc_example_app_run(void);

#endif /* ELC_EXAMPLE_APP_H_ */
