/**********************************************************************************************************************
 * File Name    : menu_flash.c
 * Version      : .
 * Description  : Contains data structures and function declarations for flash memeory operations
 *********************************************************************************************************************/
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
#include "common_init.h"

#ifndef MENU_SENSOR_H_
#define MENU_SENSOR_H_

#define INFO                       ('a')
#define WRITE_CERTIFICATE          ('b')
#define WRITE_PRIVATE_KEY           ('c')
#define WRITE_MQTT_END_POINT       ('d')
#define WRITE_IOT_THING_NAME       ('e')
#define READ_FLASH                 ('f')
#define CHECK_CREDENTIALS          ('g')
#define HELP                       ('h')

#define WRITE_SSID                  ('a')
#define WRITE_PASSWORD              ('b')
#define WRITE_SECURITY              ('c')
#define WF_HELP                     ('d')

#define SECURITY_NONE               ('a')
#define SECURITY_WPA                ('b')
#define SECURITY_WPA2               ('c')

#define SSID_MIN                    (2)
#define SSID_MAX                    (32)
#define PASS_MIN                    (1)
#define PASS_MAX                    (32)
#define MODULE_NAME_FLASH        "\r\n%d. DATA FLASH\r\n"
#define SUB_OPTIONS1             "\r\n %c) Info"
#define SUB_OPTIONS2             "\r\n %c) Write Certificate "
#define SUB_OPTIONS3             "\r\n %c) Write Private Key "
#define SUB_OPTIONS4             "\r\n %c) Write MQTT Broker end point"
#define SUB_OPTIONS5             "\r\n %c) Write IOT Thing name"
#define SUB_OPTIONS6             "\r\n %c) Read Flash"
#define SUB_OPTIONS7             "\r\n %c) Check credentials stored in flash memory"
#define SUB_OPTIONS8             "\r\n %c) Help"
#define MODULE_NAME_FLASH_AWS    "\r\n DATA FLASH WRITE\r\n"

#define OPTIONS_SELECT_FLASH                 "\r\n> Select from the options in the menu below:\r\n"
#define MODULE_NAME_FLASH_WF  "\r\n%d. CONFIGURE WIFI\r\n"
#define WF_SUB_OPTIONS1       "\r\n %c) SSID Name "
#define WF_SUB_OPTIONS2       "\r\n %c) Password "
#define WF_SUB_OPTIONS3       "\r\n %c) Security Option"
#define WF_SUB_OPTIONS4       "\r\n %c) Help"

#define MODULE_NAME_WF_SECURITY  "\r\n%d. WIFI SECURITY\r\n"
#define SECURE_SUB_OPTIONS1       "\r\n %c) None "
#define SECURE_SUB_OPTIONS2     "\r\n %c) WPA "
#define SECURE_SUB_OPTIONS3     "\r\n %c) WPA2 "
#define CONNECTION_ABORT_CRTL     (0x00)
#define MENU_EXIT_CRTL            (0x20)
#define MENU_EXIT                 (32)

test_fn data_flash_menu(void);
test_fn data_wifi_flash_menu(void);
test_fn data_security_menu(void);
int8_t user_input_pass(char * user_buff);
void help_wifi_menu(void);
#endif /* MENU_SENSOR_H_ */
