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
/***********************************************************************************************************************
 * File Name    : menu_kis.c
 * Description  : Main menu screen.
 **********************************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include "common_init.h"
#include "common_data.h"
#include "common_utils.h"
#include "common_init.h"
#include "menu_kis.h"
#include "console.h"
#include "menu_flash.h"
#include "flash/flash_hp.h"

extern TaskHandle_t app_thread; // @suppress("Global (API or Non-API) variable prefix")

static char_t s_print_buffer[BUFFER_LENGTH_SHORT] = { };
static bool cred_save_success_flag = false;

/*********************************************************************************************************************
 * @brief   Get fsp version
 *
 * This function prints fsp version
 * @param       None
 * @retval      test_fn                 Get version information screen
 *********************************************************************************************************************/
test_fn get_version(void)
{
    int8_t key_pressed = -1;
    fsp_pack_version_t version = { RESET_VALUE };

    sprintf (s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    printf_colour ((void*) s_print_buffer);

    sprintf (s_print_buffer, MODULE_NAME_VERSION, g_selected_menu);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    printf_colour ((void*) s_print_buffer);

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet (&version);

    sprintf (s_print_buffer, "   %d.%d.%d", version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch); //, version.minor, version.patch);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    printf_colour ((void*) s_print_buffer);

    sprintf (s_print_buffer, MENU_RETURN_INFO);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    printf_colour ((void*) s_print_buffer);
    while (CONNECTION_ABORT_CRTL != key_pressed)
    {
        /* Wait for space key to be pressed to return to main menu */
        key_pressed = wait_for_keypress ();
        if ((MENU_EXIT_CRTL == key_pressed) || (CONNECTION_ABORT_CRTL == key_pressed))
        {
            break;
        }
    }

    return (0);
}

/*********************************************************************************************************************
 * @brief   Get UUID
 *
 * This function prints UUID
 * @param       None
 * @retval      test_fn                 Get UUID information screen
 *********************************************************************************************************************/
test_fn get_uuid(void)
{
    int8_t key_pressed = -1;
    bsp_unique_id_t const *p_uid = R_BSP_UniqueIdGet ();

    sprintf (s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    printf_colour ((void*) s_print_buffer);

    sprintf (s_print_buffer, MODULE_NAME_UUID, g_selected_menu);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    printf_colour ((void*) s_print_buffer);

    sprintf (s_print_buffer, UUID_TAG);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    printf_colour ((void*) s_print_buffer);

    memset (s_print_buffer, 0, BUFFER_LENGTH_SHORT);
    sprintf (s_print_buffer, " %08x-%08x-%08x-%08x\r\n", (int_t) p_uid->unique_id_words[0], (int_t) p_uid->unique_id_words[1],
             (int_t) p_uid->unique_id_words[2], (int_t) p_uid->unique_id_words[3]);
    printf_colour (s_print_buffer);

    printf_colour (MENU_RETURN_INFO);

    while (CONNECTION_ABORT_CRTL != key_pressed)
    {
        /* Wait for space key to be pressed to return to main menu */
        key_pressed = wait_for_keypress ();
        if ((MENU_EXIT_CRTL == key_pressed) || (CONNECTION_ABORT_CRTL == key_pressed))
        {
            break;
        }
    }
    return (0);
}

/*********************************************************************************************************************
 * @brief   Help menu
 *
 * This function prints details of menu options
 * @param       None
 * @retval      test_fn                 Help menu information screen
 *********************************************************************************************************************/
test_fn help (void)
{
    int8_t key_pressed = -1;

    sprintf (s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    printf_colour ((void*) s_print_buffer);

    sprintf (s_print_buffer, MODULE_NAME_HELP, g_selected_menu);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    printf_colour ((void*) s_print_buffer);
    sprintf (s_print_buffer, " %s\r\n %s\r\n %s", HELP_TAG1, HELP_TAG2, HELP_TAG3);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    printf_colour ((void*) s_print_buffer);

    printf_colour ("\r\n");

    sprintf (s_print_buffer, " %s\r\n %s\r\n %s ", HELP_TAG4, HELP_TAG5, HELP_TAG6);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    printf_colour ((void*) s_print_buffer);

    printf_colour ("\r\n");

    sprintf (s_print_buffer, " %s\r\n", HELP_TAG7);

        /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
   printf_colour ((void*) s_print_buffer);

    printf_colour (MENU_RETURN_INFO);

    while (CONNECTION_ABORT_CRTL != key_pressed)
    {
        /* Wait for space key to be pressed to return to main menu */
        key_pressed = wait_for_keypress ();
        if ((MENU_EXIT_CRTL == key_pressed) || (CONNECTION_ABORT_CRTL == key_pressed))
        {
            break;
        }
    }

    return 0;
}

/*********************************************************************************************************************
 * @brief   Start AWS cloud and sensor application
 *
 * This function starts AWS cloud and sensor application
 * @param       None
 * @retval      test_fn                 Start application menu information screen
 *********************************************************************************************************************/
test_fn start_app(void)
{
    fsp_err_t err = FSP_SUCCESS;
    int8_t key_pressed = -1;

    sprintf (s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    printf_colour ((void*) s_print_buffer);

    sprintf (s_print_buffer, MODULE_START_APP, g_selected_menu);

    err = flash_mem_init ();
    err = check_credentials_stored ();
    if (err != FSP_SUCCESS)
    {
        printf_colour (
                "\r\n[RED]Cloud credentials are not saved. \r\nPlease save cloud credentials and run application[WHITE]\r\n");
        printf_colour (MENU_RETURN_INFO);
    }
    else if (err == FSP_SUCCESS)
    {
        cred_save_success_flag = true;
        printf_colour ("\r\n[GREEN]Starting AWS cloud Application....[WHITE]\r\n");
        /* Send notify to start aws thread */
        xTaskNotifyFromISR(app_thread, 1, 1, NULL);
    }
    else
    {
        /* Do nothing */
    }

    while (MENU_EXIT_CRTL != key_pressed)
    {
        if (cred_save_success_flag == false)
        {
            /* Wait for space key to be pressed to return to main menu */
            key_pressed = wait_for_keypress ();
        }
        if ((MENU_EXIT_CRTL == key_pressed) || (CONNECTION_ABORT_CRTL == key_pressed))
        {
            break;
        }
        vTaskDelay (1000);
    }

    return 0;
}

/**********************************************************************************************************************
 End of function kis_display_menu
 *********************************************************************************************************************/

