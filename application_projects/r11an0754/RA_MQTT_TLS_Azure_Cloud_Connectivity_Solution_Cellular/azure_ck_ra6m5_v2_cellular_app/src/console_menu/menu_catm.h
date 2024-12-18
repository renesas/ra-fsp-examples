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
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : menu_catm.h
 * Description  : The Kit information demo.
 *********************************************************************************************************************/

#ifndef MENU_CATM_H_
#define MENU_CATM_H_

extern test_fn catm_display_menu (void);
extern test_fn ping_display_menu (void);

#define CONNECTION_ABORT_CRTL    (0x00)
#define MENU_EXIT_CRTL_CATM1     (0x20) /*  Was space bar (0x20), now ESC key */

#define CAT_MODULE_NAME       "\r\n%d. CAT-M INFORMATION\r\n"

#define SUB_OPTIONS_CATM1     "\r\n\x1b[2m\x1b[37m a) IMEI:   %s "     \
                              "\r\n\x1b[2m\x1b[37m b) ICCID:  %s [WHITE]"

#define PING_SERVER_CATM1     "\r\n%d. Validating SIM activation\r\n"

#define MENU_ACTIVATION_ERROR_INFO  "\r\n\n **Activate the CAT-M Module via the Portal, before Validating SIM activation** \r\n"

#endif /* MENU_CATM_H_ */
