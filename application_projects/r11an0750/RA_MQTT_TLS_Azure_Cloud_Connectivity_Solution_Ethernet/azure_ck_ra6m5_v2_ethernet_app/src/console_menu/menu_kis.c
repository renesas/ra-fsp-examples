/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
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
#include "usr_app.h"

extern TX_THREAD application_thread;


static char_t s_print_buffer[BUFFER_LENGTH_SHORT] = { };
static bool cred_save_success_flag = false;

UINT status2 = EXIT_SUCCESS;

/*********************************************************************************************************************
 * @brief   Get fsp version
 *
 * This function prints fsp version
 * @param       None
 * @retval      test_fn                 Get version information screen
 *********************************************************************************************************************/
test_fn get_version(void)
{
    int8_t c = -1;
    fsp_pack_version_t version = { RESET_VALUE };

    sprintf (s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);

    /* Ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    printf_colour ((void*) s_print_buffer);

    sprintf (s_print_buffer, MODULE_NAME_VERSION, g_selected_menu);

    /* Ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    printf_colour ((void*) s_print_buffer);

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet (&version);

    sprintf (s_print_buffer, "   %d.%d.%d", version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch); //, version.minor, version.patch);

    /* Ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    printf_colour ((void*) s_print_buffer);

    sprintf (s_print_buffer, MENU_RETURN_INFO);

    /* Ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    printf_colour ((void*) s_print_buffer);
    while (CONNECTION_ABORT_CRTL != c)
    {
        /* Wait for space key to be pressed to return to main menu */
        c = wait_for_keypress ();
        if ((MENU_EXIT_CRTL == c) || (CONNECTION_ABORT_CRTL == c))
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
    int8_t c = -1;
    bsp_unique_id_t const *p_uid = R_BSP_UniqueIdGet ();

    sprintf (s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);

    /* Ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    printf_colour ((void*) s_print_buffer);

    sprintf (s_print_buffer, MODULE_NAME_UUID, g_selected_menu);

    /* Ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    printf_colour ((void*) s_print_buffer);

    sprintf (s_print_buffer, UUID_TAG);

    /* Ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    printf_colour ((void*) s_print_buffer);

    memset (s_print_buffer, 0, BUFFER_LENGTH_SHORT);

    sprintf (s_print_buffer, " %x%x%x%x-%x%x%x%x-%x%x%x%x-%x%x%x%x\r\n",
            (int_t) p_uid->unique_id_bytes[0], (int_t) p_uid->unique_id_bytes[1], (int_t) p_uid->unique_id_bytes[2], (int_t) p_uid->unique_id_bytes[3],
            (int_t) p_uid->unique_id_bytes[4], (int_t) p_uid->unique_id_bytes[5],(int_t) p_uid->unique_id_bytes[6], (int_t)p_uid->unique_id_bytes[7],
            (int_t)p_uid->unique_id_bytes[8], (int_t) p_uid->unique_id_bytes[9], (int_t) p_uid->unique_id_bytes[10], (int_t)p_uid->unique_id_bytes[11],
            (int_t)p_uid->unique_id_bytes[12], (int_t) p_uid->unique_id_bytes[13], (int_t) p_uid->unique_id_bytes[14], (int_t)p_uid->unique_id_bytes[15]);
    printf_colour (s_print_buffer);

    printf_colour (MENU_RETURN_INFO);

    while (CONNECTION_ABORT_CRTL != c)
    {
        /* Wait for space key to be pressed to return to main menu */
        c = wait_for_keypress ();
        if ((MENU_EXIT_CRTL == c) || (CONNECTION_ABORT_CRTL == c))
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
    int8_t c = -1;

    sprintf (s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);

    /* Ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    printf_colour ((void*) s_print_buffer);

    sprintf (s_print_buffer, MODULE_NAME_HELP, g_selected_menu);

    /* Ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    printf_colour ((void*) s_print_buffer);
    sprintf (s_print_buffer, " %s\r\n %s\r\n %s\r\n %s", HELP_TAG1, HELP_TAG2, HELP_TAG3, HELP_TAG4);

    /* Ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    printf_colour ((void*) s_print_buffer);

    printf_colour (MENU_RETURN_INFO);

    while (CONNECTION_ABORT_CRTL != c)
    {
        /* Wait for space key to be pressed to return to main menu */
        c = wait_for_keypress ();
        if ((MENU_EXIT_CRTL == c) || (CONNECTION_ABORT_CRTL == c))
        {
            break;
        }
    }

    return 0;
}

/*********************************************************************************************************************
 * @brief   Start AZURE cloud and sensor application
 *
 * This function starts AZURE cloud and sensor application
 * @param       None
 * @retval      test_fn                 Start application menu information screen
 *********************************************************************************************************************/
test_fn start_app(void)
{
    fsp_err_t err = FSP_SUCCESS;
    int8_t c = -1;

    sprintf (s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);

    /* Ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
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
        printf_colour ("\r\n[GREEN]Starting AZURE Ethernet cloud Application....[WHITE]\r\n");
        /* Send notify to start AZURE thread */
        status2 = tx_thread_resume(&application_thread);
    }
    else
    {
        /* Do nothing */
    }

    while (MENU_EXIT_CRTL != c)
    {
        if (cred_save_success_flag == false)
        {
            /* Wait for space key to be pressed to return to main menu */
            c = wait_for_keypress ();
        }
        if ((MENU_EXIT_CRTL == c) || (CONNECTION_ABORT_CRTL == c))
        {
            break;
        }
        tx_thread_sleep(pdMS_TO_TICKS(1000));
    }

    return 0;
}

/**********************************************************************************************************************
 End of function kis_display_menu
 *********************************************************************************************************************/

