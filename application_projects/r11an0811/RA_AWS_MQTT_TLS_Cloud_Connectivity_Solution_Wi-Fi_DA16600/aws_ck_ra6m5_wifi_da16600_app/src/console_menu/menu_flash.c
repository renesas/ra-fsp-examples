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
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
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

bool key_skip = false;
bool g_cred_type_flag = false;
bool g_cred_type_flag_ssid = false;
bool g_cred_type_flag_pass = false;
bool none_secure_ChkFlag = false;
extern char g_write_buffer[BUFFER_SIZE];

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

/*********************************************************************************************************************
 * @brief   Enter password
 *
 * This function used to enter password
 * @param[in] char *string : the buffer to read data from RTT
 * @retval    1 : The password is not saved in the buffer
 * @retval    0 : The password is saved in the buffer
 *********************************************************************************************************************/
int8_t user_input_pass(char * user_buff)
{
    char rByte[256];           // RTT buffer to read data
    uint32_t num_bytes;             // Number of bytes read by RTT

    /* Resetting user_buff */
    memset(user_buff, RESET_VALUE, 2048);
    num_bytes = RESET_VALUE;

    char ch = RESET_VALUE;
    while (true)
    {
        /* Wait for input from console */
        ch = wait_for_keypress ();
        if (0 != ch)
        {
            if(('\r' == ch)||('\n' == ch))
            {
                if(num_bytes < PASS_MIN)
                {
                    printf_colour("\r\nMinimum password length required is 1 character");
                    return 1;
                    break;
                }
                else
                {
                rByte[num_bytes] = '\0';
                num_bytes++;
                memcpy(user_buff, rByte, (size_t) num_bytes);
                return 0;
                break;
                }
            }
            else if ('\b' == ch)
            {
                if (num_bytes > 0)
                {
                    num_bytes--;
                    rByte[num_bytes] = '\0';
                    console_write("\b \b");
                }
            }
            else
            {
                rByte[num_bytes] = ch;
                num_bytes++;
                console_write("*");
            }

            if(num_bytes > PASS_MAX)
            {
                printf_colour("\r\nPassword size must be within 32 characters");
                return 1;
                break;
            }

        }
    }
}

/*********************************************************************************************************************
 * @brief   Security menu
 *
 * This function prints security menu
 * @param[in]   None
 * @retval      test_fn           Data security flash memory information screen
 *********************************************************************************************************************/
