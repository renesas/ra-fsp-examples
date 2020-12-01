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
/**********************************************************************************************************************
 * File Name    : r_console_if.c
 * Description  : RTT console support implementation file.
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "r_typedefs.h"
#include "r_console_if.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

#define SEGGER_ERR_PRINT(fn_, ...)  if(LVL_ERR)\
                                    SEGGER_RTT_printf (SEGGER_INDEX, "[ERR] In Function: %s(), %s",__FUNCTION__,(fn_),##__VA_ARGS__);

#define SEGGER_ERR_TRAP(err)        if (err) {\
                                    SEGGER_RTT_printf(SEGGER_INDEX, "\r\nReturned Error Code: 0x%x  \r\n", err);\
                                    __asm("BKPT #0\n");} /* trap upon the error  */

#define SEGGER_READ(read_data)      SEGGER_RTT_Read (SEGGER_INDEX, read_data, sizeof(read_data))

#define SEGGER_CHECK_DATA           SEGGER_RTT_HasKey()


/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_CONSOLE_IsInputReady
 * Description  : Check for input from user console, non-blocking
 * Return Value : Returns TRUE if input is ready
 *********************************************************************************************************************/
bool_t R_CONSOLE_IsInputReady(void)
{
    bool_t ret = true;

    if (0 == SEGGER_CHECK_DATA)
    {
        ret = false;
    }
    return (ret);
}
/**********************************************************************************************************************
 End of function R_CONSOLE_IsInputReady
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: R_CONSOLE_WaitForInput
 * Description  : Blocking in function till key is pressed on console
 * Argument     :
 * Return Value : .
 *********************************************************************************************************************/
void R_CONSOLE_WaitForInput()
{
    while (!R_CONSOLE_IsInputReady())
    {
        __asm volatile ("nop");
    }
}
/**********************************************************************************************************************
 End of function R_CONSOLE_WaitForInput
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_CONSOLE_WaitForAnyKey
 * Description  : Blocking in function till key is pressed on console, input is discarded
 * Argument     :
 * Return Value : .
 *********************************************************************************************************************/
void R_CONSOLE_WaitForAnyKey()
{
    R_CONSOLE_WaitForInput();

    R_CONSOLE_EmptyInputBuffer();
}
/**********************************************************************************************************************
 End of function R_CONSOLE_WaitForAnyKey
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_CONSOLE_EmptyInputBuffer
 * Description  : Flushes input buffer of surplus contents, resetting the buffer to empty
 * Argument     : None
 * Return Value : None
 *********************************************************************************************************************/
void R_CONSOLE_EmptyInputBuffer(void)
{
    while (R_CONSOLE_IsInputReady())
    {
        char_t temp_char;
        R_CONSOLE_ReadByte(&temp_char);
    }
}
/**********************************************************************************************************************
 End of function R_CONSOLE_EmptyInputBuffer
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: R_CONSOLE_ReadByte
 * Description  : Reads single byte of data from console
 * Param[out]   : read_byte_buffer -  single byte buffer containing data read from console
 * Return Value : error code (only 0 implemented)
 *********************************************************************************************************************/
int_t R_CONSOLE_ReadByte(char_t *read_byte_buffer)
{
    int_t err = 0;

    R_CONSOLE_WaitForInput();

    SEGGER_READ(read_byte_buffer);

    return (err);
}
/**********************************************************************************************************************
 End of function R_CONSOLE_ReadByte
 *********************************************************************************************************************/

