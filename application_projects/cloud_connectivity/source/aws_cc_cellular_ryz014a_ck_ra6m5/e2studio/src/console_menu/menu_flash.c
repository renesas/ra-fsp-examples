/**********************************************************************************************************************
 * File Name    : menu_flash.c
 * Version      : .
 * Description  : Flash menu details
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
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
#include <console_menu/menu_flash.h>
#include "FreeRTOS.h"
#include "FreeRTOSconfig.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"
#include "hal_data.h"
#include "common_init.h"
#include "common_utils.h"
#include "console.h"
#include "flash/flash_hp.h"
#include "rm_hs300x.h"
#include "console.h"

static char_t s_print_buffer[BUFFER_LINE_LENGTH] = {};

bool Key_skip = false;
bool g_cred_type_flag = false;

typedef struct menu_fn_tbl
{
    char_t * p_name; /*<! Name of Test */
    void ( * p_func)(void); /*<! Pointer to Test Function */
} st_menu_fn_tbl_t;

/*********************************************************************************************************************
 * @brief   Data flash menu
 *
 * This function prints data flash menu
 * @param[in]   None
 * @retval      test_fn           Data flash memory information screen
 *********************************************************************************************************************/
test_fn data_flash_menu(void)
{
    fsp_err_t err = FSP_SUCCESS;
    char Key_pressed = -1;
    credentials_t credentials;

    /* Init flash memory */
    err = flash_mem_init ();
    assert(FSP_SUCCESS == err);

    /* Wait for input from console */
    while (MENU_EXIT_CRTL != Key_pressed)
    {

        /* Sub menu index initialization*/
        char_t ndx = 'a';
        printf_colour ("%s%s", gp_clear_screen, gp_cursor_home);
        sprintf (s_print_buffer, OPTIONS_SELECT);

        /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
        /* Print sub menu */
        printf_colour ((void*) s_print_buffer);
        printf_colour (MODULE_NAME_FLASH, g_selected_menu);
        printf_colour (SUB_OPTIONS1, ndx++);
        printf_colour (SUB_OPTIONS2, ndx++);
        printf_colour (SUB_OPTIONS3, ndx++);
        printf_colour (SUB_OPTIONS4, ndx++);
        printf_colour (SUB_OPTIONS5, ndx++);
        printf_colour (SUB_OPTIONS6, ndx++);
        printf_colour (SUB_OPTIONS7, ndx++);
        printf_colour (SUB_OPTIONS8, ndx++);

        printf_colour (MENU_RETURN_INFO);

        /* Check for key pressed when wrong key is pressed from Sub menu (other than a to h)*/
        if (Key_skip == false)
        {
            Key_pressed = (int8_t) wait_for_keypress ();
        }
        switch (Key_pressed)
        {
            case INFO:
            {
                Key_skip = false;
                /* Print data flash memory information on console */
                flash_info ();
                while (MENU_EXIT_CRTL != (Key_pressed = (int8_t) wait_for_keypress ()));
                Key_pressed = 0;
                break;
            }
            case WRITE_CERTIFICATE:
            {
                Key_skip = false;
                /* select certificate type */
                credentials = CERTIFICATE;
                /* Print certificate write menu on console */
                flash_display_menu (credentials);
                /* Write AWS certificate to data flash memory */
                err = aws_certficate_write (credentials);
                assert(FSP_SUCCESS == err);
                /* Wait for space key to be pressed to return to sub menu */
                while (MENU_EXIT_CRTL != (Key_pressed = (int8_t) wait_for_keypress ()));
                Key_pressed = 0;
                break;
            }
            case WRITE_PRIVET_KEY:
            {
                Key_skip = false;
                /* select certificate type */
                credentials = PRIVATE_KEY;
                /* Print key write menu on console */
                flash_display_menu (credentials);
                /* Write AWS KEY to data flash memory */
                err = aws_certficate_write (credentials);
                assert(FSP_SUCCESS == err);
                /* Wait for space key to be pressed to return to sub menu */
                while (MENU_EXIT_CRTL != (Key_pressed = (int8_t) wait_for_keypress ()));
                Key_pressed = 0;
                break;
            }
            case READ_FLASH:
            {
                Key_skip = false;
                /* Read data from flash memory */
                err = flash_hp_data_read (true);
                assert(FSP_SUCCESS == err);
                /* Wait for space key to be pressed to return to sub menu */
                while (MENU_EXIT_CRTL != (Key_pressed = (int8_t) wait_for_keypress ()));
                Key_pressed = 0;
                break;
            }
            case WRITE_MQTT_END_POINT:
            {
                Key_skip = false;
                g_cred_type_flag = true;
                /* select certificate type */
                credentials = MQTT_ENDPOINT;
                /* Print MQTT end point write menu on console */
                flash_display_menu (credentials);
                /* Write MQTT end point to data flash memory */
                err = aws_certficate_write (credentials);
                assert(FSP_SUCCESS == err);
                /* Wait for space key to be pressed to return to sub menu */
                while (MENU_EXIT_CRTL != (Key_pressed = (int8_t) wait_for_keypress ()));
                Key_pressed = 0;
                break;
            }
            case WRITE_IOT_THING_NAME:
            {
                Key_skip = false;
                g_cred_type_flag = true;
                /* select certificate type */
                credentials = IOT_THING_NAME;
                /* Print IOT thing name write menu on console */
                flash_display_menu (credentials);
                /* Write IOT thing name to data flash memory */
                err = aws_certficate_write (credentials);
                assert(FSP_SUCCESS == err);
                /* Wait for space key to be pressed to return to sub menu */
                while (MENU_EXIT_CRTL != (Key_pressed = (int8_t) wait_for_keypress ()));
                Key_pressed = 0;
                break;
            }
            case CHECK_CREDENTIALS:
            {
                Key_skip = false;
                /* Validate stored credentials */
                check_credentials_stored ();
                /* Wait for space key to be pressed to return to sub menu */
                while (MENU_EXIT_CRTL != (Key_pressed = (int8_t) wait_for_keypress ()));
                Key_pressed = 0;
                break;
            }
            case HELP:
            {
                Key_skip = false;
                /* Print help contain */
                help_menu ();
                /* Wait for space key to be pressed to return to sub menu */
                while (MENU_EXIT_CRTL != (Key_pressed = (int8_t) wait_for_keypress ()));
                Key_pressed = 0;
                break;
            }
            case ' ':
            {
                break;
            }
            default:
            {
                printf_colour ("\r\n[ORANGE]Please enter correct option....[WHITE]");
                while (1)
                {
                    /* Wait till valid option selected */
                    Key_pressed = wait_for_keypress ();
                    if ((Key_pressed >= 'a') && (Key_pressed <= 'h'))
                    {
                        break;
                    }
                }
                Key_skip = true;
            }
        }
    }
    return 0;
}

