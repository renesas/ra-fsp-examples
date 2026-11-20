/***********************************************************************************************************************
 * File Name    : usb_pcdc_example_app.h
 * Description  : Public interface for the USB PCDC example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef USB_PCDC_EXAMPLE_APP_H_
#define USB_PCDC_EXAMPLE_APP_H_

#include "usb_pcdc_board_cfg.h"

/* Macros for the commands entered from the terminal */
#define KIT_INFO                    ('1')
#define NEXT_STEPS                  ('2')
#define CARRIAGE_RETURN             ('\r')

/* Macro for the menu text */
#define MENU_TEXT                   "\r\nPress 1 for kit information or 2 for next steps.\r\n"

/* Macro for the welcome text */
#define WELCOME_TEXT                ("\r\nWelcome to USB_PCDC example project for "\
                                    KIT_NAME_MACRO\
                                    "!"\
                                    MENU_TEXT)

/* Macro for the next steps text */
#define NEXT_STEPS_TEXT             ("\r\n2. Next steps\r\n"\
                                    "\r\nVisit the following URLs to learn about the kit and the RA family of MCUs,"\
                                    "\r\ndownload tools and documentation, and get support:\r\n"\
                                    "\r\na) "\
                                    KIT_NAME_MACRO\
                                    " resources: \t\t"\
                                    KIT_LANDING_URL\
                                    "\r\nb) RA product information:  \t"\
                                    PRODUCT_INFO_URL\
                                    "\r\nc) RA product support forum: \t"\
                                    PRODUCT_SUPPORT_URL\
                                    "\r\nd) Renesas support: \t\t"\
                                    RENESAS_SUPPORT_URL\
                                    "\r\n"\
                                    MENU_TEXT)

/* Macros for the kit information text */
#define KIT_INFO_PRIMARY_TEXT       ("\r\n1. KIT INFORMATION\r\n"\
                                    "\r\na) Kit name: \t\t\t\t"\
                                    KIT_NAME_MACRO\
                                    "\r\nb) Kit ordering part number:\t\t"\
                                    KIT_PARTNUM_MACRO)
#define LED_FREQ_TEXT               "\r\nc) Current blinking frequency (Hz): "
#define MCU_TEMP_TEXT               "\r\nd) MCU die temperature (F/C):  "

/* Single entry-point called in hal_entry.c */
void usb_pcdc_example_app_run(void);

#endif /* USB_PCDC_EXAMPLE_APP_H_ */
