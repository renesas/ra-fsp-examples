/**********************************************************************************************************************
 * File Name    : menu_flash.c
 * Version      : .
 * Description  : Flash menu details
 *********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#include <console_menu/menu_flash.h>
#include "FreeRTOS.h"
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

bool key_skip = false;
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
    char key_pressed = -1;
    credentials_t credentials;

    /* Init flash memory */
    err = flash_mem_init ();
    assert(FSP_SUCCESS == err);

    /* Wait for input from console */
    while (MENU_EXIT_CRTL != key_pressed)
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

        printf_colour (MENU_FLASH_SELECT);

        /* Check for key pressed when wrong key is pressed from Sub menu (other than a to h)*/
        if (key_skip == false)
        {
            key_pressed = (int8_t) wait_for_keypress ();
        }
        switch (key_pressed)
        {
            case INFO:
            {
                key_skip = false;
                /* Print data flash memory information on console */
                flash_info ();
                while (MENU_EXIT_CRTL != (key_pressed = (int8_t) wait_for_keypress ()));
                key_pressed = 0;
                break;
            }
            case WRITE_CERTIFICATE:
            {
                key_skip = false;
                /* select certificate type */
                credentials = CERTIFICATE;
                /* Print certificate write menu on console */
                flash_display_menu (credentials);
                /* Write AWS certificate to data flash memory */
                err = aws_certficate_write (credentials);
                assert(FSP_SUCCESS == err);
                /* Wait for space key to be pressed to return to sub menu */
                while (MENU_EXIT_CRTL != (key_pressed = (int8_t) wait_for_keypress ()));
                key_pressed = 0;
                break;
            }
            case WRITE_PRIVATE_KEY:
            {
                key_skip = false;
                /* select certificate type */
                credentials = PRIVATE_KEY;
                /* Print key write menu on console */
                flash_display_menu (credentials);
                /* Write AWS KEY to data flash memory */
                err = aws_certficate_write (credentials);
                assert(FSP_SUCCESS == err);
                /* Wait for space key to be pressed to return to sub menu */
                while (MENU_EXIT_CRTL != (key_pressed = (int8_t) wait_for_keypress ()));
                key_pressed = 0;
                break;
            }
            case READ_FLASH:
            {
                key_skip = false;
                /* Read data from flash memory */
                err = flash_hp_data_read (true);
                assert(FSP_SUCCESS == err);
                /* Wait for space key to be pressed to return to sub menu */
                while (MENU_EXIT_CRTL != (key_pressed = (int8_t) wait_for_keypress ()));
                key_pressed = 0;
                break;
            }
            case WRITE_MQTT_END_POINT:
            {
                key_skip = false;
                g_cred_type_flag = true;
                /* select certificate type */
                credentials = MQTT_ENDPOINT;
                /* Print MQTT end point write menu on console */
                flash_display_menu (credentials);
                /* Write MQTT end point to data flash memory */
                err = aws_certficate_write (credentials);
                assert(FSP_SUCCESS == err);
                /* Wait for space key to be pressed to return to sub menu */
                while (MENU_EXIT_CRTL != (key_pressed = (int8_t) wait_for_keypress ()));
                key_pressed = 0;
                break;
            }
            case WRITE_IOT_THING_NAME:
            {
                key_skip = false;
                g_cred_type_flag = true;
                /* select certificate type */
                credentials = IOT_THING_NAME;
                /* Print IOT thing name write menu on console */
                flash_display_menu (credentials);
                /* Write IOT thing name to data flash memory */
                err = aws_certficate_write (credentials);
                assert(FSP_SUCCESS == err);
                /* Wait for space key to be pressed to return to sub menu */
                while (MENU_EXIT_CRTL != (key_pressed = (int8_t) wait_for_keypress ()));
                key_pressed = 0;
                break;
            }
            case CHECK_CREDENTIALS:
            {
                key_skip = false;
                /* Validate stored credentials */
                check_credentials_stored ();
                /* Wait for space key to be pressed to return to sub menu */
				printf_colour (MENU_RETURN_INFO);
                while (MENU_EXIT_CRTL != (key_pressed = (int8_t) wait_for_keypress ()));
                key_pressed = 0;
                break;
            }
            case HELP:
            {
                key_skip = false;
                /* Print help contain */
                help_menu ();
                /* Wait for space key to be pressed to return to sub menu */
                while (MENU_EXIT_CRTL != (key_pressed = (int8_t) wait_for_keypress ()));
                key_pressed = 0;
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
                    key_pressed = wait_for_keypress ();
                    if ((key_pressed >= 'a') && (key_pressed <= 'h'))
                    {
                    	key_skip = true;
						break;
					}
					else if (key_pressed == MENU_EXIT_CRTL)
					{
						key_skip = false;
                        break;
                    }
					else
					{
						/* Do nothing */
					}
                }
            }
        }
    }
    return 0;
}