test_fn data_security_menu(void)
{
    fsp_err_t err = FSP_SUCCESS;
    int8_t key_pressed = -1;
    char secure_none[] = "None";
    char secure_wpa[] = "WPA";
    char secure_wpa2[] = "WPA2";

    while (MENU_EXIT_CRTL != key_pressed)
    {

    char_t ndx = 'a';
    printf_colour ("%s%s", gp_clear_screen, gp_cursor_home);
    sprintf (s_print_buffer, OPTIONS_SELECT_FLASH);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    printf_colour((void*)s_print_buffer);
    printf_colour(MODULE_NAME_WF_SECURITY, g_selected_menu);
    printf_colour(SECURE_SUB_OPTIONS1, ndx++);
    printf_colour(SECURE_SUB_OPTIONS2, ndx++);
    printf_colour(SECURE_SUB_OPTIONS3, ndx++);

    printf_colour(MENU_SECURITY_SELECT);

    if (key_skip == false)
    {
        key_pressed = (int8_t)wait_for_keypress ();
    }
    switch (key_pressed)
    {
        case SECURITY_NONE :
        {
            key_skip = false;
            size_t copy_length = strlen(secure_none);
            memset (g_write_buffer, 0, BUFFER_SIZE);
            strncpy(g_write_buffer, secure_none, sizeof(g_write_buffer));
            g_write_buffer[copy_length] = '\0';
            err = aws_certficate_write_secure();
            if (err == FSP_SUCCESS){
                none_secure_ChkFlag=true;
                printf_colour ("\r\n[GREEN]Security option 'None' saved in the data flash successfully[WHITE]\r\n");
            }
            else {
                none_secure_ChkFlag=false;
                printf_colour("\r\n [RED]Security option 'None' is not saved in the data flash[WHITE]\r\n");
            }
            printf_colour (MENU_RETURN_INFO);
            while (MENU_EXIT_CRTL != (key_pressed = (int8_t)wait_for_keypress ()));
            break;
        }
        case SECURITY_WPA :
        {
            key_skip = false;
            memset (g_write_buffer, 0, BUFFER_SIZE);
            size_t copy_length = strlen(secure_wpa);
            strncpy(g_write_buffer, secure_wpa, sizeof(g_write_buffer));
            g_write_buffer[copy_length] = '\0';
            err = aws_certficate_write_secure();
            if (err == FSP_SUCCESS){
                printf_colour ("\r\n[GREEN]Security option 'WPA' saved in the data flash successfully[WHITE]\r\n");
            }
            else {
                printf_colour("\r\n [RED]Security option 'WPA' is not saved in data flash[WHITE]\r\n");
            }
            none_secure_ChkFlag=false;
            printf_colour (MENU_RETURN_INFO);
            while (MENU_EXIT_CRTL != (key_pressed = (int8_t)wait_for_keypress ()));
            break;
        }
        case SECURITY_WPA2 :
        {
            key_skip = false;
            memset (g_write_buffer, 0, BUFFER_SIZE);
            size_t copy_length = strlen(secure_wpa2);
            strncpy(g_write_buffer, secure_wpa2, sizeof(g_write_buffer));
            g_write_buffer[copy_length] = '\0';
            err = aws_certficate_write_secure();
            if (err == FSP_SUCCESS){
                printf_colour ("\r\n[GREEN]Security option 'WPA2' saved in the data flash successfully[WHITE]\r\n");
            }
            else {
                printf_colour("\r\n [RED]Security option 'WPA2' is not saved in data flash[WHITE]\r\n");
            }
            none_secure_ChkFlag=false;
            printf_colour (MENU_RETURN_INFO);
            while (MENU_EXIT_CRTL != (key_pressed = (int8_t)wait_for_keypress ()));
            break;
        }
        case ' ' :
        {
            break;
        }
        default :
        {
            printf_colour ("\r\n[ORANGE]Please enter correct option....[WHITE]");
            while (1)
            {
                /* Wait till valid option selected */
                key_pressed = wait_for_keypress ();
                if ((key_pressed >= 'a') && (key_pressed <= 'c'))
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

/*********************************************************************************************************************
 * @brief   Menu options for Wi-Fi
 *
 * This function prints details of menu options for Wi-Fi
 * @param[in]   None
 * @retval      None
 *********************************************************************************************************************/
void help_wifi_menu(void)
{
    sprintf (s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    printf_colour((void*)s_print_buffer);

    sprintf (s_print_buffer, MODULE_NAME_FLASH_HELP);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    printf_colour((void*)s_print_buffer);

    printf_colour(WF_FLASH_HELP_TAG1);
    printf_colour(WF_FLASH_HELP_TAG2);
    printf_colour(WF_FLASH_HELP_TAG3);
    printf_colour(MENU_RETURN_INFO);

}

/*********************************************************************************************************************
 * @brief   Data wifi flash menu
 *
 * This function prints data wifi flash menu
 * @param[in]   None
 * @retval      test_fn           Data wifi flash memory information screen
 *********************************************************************************************************************/
test_fn data_wifi_flash_menu(void)
{
    fsp_err_t err = FSP_SUCCESS;
    int8_t key_pressed = -1;
    credentials_t credentials;

    /* Init flash memory */
    err = flash_mem_init ();
    assert(FSP_SUCCESS == err);

    while (MENU_EXIT_CRTL != key_pressed)
    {

    char_t ndx = 'a';
    printf_colour ("%s%s", gp_clear_screen, gp_cursor_home);
   sprintf (s_print_buffer, OPTIONS_SELECT_FLASH);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    printf_colour((void*)s_print_buffer);
    printf_colour(MODULE_NAME_FLASH_WF, g_selected_menu);
    printf_colour(WF_SUB_OPTIONS1, ndx++);
    printf_colour(WF_SUB_OPTIONS2, ndx++);
    printf_colour(WF_SUB_OPTIONS3, ndx++);
    printf_colour(WF_SUB_OPTIONS4, ndx++);
    printf_colour(MENU_WIFI_SELECT);

    if (key_skip == false)
    {
        key_pressed = (int8_t)wait_for_keypress ();
    }
        switch (key_pressed)
        {
            case WRITE_SSID :
            {
                key_skip = false;
                g_cred_type_flag_ssid = true;
                g_cred_type_flag = true;
                credentials = SSID;
                flash_display_menu(credentials);
                err = aws_certficate_write(credentials);
                assert(FSP_SUCCESS == err);
                while (MENU_EXIT_CRTL != (key_pressed = (int8_t)wait_for_keypress ()));
                key_pressed = 0;
                break;
            }
            case WRITE_PASSWORD :
            {
                key_skip = false;
                g_cred_type_flag_pass = true;
                g_cred_type_flag = false;
                credentials = PASS;
                flash_display_menu(credentials);
                printf_colour("\n User Input: ");
                if (user_input_pass(g_write_buffer) == 0)
                {
                    err = aws_certficate_write(credentials);
                }
                else {
                    printf_colour (MENU_RETURN_INFO);
                    g_cred_type_flag_pass = false;
                }
                while (MENU_EXIT_CRTL != (key_pressed = (int8_t)wait_for_keypress ()));
                key_pressed = 0;
                break;
            }
            case WRITE_SECURITY :
            {
                key_skip = false;
                data_security_menu();
                break;
            }

            case WF_HELP :
            {
                key_skip = false;
                help_wifi_menu();
                while (MENU_EXIT_CRTL != (key_pressed = (int8_t)wait_for_keypress ()));
                key_pressed = 0;
                break;
            }
            case ' ' :
            {
                break;
            }
            default :
            {
                printf_colour ("\r\n[ORANGE]Please enter correct option....[WHITE]");
                while (1)
                {
                    /* Wait till valid option selected */
                    key_pressed = wait_for_keypress ();
                    if ((key_pressed >= 'a') && (key_pressed <= 'd'))
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

