/***********************************************************************************************************************
 * File Name    : led.c
 * Description  : Contains data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
#include <led.h>

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
/***********************************************************************************************************************
Export global variables
***********************************************************************************************************************/



/***********************************************************************************************************************
* Function Name: R_LED_LED415OFF
* Description  : This function performs the processing in the code
* Arguments    : void
* Return Value : none
***********************************************************************************************************************/
void R_LED_LED415OFF(void ) // @suppress("API function naming")
{
    g_ioport.p_api->pinWrite(g_ioport.p_ctrl, BSP_IO_PORT_04_PIN_15,BSP_IO_LEVEL_LOW);
}

/***********************************************************************************************************************
* Function Name: R_LED_LED415ON
* Description  : This function performs the processing in the code
* Arguments    : void
* Return Value : none
***********************************************************************************************************************/
void R_LED_LED415ON(void ) // @suppress("API function naming")
{
    g_ioport.p_api->pinWrite(g_ioport.p_ctrl, BSP_IO_PORT_04_PIN_15 ,BSP_IO_LEVEL_HIGH);
}

/********************************************************************************************************************//*
 End of function Reset Led
**********************************************************************************************************************/
/***********************************************************************************************************************
 * End of function LED_LED415nOff
 **********************************************************************************************************************/


