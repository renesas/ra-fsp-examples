/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/ 
/**********************************************************************************************************************
 * File Name    : menu_catm.h
 * Description  : The Kit information demo.
 *********************************************************************************************************************/

#ifndef MENU_CATM_H_
#define MENU_CATM_H_

extern test_fn catm_display_menu (void);
extern test_fn ping_display_menu (void);
extern char wait_for_keypress(void);

#define CONNECTION_ABORT_CRTL    (0x00)
#define MENU_EXIT_CRTL_CATM1     (0x20) // was space bar (0x20), now ESC key

#define CAT_MODULE_NAME       "\r\n%d. CAT-M INFORMATION\r\n"

#define SUB_OPTIONS_CATM1     "\r\n\x1b[2m\x1b[37m a) IMEI:   %s "     \
                              "\r\n\x1b[2m\x1b[37m b) ICCID:  %s "

#define PING_SERVER_CATM1     "\r\n%d. Validating SIM activation\r\n"

#define MENU_ACTIVATION_ERROR_INFO  "\r\n\n **Activate the CAT-M Module via the Portal, before Validating SIM activation** \r\n"

#endif /* MENU_CATM_H_ */
