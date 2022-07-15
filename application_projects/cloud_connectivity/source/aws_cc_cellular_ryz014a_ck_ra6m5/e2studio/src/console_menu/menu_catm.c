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
 * Copyright (C) 2022 Renesas Electronics Corporation. All rights reserved.
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

static char_t s_print_buffer[BUFFER_LENGTH_SHORT] = {RESET_VALUE};
static const TickType_t s_ticks_to_wait = 100 / portTICK_PERIOD_MS;
static bool m_isCATMpresent = false;
static bool m_isSIMpresent = false;


/**********************************************************************************************************************
 * Function Name: catm_display_menu
 * Description  : .
 * Return Value : The CATM1 information  screen
 *********************************************************************************************************************/
test_fn catm_display_menu(void)
{
    char c = -1;
    fsp_err_t  fsp_err = FSP_SUCCESS;

    m_isCATMpresent = false;
    m_isSIMpresent = false;

    // Clear screen
    sprintf (s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);
    printf_colour((void*)s_print_buffer);

    APP_PRINT("\r\nReading info from CAT-M module\r\n");
    // connect to CAT-M
    while (1)
    {
        APP_PRINT((void*)".");
        if ((strlen(g_strIMEI) > 0) && (strlen(g_strICCID) > 0))  // read the first time only
        {
            m_isCATMpresent = true;
            break;
        }
        else
        {
            APP_DBG_PRINT("\r\nReading info from CAT-M module\r\n");
            APP_DBG_PRINT("\r\nInit UART");

            fsp_err = CATM_uart_initialize();
            if (FSP_SUCCESS != fsp_err)
            {
                APP_ERR_PRINT("\r\n ** UART INIT FAILED ** \r\n");
            }

            vTaskDelay(100);     // wait a brief moment for RTS to wake up on the CAT-M

            fsp_err = CATM_initialize();
            if (FSP_SUCCESS != fsp_err)
            {
                sprintf (s_print_buffer, "\r\n\n ** CAT-M init FAILED ** \r\n");
                APP_ERR_PRINT((void*)s_print_buffer);
                vTaskDelay(5000);
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
    APP_PRINT((void*)s_print_buffer);

    sprintf (s_print_buffer, CAT_MODULE_NAME, g_selected_menu);
    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    APP_PRINT((void*)s_print_buffer);

    if (m_isCATMpresent == true || m_isSIMpresent == true)
    {
        sprintf (s_print_buffer, SUB_OPTIONS_CATM1, (uint8_t *)g_strIMEI, (uint8_t *)g_strICCID);
        /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
        APP_PRINT((void*)s_print_buffer);
        APP_PRINT("\r\n\n --> Next, go to \"renesas.micro.ai\" to activate the SIM card with this info\r\n");
    }

    if (m_isSIMpresent == false)
    {
        APP_PRINT("\r\n\n ** ERROR - No SIM card present ** \r\n");
    }

    sprintf (s_print_buffer, MENU_RETURN_INFO);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    APP_PRINT((void*)s_print_buffer);

    /* provide small delay so board_status should be up to date */
    vTaskDelay (s_ticks_to_wait);


    while (CONNECTION_ABORT_CRTL != c)
    {
        c = (char)wait_for_keypress ();
        if ((MENU_EXIT_CRTL_CATM1 == c) || (CONNECTION_ABORT_CRTL == c))
        {
            break;
        }
    }
    return (0);
}
/**********************************************************************************************************************
 End of function kis_display_menu
 *********************************************************************************************************************/


