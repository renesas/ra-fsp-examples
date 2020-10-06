/**********************************************************************************************************************
 * File Name    : menu_thread_entry.c
 * Version      : .
 * Description  : The main menu demo thread.
 *********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright [2020] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
 *
 * This software and documentation are supplied by Renesas Electronics America Inc. and may only be used with products
 * of Renesas Electronics Corp. and its affiliates ("Renesas").  No other uses are authorized.  Renesas products are
 * sold pursuant to Renesas terms and conditions of sale.  Purchasers are solely responsible for the selection and use
 * of Renesas products and Renesas assumes no liability.  No license, express or implied, to any intellectual property
 * right is granted by Renesas. This software is protected under all applicable laws, including copyright laws. Renesas
 * reserves the right to change or discontinue this software and/or this documentation. THE SOFTWARE AND DOCUMENTATION
 * IS DELIVERED TO YOU "AS IS," AND RENESAS MAKES NO REPRESENTATIONS OR WARRANTIES, AND TO THE FULLEST EXTENT
 * PERMISSIBLE UNDER APPLICABLE LAW, DISCLAIMS ALL WARRANTIES, WHETHER EXPLICITLY OR IMPLICITLY, INCLUDING WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT, WITH RESPECT TO THE SOFTWARE OR
 * DOCUMENTATION.  RENESAS SHALL HAVE NO LIABILITY ARISING OUT OF ANY SECURITY VULNERABILITY OR BREACH.  TO THE MAXIMUM
 * EXTENT PERMITTED BY LAW, IN NO EVENT WILL RENESAS BE LIABLE TO YOU IN CONNECTION WITH THE SOFTWARE OR DOCUMENTATION
 * (OR ANY PERSON OR ENTITY CLAIMING RIGHTS DERIVED FROM YOU) FOR ANY LOSS, DAMAGES, OR CLAIMS WHATSOEVER, INCLUDING,
 * WITHOUT LIMITATION, ANY DIRECT, CONSEQUENTIAL, SPECIAL, INDIRECT, PUNITIVE, OR INCIDENTAL DAMAGES; ANY LOST PROFITS,
 * OTHER ECONOMIC DAMAGE, PROPERTY DAMAGE, OR PERSONAL INJURY; AND EVEN IF RENESAS HAS BEEN ADVISED OF THE POSSIBILITY
 * OF SUCH LOSS, DAMAGES, CLAIMS OR COSTS.
 **********************************************************************************************************************/
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "menu_thread.h"
#include "common_init.h"
#include "common_data.h"
#include "board_cfg.h"
#include "usb_console.h"

#define WAIT_TIME (500u)  /* Wait time for task on Semaphore and Message Queue*/

fsp_err_t print_to_console(char *p_data);
extern bool g_show_intro;
char s_print_buff[256] = {};
extern int main_display_menu(void);

/**********************************************************************************************************************
 * Function Name: menu_thread_entry
 * Description  : Main menu thread.
 * Argument     : pvParameters contains TaskHandle_t
 * Return Value : .
 *********************************************************************************************************************/
void menu_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);

    SYSTEM_OK

    TickType_t Semphr_wait_ticks = pdMS_TO_TICKS (WAIT_TIME);

    while (1)
    {
        if (g_show_intro == true)
        {
            while (pdPASS != xSemaphoreTake(g_start_menu_binary_semaphore, Semphr_wait_ticks))
            {
                vTaskDelay(10);
            }
        }
        vTaskDelay(10);

        main_display_menu();

    }
}
/**********************************************************************************************************************
 End of function menu_thread_entry
 *********************************************************************************************************************/

