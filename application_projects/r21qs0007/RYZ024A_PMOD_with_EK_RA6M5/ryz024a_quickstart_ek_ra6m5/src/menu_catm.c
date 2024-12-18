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
/***********************************************************************************************************************
 * File Name    : menu_catm.c
 * Description  : CAT-M Information Screen.
 **********************************************************************************************************************/

#define MENU_CATM_C

#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "FreeRTOSconfig.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"

#include "common_init.h"
#include "common_data.h"
#include "common_utils.h"
#include "uart_CATM.h"
#include "menu_catm.h"
#include "one_common.h"


#define CONNECTION_ABORT_CRTL    (0x00)
#define MENU_EXIT_CRTL           (0x1B) // was space bar (0x20), now ESC key

#define MODULE_NAME     "\r\n%d. CAT-M INFORMATION\r\n"

#define SUB_OPTIONS     "\r\n\x1b[2m\x1b[37m a) IMEI:   %s "     \
                        "\r\n\x1b[2m\x1b[37m b) ICCID:  %s "     \

#define PING_SERVER     "\r\n%d. Validate SIM activation\r\n"


static char_t s_print_buffer[BUFFER_LINE_LENGTH] = { };
static const TickType_t s_ticks_to_wait = 100 / portTICK_PERIOD_MS;

static bool m_isCATMpresent = false;
static bool m_isSIMpresent = false;


/**********************************************************************************************************************
 * Function Name: catm_display_menu
 * Description  : .
 * Return Value : The module information demo screen
 *********************************************************************************************************************/
test_fn catm_display_menu(void)
{
    int8_t c = -1;

    fsp_err_t           fsp_err;

    // Clear screen
    sprintf (s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);
    print_to_console((void*)s_print_buffer);

    m_isCATMpresent = false;
    m_isSIMpresent = false;

    // connect to CAT-M
    while (1)
    {
        if ((strlen(g_strIMEI) > 0) && (strlen(g_strICCID) > 0))  // read the first time only
        {
            m_isCATMpresent = true;
            break;
        }
        else
        {
            ONE_PRINT("\r\nReading info from RYZ024A module\r\n");
            ONE_PRINT_AT("\r\nInit Comms UART");

            fsp_err = comms_initialize();
            if (FSP_SUCCESS != fsp_err)
            {
                print_to_console((uint8_t *)"\r\n ** UART INIT FAILED ** \r\n");
            }

            fsp_err = CATM_initialize();
            if (FSP_SUCCESS != fsp_err)
            {
                sprintf (s_print_buffer, "\r\n\n ** RYZ024A init FAILED ** \r\n");
                print_to_console((void*)s_print_buffer);
                ONE_DelayMsec(5000);
            }
            else
            {
                break;
            }
        }
    }

    if (strlen(g_strIMEI) > 0)
    {
        m_isCATMpresent = true;
    }
    if (strlen(g_strICCID) > 0)
    {
        m_isSIMpresent = true;
    }

    sprintf (s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);
    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    print_to_console((void*)s_print_buffer);

    sprintf (s_print_buffer, MODULE_NAME, g_selected_menu);
    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    print_to_console((void*)s_print_buffer);

    if (m_isCATMpresent == true || m_isSIMpresent == true)
    {
        sprintf (s_print_buffer, SUB_OPTIONS, (uint8_t *)g_strIMEI, (uint8_t *)g_strICCID);
        /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
        print_to_console((void*)s_print_buffer);
        ONE_PRINT("\r\n\n--> Next go to \"Activating the SIM Card\" in Quick Start Guide (Renesas Part Number R21QS0007)\r\n");
    }

    if (m_isSIMpresent == false)
    {
        ONE_PRINT("\r\n\n ** ERROR - No SIM card present ** \r\n");
    }

    sprintf (s_print_buffer, MENU_RETURN_INFO);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    print_to_console((void*)s_print_buffer);

    /* provide small delay so board_status should be up to date */
    vTaskDelay (s_ticks_to_wait);

    while (CONNECTION_ABORT_CRTL != c)
    {
        c = input_from_console ();
        if ((MENU_EXIT_CRTL == c) || (CONNECTION_ABORT_CRTL == c))
        {
            break;
        }
    }
    return (0);
}
/**********************************************************************************************************************
 End of function catm_display_menu
 *********************************************************************************************************************/


/*********************************************************************************************************************
* @brief        Run the Menu Validate SIM Card (Ping Server) Operation
* @param[in]    None
* @retval       0
*********************************************************************************************************************/
test_fn ping_display_menu(void)
{
    int8_t c = -1;

    fsp_err_t           fsp_err;

    // Clear screen
    sprintf (s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);
    print_to_console((void*)s_print_buffer);

    sprintf (s_print_buffer, PING_SERVER, g_selected_menu);
    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    print_to_console((void*)s_print_buffer);

    if (m_isSIMpresent == true)
    {
        for (int ii=0; ii<1; ii++)      // allow multiple tests here if desired
        {
            // send ping to CAT-M
            fsp_err = CATM_Ping();
            if (FSP_SUCCESS != fsp_err)
            {
                ONE_PRINT("\r\n\n ** ERROR - SIM card operation failed! ** \r\n");
                sprintf (s_print_buffer, MENU_ACTIVATION_ERROR_INFO);
                print_to_console((void*)s_print_buffer);
            }
            else
            {
                ONE_PRINT("\r\n\n ** SIM card active! **\r\n");
            }
            ONE_DelayMsec(1000);
        }
    }
    else
    {
        ONE_PRINT("\r\n\n ** ERROR - No SIM card present ** \r\n");
        sprintf (s_print_buffer, MENU_ACTIVATION_ERROR_INFO);
        print_to_console((void*)s_print_buffer);
    }

    sprintf (s_print_buffer, MENU_RETURN_INFO);
    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    print_to_console((void*)s_print_buffer);

    /* provide small delay so board_status should be up to date */
    vTaskDelay (s_ticks_to_wait);

    while (CONNECTION_ABORT_CRTL != c)
    {
        c = input_from_console ();
        if ((MENU_EXIT_CRTL == c) || (CONNECTION_ABORT_CRTL == c))
        {
            break;
        }
    }

    return (0);
}

