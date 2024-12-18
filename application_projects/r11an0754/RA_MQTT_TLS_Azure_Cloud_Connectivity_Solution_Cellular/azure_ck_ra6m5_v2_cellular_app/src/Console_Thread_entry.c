/***********************************************************************************************************************
 * File Name    : Console_Thread_entry.c
 * Description  : Contains data structures and functions used in Console related application
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright [2015-2023] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
 *
 * The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
 * and/or its licensors ("Renesas") and subject to statutory and contractual protections.
 *
 * This file is subject to a Renesas FSP license agreement. Unless otherwise agreed in an FSP license agreement with
 * Renesas: 1) you may not use, copy, modify, distribute, display, or perform the contents; 2) you may not use any name
 * or mark of Renesas for advertising or publicity purposes or in connection with your use of the contents; 3) RENESAS
 * MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED
 * "AS IS" WITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, AND NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR
 * CONSEQUENTIAL DAMAGES, INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF
 * CONTRACT OR TORT, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents
 * included in this file may be subject to different terms.
 **********************************************************************************************************************/
#include "Console_Thread.h"
#include "console_menu/menu_main.h"
#include "common_utils.h"
#include "base64.h"
#include "usr_app.h"
#define CERT_1ST_LINE_LENGTH            (50)
#define CERT_BASE64_DATA_MAX_LENGTH     (5000)

uint8_t g_certpem[4096];
void console_init(void);
void flash_init(void);
unsigned int DecodeBASE64(char *incertbuff, unsigned char*outcerthexbuff, const int length_out);
/* Console_Thread entry function */
void Console_Thread_entry(void)
{
    /* TODO: add your own code here */
    console_init();

    while (1)
    {
        main_display_menu();
        tx_thread_sleep (pdMS_TO_TICKS(10));
    }
}

/**************************************************************************************
 * @brief  Console Initialization
 * @param[in]
 * @retval
 **************************************************************************************/
void console_init(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Version get API for FLEX pack information */
    /* Initializing UART */
    err = uart_initialize();
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\nUART INIT FAILED\r\n");
        APP_ERR_TRAP(err);
    }
    else
    {
       APP_DBG_PRINT ("\r\nUART INIT SUCCESS\r\n");
    }

    flash_init();
}

/**************************************************************************************
 * @brief     Initialize flash
 * @param[in]
 * @retval
 **************************************************************************************/
void flash_init(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Open Flash_HP */
    err = R_FLASH_HP_Open(&user_flash_ctrl, &user_flash_cfg);
    /* Handle Error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n Flah_HP_Open API failed");
        APP_ERR_TRAP(err);
    }
}

/*********************************************************************************************************************
* @brief  DecodeBASE64 function
*
* This function gets the rootCA certificate from the user and store in the internal flash.
*********************************************************************************************************************/
unsigned int DecodeBASE64(char *incertbuff, unsigned char*outcerthexbuff, const int length_out)
{
    memset(outcerthexbuff,0,length_out);
    memset(g_certpem,0,sizeof(g_certpem));

    unsigned long int t = 0;
    unsigned long int t2 = 0;
    unsigned long int t3 = 0;

    /* Remove 1st line of certificate ,skip till \r */
    for (t=0;t<CERT_1ST_LINE_LENGTH;t++)
    {
        if(incertbuff[t]=='\r')
        {
            break;
        }
    }
    t++;

    /* Copy BASE64 data till certificate's last line's 1st character '-' */
    for(;t<CERT_BASE64_DATA_MAX_LENGTH;t++)
    {
        if(incertbuff[t]=='-')
        {
            break;
        }
        if((incertbuff[t]!='\r')&&(incertbuff[t]!='\n'))
        {
            g_certpem[t2]=incertbuff[t];

            t2++;
        }

    }

    /* Generate hex data from BASE64 data */
    t3= base64_decode(&g_certpem[0], t2 ,&outcerthexbuff[0]);

    /* Return hex data len */
    return(t3);
}
